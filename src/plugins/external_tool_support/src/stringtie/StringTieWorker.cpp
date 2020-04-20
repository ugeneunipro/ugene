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

#include "StringTieWorker.h"
#include "StringTieSupport.h"
#include "StringTieTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/AttributeRelation.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowSettings.h>

#include <QThread>


namespace U2 {
namespace LocalWorkflow {

namespace {
    const QString STRINGTIE_DIR = "stringtie";

    // input
    const QString IN_PORT_ID("in");
    const QString IN_URL_SLOT_ID("url");

    // output
    const QString OUT_PORT_ID("out");
    const QString TRANSCRIPT_OUT_SLOT_ID("out-transcripts");
    const QString GENE_ABUND_OUT_SLOT_ID("out-gene-abundance");

    // parameters
    const QString REFERENCE_ANNOTATIONS("reference-annotations");
    const QString READS_ORIENTATION("reads-orientation");
    const QString LABEL("label");
    const QString MIN_ISOFORM_FRACTION("min-isoform-fraction");
    const QString MIN_TRANSCRIPT_LEN("min-transcript-length");

    const QString MIN_ANCHOR_LEN("min-anchor-length");
    const QString MIN_JUNCTION_COVERAGE("min-junction-coverage");
    const QString TRIM_TRANSCRIPT("trim-transcripts");
    const QString MIN_COVERAGE("min-coverage");
    const QString MIN_LOCUS_SEPARATION("min-locus-gap");

    const QString MULTI_HIT_FRACTION("multi-hit-fraction");
    const QString SKIP_SEQUENCES("skip-sequences");
    const QString REF_ONLY_ABUNDANCE("ref-only-abundance");
    const QString MULTI_MAPPING_CORRECTION("multi-mapping-correction");
    const QString VERBOSE_LOG("verbose-log");

    const QString THREAD_NUM("threads");

    const QString PRIMARY_OUTPUT("transcripts-output-url");
    const QString GENE_ABUDANCE_OUTPUT("gene-abundance-output");
    const QString GENE_ABUDANCE_OUTPUT_FILE("gene-abundance-output-url");
    const QString COVERAGE_REF_OUTPUT("covered-transcripts-output");
    const QString COVERAGE_REF_OUTPUT_FILE("covered-transcripts-output-url");
    const QString BALLGOWN_OUTPUT("ballgown-output");
    const QString BALLGOWN_OUTPUT_FOLDER("ballgown-output-url");
}

const QString StringTieWorkerFactory::ACTOR_ID("stringtie");

/************************************************************************/
/* Worker */
/************************************************************************/
StringTieWorker::StringTieWorker(Actor *p)
    : BaseWorker(p),
      inputPort(NULL),
      outputPort(NULL) {

}

void StringTieWorker::init() {
    inputPort = ports.value(IN_PORT_ID);
    outputPort = ports.value(OUT_PORT_ID);
}

Task* StringTieWorker::tick() {
    if (inputPort->hasMessage()) {
        const Message message = getMessageAndSetupScriptValues(inputPort);
        QVariantMap data = message.getData().toMap();

        U2OpStatus2Log os;
        StringTieTaskSettings settings = getSettings(os, data[IN_URL_SLOT_ID].toString());
        if (os.hasError()) {
            return new FailTask(os.getError());
        }

        StringTieTask* task = new StringTieTask(settings);
        task->addListeners(createLogListeners());
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));

        return task;
    } else if (inputPort->isEnded()) {
        setDone();
        outputPort->setEnded();
    }
    return NULL;
}

void StringTieWorker::cleanup() {

}

void StringTieWorker::sl_taskFinished() {
    StringTieTask *t = qobject_cast<StringTieTask*>(sender());
    if (!t->isFinished() || t->hasError() || t->isCanceled()) {
        return;
    }

    QString outputPrimary = t->getSettings().primaryOutputFile;

    QVariantMap data;
    data[TRANSCRIPT_OUT_SLOT_ID] = outputPrimary;
    context->getMonitor()->addOutputFile(outputPrimary, getActor()->getId());

    if (t->getSettings().geneAbundanceOutput) {
        data[GENE_ABUND_OUT_SLOT_ID] = qVariantFromValue<QString>(t->getSettings().geneAbundanceOutputFile);
        context->getMonitor()->addOutputFile(t->getSettings().geneAbundanceOutputFile, getActor()->getId());
    }
    outputPort->put(Message(outputPort->getBusType(), data));
}

