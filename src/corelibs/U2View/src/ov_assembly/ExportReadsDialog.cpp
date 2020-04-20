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
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportReadsDialog.h"

namespace U2 {

ExportReadsDialog::ExportReadsDialog(QWidget * p, const QList<DocumentFormatId> & formats) : QDialog(p) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748903");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    SaveDocumentControllerConfig conf;
    conf.defaultDomain = "ExportReadsDialog";
    conf.fileNameEdit = filepathLineEdit;
    conf.fileDialogButton = filepathToolButton;
    conf.formatCombo = documentFormatComboBox;
    conf.parentWidget = this;
    conf.saveTitle = tr("Select file to save");

    const QString ugeneDataDir = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath();
    LastUsedDirHelper lod("ExportReadsDialog", ugeneDataDir);
    const QString filePath = lod.dir + "/exported_reads";
    conf.defaultFileName = GUrlUtils::rollFileName(filePath, "_", QSet<QString>());

    saveController = new SaveDocumentController(conf, formats, this);
    setMaximumHeight(layout()->minimumSize().height());
}

void ExportReadsDialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, tr("Error!"), tr("Select destination file"));
        filepathLineEdit->setFocus(Qt::OtherFocusReason);
        return;
    }
    QDialog::accept();
}

ExportReadsDialogModel ExportReadsDialog::getModel() const {
    ExportReadsDialogModel ret;
    ret.filepath = saveController->getSaveFileName();
    ret.format = saveController->getFormatIdToSave();
    ret.addToProject = addToProjectCheckBox->isChecked();
    return ret;
}

} // U2
