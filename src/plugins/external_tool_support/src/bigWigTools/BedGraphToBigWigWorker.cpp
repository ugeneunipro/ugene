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

#include <U2Formats/BAMUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "BedGraphToBigWigTask.h"
#include "BedGraphToBigWigWorker.h"
#include "BigWigSupport.h"

namespace U2 {
namespace LocalWorkflow {

const QString BedGraphToBigWigWorker::INPUT_PORT = "in-file";
const QString BedGraphToBigWigWorker::OUTPUT_PORT = "out-file";
const QString BedGraphToBigWigWorker::OUT_MODE_ID = "out-mode";
const QString BedGraphToBigWigWorker::CUSTOM_DIR_ID = "custom-dir";
const QString BedGraphToBigWigWorker::OUT_NAME_ID = "out-name";
const QString BedGraphToBigWigWorker::DEFAULT_NAME = "Default";
const QString BedGraphToBigWigWorker::BLOCK_SIZE = "bs";
const QString BedGraphToBigWigWorker::ITEMS_PER_SLOT = "its";
const QString BedGraphToBigWigWorker::UNCOMPRESSED = "unc";
const QString BedGraphToBigWigWorker::GENOME = "genome";
const QString BedGraphToBigWigFactory::ACTOR_ID("bgtbw-bam");


/************************************************************************/
/* BedGraphToBigWigPrompter */
/************************************************************************/
QString BedGraphToBigWigPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BedGraphToBigWigWorker::INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Converts bedGraph files to bigWig %1 with bedGraphToBigWig.").arg(producerName);
    return doc;
}

/************************************************************************/
/* BedGraphToBigWigFactory */
/************************************************************************/
void BedGraphToBigWigFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(BigWigSupport::GENOMES_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }
    Descriptor desc( ACTOR_ID, BedGraphToBigWigWorker::tr("Convert bedGraph Files to bigWig"),
        BedGraphToBigWigWorker::tr("Convert bedGraph Files to bigWig.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(BedGraphToBigWigWorker::INPUT_PORT, BedGraphToBigWigWorker::tr("BedGrapgh files"),
            BedGraphToBigWigWorker::tr("Set of bedGraph files"));
        Descriptor outD(BedGraphToBigWigWorker::OUTPUT_PORT, BedGraphToBigWigWorker::tr("BigWig files"),
            BedGraphToBigWigWorker::tr("BigWig files"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("bgbw.input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("bgbw.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {

        Descriptor outDir(BedGraphToBigWigWorker::OUT_MODE_ID, BedGraphToBigWigWorker::tr("Output folder"),
            BedGraphToBigWigWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(BedGraphToBigWigWorker::CUSTOM_DIR_ID, BedGraphToBigWigWorker::tr("Custom folder"),
            BedGraphToBigWigWorker::tr("Select the custom output folder."));

        Descriptor outName(BedGraphToBigWigWorker::OUT_NAME_ID, BedGraphToBigWigWorker::tr("Output name"),
            BedGraphToBigWigWorker::tr("A name of an output file. If default of empty value is provided the output name is the name of the first BAM file with an extension."));

        Descriptor blockSize(BedGraphToBigWigWorker::BLOCK_SIZE, BedGraphToBigWigWorker::tr("Block size"),
            BedGraphToBigWigWorker::tr("Number of items to bundle in r-tree (-blockSize)."));

        Descriptor itemsPerSlot(BedGraphToBigWigWorker::ITEMS_PER_SLOT, BedGraphToBigWigWorker::tr("Items per slot"),
            BedGraphToBigWigWorker::tr("Number of data points bundled at lowest level (-itemsPerSlot)."));

        Descriptor unc(BedGraphToBigWigWorker::UNCOMPRESSED, BedGraphToBigWigWorker::tr("Uncompressed"),
            BedGraphToBigWigWorker::tr("If set, do not use compression (-unc)."));

        Descriptor genomeAttrDesc(BedGraphToBigWigWorker::GENOME, BedGraphToBigWigWorker::tr("Genome"),
            BedGraphToBigWigWorker::tr("File with genome length."));


        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(BedGraphToBigWigWorker::OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        Attribute* genomeAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true, dataPath->getPathByName(dataNames.first()));
            }else{
                genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
            }
        }else{
            genomeAttr = new Attribute(genomeAttrDesc, BaseTypes::STRING_TYPE(), true);
        }
        a << genomeAttr;

        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(BedGraphToBigWigWorker::DEFAULT_NAME));
        a << new Attribute( blockSize, BaseTypes::NUM_TYPE(), false, QVariant(256));
        a << new Attribute( itemsPerSlot, BaseTypes::NUM_TYPE(), false, QVariant(1024));
        a << new Attribute( unc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {

        QVariantMap directoryMap;
        QString fileDir = BedGraphToBigWigWorker::tr("Input file");
        QString workflowDir = BedGraphToBigWigWorker::tr("Workflow");
        QString customD = BedGraphToBigWigWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[BedGraphToBigWigWorker::OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[BedGraphToBigWigWorker::CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        QVariantMap m; m["minimum"] = 0.; m["maximum"] = INT_MAX;
        delegates[BedGraphToBigWigWorker::BLOCK_SIZE] = new SpinBoxDelegate(m);
        delegates[BedGraphToBigWigWorker::ITEMS_PER_SLOT] = new SpinBoxDelegate(m);

        QVariantMap vm;
        if (dataPath){
            vm = dataPath->getDataItemsVariantMap();
        }
        delegates[BedGraphToBigWigWorker::GENOME] = new ComboBoxWithUrlsDelegate(vm);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new BedGraphToBigWigPrompter());
    proto->addExternalTool(BigWigSupport::ET_BIGWIG_ID);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CONVERTERS(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new BedGraphToBigWigFactory());
}


//////////////////////////////////////////////////////////////////////////
//BedGraphToBigWigWorker
BedGraphToBigWigWorker::BedGraphToBigWigWorker(Actor *a)
:BaseWorker(a)
,inputUrlPort(NULL)
,outputUrlPort(NULL)
,outUrls("")
{

}

void BedGraphToBigWigWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
    outputUrlPort = ports.value(OUTPUT_PORT);
}

Task * BedGraphToBigWigWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);

        const QString outputDir = FileAndDirectoryUtils::createWorkingDir(url, getValue<int>(OUT_MODE_ID), getValue<QString>(CUSTOM_DIR_ID), context->workingDir());

        BedGraphToBigWigSetting setting;
        setting.outDir = outputDir;
        setting.outName = getTargetName(url, outputDir);
        setting.inputUrl = url;
        setting.blockSize = getValue<int>(BLOCK_SIZE);
        setting.itemsPerSlot = getValue<int>(ITEMS_PER_SLOT);
        setting.uncompressed = getValue<bool>(UNCOMPRESSED);
        setting.genomePath = getValue<QString>(GENOME);

        ExternalToolSupportTask *t = new BedGraphToBigWigTask (setting);
        t->addListeners(createLogListeners());
        connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return t;
    }

    if (inputUrlPort->isEnded()) {
        setDone();
        outputUrlPort->setEnded();
    }
    return NULL;
}

void BedGraphToBigWigWorker::cleanup() {
    outUrls.clear();
}

namespace {
    QString getTargetTaskUrl(Task *task) {

        BedGraphToBigWigTask *curtask = dynamic_cast<BedGraphToBigWigTask*>(task);

        if (NULL != curtask) {
            return curtask->getResult();
        }
        return "";
    }
}

void BedGraphToBigWigWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetTaskUrl(task);
    CHECK(!url.isEmpty(), );

    sendResult(url);
    monitor()->addOutputFile(url, getActorId());
}

QString BedGraphToBigWigWorker::getTargetName (const QString &fileUrl, const QString &outDir){
    QString name = getValue<QString>(OUT_NAME_ID);

    if(name == DEFAULT_NAME || name.isEmpty()){
        name = QFileInfo(fileUrl).fileName();
        name = name + getDefaultFileName();
    }
    if(outUrls.contains(outDir + name)){
        name.append(QString("_%1").arg(outUrls.size()));
    }
    outUrls.append(outDir+name);
    return name;
}


QString BedGraphToBigWigWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        outputUrlPort->transit();
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

void BedGraphToBigWigWorker::sendResult(const QString &url) {
    const Message message(BaseTypes::STRING_TYPE(), url);
    outputUrlPort->put(message);
}

} //LocalWorkflow
} //U2
