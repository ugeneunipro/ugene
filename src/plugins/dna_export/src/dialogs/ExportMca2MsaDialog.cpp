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

#include <QPushButton>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "dialogs/ExportMca2MsaDialog.h"

namespace U2 {

ExportMca2MsaDialog::ExportMca2MsaDialog(const QString &defaultFilePath, QWidget *parent)
    : QDialog(parent),
      saveController(NULL)
{
    setupUi(this);
    GCOUNTER(cvar, tvar, "'Export Alignment without Chromatograms' dialog opening");

    new HelpButton(this, buttonBox, "24748722");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController(defaultFilePath);
}

QString ExportMca2MsaDialog::getSavePath() const {
    return saveController->getSaveFileName();
}

QString ExportMca2MsaDialog::getFormatId() const {
    return saveController->getFormatIdToSave();
}

bool ExportMca2MsaDialog::getAddToProjectOption() const {
    return chbAddToProject->isChecked();
}

bool ExportMca2MsaDialog::getIncludeReferenceOption() const {
    return chbIncludeReference->isChecked();
}

void ExportMca2MsaDialog::initSaveController(const QString &defaultFilePath) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = defaultFilePath;
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = tbFilePath;
    config.fileNameEdit = leFilePath;
    config.formatCombo = cbFormat;
    config.parentWidget = this;
    config.saveTitle = tr("Export Alignment");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

}   // namespace U2
