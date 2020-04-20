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

#include <U2Core/BaseDocumentFormats.h>
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

#include "RmdupBamWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString RmdupBamWorkerFactory::ACTOR_ID("rmdup-bam");
static const QString SHORT_NAME( "mb" );
static const QString INPUT_PORT( "in-file" );
static const QString OUTPUT_PORT( "out-file" );
static const QString OUT_MODE_ID( "out-mode" );
static const QString CUSTOM_DIR_ID( "custom-dir" );
static const QString OUT_NAME_ID( "out-name" );
static const QString REMOVE_SINGLE_END_ID( "remove-single-end" );
static const QString TREAT_READS_ID( "treat_reads" );

/************************************************************************/
/* RmdupBamPrompter */
/************************************************************************/
QString RmdupBamPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr("<u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Remove PCR duplicates of BAM files from %1 with SAMTools rmdup.").arg(producerName);
    return doc;
}

/************************************************************************/
/* RmdupBamWorkerFactory */
/************************************************************************/
namespace {

    static const QString DEFAULT_NAME( "Default" );
}

void RmdupBamWorkerFactory::init() {
    Descriptor desc( ACTOR_ID, RmdupBamWorker::tr("Remove Duplicates in BAM Files"),
        RmdupBamWorker::tr("Remove PCR duplicates of BAM files using SAMTools rmdup.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, RmdupBamWorker::tr("BAM File"),
            RmdupBamWorker::tr("Set of BAM files to rmdup"));
        Descriptor outD(OUTPUT_PORT, RmdupBamWorker::tr("Cleaned BAM File"),
            RmdupBamWorker::tr("Cleaned BAM file"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType(SHORT_NAME + ".input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType(SHORT_NAME + ".output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor outDir(OUT_MODE_ID, RmdupBamWorker::tr("Output folder"),
            RmdupBamWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(CUSTOM_DIR_ID, RmdupBamWorker::tr("Custom folder"),
            RmdupBamWorker::tr("Select the custom output folder."));

        Descriptor outName(OUT_NAME_ID, RmdupBamWorker::tr("Output BAM name"),
            RmdupBamWorker::tr("A name of an output BAM file. If default of empty value is provided the output name is the name of the first BAM file with .nodup.bam extension."));

        Descriptor removeSE(REMOVE_SINGLE_END_ID, RmdupBamWorker::tr("Remove for single-end reads"),
            RmdupBamWorker::tr("Remove duplicate for single-end reads. By default, the command works for paired-end reads only (-s)."));

        Descriptor treatReads(TREAT_READS_ID, RmdupBamWorker::tr("Treat as single-end"),
            RmdupBamWorker::tr("Treat paired-end reads and single-end reads (-S)."));

        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(DEFAULT_NAME));
        a << new Attribute( removeSE, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute( treatReads, BaseTypes::BOOL_TYPE(), false, QVariant(false));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap directoryMap;
        QString fileDir = RmdupBamWorker::tr("Input file");
        QString workflowDir = RmdupBamWorker::tr("Workflow");
        QString customD = RmdupBamWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new RmdupBamPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new RmdupBamWorkerFactory());
}

/************************************************************************/
/* RmdupBamWorker */
/************************************************************************/
RmdupBamWorker::RmdupBamWorker(Actor *a)
:BaseWorker(a)
,inputUrlPort(NULL)
,outputUrlPort(NULL)
,outUrls("")
{

}

void RmdupBamWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
    outputUrlPort = ports.value(OUTPUT_PORT);
}

Task * RmdupBamWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);

        const QString detectedFormat = FileAndDirectoryUtils::detectFormat(url);
        if(detectedFormat.isEmpty()){
            coreLog.info(tr("Unknown file format: ") + url);
            return NULL;
        }

        if(detectedFormat == BaseDocumentFormats::BAM){
            const QString outputDir = FileAndDirectoryUtils::createWorkingDir(url, getValue<int>(OUT_MODE_ID), getValue<QString>(CUSTOM_DIR_ID), context->workingDir());

            BamRmdupSetting setting;
            setting.outDir = outputDir;
            setting.outName = getTargetName(url, outputDir);
            setting.inputUrl = url;
            setting.removeSingleEnd = getValue<bool>(REMOVE_SINGLE_END_ID);
            setting.treatReads = getValue<bool>(TREAT_READS_ID);

            SamtoolsRmdupTask *t = new SamtoolsRmdupTask(setting);
            t->addListeners(createLogListeners());
            connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
            return t;
        }
    }

    if (inputUrlPort->isEnded()) {
        setDone();
        outputUrlPort->setEnded();
    }
    return NULL;
}

