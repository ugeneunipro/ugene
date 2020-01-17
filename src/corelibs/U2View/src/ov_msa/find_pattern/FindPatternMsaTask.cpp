/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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
#include "../../ov_sequence/find_pattern/FindPatternTask.h"

#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

FindPatternMsaSettings::FindPatternMsaSettings()
    : msaObj(nullptr),
    removeOverlaps(false),
    matchValue(0) {

}

FindPatternMsaTask::FindPatternMsaTask(const FindPatternMsaSettings& _settings)
    : Task(tr("Searching a pattern in multiple alignment task"), TaskFlags_NR_FOSE_COSC),
    settings(_settings),
    currentSequenceIndex(0),
    searchInSingleSequenceTask(nullptr),
    totalResultsCounter(0) {
}

void FindPatternMsaTask::prepare() {
    createSearchTaskForCurrentSequence();
    addSubTask(searchInSingleSequenceTask);
}

void FindPatternMsaTask::createSearchTaskForCurrentSequence() {
    FindAlgorithmTaskSettings algoSettings;
    algoSettings.searchIsCircular = false;
    algoSettings.strand = FindAlgorithmStrand_Direct;
    //TODO: UGENE-6675
    algoSettings.maxResult2Find = FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED;
    algoSettings.useAmbiguousBases = false;
    algoSettings.patternSettings = settings.findSettings.patternSettings;
    algoSettings.sequenceAlphabet = settings.msaObj->getAlphabet();
    algoSettings.searchIsCircular = false;
    QByteArray seq = settings.msaObj->getRow(currentSequenceIndex)->getUngappedSequence().constSequence();
    FindAlgorithmTaskSettings currentSettings = algoSettings;
    currentSettings.sequence = seq;
    currentSettings.searchRegion = settings.msaObj->getRow(currentSequenceIndex)->getUngappedRegion(settings.findSettings.searchRegion);
    searchInSingleSequenceTask = new FindPatternListTask(currentSettings, settings.patterns, settings.removeOverlaps, settings.matchValue);
    return;
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
    if (!searchInSingleSequenceTask->getResults().isEmpty()) {
        QList<U2Region> resultRegions;
        foreach(const SharedAnnotationData & data, searchInSingleSequenceTask->getResults()) {
            if (totalResultsCounter >= settings.findSettings.maxResult2Find) {
                break;
            }
            QList<U2Region> gappedRegionList;
            resultRegions.append(settings.msaObj->getMultipleAlignment()->getRow(currentSequenceIndex).data()->getGapped(data->getRegions().first()));
            totalResultsCounter++;
        }
        if (settings.findSettings.patternSettings == FindAlgorithmPatternSettings_RegExp || settings.patterns.size() > 1) { //Other algos always return sorted results
            qSort(resultRegions.begin(), resultRegions.end());
        }
        resultsBySeqIndex.insert(currentSequenceIndex, resultRegions);
    }
    currentSequenceIndex++;
}

const QMap<int, QList<U2::U2Region> >& FindPatternMsaTask::getResults() const {
    return resultsBySeqIndex;
}

}