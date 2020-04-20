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

#ifndef _U2_FIND_ENZYMES_TASK_H_
#define _U2_FIND_ENZYMES_TASK_H_

#include <limits>

#include <QList>
#include <QMutex>
#include <QObject>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNASequence.h>
#include <U2Core/SequenceDbiWalkerTask.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include "FindEnzymesAlgorithm.h"

namespace U2 {

class FindEnzymesAlgResult {
public:
    FindEnzymesAlgResult() {pos = -1;}
    FindEnzymesAlgResult(const SEnzymeData& _enzyme, int _pos, const U2Strand& _strand) : enzyme(_enzyme), pos(_pos), strand(_strand){}
    SEnzymeData     enzyme;
    int             pos;
    U2Strand        strand;
};

class FindEnzymesTask;

struct FindEnzymesTaskConfig {
    FindEnzymesTaskConfig()
        : maxResults(0x7FFFFFFF),
          minHitCount(1),
          maxHitCount(INT_MAX),
          circular(false),
          isAutoAnnotationUpdateTask(false) {}
    U2Region            searchRegion;
    QVector<U2Region>   excludedRegions;
    QString             groupName;
    int                 maxResults;
    int                 minHitCount;
    int                 maxHitCount;
    bool                circular;
    bool                isAutoAnnotationUpdateTask;

};

class FindEnzymesToAnnotationsTask : public Task {
    Q_OBJECT
public:
                                        FindEnzymesToAnnotationsTask(AnnotationTableObject *aobj, const U2EntityRef &seqRef,
                                            const QList<SEnzymeData> &enzymes, const FindEnzymesTaskConfig &cfg);
    void                                prepare();
    QList<Task *>                       onSubTaskFinished(Task* subTask);
    ReportResult                        report();

private:
    U2EntityRef                         dnaSeqRef;
    QList<SEnzymeData>                  enzymes;
    QPointer<AnnotationTableObject>     aObj;
    FindEnzymesTaskConfig               cfg;
    FindEnzymesTask *                   fTask;
};

class FindEnzymesTask : public Task, public FindEnzymesAlgListener {
    Q_OBJECT
public:
    FindEnzymesTask(const U2EntityRef& seqRef, const U2Region& region, const QList<SEnzymeData>& enzymes, int maxResults = 0x7FFFFFFF,
                    bool _circular = false, QVector<U2Region> excludedRegions = QVector<U2Region>());

    QList<FindEnzymesAlgResult>  getResults() const {return results;}

    virtual void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& stand);

    ReportResult report();

    QList<SharedAnnotationData> getResultsAsAnnotations(const QString& enzymeId = QString()) const;

    void cleanup();

private:
    void registerResult(const FindEnzymesAlgResult& r);

    int                                 maxResults;
    QVector<U2Region>                   excludedRegions;
    bool                                circular;
    int                                 seqlen;
    QList<FindEnzymesAlgResult>         results;
    QMutex                              resultsLock;

    QString                             group;
};


class FindSingleEnzymeTask: public Task, public FindEnzymesAlgListener, public SequenceDbiWalkerCallback {
    Q_OBJECT
public:
    FindSingleEnzymeTask(const U2EntityRef& seqRef, const U2Region& region, const SEnzymeData& enzyme,
                        FindEnzymesAlgListener* l = NULL, bool circular = false, int maxResults = 0x7FFFFFFF);

    QList<FindEnzymesAlgResult>  getResults() const {return results;}
    virtual void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand);
    virtual void onRegion(SequenceDbiWalkerSubtask* t, TaskStateInfo& ti);
    void cleanup();
private:
    U2EntityRef                 dnaSeqRef;
    U2Region                    region;
    SEnzymeData                 enzyme;
    int                         maxResults;
    FindEnzymesAlgListener*     resultListener;
    QList<FindEnzymesAlgResult> results;
    QMutex                      resultsLock;
    bool                        circular;
};

class FindEnzymesAutoAnnotationUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
public:
    FindEnzymesAutoAnnotationUpdater();
    Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* aa);
    bool checkConstraints(const AutoAnnotationConstraints& constraints);
};


} //namespace

#endif
