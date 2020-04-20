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
#include <QDirIterator>
#include <QUuid>

#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/StateLockableDataModel.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2Mod.h>
#include <U2Core/UserApplicationsSettings.h>

#include "RealignSequencesInAlignmentTask.h"
#include "../AlignSequencesToAlignment/AlignSequencesToAlignmentTask.h"
#include "../ExportSequencesTask.h"

namespace U2 {

RealignSequencesInAlignmentTask::RealignSequencesInAlignmentTask(MultipleSequenceAlignmentObject* msaObjectToClone, const QSet<qint64>& _rowsToAlignIds, bool forceUseUgeneNativeAligner)
    : Task(tr("Realign sequences in this alignment"), TaskFlags_NR_FOSE_COSC),
    originalMsaObject(msaObjectToClone),
    msaObject(nullptr),
    rowsToAlignIds(_rowsToAlignIds)
{
    locker = new StateLocker(originalMsaObject);
    msaObject = msaObjectToClone->clone(msaObjectToClone->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    for (int index = 0; index < msaObject->getNumRows(); index++) {
        const QString name = QString::number(index);
        msaObject->renameRow(index, name);
        originalRowOrder.append(name);
    }

    CreateSubalignmentSettings settings;
    settings.window = U2Region(0, msaObject->getLength());

    QList<qint64> rowsToKeepIds = msaObject->getMultipleAlignment()->getRowsIds();
    QSet<qint64> clonedObjectRowsToAlignIds;
    foreach(const qint64 idToRemove, rowsToAlignIds) {
        int rowPos = msaObjectToClone->getRowPosById(idToRemove);
        qint64 id = msaObject->getRow(rowPos)->getRowId();
        rowsToKeepIds.removeAll(id);
        clonedObjectRowsToAlignIds.insert(id);
    }
    settings.rowIds = rowsToKeepIds;

    QString url;
    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }

    extractedSequencesDirUrl = path + "/tmp" + GUrlUtils::fixFileName(QUuid::createUuid().toString());
    dir = QDir(extractedSequencesDirUrl);
    dir.mkpath(extractedSequencesDirUrl);

    extractSequences = new ExportSequencesTask(msaObject->getMsa(), clonedObjectRowsToAlignIds, false, false, extractedSequencesDirUrl, BaseDocumentFormats::FASTA, "fa");
    addSubTask(extractSequences);
}

RealignSequencesInAlignmentTask::~RealignSequencesInAlignmentTask() {
    delete msaObject;
}

U2::Task::ReportResult RealignSequencesInAlignmentTask::report() {
    msaObject->sortRowsByList(originalRowOrder);
    delete locker;
    locker = nullptr;
    U2UseCommonUserModStep modStep(originalMsaObject->getEntityRef(), stateInfo);
    CHECK_OP(stateInfo, Task::ReportResult_Finished);
    originalMsaObject->updateGapModel(msaObject->getMsa()->getMsaRows());
    QDir tmpDir(extractedSequencesDirUrl);
    foreach(const QString & file, tmpDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        tmpDir.remove(file);
    }
    tmpDir.rmdir(tmpDir.absolutePath());

    DbiConnection con(msaObject->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, Task::ReportResult_Finished);
    CHECK(con.dbi->getFeatures().contains(U2DbiFeature_RemoveObjects), Task::ReportResult_Finished);
    con.dbi->getObjectDbi()->removeObject(msaObject->getEntityRef().entityId, true, stateInfo);

    return Task::ReportResult_Finished;
}

QList<Task*> RealignSequencesInAlignmentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if (subTask == extractSequences) {
        QList<int> rowPosToRemove;
        foreach(qint64 idToRemove, rowsToAlignIds) {
            rowPosToRemove.append(originalMsaObject->getRowPosById(idToRemove));
        }
        qSort(rowPosToRemove);
        std::reverse(rowPosToRemove.begin(), rowPosToRemove.end());
        foreach(int rowPos, rowPosToRemove) {
            msaObject->removeRow(rowPos);
        }
        QStringList sequenceFilesToAlign;
        QDirIterator it(extractedSequencesDirUrl, QStringList() << "*.fa", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            sequenceFilesToAlign.append(it.next());
        }

        LoadSequencesAndAlignToAlignmentTask* task = new LoadSequencesAndAlignToAlignmentTask(msaObject, sequenceFilesToAlign);
        res.append(task);
    }

    return res;
}

}