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
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "CAP3Support.h"
#include "CAP3Worker.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {


 //////////////////////////////////////////////////////////////////////////
 //CAP3WorkerFactory
 //////////////////////////////////////////////////////////////////////////

const QString CAP3WorkerFactory::ACTOR_ID("cap3");

namespace {
    const QString OUTPUT_FILE("out-file");
    const QString CLIPPING_CUTOFF("clipping-cutoff");
    const QString CLIPPING_RANGE("clipping-range");
    const QString DIFF_CUTOFF("diff-cutoff");
    const QString DIFF_MAX_QSCORE("diff-max-qscore");
    const QString MATCH_SCORE_FACTOR("match-score-factor");
    const QString MISMATCH_SCORE_FACTOR("mismatch-score-factor");
    const QString GAP_PENALTY_FACTOR("gap-penalty-factor");
    const QString OVERLAP_SIM_SCORE_CUTOFF("overlap-sim-score-cutoff");
    const QString OVERLAP_LENGTH_CUTOFF("overlap-length-cutoff");
    const QString OVERLAP_PERC_ID_CUTOFF("overlap-perc-id-cutoff");
    const QString MAX_NUM_WORD_MATCHES("max-num-word-matches");
    const QString BAND_EXP_SIZE("band-exp-size");
    const QString MAX_GAP_IN_OVERLAP("max-gap-in-overlap");
    const QString ASSEMBLY_REVERSE("assembly-reverse");

    const QString CAP3_EXT_TOOL_PATH("path");
    const QString TMP_DIR_PATH("tmp-dir");

    const QString IN_PORT_DESCR("in-data");
    const QString IN_URL_SLOT_ID("in.url");
    const QString IN_TYPE_ID("cap3-in-data");
}

