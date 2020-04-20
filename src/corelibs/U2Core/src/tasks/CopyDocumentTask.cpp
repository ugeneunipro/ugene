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
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "CopyDocumentTask.h"

namespace U2 {

CopyDocumentTask::CopyDocumentTask(Document *_srcDoc, const DocumentFormatId &_formatId,
    const QString &_dstUrl, bool _addToProject)
    : Task("Copy document", TaskFlag_NoRun), srcDoc(_srcDoc), dstDoc(NULL), formatId(_formatId),
    dstUrl(_dstUrl), addToProject(_addToProject), cloneTask(NULL), saveTask(NULL) {

}

CopyDocumentTask::~CopyDocumentTask() {
    if (addToProject) { // it means not "SaveDoc_DestroyAfter", so it is needed to be deleted
        if (hasError() || isCanceled()) {
            delete dstDoc;
        }
    }
}

void CopyDocumentTask::prepare() {
    QSet<QString> excludeFileNames = DocumentUtils::getNewDocFileNameExcludesHint();
    if (!GUrlUtils::renameFileWithNameRoll(dstUrl, stateInfo, excludeFileNames, &coreLog)) {
        return;
    }
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(dstUrl));
    CHECK_EXT(iof != NULL, stateInfo.setError(QString("Can not create IO factory for %1").arg(dstUrl)), );
    DocumentFormatRegistry *dfr = AppContext::getDocumentFormatRegistry();
    DocumentFormat *df = dfr->getFormatById(formatId);
    CHECK_EXT(df != NULL, stateInfo.setError(QString("Unknown document format IO factory: %1").arg(formatId)), );

    QVariantMap hints = srcDoc->getGHintsMap();
    U2DbiRef dstDbiRef(DEFAULT_DBI_ID, U2DbiId(dstUrl));
    hints[DocumentFormat::DBI_REF_HINT] = qVariantFromValue(dstDbiRef);
    dstDoc = df->createNewLoadedDocument(iof, dstUrl, stateInfo, hints);
    CHECK_OP(stateInfo, );

    cloneTask = new CloneObjectsTask(srcDoc, dstDoc);
    addSubTask(cloneTask);
}

QList<Task*> CopyDocumentTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    if (hasError() || isCanceled()) {
        return result;
    }

    if (cloneTask == subTask) {
        QList<GObject*> objs = cloneTask->takeResult();
        foreach(GObject *dstObj, objs) {
            dstObj->moveToThread(QCoreApplication::instance()->thread());
            dstDoc->addObject(dstObj);
        }
        foreach(GObject *dstObj, objs) {
            GObjectUtils::updateRelationsURL(dstObj, srcDoc->getURL(), dstUrl);
        }
        if (addToProject) {
            Project *p = AppContext::getProject();
            if (NULL != p) {
                dstDoc->setModified(true);
                p->addDocument(dstDoc);
            }
        }

        saveTask = new SaveDocumentTask(dstDoc, dstDoc->getIOAdapterFactory(), dstUrl, SaveDoc_Append);
        if (!addToProject) {
            saveTask->addFlag(SaveDoc_DestroyAfter);
        }
        result << saveTask;
    }

    return result;
}

void CopyDocumentTask::sl_onCopySaved() {

}

CloneObjectsTask::CloneObjectsTask(Document *_srcDoc, Document *_dstDoc)
    : Task("Clone objects", TaskFlag_None), srcDoc(_srcDoc), dstDoc(_dstDoc) {
    CHECK_EXT(NULL != srcDoc, stateInfo.setError("NULL source document"), );
    CHECK_EXT(NULL != dstDoc, stateInfo.setError("NULL destination document"), );
}

void CloneObjectsTask::run() {
    DocumentFormat *df = dstDoc->getDocumentFormat();
    CHECK_EXT(NULL != df, stateInfo.setError("NULL document format"), );

    QList<GObject*> objs = srcDoc->getObjects();
    foreach(GObject *srcObj, objs) {
        if (df->isObjectOpSupported(dstDoc, DocumentFormat::DocObjectOp_Add, srcObj->getGObjectType())) {
            GObject *dstObj = srcObj->clone(dstDoc->getDbiRef(), stateInfo);
            CHECK_OP(stateInfo, );
            if (dstObj->getGObjectType() == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
                QString name = QFileInfo(dstDoc->getURLString()).baseName();
                dstObj->setGObjectName(name);
                dstObj->setModified(false);
            }
            cloned << dstObj;
        }
    }
}

QList<GObject*> CloneObjectsTask::takeResult() {
    QList<GObject*> result = cloned;
    cloned.clear();

    return result;
}

} // U2
