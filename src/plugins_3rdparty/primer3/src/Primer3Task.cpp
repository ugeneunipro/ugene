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

#include <cstdlib>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MultiTask.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/U1AnnotationUtils.h>

#include "Primer3Task.h"
#include "primer3_core/boulder_input.h"
#include "primer3_core/primer3_main.h"

namespace U2 {

// Primer

Primer::Primer():
    start(0),
    length(0),
    meltingTemperature(0),
    gcContent(0),
    selfAny(0),
    selfEnd(0),
    endStability(0)
{
}

Primer::Primer(const primer_rec &primerRec):
    start(primerRec.start),
    length(primerRec.length),
    meltingTemperature(primerRec.temp),
    gcContent(primerRec.gc_content),
    selfAny(primerRec.self_any),
    selfEnd(primerRec.self_end),
    endStability(primerRec.end_stability)
{
}

bool Primer::operator ==(const Primer& p) const {
    bool result = true;

    result &= start == p.start;
    result &= length == p.length;
    result &= meltingTemperature == p.meltingTemperature;
    result &= gcContent == p.gcContent;
    result &= selfAny == p.selfAny;
    result &= selfEnd == p.selfEnd;
    result &= endStability == p.endStability;

    return result;
}

bool Primer::areEqual(const Primer *p1, const Primer *p2) {
    if (p1 != NULL && p2 != NULL) {
        return (*p1 == *p2);
    } else {
        return (p1 == p2);
    }
}

int Primer::getStart()const
{
    return start;
}

int Primer::getLength()const
{
    return length;
}

double Primer::getMeltingTemperature()const
{
    return meltingTemperature;
}

double Primer::getGcContent()const
{
    return gcContent;
}

short Primer::getSelfAny()const
{
    return selfAny;
}

short Primer::getSelfEnd()const
{
    return selfEnd;
}

double Primer::getEndStabilyty()const
{
    return endStability;
}

void Primer::setStart(int start)
{
    this->start = start;
}

void Primer::setLength(int length)
{
    this->length = length;
}

void Primer::setMeltingTemperature(double meltingTemperature)
{
    this->meltingTemperature = meltingTemperature;
}

void Primer::setGcContent(double gcContent)
{
    this->gcContent = gcContent;
}

void Primer::setSelfAny(short selfAny)
{
    this->selfAny = selfAny;
}

void Primer::setSelfEnd(short selfEnd)
{
    this->selfEnd = selfEnd;
}

void Primer::setEndStability(double endStability)
{
    this->endStability = endStability;
}

// PrimerPair

PrimerPair::PrimerPair():
    leftPrimer(NULL),
    rightPrimer(NULL),
    internalOligo(NULL),
    complAny(0),
    complEnd(0),
    productSize(0),
    quality(0),
    complMeasure(0)
{
}

PrimerPair::PrimerPair(const primer_pair &primerPair, int offset):
    leftPrimer((NULL == primerPair.left)? NULL:new Primer(*primerPair.left)),
    rightPrimer((NULL == primerPair.right)? NULL:new Primer(*primerPair.right)),
    internalOligo((NULL == primerPair.intl)? NULL:new Primer(*primerPair.intl)),
    complAny(primerPair.compl_any),
    complEnd(primerPair.compl_end),
    productSize(primerPair.product_size),
    quality(primerPair.pair_quality),
    complMeasure(primerPair.compl_measure)
{
    if ( !leftPrimer.isNull( ) )
    {
        leftPrimer->setStart(leftPrimer->getStart() + offset);
    }
    if ( !rightPrimer.isNull( ) )
    {
        rightPrimer->setStart(rightPrimer->getStart() + offset);
    }
    if ( !internalOligo.isNull( ) )
    {
        internalOligo->setStart(internalOligo->getStart() + offset);
    }
}

PrimerPair::PrimerPair(const PrimerPair &primerPair):
        leftPrimer( ( primerPair.leftPrimer.isNull( ) ) ? NULL : new Primer( *primerPair.leftPrimer ) ),
        rightPrimer( ( primerPair.rightPrimer.isNull( ) ) ? NULL : new Primer( *primerPair.rightPrimer ) ),
        internalOligo( ( primerPair.internalOligo.isNull( ) ) ? NULL : new Primer( *primerPair.internalOligo ) ),
        complAny(primerPair.complAny),
        complEnd(primerPair.complEnd),
        productSize(primerPair.productSize),
        quality(primerPair.quality),
        complMeasure(primerPair.complMeasure)
{
}

PrimerPair &PrimerPair::operator=(const PrimerPair &primerPair)
{
    leftPrimer.reset( ( primerPair.leftPrimer.isNull( ) ) ? NULL : new Primer( *primerPair.leftPrimer ) );
    rightPrimer.reset( ( primerPair.rightPrimer.isNull( ) ) ? NULL : new Primer( *primerPair.rightPrimer ) );
    internalOligo.reset( ( primerPair.internalOligo.isNull( ) ) ? NULL : new Primer( *primerPair.internalOligo ) );
    complAny = primerPair.complAny;
    complEnd = primerPair.complEnd;
    productSize = primerPair.productSize;
    quality = primerPair.quality;
    complMeasure = primerPair.complMeasure;
    return *this;
}

bool PrimerPair::operator ==(const PrimerPair &primerPair) const {
    bool result = true;

    result &= Primer::areEqual(leftPrimer.data(), primerPair.leftPrimer.data());
    result &= Primer::areEqual(rightPrimer.data(), primerPair.rightPrimer.data());
    result &= Primer::areEqual(internalOligo.data(), primerPair.internalOligo.data());

    result &= complAny == primerPair.complAny;
    result &= complEnd == primerPair.complEnd;
    result &= productSize == primerPair.productSize;
    result &= quality == primerPair.quality;
    result &= complMeasure == primerPair.complMeasure;

    return result;
}

Primer *PrimerPair::getLeftPrimer()const
{
    return leftPrimer.data( );
}

Primer *PrimerPair::getRightPrimer()const
{
    return rightPrimer.data( );
}

Primer *PrimerPair::getInternalOligo()const
{
    return internalOligo.data( );
}

short PrimerPair::getComplAny()const
{
    return complAny;
}

short PrimerPair::getComplEnd()const
{
    return complEnd;
}

int PrimerPair::getProductSize()const
{
    return productSize;
}

void PrimerPair::setLeftPrimer(Primer *leftPrimer)
{
    this->leftPrimer.reset((NULL == leftPrimer)? NULL:new Primer(*leftPrimer));
}

void PrimerPair::setRightPrimer(Primer *rightPrimer)
{
    this->rightPrimer.reset((NULL == rightPrimer)? NULL:new Primer(*rightPrimer));
}

void PrimerPair::setInternalOligo(Primer *internalOligo)
{
    this->internalOligo.reset((NULL == internalOligo)? NULL:new Primer(*internalOligo));
}

void PrimerPair::setComplAny(short complAny)
{
    this->complAny = complAny;
}

void PrimerPair::setComplEnd(short complEnd)
{
    this->complEnd = complEnd;
}

void PrimerPair::setProductSize(int productSize)
{
    this->productSize = productSize;
}

bool PrimerPair::operator<(const PrimerPair &pair)const
{
    if(quality < pair.quality)
    {
        return true;
    }
    if(quality > pair.quality)
    {
        return false;
    }

    if (complMeasure < pair.complMeasure)
    {
        return true;
    }
    if (complMeasure > pair.complMeasure)
    {
        return false;
    }

    if(leftPrimer->getStart() > pair.leftPrimer->getStart())
    {
        return true;
    }
    if(leftPrimer->getStart() < pair.leftPrimer->getStart())
    {
        return false;
    }

    if(rightPrimer->getStart() < pair.rightPrimer->getStart())
    {
        return true;
    }
    if(rightPrimer->getStart() > pair.rightPrimer->getStart())
    {
        return false;
    }

    if(leftPrimer->getLength() < pair.leftPrimer->getLength())
    {
        return true;
    }
    if(leftPrimer->getLength() > pair.leftPrimer->getLength())
    {
        return false;
    }

    if(rightPrimer->getLength() < pair.rightPrimer->getLength())
    {
        return true;
    }
    if(rightPrimer->getLength() > pair.rightPrimer->getLength())
    {
        return false;
    }

    return false;
}

// Primer3Task

namespace
{
    bool clipRegion(U2Region &region, const U2Region &clippingRegion)
    {
        qint64 start = qMax(region.startPos, clippingRegion.startPos);
        qint64 end = qMin(region.endPos(), clippingRegion.endPos());
        if(start > end)
        {
            return false;
        }
        region.startPos = start;
        region.length = end - start;
        return true;
    }
}

Primer3Task::Primer3Task(const Primer3TaskSettings &settingsArg):
    Task(tr("Pick primers task"), TaskFlag_ReportingIsEnabled),
    settings(settingsArg)
{
    GCOUNTER( cvar, tvar, "Primer3Task" );
    {
        U2Region region = settings.getIncludedRegion();
        region.startPos -= settings.getFirstBaseIndex();
        settings.setIncludedRegion(region);
    }
    offset = settings.getIncludedRegion().startPos;
    settings.setSequence(settings.getSequence().mid(
            settings.getIncludedRegion().startPos, settings.getIncludedRegion().length));
    settings.setSequenceQuality(settings.getSequenceQuality().mid(
            settings.getIncludedRegion().startPos, settings.getIncludedRegion().length));
    settings.setIncludedRegion(0, settings.getIncludedRegion().length);
    if(!PR_START_CODON_POS_IS_NULL(settings.getSeqArgs()))
    {
        int startCodonPosition = PR_DEFAULT_START_CODON_POS;
        if(settings.getIntProperty("PRIMER_START_CODON_POSITION", &startCodonPosition))
        {
            settings.setIntProperty("PRIMER_START_CODON_POSITION",
                                    startCodonPosition - settings.getFirstBaseIndex());
        }
    }
    {
        QList< U2Region> regionList;
        foreach (U2Region region, settings.getTarget()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if(clipRegion(region, settings.getIncludedRegion()))
            {
                regionList.append(region);
            }
        }
        settings.setTarget(regionList);
    }
    {
        QList< U2Region > regionList;
        foreach (U2Region region, settings.getExcludedRegion()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if(clipRegion(region, settings.getIncludedRegion()))
            {
                regionList.append(region);
            }
        }
        settings.setExcludedRegion(regionList);
    }
    {
        QList< U2Region > regionList;
        foreach (U2Region region, settings.getInternalOligoExcludedRegion()) {
            region.startPos -= settings.getFirstBaseIndex();
            region.startPos -= offset;
            if(clipRegion(region, settings.getIncludedRegion()))
            {
                regionList.append(region);
            }
        }
        settings.setInternalOligoExcludedRegion(regionList);
    }
}

void Primer3Task::run()
{
    if(!settings.getRepeatLibrary().isEmpty())
    {
        read_seq_lib(&settings.getPrimerArgs()->repeat_lib, settings.getRepeatLibrary().constData(), "mispriming library");
        if(NULL != settings.getPrimerArgs()->repeat_lib.error.data)
        {
            pr_append_new_chunk(&settings.getPrimerArgs()->glob_err, settings.getPrimerArgs()->repeat_lib.error.data);
            pr_append_new_chunk(&settings.getSeqArgs()->error, settings.getPrimerArgs()->repeat_lib.error.data);
            return;
        }
    }
    if(!settings.getMishybLibrary().isEmpty())
    {
        read_seq_lib(&settings.getPrimerArgs()->io_mishyb_library, settings.getMishybLibrary().constData(), "internal oligo mishyb library");
        if(NULL != settings.getPrimerArgs()->io_mishyb_library.error.data)
        {
            pr_append_new_chunk(&settings.getPrimerArgs()->glob_err, settings.getPrimerArgs()->io_mishyb_library.error.data);
            pr_append_new_chunk(&settings.getSeqArgs()->error, settings.getPrimerArgs()->io_mishyb_library.error.data);
            return;
        }
    }

    bool spanExonsEnabled = settings.getSpanIntronExonBoundarySettings().enabled;
    int toReturn = settings.getPrimerArgs()->num_return;
    if (spanExonsEnabled) {
        settings.getPrimerArgs()->num_return = settings.getSpanIntronExonBoundarySettings().maxPairsToQuery; // not an optimal algorithm
    }
    primers_t primers = runPrimer3(settings.getPrimerArgs(), settings.getSeqArgs(), &stateInfo.cancelFlag, &stateInfo.progress);

    bestPairs.clear();

    if (settings.getSpanIntronExonBoundarySettings().enabled) {
        if (settings.getSpanIntronExonBoundarySettings().overlapExonExonBoundary) {
            selectPairsSpanningExonJunction(primers, toReturn);
        } else {
            selectPairsSpanningIntron(primers, toReturn);
        }
    } else {

        for(int index = 0;index < primers.best_pairs.num_pairs;index++)
        {
            bestPairs.append(PrimerPair(primers.best_pairs.pairs[index], offset));
        }
    }

    int maxCount = 0;
    settings.getIntProperty("PRIMER_NUM_RETURN", &maxCount);

    if (settings.getTask() == pick_left_only) {
        if (primers.left != NULL) {
            for (int i = 0; i < settings.getSeqArgs()->left_expl.ok && i < maxCount; ++i) {
                singlePrimers.append( Primer( * ( primers.left + i ) ));
            }

        }
    } else if (settings.getTask() == pick_right_only) {
        if (primers.right != NULL) {
            for (int i = 0; i < settings.getSeqArgs()->right_expl.ok && i < maxCount; ++i) {
                singlePrimers.append( Primer( * ( primers.right + i ) ));
            }

        }
    }

    if(primers.best_pairs.num_pairs > 0)
    {
        std::free(primers.best_pairs.pairs);
        primers.best_pairs.pairs = NULL;
    }
    if(NULL != primers.left)
    {
        std::free(primers.left);
        primers.left = NULL;
    }
    if(NULL != primers.right)
    {
        std::free(primers.right);
        primers.right = NULL;
    }
    if(NULL != primers.intl)
    {
        std::free(primers.intl);
        primers.intl = NULL;
    }
}

Task::ReportResult Primer3Task::report()
{
    if(!settings.getError().isEmpty())
    {
        stateInfo.setError(settings.getError());
    }
    return Task::ReportResult_Finished;
}

void Primer3Task::sumStat(Primer3TaskSettings *st) {
    st->getSeqArgs()->left_expl.considered += settings.getSeqArgs()->left_expl.considered;
    st->getSeqArgs()->left_expl.ns += settings.getSeqArgs()->left_expl.ns;
    st->getSeqArgs()->left_expl.target += settings.getSeqArgs()->left_expl.target;
    st->getSeqArgs()->left_expl.excluded += settings.getSeqArgs()->left_expl.excluded;
    st->getSeqArgs()->left_expl.gc += settings.getSeqArgs()->left_expl.gc;
    st->getSeqArgs()->left_expl.gc_clamp += settings.getSeqArgs()->left_expl.gc_clamp;
    st->getSeqArgs()->left_expl.temp_min += settings.getSeqArgs()->left_expl.temp_min;
    st->getSeqArgs()->left_expl.temp_max += settings.getSeqArgs()->left_expl.temp_max;
    st->getSeqArgs()->left_expl.compl_any += settings.getSeqArgs()->left_expl.compl_any;
    st->getSeqArgs()->left_expl.compl_end += settings.getSeqArgs()->left_expl.compl_end;
    st->getSeqArgs()->left_expl.poly_x += settings.getSeqArgs()->left_expl.poly_x;
    st->getSeqArgs()->left_expl.stability += settings.getSeqArgs()->left_expl.stability;
    st->getSeqArgs()->left_expl.ok += settings.getSeqArgs()->left_expl.ok;

    st->getSeqArgs()->right_expl.considered += settings.getSeqArgs()->right_expl.considered;
    st->getSeqArgs()->right_expl.ns += settings.getSeqArgs()->right_expl.ns;
    st->getSeqArgs()->right_expl.target += settings.getSeqArgs()->right_expl.target;
    st->getSeqArgs()->right_expl.excluded += settings.getSeqArgs()->right_expl.excluded;
    st->getSeqArgs()->right_expl.gc += settings.getSeqArgs()->right_expl.gc;
    st->getSeqArgs()->right_expl.gc_clamp += settings.getSeqArgs()->right_expl.gc_clamp;
    st->getSeqArgs()->right_expl.temp_min += settings.getSeqArgs()->right_expl.temp_min;
    st->getSeqArgs()->right_expl.temp_max += settings.getSeqArgs()->right_expl.temp_max;
    st->getSeqArgs()->right_expl.compl_any += settings.getSeqArgs()->right_expl.compl_any;
    st->getSeqArgs()->right_expl.compl_end += settings.getSeqArgs()->right_expl.compl_end;
    st->getSeqArgs()->right_expl.poly_x += settings.getSeqArgs()->right_expl.poly_x;
    st->getSeqArgs()->right_expl.stability += settings.getSeqArgs()->right_expl.stability;
    st->getSeqArgs()->right_expl.ok += settings.getSeqArgs()->right_expl.ok;

    st->getSeqArgs()->pair_expl.considered += settings.getSeqArgs()->pair_expl.considered;
    st->getSeqArgs()->pair_expl.product += settings.getSeqArgs()->pair_expl.product;
    st->getSeqArgs()->pair_expl.compl_end += settings.getSeqArgs()->pair_expl.compl_end;
    st->getSeqArgs()->pair_expl.ok += settings.getSeqArgs()->pair_expl.ok;
}


// TODO: reuse functions from U2Region!
static QList<int> findIntersectingRegions(const QList<U2Region>& regions, int start, int length) {
    QList<int> indexes;

    U2Region target(start,length);
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

    foreach (qint64 junctionPos, junctions) {
        U2Region testRegion(junctionPos - minLeftOverlap, minLeftOverlap + minRightOverlap);
        if (primerRegion.contains(testRegion)) {
            return true;
        }
    }

    return false;
}

void Primer3Task::selectPairsSpanningExonJunction(primers_t &primers, int toReturn)
{
    int minLeftOverlap = settings.getSpanIntronExonBoundarySettings().minLeftOverlap;
    int minRightOverlap = settings.getSpanIntronExonBoundarySettings().minRightOverlap;

    QVector<qint64> junctionPositions;
    const QList<U2Region>& regions = settings.getExonRegions();
    for (int i = 0; i < regions.size() - 1; ++i) {
        qint64 end = regions.at(i).endPos();
        junctionPositions.push_back( end );
    }

    for(int index = 0;index < primers.best_pairs.num_pairs;index++)
    {
        const primer_pair& pair  = primers.best_pairs.pairs[index];
        const primer_rec* left = pair.left;
        const primer_rec* right = pair.right;


        if (pairIntersectsJunction(left, junctionPositions, minLeftOverlap, minRightOverlap )
                || pairIntersectsJunction(right, junctionPositions, minLeftOverlap, minRightOverlap)) {
            bestPairs.append(PrimerPair(pair,offset));
        }

        if (bestPairs.size() == toReturn) {
            break;
        }

    }



}


void Primer3Task::selectPairsSpanningIntron(primers_t& primers, int toReturn)
{
    const QList<U2Region>& regions = settings.getExonRegions();

    for(int index = 0;index < primers.best_pairs.num_pairs;index++)
    {
        const primer_pair& pair  = primers.best_pairs.pairs[index];
        const primer_rec* left = pair.left;
        const primer_rec* right = pair.right;

        QList<int> regionIndexes = findIntersectingRegions(regions, left->start, left->length );

        int numIntersecting = 0;
        U2Region rightRegion(right->start, right->length);
        foreach (int idx, regionIndexes) {
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

Primer3SWTask::Primer3SWTask(const Primer3TaskSettings &settingsArg):
    Task("Pick primers SW task", TaskFlags_NR_FOSCOE),
    settings(settingsArg)
{
    median = settings.getSequenceSize() / 2;
    setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);
}

void Primer3SWTask::prepare()
{
    if((settings.getIncludedRegion().startPos < settings.getFirstBaseIndex()) ||
       (settings.getIncludedRegion().length <= 0))
    {
        setError("invalid included region");
        return;
    }

    // selected region covers circular junction
    if (settings.getIncludedRegion().endPos() > settings.getSequenceSize() + settings.getFirstBaseIndex()) {
        if (!settings.isSequenceCircular()) {
            U2Region endRegion(settings.getIncludedRegion().startPos,
                               settings.getSequenceSize() - settings.getIncludedRegion().startPos + 1);
            U2Region startRegion(1, settings.getIncludedRegion().endPos() - settings.getSequenceSize());

            if (settings.checkIncludedRegion(startRegion)) {
                addPrimer3Subtasks(settings, startRegion, regionTasks);
            }
            if (settings.checkIncludedRegion(endRegion)) {
                addPrimer3Subtasks(settings, endRegion, regionTasks);
            }
            return;
        } else {
            QByteArray seq = settings.getSequence();
            seq.append( seq.left(settings.getIncludedRegion().endPos() - settings.getSequenceSize() - settings.getFirstBaseIndex() ));
            settings.setSequence(seq);
        }
    }

    addPrimer3Subtasks(settings, regionTasks);

    if (settings.isSequenceCircular() && settings.getIncludedRegion().startPos == 1 &&
            settings.getIncludedRegion().length == settings.getSequenceSize()) {
        // Based on conversation with Vladimir Trifonov:
        // Consider the start position in the center of the sequence and find primers for it.
        // This should be enough for circular primers search.
        QByteArray oppositeSeq = settings.getSequence().right(median);
        oppositeSeq.append( settings.getSequence().left(settings.getSequenceSize() - median));
        Primer3TaskSettings circSettings = settings;
        circSettings.setSequence(oppositeSeq, true);

        addPrimer3Subtasks(circSettings, circRegionTasks);
    }
}

inline int getIntersectingRegionIndex(const U2Region& reg, const QList<U2Region>& regions) {
    for (int i = 0; i < regions.size(); ++i) {
        const U2Region& targetRegion = regions.at(i);
        if (targetRegion.contains(reg.startPos)) {
            return i;
        }
    }
    return -1;
}


Task::ReportResult Primer3SWTask::report()
{
    foreach(Primer3Task *task, regionTasks)
    {
        bestPairs.append(task->getBestPairs());
        singlePrimers.append(task->getSinglePrimers());
    }

    foreach(Primer3Task *task, circRegionTasks)
    {
        // relocate primers that were found for sequence splitted in the center
        foreach (PrimerPair p, task->getBestPairs()) {
            relocatePrimerOverMedian(p.getLeftPrimer());
            relocatePrimerOverMedian(p.getRightPrimer());
            if (!bestPairs.contains(p)) {
                bestPairs.append(p);
            }
        }

        foreach (Primer p, task->getSinglePrimers()) {
            relocatePrimerOverMedian(&p);
            if (!singlePrimers.contains(p)) {
                singlePrimers.append(p);
            }
        }
    }

    if (regionTasks.size() + circRegionTasks.size() > 1)
    {
        qStableSort(bestPairs);
        int pairsCount = 0;
        if(!settings.getIntProperty("PRIMER_NUM_RETURN", &pairsCount))
        {
            setError("can't get PRIMER_NUM_RETURN property");
            return Task::ReportResult_Finished;
        }

        bestPairs = bestPairs.mid(0, pairsCount);
    }
    return Task::ReportResult_Finished;
}

void Primer3SWTask::addPrimer3Subtasks(const Primer3TaskSettings &settings, const U2Region& rangeToSplit, QList<Primer3Task*> &list) {
    QVector<U2Region> regions = SequenceWalkerTask::splitRange(rangeToSplit,
                                                               CHUNK_SIZE, 0, CHUNK_SIZE/2, false);
    foreach(const U2Region& region, regions)
    {
        Primer3TaskSettings regionSettings = settings;
        regionSettings.setIncludedRegion(region);
        Primer3Task *task = new Primer3Task(regionSettings);
        list.append(task);
        addSubTask(task);
    }
}

void Primer3SWTask::addPrimer3Subtasks(const Primer3TaskSettings &settings, QList<Primer3Task*> &list) {
    addPrimer3Subtasks(settings, settings.getIncludedRegion(), list);
}


void Primer3SWTask::relocatePrimerOverMedian(Primer *primer) {
    primer->setStart( primer->getStart() + ( primer->getStart() >= median ? -median : settings.getSequenceSize() - median));
}


//////////////////////////////////////////////////////////////////////////
////Primer3ToAnnotationsTask

Primer3ToAnnotationsTask::Primer3ToAnnotationsTask(const Primer3TaskSettings &settings, U2SequenceObject* so_,
AnnotationTableObject *aobj_, const QString & groupName_, const QString & annName_ , const QString &annDescription) :
Task(tr("Search primers to annotations"), /*TaskFlags_NR_FOSCOE*/TaskFlags(TaskFlag_NoRun) | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled | TaskFlag_FailOnSubtaskError),
    settings(settings), seqObj(so_), aobj(aobj_),
    groupName(groupName_), annName(annName_), annDescription(annDescription), searchTask(NULL), findExonsTask(NULL)
{
}

void Primer3ToAnnotationsTask::prepare()
{
    if (settings.getSpanIntronExonBoundarySettings().enabled) {
        findExonsTask = new FindExonRegionsTask(seqObj,settings.getSpanIntronExonBoundarySettings().exonAnnotationName);
        addSubTask(findExonsTask);
    } else {
        searchTask = new Primer3SWTask(settings);
        addSubTask(searchTask);
    }
}

QList<Task *> Primer3ToAnnotationsTask::onSubTaskFinished(Task *subTask)
{
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
            setError( tr( "Failed to find any exon annotations associated with the sequence %1."
                          "Make sure the provided sequence is cDNA and has exonic structure annotated")
                      .arg(seqObj->getSequenceName()));
            return res;
        } else {
            const U2Range<int>& exonRange = settings.getSpanIntronExonBoundarySettings().exonRange;

            if (exonRange.minValue != 0 && exonRange.maxValue != 0) {
                int firstExonIdx = exonRange.minValue;
                int lastExonIdx = exonRange.maxValue;
                if (firstExonIdx > regions.size()) {
                    setError(tr("The first exon from the selected range [%1,%2] is larger the number of exons (%2)."
                                " Please set correct exon range.")
                             .arg(firstExonIdx).arg(lastExonIdx).arg(regions.size()) );
                }

                if (lastExonIdx > regions.size()) {
                    setError(tr("The the selected exon range [%1,%2] is larger the number of exons (%2)."
                                " Please set correct exon range.")
                             .arg(firstExonIdx).arg(lastExonIdx).arg(regions.size()) );
                }

                regions = regions.mid(firstExonIdx - 1, lastExonIdx - firstExonIdx + 1);
                int totalLen = 0;
                foreach (const U2Region& r, regions ) {
                    totalLen += r.length;
                }
                settings.setIncludedRegion(regions.first().startPos + settings.getFirstBaseIndex(), totalLen);


            }
            settings.setExonRegions(regions);
            // reset target and excluded regions regions
            QList< U2Region > emptyList;
            settings.setExcludedRegion(emptyList);
            settings.setTarget(emptyList);

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

    foreach(Primer3Task *t, searchTask->regionTasks) {
        t->sumStat(&searchTask->settings);
    }
    foreach(Primer3Task *t, searchTask->circRegionTasks) {
        t->sumStat(&searchTask->settings);
    }

    oligo_stats leftStats = searchTask->settings.getSeqArgs()->left_expl;
    oligo_stats rightStats = searchTask->settings.getSeqArgs()->right_expl;
    pair_stats pairStats = searchTask->settings.getSeqArgs()->pair_expl;

    if(!leftStats.considered) {
        leftStats.considered = leftStats.ns + leftStats.target + leftStats.excluded + leftStats.gc + leftStats.gc_clamp + leftStats.temp_min
            + leftStats.temp_max + leftStats.compl_any + leftStats.compl_end + leftStats.poly_x + leftStats.stability + leftStats.ok;
    }

    if(!rightStats.considered) {
        rightStats.considered = rightStats.ns + rightStats.target + rightStats.excluded + rightStats.gc + rightStats.gc_clamp + rightStats.temp_min
            + rightStats.temp_max + rightStats.compl_any + rightStats.compl_end + rightStats.poly_x + rightStats.stability + rightStats.ok;
    }

    res+="<table cellspacing='7'>";
    res += "<tr><th>Statistics</th></tr>\n";

    res += QString("<tr><th></th> <th>con</th> <th>too</th> <th>in</th> <th>in</th> <th></th> <th>no</th> <th>tm</th> <th>tm</th> <th>high</th> <th>high</th> <th></th> <th>high</th> <th></th></tr>");
    res += QString("<tr><th></th> <th>sid</th> <th>many</th> <th>tar</th> <th>excl</th> <th>bag</th> <th>GC</th> <th>too</th> <th>too</th> <th>any</th> <th>3'</th> <th>poly</th> <th>end</th> <th></th></tr>");
    res += QString("<tr><th></th> <th>ered</th> <th>Ns</th> <th>get</th> <th>reg</th> <th>GC&#37;</th> <th>clamp</th> <th>low</th> <th>high</th> <th>compl</th> <th>compl</th> <th>X</th> <th>stab</th> <th>ok</th></tr>");

    res += QString("<tr><th>Left</th><th> %1 </th><th> %2 </th><th> %3 </th><th> %4 </th><th> %5 </th><th> %6 </th><th> %7 </th><th> %8 </th><th> %9 </th><th> %10 </th><th> %11 </th><th> %12 </th><th> %13 </th></tr>")
        .arg(leftStats.considered).arg(leftStats.ns).arg(leftStats.target).arg(leftStats.excluded).arg(leftStats.gc).arg(leftStats.gc_clamp).arg(leftStats.temp_min).arg(leftStats.temp_max)
        .arg(leftStats.compl_any).arg(leftStats.compl_end).arg(leftStats.poly_x).arg(leftStats.stability).arg(leftStats.ok);
    res += QString("<tr><th>Right</th><th> %1 </th><th> %2 </th><th> %3 </th><th> %4 </th><th> %5 </th><th> %6 </th><th> %7 </th><th> %8 </th><th> %9 </th><th> %10 </th><th> %11 </th><th> %12 </th><th> %13 </th></tr>")
        .arg(rightStats.considered).arg(rightStats.ns).arg(rightStats.target).arg(rightStats.excluded).arg(rightStats.gc).arg(rightStats.gc_clamp).arg(rightStats.temp_min).arg(rightStats.temp_max)
        .arg(rightStats.compl_any).arg(rightStats.compl_end).arg(rightStats.poly_x).arg(rightStats.stability).arg(rightStats.ok);
    res+="</table>";
    res += "<br>Pair stats:<br>";
    res += QString("considered %1, unacceptable product size %2, high end compl %3, ok %4.")
        .arg(pairStats.considered).arg(pairStats.product).arg(pairStats.compl_end).arg(pairStats.ok);

    return res;
}

Task::ReportResult Primer3ToAnnotationsTask::report()
{
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    assert( searchTask );

    const QList<PrimerPair>& bestPairs = searchTask->getBestPairs();

    QMap<QString, QList<SharedAnnotationData> > resultAnnotations;
    int index = 0;
    foreach(const PrimerPair& pair, bestPairs)
    {
        QList<SharedAnnotationData> annotations;
        if(NULL != pair.getLeftPrimer())
        {
            annotations.append(oligoToAnnotation(annName, *pair.getLeftPrimer(), pair.getProductSize(), U2Strand::Direct));
        }
        if(NULL != pair.getInternalOligo())
        {
            annotations.append(oligoToAnnotation("internalOligo", *pair.getInternalOligo(), pair.getProductSize(), U2Strand::Direct));
        }
        if(NULL != pair.getRightPrimer())
        {
            annotations.append(oligoToAnnotation(annName, *pair.getRightPrimer(), pair.getProductSize(), U2Strand::Complementary));
        }
        resultAnnotations[groupName + "/pair " + QString::number(index + 1)].append(annotations);
        index++;
    }

    if (settings.getTask() == pick_left_only || settings.getTask() == pick_right_only) {
        const QList<Primer> singlePrimers = searchTask->getSinglePrimers();
        QList<SharedAnnotationData> annotations;
        U2Strand s = settings.getTask() == pick_left_only ? U2Strand::Direct : U2Strand::Complementary;
        foreach (const Primer &p, singlePrimers) {
            annotations.append(oligoToAnnotation(annName, p, 0, s));
        }
        U1AnnotationUtils::addDescriptionQualifier(annotations, annDescription);

        if (!annotations.isEmpty()) {
            resultAnnotations[groupName].append(annotations);
        }
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(new CreateAnnotationsTask(aobj, resultAnnotations));

    return ReportResult_Finished;
}

SharedAnnotationData Primer3ToAnnotationsTask::oligoToAnnotation(const QString& title, const Primer &primer, int productSize, U2Strand strand)
{
    SharedAnnotationData annotationData(new AnnotationData);
    annotationData->name = title;
    annotationData->type = U2FeatureTypes::Primer;
    qint64 seqLen = seqObj->getSequenceLength();
    // primer can be found on circular extension of the sequence
    int start = primer.getStart() + (primer.getStart() > seqLen ?  (- seqLen) : 0);
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
    annotationData->qualifiers.append(U2Qualifier("any", QString::number(0.01*primer.getSelfAny())));
    annotationData->qualifiers.append(U2Qualifier("3'", QString::number(0.01*primer.getSelfEnd())));
    annotationData->qualifiers.append(U2Qualifier("product_size", QString::number(productSize)));

    //recalculate gc content
    QByteArray primerSequence;
    foreach(const U2Region &region, annotationData->getRegions()) {
        primerSequence.append(seqObj->getSequence(region, stateInfo).seq);
    }

    int gcCounter = 0;
    foreach(QChar c, primerSequence) {
        if (c.toUpper() == 'G' || c.toUpper() == 'C') {
            gcCounter++;
        }
    }
    double gcContentPercentage = ((double)gcCounter / primerSequence.size()) * 100;
    annotationData->qualifiers.append(U2Qualifier("gc%", QString::number(gcContentPercentage)));

    return annotationData;
}

} // namespace U2
