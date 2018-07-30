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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
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
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/ActorValidator.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/PairedReadsPortValidator.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "ClarkClassifyWorker.h"
#include "ClarkSupport.h"
#include "../ngs_reads_classification/src/DatabaseDelegate.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace LocalWorkflow {

///////////////////////////////////////////////////////////////
//ClarkClassify

QString ClarkClassifyPrompter::composeRichDoc() {
    const QString databaseUrl = getHyperlink(ClarkClassifyWorkerFactory::DB_URL, getURL(ClarkClassifyWorkerFactory::DB_URL));

    if (getParameter(ClarkClassifyWorkerFactory::SEQUENCING_READS).toString() == ClarkClassifyWorkerFactory::SINGLE_END) {
        const QString readsProducerName = getProducersOrUnset(ClarkClassifyWorkerFactory::INPUT_PORT, ClarkClassifyWorkerFactory::INPUT_SLOT);
        return tr("Classify sequences from <u>%1</u> with CLARK, use %2 database.").arg(readsProducerName).arg(databaseUrl);
    } else {
        const QString pairedReadsProducerName = getProducersOrUnset(ClarkClassifyWorkerFactory::INPUT_PORT, ClarkClassifyWorkerFactory::PAIRED_INPUT_SLOT);
        return tr("Classify paired-end reads from <u>%1</u> with CLARK, use %2 database.")
                .arg(pairedReadsProducerName).arg(databaseUrl);
    }
}

/************************************************************************/
/* DatabaseValidator */
/************************************************************************/

bool ClarkClassifyValidator::validate(const Actor *actor, ProblemList &problemList, const QMap<QString, QString> &) const {
    return validateDatabase(actor, problemList);
}

bool ClarkClassifyValidator::validateDatabase(const Actor *actor, ProblemList &problemList) const {
    const QString databaseUrl = actor->getParameter(ClarkClassifyWorkerFactory::DB_URL)->getAttributeValueWithoutScript<QString>();
    if (!databaseUrl.isEmpty()) {
        const bool doesDatabaseDirExist = QFileInfo(databaseUrl).exists();
        CHECK_EXT(doesDatabaseDirExist,
                  problemList.append(Problem(ClarkClassifyPrompter::tr("The database folder doesn't exist: %1.").arg(databaseUrl), actor->getId())),
                  false);

        const QStringList files = QStringList() << "targets.txt" << ".custom.fileToAccssnTaxID" << ".custom.fileToTaxIDs";

        QStringList missedFiles;
        foreach (const QString &file, files) {
            QString f = databaseUrl + "/" + file;
            if (!QFileInfo(f).exists()) {
                missedFiles << f;
            }
        }

        foreach (const QString &missedFile, missedFiles) {
            problemList.append(Problem(ClarkClassifyPrompter::tr("The mandatory database file doesn't exist: %1.").arg(missedFile), actor->getId()));
        }
        CHECK(missedFiles.isEmpty(), false);
    }

    return true;
}

/************************************************************************/
/* ClarkClassifyWorkerFactory */
/************************************************************************/

const QString ClarkClassifyWorkerFactory::ACTOR_ID = "clark-classify";

const QString ClarkClassifyWorkerFactory::INPUT_PORT = "in";
const QString ClarkClassifyWorkerFactory::PAIRED_INPUT_PORT = "in2";

// Slots should be the same as in GetReadsListWorkerFactory
const QString ClarkClassifyWorkerFactory::INPUT_SLOT = "reads-url1";
const QString ClarkClassifyWorkerFactory::PAIRED_INPUT_SLOT = "reads-url2";

const QString ClarkClassifyWorkerFactory::OUTPUT_PORT = "out";

