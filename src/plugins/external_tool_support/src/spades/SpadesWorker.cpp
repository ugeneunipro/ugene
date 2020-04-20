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

#include <QScopedPointer>

#include <U2Algorithm/GenomeAssemblyMultiTask.h>
#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/GUrlUtils.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "SpadesPortRelationDescriptor.h"
#include "SpadesSlotRelationDescriptor.h"
#include "SpadesSupport.h"
#include "SpadesWorker.h"
#include "SpadesTask.h"
#include "SpadesDelegate.h"

namespace U2 {
namespace LocalWorkflow {

const QString SpadesWorkerFactory::ACTOR_ID = "spades-id";

const QStringList SpadesWorkerFactory::READS_URL_SLOT_ID_LIST = QStringList() <<
                                                                 "readsurl" <<
                                                                 "readsurl-2" <<
                                                                 "readsurl-3" <<
                                                                 "readsurl-4" <<
                                                                 "readsurl-5" <<
                                                                 "readsurl-6" <<
                                                                 "readsurl-7" <<
                                                                 "readsurl-8" <<
                                                                 "readsurl-9" <<
                                                                 "readsurl-10";
const QStringList SpadesWorkerFactory::READS_PAIRED_URL_SLOT_ID_LIST = QStringList() <<
                                                                  "readspairedurl" <<
                                                                  "readspairedurl-2" <<
                                                                  "readspairedurl-3";

const QStringList SpadesWorkerFactory::IN_TYPE_ID_LIST = QStringList() <<
                                                       "spades-paired-data" <<
                                                       "spades-paired-data-2" <<
                                                       "spades-paired-data-3"<<
                                                       "spades-data" <<
                                                       "spades-data-2" <<
                                                       "spades-data-3" <<
                                                       "spades-data-4" <<
                                                       "spades-data-5" <<
                                                       "spades-data-6" <<
                                                       "spades-data-7";

const QString SpadesWorkerFactory::OUT_TYPE_ID = "spades-data-out";

const QString SpadesWorkerFactory::SCAFFOLD_OUT_SLOT_ID = "scaffolds-out";
const QString SpadesWorkerFactory::CONTIGS_URL_OUT_SLOT_ID = "contigs-out";

const QString SpadesWorkerFactory::SEQUENCING_PLATFORM_ID = "platform-id";

const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_UNPAIRED = "in-unpaired-reads";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_CSS = "in-pac-bio-ccs-reads";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_CLR = "in-pac-bio-clr-reads";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_NANOPORE = "in-oxford-nanopore-reads";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_SANGER = "in-sanger-reads";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_TRUSTED = "in-trusted-contigs";
const QString SpadesWorkerFactory::IN_PORT_ID_SINGLE_UNTRUSTED = "in-untrusted-contigs";
const QString SpadesWorkerFactory::IN_PORT_ID_PAIR_DEFAULT = "in-data";
const QString SpadesWorkerFactory::IN_PORT_ID_PAIR_MATE = "in-mate-pairs";
const QString SpadesWorkerFactory::IN_PORT_ID_PAIR_HQ_MATE = "in-high-quality-mate-pairs";

const QStringList SpadesWorkerFactory::IN_PORT_ID_LIST = QStringList() << SpadesWorkerFactory::IN_PORT_ID_SINGLE_UNPAIRED
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_CSS
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_CLR
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_NANOPORE
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_SANGER
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_TRUSTED
                                                                       << SpadesWorkerFactory::IN_PORT_ID_SINGLE_UNTRUSTED;

const QStringList SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST = QStringList() << SpadesWorkerFactory::IN_PORT_ID_PAIR_DEFAULT
                                                                              << SpadesWorkerFactory::IN_PORT_ID_PAIR_MATE
                                                                              << SpadesWorkerFactory::IN_PORT_ID_PAIR_HQ_MATE;

const QString SpadesWorkerFactory::MAP_TYPE_ID = "map";

const QString SpadesWorkerFactory::OUT_PORT_DESCR = "out-data";

const QString SpadesWorkerFactory::OUTPUT_DIR = "output-dir";

const QString SpadesWorkerFactory::BASE_SPADES_SUBDIR = "spades";

const StrStrMap SpadesWorkerFactory::PORT_ID_2_YAML_LIBRARY_NAME = SpadesWorkerFactory::getPortId2YamlLibraryName();

const QString SpadesWorkerFactory::getPortNameById(const QString& portId) {
    QString res;
    if (portId == IN_PORT_ID_LIST[0]) {
        res = tr("unpaired reads");
    } else if (portId == IN_PORT_ID_LIST[1]) {
        res = tr("PacBio CCS reads");
    } else if (portId == IN_PORT_ID_LIST[2]) {
        res = tr("PacBio CLR reads");
    } else if (portId == IN_PORT_ID_LIST[3]) {
        res = tr("Oxford Nanopore reads");
    } else if (portId == IN_PORT_ID_LIST[4]) {
        res = tr("Sanger reads");
    } else if (portId == IN_PORT_ID_LIST[5]) {
        res = tr("trusted contigs");
    } else if (portId == IN_PORT_ID_LIST[6]) {
        res = tr("untrusted contigs");
    } else if (portId == IN_PORT_PAIRED_ID_LIST[0]) {
        res = tr("paired-end reads");
    } else if (portId == IN_PORT_PAIRED_ID_LIST[1]) {
        res = tr("mate-pairs");
    } else if (portId == IN_PORT_PAIRED_ID_LIST[2]) {
        res = tr("high-quality mate-pairs");
    } else {
        FAIL("Incorrect port id", QString());
    }

    return res;
}

StrStrMap SpadesWorkerFactory::getPortId2YamlLibraryName() {
    StrStrMap map;
    map.insert(IN_PORT_ID_LIST[0], LIB_SINGLE_UNPAIRED);
    map.insert(IN_PORT_ID_LIST[1], LIB_SINGLE_CSS);
    map.insert(IN_PORT_ID_LIST[2], LIB_SINGLE_CLR);
    map.insert(IN_PORT_ID_LIST[3], LIB_SINGLE_NANOPORE);
    map.insert(IN_PORT_ID_LIST[4], LIB_SINGLE_SANGER);
    map.insert(IN_PORT_ID_LIST[5], LIB_SINGLE_TRUSTED);
    map.insert(IN_PORT_ID_LIST[6], LIB_SINGLE_UNTRUSTED);
    map.insert(IN_PORT_PAIRED_ID_LIST[0], LIB_PAIR_DEFAULT);
    map.insert(IN_PORT_PAIRED_ID_LIST[1], LIB_PAIR_MATE);
    map.insert(IN_PORT_PAIRED_ID_LIST[2], LIB_PAIR_MATE_HQ);
    return map;
}

const QString SpadesWorker::DATASET_TYPE_STANDARD_ISOLATE = "Standard isolate";
const QString SpadesWorker::DATASET_TYPE_MDA_SINGLE_CELL = "MDA single-cell";

const QString SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_AND_ASSEMBLY = "Error correction and assembly";
const QString SpadesWorker::RUNNING_MODE_ASSEMBLY_ONLY = "Assembly only";
const QString SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_ONLY = "Error correction only";

const QString SpadesWorker::K_MER_AUTO = "Auto";


/************************************************************************/
/* Worker */
/************************************************************************/
SpadesWorker::SpadesWorker(Actor *p)
    : BaseWorker(p, false)
    , output(NULL) {}

void SpadesWorker::init() {
    const QStringList portIds = QStringList() <<
        SpadesWorkerFactory::IN_PORT_PAIRED_ID_LIST <<
        SpadesWorkerFactory::IN_PORT_ID_LIST;
    foreach(const QString& portId, portIds) {
        IntegralBus* channel = ports.value(portId);
        inChannels << channel;
        readsFetchers << DatasetFetcher(this, channel, context);
    }
    output = ports.value(SpadesWorkerFactory::OUT_PORT_DESCR);
}

namespace {

QVariantMap uniteUniquely(const QVariantMap &first, const QVariantMap &second) {
    QVariantMap result;
    foreach (const QString &key, first.keys()) {
        result[key] = first.value(key);
    }

    foreach (const QString &key, second.keys()) {
        result[key] = second.value(key);
    }
    return result;
}

}

Task *SpadesWorker::tick() {
    U2OpStatus2Log os;
    trySetDone(os);
    CHECK(!os.hasError(), new FailTask(os.getError()));

    CHECK(processInputMessagesAndCheckReady(), NULL);

    GenomeAssemblyTaskSettings settings = getSettings(os);
    CHECK(!os.hasError(), new FailTask(os.getError()));

    QVariantMap unitedPortContext;

    int messageCounter = 0;
    int messageId = MessageMetadata::INVALID_ID;
    for (int i = 0; i < readsFetchers.size(); i++) {
        const bool isPortEnabled = readsFetchers[i].hasFullDataset();
        CHECK_CONTINUE(isPortEnabled);

        AssemblyReads read;
        const QString portId = ports.key(inChannels[i]);
        read.libName = SpadesWorkerFactory::PORT_ID_2_YAML_LIBRARY_NAME.value(portId, "");

        unitedPortContext = uniteUniquely(unitedPortContext, inChannels[i]->getLastMessageContext());

        bool isPaired = false;
        const int index = SpadesWorkerFactory::getReadsUrlSlotIdIndex(portId, isPaired);

        QList<Message> fullDataset = readsFetchers[i].takeFullDataset();
        foreach(const Message& m, fullDataset) {
            messageCounter++;
            messageId = m.getMetadataId();

            QVariantMap data = m.getData().toMap();

            const QString urlSlotId = SpadesWorkerFactory::READS_URL_SLOT_ID_LIST[index];
            const QString readsUrl = data[urlSlotId].toString();
            read.left << readsUrl;

            if (isPaired) {
                const QString urlPairedSlotId = SpadesWorkerFactory::READS_PAIRED_URL_SLOT_ID_LIST[index];
                const QString readsPairedUrl = data[urlPairedSlotId].toString();
                read.right << readsPairedUrl;
            }
        }

        if (isPaired) {
            QVariant inputData = settings.getCustomValue(SpadesTask::OPTION_INPUT_DATA, QVariant());
            SAFE_POINT(inputData != QVariant(), tr("Incorrect input data"), new FailTask(tr("Incorrect input data")));

            QVariantMap mapData = inputData.toMap();
            QStringList values = mapData[portId].toString().split(":");
            SAFE_POINT(values.size() == 2, tr("Incorrect port values"), new FailTask(tr("Incorrect port values")));

            read.orientation = values.first();
            read.readType = values.last();
        }

        settings.reads << read;
    }
    CHECK(!settings.reads.isEmpty(), NULL);

    int currentMetadataId = messageCounter == 1 ? messageId : MessageMetadata::INVALID_ID;
    output->setContext(unitedPortContext, currentMetadataId);

    settings.listeners = createLogListeners();
    GenomeAssemblyMultiTask* t = new GenomeAssemblyMultiTask(settings);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

void SpadesWorker::cleanup() {
}

bool SpadesWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    bool res = true;
    QList<Port*> inPorts = actor->getInputPorts();
    foreach (Port* port, inPorts) {
        CHECK_CONTINUE(port->isEnabled());

        IntegralBus *inChannel = ports.value(port->getId());
        int hasMsg = inChannel->hasMessage();
        bool ended = inChannel->isEnded();
        res = res && (hasMsg || ended);
    }

    return res;
}

bool SpadesWorker::processInputMessagesAndCheckReady() {
    bool result = true;
    QList<Port*> inPorts = actor->getInputPorts();
    for (int i = 0; i < readsFetchers.size(); i++){
        const QString portId = readsFetchers[i].getPortId();
        Port* port = actor->getPort(portId);
        SAFE_POINT(port != NULL, QString("Port with id %1 not found").arg(portId), false);
        CHECK_CONTINUE(port->isEnabled());

        readsFetchers[i].processInputMessage();
        result = result && readsFetchers[i].hasFullDataset();
        CHECK(result, false);
    }

    return result;
}

void SpadesWorker::trySetDone(U2OpStatus &os) {
    CHECK(!isDone(), );

    bool isDone = true;
    bool hasReadyFetcher = false;
    bool hasDoneFetcher = false;
    for (int i = 0; i < readsFetchers.size(); i++){
        const QString portId = readsFetchers[i].getPortId();
        Port* port = actor->getPort(portId);
        SAFE_POINT(port != NULL, QString("Port with id %1 not found").arg(portId), );
        CHECK_CONTINUE(port->isEnabled());

        const bool fetcherHasFullDataset = readsFetchers[i].hasFullDataset();
        const bool fetcherIsDone = readsFetchers[i].isDone();
        hasReadyFetcher = hasReadyFetcher || fetcherHasFullDataset;
        hasDoneFetcher = hasDoneFetcher || fetcherIsDone;
        isDone = isDone && fetcherIsDone;
    }

    if (hasReadyFetcher && hasDoneFetcher) {
        os.setError(tr("Some input data elements sent data while some elements already finished their work. Check that all input data elements have the same datasets quantity."));
    }

    if (isDone ) {
        setDone();
        output->setEnded();
    }
}

void SpadesWorker::sl_taskFinished() {
    GenomeAssemblyMultiTask *t = dynamic_cast<GenomeAssemblyMultiTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled() || t->getResultUrl().isEmpty()) {
        return;
    }

