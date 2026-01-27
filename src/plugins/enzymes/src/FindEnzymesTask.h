/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <limits>

#include <QList>
#include <QMutex>
#include <QObject>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
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
    /** Regions to search enzymes. When is empty the whole sequence range is processed. */
    QVector<U2Region> searchRegions;

    /** Regions to exclude from the search. */
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

    /** Exclude enzymes from result by region or by enzymes found in region */
    bool excludeMode = false;
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
    QList<Task*> searchEnzymesTasks;
};

class FindEnzymesTask : public Task, public FindEnzymesAlgListener {
    Q_OBJECT
public:
    FindEnzymesTask(const U2EntityRef& seqRef, const U2Region& region, const QVector<U2Region>& excludeRegions, 
                    const QList<SEnzymeData>& enzymes, int maxResults = 0x7FFFFFFF, bool _circular = false);
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand, bool& stop) override;

    ReportResult report() override;

    QList<SharedAnnotationData> getResultsAsAnnotations(const QString& enzymeId) const;

    void cleanup() override;

private:
    int maxResults;
    bool isCircular;
    int seqlen;

    /** Map of search results by enzyme id. */
    QMap<QString, QList<FindEnzymesAlgResult>> searchResultMap;
    int countOfResultsInMap;
    QMutex resultsLock;

    QString group;

    QList<SEnzymeData> enzymes;
    U2EntityRef seqRef;
    U2Region region;
    QMap<QString, int> excludeSearchTasksRunningCounter;
    QVector<U2Region> excludeRegions;
    QSet<QString> enzymesFoundInExcludedRegion;
};

class FindSingleEnzymeTask : public Task, public FindEnzymesAlgListener, public SequenceDbiWalkerCallback {
    Q_OBJECT
public:
    /*
     * \param sequenceObjectRef Reference to DB representation of sequence object.
     * \param region Region to search enzymes in.
     * \param enzyme Enzyme to find.
     * \param l Every time result is found this object onResult will be called.
     * \param isCircular Is sequence circular.
     * \param maxResults Maximum number of results. Cancel and, optionally (see @failOnMaxResultExceed), fail if more results.
     * \param cancelOnMaxResults Cancel if more than @maxResults if true or just set onResult::stop to false if false.
    **/
    FindSingleEnzymeTask(const U2EntityRef& sequenceObjectRef, const U2Region& region, const SEnzymeData& enzyme, FindEnzymesAlgListener* l = nullptr, bool isCircular = false, int maxResults = 0x7FFFFFFF, bool cancelOnMaxResults = true);

    void prepare() override;

    QList<FindEnzymesAlgResult> getResults() const {
        return resultList;
    }
    void onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand, bool& stop) override;
    void onRegion(SequenceDbiWalkerSubtask* t, TaskStateInfo& ti) override;
    void cleanup() override;
    SEnzymeData getEnzyme() const;
    bool wasStoppedOnMaxResults() const;

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
    bool cancelOnMaxResults = true;
    bool stoppedOnMaxResults = false;
};

class FindEnzymesAutoAnnotationUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
public:

    FindEnzymesAutoAnnotationUpdater();

    Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* annotationObject) override;

    bool checkConstraints(const AutoAnnotationConstraints& constraints) override;

    /** Returns last saved search location for the given sequence object or empty region if no region was saved. */
    static U2Location getLastSearchLocationForObject(const U2SequenceObject* sequenceObject);

    /**
     * Saves the location as last used 'search' region for the object.
     * This region will be used by default during the next auto-annotation task run.
     * If no region is set, the whole sequence will be processed.
     */
    static void setLastSearchLocationForObject(U2SequenceObject* sequenceObject, const U2Location& location);

    /** Returns last saved 'excluded' location for the given sequence object or empty region if no region was saved. */
    static U2Location getLastExcludeLocationForObject(const U2SequenceObject* sequenceObject);

    /**
     * Saves the region as last used 'exclude' location for the object.
     * This region will be used by default during the next auto-annotation task run.
     * If no region is set, the whole sequence will be processed.
     */
    static void setLastExcludeLocationForObject(U2SequenceObject* sequenceObject, const U2Location& region);

    /** Returns true if the task can safely be started for the given sequence length and number of enzymes. */
    static bool isTooManyAnnotationsInTheResult(qint64 sequenceLength, int countOfEnzymeVariants);
    
    /** Set exclude mode for the given sequence object. */
    static void setExcludeModeEnabledForObject(U2SequenceObject* sequenceObject, bool enabled);

    /** Returns exclude mode for the given sequence object. */
    static bool getExcludeModeEnabledForObject(U2SequenceObject* sequenceObject);
};

}  // namespace U2
