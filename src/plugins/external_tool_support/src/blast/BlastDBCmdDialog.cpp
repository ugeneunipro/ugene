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

#include "BlastDBCmdDialog.h"

#include <QPushButton>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

namespace U2 {

BlastDBCmdDialog::BlastDBCmdDialog(QWidget* parent)
    : QDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930731");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Fetch"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    fetchButton = buttonBox->button(QDialogButtonBox::Ok);
    dbSelector = new BlastDBSelectorWidgetController(this);
    dbSelectorWidget->layout()->addWidget(dbSelector);

    initSaveController();

    connect(dbSelector, SIGNAL(si_dbChanged()), SLOT(sl_update()));
    connect(queryIdEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_update()));
    connect(browseOutputButton, SIGNAL(clicked(bool)), SLOT(sl_update()));

    sl_update();
}

void BlastDBCmdDialog::accept() {
    if (!dbSelector->validateDatabaseDir()) {
        return;
    }
    settings.query = queryIdEdit->text();
    settings.databasePath = dbSelector->getDatabasePath();
    settings.outputPath = saveController->getSaveFileName();
    settings.isNuclDatabase = dbSelector->isNuclDatabase();
    settings.addToProject = addToProjectBox->isChecked();

    QDialog::accept();
}

const BlastDBCmdSupportTaskSettings& BlastDBCmdDialog::getTaskSettings() const {
    return settings;
}

void BlastDBCmdDialog::sl_update() {
    bool outputPathIsSet = !saveController->getSaveFileName().isEmpty();
    bool queryIsSet = !queryIdEdit->text().isEmpty();

    fetchButton->setEnabled(dbSelector->isInputDataValid() && outputPathIsSet && queryIsSet);
}

void BlastDBCmdDialog::initSaveController() {
    QString domain = "blast_result";
    QString dir = LastUsedDirHelper::getLastUsedDir(domain, GUrlUtils::getDefaultDataPath());
    QString fileName = GUrlUtils::fixFileName(settings.query.isEmpty() ? "blast_result" : settings.query).replace('.', '_') + ".fa";

    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::FASTA;
    config.defaultFileName = dir + "/" + fileName;
    config.defaultDomain = domain;
    config.rollFileName = true;
    config.fileDialogButton = browseOutputButton;
    config.fileNameEdit = outputPathLineEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Set a result FASTA file name");

    saveController = new SaveDocumentController(config, {BaseDocumentFormats::FASTA}, this);
}

void BlastDBCmdDialog::setQueryId(const QString& queryId) {
    queryIdEdit->setText(queryId);
    settings.query = queryId;
    delete saveController;
    initSaveController();
}

}  // namespace U2
