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

#include "FilterBamWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString FilterBamWorkerFactory::ACTOR_ID("filter-bam");
static const QString SHORT_NAME( "mb" );
static const QString INPUT_PORT( "in-file" );
static const QString OUTPUT_PORT( "out-file" );
static const QString OUTPUT_SUBDIR( "Filtered_BAM/" );
static const QString OUT_MODE_ID( "out-mode" );
static const QString CUSTOM_DIR_ID( "custom-dir" );
static const QString OUT_NAME_ID( "out-name" );
static const QString OUT_FORMAT_ID( "out-format" );
static const QString REGION_ID( "region" );
static const QString MAPQ_ID( "mapq" );
static const QString ACCEPT_FLAG_ID( "accept-flag" );
static const QString FLAG_ID( "flag" );


/************************************************************************/
/* FilterBamPrompter */
/************************************************************************/
QString FilterBamPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr("<u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Filter BAM/SAM files from %1 with SAMTools view.").arg(producerName);
    return doc;
}

/************************************************************************/
/* FilterBamWorkerFactory */
/************************************************************************/
namespace {
    static const QString DEFAULT_NAME( "Default" );

    QMap<QString, QString> getFilterCodes(){
        QMap <QString, QString> res;
        res.insert("Read is paired", "0x0001");
        res.insert("Read is mapped in a proper pair", "0x0002");
        res.insert("The read is unmapped", "0x0004");
        res.insert("The mate is unmapped", "0x0008");
        res.insert("Read strand", "0x0010");
        res.insert("Mate strand", "0x0020");
        res.insert("Read is the first in a pair", "0x0040");
        res.insert("Read is the second in a pair", "0x0080");
        res.insert("The alignment or this read is not primary", "0x0100");
        res.insert("The read fails platform/vendor quality checks", "0x0200");
        res.insert("The read is a PCR or optical duplicate", "0x0400");
        return res;
    }

    QString getHexValueByFilterString(const QString& filterString, const QMap<QString, QString>& codes){

        int val = 0;
        QStringList filterCodes = filterString.split(",");
        foreach(const QString fCode, filterCodes){
            if(codes.contains(fCode)){
                bool ok = true;
                val += codes.value(fCode).toInt(&ok, 16);
            }
        }
        if(val == 0){
            return "";
        }
        return QString::number(val, 16);
    }
}

