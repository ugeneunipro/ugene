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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportBlastResultDialog.h"

namespace U2 {

ExportBlastResultDialog::ExportBlastResultDialog(QWidget* p, const QString& defaultUrl)
    : QDialog(p),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748946");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    addToProjectFlag = true;

    nameIdBox->addItem("accession");
    nameIdBox->addItem("def");
    nameIdBox->addItem("id");

    nameIdBox->setCurrentIndex(0);

    initSaveController(defaultUrl);
}

void ExportBlastResultDialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File name is empty!"));
        return;
    }

    url = saveController->getSaveFileName();
    format = saveController->getFormatIdToSave();
    addToProjectFlag = addToProjectBox->isChecked();
    qualiferId = nameIdBox->currentText();
    addRefFlag = addRefBox->isChecked();
    QDialog::accept();
}

void ExportBlastResultDialog::initSaveController(const QString &defaultUrl) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = defaultUrl;
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileNameEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

void ExportBlastResultDialog::setOkButtonText(const QString& text) const {
    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(text);
}

void ExportBlastResultDialog::setFileLabelText(const QString& text) const {
    fileLabel->setText(text);
}

}//namespace
