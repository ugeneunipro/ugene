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

#include <QCoreApplication>

#include <U2Core/DocumentModel.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "CloneObjectTask.h"

namespace U2 {

CloneObjectTask::CloneObjectTask(GObject *srcObj, Document *dstDoc, const QString &dstFolder)
    : Task(tr("Copy object"), TaskFlag_None),
      srcObj(srcObj),
      dstDoc(dstDoc),
      dstDbiRef(NULL != dstDoc ? dstDoc->getDbiRef() : U2DbiRef()),
      dstFolder(dstFolder),
      dstObj(NULL)
{
    CHECK_EXT(NULL != srcObj, setError(tr("Invalid source object")), );
    CHECK_EXT(NULL != dstDoc, setError(tr("Invalid destination document")), );
    setTaskName(getTaskName() + ": " + srcObj->getGObjectName());
    tpm = Progress_Manual;
}

CloneObjectTask::CloneObjectTask(GObject *srcObj, const U2DbiRef &dstDbiRef, const QString &dstFolder) :
    Task(tr("Copy object %1").arg(NULL != srcObj ? srcObj->getGObjectName() : "") , TaskFlag_None),
    srcObj(srcObj),
    dstDbiRef(dstDbiRef),
    dstFolder(dstFolder),
    dstObj(NULL)
{
    CHECK_EXT(dstDbiRef.isValid(), setError(tr("Invalid destination database reference")), );
    tpm = Progress_Manual;
}

CloneObjectTask::~CloneObjectTask() {
    delete dstObj;
}

void CloneObjectTask::run() {
    DbiConnection con(dstDbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    SAFE_POINT_EXT(NULL != con.dbi, setError(tr("Error! No DBI")), );

    QVariantMap hints;
    hints[DocumentFormat::DBI_FOLDER_HINT] = dstFolder;

    CHECK_EXT(!srcObj.isNull(), setError(tr("The object has been removed")), );
    dstObj = srcObj->clone(dstDbiRef, stateInfo, hints);
    CHECK_OP(stateInfo, );
    dstObj->moveToThread(QCoreApplication::instance()->thread());
}

GObject * CloneObjectTask::takeResult() {
    GObject *result = dstObj;
    dstObj = NULL;
    return result;
}

const QString & CloneObjectTask::getFolder() const {
    return dstFolder;
}

GObject *CloneObjectTask::getSourceObject() const {
    return srcObj.data();
}

Document * CloneObjectTask::getDocument() const {
    return dstDoc.data();
}

} // U2
