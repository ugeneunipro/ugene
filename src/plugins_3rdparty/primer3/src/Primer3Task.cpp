/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/U1AnnotationUtils.h>

#include "Primer3Plugin.h"
#include "primer3_core/libprimer3.h"
#include "primer3_core/primer3_boulder_main.h"

namespace U2 {

// PrimerSingle

PrimerSingle::PrimerSingle(oligo_type type)
    : type(type) {
}

PrimerSingle::PrimerSingle(const primer_rec& primerRec, oligo_type _type, int offset)
    : start(primerRec.start + offset),
      length(primerRec.length),
      meltingTemperature(primerRec.temp),
      bound(primerRec.bound),
      gcContent(primerRec.gc_content),
      selfAny(primerRec.self_any),
      selfEnd(primerRec.self_end),
      templateMispriming(primerRec.template_mispriming),
      hairpin(primerRec.hairpin_th),
      endStability(primerRec.end_stability),
      quality(primerRec.quality),
      repeatSim(primerRec.repeat_sim.score != nullptr ? primerRec.repeat_sim.score[primerRec.repeat_sim.max] : 0),
      repeatSimName(primerRec.repeat_sim.name),
      selfAnyStruct(primerRec.self_any_struct),
      selfEndStruct(primerRec.self_end_struct),
      type(_type) {
    if (type == oligo_type::OT_RIGHT) {
        // Primer3 calculates all positions from 5' to 3' sequence ends -
        // from the left to the right in case of the direct sequence and from the right to the left in case of the reverse-complementary sequence
        start = start - length + 1;
    }
}

int PrimerSingle::getStart() const {
    return start;
}

int PrimerSingle::getLength() const {
    return length;
}

double PrimerSingle::getMeltingTemperature() const {
    return meltingTemperature;
}

double PrimerSingle::getBound() const {
    return bound;
}

double PrimerSingle::getGcContent() const {
    return gcContent;
}

double PrimerSingle::getSelfAny() const {
    return selfAny;
}

double PrimerSingle::getSelfEnd() const {
    return selfEnd;
}

double PrimerSingle::getTemplateMispriming() const {
    return templateMispriming;
}

double PrimerSingle::getHairpin() const {
    return hairpin;
}

double PrimerSingle::getEndStability() const {
    return endStability;
}

double PrimerSingle::getQuality() const {
    return quality;
}

double PrimerSingle::getRepeatSim() const {
    return repeatSim;
}

const QString& PrimerSingle::getRepeatSimName() const {
    return repeatSimName;
}

const QString& PrimerSingle::getSelfAnyStruct() const {
    return selfAnyStruct;
}

const QString& PrimerSingle::getSelfEndStruct() const {
    return selfEndStruct;
}

oligo_type PrimerSingle::getType() const {
    return type;
}

void PrimerSingle::setStart(int start) {
    this->start = start;
}

void PrimerSingle::setLength(int length) {
    this->length = length;
}

void PrimerSingle::setMeltingTemperature(double meltingTemperature) {
    this->meltingTemperature = meltingTemperature;
}

void PrimerSingle::setBound(double bound) {
    this->bound = bound;
}

void PrimerSingle::setGcContent(double gcContent) {
    this->gcContent = gcContent;
}

void PrimerSingle::setSelfAny(double selfAny) {
    this->selfAny = selfAny;
}

void PrimerSingle::setSelfEnd(double selfEnd) {
    this->selfEnd = selfEnd;
}

void PrimerSingle::setTemplateMispriming(double templateMispriming) {
    this->templateMispriming = templateMispriming;
}

void PrimerSingle::setHairpin(double hairpin) {
    this->hairpin = hairpin;
}

void PrimerSingle::setEndStability(double endStability) {
    this->endStability = endStability;
}

void PrimerSingle::setQuality(double quality) {
    this->quality = quality;
}

void PrimerSingle::setRepeatSim(double repeatSim) {
    this->repeatSim = repeatSim;
}

void PrimerSingle::setRepeatSimName(const QString& repeatSimName) {
    this->repeatSimName = repeatSimName;
}

void PrimerSingle::setSelfAnyStruct(const QString& selfAnyStruct) {
    this->selfAnyStruct = selfAnyStruct;
}

void PrimerSingle::setSelfEndStruct(const QString& selfEndStruct) {
    this->selfEndStruct = selfEndStruct;
}

// PrimerPair
PrimerPair::PrimerPair(const primer_pair& primerPair, int offset)
    : leftPrimer((primerPair.left == nullptr) ? nullptr : new PrimerSingle(*primerPair.left, oligo_type::OT_LEFT, offset)),
      rightPrimer((primerPair.right == nullptr) ? nullptr : new PrimerSingle(*primerPair.right, oligo_type::OT_RIGHT, offset)),
      internalOligo((primerPair.intl == nullptr) ? nullptr : new PrimerSingle(*primerPair.intl, oligo_type::OT_INTL, offset)),
      complAny(primerPair.compl_any),
      complEnd(primerPair.compl_end),
      productSize(primerPair.product_size),
      quality(primerPair.pair_quality),
      tm(primerPair.product_tm),
      repeatSim(primerPair.repeat_sim),
      repeatSimName(primerPair.rep_name),
      complAnyStruct(primerPair.compl_any_struct),
      complEndStruct(primerPair.compl_end_struct) {
}

PrimerPair::PrimerPair(const PrimerPair& primerPair)
    : leftPrimer((primerPair.leftPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.leftPrimer)),
      rightPrimer((primerPair.rightPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.rightPrimer)),
      internalOligo((primerPair.internalOligo.isNull()) ? nullptr : new PrimerSingle(*primerPair.internalOligo)),
      complAny(primerPair.complAny),
      complEnd(primerPair.complEnd),
      productSize(primerPair.productSize),
      quality(primerPair.quality),
      tm(primerPair.tm),
      repeatSim(primerPair.repeatSim),
      repeatSimName(primerPair.repeatSimName),
      complAnyStruct(primerPair.complAnyStruct),
      complEndStruct(primerPair.complEndStruct) {
}

PrimerPair& PrimerPair::operator=(const PrimerPair& primerPair) {
    leftPrimer.reset((primerPair.leftPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.leftPrimer));
    rightPrimer.reset((primerPair.rightPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.rightPrimer));
    internalOligo.reset((primerPair.internalOligo.isNull()) ? nullptr : new PrimerSingle(*primerPair.internalOligo));
    complAny = primerPair.complAny;
    complEnd = primerPair.complEnd;
    productSize = primerPair.productSize;
    quality = primerPair.quality;
    tm = primerPair.tm;
    repeatSim = primerPair.repeatSim;
    repeatSimName = primerPair.repeatSimName;
    complAnyStruct = primerPair.complAnyStruct;
    complEndStruct = primerPair.complEndStruct;

    return *this;
}

PrimerSingle* PrimerPair::getLeftPrimer() const {
    return leftPrimer.data();
}

PrimerSingle* PrimerPair::getRightPrimer() const {
    return rightPrimer.data();
}

PrimerSingle* PrimerPair::getInternalOligo() const {
    return internalOligo.data();
}

double PrimerPair::getComplAny() const {
    return complAny;
}

double PrimerPair::getComplEnd() const {
    return complEnd;
}

int PrimerPair::getProductSize() const {
    return productSize;
}

double PrimerPair::getProductQuality() const {
    return quality;
}

double PrimerPair::getProductTm() const {
    return tm;
}

void PrimerPair::setLeftPrimer(PrimerSingle* leftPrimer) {
    this->leftPrimer.reset((leftPrimer == nullptr) ? nullptr : new PrimerSingle(*leftPrimer));
}

void PrimerPair::setRightPrimer(PrimerSingle* rightPrimer) {
    this->rightPrimer.reset((rightPrimer == nullptr) ? nullptr : new PrimerSingle(*rightPrimer));
}

void PrimerPair::setInternalOligo(PrimerSingle* internalOligo) {
    this->internalOligo.reset((internalOligo == nullptr) ? nullptr : new PrimerSingle(*internalOligo));
}

void PrimerPair::setComplAny(double newComplAny) {
    complAny = newComplAny;
}

void PrimerPair::setComplEnd(double newComplEnd) {
    complEnd = newComplEnd;
}

void PrimerPair::setProductSize(int newProductSize) {
    productSize = newProductSize;
}

double PrimerPair::getRepeatSim() const {
    return repeatSim;
}

void PrimerPair::setProductQuality(double quality) {
    this->quality = quality;
}

const QString& PrimerPair::getRepeatSimName() const {
    return repeatSimName;
}

void PrimerPair::setProductTm(double tm) {
    this->tm = tm;
}

const QString& PrimerPair::getComplAnyStruct() const {
    return complAnyStruct;
}

const QString& PrimerPair::getComplEndStruct() const {
    return complEndStruct;
}

void PrimerPair::setRepeatSim(double repeatSim) {
    this->repeatSim = repeatSim;
}

void PrimerPair::setRepeatSimName(const QString& repeatSimName) {
    this->repeatSimName = repeatSimName;
}

void PrimerPair::setComplAnyStruct(const QString& complAnyStruct) {
    this->complAnyStruct = complAnyStruct;
}

void PrimerPair::setComplEndStruct(const QString& complEndStruct) {
    this->complEndStruct = complEndStruct;
}

bool PrimerPair::operator<(const PrimerPair& pair) const {
    if (quality < pair.quality) {
        return true;
    }
    if (quality > pair.quality) {
        return false;
    }
    if (tm < pair.tm) {
        return true;
    }
    if (tm > pair.tm) {
        return false;
    }
    if (leftPrimer->getStart() > pair.leftPrimer->getStart()) {
        return true;
    }
    if (leftPrimer->getStart() < pair.leftPrimer->getStart()) {
        return false;
    }

    if (rightPrimer->getStart() < pair.rightPrimer->getStart()) {
        return true;
    }
    if (rightPrimer->getStart() > pair.rightPrimer->getStart()) {
        return false;
    }

    if (leftPrimer->getLength() < pair.leftPrimer->getLength()) {
        return true;
    }
    if (leftPrimer->getLength() > pair.leftPrimer->getLength()) {
        return false;
    }

    if (rightPrimer->getLength() < pair.rightPrimer->getLength()) {
        return true;
    }
    if (rightPrimer->getLength() > pair.rightPrimer->getLength()) {
        return false;
    }

    return false;
}

// Primer3Task

Primer3Task::Primer3Task(Primer3TaskSettings* _settings)
    : Task(tr("Pick primers task"), TaskFlag_ReportingIsEnabled),
      settings(_settings) {
    GCOUNTER(cvar, "Primer3Task");

    const auto& sequenceRange = settings->getSequenceRange();
    const auto& includedRegion = settings->getIncludedRegion();
    int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - settings->getFirstBaseIndex() : 0;
    offset = sequenceRange.startPos + includedRegionOffset;

    settings->setSequence(settings->getSequence().mid(sequenceRange.startPos, sequenceRange.length));
    settings->setSequenceQuality(settings->getSequenceQuality().mid(sequenceRange.startPos, sequenceRange.length));

    // Primer3Task is single threaded: the original "primer3" tool doesn't support parallel calculations.
    addTaskResource(TaskResourceUsage(AppResource::buildDynamicResourceId("Primer 3 single thread"), 1, TaskResourceStage::Run));
}

void Primer3Task::run() {
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

    QByteArray thermodynamicParametersPath = settings->getThermodynamicParametersPath();
    if (!thermodynamicParametersPath.isEmpty()) {
        auto primerSettings = settings->getPrimerSettings();
        char* path = thermodynamicParametersPath.data();
        if (path[strlen(path) - 1] == '\n') {
            path[strlen(path) - 1] = '\0';
        }
        thal_results o;
        if (thal_load_parameters(path, &primerSettings->thermodynamic_parameters, &o) == -1) {
            stateInfo.setError(o.msg);
        }
    }
    CHECK_OP(stateInfo, );

    bool spanExonsEnabled = settings->getSpanIntronExonBoundarySettings().enabled;
    int toReturn = settings->getPrimerSettings()->num_return;
    if (spanExonsEnabled) {
        settings->getPrimerSettings()->num_return = settings->getSpanIntronExonBoundarySettings().maxPairsToQuery;  // not an optimal algorithm
    }

    p3retval* resultPrimers = runPrimer3(settings->getPrimerSettings(), settings->getSeqArgs(), settings->isShowDebugging(), settings->isFormatOutput(), settings->isExplain());
    settings->setP3RetVal(resultPrimers);

    bestPairs.clear();
    if (settings->getSpanIntronExonBoundarySettings().enabled) {
        if (settings->getSpanIntronExonBoundarySettings().overlapExonExonBoundary) {
            selectPairsSpanningExonJunction(resultPrimers, toReturn);
        } else {
            selectPairsSpanningIntron(resultPrimers, toReturn);
        }
    } else {
        for (int index = 0; index < resultPrimers->best_pairs.num_pairs; index++) {
            bestPairs.append(PrimerPair(resultPrimers->best_pairs.pairs[index], offset));
        }
    }

    if (resultPrimers->output_type == primer_list) {
        singlePrimers.clear();
        int maxCount = 0;
        settings->getIntProperty("PRIMER_NUM_RETURN", &maxCount);
        if (resultPrimers->fwd.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->fwd.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(PrimerSingle(*(resultPrimers->fwd.oligo + i), oligo_type::OT_LEFT, offset));
            }
        }
        if (resultPrimers->rev.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->rev.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(PrimerSingle(*(resultPrimers->rev.oligo + i), oligo_type::OT_RIGHT, offset));
            }
        }
        if (resultPrimers->intl.oligo != nullptr) {
            for (int i = 0; i < resultPrimers->intl.expl.ok && i < maxCount; ++i) {
                singlePrimers.append(PrimerSingle(*(resultPrimers->intl.oligo + i), oligo_type::OT_INTL, offset));
            }
        }
    }
}