void CAP3WorkerFactory::init() {
    QList<PortDescriptor*> portDescriptors;
    QList<Attribute*> attributes;

    // Input port
    Descriptor inPortDesc(IN_PORT_DESCR,
        CAP3Worker::tr("Input sequences"),
        CAP3Worker::tr("DNA sequences that need to be assembled"));

    QMap<Descriptor, DataTypePtr> inputMap;
    Descriptor datasetDesc(IN_URL_SLOT_ID,
        CAP3Worker::tr("Input URL(s)"),
        CAP3Worker::tr("URL(s) of input file(s) in FASTA format with DNA sequences that need to be assembled"));

    inputMap[datasetDesc] = BaseTypes::STRING_TYPE();
    inputMap[BaseSlots::DATASET_SLOT()] = BaseTypes::STRING_TYPE();

    portDescriptors << new PortDescriptor(inPortDesc,
        DataTypePtr(new MapDataType(IN_TYPE_ID, inputMap)),
        true /* input */);

    // Description of the element
    QString cap3Description = AppContext::getExternalToolRegistry()->getById(CAP3Support::ET_CAP3_ID)->getDescription();
    Descriptor desc(ACTOR_ID, CAP3Worker::tr("Assembly Sequences with CAP3"), cap3Description);

    // Define parameters of the element
    // Default values are taken from the console program CAP3 (version date 12/21/07)
    Descriptor outputFile(OUTPUT_FILE,
        CAP3Worker::tr("Output file"),
        CAP3Worker::tr("Write assembly results to this output file in ACE format."));

    Descriptor clippingCutoff(CLIPPING_CUTOFF,
        CAP3Worker::tr("Quality cutoff for clipping"),
        CAP3Worker::tr("Base quality cutoff for clipping (-c)"));

    Descriptor clippingRange(CLIPPING_RANGE,
        CAP3Worker::tr("Clipping range"),
        CAP3Worker::tr("One of the parameters to control clipping of a poor"
            " end region of a read (-y)"
            "<p>If there are quality values, CAP3 computes two positions qualpos5 and"
            " qualpos3 of read f such that the region of read f from position qualpos5"
            " to position qualpos3 consists mostly of quality values greater than"
            " <i>Quality cutoff for clipping</i>.</p>"
            " <p>If there are no quality values, then qualpos5 is set to 1 and qualpos3 is set"
            " the length of read f."
            " <p>The range for the left clipping position of read f is from 1 to qualpos5 + <i>Clipping range</i>."
            " <br/>The range for the right clipping position of read f is from qualpos3 - <i>Clipping range</i>"
            " to the end of read f.</p>"));

    Descriptor diffCutoff(DIFF_CUTOFF,
        CAP3Worker::tr("Quality cutoff for differences"),
        CAP3Worker::tr("Base quality cutoff for differences (-b)"));

    Descriptor diffMaxQscore(DIFF_MAX_QSCORE,
        CAP3Worker::tr("Maximum difference score"),
        CAP3Worker::tr("Max qscore sum at differences (-d)"
            "<p>If an overlap contains lots of differences at"
            " bases of high quality, then the overlap is removed.</p>"
            "<p>The difference score is calculated as follows."
            " If the overlap contains a difference"
            " at bases of quality values q1 and q2, then the score at"
            " the difference is max(0, min(q1, q2) - b), where"
            " b is <i>Quality cutoff for differences</i>."
            " The difference score of an overlap is the sum of scores at each difference.</p>"));

    Descriptor matchScoreFactor(MATCH_SCORE_FACTOR,
        CAP3Worker::tr("Match score factor"),
        CAP3Worker::tr("Match score factor (-m) is one of the parameters that affects"
            " similarity score of an overlap. See <i>Overlap similarity score cutoff</i>"
            " description for details."));

    Descriptor mismatchScoreFactor(MISMATCH_SCORE_FACTOR,
        CAP3Worker::tr("Mismatch score factor"),
        CAP3Worker::tr("Mismatch score factor (-n) is one of the parameters that affects"
            " similarity score of an overlap. See <i>Overlap similarity score cutoff</i>"
            " description for details."));

    Descriptor gapPenaltyFactor(GAP_PENALTY_FACTOR,
        CAP3Worker::tr("Gap penalty factor"),
        CAP3Worker::tr("Gap penalty factor (-g) is one of the parameters that affects"
            " similarity score of an overlap. See <i>Overlap similarity score cutoff</i>"
            " description for details."));

    Descriptor overlapSimScoreCutoff(OVERLAP_SIM_SCORE_CUTOFF,
        CAP3Worker::tr("Overlap similarity score cutoff"),
        CAP3Worker::tr("If the similarity score of an overlap is less than"
            " the overlap similarity score cutoff (-s), then the overlap is removed."
            "<p>The similarity score of an overlapping alignment is defined"
            " using base quality values as follows."
            "<p>A match at bases of quality values q1 and q2 is given a score of m * min(q1,q2),"
            " where m is <i>Match score factor</i>.</p>"
            "<p>A mismatch at bases of quality values q1 and q2 is given a score of n * min(q1,q2),"
            " where n is <i>Mismatch score factor</i>.</p>"
            "<p>A base of quality value q1 in a gap is given a score of -g * min(q1,q2),"
            " where q2 is the quality value of the base in the other sequence right before"
            " the gap and g is <i>Gap penalty factor</i>.</p>"
            "<p>The score of a gap is the sum of scores of each base in the gap"
            " minus a gap open penalty.</p>"
            "<p>The similarity score of an overlapping alignment"
            " is the sum of scores of each match, each mismatch, and each gap. "
            "</p>"));

    Descriptor overlapLengthCutoff(OVERLAP_LENGTH_CUTOFF,
        CAP3Worker::tr("Overlap length cutoff"),
        CAP3Worker::tr("An overlap is taken into account only if the length"
            " of the overlap in bp is no less than the specified value (parameter -o of CAP3)."));

    Descriptor overlapPercIdCutoff(OVERLAP_PERC_ID_CUTOFF,
        CAP3Worker::tr("Overlap percent identity cutoff"),
        CAP3Worker::tr("An overlap is taken into account only if the percent"
            " identity of the overlap is no less than the specified value (parameter -p of CAP3)."));

    Descriptor maxNumWordMatches(MAX_NUM_WORD_MATCHES,
        CAP3Worker::tr("Max number of word matches"),
        CAP3Worker::tr("This parameter allows one to trade off the efficiency of the"
            " program for its accuracy (parameter -t of CAP3)."
            "<p>For a read f, CAP3 computes overlaps between"
            " read f and other reads by considering short word matches between"
            " read f and other reads. A word match is examined to see if it can be"
            " extended into a long overlap. If read f has overlaps with many other reads,"
            " then read f has many short word matches with many other reads.</p>"
            "<p>This parameter gives an upper limit, for any word, on the number of word matches"
            " between read f and other reads that are considered by CAP3.</p>"
            "<p>Using a large value for this parameter allows CAP3"
            " to consider more word matches between read f and other reads, which can find"
            " more overlaps for read f, but slows down the program.</p>"
            "<p>Using a small value for this parameter has the opposite effect.</p>"));

    Descriptor bandExpSize(BAND_EXP_SIZE,
        CAP3Worker::tr("Band expansion size"),
        CAP3Worker::tr("CAP3 determines a minimum band of diagonals for an overlapping"
            " alignment between two sequence reads. The band is expanded"
            " by a number of bases specified by this value (parameter -a of CAP3)."));

    Descriptor maxGapInOverlap(MAX_GAP_IN_OVERLAP,
        CAP3Worker::tr("Max gap length in an overlap"),
        CAP3Worker::tr("The maximum length of gaps allowed in any overlap (-f)"
            "<p>I.e. overlaps with longer gaps are rejected.</p>"
            "<p>Note that a small value for this parameter may cause the program to"
            " remove true overlaps and to produce incorrect results.</p>"
            "<p>The parameter may be used to split reads from"
            " alternative splicing forms into separate contigs.</p>"));

    Descriptor assemblyReverse(ASSEMBLY_REVERSE,
        CAP3Worker::tr("Assembly reverse reads"),
        CAP3Worker::tr("Specifies whether to consider reads in reverse orientation"
            " for assembly (originally, parameter -r of CAP3)."));

    Descriptor extToolPath(CAP3_EXT_TOOL_PATH,
        CAP3Worker::tr("CAP3 tool path"),
        CAP3Worker::tr("The path to the CAP3 external tool in UGENE."));

    Descriptor tmpDir(TMP_DIR_PATH,
        CAP3Worker::tr("Temporary folder"),
        CAP3Worker::tr("The folder for temporary files."));

    attributes << new Attribute(outputFile, BaseTypes::STRING_TYPE(), true, "result.ace");
    attributes << new Attribute(clippingCutoff, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultBaseQualityClipCutoff));
    attributes << new Attribute(clippingRange, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultClippingRange));
    attributes << new Attribute(diffCutoff, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultBaseQualityDiffCutoff));
    attributes << new Attribute(diffMaxQscore, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultMaxQScoreSum));
    attributes << new Attribute(matchScoreFactor, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultMatchScoreFactor));
    attributes << new Attribute(mismatchScoreFactor, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultMismatchScoreFactor));
    attributes << new Attribute(gapPenaltyFactor, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultGapPenaltyFactor));
    attributes << new Attribute(overlapSimScoreCutoff, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultOverlapSimilarityScoreCutoff));
    attributes << new Attribute(overlapLengthCutoff, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultOverlapLengthCutoff));
    attributes << new Attribute(overlapPercIdCutoff, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultOverlapPercentIdentityCutoff));
    attributes << new Attribute(maxNumWordMatches, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultMaxNumberOfWordMatches));
    attributes << new Attribute(bandExpSize, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultBandExpansionSize));
    attributes << new Attribute(maxGapInOverlap, BaseTypes::NUM_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultMaxGapLength));
    attributes << new Attribute(assemblyReverse, BaseTypes::BOOL_TYPE(), false, QVariant(CAP3SupportTaskSettings::defaultReverseReads));
    attributes << new Attribute(extToolPath, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));
    attributes << new Attribute(tmpDir, BaseTypes::STRING_TYPE(), true, QVariant(L10N::defaultStr()));

    // Create the actor prototype
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, portDescriptors, attributes);

    // Values range of parameters
    QMap<QString, PropertyDelegate*> delegates;
    delegates[OUTPUT_FILE] = new URLDelegate("", "", false, false);
    delegates[CAP3_EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);
    {
        QVariantMap vm;
        vm["minimum"] = 5;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[CLIPPING_CUTOFF] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 5;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[CLIPPING_RANGE] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 15;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[DIFF_CUTOFF] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 100;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[DIFF_MAX_QSCORE] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MATCH_SCORE_FACTOR] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = INT_MIN;
        vm["maximum"] = 0;
        vm["singleStep"] = -1;
        delegates[MISMATCH_SCORE_FACTOR] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 0;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[GAP_PENALTY_FACTOR] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 250;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[OVERLAP_SIM_SCORE_CUTOFF] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 15;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[OVERLAP_LENGTH_CUTOFF] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 65;
        vm["maximum"] = 100;
        vm["singleStep"] = 1;
        delegates[OVERLAP_PERC_ID_CUTOFF] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 30;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MAX_NUM_WORD_MATCHES] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 10;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[BAND_EXP_SIZE] = new SpinBoxDelegate(vm);
    }
    {
        QVariantMap vm;
        vm["minimum"] = 10;
        vm["maximum"] = INT_MAX;
        vm["singleStep"] = 1;
        delegates[MAX_GAP_IN_OVERLAP] = new SpinBoxDelegate(vm);
    }

    // Init and register the actor prototype
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new CAP3Prompter());
    proto->addExternalTool(CAP3Support::ET_CAP3_ID, CAP3_EXT_TOOL_PATH);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ASSEMBLY(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new CAP3WorkerFactory());
}