void FilterBamWorkerFactory::init() {
    Descriptor desc( ACTOR_ID, FilterBamWorker::tr("Filter BAM/SAM files"),
        FilterBamWorker::tr("Filters BAM/SAM files using SAMTools view.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, FilterBamWorker::tr("BAM/SAM File"),
            FilterBamWorker::tr("Set of BAM/SAM files to filter"));
        Descriptor outD(OUTPUT_PORT, FilterBamWorker::tr("Filtered BAM/SAM files"),
            FilterBamWorker::tr("Filtered BAM/SAM files"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType(SHORT_NAME + ".input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType(SHORT_NAME + ".output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor outDir(OUT_MODE_ID, FilterBamWorker::tr("Output folder"),
            FilterBamWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(CUSTOM_DIR_ID, FilterBamWorker::tr("Custom folder"),
            FilterBamWorker::tr("Select the custom output folder."));

        Descriptor outName(OUT_NAME_ID, FilterBamWorker::tr("Output name"),
            FilterBamWorker::tr("A name of an output BAM/SAM file. If default of empty value is provided the output name is the name of the first BAM/SAM file with .filtered extension."));

        Descriptor outFormat(OUT_FORMAT_ID, FilterBamWorker::tr("Output format"),
            FilterBamWorker::tr("Format of an output assembly file."));

        Descriptor regionFilter(REGION_ID, FilterBamWorker::tr("Region"),
            FilterBamWorker::tr("Regions to filter. For BAM output only. chr2 to output the whole chr2. chr2:1000 to output regions of chr 2 starting from 1000. "
                                "chr2:1000-2000 to output regions of chr2 between 1000 and 2000 including the end point. To input multiple regions use the space separator (e.g. chr1 chr2 chr3:1000-2000)."));

        Descriptor mapqFilter(MAPQ_ID, FilterBamWorker::tr("MAPQ threshold"),
            FilterBamWorker::tr("Minimum MAPQ quality score."));

        Descriptor flagAccept(ACCEPT_FLAG_ID, FilterBamWorker::tr("Accept flag"),
            FilterBamWorker::tr("Only output alignments with the selected items. Select the items in the combobox to configure bit flag. Do not select the items to avoid filtration by this parameter."));

        Descriptor flagFilter(FLAG_ID, FilterBamWorker::tr("Skip flag"),
            FilterBamWorker::tr("Skip alignment with the selected items. Select the items in the combobox to configure bit flag. Do not select the items to avoid filtration by this parameter."));

        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(DEFAULT_NAME));
        a << new Attribute( outFormat, BaseTypes::STRING_TYPE(), false, QVariant(BaseDocumentFormats::BAM));
        a << new Attribute( regionFilter, BaseTypes::STRING_TYPE(), false, QVariant(""));
        a << new Attribute( mapqFilter, BaseTypes::NUM_TYPE(), false, QVariant(0));
        a << new Attribute( flagAccept, BaseTypes::STRING_TYPE(), false, QVariant(""));
        a << new Attribute( flagFilter, BaseTypes::STRING_TYPE(), false, QVariant(""));
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap directoryMap;
        QString fileDir = FilterBamWorker::tr("Input file");
        QString workflowDir = FilterBamWorker::tr("Workflow");
        QString customD = FilterBamWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        QVariantMap formatMap;
        formatMap[BaseDocumentFormats::BAM] = BaseDocumentFormats::BAM;
        formatMap[BaseDocumentFormats::SAM] = BaseDocumentFormats::SAM;
        delegates[OUT_FORMAT_ID] = new ComboBoxDelegate(formatMap);
        QVariantMap lenMap; lenMap["minimum"] = QVariant(0); lenMap["maximum"] = QVariant(254);
        delegates[MAPQ_ID] = new SpinBoxDelegate(lenMap);

        QVariantMap flags;
        QMap<QString, QString> filterCodes = getFilterCodes();
        foreach(const QString& key, filterCodes.keys()){
            flags[key] = false;
        }
        delegates[ACCEPT_FLAG_ID] = new ComboBoxWithChecksDelegate(flags);
        delegates[FLAG_ID] = new ComboBoxWithChecksDelegate(flags);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new FilterBamPrompter());
    //no way to include tool support files, so ids passed to functions manually
    proto->addExternalTool("USUPP_SAMTOOLS");//SamToolsExtToolSupport::ET_SAMTOOLS_EXT_ID

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new FilterBamWorkerFactory());
}

/************************************************************************/
/* FilterBamWorker */
/************************************************************************/
FilterBamWorker::FilterBamWorker(Actor *a)
:BaseWorker(a)
,inputUrlPort(NULL)
,outputUrlPort(NULL)
{

}

void FilterBamWorker::init() {
    inputUrlPort = ports.value(INPUT_PORT);
    outputUrlPort = ports.value(OUTPUT_PORT);
}

Task * FilterBamWorker::tick() {
    if (inputUrlPort->hasMessage()) {
        const QString url = takeUrl();
        CHECK(!url.isEmpty(), NULL);

        const QString detectedFormat = FileAndDirectoryUtils::detectFormat(url);
        if(detectedFormat.isEmpty()){
            coreLog.info(tr("Unknown file format: ") + url);
            return NULL;
        }
        if(detectedFormat == BaseDocumentFormats::BAM || detectedFormat == BaseDocumentFormats::SAM){
            const QString outputDir = FileAndDirectoryUtils::createWorkingDir(url, getValue<int>(OUT_MODE_ID), getValue<QString>(CUSTOM_DIR_ID), context->workingDir());

            BamFilterSetting setting;
            setting.outDir = outputDir;
            setting.outName = getTargetName(url, outputDir);
            setting.inputUrl = url;
            setting.inputFormat = detectedFormat;
            setting.outputFormat = getValue<QString>(OUT_FORMAT_ID);
            setting.mapq = getValue<int>(MAPQ_ID);
            setting.acceptFilter = getHexValueByFilterString(getValue<QString>(ACCEPT_FLAG_ID), getFilterCodes());
            setting.skipFilter = getHexValueByFilterString(getValue<QString>(FLAG_ID), getFilterCodes());
            setting.regionFilter = getValue<QString>(REGION_ID);

            ExternalToolSupportTask *t = new SamtoolsViewFilterTask(setting);
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

void FilterBamWorker::cleanup() {
    outUrls.clear();
}

namespace {
    QString getTargetUrl(Task *task) {
        SamtoolsViewFilterTask *filterTask = dynamic_cast<SamtoolsViewFilterTask*>(task);

        if (NULL != filterTask) {
            return filterTask->getResult();
        }
        return "";
    }
}

void FilterBamWorker::sl_taskFinished(Task *task) {
    CHECK(!task->hasError(), );
    CHECK(!task->isCanceled(), );

    QString url = getTargetUrl(task);
    CHECK(!url.isEmpty(), );

    sendResult(url);
    monitor()->addOutputFile(url, getActorId());
}

QString FilterBamWorker::takeUrl() {
    const Message inputMessage = getMessageAndSetupScriptValues(inputUrlPort);
    if (inputMessage.isEmpty()) {
        outputUrlPort->transit();
        return "";
    }

    const QVariantMap data = inputMessage.getData().toMap();
    return data[BaseSlots::URL_SLOT().getId()].toString();
}

void FilterBamWorker::sendResult(const QString &url) {
    const Message message(BaseTypes::STRING_TYPE(), url);
    outputUrlPort->put(message);
}

QString FilterBamWorker::getTargetName(const QString &fileUrl, const QString &outDir){
    QString name = getValue<QString>(OUT_NAME_ID);

    if(name == DEFAULT_NAME || name.isEmpty()){
        name = QFileInfo(fileUrl).fileName();
        name = name + ".filtered.bam";
    }
    if(outUrls.contains(outDir + name)){
        name.append(QString("_%1").arg(outUrls.size()));
    }
    outUrls.append(outDir+name);
    return name;
}

////////////////////////////////////////////////////////
//BamFilterSetting
QStringList BamFilterSetting::getSamtoolsArguments() const{
    QStringList result;

    result << "view";
    if(inputFormat == BaseDocumentFormats::SAM){
        result << "-S";
    }
    if(outputFormat == BaseDocumentFormats::BAM){
        result << "-b";
    }

    if(!acceptFilter.isEmpty()){
        result<< "-f" << acceptFilter;
    }

    if(!skipFilter.isEmpty()){
        result<< "-F" << skipFilter;
    }
    result << "-q" << QString::number(mapq);

    result << "-o" << outDir + outName;

    result << inputUrl;

    if(!regionFilter.isEmpty()){
        QStringList regions = regionFilter.split(" ");
        foreach(const QString& reg, regions){
            result << reg;
        }
    }

    return result;
}

////////////////////////////////////////////////////////
//SamtoolsViewFilterTask
const QString SamtoolsViewFilterTask::SAMTOOLS_ID = "USUPP_SAMTOOLS";

SamtoolsViewFilterTask::SamtoolsViewFilterTask(const BamFilterSetting &settings)
:ExternalToolSupportTask(tr("Samtool view (filter) for %1 ").arg(settings.inputUrl), TaskFlags(TaskFlag_None)),settings(settings),resultUrl(""){

}

void SamtoolsViewFilterTask::prepare(){
    if (settings.inputUrl.isEmpty()){
        setError(tr("No assembly URL to filter"));
        return ;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError(tr("Folder does not exist: ") + outDir.absolutePath());
        return ;
    }

    if(settings.outputFormat == BaseDocumentFormats::BAM && !settings.regionFilter.isEmpty()){
        BAMUtils::createBamIndex(settings.inputUrl, stateInfo);
    }
}

void SamtoolsViewFilterTask::run(){
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

void SamtoolsViewFilterTask::start(const ProcessRun &pRun, const QString &toolName){
    pRun.process->start(pRun.program, pRun.arguments);
    bool started = pRun.process->waitForStarted();
    CHECK_EXT(started, setError(tr("Can not run %1 tool").arg(toolName)), );
}

void SamtoolsViewFilterTask::checkExitCode(QProcess *process, const QString &toolName){
    int exitCode = process->exitCode();
    if (exitCode != EXIT_SUCCESS && !hasError()) {
        setError(tr("%1 tool exited with code %2").arg(toolName).arg(exitCode));
    } else {
        algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
    }
}

} //LocalWorkflow
} //U2