    QString scaffoldUrl = t->getResultUrl();
    SpadesTask *spadesTask = qobject_cast<SpadesTask*>(t->getAssemblyTask());
    CHECK(spadesTask != NULL, );
    QString contigsUrl = spadesTask->getContigsUrl();

    QVariantMap data;
    data[SpadesWorkerFactory::SCAFFOLD_OUT_SLOT_ID] = qVariantFromValue<QString>(scaffoldUrl);
    data[SpadesWorkerFactory::CONTIGS_URL_OUT_SLOT_ID] = qVariantFromValue<QString>(contigsUrl);
    output->put(Message(output->getBusType(), data));

    context->getMonitor()->addOutputFile(scaffoldUrl, getActor()->getId());
    context->getMonitor()->addOutputFile(contigsUrl, getActor()->getId());

}

GenomeAssemblyTaskSettings SpadesWorker::getSettings(U2OpStatus &os) {
    GenomeAssemblyTaskSettings settings;

    settings.algName = SpadesSupport::ET_SPADES;
    settings.openView = false;
    QString outDir = getValue<QString>(SpadesWorkerFactory::OUTPUT_DIR);
    if (outDir.isEmpty()) {
        outDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    }
    outDir = GUrlUtils::createDirectory(outDir + "/" + SpadesWorkerFactory::BASE_SPADES_SUBDIR, "_", os);
    CHECK_OP(os, settings);
    if (outDir.endsWith("/")) {
        outDir.chop(1);
    }

    settings.outDir = outDir;

    QMap<QString, QVariant> customSettings;
    customSettings.insert(SpadesTask::OPTION_THREADS, getValue<int>(SpadesTask::OPTION_THREADS));
    customSettings.insert(SpadesTask::OPTION_MEMLIMIT, getValue<int>(SpadesTask::OPTION_MEMLIMIT));
    customSettings.insert(SpadesTask::OPTION_K_MER, getValue<QString>(SpadesTask::OPTION_K_MER));
    customSettings.insert(SpadesTask::OPTION_INPUT_DATA, getValue<QVariantMap>(SpadesTask::OPTION_INPUT_DATA));
    customSettings.insert(SpadesTask::OPTION_DATASET_TYPE, getValue<QString>(SpadesTask::OPTION_DATASET_TYPE));
    customSettings.insert(SpadesTask::OPTION_RUNNING_MODE, getValue<QString>(SpadesTask::OPTION_RUNNING_MODE));

    settings.setCustomSettings(customSettings);

    return settings;
}

/************************************************************************/
/* Factory */
/************************************************************************/

void SpadesWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;