const QString ClarkClassifyWorkerFactory::TOOL_VARIANT = "tool-variant";
const QString ClarkClassifyWorkerFactory::DB_URL = "database";
const QString ClarkClassifyWorkerFactory::OUTPUT_URL = "output-url";
const QString ClarkClassifyWorkerFactory::TAXONOMY = "taxonomy";
const QString ClarkClassifyWorkerFactory::TAXONOMY_RANK = "taxonomy-rank";
const QString ClarkClassifyWorkerFactory::K_LENGTH = "k-length";
const QString ClarkClassifyWorkerFactory::K_MIN_FREQ = "k-min-freq";
const QString ClarkClassifyWorkerFactory::MODE = "mode";
const QString ClarkClassifyWorkerFactory::FACTOR = "factor";
const QString ClarkClassifyWorkerFactory::GAP = "gap";
const QString ClarkClassifyWorkerFactory::EXTEND_OUT = "extend-out";
const QString ClarkClassifyWorkerFactory::DB_TO_RAM = "preload";
const QString ClarkClassifyWorkerFactory::NUM_THREADS = "threads";
const QString ClarkClassifyWorkerFactory::SEQUENCING_READS = "sequencing-reads";

const QString ClarkClassifyWorkerFactory::SINGLE_END = "single-end";
const QString ClarkClassifyWorkerFactory::PAIRED_END = "paired-end";

const QString ClarkClassifyWorkerFactory::WORKFLOW_CLASSIFY_TOOL_CLARK = "CLARK";

