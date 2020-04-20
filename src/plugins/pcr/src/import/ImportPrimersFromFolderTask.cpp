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

#include <U2Core/U2DbiUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ImportPrimersFromFolderTask.h"
#include "ImportPrimerFromObjectTask.h"

namespace U2 {

ImportPrimersFromFolderTask::ImportPrimersFromFolderTask(const Folder &folder) :
    Task(tr("Import primers from the shared database folder: %1").arg(folder.getFolderPath()),
         TaskFlags(TaskFlag_NoRun | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported)),
    folder(folder)
{
    SAFE_POINT_EXT(NULL != folder.getDocument(), setError(L10N::nullPointerError("folder's document")), );
}

void ImportPrimersFromFolderTask::prepare() {
    const QStringList directSubfolders = getDirectSubfolders();
    const QList<GObject *> subobjects = getSubobjects();

    foreach (const QString &subfolder, directSubfolders) {
        addSubTask(new ImportPrimersFromFolderTask(Folder(folder.getDocument(), subfolder)));
    }

    foreach (GObject *subobject, subobjects) {
        addSubTask(new ImportPrimerFromObjectTask(subobject));
    }
}

QString ImportPrimersFromFolderTask::generateReport() const {
    QString report;
    foreach (const QPointer<Task> &subtask, getSubtasks()) {
        report += subtask->generateReport() + "<br>";
    }
    return report;
}

QStringList ImportPrimersFromFolderTask::getDirectSubfolders() {
    QStringList directSubfolders;
    Document *document = folder.getDocument();
    DbiConnection connection(document->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, directSubfolders);
    U2ObjectDbi *objectDbi = connection.dbi->getObjectDbi();
    SAFE_POINT_EXT(NULL != objectDbi, setError(L10N::nullPointerError("object DBI")), directSubfolders);

    const QStringList parentFolderPathParts = folder.getFolderPath().split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
    const QStringList folderPaths = objectDbi->getFolders(stateInfo);
    foreach (const QString &folderPath, folderPaths) {
        QStringList folderPathParts = folderPath.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
        if (!folderPathParts.isEmpty()) {
            folderPathParts.removeLast();
            if (parentFolderPathParts == folderPathParts) {
                directSubfolders << folderPath;
            }
        }
    }

    return directSubfolders;
}

QList<GObject *> ImportPrimersFromFolderTask::getSubobjects() {
    QList<GObject *> subobjects;
    Document *document = folder.getDocument();
    DbiConnection connection(document->getDbiRef(), stateInfo);
    CHECK_OP(stateInfo, subobjects);
    U2ObjectDbi *objectDbi = connection.dbi->getObjectDbi();
    SAFE_POINT_EXT(NULL != objectDbi, setError(L10N::nullPointerError("object DBI")), subobjects);

    const QString folderPath = folder.getFolderPath();
    const QList<U2DataId> objectsIds = objectDbi->getObjects(folderPath, 0, U2DbiOptions::U2_DBI_NO_LIMIT, stateInfo);
    SAFE_POINT_OP(stateInfo, subobjects);
    foreach (const U2DataId &objectId, objectsIds) {
        if (U2DbiUtils::toType(objectId) == U2Type::Sequence) {
            subobjects << document->getObjectById(objectId);
        }
    }

    return subobjects;
}

}   // namespace U2
