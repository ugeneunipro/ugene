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

#include <QScopedPointer>

#include <U2Algorithm/DnaAssemblyTask.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "BwaSupport.h"
#include "BwaMemWorker.h"
#include "BwaTask.h"

namespace U2 {
namespace LocalWorkflow {

const QString BwaMemWorkerFactory::ACTOR_ID("bwamem-id");

static const QString THREADS = "threads";
static const QString MIN_SEED = "min-seed";
static const QString INDEX_ALG = "index-alg";
static const QString BAND_WIDTH = "band-width";
static const QString DROPOFF = "dropoff";
static const QString INTERNAL_SEED_LOOKUP = "seed-lookup";
static const QString SKIP_SEED_THRESHOLD = "seed-threshold";
static const QString DROP_CHAINS_THRESHOLD = "drop-chains";
static const QString MAX_MATE_RESCUES = "mate-rescue";
static const QString SKIP_MATE_RESCUES = "skip-mate-rescues";
static const QString SKIP_PAIRING = "skip-pairing";
static const QString SKIP_MISTMATCH_SCORE = "skip-score";
static const QString MATCH_SCORE = "match-score";
static const QString MISMATCH_PENALTY = "mistmatch-penalty";
static const QString GAP_OPEN_PENALTY = "gap-open-penalty";
static const QString GAP_EXTENSION_PENALTY = "gap-ext-penalty";
static const QString CLIPPING_PENALTY = "clipping-penalty";
static const QString UNPAIRED_PENALTY = "inpaired-panalty";
static const QString SCORE_THRESHOLD = "score-threshold";

static const QString BASE_BWA_SUBDIR("bwa");
static const QString BASE_BWA_OUTFILE("out.sam");

const QString OUTPUT_SUBDIR("BWA-MEM");

/************************************************************************/
/* Worker */
/************************************************************************/
BwaMemWorker::BwaMemWorker(Actor *p)
: BaseShortReadsAlignerWorker(p, BwaTask::ALGORITHM_BWA_MEM)
{
}

QVariantMap BwaMemWorker::getCustomParameters() const {
    QMap<QString, QVariant> customSettings;

    customSettings.insert(BwaTask::OPTION_INDEX_ALGORITHM, getValue<QString>(INDEX_ALG));
    customSettings.insert(BwaTask::OPTION_THREADS, getValue<int>(THREADS));
    customSettings.insert(BwaTask::OPTION_MIN_SEED, getValue<int>(MIN_SEED));
    customSettings.insert(BwaTask::OPTION_BAND_WIDTH, getValue<int>(BAND_WIDTH));
    customSettings.insert(BwaTask::OPTION_DROPOFF, getValue<int>(DROPOFF));
    customSettings.insert(BwaTask::OPTION_INTERNAL_SEED_LOOKUP, getValue<float>(INTERNAL_SEED_LOOKUP));
    customSettings.insert(BwaTask::OPTION_SKIP_SEED_THRESHOLD, getValue<int>(SKIP_SEED_THRESHOLD));
    customSettings.insert(BwaTask::OPTION_DROP_CHAINS_THRESHOLD, getValue<float>(DROP_CHAINS_THRESHOLD));
    customSettings.insert(BwaTask::OPTION_MAX_MATE_RESCUES, getValue<int>(MAX_MATE_RESCUES));

    if (getValue<bool>(SKIP_MATE_RESCUES)) {
        customSettings.insert(BwaTask::OPTION_SKIP_MATE_RESCUES, true);
    }
    if (getValue<bool>(SKIP_PAIRING)) {
        customSettings.insert(BwaTask::OPTION_SKIP_PAIRING, true);
    }

    customSettings.insert(BwaTask::OPTION_MATCH_SCORE, getValue<int>(MATCH_SCORE));
    customSettings.insert(BwaTask::OPTION_MISMATCH_PENALTY, getValue<int>(MISMATCH_PENALTY));
    customSettings.insert(BwaTask::OPTION_GAP_OPEN_PENALTY, getValue<int>(GAP_OPEN_PENALTY));
    customSettings.insert(BwaTask::OPTION_GAP_EXTENSION_PENALTY, getValue<int>(GAP_EXTENSION_PENALTY));
    customSettings.insert(BwaTask::OPTION_CLIPPING_PENALTY, getValue<int>(CLIPPING_PENALTY));
    customSettings.insert(BwaTask::OPTION_UNPAIRED_PENALTY, getValue<int>(UNPAIRED_PENALTY));
    customSettings.insert(BwaTask::OPTION_SCORE_THRESHOLD, getValue<int>(SCORE_THRESHOLD));

    customSettings.insert(BwaTask::OPTION_MEM_ALIGNMENT,true);

    return customSettings;
}

QString BwaMemWorker::getDefaultFileName() const {
    return BASE_BWA_OUTFILE;
}

QString BwaMemWorker::getBaseSubdir() const {
    return BASE_BWA_SUBDIR;
}

void BwaMemWorker::setGenomeIndex(DnaAssemblyToRefTaskSettings& settings) {
    QString referenceInputType = getValue<QString>(REFERENCE_INPUT_TYPE);
    if (referenceInputType == DnaAssemblyToRefTaskSettings::INDEX) {
        settings.prebuiltIndex = true;
        settings.indexDir = getValue<QString>(INDEX_DIR);
        settings.indexBasename = getValue<QString>(INDEX_BASENAME);

        QString baseUrl = QDir(settings.indexDir).filePath(settings.indexBasename);
        settings.refSeqUrl = baseUrl;
        settings.indexFileName = baseUrl;
    } else {
        settings.refSeqUrl = getValue<QString>(REFERENCE_GENOME);
        settings.prebuiltIndex = DnaAssemblyToReferenceTask::isIndexUrl(settings.refSeqUrl.getURLString(), BwaTask::indexSuffixes);
        if (settings.prebuiltIndex) {
            settings.indexDir = settings.refSeqUrl.dirPath();
            settings.indexBasename = settings.refSeqUrl.baseFileName();
        } else {
            settings.indexDir = "";
            settings.indexBasename = "";
            settings.indexFileName = QDir(settings.refSeqUrl.dirPath()).filePath(settings.refSeqUrl.baseFileName());
        }
    }
}

QString BwaMemWorker::getAlignerSubdir() const {
    return OUTPUT_SUBDIR;
}

/************************************************************************/
/* Factory */
/************************************************************************/
void BwaMemWorkerFactory::init() {
    QList<Attribute*> attrs;
    QMap<QString, PropertyDelegate*> delegates;

    addCommonAttributes(attrs, delegates,
                        BwaMemWorker::tr("BWA index folder"),
                        BwaMemWorker::tr("BWA index basename"));
    {
        Descriptor threads(THREADS,
            BwaMemWorker::tr("Number of threads"),
            BwaMemWorker::tr("Number of threads (-t)."));

        Descriptor minSeed(MIN_SEED,
            BwaMemWorker::tr("Min seed length"),
            BwaMemWorker::tr("Path to indexed reference genome (-k)."));

        Descriptor indexAlg(INDEX_ALG,
            BwaMemWorker::tr("Index algorithm"),
            BwaMemWorker::tr("Index algorithm (-a)."));

        Descriptor bandWidth(BAND_WIDTH,
            BwaMemWorker::tr("Band width"),
            BwaMemWorker::tr("Band width for banded alignment (-w)."));

        Descriptor dropoff(DROPOFF,
            BwaMemWorker::tr("Dropoff"),
            BwaMemWorker::tr("Off-diagonal X-dropoff (-d)."));

        Descriptor internalSeed(INTERNAL_SEED_LOOKUP,
            BwaMemWorker::tr("Internal seed length"),
            BwaMemWorker::tr("Look for internal seeds inside a seed longer than {-k} (-r)."));

        Descriptor skipSeed(SKIP_SEED_THRESHOLD,
            BwaMemWorker::tr("Skip seed threshold"),
            BwaMemWorker::tr("Skip seeds with more than INT occurrences (-c)."));

        Descriptor dropChains(DROP_CHAINS_THRESHOLD,
            BwaMemWorker::tr("Drop chain threshold"),
            BwaMemWorker::tr("Drop chains shorter than FLOAT fraction of the longest overlapping chain (-D)."));

        Descriptor maxMate(MAX_MATE_RESCUES,
            BwaMemWorker::tr("Rounds of mate rescues"),
            BwaMemWorker::tr("Perform at most INT rounds of mate rescues for each read (-m)."));

        Descriptor skipMate(SKIP_MATE_RESCUES,
            BwaMemWorker::tr("Skip mate rescue"),
            BwaMemWorker::tr("Skip mate rescue (-S)."));

        Descriptor skipPairing(SKIP_PAIRING,
            BwaMemWorker::tr("Skip pairing"),
            BwaMemWorker::tr("Skip pairing; mate rescue performed unless -S also in use (-P)."));

        Descriptor matchScore(MATCH_SCORE,
            BwaMemWorker::tr("Matching score"),
            BwaMemWorker::tr("Score for a sequence match (-A)."));

        Descriptor mismatchPenalty(MISMATCH_PENALTY,
            BwaMemWorker::tr("Mismatch penalty"),
            BwaMemWorker::tr("Penalty for a mismatch (-B)."));

        Descriptor gapOpen(GAP_OPEN_PENALTY,
            BwaMemWorker::tr("Gap open penalty"),
            BwaMemWorker::tr("Gap open penalty (-O)."));

        Descriptor gapExt(GAP_EXTENSION_PENALTY,
            BwaMemWorker::tr("Gap extension penalty"),
            BwaMemWorker::tr("Gap extension penalty; a gap of size k cost {-O} (-E)."));

        Descriptor clippingPen(CLIPPING_PENALTY,
            BwaMemWorker::tr("Penalty for clipping"),
            BwaMemWorker::tr("Penalty for clipping (-L)."));

        Descriptor unpairedPenalty(UNPAIRED_PENALTY,
            BwaMemWorker::tr("Penalty unpaired"),
            BwaMemWorker::tr("Penalty for an unpaired read pair (-U)."));

        Descriptor scoreThreshold(SCORE_THRESHOLD,
            BwaMemWorker::tr("Score threshold"),
            BwaMemWorker::tr("Minimum score to output (-T)."));


        attrs << new Attribute(threads, BaseTypes::NUM_TYPE(), false, QVariant(getThreadsCount()));
        attrs << new Attribute(minSeed, BaseTypes::NUM_TYPE(), false, QVariant(19));
        attrs << new Attribute(indexAlg, BaseTypes::STRING_TYPE(), false, QVariant("autodetect"));
        attrs << new Attribute(bandWidth, BaseTypes::NUM_TYPE(), false, QVariant(100));
        attrs << new Attribute(dropoff, BaseTypes::NUM_TYPE(), false, QVariant(100));
        attrs << new Attribute(internalSeed, BaseTypes::NUM_TYPE(), false, QVariant(1.5));
        attrs << new Attribute(skipSeed, BaseTypes::NUM_TYPE(), false, QVariant(10000));
        attrs << new Attribute(dropChains, BaseTypes::NUM_TYPE(), false, QVariant(0.5));
        attrs << new Attribute(maxMate, BaseTypes::NUM_TYPE(), false, QVariant(100));
        attrs << new Attribute(skipMate, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(skipPairing, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        attrs << new Attribute(matchScore, BaseTypes::NUM_TYPE(), false, QVariant(1));
        attrs << new Attribute(mismatchPenalty, BaseTypes::NUM_TYPE(), false, QVariant(4));
        attrs << new Attribute(gapOpen, BaseTypes::NUM_TYPE(), false, QVariant(6));
        attrs << new Attribute(gapExt, BaseTypes::NUM_TYPE(), false, QVariant(1));
        attrs << new Attribute(clippingPen, BaseTypes::NUM_TYPE(), false, QVariant(5));
        attrs << new Attribute(unpairedPenalty, BaseTypes::NUM_TYPE(), false, QVariant(17));
        attrs << new Attribute(scoreThreshold, BaseTypes::NUM_TYPE(), false, QVariant(30));
     }

     {
         QVariantMap spinMap; spinMap["minimum"] = QVariant(0); spinMap["maximum"] = QVariant(INT_MAX);
         delegates[THREADS]  = new SpinBoxDelegate(spinMap);
         delegates[MIN_SEED] = new SpinBoxDelegate(spinMap);
         delegates[BAND_WIDTH] = new SpinBoxDelegate(spinMap);
         delegates[DROPOFF] = new SpinBoxDelegate(spinMap);
         delegates[INTERNAL_SEED_LOOKUP] = new DoubleSpinBoxDelegate(spinMap);
         delegates[SKIP_MATE_RESCUES] = new SpinBoxDelegate(spinMap);
         delegates[DROP_CHAINS_THRESHOLD] = new DoubleSpinBoxDelegate(spinMap);
         delegates[MAX_MATE_RESCUES] = new SpinBoxDelegate(spinMap);
         delegates[MATCH_SCORE] = new SpinBoxDelegate(spinMap);
         delegates[MISMATCH_PENALTY] = new SpinBoxDelegate(spinMap);
         delegates[GAP_OPEN_PENALTY] = new SpinBoxDelegate(spinMap);
         delegates[GAP_EXTENSION_PENALTY] = new SpinBoxDelegate(spinMap);
         delegates[CLIPPING_PENALTY] = new SpinBoxDelegate(spinMap);
         delegates[UNPAIRED_PENALTY] = new SpinBoxDelegate(spinMap);
         delegates[SCORE_THRESHOLD] = new SpinBoxDelegate(spinMap);

         QVariantMap vm;
         vm["autodetect"] = "autodetect";
         vm["bwtsw"] = "bwtsw";
         vm["div"] = "div";
         vm["is"] = "is";
         delegates[INDEX_ALG] = new ComboBoxDelegate(vm);
    }

    Descriptor protoDesc(BwaMemWorkerFactory::ACTOR_ID,
        BwaMemWorker::tr("Map Reads with BWA-MEM"),
        BwaMemWorker::tr("Burrows-Wheeler Alignment (BWA) is a program for mapping short DNA sequence reads"
                         " to a long reference sequence. This element runs \"BWA-MEM\" algorithm"
                         " of the program. The algorithm is designed for sequence reads from 70bp to 1Mbp."
                         "<br/><br/>Provide URL(s) to FASTA or FASTQ file(s) with NGS reads to the input"
                         " port of the element, set up the reference sequence in the parameters."
                         " The result is saved to the specified SAM file, URL to the file is passed"
                         " to the output port."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, getPortDescriptors(), attrs);
    proto->setPrompter(new ShortReadsAlignerPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPortValidator(IN_PORT_DESCR, new ShortReadsAlignerSlotsValidator());
    proto->addExternalTool(ET_BWA);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_MAP_ASSEMBLE_READS(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new BwaMemWorkerFactory());
}

Worker *BwaMemWorkerFactory::createWorker(Actor *a) {
    return new BwaMemWorker(a);
}
} // LocalWorkflow
} // U2