Task::ReportResult Primer3Task::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

    auto resultPrimers = settings->getP3RetVal();
    if (resultPrimers->glob_err.storage_size != 0) {
        stateInfo.setError(resultPrimers->glob_err.data);
    }
    if (resultPrimers->per_sequence_err.storage_size != 0) {
        stateInfo.setError(resultPrimers->per_sequence_err.data);
    }
    if (resultPrimers->warnings.storage_size != 0) {
        stateInfo.addWarning(resultPrimers->warnings.data);
    }

    return Task::ReportResult_Finished;
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

    for (qint64 junctionPos : junctions) {
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
            bestPairs.append(PrimerPair(pair, offset));
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
        for (int idx : regionIndexes) {
            const U2Region& exonRegion = regions.at(idx);
            if (exonRegion.intersects(rightRegion)) {
                ++numIntersecting;
            }
        }

        if (numIntersecting != regionIndexes.length()) {
            bestPairs.append(PrimerPair(pair, offset));
        }

        if (bestPairs.size() == toReturn) {
            break;
        }
    }
}

// Primer3SWTask

Primer3SWTask::Primer3SWTask(Primer3TaskSettings* _settings, bool _ownsSettings)
    : Task("Pick primers SW task", TaskFlags_NR_FOSCOE | TaskFlag_CollectChildrenWarnings),
      settings(_settings),
      ownsSettings(_ownsSettings) {
    median = settings->getSequenceSize() / 2;
}

