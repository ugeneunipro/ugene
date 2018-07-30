/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/DnaAssemblyTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Formats/BAMUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2View/DnaAssemblyUtils.h>

#include "BaseShortReadsAlignerWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString READS_URL_SLOT_ID("readsurl");
const QString READS_PAIRED_URL_SLOT_ID("readspairedurl");

const QString IN_TYPE_ID("input-short-reads");
const QString OUT_TYPE_ID("aligned-short-reads");

const QString ASSEBLY_OUT_SLOT_ID("assembly-out");

const QString IN_PORT_DESCR_PAIRED("in-data-paired");
const QString OUT_PORT_DESCR("out-data");

const QString OUTPUT_DIR("output-dir");

const QString OUTPUT_SUBDIR("short-reads-aligner");

const QString OUTPUT_NAME = "outname";

const QString BASE_OUTFILE = "out.sam";

const QString LIBRARY = "library";

const QString FILTER_UNPAIRED = "filter-unpaired";

//////////////////////////////////////////////////////////////////////////
//BaseShortReadsAlignerWorker
BaseShortReadsAlignerWorker::BaseShortReadsAlignerWorker(Actor *a, const QString& algName)
: BaseWorker(a, false)
, algName(algName)
, inChannel(NULL)
, inPairedChannel(NULL)
, output(NULL)
, pairedReadsInput(false)
, filterUnpaired(true)
{
}

void BaseShortReadsAlignerWorker::init() {
    inChannel = ports.value(IN_PORT_DESCR);
    inPairedChannel = ports.value(IN_PORT_DESCR_PAIRED);
    output = ports.value(OUT_PORT_DESCR);
    pairedReadsInput = getValue<QString>(LIBRARY) == "Paired-end";
    filterUnpaired = getValue<bool>(FILTER_UNPAIRED);
    readsFetcher = DatasetFetcher(this, inChannel, context);
    pairedReadsFetcher = DatasetFetcher(this, inPairedChannel, context);

    // Aligners output data keep context (e.g. dataset info) of upstream reads
    output->addComplement(inChannel);
    inChannel->addComplement(output);
}

DnaAssemblyToRefTaskSettings BaseShortReadsAlignerWorker::getSettings(U2OpStatus &os) {
    DnaAssemblyToRefTaskSettings settings;

    //settings.prebuiltIndex = true;
    setGenomeIndex(settings);

    QString outDir = GUrlUtils::createDirectory(
        getValue<QString>(OUTPUT_DIR) + QDir::separator() + getBaseSubdir(),
        "_", os);
    CHECK_OP(os, settings);

    if (!outDir.endsWith(QDir::separator())){
        outDir  = outDir + QDir::separator();
    }

    QString outFileName = getValue<QString>(OUTPUT_NAME);
    if(outFileName.isEmpty()){
        outFileName = getDefaultFileName();
    }
    settings.resultFileName = outDir + outFileName;

    //settings.indexFileName = getValue<QString>(REFERENCE_GENOME);
    //settings.refSeqUrl = GUrl(settings.indexFileName).baseFileName();
    settings.algName = algName;

    QString tmpDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    tmpDir = GUrlUtils::createDirectory(tmpDir + getAlignerSubdir() , "_", os);

    settings.setCustomSettings(getCustomParameters());
    settings.cleanTmpDir = false;
    settings.tmpDirPath = tmpDir;

    return settings;
}

Task *BaseShortReadsAlignerWorker::tick() {
    readsFetcher.processInputMessage();
    if (pairedReadsInput) {
        pairedReadsFetcher.processInputMessage();
    }

    if (isReadyToRun()) {
        U2OpStatus2Log os;
        DnaAssemblyToRefTaskSettings settings = getSettings(os);
        if (os.hasError()) {
            return new FailTask(os.getError());
        }
        settings.pairedReads = pairedReadsInput;
        settings.filterUnpaired = filterUnpaired;
        settings.tmpDirectoryForFilteredFiles = context->workingDir();

        if (pairedReadsInput) {
            settings.shortReadSets << toUrls(readsFetcher.takeFullDataset(), READS_URL_SLOT_ID, ShortReadSet::PairedEndReads, ShortReadSet::UpstreamMate);
            settings.shortReadSets << toUrls(pairedReadsFetcher.takeFullDataset(), READS_PAIRED_URL_SLOT_ID, ShortReadSet::PairedEndReads, ShortReadSet::DownstreamMate);
        } else {
            settings.shortReadSets << toUrls(readsFetcher.takeFullDataset(), READS_URL_SLOT_ID, ShortReadSet::SingleEndReads, ShortReadSet::UpstreamMate);
        }

        DnaAssemblyTaskWithConversions *t = new DnaAssemblyTaskWithConversions(settings);
        t->addListeners(createLogListeners(2));
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    }

    if (dataFinished()) {
        setDone();
        output->setEnded();
    }

    if (pairedReadsInput) {
        const QString error = checkPairedReads();
        if (!error.isEmpty()) {
            return new FailTask(error);
        }
    }
    return NULL;
}