    //in port
    QList<Descriptor> readDescriptors;
    foreach(const QString& readId, QStringList() << IN_PORT_PAIRED_ID_LIST << IN_PORT_ID_LIST) {
        const QString dataName = SpadesWorkerFactory::getPortNameById(readId);
        readDescriptors << Descriptor(readId,
            SpadesWorker::tr("Input %1").arg(dataName),
            SpadesWorker::tr("Input %1 to be assembled with SPAdes.").arg(dataName));
    }

    QList<Descriptor> inputDescriptors;
    foreach(const QString& id, READS_URL_SLOT_ID_LIST) {
        inputDescriptors << Descriptor(id,
            SpadesWorker::tr("File URL 1"),
            SpadesWorker::tr("File URL 1."));
    }
    SAFE_POINT(READS_URL_SLOT_ID_LIST.size() == inputDescriptors.size(),
               "Incorrect descriptors quantity", );

    QList<Descriptor> inputPairedDescriptors;
    foreach(const QString& pairedId, READS_PAIRED_URL_SLOT_ID_LIST) {
        inputPairedDescriptors << Descriptor(pairedId,
            SpadesWorker::tr("File URL 2"),
            SpadesWorker::tr("File URL 2."));
    }
    SAFE_POINT(READS_PAIRED_URL_SLOT_ID_LIST.size() == inputPairedDescriptors.size(),
               "Incorrect paired descriptors quantity", );

