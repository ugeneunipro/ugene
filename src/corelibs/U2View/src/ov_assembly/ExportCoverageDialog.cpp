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

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportCoverageDialog.h"

namespace U2 {

const QString ExportCoverageDialog::DIR_HELPER_NAME = "export_assembly_coverage";

ExportCoverageDialog::ExportCoverageDialog(const QString &assemblyName, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    initLayout();
    init(assemblyName);
    connect(cbFormat, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
}

ExportCoverageSettings::Format ExportCoverageDialog::getFormat() const {
    if (ExportCoverageSettings::HISTOGRAM == cbFormat->currentText()) {
        return ExportCoverageSettings::Histogram;
    } else if (ExportCoverageSettings::PER_BASE == cbFormat->currentText()) {
        return ExportCoverageSettings::PerBase;
    } else if (ExportCoverageSettings::BEDGRAPH == cbFormat->currentText()) {
        return ExportCoverageSettings::Bedgraph;
    } else {
        Q_ASSERT(false);
        return ExportCoverageSettings::PerBase;
    }
}

ExportCoverageSettings ExportCoverageDialog::getSettings() const {
    ExportCoverageSettings settings;
    settings.url = saveController->getSaveFileName();
    settings.compress = chbCompress->isChecked();
    settings.exportCoverage = chbExportCoverage->isChecked();
    settings.exportBasesCount = chbExportBasesQuantity->isChecked();
    settings.threshold = sbThreshold->value();
    return settings;
}

void ExportCoverageDialog::accept() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("The output file path is not specified."));
        leFilePath->setFocus();
        return;
    }
    if (!checkPermissions()) {
        QMessageBox::critical(this, tr("Error"), tr("Not enough permissions to write here. Please set another output file."));
        return;
    }
    if (ExportCoverageSettings::PER_BASE == saveController->getFormatIdToSave() && !chbExportCoverage->isChecked() && !chbExportBasesQuantity->isChecked()) {
        QMessageBox::critical(this, tr("Error"), tr("Nothing to export"));
        return;
    }

    LastUsedDirHelper dirHelper(DIR_HELPER_NAME);
    dirHelper.url = saveController->getSaveFileName();

    QDialog::accept();
}

void ExportCoverageDialog::sl_formatChanged(const QString &format) {
    gbAdditionalOptions->setVisible(ExportCoverageSettings::PER_BASE == format);
    adjustSize();
}

void ExportCoverageDialog::initLayout() {
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    new HelpButton(this, buttonBox, "24748908");
    gbAdditionalOptions->hide();
    setMaximumHeight(layout()->minimumSize().height());
    adjustSize();
}

void ExportCoverageDialog::init(QString assemblyName) {
    SaveDocumentControllerConfig conf;
    conf.fileDialogButton = tbFilePath;
    conf.fileNameEdit = leFilePath;
    conf.formatCombo = cbFormat;
    conf.compressCheckbox = chbCompress;
    conf.parentWidget = this;
    conf.saveTitle = tr("Export coverage");
    conf.defaultFormatId = ExportCoverageSettings::BEDGRAPH;
    conf.defaultDomain = DIR_HELPER_NAME;

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(ExportCoverageSettings::HISTOGRAM, QStringList() << ExportCoverageSettings::HISTOGRAM_EXTENSION);
    formats.addFormat(ExportCoverageSettings::PER_BASE, QStringList() << ExportCoverageSettings::PER_BASE_EXTENSION);
    formats.addFormat(ExportCoverageSettings::BEDGRAPH, QStringList() << ExportCoverageSettings::BEDGRAPH_EXTENSION);

    LastUsedDirHelper dirHelper(DIR_HELPER_NAME, GUrlUtils::getDefaultDataPath());
    assemblyName.replace(QRegExp("[^0-9a-zA-Z._\\-]"), "_").replace(QRegExp("_+"), "_");
    conf.defaultFileName = dirHelper.dir + "/" + assemblyName + "_coverage" +
            cbFormat->itemData(cbFormat->currentIndex()).toString() + (chbCompress->isChecked() ? ExportCoverageSettings::COMPRESSED_EXTENSION : "");

    saveController = new SaveDocumentController(conf, formats, this);
}

bool ExportCoverageDialog::checkPermissions() const {
    QFileInfo fileInfo(saveController->getSaveFileName());
    QFileInfo dirInfo(fileInfo.absoluteDir().absolutePath());
    bool isFileExist = fileInfo.exists();
    bool isFileWritable = fileInfo.isWritable();
    bool isDirWritable = dirInfo.isWritable();
    while (!dirInfo.exists()) {
        dirInfo = QFileInfo(dirInfo.dir().absolutePath());
        isDirWritable = dirInfo.isWritable();
    }
    return (isFileExist && isFileWritable) || isDirWritable;
}

}   // namespace U2