Primer3SWTask::~Primer3SWTask() {
    if (ownsSettings) {
        delete settings;
    }
}

void Primer3SWTask::prepare() {
    // selected region covers circular junction
    const auto& sequenceRange = settings->getSequenceRange();
    int sequenceSize = settings->getSequenceSize();

    const auto& includedRegion = settings->getIncludedRegion();
    int fbs = settings->getFirstBaseIndex();
    int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - fbs : 0;
    CHECK_EXT(includedRegionOffset >= 0, stateInfo.setError(tr("Incorrect sum \"Included Region Start + First Base Index\" - should be more or equal than 0")), );

    if (sequenceRange.endPos() > sequenceSize + includedRegionOffset) {
        SAFE_POINT_EXT(settings->isSequenceCircular(), stateInfo.setError("Unexpected region, sequence should be circular"), );

        QByteArray seq = settings->getSequence();
        seq.append(seq.left(sequenceRange.endPos() - sequenceSize - fbs));
        settings->setSequence(seq);
    }

    primer3Task = new Primer3Task(settings);
    addSubTask(primer3Task);
}

Task::ReportResult Primer3SWTask::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);
    CHECK(primer3Task != nullptr, Task::ReportResult_Finished);

    bestPairs.append(primer3Task->getBestPairs());
    singlePrimers.append(primer3Task->getSinglePrimers());

    return Task::ReportResult_Finished;
}

