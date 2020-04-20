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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "ConvertAssemblyToSamDialog.h"
#include "ui_AssemblyToSamDialog.h"

namespace U2 {

GUrl ConvertAssemblyToSamDialog::dbFileUrl;

ConvertAssemblyToSamDialog::ConvertAssemblyToSamDialog(QWidget* parent, QString dbPath)
    : QDialog(parent),
      ui(new Ui_AssemblyToSamDialog),
      saveController(NULL)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24749000");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Convert"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();
    connect(ui->setDbPathButton, SIGNAL(clicked()), SLOT(sl_onSetDbPathButtonClicked()));

    if ("" != dbPath) {
        ui->dbPathEdit->setText(dbPath);
        ui->dbPathEdit->setReadOnly(true);
        ui->setDbPathButton->setEnabled(false);
        buildSamUrl(dbPath);
        return;
    }

    if (!dbFileUrl.isEmpty()) {
        ui->dbPathEdit->setText(dbFileUrl.getURLString());
        buildSamUrl(dbFileUrl);
    }
}

void ConvertAssemblyToSamDialog::accept() {
    if (ui->dbPathEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("Data base to SAM converter"),
            tr("Data base file url is not set!") );
    } else if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::information(this, tr("Data base to SAM converter"),
            tr("SAM file url is not set!") );
    } else {
        if (ui->setDbPathButton->isEnabled()) {
            dbFileUrl = getDbFileUrl();
        }
        QDialog::accept();
    }
}

void ConvertAssemblyToSamDialog::reject() {
    if (ui->setDbPathButton->isEnabled() && !ui->dbPathEdit->text().isEmpty()) {
        dbFileUrl = getDbFileUrl();
    }
    QDialog::reject();
}

void ConvertAssemblyToSamDialog::buildSamUrl(const GUrl &dbUrl) {
    GUrl url = GUrlUtils::rollFileName(dbUrl.dirPath() + "/" + dbUrl.baseFileName() + ".sam", DocumentUtils::getNewDocFileNameExcludesHint());
    ui->samPathEdit->setText(url.getURLString());
}

void ConvertAssemblyToSamDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::SAM;
    config.fileDialogButton = ui->setSamPathButton;
    config.fileNameEdit = ui->samPathEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Set a result SAM file name");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::SAM;

    saveController = new SaveDocumentController(config, formats, this);
}

void ConvertAssemblyToSamDialog::sl_onSetDbPathButtonClicked() {
    LastUsedDirHelper lod;
    QString filter;

    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an Assembly Database File"), lod.dir, filter);
    if (lod.url.isEmpty()) {
        return;
    }

    ui->dbPathEdit->setText(lod.url);
    buildSamUrl(lod.url);
}

const GUrl ConvertAssemblyToSamDialog::getDbFileUrl() {
    return ui->dbPathEdit->text();
}

const GUrl ConvertAssemblyToSamDialog::getSamFileUrl() {
    return saveController->getSaveFileName();
}

} // U2
