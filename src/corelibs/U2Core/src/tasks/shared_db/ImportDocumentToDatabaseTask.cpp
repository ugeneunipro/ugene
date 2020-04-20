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

#include <U2Core/Counter.h>
#include <U2Core/Folder.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ImportDocumentToDatabaseTask.h"
#include "ImportObjectToDatabaseTask.h"

namespace U2 {

ImportDocumentToDatabaseTask::ImportDocumentToDatabaseTask(Document* document, const U2DbiRef& dstDbiRef, const QString& dstFolder, const ImportToDatabaseOptions& options) :
Task(tr("Import document %1 to the database").arg(NULL != document ? document->getName() : ""), TaskFlag_NoRun),
document(document),
dstDbiRef(dstDbiRef),
dstFolder(dstFolder),
options(options) {
    GCOUNTER(cvar, tvar, "ImportDocumentToDatabaseTask");
    CHECK_EXT(NULL != document, setError(tr("Invalid document to import")), );
    CHECK_EXT(dstDbiRef.isValid(), setError(tr("Invalid database reference")), );

    setMaxParallelSubtasks(1);

    if (options.createSubfolderForEachDocument) {
        this->dstFolder = Folder::createPath(dstFolder, document->getName());
    }
}

void ImportDocumentToDatabaseTask::prepare() {
    foreach(GObject* object, document->getObjects()) {
        addSubTask(new ImportObjectToDatabaseTask(object, dstDbiRef, dstFolder));
    }
}

QList<Task *> ImportDocumentToDatabaseTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> res;
    CHECK(isTopLevelTask(), res);

    if (subTask->hasError()) {
        taskLog.error(QString("Subtask '%1' fails: %2").arg(subTask->getTaskName()).arg(subTask->getError()));
    }

    if (subTask->isCanceled()) {
        taskLog.error(QString("Subtask '%1' was cancelled").arg(subTask->getTaskName()));
    }

    return res;
}

Task::ReportResult ImportDocumentToDatabaseTask::report() {
    CHECK_EXT(!document.isNull(), setError(tr("Source document was removed, can't set object relations")), ReportResult_Finished);

    QStringList errors;
    propagateObjectsRelations(errors);
    if (!errors.isEmpty()) {
        setError(errors.join("\n"));
    }

    return ReportResult_Finished;
}

Document *ImportDocumentToDatabaseTask::getSourceDocument() const {
    return document.data();
}

QStringList ImportDocumentToDatabaseTask::getImportedObjectNames() const {
    QStringList result;
    const QMap<GObject *, GObject *> objects = getObjectPairs();
    foreach(GObject* object, objects) {
        result << object->getGObjectName();
    }

    return result;
}

QStringList ImportDocumentToDatabaseTask::getSkippedObjectNames() const {
    QStringList result;
    foreach(const QPointer<Task> &subtask, getSubtasks()) {
        if (subtask->isCanceled() || subtask->hasError()) {
            ImportObjectToDatabaseTask* importObjectTask = qobject_cast<ImportObjectToDatabaseTask*>(subtask);
            if (NULL != importObjectTask) {
                GObject* object = importObjectTask->getSourceObject();
                if (NULL != object) {
                    result << object->getGObjectName();
                }
            }
        }
    }

    return result;
}

const QString & ImportDocumentToDatabaseTask::getDstFolder() const {
    return dstFolder;
}

const U2DbiRef & ImportDocumentToDatabaseTask::getDstDbiRef() const {
    return dstDbiRef;
}

QSet<GObject *> ImportDocumentToDatabaseTask::getImportedObjects() const {
    return getObjectPairs().values().toSet();
}

QMap<GObject *, GObject *> ImportDocumentToDatabaseTask::getObjectPairs() const {
    QMap<GObject *, GObject *> objects;
    foreach(const QPointer<Task> &subtask, getSubtasks()) {
        if (!subtask->isCanceled() && !subtask->hasError()) {
            ImportObjectToDatabaseTask* importObjectTask = qobject_cast<ImportObjectToDatabaseTask*>(subtask);
            if (NULL != importObjectTask) {
                GObject* srcObject = importObjectTask->getSourceObject();
                GObject* dstObject = importObjectTask->getDestinationObject();
                if (NULL != srcObject && NULL != dstObject) {
                    objects.insert(srcObject, dstObject);
                }
            }
        }
    }

    return objects;
}

void ImportDocumentToDatabaseTask::propagateObjectsRelations(QStringList& errors) const {
    const QMap<GObject *, GObject *> objects = getObjectPairs();
    const QString srcDocUrl = document->getURLString();

    foreach(GObject* srcObject, objects.keys()) {
        GObject* dstObject = objects.value(srcObject);
        dstObject->setObjectRelations(QList<GObjectRelation>());

        QList<GObjectRelation> relations = srcObject->getObjectRelations();
        foreach(const GObjectRelation& relation, relations) {
            if (srcDocUrl != relation.getDocURL()) {
                // skip this relation: target object is not imported to the database
                continue;
            }

            GObject* srcRelationTargetObject = document->getObjectById(relation.ref.entityRef.entityId);
            if (NULL == srcRelationTargetObject) {
                errors << tr("Can't set object relation: target object is not found in the source document (%1)").arg(relation.ref.objName);
                continue;
            }
            if (!objects.keys().contains(srcRelationTargetObject)) {
                errors << tr("Can't set object relation: target object is not imported (%1)").arg(srcRelationTargetObject->getGObjectName());
                continue;
            }

            GObject* dstRelationTargetObject = objects.value(srcRelationTargetObject);
            GObjectReference dstReference(U2DbiUtils::ref2Url(dstDbiRef),
                dstRelationTargetObject->getGObjectName(),
                dstRelationTargetObject->getGObjectType(),
                dstRelationTargetObject->getEntityRef());
            GObjectRelation dstRelation(dstReference, relation.role);
            dstObject->addObjectRelation(dstRelation);
        }
    }
}

GObject *ImportDocumentToDatabaseTask::getAppropriateObject(const QList<GObject *> objects, const GObject *pattern) {
    CHECK(NULL != pattern, NULL);

    foreach(GObject* object, objects) {
        if (object->getGObjectName() == pattern->getGObjectName() &&
            object->getGObjectType() == pattern->getGObjectType()) {
            return object;
        }
    }

    return NULL;
}

}   // namespace U2
