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
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportChromatogramDialog.h"
#include "ExportUtils.h"

#define SETTINGS_ROOT QString("dna_export/")

namespace U2 {

ExportChromatogramDialog::ExportChromatogramDialog(QWidget* p, const GUrl& fileUrl) :
    QDialog(p),
    saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748814");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    addToProjectFlag = true;

    initSaveController(fileUrl);
}

void ExportChromatogramDialog::initSaveController(const GUrl &fileUrl) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = GUrlUtils::getNewLocalUrlByExtention(fileUrl, "chromatogram", ".scf", "_copy");
    config.defaultFormatId = BaseDocumentFormats::SCF;
    config.formatCombo = formatCombo;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select a file");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::SCF;

    saveController = new SaveDocumentController(config, formats, this);
}

void ExportChromatogramDialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File name is empty!"));
        return;
    }

    url = saveController->getSaveFileName();
    addToProjectFlag = addToProjectBox->isChecked();
    reversed = reverseBox->isChecked();
    complemented = complementBox->isChecked();
    format = saveController->getFormatIdToSave();

    QDialog::accept();
}


}//namespace
