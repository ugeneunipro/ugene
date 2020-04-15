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

#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include "FindPatternTask.h"

namespace U2 {

const float FindPatternTask::MAX_OVERLAP_K = 0.5F;

FindPatternTask::FindPatternTask(const FindAlgorithmTaskSettings &settings, bool removeOverlaps)
    : Task(tr("Searching a pattern in sequence task"), TaskFlags_NR_FOSE_COSC),
      settings(settings),
      removeOverlaps(removeOverlaps),
      findAlgorithmTask(NULL),
      noResults(false)
{

}

QList<Task *> FindPatternTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    if (subTask->hasError() && subTask == findAlgorithmTask) {
        stateInfo.setError(subTask->getError());
        return res;
    }

    if (subTask == findAlgorithmTask) {
        FindAlgorithmTask *task = qobject_cast<FindAlgorithmTask *>(findAlgorithmTask);
        SAFE_POINT(task, "Failed to cast FindAlgorithTask!", QList<Task *>());

        QList<FindAlgorithmResult> resultz = task->popResults();
        if (settings.patternSettings == FindAlgorithmPatternSettings_RegExp) { //Other algos always return sorted results
            qSort(resultz.begin(), resultz.end(), FindAlgorithmResult::lessByRegionStartPos);
        }
        if (removeOverlaps && !resultz.isEmpty()) {
            removeOverlappedResults(resultz);
        }

        results.append(FindAlgorithmResult::toTable(resultz, settings.name, settings.searchIsCircular, settings.sequence.size()));
    }

    return res;
}

void FindPatternTask::removeOverlappedResults(QList<FindAlgorithmResult> &results) {
    int numberBefore = results.count();

    for (int i = 0, n = results.count(); i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (stateInfo.isCoR()) {
                return;
            }
            SAFE_POINT(results.at(j).region.startPos >= results.at(i).region.startPos,
                "Internal error: inconsistence between regions start positions."
                "Skipping further removing of overlapped results.",);

            if (results.at(j).strand != results.at(i).strand) {
                continue;
            }

            if (results.at(j).translation != results.at(i).translation) {
                continue;
            }

            if (results.at(j).translation) {
                U2Strand strand = results.at(j).strand;
                int framej = strand == U2Strand::Complementary ? results.at(j).region.endPos() % 3 : results.at(j).region.startPos % 3;
                int framei = strand == U2Strand::Complementary ? results.at(i).region.endPos() % 3 : results.at(i).region.startPos % 3;
                if (framei != framej) {
                    continue;
                }
            }

            U2Region region = results.at(j).region.intersect(results.at(i).region);
            if ((region.length > 0) && (region.length >= MAX_OVERLAP_K * results.at(i).region.length)) {
                results.removeAt(j);
                j--;
                n--;
            }
            else {
                break;
            }
        }
    }

    int removed = numberBefore - results.count();
    coreLog.info(tr("Removed %1 overlapped results.").arg(removed));
}

const QList<SharedAnnotationData> & FindPatternTask::getResults() const {
    return results;
}

void FindPatternTask::prepare() {
    // Note that even if the subtask has been canceled, the already calculated results are
    // saved anyway. This mechanism is used to limit the number of results.
    addSubTask(findAlgorithmTask = new FindAlgorithmTask(settings));
}

FindPatternListTask::FindPatternListTask(const FindAlgorithmTaskSettings &settings, const QList<NamePattern> &patterns, bool removeOverlaps, int match)
    : Task( tr( "Searching patterns in sequence task" ), TaskFlags_NR_FOSE_COSC), settings(settings), removeOverlaps(removeOverlaps),
    match(match), noResults(true), patterns(patterns)
{

}

QList<Task *> FindPatternListTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;
    FindPatternTask *task = qobject_cast<FindPatternTask *>(subTask);
    SAFE_POINT(NULL != task, "Failed to cast FindPatternTask!", QList<Task *>());
    if (!task->hasNoResults()) {
        noResults = false;
    }
    results.append(task->getResults());
    return res;
}

int FindPatternListTask::getMaxError(const QString &pattern) const {
    if (settings.patternSettings == FindAlgorithmPatternSettings_Exact) {
        return 0;
    }
    return int((float)(1 - float(match) / 100) * pattern.length());
}

const QList<SharedAnnotationData> & FindPatternListTask::getResults() const {
    return results;
}

bool FindPatternListTask::hasNoResults() const {
    return noResults;
}

void FindPatternListTask::prepare() {
    foreach (const NamePattern &pattern, patterns) {
        if (pattern.second.isEmpty()) {
            uiLog.error(tr("Empty pattern"));
            continue;
        }
        FindAlgorithmTaskSettings subTaskSettings = settings;
        subTaskSettings.pattern = pattern.second.toUtf8();
        subTaskSettings.maxErr = getMaxError( subTaskSettings.pattern );
        subTaskSettings.name = pattern.first;
        subTaskSettings.countTask = false;
        FindPatternTask *task = new FindPatternTask(subTaskSettings, removeOverlaps);
        addSubTask(task);
    }
}

} // namespace