    for (int i = 0; i < inputDescriptors.size(); i++) {
        const Descriptor& desc = inputDescriptors[i];

        QMap<Descriptor, DataTypePtr> inTypeMap;
        inTypeMap[desc] = BaseTypes::STRING_TYPE();
        if (i < inputPairedDescriptors.size()) {
            const Descriptor& pairedDesc = inputPairedDescriptors[i];
            inTypeMap[pairedDesc] = BaseTypes::STRING_TYPE();
        }

        DataTypePtr inTypeSet(new MapDataType(IN_TYPE_ID_LIST[i], inTypeMap));
        portDescs << new PortDescriptor(readDescriptors[i], inTypeSet, true);
    }

    //out port
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor scaffoldOutDesc(SCAFFOLD_OUT_SLOT_ID,
        SpadesWorker::tr("Scaffolds URL"),
        SpadesWorker::tr("Output scaffolds URL."));

    Descriptor contigsOutDesc(CONTIGS_URL_OUT_SLOT_ID,
        SpadesWorker::tr("Contigs URL"),
        SpadesWorker::tr("Output contigs URL."));

    Descriptor outPortDesc(OUT_PORT_DESCR,
        SpadesWorker::tr("Output File"),
        SpadesWorker::tr("Output assembly files."));

    outTypeMap[scaffoldOutDesc] = BaseTypes::STRING_TYPE();
    outTypeMap[contigsOutDesc] = BaseTypes::STRING_TYPE();

