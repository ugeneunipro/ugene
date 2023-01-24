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

#include "ExportMSA2SequencesDialog.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ExportTasks.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportUtils.h"

namespace U2 {

ExportMSA2SequencesDialog::ExportMSA2SequencesDialog(const QString& defaultDir, const QString& defaultFileName, QWidget* p)
    : QDialog(p),
      defaultDir(defaultDir),
      defaultFileName(defaultFileName),
      saveController(nullptr) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929311");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    trimGapsFlag = false;
    addToProjectFlag = true;

    initSaveController();
}

void ExportMSA2SequencesDialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File name is empty!"));
        return;
    }

    url = saveController->getSaveFileName();
    format = saveController->getFormatIdToSave();
    trimGapsFlag = trimGapsRB->isChecked();
    addToProjectFlag = addToProjectBox->isChecked();

    QDialog::accept();
}

void ExportMSA2SequencesDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::FASTA;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileNameEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;
    QString defaultFileExtension = AppContext::getDocumentFormatRegistry()->getFormatById(config.defaultFormatId)->getSupportedDocumentFileExtensions().first();
    config.defaultFileName = defaultDir + "/" + defaultFileName + "." + defaultFileExtension;

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::SEQUENCE;
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

void ExportMSA2SequencesDialog::showDialogAndStartExportTask(MultipleSequenceAlignmentObject* msaObject) {
    SAFE_POINT(msaObject != nullptr, "ExportMSA2SequencesDialog: msaObject is null!", );

    QPointer<MultipleSequenceAlignmentObject> msaObjectPtr(msaObject);

    LastUsedDirHelper lod;
    QString defaultDir = lod.dir.isEmpty() ? GUrl(msaObject->getDocument()->getURLString()).dirPath() : lod.dir;
    QString defaultFileName = GUrlUtils::fixFileName(msaObject->getGObjectName());
    QObjectScopedPointer<ExportMSA2SequencesDialog> d = new ExportMSA2SequencesDialog(defaultDir, defaultFileName, AppContext::getMainWindow()->getQMainWindow());
    int rc = d->exec();
    CHECK(!d.isNull() && rc != QDialog::Rejected && !msaObjectPtr.isNull(), );
    lod.url = d->url;

    const MultipleSequenceAlignment& msa = msaObject->getMultipleAlignment();
    auto t = ExportUtils::wrapExportTask(new ExportMSA2SequencesTask(msa, d->url, d->trimGapsFlag, d->format), d->addToProjectFlag);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

}  // namespace U2