//////////////////////////////////////////////////////////////////////////
// CAP3Prompter
//////////////////////////////////////////////////////////////////////////

CAP3Prompter::CAP3Prompter(Actor* p) : PrompterBase<CAP3Prompter>(p) {
}

QString CAP3Prompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(IN_PORT_DESCR));
    Actor* producer = input->getProducer(IN_URL_SLOT_ID);
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";
    QString doc = CAP3Worker::tr("Aligns long DNA reads from <u>%1</u> with CAP3.").arg(producerName);

    return doc;
}

//////////////////////////////////////////////////////////////////////////
// CAP3Worker
//////////////////////////////////////////////////////////////////////////

CAP3Worker::CAP3Worker(Actor* a) : BaseWorker(a), input(NULL), datasetNumber(0) {
}

void CAP3Worker::initSettings() {
    settings.openView = false;
    settings.bandExpansionSize = getValue<int>(BAND_EXP_SIZE);
    settings.baseQualityDiffCutoff = getValue<int>(DIFF_CUTOFF);
    settings.baseQualityClipCutoff = getValue<int>(CLIPPING_CUTOFF);
    settings.maxQScoreSum = getValue<int>(DIFF_MAX_QSCORE);
    settings.maxGapLength = getValue<int>(MAX_GAP_IN_OVERLAP);
    settings.gapPenaltyFactor = getValue<int>(GAP_PENALTY_FACTOR);
    settings.matchScoreFactor = getValue<int>(MATCH_SCORE_FACTOR);
    settings.mismatchScoreFactor = getValue<int>(MISMATCH_SCORE_FACTOR);
    settings.overlapSimilarityScoreCutoff = getValue<int>(OVERLAP_SIM_SCORE_CUTOFF);
    settings.overlapLengthCutoff = getValue<int>(OVERLAP_LENGTH_CUTOFF);
    settings.overlapPercentIdentityCutoff = getValue<int>(OVERLAP_PERC_ID_CUTOFF);
    settings.maxNumberOfWordMatches = getValue<int>(MAX_NUM_WORD_MATCHES);
    settings.clippingRange = getValue<int>(CLIPPING_RANGE);
    settings.reverseReads = getValue<bool>(ASSEMBLY_REVERSE);

    settings.outputFilePath = getValue<QString>(OUTPUT_FILE);
}