void Primer3SWTask::relocatePrimerOverMedian(PrimerSingle* primer) {
    primer->setStart(primer->getStart() + (primer->getStart() >= median ? -median : settings->getSequenceSize() - median));
}

//////////////////////////////////////////////////////////////////////////
////Primer3ToAnnotationsTask

Primer3ToAnnotationsTask::Primer3ToAnnotationsTask(Primer3TaskSettings* _settings, U2SequenceObject* so_, AnnotationTableObject* aobj_, const QString& groupName_, const QString& annName_, const QString& annDescription)
    : Task(tr("Search primers to annotations"), TaskFlags(TaskFlag_NoRun) | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled | TaskFlag_FailOnSubtaskError),
      settings(_settings), annotationTableObject(aobj_), seqObj(so_),
      groupName(groupName_), annName(annName_), annDescription(annDescription), searchTask(nullptr), findExonsTask(nullptr) {
}

Primer3ToAnnotationsTask::~Primer3ToAnnotationsTask() {
    delete settings;
}

void Primer3ToAnnotationsTask::prepare() {
    if (settings->getSpanIntronExonBoundarySettings().enabled) {
        findExonsTask = new FindExonRegionsTask(seqObj, settings->getSpanIntronExonBoundarySettings().exonAnnotationName);
        addSubTask(findExonsTask);
    } else {
        searchTask = new Primer3SWTask(settings);
        addSubTask(searchTask);
    }
}