    DataTypePtr outTypeSet(new MapDataType(OUT_TYPE_ID, outTypeMap));
    portDescs << new PortDescriptor(outPortDesc, outTypeSet, false, true);


    QList<Attribute*> attrs;
    {
        Descriptor inputData(SpadesTask::OPTION_INPUT_DATA,
            SpadesWorker::tr("Input data"),
                             QCoreApplication::tr("<html>"
                                 "Select the type of input for SPAdes. URL(s) to the input files of the selected type(s) should be provided to the corresponding port(s) of the workflow element."
                                 "<p>At least one library of the following types is required:"
                                 "<ul>"
                                 "<li>Illumina paired-end/high-quality mate-pairs/unpaired reads</li>"
                                 "<li>IonTorrent paired-end/high-quality mate-pairs/unpaired reads</li>"
                                 "<li>PacBio CCS reads (at least 5 reads coverage is recommended)</li>"
                                 "</ul></p>"
                                 "<p>It is strongly suggested to provide multiple paired-end and mate-pair libraries according to their insert size (from smallest to longest).</p>"
                                 "<p>Additionally, one may input Oxford Nanopore reads, Sanger reads, contigs generated by other assembler(s), etc."
                                 "Note that Illumina and IonTorrent libraries should not be assembled together. All other types of input data are compatible.</p>"
                                 "<p>It is also possible to set up reads orientation (forward-reverse (fr), reverse-forward (rf), forward-forward (ff)) and specify whether paired reads are separate or interlaced.</p>"
                                 "<p>Illumina, IonTorrent or PacBio CCS reads should be provided in FASTQ format.<br>"
                                 "Illumina or PacBio read may also be provided in FASTA format. Error correction should be skipped in this case (see the \"Running mode\" parameter).<br>"
                                 "Sanger, Oxford Nanopore and PacBio CLR reads can be provided in both formats since SPAdes does not run error correction for these types of data.</p>"
                                 "</html>"));

        Descriptor outDir(OUTPUT_DIR,
            SpadesWorker::tr("Output folder"),
            SpadesWorker::tr("Folder to save Spades output files."));

        Descriptor threads(SpadesTask::OPTION_THREADS,
            SpadesWorker::tr("Number of threads"),
            SpadesWorker::tr("Number of threads (-t)."));

        Descriptor memLim(SpadesTask::OPTION_MEMLIMIT,
            SpadesWorker::tr("Memory limit"),
            SpadesWorker::tr("Memory limit (-m)."));

        Descriptor datasetType(SpadesTask::OPTION_DATASET_TYPE,
            SpadesWorker::tr("Dataset type"),
            SpadesWorker::tr("Select the input dataset type: standard isolate (the default value) or multiple displacement amplification (corresponds to --sc)."));

        Descriptor rMode(SpadesTask::OPTION_RUNNING_MODE,
            SpadesWorker::tr("Running mode"),
            SpadesWorker::tr("By default, SPAdes performs both read error correction and assembly. You can select leave one of only (corresponds to --only-assembler, --only-error-correction).<br><br>\
                              Error correction is performed using BayesHammer module in case of Illumina input reads and IonHammer in case of IonTorrent data. Note that you should not use error correction \
                              in case input reads do not have quality information(e.g. FASTA input files are provided)."));

         Descriptor kMer(SpadesTask::OPTION_K_MER,
             SpadesWorker::tr("K-mers"),
             SpadesWorker::tr("k-mer sizes (-k)."));

        QVariantMap defaultValue;
        defaultValue.insert(IN_PORT_PAIRED_ID_LIST[0], QString("%1:%2").arg(ORIENTATION_FR).arg(TYPE_SINGLE));
        defaultValue.insert(SEQUENCING_PLATFORM_ID, PLATFORM_ILLUMINA);
        Attribute* inputAttr = new Attribute(inputData, BaseTypes::MAP_TYPE(), false, QVariant::fromValue<QVariantMap>(defaultValue));

        foreach (const QString& read, IN_PORT_ID_LIST) {
            inputAttr->addPortRelation(new SpadesPortRelationDescriptor(read, QVariantList() << read));
        }

        foreach (const QString& pairedRead, IN_PORT_PAIRED_ID_LIST) {
            inputAttr->addPortRelation(new SpadesPortRelationDescriptor(pairedRead, QVariantList() << pairedRead));
            bool unused = false;
            const int index = getReadsUrlSlotIdIndex(pairedRead, unused);
            assert(unused);
            const QString slotId = SpadesWorkerFactory::READS_PAIRED_URL_SLOT_ID_LIST[index];
            inputAttr->addSlotRelation(new SpadesSlotRelationDescriptor(pairedRead, slotId));
        }

        attrs << inputAttr;
        attrs << new Attribute(datasetType, BaseTypes::STRING_TYPE(), true, SpadesWorker::DATASET_TYPE_STANDARD_ISOLATE);
        attrs << new Attribute(rMode, BaseTypes::STRING_TYPE(), true, SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_AND_ASSEMBLY);
        attrs << new Attribute(kMer, BaseTypes::STRING_TYPE(), true, SpadesWorker::K_MER_AUTO);
        attrs << new Attribute(threads, BaseTypes::NUM_TYPE(), false, QVariant(16));
        attrs << new Attribute(memLim, BaseTypes::NUM_TYPE(), false, QVariant(250));
        attrs << new Attribute(outDir, BaseTypes::STRING_TYPE(), Attribute::CanBeEmpty | Attribute::Required);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, SpadesWorker::tr("Auto"));
        delegates[OUTPUT_DIR] = new URLDelegate(outputUrlTags, "spades/output", false, true);

