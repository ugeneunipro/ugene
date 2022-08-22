/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <cstdlib>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MultiTask.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/U1AnnotationUtils.h>

#include "Primer3Plugin.h"
#include "Primer3Task.h"

#include "primer3_core/primer3_boulder_main.h"
#include "primer3_core/print_boulder.h"
#include "primer3_core/p3_seq_lib.h"
#include "primer3_core/libprimer3.h"

namespace U2 {

// PrimerSingle

PrimerSingle::PrimerSingle(oligo_type type)
    : start(0),
      length(0),
      meltingTemperature(0),
      gcContent(0),
      selfAny(0),
      selfEnd(0),
      hairpin(0.0),
      endStability(0),
      quality(0),
      repeatSim(0),
      type(type) {
}

PrimerSingle::PrimerSingle(const primer_rec& primerRec, oligo_type _type, int offset)
    : start(primerRec.start + offset),
      length(primerRec.length),
      meltingTemperature(primerRec.temp),
      gcContent(primerRec.gc_content),
      selfAny(primerRec.self_any),
      selfEnd(primerRec.self_end),
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

bool PrimerSingle::operator==(const PrimerSingle& p) const {
    bool result = true;

    result &= start == p.start;
    result &= length == p.length;
    result &= meltingTemperature == p.meltingTemperature;
    result &= gcContent == p.gcContent;
    result &= selfAny == p.selfAny;
    result &= selfEnd == p.selfEnd;
    result &= hairpin == p.hairpin;
    result &= endStability == p.endStability;
    result &= quality == p.quality;
    result &= repeatSim == p.repeatSim;
    result &= repeatSimName == p.repeatSimName;
    result &= selfAnyStruct == p.selfAnyStruct;
    result &= selfEndStruct == p.selfEndStruct;
    result &= type == p.type;

    return result;
}

bool PrimerSingle::areEqual(const PrimerSingle* p1, const PrimerSingle* p2) {
    if (p1 != nullptr && p2 != nullptr) {
        return (*p1 == *p2);
    } else {
        return (p1 == p2);
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

double PrimerSingle::getGcContent() const {
    return gcContent;
}

double PrimerSingle::getSelfAny() const {
    return selfAny;
}

double PrimerSingle::getSelfEnd() const {
    return selfEnd;
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

void PrimerSingle::setGcContent(double gcContent) {
    this->gcContent = gcContent;
}

void PrimerSingle::setSelfAny(double selfAny) {
    this->selfAny = selfAny;
}

void PrimerSingle::setSelfEnd(double selfEnd) {
    this->selfEnd = selfEnd;
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

PrimerPair::PrimerPair()
    : leftPrimer(nullptr),
      rightPrimer(nullptr),
      internalOligo(nullptr),
      complAny(0),
      complEnd(0),
      productSize(0),
      quality(0),
      tm(0),
      repeatSim(0) {
}

PrimerPair::PrimerPair(const primer_pair& primerPair, int offset)
    : leftPrimer((nullptr == primerPair.left) ? nullptr : new PrimerSingle(*primerPair.left, oligo_type::OT_LEFT, offset)),
      rightPrimer((nullptr == primerPair.right) ? nullptr : new PrimerSingle(*primerPair.right, oligo_type::OT_RIGHT, offset)),
      internalOligo((nullptr == primerPair.intl) ? nullptr : new PrimerSingle(*primerPair.intl, oligo_type::OT_INTL, offset)),
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

bool PrimerPair::operator==(const PrimerPair& primerPair) const {
    bool result = true;

    result &= PrimerSingle::areEqual(leftPrimer.data(), primerPair.leftPrimer.data());
    result &= PrimerSingle::areEqual(rightPrimer.data(), primerPair.rightPrimer.data());
    result &= PrimerSingle::areEqual(internalOligo.data(), primerPair.internalOligo.data());

    result &= complAny == primerPair.complAny;
    result &= complEnd == primerPair.complEnd;
    result &= productSize == primerPair.productSize;
    result &= quality == primerPair.quality;
    result &= tm == primerPair.tm;
    result &= repeatSim == primerPair.repeatSim;
    result &= repeatSimName == primerPair.repeatSimName;
    result &= complAnyStruct == primerPair.complAnyStruct;
    result &= complEndStruct == primerPair.complEndStruct;

    return result;
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
    this->leftPrimer.reset((nullptr == leftPrimer) ? nullptr : new PrimerSingle(*leftPrimer));
}

void PrimerPair::setRightPrimer(PrimerSingle* rightPrimer) {
    this->rightPrimer.reset((nullptr == rightPrimer) ? nullptr : new PrimerSingle(*rightPrimer));
}

void PrimerPair::setInternalOligo(PrimerSingle* internalOligo) {
    this->internalOligo.reset((nullptr == internalOligo) ? nullptr : new PrimerSingle(*internalOligo));
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

namespace {
bool clipRegion(U2Region& region, const U2Region& clippingRegion) {
    qint64 start = qMax(region.startPos, clippingRegion.startPos);
    qint64 end = qMin(region.endPos(), clippingRegion.endPos());
    if (start > end) {
        return false;
    }
    region.startPos = start;
    region.length = end - start;
    return true;
}
}  // namespace

Primer3Task::Primer3Task(Primer3TaskSettings* _settings)
    : Task(tr("Pick primers task"), TaskFlag_ReportingIsEnabled),
      settings(_settings) {
    GCOUNTER(cvar, "Primer3Task");

    /*{
        U2Region region = settings.getSequenceRange();
        region.startPos -= settings.getFirstBaseIndex();
        settings.setIncludedRegion(region);
    }*/
    const auto& sequenceRange = settings->getSequenceRange();
    const auto& includedRegion = settings->getIncludedRegion();
    int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - settings->getFirstBaseIndex() : 0;
    offset = sequenceRange.startPos + includedRegionOffset/*includedRegion.startPos + settings.getFirstBaseIndex()*/;

    settings->setSequence(settings->getSequence().mid(sequenceRange.startPos, sequenceRange.length));
    settings->setSequenceQuality(settings->getSequenceQuality().mid(sequenceRange.startPos, sequenceRange.length));

    addTaskResource(TaskResourceUsage(PRIMER3_STATIC_LOCK, 1));

    //settings.setIncludedRegion(0, sequenceRange.length);

    /*if (!PR_START_CODON_POS_IS_NULL(settings.getSeqArgs())) {
        int startCodonPosition = PR_DEFAULT_START_CODON_POS;
        if (settings.getIntProperty("PRIMER_START_CODON_POSITION", &startCodonPosition)) {
            settings.setIntProperty("PRIMER_START_CODON_POSITION",
                                    startCodonPosition - settings.getFirstBaseIndex());
        }
    }
    {
        QList<U2Region> regionList;
        for (U2Region region : settings.getTarget()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if (clipRegion(region, settings.getSequenceRange())) {
                regionList.append(region);
            }
        }
        settings.setTarget(regionList);
    }
    {
        QList<U2Region> regionList;
        for (int pos : settings.getOverlapJunctionList()) {
            pos -= settings.getFirstBaseIndex();
            pos -= offset;
            if (clipRegion(region, settings.getSequenceRange())) {
                regionList.append(region);
            }
        }
        settings.setTarget(regionList);
    }
    {
        QList<U2Region> regionList;
        foreach (U2Region region, settings.getExcludedRegion()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if (clipRegion(region, settings.getSequenceRange())) {
                regionList.append(region);
            }
        }
        settings.setExcludedRegion(regionList);
    }
    {
        QList<U2Region> regionList;
        foreach (U2Region region, settings.getInternalOligoExcludedRegion()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if (clipRegion(region, settings.getSequenceRange())) {
                regionList.append(region);
            }
        }
        settings.setInternalOligoExcludedRegion(regionList);
    }*/
}

void Primer3Task::run() {
    coreLog.details(QString("Test %1, start").arg(settings->getSeqArgs()->sequence_name));
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
        } /*else {
            if (get_thermodynamic_values(&primerSettings->thermodynamic_parameters, &o)) {
                stateInfo.setError(o.msg);
            }
        }*/
    }
    CHECK_OP(stateInfo, );

    bool spanExonsEnabled = settings->getSpanIntronExonBoundarySettings().enabled;
    int toReturn = settings->getPrimerSettings()->num_return;
    if (spanExonsEnabled) {
        settings->getPrimerSettings()->num_return = settings->getSpanIntronExonBoundarySettings().maxPairsToQuery;  // not an optimal algorithm
    }
    
    resultPrimers = runPrimer3(settings->getPrimerSettings(), settings->getSeqArgs(), settings->isShowDebugging(), settings->isFormatOutput(), settings->isExplain());

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

    /*bool noPairsWereFound = bestPairs.isEmpty();
    bool genericTask = settings.getTask() == task::generic;
    int pickLeft = 0;
    settings.getIntProperty("PRIMER_PICK_LEFT_PRIMER", &pickLeft);
    int pickRight = 0;
    settings.getIntProperty("PRIMER_PICK_RIGHT_PRIMER", &pickRight);
    bool noPairsIntended2BeFound = !(bool(pickLeft) && bool(pickRight));
    bool getSinglePairs = genericTask && noPairsIntended2BeFound || !genericTask && noPairsWereFound;*/
    if (resultPrimers->output_type == primer_list/* || resultPrimers->intl.expl.ok*/) {
    //if (getSinglePairs) {
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
    coreLog.details(QString("Test %1, end").arg(settings->getSeqArgs()->sequence_name));
}

Task::ReportResult Primer3Task::report() {
    CHECK_OP(stateInfo, Task::ReportResult_Finished);

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

void Primer3Task::sumStat(Primer3TaskSettings* st) {
    st->getP3RetVal()->fwd.expl.considered += resultPrimers->fwd.expl.considered;
    st->getP3RetVal()->fwd.expl.ns += resultPrimers->fwd.expl.ns;
    st->getP3RetVal()->fwd.expl.target += resultPrimers->fwd.expl.target;
    st->getP3RetVal()->fwd.expl.excluded += resultPrimers->fwd.expl.excluded;
    st->getP3RetVal()->fwd.expl.gc += resultPrimers->fwd.expl.gc;
    st->getP3RetVal()->fwd.expl.gc_clamp += resultPrimers->fwd.expl.gc_clamp;
    st->getP3RetVal()->fwd.expl.temp_min += resultPrimers->fwd.expl.temp_min;
    st->getP3RetVal()->fwd.expl.temp_max += resultPrimers->fwd.expl.temp_max;
    st->getP3RetVal()->fwd.expl.compl_any += resultPrimers->fwd.expl.compl_any;
    st->getP3RetVal()->fwd.expl.compl_end += resultPrimers->fwd.expl.compl_end;
    st->getP3RetVal()->fwd.expl.poly_x += resultPrimers->fwd.expl.poly_x;
    st->getP3RetVal()->fwd.expl.stability += resultPrimers->fwd.expl.stability;
    st->getP3RetVal()->fwd.expl.ok += resultPrimers->fwd.expl.ok;

    st->getP3RetVal()->rev.expl.considered += resultPrimers->rev.expl.considered;
    st->getP3RetVal()->rev.expl.ns += resultPrimers->rev.expl.ns;
    st->getP3RetVal()->rev.expl.target += resultPrimers->rev.expl.target;
    st->getP3RetVal()->rev.expl.excluded += resultPrimers->rev.expl.excluded;
    st->getP3RetVal()->rev.expl.gc += resultPrimers->rev.expl.gc;
    st->getP3RetVal()->rev.expl.gc_clamp += resultPrimers->rev.expl.gc_clamp;
    st->getP3RetVal()->rev.expl.temp_min += resultPrimers->rev.expl.temp_min;
    st->getP3RetVal()->rev.expl.temp_max += resultPrimers->rev.expl.temp_max;
    st->getP3RetVal()->rev.expl.compl_any += resultPrimers->rev.expl.compl_any;
    st->getP3RetVal()->rev.expl.compl_end += resultPrimers->rev.expl.compl_end;
    st->getP3RetVal()->rev.expl.poly_x += resultPrimers->rev.expl.poly_x;
    st->getP3RetVal()->rev.expl.stability += resultPrimers->rev.expl.stability;
    st->getP3RetVal()->rev.expl.ok += resultPrimers->rev.expl.ok;

    st->getP3RetVal()->best_pairs.expl.considered += resultPrimers->best_pairs.expl.considered;
    st->getP3RetVal()->best_pairs.expl.product += resultPrimers->best_pairs.expl.product;
    st->getP3RetVal()->best_pairs.expl.compl_end += resultPrimers->best_pairs.expl.compl_end;
    st->getP3RetVal()->best_pairs.expl.ok += resultPrimers->best_pairs.expl.ok;
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

Primer3SWTask::Primer3SWTask(Primer3TaskSettings* _settings)
    : Task("Pick primers SW task", TaskFlags_NR_FOSCOE | TaskFlag_CollectChildrenWarnings),
      settings(_settings) {
    median = settings->getSequenceSize() / 2;
    //setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);
}

void Primer3SWTask::prepare() {
    // selected region covers circular junction
    const auto& sequenceRange = settings->getSequenceRange();
    int sequenceSize = settings->getSequenceSize();

    const auto& includedRegion = settings->getIncludedRegion();
    int fbs = settings->getFirstBaseIndex();
    int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - fbs : 0;
    CHECK_EXT(includedRegionOffset >= 0, stateInfo.setError(tr("Incorrect summ \"Included Region Start + First Base Index\" - should be more or equal than 0")), );

    if (sequenceRange.endPos() > sequenceSize + includedRegionOffset) {
        SAFE_POINT_EXT(settings->isSequenceCircular(), stateInfo.setError("Unexpected region, sequence should be circular"), );

        QByteArray seq = settings->getSequence();
        seq.append(seq.left(sequenceRange.endPos() - sequenceSize - fbs));
        settings->setSequence(seq);
    }

    /*Primer3TaskSettings regionSettings = settings;
    if (regionSettings.getTask() != task::pick_sequencing_primers) {
        regionSettings.setIncludedRegion(U2Region(0, -1));
    }*/
    Primer3Task* task = new Primer3Task(settings);
    regionTasks.append(task);
    addSubTask(task);



    //if (settings.isSequenceCircular() && sequenceRange.startPos == 0 &&
    //    sequenceRange.length == sequenceSize) {
    //    // Based on conversation with Vladimir Trifonov:
    //    // Consider the start position in the center of the sequence and find primers for it.
    //    // This should be enough for circular primers search.
    //    QByteArray oppositeSeq = settings.getSequence().right(median);
    //    oppositeSeq.append(settings.getSequence().left(settings.getSequenceSize() - median));
    //    Primer3TaskSettings circSettings = settings;
    //    circSettings.setSequence(oppositeSeq, true);

    //    addPrimer3Subtasks(circSettings, circRegionTasks);
    //} else {
    //    addPrimer3Subtasks(settings, regionTasks);
    //}
}

Task::ReportResult Primer3SWTask::report() {
    for (Primer3Task* task : qAsConst(regionTasks)) {
        bestPairs.append(task->getBestPairs());
        singlePrimers.append(task->getSinglePrimers());
    }

    for (Primer3Task* task : qAsConst(circRegionTasks)) {
        // relocate primers that were found for sequence splitted in the center
        for (PrimerPair p : qAsConst(task->getBestPairs())) {
            relocatePrimerOverMedian(p.getLeftPrimer());
            relocatePrimerOverMedian(p.getRightPrimer());
            if (!bestPairs.contains(p)) {
                bestPairs.append(p);
            }
        }

        for (const auto& p : task->getSinglePrimers()) {
            PrimerSingle primer = p;
            relocatePrimerOverMedian(&primer);
            if (!singlePrimers.contains(primer)) {
                singlePrimers.append(primer);
            }
        }
    }

    if (regionTasks.size() + circRegionTasks.size() > 1) {
        std::stable_sort(bestPairs.begin(), bestPairs.end());
        int pairsCount = 0;
        if (!settings->getIntProperty("PRIMER_NUM_RETURN", &pairsCount)) {
            setError("can't get PRIMER_NUM_RETURN property");
            return Task::ReportResult_Finished;
        }

        bestPairs = bestPairs.mid(0, pairsCount);
    }
    return Task::ReportResult_Finished;
}

void Primer3SWTask::relocatePrimerOverMedian(PrimerSingle* primer) {
    primer->setStart(primer->getStart() + (primer->getStart() >= median ? -median : settings->getSequenceSize() - median));
}

//////////////////////////////////////////////////////////////////////////
////Primer3ToAnnotationsTask

Primer3ToAnnotationsTask::Primer3ToAnnotationsTask(Primer3TaskSettings* _settings, U2SequenceObject* so_, AnnotationTableObject* aobj_, const QString& groupName_, const QString& annName_, const QString& annDescription)
    : Task(tr("Search primers to annotations"), /*TaskFlags_NR_FOSCOE*/ TaskFlags(TaskFlag_NoRun) | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled | TaskFlag_FailOnSubtaskError),
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
                    setError(tr("The first exon from the selected range [%1,%2] is larger the number of exons (%2)."
                                " Please set correct exon range.")
                                 .arg(firstExonIdx)
                                 .arg(lastExonIdx)
                                 .arg(regions.size()));
                }

                if (lastExonIdx > regions.size()) {
                    setError(tr("The the selected exon range [%1,%2] is larger the number of exons (%2)."
                                " Please set correct exon range.")
                                 .arg(firstExonIdx)
                                 .arg(lastExonIdx)
                                 .arg(regions.size()));
                }

                regions = regions.mid(firstExonIdx - 1, lastExonIdx - firstExonIdx + 1);
                int totalLen = 0;
                foreach (const U2Region& r, regions) {
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

    foreach (Primer3Task* t, searchTask->regionTasks) {
        t->sumStat(searchTask->settings);
    }
    foreach (Primer3Task* t, searchTask->circRegionTasks) {
        t->sumStat(searchTask->settings);
    }

    oligo_stats leftStats = searchTask->settings->getP3RetVal()->fwd.expl;
    oligo_stats rightStats = searchTask->settings->getP3RetVal()->rev.expl;
    pair_stats pairStats = searchTask->settings->getP3RetVal()->best_pairs.expl;

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
    CHECK_EXT(!annotationTableObject.isNull(), setError(tr("Object with annotations was removed")), {});

    assert(searchTask);

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

    annotationData->qualifiers.append(U2Qualifier("tm", QString::number(primer.getMeltingTemperature())));
    annotationData->qualifiers.append(U2Qualifier("any", QString::number(primer.getSelfAny())));
    annotationData->qualifiers.append(U2Qualifier("3'", QString::number(primer.getSelfEnd())));
    annotationData->qualifiers.append(U2Qualifier("product_size", QString::number(productSize)));
    annotationData->qualifiers.append(U2Qualifier("hairpin", QString::number(primer.getHairpin())));

    // recalculate gc content
    QByteArray primerSequence;
    foreach (const U2Region& region, annotationData->getRegions()) {
        primerSequence.append(seqObj->getSequence(region, stateInfo).seq);
    }

    int gcCounter = 0;
    foreach (QChar c, primerSequence) {
        if (c.toUpper() == 'G' || c.toUpper() == 'C') {
            gcCounter++;
        }
    }
    double gcContentPercentage = ((double)gcCounter / primerSequence.size()) * 100;
    annotationData->qualifiers.append(U2Qualifier("gc%", QString::number(gcContentPercentage)));

    return annotationData;
}

}  // namespace U2