void ClarkClassifyWorkerFactory::init() {

    Descriptor desc( ACTOR_ID, ClarkClassifyWorker::tr("Classify Sequences with CLARK"),
        ClarkClassifyWorker::tr("CLARK (CLAssifier based on Reduced K-mers) is a tool for supervised sequence "
                                "classification based on discriminative k-mers. UGENE provides the GUI for CLARK and CLARK-l "
                                "variants of the CLARK framework for solving the problem of the assignment of metagenomic reads to known genomes.") );

    QList<PortDescriptor*> p;
    {
        Descriptor inD(INPUT_PORT, ClarkClassifyWorker::tr("Input sequences"), ClarkClassifyWorker::tr("URL(s) to FASTQ or FASTA file(s) should be provided.\n\n"
                                                                                                       "In case of SE reads or contigs use the \"Input URL 1\" slot only.\n\n"
                                                                                                       "In case of PE reads input \"left\" reads to \"Input URL 1\", \"right\" reads to \"Input URL 2\".\n\n"
                                                                                                       "See also the \"Input data\" parameter of the element."));
        Descriptor outD(OUTPUT_PORT, ClarkClassifyWorker::tr("CLARK Classification"), ClarkClassifyWorker::tr("A map of sequence names with the associated taxonomy IDs, classified by CLARK."));

        Descriptor inSlot1Descriptor(INPUT_SLOT, ClarkClassifyWorker::tr("Input URL 1"), ClarkClassifyWorker::tr("Input URL 1."));
        Descriptor inSlot2Descriptor(PAIRED_INPUT_SLOT, ClarkClassifyWorker::tr("Input URL 2"), ClarkClassifyWorker::tr("Input URL 2."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[inSlot1Descriptor] = BaseTypes::STRING_TYPE();
        inM[inSlot2Descriptor] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(inD, DataTypePtr(new MapDataType("clark.input", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT()] = TaxonomySupport::TAXONOMY_CLASSIFICATION_TYPE();
        p << new PortDescriptor(outD, DataTypePtr(new MapDataType("clark.output", outM)), false, true);
    }

    QList<Attribute*> a;
    {
        Descriptor tool(TOOL_VARIANT, ClarkClassifyWorker::tr("Classification tool"),
            ClarkClassifyWorker::tr("Use CLARK-l on workstations with limited memory (i.e., \"l\" for light), this software tool provides precise classification on small metagenomes. It works with a sparse or ''light'' database (up to 4 GB of RAM) while still performing ultra accurate and fast results.<br><br>"
                                    "Use CLARK on powerful workstations, it requires a significant amount of RAM to run with large database (e.g. all bacterial genomes from NCBI/RefSeq)."));

        Descriptor dbUrl(DB_URL, ClarkClassifyWorker::tr("Database"),
            ClarkClassifyWorker::tr("A path to the folder with the CLARK database files (-D).<br><br>"
                                    "It is assumed that \"targets.txt\" file is located in this folder (the file is passed to the \"classify_metagenome.sh\" script from the CLARK package via parameter -T)."));

        Descriptor outputUrl(OUTPUT_URL, ClarkClassifyWorker::tr("Output file"),
                         ClarkClassifyWorker::tr("Specify the output file name."));

//        Descriptor taxonomy(TAXONOMY, ClarkClassifyWorker::tr("Taxonomy"),
//            ClarkClassifyWorker::tr("A set of files that define the taxonomic name and tree information, and the GI number to taxon map."
//                                    "<br>The NCBI taxonomy is used by default."));

//        Descriptor rank(TAXONOMY_RANK, ClarkClassifyWorker::tr("Taxonomy rank"),
//            ClarkClassifyWorker::tr("All input sequences are classified against all taxa from the selected database, which are defined all at the same taxonomy level (--species, --genus, --family, --order, --class, --phylum)."));

        Descriptor kLength(K_LENGTH, ClarkClassifyWorker::tr("K-mer length"),
            ClarkClassifyWorker::tr("Set the k-mer length (-k).<br><br>"
                                    "This value is critical for the classification accuracy and speed.<br><br>"
                                    "For high sensitivity, it is recommended to set this value to 20 or 21 (along with the \"Full\" mode).<br><br>"
                                    "However, if the precision and the speed are the main concern, use any value between 26 and 32.<br><br>"
                                    "Note that the higher the value, the higher is the RAM usage. So, as a good tradeoff between speed, precision, and RAM usage, it is recommended to set this value to 31 (along with the \"Default\" or \"Express\" mode)."));


        Descriptor kMinFreq(K_MIN_FREQ, ClarkClassifyWorker::tr("Minimum k-mer frequency"),
            ClarkClassifyWorker::tr("Minimum of k-mer frequency/occurrence for the discriminative k-mers (-t).<br><br>"
                                    "For example, for 1 (or, 2), the program will discard any discriminative k-mer that appear only once (or, less than twice)."));

        Descriptor mode(MODE, ClarkClassifyWorker::tr("Mode"),
            ClarkClassifyWorker::tr("Set the mode of the execution (-m):<ul>"
                                    "<li>\"Full\" to get detailed results, confidence scores and other statistics."
                                    "<li>\"Default\" to get results summary and perform best trade-off between classification speed, accuracy and RAM usage."
                                    "<li>\"Express\" to get results summary with the highest speed possible."
                                    "</ul>"));

        Descriptor factor(FACTOR, ClarkClassifyWorker::tr("Sampling factor value"),
            ClarkClassifyWorker::tr("Sample factor value (-s).<br><br>"
                                    "To load in memory half the discriminative k-mers set this value to 2. To load a third of these k-mers set it to 3.<br><br>"
                                    "The higher the factor is, the lower the RAM usage is and the higher the classification speed/precision is. However, the sensitivity can be quickly degraded, especially for values higher than 3."));

        Descriptor gap(GAP, ClarkClassifyWorker::tr("Gap"),
            ClarkClassifyWorker::tr("\"Gap\" or number of non-overlapping k-mers to pass when creating the database (-п).<br><br>"
                                    "Increase the value if it is required to reduce the RAM usage. Note that this will degrade the sensitivity."));

        Descriptor extendedOutput(EXTEND_OUT, ClarkClassifyWorker::tr("Extended output"),
            ClarkClassifyWorker::tr("Request an extended output for the result file (--extended)."));

        Descriptor db2ram(DB_TO_RAM, ClarkClassifyWorker::tr("Load database into memory"),
            ClarkClassifyWorker::tr("Request the loading of database file by memory mapped-file (--ldm).<br><br>"
                                    "This option accelerates the loading time but it will require an additional amount of RAM significant. "
                                    "This option also allows to load the database in multithreaded-task (see also the \"Number of threads\" parameter)."));

        Descriptor numThreads(NUM_THREADS, ClarkClassifyWorker::tr("Number of threads"),
            ClarkClassifyWorker::tr("Use multiple threads for the classification and, with the \"Load database into memory\" option enabled, for the loading of the database into RAM (-n)."));

        Descriptor sequencingReadsDesc(SEQUENCING_READS, ClarkClassifyWorker::tr("Input data"),
                                             ClarkClassifyWorker::tr("To classify single-end (SE) reads or contigs, received by reads de novo assembly, set this parameter to \"SE reads or contigs\".<br><br>"
                                                                     "To classify paired-end (PE) reads, set the value to \"PE reads\".<br><br>"
                                                                     "One or two slots of the input port are used depending on the value of the parameter. Pass URL(s) to data to these slots.<br><br>"
                                                                     "The input files should be in FASTA or FASTQ formats."));

        const Descriptor classifyToolDesc(NgsReadsClassificationPlugin::WORKFLOW_CLASSIFY_TOOL_ID,
                                          WORKFLOW_CLASSIFY_TOOL_CLARK,
                                          "Classify tool. Hidden attribute");

        Attribute *sequencingReadsAttribute = new Attribute(sequencingReadsDesc, BaseTypes::STRING_TYPE(), Attribute::None, SINGLE_END);
        sequencingReadsAttribute->addSlotRelation(SlotRelationDescriptor(INPUT_PORT, PAIRED_INPUT_SLOT, QVariantList() << PAIRED_END));
        a << sequencingReadsAttribute;
        a << new Attribute(tool, BaseTypes::STRING_TYPE(), Attribute::None, ClarkClassifySettings::TOOL_LIGHT);

        QString clarkDatabasePath;
        U2DataPath *clarkBacteriaViralDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::CLARK_BACTERIAL_VIRAL_DATABASE_DATA_ID);
        if (NULL != clarkBacteriaViralDataPath && clarkBacteriaViralDataPath->isValid()) {
            clarkDatabasePath = clarkBacteriaViralDataPath->getPathByName(NgsReadsClassificationPlugin::CLARK_BACTERIAL_VIRAL_DATABASE_ITEM_ID);
        } else {
            U2DataPath *clarkViralDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::CLARK_VIRAL_DATABASE_DATA_ID);
            if (NULL != clarkViralDataPath && clarkViralDataPath->isValid()) {
                clarkDatabasePath = clarkViralDataPath->getPathByName(NgsReadsClassificationPlugin::CLARK_VIRAL_DATABASE_ITEM_ID);
            }
        }
        a << new Attribute(dbUrl, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding, clarkDatabasePath);

//        a << new Attribute( taxonomy, BaseTypes::STRING_TYPE(), false, "Default");
//        a << new Attribute( rank, BaseTypes::NUM_TYPE(), false, ClarkClassifySettings::Species);

        Attribute *klenAttr = new Attribute(kLength, BaseTypes::NUM_TYPE(), Attribute::None, 31);
        klenAttr->addRelation(new VisibilityRelation(TOOL_VARIANT, QVariant(ClarkClassifySettings::TOOL_DEFAULT)));
        a << klenAttr;

        a << new Attribute(kMinFreq, BaseTypes::NUM_TYPE(), Attribute::None, 0);
        a << new Attribute(mode, BaseTypes::NUM_TYPE(), Attribute::None, ClarkClassifySettings::Default);

        Attribute *extAttr = new Attribute(extendedOutput, BaseTypes::BOOL_TYPE(), Attribute::None, false);
        extAttr->addRelation(new VisibilityRelation(MODE, QVariant(ClarkClassifySettings::Full)));
        a << extAttr;

        Attribute *factorAttr = new Attribute(factor, BaseTypes::NUM_TYPE(), Attribute::None, 2);
        factorAttr->addRelation(new VisibilityRelation(TOOL_VARIANT, QVariant(ClarkClassifySettings::TOOL_DEFAULT)));
        a << factorAttr;

        Attribute *gapAttr = new Attribute(gap, BaseTypes::NUM_TYPE(), Attribute::None, 4);
        gapAttr->addRelation(new VisibilityRelation(TOOL_VARIANT, QVariant(ClarkClassifySettings::TOOL_LIGHT)));
        a << gapAttr;

        //a << new Attribute( outFile, BaseTypes::STRING_TYPE(), false, "results.csv");

        a << new Attribute(db2ram, BaseTypes::BOOL_TYPE(), Attribute::None, false);
        a << new Attribute(numThreads, BaseTypes::NUM_TYPE(), Attribute::None, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
        a << new Attribute(outputUrl, BaseTypes::STRING_TYPE(), Attribute::Required | Attribute::NeedValidateEncoding | Attribute::CanBeEmpty);

        a << new Attribute(classifyToolDesc, BaseTypes::STRING_TYPE(),
                           static_cast<Attribute::Flags>(Attribute::Hidden),
                           WORKFLOW_CLASSIFY_TOOL_CLARK);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap sequencingReadsMap;
        sequencingReadsMap[ClarkClassifyWorker::tr("SE reads or contigs")] = SINGLE_END;
        sequencingReadsMap[ClarkClassifyWorker::tr("PE reads")] = PAIRED_END;
        delegates[SEQUENCING_READS] = new ComboBoxDelegate(sequencingReadsMap);

        QVariantMap toolMap;
        toolMap["CLARK"] = ClarkClassifySettings::TOOL_DEFAULT;
        toolMap["CLARK-l"] = ClarkClassifySettings::TOOL_LIGHT;
        delegates[TOOL_VARIANT] = new ComboBoxDelegate(toolMap);

        DelegateTags outputUrlTags;
        outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
        outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareFileFilter("CSV", QStringList("csv"), false, QStringList()));
        delegates[OUTPUT_URL] = new URLDelegate(outputUrlTags, "clark/output");

        QVariantMap lenMap;
        lenMap["minimum"] = QVariant(2);
        lenMap["maximum"] = QVariant(32);
        delegates[K_LENGTH] = new SpinBoxDelegate(lenMap);

        QVariantMap freqMap;
        freqMap["minimum"] = QVariant(0);
        freqMap["maximum"] = QVariant(65535);
        delegates[K_MIN_FREQ] = new SpinBoxDelegate(freqMap);

        QVariantMap modeMap;
        modeMap["Default"] = ClarkClassifySettings::Default;
        modeMap["Full"] = ClarkClassifySettings::Full;
        modeMap["Express"] = ClarkClassifySettings::Express;
        delegates[MODE] = new ComboBoxDelegate(modeMap);

        QVariantMap factorMap;
        factorMap["minimum"] = QVariant(1);
        factorMap["maximum"] = QVariant(30);
        delegates[FACTOR] = new SpinBoxDelegate(factorMap);

        QVariantMap gapMap;
        gapMap["minimum"] = QVariant(1);
        gapMap["maximum"] = QVariant(49);
        delegates[GAP] = new SpinBoxDelegate(gapMap);

        QVariantMap thrMap;
        thrMap["minimum"] = QVariant(1);
        thrMap["maximum"] = QVariant(AppResourcePool::instance()->getIdealThreadCount());
        delegates[NUM_THREADS] = new SpinBoxDelegate(thrMap);

        QList<StrStrPair> dataPathItems;
        dataPathItems << StrStrPair(NgsReadsClassificationPlugin::CLARK_BACTERIAL_VIRAL_DATABASE_DATA_ID, NgsReadsClassificationPlugin::CLARK_BACTERIAL_VIRAL_DATABASE_ITEM_ID);
        dataPathItems << StrStrPair(NgsReadsClassificationPlugin::CLARK_VIRAL_DATABASE_DATA_ID, NgsReadsClassificationPlugin::CLARK_VIRAL_DATABASE_ITEM_ID);
        delegates[DB_URL] = new DatabaseDelegate(ACTOR_ID, DB_URL, dataPathItems, "clark/database", true);
    }

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ClarkClassifyPrompter());
    proto->setValidator(new ClarkClassifyValidator());
    proto->setPortValidator(ClarkClassifyWorkerFactory::INPUT_PORT, new PairedReadsPortValidator(INPUT_SLOT, PAIRED_INPUT_SLOT));
    proto->addExternalTool(ET_CLARK);
    proto->addExternalTool(ET_CLARK_L);

    WorkflowEnv::getProtoRegistry()->registerProto(NgsReadsClassificationPlugin::WORKFLOW_ELEMENTS_GROUP, proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ClarkClassifyWorkerFactory());
}

void ClarkClassifyWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR_ID);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR_ID);
}