        QVariantMap spinMapThreads;
        spinMapThreads["minimum"] = QVariant(1);
        spinMapThreads["maximum"] = QVariant(INT_MAX);

        QVariantMap spinMapMemory(spinMapThreads);
        spinMapMemory["suffix"] = SpadesWorker::tr(" Gb");

        delegates[SpadesTask::OPTION_THREADS] = new SpinBoxDelegate(spinMapThreads);
        delegates[SpadesTask::OPTION_MEMLIMIT] = new SpinBoxDelegate(spinMapMemory);

        QVariantMap contentMap;
        contentMap[SpadesWorker::tr("Standard isolate")] = SpadesWorker::DATASET_TYPE_STANDARD_ISOLATE;
        contentMap[SpadesWorker::tr("MDA single-cell")] = SpadesWorker::DATASET_TYPE_MDA_SINGLE_CELL;
        delegates[SpadesTask::OPTION_DATASET_TYPE] = new ComboBoxDelegate(contentMap);

        QVariantMap contentMap2;
        contentMap2[SpadesWorker::tr("Error correction and assembly")] = SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_AND_ASSEMBLY;
        contentMap2[SpadesWorker::tr("Assembly only")] = SpadesWorker::RUNNING_MODE_ASSEMBLY_ONLY;
        contentMap2[SpadesWorker::tr("Error correction only")] = SpadesWorker::RUNNING_MODE_ERROR_CORRECTION_ONLY;
        delegates[SpadesTask::OPTION_RUNNING_MODE] = new ComboBoxDelegate(contentMap2);