StringTieTaskSettings StringTieWorker::getSettings(U2OpStatus &os, const QString& inputFile) {
    StringTieTaskSettings settings;
    settings.inputBam = inputFile;

    settings.referenceAnnotations = getValue<QString>(REFERENCE_ANNOTATIONS);
    settings.readOrientation = getValue<QString>(READS_ORIENTATION);
    settings.label = getValue<QString>(LABEL);
    settings.minIsoformFraction = getValue<double>(MIN_ISOFORM_FRACTION);
    settings.minTransciptLen = getValue<int>(MIN_TRANSCRIPT_LEN);

    settings.minAnchorLen = getValue<int>(MIN_ANCHOR_LEN);
    settings.minJunctionCoverage = getValue<double>(MIN_JUNCTION_COVERAGE);
    settings.trimTranscript = getValue<bool>(TRIM_TRANSCRIPT);
    settings.minCoverage = getValue<double>(MIN_COVERAGE);
    settings.minLocusSeparation = getValue<int>(MIN_LOCUS_SEPARATION);

    settings.multiHitFraction = getValue<double>(MULTI_HIT_FRACTION);
    settings.skipSequences = getValue<QString>(SKIP_SEQUENCES);
    settings.refOnlyAbudance = getValue<bool>(REF_ONLY_ABUNDANCE);
    settings.multiMappingCorrection = getValue<bool>(MULTI_MAPPING_CORRECTION);
    settings.verboseLog = getValue<bool>(VERBOSE_LOG);

    settings.threadNum = getValue<int>(THREAD_NUM);

    QString workingDir = FileAndDirectoryUtils::createWorkingDir(context->workingDir(), FileAndDirectoryUtils::WORKFLOW_INTERNAL, "", context->workingDir());
    workingDir = GUrlUtils::createDirectory(workingDir + STRINGTIE_DIR, "_", os);

    settings.primaryOutputFile = getValue<QString>(PRIMARY_OUTPUT);
    if (settings.primaryOutputFile.isEmpty()) {
        QFileInfo src(inputFile);
        settings.primaryOutputFile = workingDir + "/" + src.baseName() + "_transcripts.gtf";
    }
    settings.primaryOutputFile = GUrlUtils::rollFileName(settings.primaryOutputFile, "_");

    settings.geneAbundanceOutput = getValue<bool>(GENE_ABUDANCE_OUTPUT);
    settings.geneAbundanceOutputFile = getValue<QString>(GENE_ABUDANCE_OUTPUT_FILE);
    if (settings.geneAbundanceOutput && settings.geneAbundanceOutputFile.isEmpty()) {
        QFileInfo src(inputFile);
        settings.geneAbundanceOutputFile = workingDir + "/" + src.baseName() + "_gene_abund.tab";
    }
    settings.geneAbundanceOutputFile = GUrlUtils::rollFileName(settings.geneAbundanceOutputFile, "_");

    settings.coveredRefOutput = getValue<bool>(COVERAGE_REF_OUTPUT);
    settings.coveredRefOutputFile = getValue<QString>(COVERAGE_REF_OUTPUT_FILE);
    if (settings.coveredRefOutput && settings.coveredRefOutputFile.isEmpty()) {
        QFileInfo src(inputFile);
        settings.coveredRefOutputFile = workingDir + "/" + src.baseName() + "_cov_refs.gtf";
    }
    settings.coveredRefOutputFile = GUrlUtils::rollFileName(settings.coveredRefOutputFile, "_");

    settings.ballgownOutput = getValue<bool>(BALLGOWN_OUTPUT);
    settings.ballgowmOutputFolder = getValue<QString>(BALLGOWN_OUTPUT_FOLDER);
    if (settings.ballgownOutput && settings.ballgowmOutputFolder.isEmpty()) {
        settings.ballgowmOutputFolder = workingDir + "/" + "ballgown_input";
    }
    settings.ballgowmOutputFolder = GUrlUtils::rollFileName(settings.ballgowmOutputFolder, "_");

    return settings;
}