/************************************************************************/
/* ClarkClassifyWorker */
/************************************************************************/
ClarkClassifyWorker::ClarkClassifyWorker(Actor *a)
:BaseWorker(a, false), input(NULL), output(NULL), paired(false)
{
}

void ClarkClassifyWorker::init() {
    paired = (getValue<QString>(ClarkClassifyWorkerFactory::SEQUENCING_READS) == ClarkClassifyWorkerFactory::PAIRED_END);

    input = ports.value(/*paired ? PAIRED_INPUT_PORT :*/ ClarkClassifyWorkerFactory::INPUT_PORT);
    output = ports.value(ClarkClassifyWorkerFactory::OUTPUT_PORT);

    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL").arg(ClarkClassifyWorkerFactory::INPUT_PORT), );
    SAFE_POINT(NULL != output, QString("Port with id '%1' is NULL").arg(ClarkClassifyWorkerFactory::OUTPUT_PORT), );

    output->addComplement(input);
    input->addComplement(output);

    cfg.tool = getValue<QString>(ClarkClassifyWorkerFactory::TOOL_VARIANT).toLower();
    cfg.databaseUrl = getValue<QString>(ClarkClassifyWorkerFactory::DB_URL);
    cfg.numberOfThreads = getValue<int>(ClarkClassifyWorkerFactory::NUM_THREADS);
    cfg.preloadDatabase = getValue<bool>(ClarkClassifyWorkerFactory::DB_TO_RAM);
    cfg.minFreqTarget = getValue<int>(ClarkClassifyWorkerFactory::K_MIN_FREQ);
    if (cfg.tool == ClarkClassifySettings::TOOL_DEFAULT.toLower()) {
        cfg.kmerSize = getValue<int>(ClarkClassifyWorkerFactory::K_LENGTH);
        cfg.factor = getValue<int>(ClarkClassifyWorkerFactory::FACTOR);
    } else {
        cfg.gap = getValue<int>(ClarkClassifyWorkerFactory::GAP);
    }
    cfg.extOut = getValue<bool>(ClarkClassifyWorkerFactory::EXTEND_OUT);

    cfg.mode = (U2::LocalWorkflow::ClarkClassifySettings::Mode)getValue<int>(ClarkClassifyWorkerFactory::MODE);
    if (!(cfg.mode >=ClarkClassifySettings::Full && cfg.mode <= ClarkClassifySettings::Spectrum)) {
        reportError(tr("Unrecognized mode of execution, expected any of: 0 (full), 1 (default), 2 (express) or 3 (spectrum)"));
    }
}