QList<Task*> Primer3ToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (isCanceled() || hasError()) {
        return res;
    }

    if (!subTask->isFinished()) {
        return res;
    }

    if (subTask == findExonsTask) {
        QList<U2Region> regions = findExonsTask->getRegions();
        if (regions.isEmpty()) {
            setError(tr("Failed to find any exon annotations associated with the sequence %1."
                        "Make sure the provided sequence is cDNA and has exonic structure annotated")
                         .arg(seqObj->getSequenceName()));
            return res;
        } else {
            const U2Range<int>& exonRange = settings->getSpanIntronExonBoundarySettings().exonRange;

            if (exonRange.minValue != 0 && exonRange.maxValue != 0) {
                int firstExonIdx = exonRange.minValue;
                int lastExonIdx = exonRange.maxValue;
                if (firstExonIdx > regions.size()) {
                    setError(tr("The first exon from the selected range [%1,%2] is larger the number of exons (%3)."
                                " Please set correct exon range.")
                                 .arg(firstExonIdx)
                                 .arg(lastExonIdx)
                                 .arg(regions.size()));
                    return res;
                }

                if (lastExonIdx > regions.size()) {
                    setError(tr("The the selected exon range [%1,%2] is larger the number of exons (%3)."
                                " Please set correct exon range.")
                                 .arg(firstExonIdx)
                                 .arg(lastExonIdx)
                                 .arg(regions.size()));
                    return res;
                }

                regions = regions.mid(firstExonIdx - 1, lastExonIdx - firstExonIdx + 1);
                int totalLen = 0;
                for (const U2Region& r : regions) {
                    totalLen += r.length;
                }
                settings->setIncludedRegion(regions.first().startPos + settings->getFirstBaseIndex(), totalLen);
            }
            settings->setExonRegions(regions);
            // reset target and excluded regions regions
            QList<U2Region> emptyList;
            settings->setExcludedRegion(emptyList);
            settings->setTarget(emptyList);
        }

        searchTask = new Primer3SWTask(settings);
        res.append(searchTask);
    }

    return res;
}

