/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
#include "RealignSequencesInAlignmentTask.h"

#include <QDirIterator>
#include <QUuid>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ov_msa/ExportSequencesTask.h"
#include "ov_msa/align_to_alignment/AlignSequencesToAlignmentTask.h"

namespace U2 {

RealignSequencesInAlignmentTask::RealignSequencesInAlignmentTask(MultipleSequenceAlignmentObject* msaObjectToClone,
                                                                 const QSet<qint64>& _rowsToAlignIds,
                                                                 const QString& _algorithmId)
    : Task(tr("Realign sequences in this alignment"), TaskFlags_NR_FOSE_COSC),
      originalMsaObject(msaObjectToClone),
      msaObject(nullptr),
      rowsToAlignIds(_rowsToAlignIds), algorithmId(_algorithmId) {
    locker = new StateLocker(originalMsaObject);
    msaObject = msaObjectToClone->clone(msaObjectToClone->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, );

    for (int index = 0; index < msaObject->getRowCount(); index++) {
        const QString name = QString::number(index);
        msaObject->renameRow(index, name);
        originalRowOrder.append(name);
    }

    QSet<qint64> clonedObjectRowsToAlignIds;
    for (qint64 idToRemove : qAsConst(rowsToAlignIds)) {
        int rowPos = msaObjectToClone->getRowPosById(idToRemove);
        qint64 id = msaObject->getRow(rowPos)->getRowId();
        clonedObjectRowsToAlignIds.insert(id);
    }

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
    foreach (const QString& file, tmpDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
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
    CHECK(subTask == extractSequences, res);

    QList<int> rowPosToRemove;
    for (qint64 idToRemove : qAsConst(rowsToAlignIds)) {
        rowPosToRemove.append(originalMsaObject->getRowPosById(idToRemove));
    }
    std::sort(rowPosToRemove.begin(), rowPosToRemove.end());
    std::reverse(rowPosToRemove.begin(), rowPosToRemove.end());
    for (int rowPos : qAsConst(rowPosToRemove)) {
        msaObject->removeRow(rowPos);
    }
    QStringList sequenceFilesToAlign;
    QDirIterator it(extractedSequencesDirUrl, {"*.fa"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        sequenceFilesToAlign.append(it.next());
    }
    res.append(new LoadSequencesAndAlignToAlignmentTask(msaObject, algorithmId, sequenceFilesToAlign));

    return res;
}

}  // namespace U2