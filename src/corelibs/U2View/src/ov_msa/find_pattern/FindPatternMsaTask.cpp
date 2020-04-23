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

#include "FindPatternMsaTask.h"
#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

FindPatternMsaSettings::FindPatternMsaSettings()
        : msaObj(nullptr),
          removeOverlaps(false),
          matchValue(100) {
}

FindPatternMsaTask::FindPatternMsaTask(const FindPatternMsaSettings& settings)
        : Task(tr("Searching a pattern in multiple alignment task"), TaskFlags_NR_FOSE_COSC),
          settings(settings),
          currentSequenceIndex(0),
          searchInSingleSequenceTask(nullptr),
          totalResultsCounter(0) {
}

void FindPatternMsaTask::prepare() {
    createSearchTaskForCurrentSequence();
    addSubTask(searchInSingleSequenceTask);
}

void FindPatternMsaTask::createSearchTaskForCurrentSequence() {
    FindAlgorithmTaskSettings findPatternSettings;
    findPatternSettings.searchIsCircular = false;
    findPatternSettings.strand = FindAlgorithmStrand_Direct;
    //TODO: UGENE-6675
    findPatternSettings.maxResult2Find = FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED;
    findPatternSettings.useAmbiguousBases = false;
    findPatternSettings.maxRegExpResultLength = settings.findSettings.maxRegExpResultLength;
    findPatternSettings.patternSettings = settings.findSettings.patternSettings;
    findPatternSettings.sequenceAlphabet = settings.msaObj->getAlphabet();
    findPatternSettings.searchIsCircular = false;
    findPatternSettings.sequence = settings.msaObj->getRow(currentSequenceIndex)->getUngappedSequence().constSequence();
    findPatternSettings.searchRegion = settings.msaObj->getRow(currentSequenceIndex)->getUngappedRegion(settings.findSettings.searchRegion);
    searchInSingleSequenceTask = new FindPatternListTask(findPatternSettings, settings.patterns, settings.removeOverlaps, settings.matchValue);
}

QList<Task*> FindPatternMsaTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    if (subTask->isCanceled()) {
        return result;
    }
    if (subTask->hasError() && subTask == searchInSingleSequenceTask) {
        stateInfo.setError(subTask->getError());
        return result;
    }

    if (subTask == searchInSingleSequenceTask) {
        getResultFromTask();
        if (currentSequenceIndex < settings.msaObj->getNumRows() && totalResultsCounter < settings.findSettings.maxResult2Find) {
            createSearchTaskForCurrentSequence();
            result.append(searchInSingleSequenceTask);
        }
    }
    return result;
}

void FindPatternMsaTask::getResultFromTask() {
    const QList<SharedAnnotationData>& rowResults = searchInSingleSequenceTask->getResults();
    if (rowResults.isEmpty()) {
        currentSequenceIndex++;
        return;
    }
    const MultipleAlignment& multipleAlignment = settings.msaObj->getMultipleAlignment();
    QList<U2Region> regions;
    const MultipleAlignmentRow& msaRow = multipleAlignment->getRow(currentSequenceIndex);
    for (int i = 0; i < rowResults.length() && totalResultsCounter < settings.findSettings.maxResult2Find; i++) {
        const SharedAnnotationData& annotationData = rowResults[i];
        const U2Region& resultRegion = annotationData->getRegions().first();
        const U2Region& resultRegionWithGaps = msaRow.data()->getGapped(resultRegion);
        regions.append(resultRegionWithGaps);
        totalResultsCounter++;
    }
    qSort(regions.begin(), regions.end());
    qint64 rowId = msaRow->getRowId();
    results.insert(rowId, FindPatternInMsaResult(rowId, regions));
    currentSequenceIndex++;
}

const QList<FindPatternInMsaResult>& FindPatternMsaTask::getResults() const {
    return results;
}

FindPatternInMsaResult::FindPatternInMsaResult(qint64 rowId, const QList<U2Region>& regions)
        : rowId(rowId), regions(regions) {
}

}