        delegates[SpadesTask::OPTION_INPUT_DATA] = new SpadesDelegate();
    }

    Descriptor protoDesc(SpadesWorkerFactory::ACTOR_ID,
        SpadesWorker::tr("Assemble Reads with SPAdes"),
        SpadesWorker::tr("In general, SPAdes (St. Petersburg genome assembler) is an assembly toolkit containing various assembly pipelines. \
                          This workflow element provides GUI for the main SPAdes executable script. One can specify Illumina, IonTorrent or \
                          PacBio reads as input. Hybrid assemblies are also possible, for example, with Oxford Nanopore or Sanger reads.<br><br>\
                          To use the element, configure the type of input in the \"Input data\" parameter. The corresponding input ports will appear \
                          on the element. Provide URL(s) to the corresponding FASTA or FASTQ file(s) to these ports."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setPrompter(new SpadesPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    for (int i = 0; i < IN_PORT_PAIRED_ID_LIST.size(); i++) {
        proto->setPortValidator(IN_PORT_PAIRED_ID_LIST[i], new PairedReadsPortValidator(READS_URL_SLOT_ID_LIST[i], READS_PAIRED_URL_SLOT_ID_LIST[i]));
    }
    proto->addExternalTool(SpadesSupport::ET_SPADES_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_MAP_ASSEMBLE_READS(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new SpadesWorkerFactory());
}

Worker *SpadesWorkerFactory::createWorker(Actor *a) {
    return new SpadesWorker(a);
}

int SpadesWorkerFactory::getReadsUrlSlotIdIndex(const QString &portId, bool &isPaired) {
    int index = -1;
    isPaired = IN_PORT_PAIRED_ID_LIST.contains(portId);
    if (isPaired) {
        index = IN_PORT_PAIRED_ID_LIST.indexOf(portId);
    } else {
        index = IN_PORT_ID_LIST.indexOf(portId) + IN_PORT_PAIRED_ID_LIST.size();
    }

    return index;
}

QString SpadesPrompter::composeRichDoc() {
    return tr("Assemble de novo the input data into contigs and scaffolds.");
}

} // LocalWorkflow
} // U2

