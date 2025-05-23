/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "CreateSubalignmentDialogController.h"

#include <QDir>
#include <QMessageBox>
#include <QPalette>

#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2LongLongValidator.h>

namespace U2 {

#define ROW_ID_PROPERTY "row-id"

CreateSubalignmentDialogController::CreateSubalignmentDialogController(MsaObject* obj, const QList<qint64>& preSelectedRowIdList, const U2Region& preSelectedColumnsRegion, QWidget* p)
    : QDialog(p), msaObject(obj), selectedRowIds(preSelectedRowIdList), selectedColumnRegion(preSelectedColumnsRegion), saveController(nullptr) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929690");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Save"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    startLineEdit->setValidator(new U2LongLongValidator(1, msaObject->getLength(), startLineEdit));
    endLineEdit->setValidator(new U2LongLongValidator(1, msaObject->getLength(), endLineEdit));

    connect(allButton, SIGNAL(clicked()), SLOT(sl_allButtonClicked()));
    connect(noneButton, SIGNAL(clicked()), SLOT(sl_noneButtonClicked()));
    connect(invertButton, SIGNAL(clicked()), SLOT(sl_invertButtonClicked()));

    connect(startLineEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_regionChanged()));
    connect(endLineEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_regionChanged()));

    int rowCount = (int)msaObject->getRowCount();
    int msaLength = (int)msaObject->getLength();

    sequencesTableWidget->clearContents();
    sequencesTableWidget->setRowCount(rowCount);
    sequencesTableWidget->setColumnCount(1);
    sequencesTableWidget->verticalHeader()->setHidden(true);
    sequencesTableWidget->horizontalHeader()->setHidden(true);
    sequencesTableWidget->setShowGrid(false);
    sequencesTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    initSaveController();

    if (selectedColumnRegion.isEmpty()) {
        selectedColumnRegion = U2Region(0, msaLength);
    }
    startLineEdit->setText(QString::number(selectedColumnRegion.startPos + 1));  // Visual range starts with 1, not 0.
    endLineEdit->setText(QString::number(selectedColumnRegion.endPos()));

    const Msa msa = msaObject->getAlignment();
    for (int i = 0; i < rowCount; i++) {
        const MsaRow& row = msa->getRow(i);
        auto checkBox = new QCheckBox(row->getName(), this);
        checkBox->setProperty(ROW_ID_PROPERTY, row->getRowId());
        checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        checkBox->setChecked(selectedRowIds.contains(row->getRowId()));
        sequencesTableWidget->setCellWidget(i, 0, checkBox);
        sequencesTableWidget->setRowHeight(i, 15);
    }
}

QString CreateSubalignmentDialogController::getSavePath() const {
    SAFE_POINT(saveController != nullptr, "saveController is nullptr!", "");
    return saveController->getSaveFileName();
}

DocumentFormatId CreateSubalignmentDialogController::getFormatId() const {
    SAFE_POINT(saveController != nullptr, "saveController is nullptr!", DocumentFormatId());
    return saveController->getFormatIdToSave();
}

const U2Region& CreateSubalignmentDialogController::getSelectedColumnsRegion() const {
    return selectedColumnRegion;
}

void CreateSubalignmentDialogController::sl_allButtonClicked() {
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        auto cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(true);
    }
}

void CreateSubalignmentDialogController::sl_invertButtonClicked() {
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        auto cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(!cb->isChecked());
    }
}

void CreateSubalignmentDialogController::sl_noneButtonClicked() {
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        auto cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        cb->setChecked(false);
    }
}

void CreateSubalignmentDialogController::sl_regionChanged() {
    int start = startLineEdit->text().toInt();
    int end = endLineEdit->text().toInt();

    QPalette happyP = filepathEdit->palette();
    startLineEdit->setPalette(happyP);
    endLineEdit->setPalette(happyP);

    if (start <= 0) {
        QPalette p = startLineEdit->palette();
        p.setColor(QPalette::Base, QColor(255, 200, 200));
        startLineEdit->setPalette(p);
    }
    if (end <= start || end > msaObject->getLength()) {
        QPalette p = endLineEdit->palette();
        p.setColor(QPalette::Base, QColor(255, 200, 200));
        endLineEdit->setPalette(p);
    }
}

void CreateSubalignmentDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFileName = GUrlUtils::getNewLocalUrlByFormat(msaObject->getDocument()->getURLString(), msaObject->getGObjectName(), BaseDocumentFormats::CLUSTAL_ALN, "_subalign");
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = browseButton;
    config.fileNameEdit = filepathEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

void CreateSubalignmentDialogController::accept() {
    QFileInfo fi(saveController->getSaveFileName());
    QDir dirToSave(fi.dir());
    if (!dirToSave.exists()) {
        QMessageBox::critical(this, this->windowTitle(), tr("Export folder does not exist"));
        return;
    }
    if (!FileAndDirectoryUtils::isDirectoryWritable(dirToSave.absolutePath())) {
        QMessageBox::critical(this, this->windowTitle(), tr("No write permission for the folder: '%1'").arg(dirToSave.absolutePath()));
        return;
    }
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), tr("No path specified"));
        return;
    }
    if (fi.baseName().isEmpty() || fi.isDir()) {
        QMessageBox::critical(this, this->windowTitle(), tr("Export file name is empty"));
        return;
    }
    if (fi.exists() && !fi.permissions().testFlag(QFile::WriteUser)) {
        QMessageBox::critical(this, this->windowTitle(), tr("No write permission for the file '%1'").arg(fi.fileName()));
        return;
    }

    // '-1' because in memory positions start from 0 not 1
    int start = startLineEdit->text().toInt() - 1;
    int end = endLineEdit->text().toInt() - 1;
    int seqLen = msaObject->getLength();

    if (start > end) {
        QMessageBox::critical(this, windowTitle(), tr("Illegal column range!"));
        return;
    }

    U2Region newSelectedColumnRegion(start, end - start + 1);
    U2Region wholeSequenceRegion(0, seqLen);
    if (!wholeSequenceRegion.contains(newSelectedColumnRegion)) {
        QMessageBox::critical(this, this->windowTitle(), tr("Illegal column range!"));
        return;
    }

    updateSelectedRowIds();

    if (selectedRowIds.empty()) {
        QMessageBox::critical(this, this->windowTitle(), tr("No selected sequence found"));
        return;
    }

    selectedColumnRegion = newSelectedColumnRegion;

    this->close();
    QDialog::accept();
}

bool CreateSubalignmentDialogController::getAddToProjFlag() const {
    return addToProjBox->isChecked();
}

void CreateSubalignmentDialogController::updateSelectedRowIds() {
    selectedRowIds.clear();
    for (int i = 0; i < sequencesTableWidget->rowCount(); i++) {
        auto cb = qobject_cast<QCheckBox*>(sequencesTableWidget->cellWidget(i, 0));
        if (cb->isChecked()) {
            qint64 rowId = (qint64)cb->property(ROW_ID_PROPERTY).toLongLong();
            selectedRowIds << rowId;
        }
    }
}

const QList<qint64>& CreateSubalignmentDialogController::getSelectedRowIds() const {
    return selectedRowIds;
}

CreateSubalignmentAndOpenViewTask::CreateSubalignmentAndOpenViewTask(MsaObject* maObj, const CreateSubalignmentSettings& settings)
    : Task(tr("Create sub-alignment and open view: %1").arg(maObj->getDocument()->getName()), TaskFlags_NR_FOSCOE) {
    csTask = new CreateSubalignmentTask(maObj, settings);
    addSubTask(csTask);
    setMaxParallelSubtasks(1);
}

QList<Task*> CreateSubalignmentAndOpenViewTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if ((subTask == csTask) && csTask->getSettings().addToProject) {
        Document* doc = csTask->takeDocument();
        assert(doc != nullptr);
        res.append(new AddDocumentAndOpenViewTask(doc));
    }

    return res;
}

};  // namespace U2
