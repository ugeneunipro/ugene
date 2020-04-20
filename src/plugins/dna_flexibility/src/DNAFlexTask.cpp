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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DNAFlexTask.h"
#include "FindHighFlexRegions.h"

namespace U2 {

DNAFlexTask::DNAFlexTask(const HighFlexSettings& _settings,
        AnnotationTableObject *_annotObject,
        const QString& _annotName,
        const QString& _annotGroup,
        const QString &annDescription,
        const DNASequence& _sequence)

    : Task(tr("DNA Flexibility task"), TaskFlags_NR_FOSCOE),
      settings(_settings),
      annotObject(_annotObject),
      annotName(_annotName),
      annotGroup(_annotGroup),
      annDescription(annDescription),
      sequence(_sequence)
{
    addSubTask(findHighFlexTask = new FindHighFlexRegions(_sequence, settings));
}

QList<Task*> DNAFlexTask::onSubTaskFinished(Task* subTask) {
    QList<Task *> resultsList;

    if (subTask->hasError() && subTask == findHighFlexTask) {
        stateInfo.setError(subTask->getError());
    }

    if (hasError() || isCanceled()) {
        return resultsList;
    }

    if (annotObject.isNull()) {
        stateInfo.setError(tr("The annotations object is not available!"));
        return resultsList;
    }

    if (subTask == findHighFlexTask) {
        FindHighFlexRegions* findTask = qobject_cast<FindHighFlexRegions*>(findHighFlexTask);
        SAFE_POINT(findTask, "Failed to cast FindHighFlexRegions task!", QList<Task*>());

        QList<HighFlexResult> results = findTask->getResults();
        QList<SharedAnnotationData> annots = getAnnotationsFromResults(results);

        if(!annots.isEmpty()) {
            resultsList.append(new CreateAnnotationsTask(annotObject, annots, annotGroup));
        }
    }
    return resultsList;
}

QList<SharedAnnotationData> DNAFlexTask::getAnnotationsFromResults(const QList<HighFlexResult> &results) {
    QList<SharedAnnotationData> annotResults;

    foreach (const HighFlexResult &result, results) {
        SharedAnnotationData annotData(new AnnotationData);
        annotData->name = annotName;
        annotData->location->regions << result.region;

        annotData->qualifiers.append(U2Qualifier("average_threshold", QString::number(result.averageThreshold, 'f', 3)));
        annotData->qualifiers.append(U2Qualifier("windows_number", QString::number(result.windowsNumber)));
        annotData->qualifiers.append(U2Qualifier("total_threshold", QString::number(result.totalThreshold, 'f', 3)));

        U1AnnotationUtils::addDescriptionQualifier(annotData, annDescription);


        annotResults.append(annotData);
    }
    return annotResults;
}

} // namespace
