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

#include "SaveDocumentWithFormatDialogController.h"

#include <QPushButton>

#include "SaveDocumentController.h"
#include "ui_ExportDocumentDialog.h"

namespace U2 {

SaveDocumentWithFormatDialogController::SaveDocumentWithFormatDialogController(const QString& defaultUrl, 
                                                                               const DocumentFormatConstraints& dfc,
                                                                               const QString& windowTitle,
                                                                               QWidget* parent) 
    : QDialog(parent) {
    ui = new Ui_ExportDocumentDialog();
    ui->setupUi(this);
    setMinimumWidth(size().width());
    setWindowTitle(windowTitle);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Save"));
    ui->compressCheck->setVisible(false);
    ui->addToProjCheck->setVisible(false);
    adjustSize();    

    SaveDocumentControllerConfig config;
    config.defaultFileName = defaultUrl;
    config.fileDialogButton = ui->browseButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.formatCombo = ui->formatCombo;
    config.compressCheckbox = ui->compressCheck;
    config.parentWidget = this;
    config.rollOutProjectUrls = true;
    config.rollSuffix = "_copy";
    saveController = new SaveDocumentController(config, dfc, this);
}

QString SaveDocumentWithFormatDialogController::getDocumentURL() const {
    return saveController->getSaveFileName();
}

DocumentFormatId SaveDocumentWithFormatDialogController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

}