QString Primer3ToAnnotationsTask::generateReport() const {
    QString res;

    if (hasError() || isCanceled()) {
        return res;
    }

    oligo_stats leftStats = settings->getP3RetVal()->fwd.expl;
    oligo_stats rightStats = settings->getP3RetVal()->rev.expl;
    pair_stats pairStats = settings->getP3RetVal()->best_pairs.expl;

    if (!leftStats.considered) {
        leftStats.considered = leftStats.ns + leftStats.target + leftStats.excluded + leftStats.gc + leftStats.gc_clamp + leftStats.temp_min + leftStats.temp_max + leftStats.compl_any + leftStats.compl_end + leftStats.poly_x + leftStats.stability + leftStats.ok;
    }

    if (!rightStats.considered) {
        rightStats.considered = rightStats.ns + rightStats.target + rightStats.excluded + rightStats.gc + rightStats.gc_clamp + rightStats.temp_min + rightStats.temp_max + rightStats.compl_any + rightStats.compl_end + rightStats.poly_x + rightStats.stability + rightStats.ok;
    }

    res += "<table cellspacing='7'>";
    res += "<tr><th>Statistics</th></tr>\n";

    res += QString("<tr><th></th> <th>con</th> <th>too</th> <th>in</th> <th>in</th> <th></th> <th>no</th> <th>tm</th> <th>tm</th> <th>high</th> <th>high</th> <th></th> <th>high</th> <th></th></tr>");
    res += QString("<tr><th></th> <th>sid</th> <th>many</th> <th>tar</th> <th>excl</th> <th>bad</th> <th>GC</th> <th>too</th> <th>too</th> <th>any</th> <th>3'</th> <th>poly</th> <th>end</th> <th></th></tr>");
    res += QString("<tr><th></th> <th>ered</th> <th>Ns</th> <th>get</th> <th>reg</th> <th>GC&#37;</th> <th>clamp</th> <th>low</th> <th>high</th> <th>compl</th> <th>compl</th> <th>X</th> <th>stab</th> <th>ok</th></tr>");

    res += QString("<tr><th>Left</th><th> %1 </th><th> %2 </th><th> %3 </th><th> %4 </th><th> %5 </th><th> %6 </th><th> %7 </th><th> %8 </th><th> %9 </th><th> %10 </th><th> %11 </th><th> %12 </th><th> %13 </th></tr>")
               .arg(leftStats.considered)
               .arg(leftStats.ns)
               .arg(leftStats.target)
               .arg(leftStats.excluded)
               .arg(leftStats.gc)
               .arg(leftStats.gc_clamp)
               .arg(leftStats.temp_min)
               .arg(leftStats.temp_max)
               .arg(leftStats.compl_any)
               .arg(leftStats.compl_end)
               .arg(leftStats.poly_x)
               .arg(leftStats.stability)
               .arg(leftStats.ok);
    res += QString("<tr><th>Right</th><th> %1 </th><th> %2 </th><th> %3 </th><th> %4 </th><th> %5 </th><th> %6 </th><th> %7 </th><th> %8 </th><th> %9 </th><th> %10 </th><th> %11 </th><th> %12 </th><th> %13 </th></tr>")
               .arg(rightStats.considered)
               .arg(rightStats.ns)
               .arg(rightStats.target)
               .arg(rightStats.excluded)
               .arg(rightStats.gc)
               .arg(rightStats.gc_clamp)
               .arg(rightStats.temp_min)
               .arg(rightStats.temp_max)
               .arg(rightStats.compl_any)
               .arg(rightStats.compl_end)
               .arg(rightStats.poly_x)
               .arg(rightStats.stability)
               .arg(rightStats.ok);
    res += "</table>";
    res += "<br>Pair stats:<br>";
    res += QString("considered %1, unacceptable product size %2, high end compl %3, ok %4.")
               .arg(pairStats.considered)
               .arg(pairStats.product)
               .arg(pairStats.compl_end)
               .arg(pairStats.ok);

    return res;
}

