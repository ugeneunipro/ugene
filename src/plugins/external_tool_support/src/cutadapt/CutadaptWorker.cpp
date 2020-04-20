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
#include <U2Core/Counter.h>
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

#include "CutadaptSupport.h"
#include "CutadaptWorker.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//CutAdaptFastq
const QString CutAdaptFastqWorkerFactory::ACTOR_ID("CutAdaptFastq");

static const QString INPUT_URLS_ID("input-urls");
static const QString ADAPTERS_URL("adapters-url");
static const QString FRONT_URL("front-url");
static const QString ANYWHERE_URL("anywhere-url");

/************************************************************************/
/* CutAdaptFastqPrompter */
/************************************************************************/
QString CutAdaptFastqPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BaseNGSWorker::INPUT_PORT));
    const Actor* producer = input->getProducer(BaseSlots::URL_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("Removes adapter sequences %1.").arg(producerName);
    return doc;
}

/************************************************************************/
/* CutAdaptFastqWorkerFactory */
/************************************************************************/
void CutAdaptFastqWorkerFactory::init() {
    //init data path
    U2DataPath* dataPath = NULL;
    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(CutadaptSupport::ADAPTERS_DATA_NAME);
        if (dp && dp->isValid()){
            dataPath = dp;
        }
    }

    Descriptor desc( ACTOR_ID, CutAdaptFastqWorker::tr("Cut Adapter"),
        CutAdaptFastqWorker::tr("Removes adapter sequences") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(BaseNGSWorker::INPUT_PORT, CutAdaptFastqWorker::tr("Input File"),
            CutAdaptFastqWorker::tr("Set of FASTQ reads files"));
        Descriptor outD(BaseNGSWorker::OUTPUT_PORT, CutAdaptFastqWorker::tr("Output File"),
            CutAdaptFastqWorker::tr("Output FASTQ file(s)"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("cf.input-url", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("cf.output-url", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor outDir(BaseNGSWorker::OUT_MODE_ID, CutAdaptFastqWorker::tr("Output folder"),
            CutAdaptFastqWorker::tr("Select an output folder. <b>Custom</b> - specify the output folder in the 'Custom folder' parameter. "
            "<b>Workflow</b> - internal workflow folder. "
            "<b>Input file</b> - the folder of the input file."));

        Descriptor customDir(BaseNGSWorker::CUSTOM_DIR_ID, CutAdaptFastqWorker::tr("Custom folder"),
            CutAdaptFastqWorker::tr("Select the custom output folder."));

        Descriptor outName(BaseNGSWorker::OUT_NAME_ID, CutAdaptFastqWorker::tr("Output file name"),
            CutAdaptFastqWorker::tr("A name of an output file. If default of empty value is provided the output name is the name of the first file with additional extension."));

        Descriptor adapters(ADAPTERS_URL, CutAdaptFastqWorker::tr("FASTA file with 3' adapters"),
            CutAdaptFastqWorker::tr("A FASTA file with one or multiple sequences of adapter that were ligated to the 3' end. "
                                    "The adapter itself and anything that follows is "
                                    "trimmed. If the adapter sequence ends with the '$' "
                                    "character, the adapter is anchored to the end of the "
                                    "read and only found if it is a suffix of the read."));

        Descriptor front(FRONT_URL, CutAdaptFastqWorker::tr("FASTA file with 5' adapters"),
            CutAdaptFastqWorker::tr("A FASTA file with one or multiple sequences of adapters that were ligated to the 5' end. "
                                                            "If the adapter sequence starts with the character '^', "
                                                            "the adapter is 'anchored'. An anchored adapter must "
                                                            "appear in its entirety at the 5' end of the read (it "
                                                            "is a prefix of the read). A non-anchored adapter may "
                                                            "appear partially at the 5' end, or it may occur within "
                                                            "the read. If it is found within a read, the sequence "
                                                            "preceding the adapter is also trimmed. In all cases, "
                                                            "the adapter itself is trimmed."));


        Descriptor anywhere(ANYWHERE_URL, CutAdaptFastqWorker::tr("FASTA file with 5' and 3' adapters"),
            CutAdaptFastqWorker::tr("A FASTA file with one or multiple sequences of adapters that were ligated to the 5' end or 3' end."));


        a << new Attribute(outDir, BaseTypes::NUM_TYPE(), false, QVariant(FileAndDirectoryUtils::WORKFLOW_INTERNAL));
        Attribute* customDirAttr = new Attribute(customDir, BaseTypes::STRING_TYPE(), false, QVariant(""));
        customDirAttr->addRelation(new VisibilityRelation(BaseNGSWorker::OUT_MODE_ID, FileAndDirectoryUtils::CUSTOM));
        a << customDirAttr;
        a << new Attribute( outName, BaseTypes::STRING_TYPE(), false, QVariant(BaseNGSWorker::DEFAULT_NAME));

        Attribute* adaptersAttr = NULL;
        if (dataPath){
            const QList<QString>& dataNames = dataPath->getDataNames();
            if (!dataNames.isEmpty()){
                adaptersAttr = new Attribute(adapters, BaseTypes::STRING_TYPE(), false, dataPath->getPathByName(dataNames.first()));
            }else{
                adaptersAttr = new Attribute(adapters, BaseTypes::STRING_TYPE(), false);
            }
        }else{
            adaptersAttr = new Attribute(adapters, BaseTypes::STRING_TYPE(), false);
        }
        a << adaptersAttr;

        a << new Attribute( front, BaseTypes::STRING_TYPE(), false, "");
        a << new Attribute( anywhere, BaseTypes::STRING_TYPE(), false, "");
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap directoryMap;
        QString fileDir = CutAdaptFastqWorker::tr("Input file");
        QString workflowDir = CutAdaptFastqWorker::tr("Workflow");
        QString customD = CutAdaptFastqWorker::tr("Custom");
        directoryMap[fileDir] = FileAndDirectoryUtils::FILE_DIRECTORY;
        directoryMap[workflowDir] = FileAndDirectoryUtils::WORKFLOW_INTERNAL;
        directoryMap[customD] = FileAndDirectoryUtils::CUSTOM;
        delegates[BaseNGSWorker::OUT_MODE_ID] = new ComboBoxDelegate(directoryMap);

        delegates[BaseNGSWorker::CUSTOM_DIR_ID] = new URLDelegate("", "", false, true);

        delegates[ADAPTERS_URL] = new URLDelegate("", "", false, false, false);
        delegates[FRONT_URL] = new URLDelegate("", "", false, false, false);
        delegates[ANYWHERE_URL] = new URLDelegate("", "", false, false, false);

    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new CutAdaptFastqPrompter());
    proto->addExternalTool(CutadaptSupport::ET_CUTADAPT_ID);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new CutAdaptFastqWorkerFactory());
}

/************************************************************************/
/* CutAdaptFastqWorker */
/************************************************************************/
CutAdaptFastqWorker::CutAdaptFastqWorker(Actor *a)
:BaseNGSWorker(a)
{

}

QVariantMap CutAdaptFastqWorker::getCustomParameters() const{
    QVariantMap res;
    res.insert(ADAPTERS_URL, getValue<QString>(ADAPTERS_URL));
    res.insert(FRONT_URL, getValue<QString>(FRONT_URL));
    res.insert(ANYWHERE_URL, getValue<QString>(ANYWHERE_URL));
    return res;
}

QString CutAdaptFastqWorker::getDefaultFileName() const{
    return ".cutadapt.fastq";
}

Task *CutAdaptFastqWorker::getTask(const BaseNGSSetting &settings) const {
    if (settings.listeners[0] != NULL) {
        settings.listeners[0]->setLogProcessor(new CutAdaptLogProcessor(monitor(), getActorId()));
    }
    return new CutAdaptFastqTask(settings);
}

//////////////////////////////////////////////////////
//CutAdaptFastqTask
CutAdaptFastqTask::CutAdaptFastqTask(const BaseNGSSetting &settings)
    :BaseNGSTask(settings){

    GCOUNTER(cvar, tvar, "NGS:FASTQCutAdaptTask");
}

void CutAdaptFastqTask::prepareStep(){
    if(settings.customParameters[ADAPTERS_URL].toString().isEmpty()
            && settings.customParameters[FRONT_URL].toString().isEmpty()
            && settings.customParameters[ANYWHERE_URL].toString().isEmpty()){

        algoLog.trace("No adapter sequence files. Input file has been copied to output.");

        bool copied = QFile::copy(settings.inputUrl, settings.outDir + settings.outName);
        if (!copied) {
            algoLog.error(tr("Can not copy the result file to: %1").arg(settings.outDir + settings.outName));
        }
    }else{
        ExternalToolRunTask* etTask = getExternalToolTask(CutadaptSupport::ET_CUTADAPT_ID, new CutAdaptParser());
        CHECK(etTask != NULL, );

        addSubTask(etTask);
    }
}

QStringList CutAdaptFastqTask::getParameters(U2OpStatus &/*os*/) {
    QStringList res;

    QString val;

    val = settings.customParameters[ADAPTERS_URL].toString();
    if(!val.isEmpty()){
        res << "-a";
        res << QString("file:%1").arg(val);
    }

    val = settings.customParameters[FRONT_URL].toString();
    if(!val.isEmpty()){
        res << "-g";
        res << QString("file:%1").arg(val);
    }

    val = settings.customParameters[ANYWHERE_URL].toString();
    if(!val.isEmpty()){
        res << "-b";
        res << QString("file:%1").arg(val);
    }

    const QString detectedFormat = FileAndDirectoryUtils::detectFormat(settings.inputUrl);
    if(detectedFormat.isEmpty()){
        stateInfo.setError(tr("Unknown file format: ") + settings.inputUrl);
        return res;
    }

    if(detectedFormat == BaseDocumentFormats::FASTA){
        res << "-f";
        res << "fasta";
    }else if (detectedFormat == BaseDocumentFormats::FASTQ){

        res << "-f";
        res << "fastq";
    }
    res << "-m" << "1";

    res << settings.inputUrl;

    return res;
}

const QStringList CutAdaptParser::stringsToIgnore = CutAdaptParser::initStringsToIgnore();

void CutAdaptParser::parseErrOutput( const QString& partOfLog ) {
    lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first() = lastErrLine + lastPartOfLog.first();
    lastErrLine = lastPartOfLog.takeLast();
    QString error = parseTextForErrors(lastPartOfLog);
    if (!error.isEmpty()) {
        setLastError(error);
    }
}

QString CutAdaptParser::parseTextForErrors(const QStringList &lastPartOfLog) {
    foreach (const QString &buf, lastPartOfLog) {
        bool ignoredStringFound = false;
        foreach(const QString &ignoredStr, stringsToIgnore) {
            if (buf.contains(ignoredStr, Qt::CaseInsensitive)) {
                ignoredStringFound = true;
                break;
            }
        }
        if(!ignoredStringFound && buf.contains("ERROR", Qt::CaseInsensitive)) {
            return "Cut adapter: " + buf;
        }
    }
    return QString();
}

QStringList CutAdaptParser::initStringsToIgnore() {
    QStringList result;

    result << "Maximum error rate:";
    result << "No. of allowed error";
    result << "max.err";
    result << "error counts";

    return result;
}

CutAdaptLogProcessor::CutAdaptLogProcessor(WorkflowMonitor *monitor, const QString &actor) :
ExternalToolLogProcessor(),
monitor(monitor),
actor(actor) {

}

void CutAdaptLogProcessor::processLogMessage(const QString &message) {
    QString error = CutAdaptParser::parseTextForErrors(QStringList() << message);
    if (!error.isEmpty()) {
        monitor->addError(error, actor, WorkflowNotification::U2_ERROR);
    }
}

} //LocalWorkflow
} //U2
