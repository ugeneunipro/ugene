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
    FindEnzymesAlgResult() {
        pos = -1;
    }
    FindEnzymesAlgResult(const SEnzymeData& _enzyme, int _pos, const U2Strand& _strand)
        : enzyme(_enzyme), pos(_pos), strand(_strand) {
    }
    SEnzymeData enzyme;
    int pos;
    U2Strand strand;
};

class FindEnzymesTask;

struct FindEnzymesTaskConfig {
    /** Region to search enzymes. When is empty the whole sequence range is processed. */
    U2Region searchRegion;

    /** List of regions to exclude from the search. */
    QVector<U2Region> excludedRegions;

    /** Group name for the result annotations. */
    QString groupName;

    /** Maximum result count after search task will be stopped. */
    int maxResults = INT_MAX;

    /** If the results count is less than 'minHitCount' there will be no annotations created. */
    int minHitCount = 1;

    /** If the results count is greater than 'maxHitCount' there will be no annotations created. */
    int maxHitCount = INT_MAX;

    /** If true, the sequence is circular and results must also be searched in start/end overlapped regions. */
    bool circular = false;

    /** If task is not Auto-Annotation-Update task and no results is found the target 'annotationObject' will be removed from the project. */
    bool isAutoAnnotationUpdateTask = false;
};

class FindEnzymesToAnnotationsTask : public Task {
    Q_OBJECT
public:
    FindEnzymesToAnnotationsTask(AnnotationTableObject* aobj, const U2EntityRef& seqRef, const QList<SEnzymeData>& enzymes, const FindEnzymesTaskConfig& cfg);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;

private:
    U2EntityRef dnaSeqRef;
    QList<SEnzymeData> enzymes;
    QPointer<AnnotationTableObject> annotationObject;
    FindEnzymesTaskConfig cfg;
    FindEnzymesTask* findTask;
};

class FindEnzymesTask : public Task, public FindEnzymesAlgListener {
    Q_OBJECT
public:
    FindEnzymesTask(const U2EntityRef& seqRef, const U2Region& region, const QList<SEnzymeData>& enzymes, int maxResults = 0x7FFFFFFF, bool _circular = false, QVector<U2Region> excludedRegions = QVector<U2Region>());

    void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& stand) override;

    ReportResult report() override;

    QList<SharedAnnotationData> getResultsAsAnnotations(const QString& enzymeId) const;

    void cleanup() override;

private:
    int maxResults;
    QVector<U2Region> excludedRegions;
    bool isCircular;
    int seqlen;

    /** Map of search results by enzyme id. */
    QMap<QString, QList<FindEnzymesAlgResult>> searchResultMap;
    int countOfResultsInMap;
    QMutex resultsLock;

    QString group;
};

class FindSingleEnzymeTask : public Task, public FindEnzymesAlgListener, public SequenceDbiWalkerCallback {
    Q_OBJECT
public:
    FindSingleEnzymeTask(const U2EntityRef& sequenceObjectRef, const U2Region& region, const SEnzymeData& enzyme, FindEnzymesAlgListener* l = nullptr, bool isCircular = false, int maxResults = 0x7FFFFFFF);

    void prepare() override;

    QList<FindEnzymesAlgResult> getResults() const {
        return resultList;
    }
    void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand) override;
    void onRegion(SequenceDbiWalkerSubtask* t, TaskStateInfo& ti) override;
    void cleanup() override;

    /**
     * Returns estimation for a number of results found both strands of the sequence of the given length
     * for a 'numberOfVariants' different enzyme kinds.
     */
    static qint64 estimateNumberOfEnzymesInSequence(qint64 sequenceLength, int numberOfVariants = 1);

private:
    U2EntityRef sequenceObjectRef;
    U2Region region;
    SEnzymeData enzyme;
    int maxResults;
    FindEnzymesAlgListener* resultListener;
    QList<FindEnzymesAlgResult> resultList;
    QMutex resultsLock;
    bool isCircular;
};

class FindEnzymesAutoAnnotationUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
public:
    FindEnzymesAutoAnnotationUpdater();

    Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* annotationObject) override;

    bool checkConstraints(const AutoAnnotationConstraints& constraints) override;

    /** Returns last saved search region for the given sequence object or empty region if no region was saved. */
    static U2Region getLastSearchRegionForObject(const U2SequenceObject* sequenceObject);

    /**
     * Saves the region as last used 'search' region for the object.
     * This region will be used by default during the next auto-annotation task run.
     * If no region is set, the whole sequence will be processed.
     */
    static void setLastSearchRegionForObject(U2SequenceObject* sequenceObject, const U2Region& region);

    /** Returns last saved 'excluded' region for the given sequence object or empty region if no region was saved. */
    static U2Region getLastExcludeRegionForObject(const U2SequenceObject* sequenceObject);

    /**
     * Saves the region as last used 'exclude' region for the object.
     * This region will be used by default during the next auto-annotation task run.
     * If no region is set, the whole sequence will be processed.
     */
    static void setLastExcludeRegionForObject(U2SequenceObject* sequenceObject, const U2Region& region);

    /** Returns true if the task can safely be started for the given sequence length and number of enzymes. */
    static bool isTooManyAnnotationsInTheResult(qint64 sequenceLength, int countOfEnzymeVariants);
};

}  // namespace U2

#endif
