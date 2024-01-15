/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Primer3Task.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/U1AnnotationUtils.h>

#include "Primer3Plugin.h"
#include "Primer3TaskSettings.h"
#include "PrimerPair.h"
#include "PrimerSingle.h"

#include "primer3_core/libprimer3.h"
#include "primer3_core/primer3_boulder_main.h"

#include <QFile>

namespace U2 {

Primer3Task::Primer3Task(const QSharedPointer<Primer3TaskSettings>& _settings)
    : Task(tr("Pick primers task"), TaskFlag_ReportingIsEnabled),
      settings(_settings) {

    // Primer3Task is single threaded: the original "primer3" tool doesn't support parallel calculations.
    addTaskResource(TaskResourceUsage(AppResource::buildDynamicResourceId("Primer 3 single thread"), 1, TaskResourceStage::Run));
}

void Primer3Task::prepare() {
    U2Region sequenceRange = settings->getSequenceRange();
    int sequenceSize = settings->getSequenceSize();
    U2Region includedRegion = settings->getIncludedRegion();
    int firstBaseIndex = settings->getFirstBaseIndex();
    int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - firstBaseIndex : 0;
    CHECK_EXT(includedRegionOffset >= 0, stateInfo.setError(tr("Incorrect sum \"Included Region Start + First Base Index\" - should be more or equal than 0")), );

    // Add a sequence shift if selected region covers the junction point
    if (sequenceRange.endPos() > sequenceSize + includedRegionOffset) {
        SAFE_POINT_EXT(settings->isSequenceCircular(), stateInfo.setError("Unexpected region, sequence should be circular"), );

        QByteArray seq = settings->getSequence();
        int appendLength = sequenceRange.endPos() - sequenceSize - firstBaseIndex;
        seq.append(seq.left(sequenceRange.endPos() - sequenceSize - firstBaseIndex));
        settings->setSequence(seq, settings->isSequenceCircular());
        auto seqQuality = settings->getSequenceQuality();
        seqQuality.append(seqQuality.mid(0, appendLength));
        settings->setSequenceQuality(seqQuality);
    }

    // Calculate an offset for result primers
    offset = sequenceRange.startPos + includedRegionOffset;

    settings->setSequence(settings->getSequence().mid(sequenceRange.startPos, sequenceRange.length), settings->isSequenceCircular());
    settings->setSequenceQuality(settings->getSequenceQuality().mid(sequenceRange.startPos, sequenceRange.length));
}

void Primer3Task::run() {
    // Repeat Library loading
    // Library could be large and loading - time consuming,
    // so it is important to do it in a separate thread
    QByteArray repeatLibPath = settings->getRepeatLibraryPath();
    if (!repeatLibPath.isEmpty()) {
        auto primerSettings = settings->getPrimerSettings();
        p3_set_gs_primer_mispriming_library(primerSettings, repeatLibPath.data());
        if (primerSettings->p_args.repeat_lib->error.storage_size != 0) {
            stateInfo.setError(primerSettings->p_args.repeat_lib->error.data);
        }
        if (primerSettings->p_args.repeat_lib->warning.storage_size != 0) {
            stateInfo.addWarning(primerSettings->p_args.repeat_lib->warning.data);
        }
    }
    CHECK_OP(stateInfo, );

    // Mishyb Library loading
    // The same thing - could be large
    QByteArray mishybLibPath = settings->getMishybLibraryPath();
    if (!mishybLibPath.isEmpty()) {
        auto primerSettings = settings->getPrimerSettings();
        p3_set_gs_primer_internal_oligo_mishyb_library(primerSettings, mishybLibPath.data());
        if (primerSettings->o_args.repeat_lib->error.storage_size != 0) {
            stateInfo.setError(primerSettings->o_args.repeat_lib->error.data);
        }
        if (primerSettings->o_args.repeat_lib->warning.storage_size != 0) {
            stateInfo.addWarning(primerSettings->o_args.repeat_lib->warning.data);
        }
    }
    CHECK_OP(stateInfo, );

    // Thermodynamic Parameters loading
    // The same thing - could be large
    QByteArray thermodynamicParametersPath = settings->getThermodynamicParametersPath();
    if (!thermodynamicParametersPath.isEmpty()) {
        auto primerSettings = settings->getPrimerSettings();
        thal_results o;
        if (thal_load_parameters(thermodynamicParametersPath.data(), &primerSettings->thermodynamic_parameters, &o) == -1) {
            stateInfo.setError(o.msg);
        }
    }
    CHECK_OP(stateInfo, );

    bool spanExonsEnabled = settings->getSpanIntronExonBoundarySettings().enabled;
    int toReturn = settings->getPrimerSettings()->num_return;
    if (spanExonsEnabled) {
        settings->getPrimerSettings()->num_return = settings->getSpanIntronExonBoundarySettings().maxPairsToQuery;  // not an optimal algorithm
    }

    // Run primer3 itself
    p3retval* resultPrimers = runPrimer3(settings->getPrimerSettings(), settings->getSeqArgs(), settings->isShowDebugging(), settings->isFormatOutput(), settings->isExplain());
    settings->setP3RetVal(resultPrimers);

    if (settings->getSpanIntronExonBoundarySettings().enabled) {
        if (settings->getSpanIntronExonBoundarySettings().overlapExonExonBoundary) {
            selectPairsSpanningExonJunction(resultPrimers, toReturn);
        } else {
            selectPairsSpanningIntron(resultPrimers, toReturn);
        }
    } else {
        // Process primer3 pair results to local @PrimerPair
        for (int index = 0; index < resultPrimers->best_pairs.num_pairs; index++) {
            bestPairs.append(QSharedPointer<PrimerPair>(new PrimerPair(resultPrimers->best_pairs.pairs[index], offset)));
        }
    }

    // We have not pair as result, but separate primers - process them from primer3 structs to local @PrimerSingle
    if (resultPrimers->output_type == primer_list) {
        int maxCount = 0;
        settings->getIntProperty("PRIMER_NUM_RETURN", &maxCount);
        if (resultPrimers->fwd.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->fwd.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(QSharedPointer<PrimerSingle>(new PrimerSingle(*(resultPrimers->fwd.oligo + i), oligo_type::OT_LEFT, offset)));
            }
        }
        if (resultPrimers->rev.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->rev.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(QSharedPointer<PrimerSingle>(new PrimerSingle(*(resultPrimers->rev.oligo + i), oligo_type::OT_RIGHT, offset)));
            }
        }
        if (resultPrimers->intl.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->intl.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(QSharedPointer<PrimerSingle>(new PrimerSingle(*(resultPrimers->intl.oligo + i), oligo_type::OT_INTL, offset)));
            }
        }
    }
}