void BaseShortReadsAlignerWorker::cleanup() {
}

bool BaseShortReadsAlignerWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    int hasMsg1 = inChannel->hasMessage();
    bool ended1 = inChannel->isEnded();
    if(!pairedReadsInput) {
        return hasMsg1 || ended1;
    }

    int hasMsg2 = inPairedChannel->hasMessage();
    bool ended2 = inPairedChannel->isEnded();

    if (hasMsg1 && hasMsg2) {
        return true;
    } else if (hasMsg1) {
        return ended2;
    } else if (hasMsg2) {
        return ended1;
    }

    return ended1 && ended2;
}

void BaseShortReadsAlignerWorker::sl_taskFinished() {
    DnaAssemblyTaskWithConversions *t = qobject_cast<DnaAssemblyTaskWithConversions*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    QString url = t->getSettings().resultFileName.getURLString();

    QVariantMap data;
    data[ASSEBLY_OUT_SLOT_ID] =  qVariantFromValue<QString>(url);
    output->put(Message(output->getBusType(), data));

    context->getMonitor()->addOutputFile(url, getActor()->getId());
}

QList<ShortReadSet> BaseShortReadsAlignerWorker::toUrls(const QList<Message> &messages, const QString &urlSlotId, ShortReadSet::LibraryType libType, ShortReadSet::MateOrder order) const {
    QList<ShortReadSet> result;
    foreach (const Message &message, messages) {
        const QVariantMap data = message.getData().toMap();
        if (data.contains(urlSlotId)) {
            const QString url = data.value(urlSlotId).value<QString>();
            result << ShortReadSet(url, libType, order);
        }
    }
    return result;
}

bool BaseShortReadsAlignerWorker::isReadyToRun() const {
    return readsFetcher.hasFullDataset() && (!pairedReadsInput || pairedReadsFetcher.hasFullDataset());
}

bool BaseShortReadsAlignerWorker::dataFinished() const {
    return readsFetcher.isDone() && (!pairedReadsInput || pairedReadsFetcher.isDone());
}

QString BaseShortReadsAlignerWorker::checkPairedReads() const {
    CHECK(pairedReadsInput, "");
    if (readsFetcher.isDone() && pairedReadsFetcher.hasFullDataset()) {
        return tr("Not enough upstream reads datasets");
    }
    if (pairedReadsFetcher.isDone() && readsFetcher.hasFullDataset()) {
        return tr("Not enough downstream reads datasets");
    }
    return "";
}

QString BaseShortReadsAlignerWorker::getAlignerSubdir() const {
    return OUTPUT_SUBDIR;
}

