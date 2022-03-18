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

#include "CreateAnnotationTask.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>

#include "LoadDocumentTask.h"

namespace U2 {

CreateAnnotationsTask::CreateAnnotationsTask(AnnotationTableObject* ao, const QList<SharedAnnotationData>& data, const QString& g)
    : Task(tr("Create annotations"), TaskFlags_FOSE_COSC), annotationTableObjectPointer(ao) {
    annotationDatasByGroupNameMap.insert(g, data);

    initAnnObjectRef();
    CHECK_OP(stateInfo, );
    tpm = Progress_Manual;
}

CreateAnnotationsTask::CreateAnnotationsTask(const GObjectReference& r, const QList<SharedAnnotationData>& data, const QString& g)
    : Task(tr("Create annotations"), TaskFlags_FOSE_COSC), aRef(r) {
    annotationDatasByGroupNameMap.insert(g, data);

    GObject* ao = GObjectUtils::selectObjectByReference(aRef, UOF_LoadedAndUnloaded);
    if (ao != nullptr && ao->isUnloaded()) {
        addSubTask(new LoadUnloadedDocumentTask(ao->getDocument()));
    }
    tpm = Progress_Manual;
}

CreateAnnotationsTask::CreateAnnotationsTask(AnnotationTableObject* annotationTableObject, const QMap<QString, QList<SharedAnnotationData>>& annotationsByGroupMap)
    : Task(tr("Create annotations"), TaskFlags_FOSE_COSC), annotationTableObjectPointer(annotationTableObject), annotationDatasByGroupNameMap(annotationsByGroupMap) {
    initAnnObjectRef();
    CHECK_OP(stateInfo, );
    tpm = Progress_Manual;
}

void CreateAnnotationsTask::initAnnObjectRef() {
    AnnotationTableObject* parentObject = getAnnotationTableObject();
    CHECK_EXT(parentObject != nullptr, setError(tr("Annotation table has been removed unexpectedly")), );
    aRef.objName = parentObject->getGObjectName();
}

void CreateAnnotationsTask::run() {
    AnnotationTableObject* annotationTableObject = getAnnotationTableObject();
    CHECK_EXT(annotationTableObject != nullptr, setError(tr("Annotation table has been removed unexpectedly")), );

    DbiOperationsBlock opBlock(annotationTableObject->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    const U2DataId rootFeatureId = annotationTableObject->getRootFeatureId();
    const U2DbiRef dbiRef = annotationTableObject->getEntityRef().dbiRef;

    DbiOperationsBlock dbiOperationsBlock(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    const QList<QString>& groupNameList = annotationDatasByGroupNameMap.keys();
    for (const QString& groupName : qAsConst(groupNameList)) {
        const QList<SharedAnnotationData>& annotationsInGroup = annotationDatasByGroupNameMap[groupName];
        if (groupName.isEmpty()) {
            for (const SharedAnnotationData& annotation : qAsConst(annotationsInGroup)) {
                AnnotationGroup* group = annotationTableObject->getRootGroup()->getSubgroup(annotation->name, true);
                U2Feature feature = U2FeatureUtils::exportAnnotationDataToFeatures(annotation, rootFeatureId, group->id, dbiRef, stateInfo);
                CHECK_OP(stateInfo, );
                annotationsByGroupMap[group] << new Annotation(feature.id, annotation, group, annotationTableObject);
            }
        } else {
            AnnotationGroup* group = annotationTableObject->getRootGroup()->getSubgroup(groupName, true);
            QList<U2Feature> features = U2FeatureUtils::exportAnnotationDataToFeatures(annotationsInGroup, rootFeatureId, group->id, dbiRef, stateInfo);
            CHECK_OP(stateInfo, );
            SAFE_POINT(features.size() == annotationsInGroup.size(), "Wrong features list size", );
            for (int i = 0; i < annotationsInGroup.size(); i++) {
                annotationsByGroupMap[group] << new Annotation(features[i].id, annotationsInGroup[i], group, annotationTableObject);
            }
        }
    }
}

Task::ReportResult CreateAnnotationsTask::report() {
    if (hasError() || isCanceled() || annotationsByGroupMap.isEmpty()) {
        return ReportResult_Finished;
    }
    AnnotationTableObject* annotationObject = getAnnotationTableObject();
    if (annotationObject == nullptr) {
        setError(tr("Annotation object '%1' not found in active project: %2").arg(aRef.objName).arg(aRef.docUrl));
        return ReportResult_Finished;
    }

    const QList<AnnotationGroup*>& groupList = annotationsByGroupMap.keys();
    for (AnnotationGroup* group : qAsConst(groupList)) {
        const QList<Annotation*>& annotationsInGroup = annotationsByGroupMap[group];
        // Add annotation not as new but as existing. We will notify parent object about update later.
        group->addShallowAnnotations(annotationsInGroup, false);
        resultAnnotations.append(annotationsInGroup);
    }
    // Notify annotation object that new annotations were added once.
    annotationObject->setModified(true);
    annotationObject->emit_onAnnotationsAdded(resultAnnotations);

    return ReportResult_Finished;
}

AnnotationTableObject* CreateAnnotationsTask::getAnnotationTableObject() const {
    if (aRef.isValid()) {
        SAFE_POINT(annotationTableObjectPointer.isNull(), "Unexpected annotation table object content!", nullptr);
        return qobject_cast<AnnotationTableObject*>(GObjectUtils::selectObjectByReference(aRef, UOF_LoadedOnly));
    }
    return annotationTableObjectPointer.data();
}

int CreateAnnotationsTask::getAnnotationCount() const {
    int result = 0;
    foreach (const QString& groupName, annotationDatasByGroupNameMap.keys()) {
        result += annotationDatasByGroupNameMap[groupName].size();
    }
    return result;
}

QList<Annotation*> CreateAnnotationsTask::getResultAnnotations() const {
    return resultAnnotations;
}

}  // namespace U2