Task::ReportResult Primer3ToAnnotationsTask::report() {
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }
    CHECK_EXT(!annotationTableObject.isNull(), setError(tr("Object with annotations was removed")), ReportResult_Finished);
    SAFE_POINT(searchTask != nullptr, L10N::nullPointerError("Primer3Task"), ReportResult_Finished);

    const QList<PrimerPair>& bestPairs = searchTask->getBestPairs();
    QMap<QString, QList<SharedAnnotationData>> resultAnnotations;
    int index = 0;
    for (const PrimerPair& pair : bestPairs) {
        QList<SharedAnnotationData> annotations;
        if (pair.getLeftPrimer() != nullptr) {
            annotations.append(oligoToAnnotation(annName, *pair.getLeftPrimer(), pair.getProductSize(), U2Strand::Direct));
        }
        if (pair.getInternalOligo() != nullptr) {
            annotations.append(oligoToAnnotation("internalOligo", *pair.getInternalOligo(), pair.getProductSize(), U2Strand::Direct));
        }
        if (pair.getRightPrimer() != nullptr) {
            annotations.append(oligoToAnnotation(annName, *pair.getRightPrimer(), pair.getProductSize(), U2Strand::Complementary));
        }
        resultAnnotations[groupName + "/pair " + QString::number(index + 1)].append(annotations);
        index++;
    }

    const auto& singlePrimers = searchTask->getSinglePrimers();
    if (!singlePrimers.isEmpty()) {
        QList<SharedAnnotationData> annotations;
        for (const auto& primer : singlePrimers) {
            auto type = primer.getType();
            U2Strand s = type == OT_RIGHT ? U2Strand::Complementary : U2Strand::Direct;
            QString annotationName = type == OT_INTL ? "internalOligo" : annName;
            annotations.append(oligoToAnnotation(annotationName, primer, 0, s));
        }
        U1AnnotationUtils::addDescriptionQualifier(annotations, annDescription);

        if (!annotations.isEmpty()) {
            resultAnnotations[groupName].append(annotations);
        }
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(new CreateAnnotationsTask(annotationTableObject, resultAnnotations));

    return ReportResult_Finished;
}

SharedAnnotationData Primer3ToAnnotationsTask::oligoToAnnotation(const QString& title, const PrimerSingle& primer, int productSize, U2Strand strand) {
    SharedAnnotationData annotationData(new AnnotationData);
    annotationData->name = title;
    annotationData->type = U2FeatureTypes::Primer;
    qint64 seqLen = seqObj->getSequenceLength();
    // primer can be found on circular extension of the sequence
    int start = primer.getStart() + (primer.getStart() > seqLen ? (-seqLen) : 0);
    int length = primer.getLength();
    if (start + length <= seqLen) {
        annotationData->location->regions << U2Region(start, length);
    } else {
        // primer covers circular junction
        annotationData->location->regions << U2Region(start, seqLen - start) << U2Region(0, start + length - seqLen);
        annotationData->location.data()->op = U2LocationOperator_Join;
    }

    annotationData->setStrand(strand);

    annotationData->qualifiers.append(U2Qualifier("product_size", QString::number(productSize)));
    annotationData->qualifiers.append(U2Qualifier("tm", QString::number(primer.getMeltingTemperature())));
    annotationData->qualifiers.append(U2Qualifier("gc%", QString::number(primer.getGcContent())));
    annotationData->qualifiers.append(U2Qualifier("any", QString::number(primer.getSelfAny())));
    annotationData->qualifiers.append(U2Qualifier("end", QString::number(primer.getSelfEnd())));
    annotationData->qualifiers.append(U2Qualifier("3'", QString::number(primer.getEndStability())));
    annotationData->qualifiers.append(U2Qualifier("penalty'", QString::number(primer.getQuality())));

    auto areDoubleValuesEqual = [](double val, double reference) -> bool {
        return qAbs(reference - val) > 0.1;
    };
    if (areDoubleValuesEqual(primer.getBound(), OLIGOTM_ERROR)) {
        annotationData->qualifiers.append(U2Qualifier("bound%", QString::number(primer.getBound())));
    }
    if (areDoubleValuesEqual(primer.getTemplateMispriming(), ALIGN_SCORE_UNDEF)) {
        annotationData->qualifiers.append(U2Qualifier("template_mispriming", QString::number(primer.getTemplateMispriming())));
    }
    if (areDoubleValuesEqual(primer.getHairpin(), ALIGN_SCORE_UNDEF)) {
        annotationData->qualifiers.append(U2Qualifier("hairpin", QString::number(primer.getHairpin())));
    }

    return annotationData;
}

}  // namespace U2