bool ClarkClassifyWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    const int hasMessage1 = input->hasMessage();
    const bool ended1 = input->isEnded();
    //if (!paired)
    {
        return hasMessage1 || ended1;
    }
/*
    const int hasMessage2 = pairedInput->hasMessage();
    const bool ended2 = pairedInput->isEnded();

    if (hasMessage1 && hasMessage2) {
        return true;
    } else if (hasMessage1) {
        return ended2;
    } else if (hasMessage2) {
        return ended1;
    }

    return ended1 && ended2;*/
}

Task * ClarkClassifyWorker::tick() {
    if (input->hasMessage() /*&& (!paired || pairedInput->hasMessage())*/) {
        const Message message = getMessageAndSetupScriptValues(input);

        QString readsUrl = message.getData().toMap()[ClarkClassifyWorkerFactory::INPUT_SLOT].toString();
        QString pairedReadsUrl;

        U2OpStatusImpl os;
        QString tmpDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
        tmpDir = GUrlUtils::createDirectory(tmpDir + "clark", "_", os);
        CHECK_OP(os, new FailTask(os.getError()));

        QString reportUrl = getValue<QString>(ClarkClassifyWorkerFactory::OUTPUT_URL);
        if (reportUrl.isEmpty()) {
            const MessageMetadata metadata = context->getMetadataStorage().get(message.getMetadataId());
            reportUrl = tmpDir + "/" + NgsReadsClassificationUtils::getClassificationFileName(metadata.getFileUrl(), "CLARK", "csv", paired);
        }
        reportUrl = GUrlUtils::ensureFileExt(reportUrl, QStringList("csv")).getURLString();
        reportUrl = GUrlUtils::rollFileName(reportUrl, "_");

        if (paired) {
//            const Message pairedMessage = getMessageAndSetupScriptValues(pairedInput);
            pairedReadsUrl = message.getData().toMap()[ClarkClassifyWorkerFactory::PAIRED_INPUT_SLOT].toString();
        }
        //TODO uncompress input files if needed

        ClarkClassifyTask *task = new ClarkClassifyTask(cfg, readsUrl, pairedReadsUrl, reportUrl);
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    }

    if (input->isEnded()/* || (paired && pairedInput->isEnded())*/) {
        setDone();
        algoLog.info("CLARK worker is done as input has ended");
        output->setEnded();
    }

//    if (paired) {
//        QString error;
//        if (input->isEnded() && (!pairedInput->isEnded() || pairedInput->hasMessage())) {
//            error = tr("Not enough downstream reads datasets");
//        }
//        if (pairedInput->isEnded() && (!input->isEnded() || input->hasMessage())) {
//            error = tr("Not enough upstream reads datasets");
//        }

//        if (!error.isEmpty()) {
//            return new FailTask(error);
//        }
//    }

    return NULL;
}

