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
#include "AddObjectsToDocumentTask.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/U2SafePoints.h>

#include <QCoreApplication>

namespace U2 {

AddObjectsToDocumentTask::AddObjectsToDocumentTask(QList<GObject*> &_objects, Document *_doc)
    : Task(tr("Add objects to document"), TaskFlags_FOSE_COSC | TaskFlag_NoRun), objects(_objects), doc(_doc) {}

void AddObjectsToDocumentTask::prepare() {
    foreach(GObject *obj, objects) {
        if (obj->isUnloaded()) {
            continue;
        }
        addSubTask(new CloneObjectTask(obj, doc, U2ObjectDbi::ROOT_FOLDER));
    }
}

QList<Task*> AddObjectsToDocumentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CloneObjectTask *cloneTask = qobject_cast<CloneObjectTask*>(subTask);
    CHECK(cloneTask != NULL, result);
    doc->addObject(cloneTask->takeResult());
    return result;
}

}