Task::ReportResult Primer3Task::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    auto resultPrimers = settings->getP3RetVal();
    QString globalError;
    if (resultPrimers->glob_err.storage_size != 0) {
        globalError = tr("Global Primer3 error: \"%1\". ").arg(resultPrimers->glob_err.data);
    }
    QString sequenceError;
    if (resultPrimers->per_sequence_err.storage_size != 0) {
        sequenceError = tr("Sequence Primer3 error: \"%1\".").arg(resultPrimers->per_sequence_err.data);
    }
    if (!globalError.isEmpty() || !sequenceError.isEmpty()) {
        stateInfo.setError(globalError + sequenceError);
    }
    if (resultPrimers->warnings.storage_size != 0) {
        stateInfo.addWarning(resultPrimers->warnings.data);
    }

    return Task::ReportResult_Finished;
}

const QList<QSharedPointer<PrimerPair>>& Primer3Task::getBestPairs() const {
    return bestPairs;
}

const QList<QSharedPointer<PrimerSingle>>& Primer3Task::getSinglePrimers() const {
    return singlePrimers;
}

// TODO: reuse functions from U2Region!
static QList<int> findIntersectingRegions(const QList<U2Region>& regions, int start, int length) {
    QList<int> indexes;

    U2Region target(start, length);
    for (int i = 0; i < regions.size(); ++i) {
        const U2Region& r = regions.at(i);
        if (r.intersects(target)) {
            indexes.append(i);
        }
    }

    return indexes;
}

static bool pairIntersectsJunction(const primer_rec* primerRec, const QVector<qint64>& junctions, int minLeftOverlap, int minRightOverlap) {
    U2Region primerRegion(primerRec->start, primerRec->length);

    for (qint64 junctionPos : qAsConst(junctions)) {
        U2Region testRegion(junctionPos - minLeftOverlap, minLeftOverlap + minRightOverlap);
        if (primerRegion.contains(testRegion)) {
            return true;
        }
    }

    return false;
}

void Primer3Task::selectPairsSpanningExonJunction(p3retval* primers, int toReturn) {
    int minLeftOverlap = settings->getSpanIntronExonBoundarySettings().minLeftOverlap;
    int minRightOverlap = settings->getSpanIntronExonBoundarySettings().minRightOverlap;

    QVector<qint64> junctionPositions;
    const QList<U2Region>& regions = settings->getExonRegions();
    for (int i = 0; i < regions.size() - 1; ++i) {
        qint64 end = regions.at(i).endPos();
        junctionPositions.push_back(end);
    }

    for (int index = 0; index < primers->best_pairs.num_pairs; index++) {
        const primer_pair& pair = primers->best_pairs.pairs[index];
        const primer_rec* left = pair.left;
        const primer_rec* right = pair.right;

        if (pairIntersectsJunction(left, junctionPositions, minLeftOverlap, minRightOverlap) || pairIntersectsJunction(right, junctionPositions, minLeftOverlap, minRightOverlap)) {
            bestPairs.append(QSharedPointer<PrimerPair>(new PrimerPair(pair, offset)));
        }

        if (bestPairs.size() == toReturn) {
            break;
        }
    }
}

void Primer3Task::selectPairsSpanningIntron(p3retval* primers, int toReturn) {
    const QList<U2Region>& regions = settings->getExonRegions();

    for (int index = 0; index < primers->best_pairs.num_pairs; index++) {
        const primer_pair& pair = primers->best_pairs.pairs[index];
        const primer_rec* left = pair.left;
        const primer_rec* right = pair.right;

        QList<int> regionIndexes = findIntersectingRegions(regions, left->start, left->length);

        int numIntersecting = 0;
        U2Region rightRegion(right->start, right->length);
        for (int idx : qAsConst(regionIndexes)) {
            const U2Region& exonRegion = regions.at(idx);
            if (exonRegion.intersects(rightRegion)) {
                ++numIntersecting;
            }
        }

        if (numIntersecting != regionIndexes.length()) {
            bestPairs.append(QSharedPointer<PrimerPair>(new PrimerPair(pair, offset)));
        }

        if (bestPairs.size() == toReturn) {
            break;
        }
    }
}

}  // namespace U2