void ClarkClassifyWorker::sl_taskFinished(Task *t) {
    ClarkClassifyTask *task = qobject_cast<ClarkClassifyTask *>(t);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (!task->isFinished() || task->hasError() || task->isCanceled()) {
        return;
    }

    const QString rawClassificationUrl = task->getReportUrl();
    algoLog.details(QString("CLARK produced classification: %1").arg(rawClassificationUrl));

    QVariantMap data;
    data[TaxonomySupport::TAXONOMY_CLASSIFICATION_SLOT_ID] = QVariant::fromValue<U2::LocalWorkflow::TaxonomyClassificationResult>(parseReport(rawClassificationUrl));
    output->put(Message(output->getBusType(), data));
    context->getMonitor()->addOutputFile(rawClassificationUrl, getActor()->getId());
}

void ClarkClassifyWorker::cleanup() {

}

//also see in clark: getObjectsDataComputeFast, getObjectsDataCompute, getObjectsDataComputeFastLight,getObjectsDataComputeFastSpaced, getObjectsDataComputeFull, printExtendedResults
static const QByteArray DEFAULT_REPORT("Object_ID, Length, Assignment");
static const QByteArray REPORT_PREFIX("Object_ID,");
static const QByteArray EXTENDED_REPORT_SUFFIX(",Length,Gamma,1st_assignment,score1,2nd_assignment,score2,confidence");