void RmdupBamWorker::cleanup() {
    outUrls.clear();
}

namespace {
    QString getTargetUrl(Task *task) {

        SamtoolsRmdupTask *rmdupTask = dynamic_cast<SamtoolsRmdupTask*>(task);

        if (NULL != rmdupTask) {
            return rmdupTask->getResult();
        }
        return "";
    }
}

void RmdupBamWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetUrl(task);
    CHECK(!url.isEmpty(), );

    sendResult(url);
    monitor()->addOutputFile(url, getActorId());
}

QString RmdupBamWorker::getTargetName (const QString &fileUrl, const QString &outDir){
    QString name = getValue<QString>(OUT_NAME_ID);

    if(name == DEFAULT_NAME || name.isEmpty()){
        name = QFileInfo(fileUrl).fileName();
        name = name + ".nodup.bam";
    }
    if(outUrls.contains(outDir + name)){
        name.append(QString("_%1").arg(outUrls.size()));
    }
    outUrls.append(outDir+name);
    return name;
}


QString RmdupBamWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        outputUrlPort->transit();
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

void RmdupBamWorker::sendResult(const QString &url) {
    const Message message(BaseTypes::STRING_TYPE(), url);
    outputUrlPort->put(message);
}

////////////////////////////////////////////////////////
//BamRmdupSetting
QStringList BamRmdupSetting::getSamtoolsArguments() const{
    QStringList result;

    result << "rmdup";

    if(removeSingleEnd){
        result << "-s";
    }

    if(treatReads){
        result << "-S";
    }


    result << inputUrl;

    result << outDir + outName;

    return result;
}


////////////////////////////////////////////////////////
//SamtoolsRmdupTask

const QString SamtoolsRmdupTask::SAMTOOLS_ID = "USUPP_SAMTOOLS";

SamtoolsRmdupTask::SamtoolsRmdupTask(const BamRmdupSetting &settings)
:ExternalToolSupportTask(tr("Samtool rmdup for %1 ").arg(settings.inputUrl), TaskFlags(TaskFlag_None)),settings(settings),resultUrl(""){

}

void SamtoolsRmdupTask::prepare(){
    if (settings.inputUrl.isEmpty()){
        setError(tr("No assembly URL to filter"));
        return ;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError(tr("Folder does not exist: ") + outDir.absolutePath());
        return ;
    }

}

void SamtoolsRmdupTask::run(){
    CHECK_OP(stateInfo, );

    ProcessRun samtools = ExternalToolSupportUtils::prepareProcess(SAMTOOLS_ID, settings.getSamtoolsArguments(), "", QStringList(), stateInfo, getListener(0));
    CHECK_OP(stateInfo, );
    QScopedPointer<QProcess> sp(samtools.process);
    QScopedPointer<ExternalToolRunTaskHelper> sh(new ExternalToolRunTaskHelper(samtools.process, new ExternalToolLogParser(), stateInfo));
    setListenerForHelper(sh.data(), 0);

    start(samtools, "SAMtools");
    CHECK_OP(stateInfo, );

    while(!samtools.process->waitForFinished(1000)){
        if (isCanceled()) {
            CmdlineTaskRunner::killProcessTree(samtools.process);
            return;
        }
    }
    checkExitCode(samtools.process, "SAMtools");

    if(!hasError()){
        resultUrl = settings.outDir + settings.outName;
    }
}

void SamtoolsRmdupTask::start(const ProcessRun &pRun, const QString &toolName){
    pRun.process->start(pRun.program, pRun.arguments);
    bool started = pRun.process->waitForStarted();
    CHECK_EXT(started, setError(tr("Can not run %1 tool").arg(toolName)), );
}

void SamtoolsRmdupTask::checkExitCode(QProcess *process, const QString &toolName){
    int exitCode = process->exitCode();
    if (exitCode != EXIT_SUCCESS && !hasError()) {
        setError(tr("%1 tool exited with code %2").arg(toolName).arg(exitCode));
    } else {
        algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
    }
}


} //LocalWorkflow
} //U2