void CAP3Worker::initPaths() {
    QString tmpDirPath = actor->getParameter(TMP_DIR_PATH)->getAttributeValue<QString>(context);
    if (QString::compare(tmpDirPath, "default", Qt::CaseInsensitive) != 0) {
        AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(tmpDirPath);
    }

    QString extToolPath = actor->getParameter(CAP3_EXT_TOOL_PATH)->getAttributeValue<QString>(context);
    if (QString::compare(extToolPath, "default", Qt::CaseInsensitive) != 0) {
        AppContext::getExternalToolRegistry()->getById(CAP3Support::ET_CAP3_ID)->setPath(extToolPath);
    }
}

Task * CAP3Worker::runCap3() {
    // Modify output file name, if required
    QString fileName = getValue<QString>(OUTPUT_FILE);
    const QString ext = ".ace";

    if (datasetNumber > 0) {
        bool appendExtToResult = false;
        if (fileName.endsWith(ext)) {
            int location = fileName.size() - ext.size();
            fileName.truncate(location);
            appendExtToResult = true;
        }
        fileName += "_" + QString::number(datasetNumber);
        if (appendExtToResult) {
            fileName += ext;
        }

        settings.outputFilePath = fileName;
    }

    // Run the task
    CAP3SupportTask *capTask = new CAP3SupportTask(settings);
    capTask->addListeners(createLogListeners());
    connect(capTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));

    datasetNumber++;
    settings.inputFiles.clear();

    return capTask;
}