TaxonomyClassificationResult ClarkClassifyWorker::parseReport(const QString &url)
{
    TaxonomyClassificationResult result;
    QFile reportFile(url);
    if (!reportFile.open(QIODevice::ReadOnly)) {
        reportError(tr("Cannot open classification report: %1").arg(url));
    } else {
        QByteArray line = reportFile.readLine().trimmed();

        bool extended = line.endsWith(EXTENDED_REPORT_SUFFIX);
        if (!line.startsWith(REPORT_PREFIX)) {
            reportError(tr("Failed to recognize CLARK report format: %1").arg(QString(line)));
        } else {
            while ((line = reportFile.readLine().trimmed()).size() != 0) {
                QList<QByteArray> row = line.split(',');
                if (extended ? row.size() < 6 : row.size() != 3) {
                    reportError(tr("Broken CLARK report: %1").arg(url));
                    break;
                }
                int assignmentIdx = extended ? row.size() - 5 : 2;
                QString objID = row[0];
                QByteArray &assStr = row[assignmentIdx];
                algoLog.trace(QString("Found CLARK classification: %1=%2").arg(objID).arg(QString(assStr)));

                bool ok = true;
                TaxID assID = (assStr != "NA") ? assStr.toUInt(&ok) : TaxonomyTree::UNCLASSIFIED_ID;
                if (!ok) {
                    reportError(tr("Broken CLARK report: %1").arg(url));
                    break;
                }
                if (result.contains(objID)) {
                    QString msg = tr("Duplicate sequence name '%1' have been detected in the classification output.").arg(objID);
                    monitor()->addInfo(msg, getActorId(), Problem::U2_WARNING);
                    algoLog.info(msg);
                } else {
                    result[objID] = assID;
                }
            }
        }
        reportFile.close();
    }
    return result;
}