//////////////////////////////////////////////////////////////////////////
//ShortReadsAlignerSlotsValidator
bool ShortReadsAlignerSlotsValidator::validate(const IntegralBusPort *port, ProblemList &problemList) const {
    QVariant busMap = port->getParameter(Workflow::IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributePureValue();
    bool data = isBinded(busMap.value<StrStrMap>(), READS_URL_SLOT_ID);
    if (!data){
        QString dataName = slotName(port, READS_URL_SLOT_ID);
        problemList.append(Problem(IntegralBusPort::tr("The slot must be not empty: '%1'").arg(dataName)));
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//BaseShortReadsAlignerWorkerFactory
int BaseShortReadsAlignerWorkerFactory::getThreadsCount(){
    AppSettings *settings = AppContext::getAppSettings();
    CHECK(NULL != settings, 1);
    AppResourcePool *pool = settings->getAppResourcePool();
    CHECK(NULL != pool, 1);

    int threads = pool->getIdealThreadCount();
    CHECK(0 != threads, 1);
    return threads;
}

void BaseShortReadsAlignerWorkerFactory::addCommonAttributes(QList<Attribute*>& attrs, QMap<QString, PropertyDelegate*>& delegates,
                                                             const QString &descrIndexFolder, const QString &descrIndexBasename) {
    {
        Descriptor referenceInputType(REFERENCE_INPUT_TYPE,
            BaseShortReadsAlignerWorker::tr("Reference input type"),
            BaseShortReadsAlignerWorker::tr("Select \"Sequence\" to input a reference genome as a sequence file. "
            "<br/>Note that any sequence file format, supported by UGENE, is allowed (FASTA, GenBank, etc.). "
            "<br/>The index will be generated automatically in this case. "
            "<br/>Select \"Index\" to input already generated index files, specific for the tool."));

        Descriptor refGenome(REFERENCE_GENOME,
            BaseShortReadsAlignerWorker::tr("Reference genome"),
            BaseShortReadsAlignerWorker::tr("Path to indexed reference genome."));

        Descriptor indexDir(INDEX_DIR,
            descrIndexFolder,
            BaseShortReadsAlignerWorker::tr("The folder with the index for the reference sequence."));

        Descriptor indexBasename(INDEX_BASENAME,
            descrIndexBasename,
            BaseShortReadsAlignerWorker::tr("The basename of the index for the reference sequence."));

        Descriptor outName(OUTPUT_NAME,
            BaseShortReadsAlignerWorker::tr("Output file name"),
            BaseShortReadsAlignerWorker::tr("Base name of the output file. 'out.sam' by default"));

        Descriptor outDir(OUTPUT_DIR,
            BaseShortReadsAlignerWorker::tr("Output folder"),
            BaseShortReadsAlignerWorker::tr("Folder to save output files."));

        Descriptor library(LIBRARY,
            BaseShortReadsAlignerWorker::tr("Library"),
            BaseShortReadsAlignerWorker::tr("Is this library mate-paired?"));

        Descriptor filter(FILTER_UNPAIRED,
            BaseShortReadsAlignerWorker::tr("Filter unpaired reads"),
            BaseShortReadsAlignerWorker::tr("Should the reads be checked for incomplete pairs?"));

        attrs << new Attribute(referenceInputType, BaseTypes::STRING_TYPE(), true, QVariant(DnaAssemblyToRefTaskSettings::SEQUENCE));
        Attribute* attrRefGenom = new Attribute(refGenome, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrRefGenom->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::SEQUENCE));
        attrs << attrRefGenom;
        Attribute* attrIndexDir = new Attribute(indexDir, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrIndexDir->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::INDEX));
        attrs << attrIndexDir;
        Attribute* attrIndexBasename = new Attribute(indexBasename, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, QVariant(""));
        attrIndexBasename->addRelation(new VisibilityRelation(REFERENCE_INPUT_TYPE, DnaAssemblyToRefTaskSettings::INDEX));
        attrs << attrIndexBasename;
        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), true, QVariant(""));
        attrs << new Attribute(outName, BaseTypes::STRING_TYPE(), true, QVariant(BASE_OUTFILE));

        Attribute* libraryAttr = new Attribute(library, BaseTypes::STRING_TYPE(), false, QVariant("Single-end"));
        QVariantList visibilityValues;
        visibilityValues << QVariant("Paired-end");
        libraryAttr->addPortRelation(new PortRelationDescriptor(IN_PORT_DESCR_PAIRED, visibilityValues));
        attrs << libraryAttr;

        Attribute* filterAttr = new Attribute(filter, BaseTypes::BOOL_TYPE(), false, true);
        filterAttr->addRelation(new VisibilityRelation(LIBRARY, "Paired-end"));
        attrs << filterAttr;
    }

    {
        QVariantMap rip;
        rip[BaseShortReadsAlignerWorker::tr("Sequence")] = DnaAssemblyToRefTaskSettings::SEQUENCE;
        rip[BaseShortReadsAlignerWorker::tr("Index")] = DnaAssemblyToRefTaskSettings::INDEX;
        delegates[REFERENCE_INPUT_TYPE] = new ComboBoxDelegate(rip);

        delegates[REFERENCE_GENOME] = new URLDelegate("", "", false, false, false);
        delegates[INDEX_DIR] = new URLDelegate("", "", false, true, false, NULL, "", true);

        delegates[OUTPUT_DIR] = new URLDelegate("", "", false, true);

        QVariantMap libMap;
        libMap["Single-end"] = "Single-end";
        libMap["Paired-end"] = "Paired-end";
        ComboBoxDelegate* libDelegate = new ComboBoxDelegate(libMap);
        delegates[LIBRARY] = libDelegate;

        delegates[FILTER_UNPAIRED] = new ComboBoxWithBoolsDelegate();
    }
}

