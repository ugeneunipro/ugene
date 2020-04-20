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

#ifndef _U2_FIND_REPEATS_TASK_H_
#define _U2_FIND_REPEATS_TASK_H_

#include "RFBase.h"

#include <U2Core/Task.h>
#include <U2Core/GObjectReference.h>

#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>

#include <QMutex>

namespace U2 {


class FindRepeatsTaskSettings {
public:
    FindRepeatsTaskSettings() :
        minLen(2),
        mismatches(0),
        minDist(0),
        maxDist(0),
        inverted(false),
        reportReflected(false),
        maxResults(10*1000*100),
        reportSeqShift(0),
        reportSeq2Shift(0),
        algo(RFAlgorithm_Auto),
        filter(DisjointRepeats),
        nThreads(MAX_PARALLEL_SUBTASKS_AUTO),
        excludeTandems(false) {}

    int                 minLen;
    int                 mismatches;
    int                 minDist;
    int                 maxDist;
    bool                inverted;
    bool                reportReflected;
    int                 maxResults;
    U2Region            seqRegion;
    U2Region            seq2Region;
    qint64              reportSeqShift;
    qint64              reportSeq2Shift;

    //all these regions are in global sequence coordinates
    QVector<U2Region>    midRegionsToInclude;  //reported repeat must contain one of these regions
    QVector<U2Region>    midRegionsToExclude;  //reported repeat must not contain none of these regions
    QVector<U2Region>    allowedRegions;       //reported repeat must fit one of these regions

    RFAlgorithm         algo;
    RepeatsFilterAlgorithm    filter;
    int                 nThreads;
    bool                excludeTandems;

    void setIdentity(int percent) {mismatches = int((minLen / 100.0) * (100 - percent));}
    int  getIdentity(int mismatch = -1) const {return qBound(50, int(100.0 - (mismatch == -1 ? mismatches : mismatch) * 100. /minLen), 100);}
    bool hasRegionFilters() const {return !midRegionsToInclude.isEmpty() || !midRegionsToExclude.isEmpty() || !allowedRegions.isEmpty();}

    static int getIdentity(int mismatch, int len) { return qBound(50, int(100.0 - (mismatch * 100. / len )),100); }
};

//WARNING: this task is suitable only for a single sequence processing -> check addResults x/y sorting
class RevComplSequenceTask;
class FindTandemsToAnnotationsTask;
class FindRepeatsTask : public Task, public RFResultsListener {
Q_OBJECT
public:
    FindRepeatsTask(const FindRepeatsTaskSettings& s, const DNASequence& seq, const DNASequence& seq2);

    void prepare();
    void run();
    ReportResult report();
    void cleanup();

    QList<Task*> onSubTaskFinished(Task* subTask);

    virtual void onResult(const RFResult& r);
    virtual void onResults(const QVector<RFResult>& v) ;

    QVector<RFResult> getResults() const {return results;} // used if createAnnotations == false
    const FindRepeatsTaskSettings&  getSettings() const {return settings;}

protected:
    void addResult(const RFResult& r);
    void _addResult(int x, int y, int l, int c);
    bool isFilteredByRegions(const RFResult& r);
    RFAlgorithmBase* createRFTask();
    void filterNestedRepeats();
    void filterUniqueRepeats();
    Task *createRepeatFinderTask();
    void filterTandems(const QList<SharedAnnotationData> &tandems, DNASequence &se);

    bool                        oneSequence;
    FindRepeatsTaskSettings     settings;
    DNASequence                 seq1, seq2;
    QVector<RFResult>           results;
    QMutex                      resultsLock;
    RevComplSequenceTask*       revComplTask;
    RFAlgorithmBase*            rfTask;
    quint64                     startTime;
    FindTandemsToAnnotationsTask *tandemTask1;
    FindTandemsToAnnotationsTask *tandemTask2;
};

class FindRepeatsToAnnotationsTask : public Task {
    Q_OBJECT
public:
    FindRepeatsToAnnotationsTask(const FindRepeatsTaskSettings& s,
                                 const DNASequence& seq,
                                 const QString& annName,
                                 const QString& groupName,
                                 const QString &annDescription,
                                 const GObjectReference& annObjRef);

    QList<Task*> onSubTaskFinished(Task* subTask);
    QList<SharedAnnotationData> importAnnotations();

private:
    QString                 annName;
    QString                 annGroup;
    const QString           annDescription;
    GObjectReference        annObjRef;
    FindRepeatsTask*        findTask;
    FindRepeatsTaskSettings settings;
};

class RevComplSequenceTask : public Task {
    Q_OBJECT
public:
    RevComplSequenceTask(const DNASequence& s, const U2Region& reg);

    void run();
    void cleanup();

    DNASequence sequence;
    U2Region     region;
    DNASequence complementSequence;
};

} //namespace

#endif
