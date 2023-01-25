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

#include "ExportSequences2MSADialog.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/L10n.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

namespace U2 {

ExportSequences2MSADialog::ExportSequences2MSADialog(QWidget* p, const QString& defaultUrl)
    : QDialog(p), Ui_ExportSequences2MSADialog(),
      saveController(nullptr) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65929308");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    okButton = buttonBox->button(QDialogButtonBox::Ok);

    addToProjectFlag = true;
    useGenbankHeader = false;

    initSaveController(defaultUrl);
}

void ExportSequences2MSADialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File name is empty!"));
        return;
    }

    url = saveController->getSaveFileName();
    format = saveController->getFormatIdToSave();
    addToProjectFlag = addToProjectBox->isChecked();
    useGenbankHeader = genbankBox->isChecked();

    QDialog::accept();
}

void ExportSequences2MSADialog::initSaveController(const QString& defaultUrl) {
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

void ExportSequences2MSADialog::setOkButtonText(const QString& text) const {
    okButton->setText(text);
}

void ExportSequences2MSADialog::setFileLabelText(const QString& text) const {
    fileLabel->setText(text);
}

}  // namespace U2