QList<PortDescriptor*> BaseShortReadsAlignerWorkerFactory::getPortDescriptors() {
    QList<PortDescriptor*> portDescs;

    //in port
    QMap<Descriptor, DataTypePtr> inTypeMap;
    QMap<Descriptor, DataTypePtr> inTypeMapPaired;
    Descriptor readsDesc(READS_URL_SLOT_ID,
        BaseShortReadsAlignerWorker::tr("URL of a file with reads"),
        BaseShortReadsAlignerWorker::tr("Input reads to be aligned."));
    Descriptor readsPairedDesc(READS_PAIRED_URL_SLOT_ID,
        BaseShortReadsAlignerWorker::tr("URL of a file with mate reads"),
        BaseShortReadsAlignerWorker::tr("Input mate reads to be aligned."));

    inTypeMap[readsDesc] = BaseTypes::STRING_TYPE();
    inTypeMapPaired[readsPairedDesc] = BaseTypes::STRING_TYPE();

    Descriptor inPortDesc(IN_PORT_DESCR,
        BaseShortReadsAlignerWorker::tr("Input data"),
        BaseShortReadsAlignerWorker::tr("Input reads to be aligned."));

    Descriptor inPortDescPaired(IN_PORT_DESCR_PAIRED,
        BaseShortReadsAlignerWorker::tr("Reverse FASTQ file"),
        BaseShortReadsAlignerWorker::tr("Reverse paired reads to be aligned."));

    DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID, inTypeMap));
    DataTypePtr inTypeSetPaired(new MapDataType(IN_TYPE_ID, inTypeMapPaired));
    PortDescriptor* readsDescriptor = new PortDescriptor(inPortDesc, inTypeSet, true);
    PortDescriptor* readsDescriptor2 = new PortDescriptor(inPortDescPaired, inTypeSetPaired, true, false, IntegralBusPort::BLIND_INPUT);
    portDescs << readsDescriptor2;
    portDescs << readsDescriptor;

    //out port
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor assemblyOutDesc(ASSEBLY_OUT_SLOT_ID,
        BaseShortReadsAlignerWorker::tr("Assembly URL"),
        BaseShortReadsAlignerWorker::tr("Output assembly URL."));

    Descriptor outPortDesc(OUT_PORT_DESCR,
        BaseShortReadsAlignerWorker::tr("Output data"),
        BaseShortReadsAlignerWorker::tr("Output assembly files."));

    outTypeMap[assemblyOutDesc] = BaseTypes::STRING_TYPE();

    DataTypePtr outTypeSet(new MapDataType(OUT_TYPE_ID, outTypeMap));
    portDescs << new PortDescriptor(outPortDesc, outTypeSet, false, true);

    return portDescs;
}

QString ShortReadsAlignerPrompter::composeRichDoc() {
    QString res = "";

    Actor* readsProducer = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR))->getProducer(READS_URL_SLOT_ID);
    Port* pairedPort = target->getPort(IN_PORT_DESCR_PAIRED);

    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString readsUrl = readsProducer ? readsProducer->getLabel() : unsetStr;
    if(pairedPort->isEnabled()) {
        IntegralBusPort* pairedBus = qobject_cast<IntegralBusPort*>(pairedPort);
        Actor* pairedReadsProducer = pairedBus->getProducer(READS_PAIRED_URL_SLOT_ID);
        QString pairedReadsUrl = pairedReadsProducer ? pairedReadsProducer->getLabel() : unsetStr;
        res.append(tr("Aligns upstream oriented reads from <u>%1</u> and downstream oriented reads from <u>%2</u> ").arg(readsUrl).arg(pairedReadsUrl));
    } else {
        res.append(tr("Maps input reads from <u>%1</u> ").arg(readsUrl));
    }

    QVariant inputType = getParameter(REFERENCE_INPUT_TYPE);
    if (inputType == DnaAssemblyToRefTaskSettings::INDEX) {
        QString baseName = getHyperlink(INDEX_BASENAME, getURL(INDEX_BASENAME));
        res.append(tr(" to reference sequence with index <u>%1</u>.").arg(baseName));
    } else {
        QString genome = getHyperlink(REFERENCE_GENOME, getURL(REFERENCE_GENOME));
        res.append(tr(" to reference sequence <u>%1</u>.").arg(genome));
    }

    return res;
}
} //LocalWorkflow
} //U2