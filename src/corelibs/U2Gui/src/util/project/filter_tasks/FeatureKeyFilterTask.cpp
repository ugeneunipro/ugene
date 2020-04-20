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

#include <U2Core/L10n.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/U2SafePoints.h>

#include "../ProjectFilterNames.h"

#include "FeatureKeyFilterTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// FeatureKeyFilterTask
//////////////////////////////////////////////////////////////////////////

FeatureKeyFilterTask::FeatureKeyFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs)
    : AbstractProjectFilterTask(settings, ProjectFilterNames::FEATURE_KEY_FILTER_NAME, docs)
{

}

void FeatureKeyFilterTask::run() {
    foreach (const QPointer<Document> &doc, docs) {
        if (!doc.isNull()) {
            filterDocument(doc.data());
        }
        if (stateInfo.isCoR()) {
            break;
        }
    }
}

void FeatureKeyFilterTask::filterDocument(Document *doc) {
    SAFE_POINT_EXT(NULL != doc, stateInfo.setError(L10N::nullPointerError("document")), );
    CHECK(doc->isLoaded(), );

    const U2DbiRef dbiRef = doc->getDbiRef();
    SAFE_POINT_EXT(dbiRef.isValid(), setError(tr("DbiRef is invalid")), );
    if (!dbiRef2AnnotationTables.contains(dbiRef)) {
        DbiConnection connection(dbiRef, stateInfo);
        CHECK_OP(stateInfo, );
        SAFE_POINT_EXT(NULL != connection.dbi, stateInfo.setError(L10N::nullPointerError("Database connection")), );
        U2FeatureDbi *featureDbi = connection.dbi->getFeatureDbi();
        SAFE_POINT_EXT(NULL != featureDbi, stateInfo.setError(L10N::nullPointerError("Feature DBI")), );

        dbiRef2AnnotationTables[dbiRef] = featureDbi->getAnnotationTablesByFeatureKey(settings.tokensToShow, stateInfo);
        SAFE_POINT_OP(stateInfo, );
    }

    const QMap<U2DataId, QStringList> &annNames = dbiRef2AnnotationTables[dbiRef];
    const int foundObjectsNumber = annNames.size();
    const int totalDocObjectsNumber = doc->getObjects().size();
    foreach (const U2DataId &annTableId, annNames.keys()) {
        GObject *annTable = doc->getObjectById(annTableId);
        if (NULL == annTable) {
            coreLog.error("Annotation table object not found in the document");
            continue;
        }
        SafeObjList filteredResult;
        filteredResult.append(annTable);
        foreach (const QString &filterName, annNames[annTableId]) {
            emit si_objectsFiltered(filterName, filteredResult);
        }
        stateInfo.setProgress(stateInfo.getProgress() + (totalDocObjectsNumber / foundObjectsNumber / totalObjectCount) * 100);
        if (stateInfo.isCoR()) {
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
/// FeatureKeyFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask * FeatureKeyFilterTaskFactory::createNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    return new FeatureKeyFilterTask(settings, docs);
}

} // namespace U2