class ClarkLogParser : public ExternalToolLogParser {
public:
    ClarkLogParser() {}

private:
    bool isError(const QString &line) const {
        foreach (const QString &wellKnownError, wellKnownErrors) {
            if (line.contains(wellKnownError)) {
                return true;
            }
        }
        return false;
    }

    static const QStringList wellKnownErrors;
};

const QStringList ClarkLogParser::wellKnownErrors("std::bad_alloc");

ClarkClassifyTask::ClarkClassifyTask(const ClarkClassifySettings &settings, const QString &readsUrl, const QString &pairedReadsUrl, const QString &reportUrl)
    : ExternalToolSupportTask(tr("Classify reads with Clark"), TaskFlags_NR_FOSE_COSC),
      cfg(settings), readsUrl(readsUrl), pairedReadsUrl(pairedReadsUrl), reportUrl(reportUrl)
{
    GCOUNTER(cvar, tvar, "ClarkClassifyTask");

    SAFE_POINT_EXT(!readsUrl.isEmpty(), setError("Reads URL is empty"), );
    SAFE_POINT_EXT(!reportUrl.isEmpty(), setError("Classification report URL is empty"), );
    SAFE_POINT_EXT(!settings.databaseUrl.isEmpty(), setError("Clark database URL is empty"), );

}

void ClarkClassifyTask::prepare() {
    QString toolName = ET_CLARK_L;
    if ( QString::compare(cfg.tool, ClarkClassifySettings::TOOL_DEFAULT, Qt::CaseInsensitive) == 0) {
        toolName = ET_CLARK;
    } else if (QString::compare(cfg.tool, ClarkClassifySettings::TOOL_LIGHT, Qt::CaseInsensitive) != 0) {
        stateInfo.setError(tr("Unsupported CLARK variant. Only default and light variants are supported."));
        return;
    }
    QScopedPointer<ExternalToolRunTask> task(new ExternalToolRunTask(toolName, getArguments(), new ClarkLogParser(), cfg.databaseUrl));
    CHECK_OP(stateInfo, );
    setListenerForTask(task.data());
    addSubTask(task.take());
}

QStringList ClarkClassifyTask::getArguments() {

    QStringList arguments;

    arguments << "-D" << cfg.databaseUrl;
    arguments << "-T" << cfg.databaseUrl + "/targets.txt";

    // CLARK will add "csv" extension unconditinally
    if (!reportUrl.endsWith(".csv")) {
        reportUrl += ".csv";
    }
    arguments << "-R" << QFileInfo(reportUrl).dir().path() + "/" + QFileInfo(reportUrl).completeBaseName();

    if (!pairedReadsUrl.isEmpty()) {
        arguments << "-P" << readsUrl << pairedReadsUrl;
    }
    else {
        arguments << "-O" << readsUrl;
    }

    if (QString::compare(cfg.tool, ClarkClassifySettings::TOOL_DEFAULT, Qt::CaseInsensitive) == 0) {
        arguments << "-s" << QString::number(cfg.factor);
        arguments << "-k" << QString::number(cfg.kmerSize);
    } else if (QString::compare(cfg.tool, ClarkClassifySettings::TOOL_LIGHT, Qt::CaseInsensitive) == 0) {
        arguments << "-g" << QString::number(cfg.gap);
    }

    arguments << "-t" << QString::number(cfg.minFreqTarget);
    arguments << "-m" << QString::number(cfg.mode);
    arguments << "-n" << QString::number(cfg.numberOfThreads);

    if (cfg.preloadDatabase) {
        arguments << "--ldm";
    }
    if (cfg.extOut) {
        arguments << "--extended";
    }

    return arguments;
}

ClarkClassifySettings::ClarkClassifySettings()
    : tool(ClarkClassifySettings::TOOL_LIGHT), gap(4), factor(2), minFreqTarget(0), kmerSize(31), numberOfThreads(1),
      extOut(false), preloadDatabase(false), mode(ClarkClassifySettings::Default)
{
}

const QString ClarkClassifySettings::TOOL_DEFAULT("default");
const QString ClarkClassifySettings::TOOL_LIGHT("light");

} //LocalWorkflow
} //U2
