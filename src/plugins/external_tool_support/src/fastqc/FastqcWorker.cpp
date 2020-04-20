/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Formats/BAMUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "FastqcSupport.h"
#include "FastqcTask.h"
#include "FastqcWorker.h"
#include "java/JavaSupport.h"
#include "perl/PerlSupport.h"

namespace U2 {
namespace LocalWorkflow {


const QString FastQCWorker::BASE_FASTQC_SUBDIR = "FastQC";
const QString FastQCWorker::INPUT_PORT = "in-file";
const QString FastQCWorker::OUT_MODE_ID = "out-mode";
const QString FastQCWorker::CUSTOM_DIR_ID = "custom-dir";
const QString FastQCWorker::OUT_FILE = "out-file";

const QString FastQCWorker::ADAPTERS = "adapter";
const QString FastQCWorker::CONTAMINANTS = "contaminants";

const QString FastQCFactory::ACTOR_ID("fastqc");

////////////////////////////////////////////////
// FastQCPrompter

QString FastQCPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(FastQCWorker::INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Builds FastQC report for file(s) %1.").arg(producerName);
    return doc;
}

////////////////////////////////////////
//FastQCFactory
void FastQCFactory::init() {
    Descriptor desc( ACTOR_ID, FastQCWorker::tr("FastQC Quality Control"),
        FastQCWorker::tr("Builds quality control reports.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(FastQCWorker::INPUT_PORT, FastQCWorker::tr("Short reads"),
            FastQCWorker::tr("Short read data"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("fastqc.input-url", inM)), true);

    }

    QList<Attribute*> a;
    {

        Descriptor outDir(FastQCWorker::OUT_MODE_ID, FastQCWorker::tr("Output folder"),
            FastQCWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor outFile(FastQCWorker::OUT_FILE, FastQCWorker::tr("Output file"),
            FastQCWorker::tr("Specify the output file name."));

        Descriptor customDir(FastQCWorker::CUSTOM_DIR_ID, FastQCWorker::tr("Custom folder"),
            FastQCWorker::tr("Select the custom output folder."));

        Descriptor adapters(FastQCWorker::ADAPTERS, FastQCWorker::tr("List of adapters"),
            FastQCWorker::tr("Specifies a non-default file which contains the list of adapter sequences which will be explicitly searched against "
                                             "the library. The file must contain sets of named adapters "
                                             "in the form name[tab]sequence.  Lines prefixed with a hash "
                                             "will be ignored."));

        Descriptor conts(FastQCWorker::CONTAMINANTS, FastQCWorker::tr("List of contaminants"),
            FastQCWorker::tr("Specifies a non-default file which contains the list of "
                             "contaminants to screen overrepresented sequences against. "
                                             "The file must contain sets of named contaminants in the "
                                             "form name[tab]sequence.  Lines prefixed with a hash will "
                                             "be ignored."));

        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), (Attribute::Flags) Attribute::Hidden, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(FastQCWorker::OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;

        a << new Attribute( adapters, BaseTypes::STRING_TYPE(), false, "");
        a << new Attribute( conts, BaseTypes::STRING_TYPE(), false, "");
        a << new Attribute(outFile, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {

        QVariantMap directoryMap;
        QString fileDir = FastQCWorker::tr("Input file");
        QString workflowDir = FastQCWorker::tr("Workflow");
        QString customD = FastQCWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[FastQCWorker::OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[FastQCWorker::CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        delegates[FastQCWorker::ADAPTERS] = new URLDelegate("", "", false, false, false);
        delegates[FastQCWorker::CONTAMINANTS] = new URLDelegate("", "", false, false, false);

        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, FastQCWorker::tr("Auto"));
        outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareFileFilter("HTML", QStringList("html"), false, QStringList()));
        delegates[FastQCWorker::OUT_FILE] = new URLDelegate(outputUrlTags, "fastqc/output");
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new FastQCPrompter());
    proto->addExternalTool(JavaSupport::ET_JAVA_ID);
    proto->addExternalTool(FastQCSupport::ET_FASTQC_ID);
    proto->addExternalTool(PerlSupport::ET_PERL_ID);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new FastQCFactory());
}

//////////////////////////////////////////////////////////////////////////
//FastQCWorker
FastQCWorker::FastQCWorker(Actor *a)
:BaseWorker(a)
,inputUrlPort(NULL)
{

}

void FastQCWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
}

Task * FastQCWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);
        QString outFile = getValue<QString>(OUT_FILE);
        FastQCSetting settings;
        if (outFile.isEmpty()) {
            QString outputDir = FileAndDirectoryUtils::createWorkingDir(url, getValue<int>(OUT_MODE_ID), "", context->workingDir());
            settings.outDir = outputDir + FastQCWorker::BASE_FASTQC_SUBDIR;
            QDir outQDir(settings.outDir);
            if (!outQDir.exists()) {
                U2OpStatusImpl os;
                GUrlUtils::createDirectory(settings.outDir, "_", os);
            }
            settings.fileName = "";
        } else {
            QFileInfo outFileFi(outFile);
            settings.outDir = outFileFi.absoluteDir().absolutePath();
            settings.fileName = outFileFi.fileName();
        }
        settings.inputUrl = url;
        settings.adapters = getValue<QString>(ADAPTERS);
        settings.conts = getValue<QString>(CONTAMINANTS);

        FastQCTask *t = new FastQCTask (settings);
        t->addListeners(createLogListeners());
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return t;
    }

    if (inputUrlPort->isEnded()) {
        setDone();
    }
    return NULL;
}

void FastQCWorker::cleanup(){

}

namespace {
    QString getTargetTaskUrl(Task * task) {
        FastQCTask *curtask = dynamic_cast<FastQCTask*>(task);
        if (NULL != curtask) {
            return curtask->getResult();
        }
        return "";
    }

}

void FastQCWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetTaskUrl(task);
    CHECK(!url.isEmpty(), );

    monitor()->addOutputFile(url, getActorId(), true);
}

QString FastQCWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

} //LocalWorkflow
} //U2

