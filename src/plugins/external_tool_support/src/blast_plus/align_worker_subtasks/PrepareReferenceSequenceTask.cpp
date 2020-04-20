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
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CopyFileTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <QFileInfo>

#include "PrepareReferenceSequenceTask.h"
#include "RemoveGapsFromSequenceTask.h"

namespace U2 {

PrepareReferenceSequenceTask::PrepareReferenceSequenceTask(const QString &referenceUrl, const U2DbiRef &dstDbiRef)
    : DocumentProviderTask(tr("Prepare reference sequence"), TaskFlags_NR_FOSE_COSC),
      referenceUrl(referenceUrl),
      dstDbiRef(dstDbiRef),
      copyTask(NULL),
      loadTask(NULL),
      removeGapsTask(NULL)
{
    SAFE_POINT_EXT(!referenceUrl.isEmpty(), setError("Reference URL is empty"), );
    SAFE_POINT_EXT(dstDbiRef.isValid(), setError("Destination DBI reference is not valid"), );
}

const U2EntityRef &PrepareReferenceSequenceTask::getReferenceEntityRef() const {
    return referenceEntityRef;
}

void PrepareReferenceSequenceTask::prepare() {
    QString tmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    copyTask = new CopyFileTask(referenceUrl, GUrlUtils::prepareTmpFileLocation(tmpDir, GUrlUtils::fixFileName(QFileInfo(referenceUrl).completeBaseName()), "tmp", stateInfo));
    addSubTask(copyTask);
}

QList<Task *> PrepareReferenceSequenceTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubTasks;
    CHECK_OP(stateInfo, newSubTasks);

    if (copyTask == subTask) {
        preparedReferenceUrl = copyTask->getTargetFilePath();
        QVariantMap hints;
        hints[DocumentFormat::DBI_REF_HINT] = QVariant::fromValue<U2DbiRef>(dstDbiRef);
        loadTask = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, preparedReferenceUrl, hints);
        CHECK_OP(stateInfo, newSubTasks);

        newSubTasks << loadTask;
    } else if (loadTask == subTask) {
        Document * const document = loadTask->getDocument(false);
        SAFE_POINT(NULL != document, "Document is NULL", newSubTasks);

        document->setDocumentOwnsDbiResources(false);

        QList<GObject *> objects = document->findGObjectByType(GObjectTypes::SEQUENCE);
        CHECK_EXT(!objects.isEmpty(), setError(tr("No reference sequence in the file: ") + referenceUrl), newSubTasks);
        CHECK_EXT(1 == objects.size(), setError(tr("More than one sequence in the reference file: ") + referenceUrl), newSubTasks);

        U2SequenceObject *referenceObject = qobject_cast<U2SequenceObject *>(objects.first());
        SAFE_POINT_EXT(NULL != referenceObject, setError(tr("Unable to cast gobject to sequence object")), newSubTasks);
        CHECK_EXT(referenceObject->getAlphabet()->isDNA(), setError(tr("The input reference sequence '%1' contains characters that don't belong to DNA alphabet.").arg(referenceObject->getSequenceName())), newSubTasks);

        referenceEntityRef = referenceObject->getEntityRef();

        newSubTasks << new RemoveGapsFromSequenceTask(referenceObject);
    } else if (qobject_cast<RemoveGapsFromSequenceTask*>(subTask) != NULL) {
        Document* doc = loadTask->getDocument(false);
        SAFE_POINT(NULL != doc, "Document is NULL", newSubTasks);

        DocumentFormat *fastaFormat = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);
        IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(doc->getURL()));

        preparedReferenceUrl = GUrlUtils::rollFileName(doc->getURL().getURLString(), "_");        // we roll the URL here because there was a strange problem when UGENE couldn't overwrite the file (UTI-242)
        Document *fastaDoc = doc->getSimpleCopy(fastaFormat, ioAdapterFactory, preparedReferenceUrl);
        SaveDocumentTask* saveTask = new SaveDocumentTask(fastaDoc, SaveDoc_Overwrite | SaveDoc_DestroyButDontUnload);
        newSubTasks << saveTask;
    }

    return newSubTasks;
}

}   // namespace U2