/************************************************************************/
/* Prompter */
/************************************************************************/
StringTiePrompter::StringTiePrompter(Actor* p) : PrompterBase<StringTiePrompter>(p) {
}

QString StringTiePrompter::composeRichDoc() {
    QString doc = StringTieWorker::tr("Uses a BAM file with RNA-Seq read mappings to assemble transcripts.");
    return doc;
}


/************************************************************************/
/* Factory */
/************************************************************************/
void StringTieWorkerFactory::init() {
    QList<PortDescriptor*> ports;
    {
        QMap<Descriptor, DataTypePtr> inputMap;
        Descriptor inSlotDesc(IN_URL_SLOT_ID,
            StringTieWorker::tr("Input URL"),
            StringTieWorker::tr("URL(s) of input file(s) in FASTA format with DNA sequences that need to be assembled"));
        inputMap[inSlotDesc] = BaseTypes::STRING_TYPE();

        QMap<Descriptor, DataTypePtr> outputMap;
        Descriptor outTranscriptsDescr(TRANSCRIPT_OUT_SLOT_ID,
                                       StringTieWorker::tr("Output URL Transcripts"),
                                       StringTieWorker::tr("Output URL Transcripts."));
        Descriptor outGeneAbundDescr(GENE_ABUND_OUT_SLOT_ID,
                                  StringTieWorker::tr("Output URL Gene Abundance"),
                                  StringTieWorker::tr("Output URL Gene Abundance."));
        outputMap[outTranscriptsDescr] = BaseTypes::STRING_TYPE();
        outputMap[outGeneAbundDescr] = BaseTypes::STRING_TYPE();

        Descriptor inPortDesc(IN_PORT_ID,
                              StringTieWorker::tr("Input BAM file(s)"),
                              StringTieWorker::tr("URL(s) to sorted BAM file(s) with RNA-Seq read mappings. "
                                                  "Note that every spliced read alignment (i.e. an alignment across at least one junction) "
                                                  "in the input file must contain the tag XS to indicate the genomic strand "
                                                  "that produced the RNA from which the read was sequenced. "
                                                  "Alignments produced by TopHat and HISAT2 (when run with --dta option) already include this tag, "
                                                  "but if you use a different read mapper you should check that this XS tag is included for spliced alignments."));

        Descriptor outPortDesc(OUT_PORT_ID, StringTieWorker::tr("StringTie output data"),
                           StringTieWorker::tr("For each input BAM file the port outputs an URL to a GTF file with assembled transcripts, produced by StringTie. "
                                               "If \"Report gene abundance\" is \"True\", the port also output an URL to a text file with gene abundances (in a tab-delimited format)."));

        ports << new PortDescriptor(inPortDesc,
                                    DataTypePtr(new MapDataType(IN_PORT_ID, inputMap)),
                                    true /* input */);
        ports << new PortDescriptor(outPortDesc,
                                    DataTypePtr(new MapDataType(OUT_PORT_ID, outputMap)),
                                    false /* input */);

    }

    QList<Attribute*> attributes;
    {
        Descriptor refAnnotations(REFERENCE_ANNOTATIONS,
                                  StringTieWorker::tr("Reference annotations"),
                                  StringTieWorker::tr("Use the reference annotation file (in GTF or GFF3 format) to guide the assembly process (-G). "
                                                      "The output will include expressed reference transcripts as well as any novel transcripts that are assembled."));

        Descriptor readsOrientation(READS_ORIENTATION,
                                  StringTieWorker::tr("Reads orientation"),
                                  StringTieWorker::tr("Select the NGS libraries type: unstranded, stranded fr-secondstrand (--fr), or stranded fr-firststand (--rf)."));

        Descriptor label(LABEL,
                         StringTieWorker::tr("Label"),
                         StringTieWorker::tr("Use the specified string as the prefix for the name of the output transcripts (-l)."));

        Descriptor minIsoformFraction(MIN_ISOFORM_FRACTION,
                              StringTieWorker::tr("Min isoform fraction"),
                              StringTieWorker::tr("Specify the minimum isoform abundance of the predicted transcripts as a fraction "
                                                  "of the most abundant transcript assembled at a given locus (-f). "
                                                  "Lower abundance transcripts are often artifacts of incompletely spliced precursors of processed transcripts."));

        Descriptor minTranscriptLen(MIN_TRANSCRIPT_LEN,
                                   StringTieWorker::tr("Min assembled transcript length"),
                                   StringTieWorker::tr("Specify the minimum length for the predicted transcripts (-m)."));

        Descriptor minAnchorLen(MIN_ANCHOR_LEN,
                                StringTieWorker::tr("Min anchor length for junctions"),
                                StringTieWorker::tr("Junctions that don't have spliced reads that align across them with "
                                                    "at least this amount of bases on both sides are filtered out (-a)."));

        Descriptor minJunctionCoverage(MIN_JUNCTION_COVERAGE,
                                       StringTieWorker::tr("Min junction coverage"),
                                       StringTieWorker::tr("There should be at least this many spliced reads that align across a junction (-j). "
                                                           "This number can be fractional, since some reads align in more than one place. "
                                                           "A read that aligns in n places will contribute 1/n to the junction coverage."));

        Descriptor trimTranscript(TRIM_TRANSCRIPT,
                                  StringTieWorker::tr("Trim transcripts based on coverage"),
                                  StringTieWorker::tr("By default StringTie adjusts the predicted transcript's start and/or stop "
                                                      "coordinates based on sudden drops in coverage of the assembled transcript. "
                                                      "Set this parameter to \"False\" to disable the trimming at the ends of the assembled transcripts (-t)."));

        Descriptor minCoverage(MIN_COVERAGE,
                               StringTieWorker::tr("Min coverage for assembled transcripts"),
                               StringTieWorker::tr("Specify the minimum read coverage allowed for the predicted transcripts (-c). "
                                                   "A transcript with a lower coverage than this value is not shown in the output. "
                                                   "This number can be fractional, since some reads align in more than one place. "
                                                   "A read that aligns in n places will contribute 1/n to the coverage."));

        Descriptor minLocusSeparation(MIN_LOCUS_SEPARATION,
                                      StringTieWorker::tr("Min locus gap separation"),
                                      StringTieWorker::tr("Reads that are mapped closer than this distance are merged together in the same processing bundle (-g)."));

        Descriptor multiHitFraction(MULTI_HIT_FRACTION,
                                    StringTieWorker::tr("Fraction covered by multi-hit reads"),
                                    StringTieWorker::tr("Specify the maximum fraction of muliple-location-mapped reads that are allowed to be present at a given locus (-M). "
                                                        "A read that aligns in n places will contribute 1/n to the coverage."));

        Descriptor skipSequences(SKIP_SEQUENCES,
                                 StringTieWorker::tr("Skip assembling for sequences"),
                                 StringTieWorker::tr("Ignore all read alignments (and thus do not attempt to perform transcript assembly) "
                                                     "on the specified reference sequences (-x). The value can be a single reference sequence name (e.g. \"chrM\") "
                                                     "or a comma-delimited list of sequence names (e.g. \"chrM,chrX,chrY\"). "
                                                     "This can speed up StringTie especially in the case of excluding the mitochondrial genome, whose genes may have "
                                                     "very high coverage in some cases, even though they may be of no interest for a particular RNA-Seq analysis. "
                                                     "The reference sequence names are case sensitive, they must match identically the names of chromosomes/contigs "
                                                     "of the target genome against which the RNA-Seq reads were aligned in the first place."));

        Descriptor refOnlyAbudance(REF_ONLY_ABUNDANCE,
                                   StringTieWorker::tr("Abundance for reference transcripts only"),
                                   StringTieWorker::tr("Limits the processing of read alignments to only estimate and output the assembled transcripts matching the reference transcripts (-e). "
                                                       "With this option, read bundles with no reference transcripts will be entirely skipped, "
                                                       "which may provide a considerable speed boost when the given set of reference transcripts is limited to a set of target genes, for example. "
                                                       "The parameter is only available if the \"Reference annotations\" file is specified."
                                                       " It is recommended to use it when Ballgown table files are produced."));

        Descriptor multiMappingCorrection(MULTI_MAPPING_CORRECTION,
                                      StringTieWorker::tr("Multi-mapping correction"),
                                      StringTieWorker::tr("Enables or disables (-u) multi-mapping correction."));

        Descriptor verboseLog(VERBOSE_LOG,
                                      StringTieWorker::tr("Verbose log"),
                                      StringTieWorker::tr("Enable detailed logging, if required (-v). "
                                                          "The messages will be written to the UGENE log (enabling of \"DETAILS\" and \"TRACE\" logging may be required) "
                                                          "and to the dashboard."));

        Descriptor threadNum(THREAD_NUM,
                             StringTieWorker::tr("Number of threads"),
                             StringTieWorker::tr("Specify the number of processing threads (CPUs) to use for transcript assembly (-p)."));

        Descriptor primaryOutput(PRIMARY_OUTPUT,
                             StringTieWorker::tr("Output transcripts file"),
                             StringTieWorker::tr("StringTie's primary output GTF file with assembled transcripts."));

        Descriptor geneAbudanceOutput(GENE_ABUDANCE_OUTPUT,
                             StringTieWorker::tr("Enable gene abundance output"),
                             StringTieWorker::tr("Select \"True\" to generate gene abundances output (-A). "
                                                 "The output is written to a tab-delimited text file. "
                                                 "Also, the file URL is passed to an output slot of the workflow element."));

        Descriptor geneAbudanceOutputFile(GENE_ABUDANCE_OUTPUT_FILE,
                             StringTieWorker::tr("Output gene abundances file"),
                             StringTieWorker::tr("Specify the name of the output file with gene abundances (-A)."));

        Descriptor coverageRefOutput(COVERAGE_REF_OUTPUT,
                             StringTieWorker::tr("Enable covered reference transcripts output"),
                             StringTieWorker::tr("Select \"True\" to generate a file with reference transcripts that are fully covered by reads (-C). "
                                                 "Thus, the parameter is only available if the \"Reference annotations\" file is specified."));

        Descriptor coverageRefOutputFile(COVERAGE_REF_OUTPUT_FILE,
                             StringTieWorker::tr("Output covered reference transcripts file"),
                             StringTieWorker::tr("Specify the name of the output file with reference transcripts that are fully covered by reads (-C)."));

        Descriptor ballgownOutput(BALLGOWN_OUTPUT,
                             StringTieWorker::tr("Enable output for Ballgown"),
                             StringTieWorker::tr("Select \"True\" to generate table files (*.ctab) that can be used as input to Ballgown (-b). "
                                                 "The files contain coverage data for the reference transcripts. "
                                                 "The parameter is only available if the \"Reference annotations\" file is specified. "
                                                 "It is also recommended to set \"Abundance for reference transcripts only\" to \"True\"."));

        Descriptor ballgownOutputFolder(BALLGOWN_OUTPUT_FOLDER,
                             StringTieWorker::tr("Output folder for Ballgown"),
                             StringTieWorker::tr("Specify a folder for table files (*.ctab) that can be used as input to Ballgown."));


        attributes << new Attribute(refAnnotations, BaseTypes::STRING_TYPE(), false, "");
        attributes << new Attribute(readsOrientation, BaseTypes::STRING_TYPE(), false, "");
        attributes << new Attribute(label, BaseTypes::STRING_TYPE(), false, "STRG");
        attributes << new Attribute(minIsoformFraction, BaseTypes::NUM_TYPE(), false, 0.1);
        attributes << new Attribute(minTranscriptLen, BaseTypes::NUM_TYPE(), false, 200);

        attributes << new Attribute(minAnchorLen, BaseTypes::NUM_TYPE(), false, 10);
        attributes << new Attribute(minJunctionCoverage, BaseTypes::NUM_TYPE(), false, 1.0);
        attributes << new Attribute(trimTranscript, BaseTypes::BOOL_TYPE(), false, true);
        attributes << new Attribute(minCoverage, BaseTypes::NUM_TYPE(), false, 2.5);
        attributes << new Attribute(minLocusSeparation, BaseTypes::NUM_TYPE(), false, 50);

        attributes << new Attribute(multiHitFraction, BaseTypes::NUM_TYPE(), false, 0.95);
        attributes << new Attribute(skipSequences, BaseTypes::STRING_TYPE(), false);

        Attribute* refOnlyAbudanceAttr = new Attribute(refOnlyAbudance, BaseTypes::BOOL_TYPE(), false, false);
        refOnlyAbudanceAttr->addRelation(new VisibilityRelation(REFERENCE_ANNOTATIONS, "", true));
        attributes << refOnlyAbudanceAttr;

        attributes << new Attribute(multiMappingCorrection, BaseTypes::BOOL_TYPE(), false, true);
        attributes << new Attribute(verboseLog, BaseTypes::BOOL_TYPE(), false, false);

        attributes << new Attribute(threadNum, BaseTypes::NUM_TYPE(), false, AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

        attributes << new Attribute(primaryOutput, BaseTypes::STRING_TYPE(), false);

        Attribute* geneAbudanceOutputAttr = new Attribute(geneAbudanceOutput, BaseTypes::BOOL_TYPE(), false, false);
        geneAbudanceOutputAttr->addSlotRelation(new SlotRelationDescriptor(OUT_PORT_ID, GENE_ABUND_OUT_SLOT_ID,
                                                                       QVariantList() << true));
        attributes << geneAbudanceOutputAttr;
        Attribute* geneAbudanceOutputFileAttr = new Attribute(geneAbudanceOutputFile, BaseTypes::STRING_TYPE(), false);
        geneAbudanceOutputFileAttr->addRelation(new VisibilityRelation(GENE_ABUDANCE_OUTPUT, true));
        attributes << geneAbudanceOutputFileAttr;

        Attribute* coverageRefOutputAttr = new Attribute(coverageRefOutput, BaseTypes::BOOL_TYPE(), false, false);
        coverageRefOutputAttr->addRelation(new VisibilityRelation(REFERENCE_ANNOTATIONS, "", true));
        attributes << coverageRefOutputAttr;
        Attribute* coverageRefOutputFileAttr = new Attribute(coverageRefOutputFile, BaseTypes::STRING_TYPE(), false);
        coverageRefOutputFileAttr->addRelation(new VisibilityRelation(COVERAGE_REF_OUTPUT, true));
        coverageRefOutputFileAttr->addRelation(new VisibilityRelation(REFERENCE_ANNOTATIONS, "", true));
        attributes << coverageRefOutputFileAttr;

        Attribute* ballgawnOutputAttr = new Attribute(ballgownOutput, BaseTypes::BOOL_TYPE(), false, false);
        ballgawnOutputAttr->addRelation(new VisibilityRelation(REFERENCE_ANNOTATIONS, "", true));
        attributes << ballgawnOutputAttr;
        Attribute* ballgownOutputFolderAttr = new Attribute(ballgownOutputFolder, BaseTypes::STRING_TYPE(), false, "");
        ballgownOutputFolderAttr->addRelation(new VisibilityRelation(BALLGOWN_OUTPUT, true));
        ballgownOutputFolderAttr->addRelation(new VisibilityRelation(REFERENCE_ANNOTATIONS, "", true));
        attributes << ballgownOutputFolderAttr;
    }

    QMap<QString, PropertyDelegate*> delegates;

    delegates[REFERENCE_ANNOTATIONS] = new URLDelegate("", "", false, false, false);
    {
        QVariantMap map;
        map[StringTieWorker::tr("Unstranded")] = "";
        map[StringTieWorker::tr("Forward (FR)")] = "--fr";
        map[StringTieWorker::tr("Reverse (RF)")] = "--rf";
        delegates[READS_ORIENTATION] = new ComboBoxDelegate(map);
    }
    {
        QVariantMap map;
        map["minimum"] = 0.0;
        map["maximum"] = 1.0;
        map["singleStep"] = 0.1;
        map["decimals"] = 2;
        delegates[MIN_ISOFORM_FRACTION] = new DoubleSpinBoxDelegate(map);
    }
    {
        QVariantMap map;
        map["minimum"] = 30;
        map["maximum"] = std::numeric_limits<int>::max();
        delegates[MIN_TRANSCRIPT_LEN] = new SpinBoxDelegate(map);
    }
    {
        QVariantMap map;
        map["minimum"] = 0;
        map["maximum"] = std::numeric_limits<int>::max();
        delegates[MIN_ANCHOR_LEN] = new SpinBoxDelegate(map);
    }
    {
        QVariantMap map;
        map["minimum"] = 0.0;
        map["maximum"] = std::numeric_limits<double>::max();
        map["singleStep"] = 0.1;
        map["decimals"] = 2;
        delegates[MIN_JUNCTION_COVERAGE] = new DoubleSpinBoxDelegate(map);
    }
    delegates[TRIM_TRANSCRIPT] = new ComboBoxWithBoolsDelegate();
    {
        QVariantMap map;
        map["minimum"] = 0.001;
        map["maximum"] = std::numeric_limits<double>::max();
        map["singleStep"] = 0.1;
        map["decimals"] = 3;
        delegates[MIN_COVERAGE] = new DoubleSpinBoxDelegate(map);
    }
    {
        QVariantMap map;
        map["minimum"] = 0;
        map["maximum"] = std::numeric_limits<int>::max();
        map["suffix"] = " bp";
        delegates[MIN_LOCUS_SEPARATION] = new SpinBoxDelegate(map);
    }

    {
        QVariantMap map;
        map["minimum"] = 0.00;
        map["maximum"] = std::numeric_limits<double>::max();
        map["singleStep"] = 0.01;
        map["decimals"] = 2;
        delegates[MULTI_HIT_FRACTION] = new DoubleSpinBoxDelegate(map);
    }
    delegates[REF_ONLY_ABUNDANCE] = new ComboBoxWithBoolsDelegate();

    {
        QVariantMap map;
        map[StringTieWorker::tr("Enabled")] = true;
        map[StringTieWorker::tr("Disabled")] = false;
        delegates[MULTI_MAPPING_CORRECTION] = new ComboBoxDelegate(map);
    }
    delegates[VERBOSE_LOG] = new ComboBoxWithBoolsDelegate();
    {
        QVariantMap map;
        map["minimum"] = 1;
        map["maximum"] = QThread::idealThreadCount();
        delegates[THREAD_NUM] = new SpinBoxDelegate(map);
    }

    DelegateTags outputUrlTags;
    outputUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
    outputUrlTags.set(DelegateTags::FILTER, DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::PLAIN_TEXT, true, QStringList()));
    outputUrlTags.set(DelegateTags::FORMAT, BaseDocumentFormats::PLAIN_TEXT);
    delegates[PRIMARY_OUTPUT] = new URLDelegate(outputUrlTags, "stringtie/primary-output");
    delegates[GENE_ABUDANCE_OUTPUT] = new ComboBoxWithBoolsDelegate();
    delegates[GENE_ABUDANCE_OUTPUT_FILE] = new URLDelegate(outputUrlTags, "stringtie/gene-abidance-output");

    delegates[COVERAGE_REF_OUTPUT] = new ComboBoxWithBoolsDelegate();
    delegates[COVERAGE_REF_OUTPUT_FILE] = new URLDelegate(outputUrlTags, "stringtie/coverage-output");

    DelegateTags ballgownUrlTags;
    ballgownUrlTags.set(DelegateTags::PLACEHOLDER_TEXT, "Auto");
    delegates[BALLGOWN_OUTPUT] = new ComboBoxWithBoolsDelegate();
    delegates[BALLGOWN_OUTPUT_FOLDER] = new URLDelegate(ballgownUrlTags, "", false, true);


    // Description of the element
    Descriptor desc(ACTOR_ID, StringTieWorker::tr("Assemble Transcripts with StringTie"),
                    StringTieWorker::tr("StringTie is a fast and highly efficient assembler of RNA-Seq alignments into potential transcripts. "
                                        "It uses a novel network flow algorithm as well as an optional de novo assembly step to assemble "
                                        "and quantitate full-length transcripts representing multiple splice variants for each gene locus."));

    // Create the actor prototype
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, ports, attributes);

    // Init and register the actor prototype
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new StringTiePrompter());
    proto->addExternalTool(StringTieSupport::ET_STRINGTIE_ID, "STRINGTIE_EXT_TOOL_PATH");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_RNA_SEQ(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new StringTieWorkerFactory());
}

Worker* StringTieWorkerFactory::createWorker(Actor *a) {
    return new StringTieWorker(a);
}

} // namespace LocalWorkflow
} // namespace U2
