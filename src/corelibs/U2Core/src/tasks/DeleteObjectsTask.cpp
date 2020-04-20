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

#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "DeleteObjectsTask.h"

namespace U2 {

namespace {

class ProgressUpdater {
    U2OpStatus &os;
    const int size;
    const int percentStep;
    int ticksCount;
    int currentStep;
public:
    ProgressUpdater(U2OpStatus &os, int size)
        : os(os), size(size), percentStep(size / 100), ticksCount(0), currentStep(0)
    {
        os.setProgress(0);
    }
    inline void tick() {
        SAFE_POINT_EXT(size > 0, os.setError("Invalid task progress"), );
        ticksCount++;
        currentStep++;
        if (currentStep >= percentStep) {
            currentStep = 0;
            os.setProgress(ticksCount * 100 / size);
        }
    }
};

}

//////////////////////////////////////////////////////////////////////////
// DeleteObjectsTask
//////////////////////////////////////////////////////////////////////////

DeleteObjectsTask::DeleteObjectsTask(const QList<GObject *> &objs)
    : Task(tr("Delete objects"), TaskFlag_None)
{
    tpm = Progress_Manual;

    foreach (GObject *obj, objs) {
        CHECK_EXT(NULL != obj, stateInfo.setError("Invalid object detected!"), );
        const U2EntityRef &objRef = obj->getEntityRef();
        if (!dbiRef2Objs.contains(objRef.dbiRef)) {
            dbiRef2Objs.insert(objRef.dbiRef, QList<U2DataId>());
        }
        dbiRef2Objs[objRef.dbiRef].append(objRef.entityId);
    }
}

void DeleteObjectsTask::run() {
    foreach (const U2DbiRef &dbiRef, dbiRef2Objs.keys()) {
        DbiConnection con(dbiRef, stateInfo);
        CHECK_OP(stateInfo, );

        if (con.dbi->getFeatures().contains(U2DbiFeature_RemoveObjects)) {
            con.dbi->getObjectDbi()->removeObjects(dbiRef2Objs[dbiRef], stateInfo);
            CHECK_OP(stateInfo, );
        }

        stateInfo.setProgress(100 * dbiRef2Objs[dbiRef].count() / dbiRef2Objs.count());
    }
}

//////////////////////////////////////////////////////////////////////////
// DeleteFoldersTask
//////////////////////////////////////////////////////////////////////////

DeleteFoldersTask::DeleteFoldersTask(const QList<Folder> &folders)
    : Task(tr("Delete folders"), TaskFlag_None)
{
    tpm = Progress_Manual;
    foreach (const Folder &folder, folders) {
        Document *doc = folder.getDocument();
        CHECK_EXT(NULL != doc, stateInfo.setError("Invalid document detected!"), );
        const U2DbiRef dbiRef = doc->getDbiRef();
        CHECK_EXT(dbiRef.isValid(), stateInfo.setError("Invalid DBI reference detected!"), );
        dbi2Path.insert(doc->getDbiRef(), folder.getFolderPath());
    }
}

void DeleteFoldersTask::run() {
    ProgressUpdater progressUpdater(stateInfo, dbi2Path.size());
    foreach (const U2DbiRef &dbiRef, dbi2Path.keys()) {
        DbiConnection con(dbiRef, stateInfo);
        CHECK_OP(stateInfo, );

        foreach (const QString &path, dbi2Path.values(dbiRef)) {
            con.dbi->getObjectDbi()->removeFolder(path, stateInfo);
        }
        progressUpdater.tick();
    }
}

} // namespace U2
