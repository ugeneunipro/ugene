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
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ExportPrimersToDatabaseTask.h"
#include "ExportPrimersToLocalFileTask.h"

namespace U2 {

ExportPrimersToLocalFileTask::ExportPrimersToLocalFileTask(const QList<Primer> &primers, const DocumentFormatId &formatId, const QString &localFilePath) :
    Task(tr("Export primers"), TaskFlags_NR_FOSE_COSC | TaskFlag_OnlyNotificationReport),
    primers(primers),
    format(AppContext::getDocumentFormatRegistry()->getFormatById(formatId)),
    url(localFilePath)
{
    SAFE_POINT_EXT(!primers.isEmpty(), setError(L10N::badArgument("primers list")), );
    SAFE_POINT_EXT(NULL != format, setError(L10N::badArgument("document format")), );
    SAFE_POINT_EXT(!localFilePath.isEmpty(), setError(L10N::badArgument("file path")), );
}

void ExportPrimersToLocalFileTask::prepare() {
    addSubTask(new ExportPrimersToDatabaseTask(primers, AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo), U2ObjectDbi::ROOT_FOLDER));
}

QList<Task *> ExportPrimersToLocalFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK_OP(stateInfo, result);

    ExportPrimersToDatabaseTask *convertTask = qobject_cast<ExportPrimersToDatabaseTask *>(subTask);
    CHECK(NULL != convertTask, result);

    Document *document = prepareDocument();
    CHECK_OP(stateInfo, result);

    addObjects(document, convertTask);
    CHECK_OP(stateInfo, result);

    result << new SaveDocumentTask(document, SaveDocFlags(SaveDoc_Overwrite | SaveDoc_DestroyAfter));
    return result;
}

Document *ExportPrimersToLocalFileTask::prepareDocument() {
    IOAdapterFactory *ioAdapterFactory = IOAdapterUtils::get(IOAdapterUtils::url2io(url));
    SAFE_POINT_EXT(NULL != ioAdapterFactory, setError(L10N::nullPointerError("I/O adapter factory")), NULL);
    return format->createNewLoadedDocument(ioAdapterFactory, url, stateInfo);
}

void ExportPrimersToLocalFileTask::addObjects(Document *document, ExportPrimersToDatabaseTask *convertTask) {
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    DbiConnection connection(dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    SAFE_POINT_EXT(NULL != connection.dbi, setError(L10N::nullPointerError("dbi")), );
    U2ObjectDbi *objectDbi = connection.dbi->getObjectDbi();
    SAFE_POINT_EXT(NULL != objectDbi, setError(L10N::nullPointerError("object dbi")), );

    const QMap<U2DataId, U2DataId> objectIds = convertTask->getImportedObjectIds();
    QMap<U2DataId, U2DataId>::ConstIterator iterator = objectIds.constBegin();
    while (iterator != objectIds.constEnd()) {
        U2Object sequence;
        objectDbi->getObject(sequence, iterator.key(), stateInfo);
        CHECK_OP(stateInfo, );
        U2SequenceObject *sequenceObject = new U2SequenceObject(sequence.visualName, U2EntityRef(dbiRef, iterator.key()));
        document->addObject(sequenceObject);

        if (format->getSupportedObjectTypes().contains(GObjectTypes::ANNOTATION_TABLE)) {
            U2Object annotationTable;
            objectDbi->getObject(annotationTable, iterator.value(), stateInfo);
            CHECK_OP(stateInfo, );
            AnnotationTableObject *annotationTableObject = new AnnotationTableObject(annotationTable.visualName, U2EntityRef(dbiRef, iterator.value()));
            annotationTableObject->addObjectRelation(sequenceObject, ObjectRole_Sequence);
            document->addObject(annotationTableObject);
        }

        iterator++;
    }
}

}   // namespace U2