void CAP3Worker::init() {
    input = ports.value(IN_PORT_DESCR);

    initSettings();
    initPaths();
}

Task* CAP3Worker::tick() {
    U2OpStatus2Log os;

    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        SAFE_POINT(!inputMessage.isEmpty(), "NULL message!", NULL);

        QVariantMap data = inputMessage.getData().toMap();
        if (!data.contains(IN_URL_SLOT_ID)) {
            os.setError("CAP3 input slot is empty!");
            return new FailTask(os.getError());
        }

        QString dataset = data[BaseSlots::DATASET_SLOT().getId()].toString();
        bool runCapForPreviousDataset = false;

        if (dataset != currentDatasetName) {
            if (!currentDatasetName.isEmpty()) {
                runCapForPreviousDataset = true;
            }

            settings.inputFiles = inputSeqUrls;
            inputSeqUrls.clear();
            currentDatasetName = dataset;
        }

        inputSeqUrls << data.value(IN_URL_SLOT_ID).value<QString>();

        if (runCapForPreviousDataset) {
            return runCap3();
        }

        settings.inputFiles = inputSeqUrls;
    }
    else if (input->isEnded()) {
        if (!settings.inputFiles.isEmpty()) {
            return runCap3();
        }
        else {
            datasetNumber = 0;
            setDone();
        }
    }

    return NULL;
}

void CAP3Worker::sl_taskFinished() {
    CAP3SupportTask* capTask = qobject_cast<CAP3SupportTask*>(sender());
    SAFE_POINT(NULL != capTask, "NULL task!", );

    if (!capTask->isFinished()) {
        return;
    }

    QString outputFile = capTask->getOutputFile();
    if (!outputFile.isEmpty()) {
        context->getMonitor()->addOutputFile(outputFile, getActor()->getId());
    }
}

void CAP3Worker::cleanup() {
    inputSeqUrls.clear();
    currentDatasetName = "";
}

} //namespace LocalWorkflow
} //namespace U2
