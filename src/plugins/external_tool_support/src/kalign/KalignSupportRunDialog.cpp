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

#include "KalignSupportRunDialog.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/FileFilters.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

Kalign3DialogWithMsaInput::Kalign3DialogWithMsaInput(QWidget* w, const Msa& _ma, Kalign3Settings& _settings)
    : QDialog(w), ma(_ma->getCopy()), settings(_settings) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930935");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // Set defaults.
    Kalign3Settings defaultSettings = Kalign3Settings::getDefaultSettings(ma->getAlphabet());
    gapOpenPenaltySpinBox->setValue(defaultSettings.gapOpenPenalty);
    gapExtensionPenaltySpinBox->setValue(defaultSettings.gapExtensionPenalty);
    terminalGapExtensionPenaltySpinBox->setValue(defaultSettings.terminalGapExtensionPenalty);
    threadCountSpinBox->setValue(defaultSettings.nThreads);

    QObject::connect(gapOpenPenaltyCheckBox, &QCheckBox::clicked, gapOpenPenaltySpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(gapExtensionPenaltyCheckBox, &QCheckBox::clicked, gapExtensionPenaltySpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(terminalGapExtensionPenaltyCheckBox, &QCheckBox::clicked, terminalGapExtensionPenaltySpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(threadCountCheckBox, &QCheckBox::clicked, threadCountSpinBox, &QSpinBox::setEnabled);

    inputGroupBox->setVisible(false);
    this->adjustSize();
}

void Kalign3DialogWithMsaInput::accept() {
    settings.gapOpenPenalty = gapOpenPenaltyCheckBox->isChecked() ? gapOpenPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.gapExtensionPenalty = gapExtensionPenaltyCheckBox->isChecked() ? gapExtensionPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.terminalGapExtensionPenalty = terminalGapExtensionPenaltyCheckBox->isChecked() ? terminalGapExtensionPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.nThreads = threadCountCheckBox->isChecked() ? threadCountSpinBox->value() : (int)Kalign3Settings::VALUE_IS_NOT_SET;
    QDialog::accept();
}

// Kalign3DialogWithFileInput
Kalign3DialogWithFileInput::Kalign3DialogWithFileInput(QWidget* w, Kalign3Settings& _settings)
    : QDialog(w),
      settings(_settings) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930983");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();

    connect(inputFilePathButton, &QToolButton::clicked, this, &Kalign3DialogWithFileInput::sl_inputPathButtonClicked);
}

void Kalign3DialogWithFileInput::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir, FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT}));
    CHECK(!lod.url.isEmpty(), );
    inputFileLineEdit->setText(lod.url);
}

void Kalign3DialogWithFileInput::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = outputFilePathButton;
    config.fileNameEdit = outputFileLineEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save an multiple alignment file");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::CLUSTAL_ALN;

    saveController = new SaveDocumentController(config, formats, this);
}

void Kalign3DialogWithFileInput::accept() {
    if (inputFileLineEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("Align with Kalign"), tr("Input file is not set!"));
        return;
    }
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::information(this, tr("Align with Kalign"), tr("Output file is not set!"));
        return;
    }
    settings.outputFilePath = saveController->getSaveFileName();
    settings.inputFilePath = inputFileLineEdit->text();
    settings.gapOpenPenalty = gapOpenPenaltyCheckBox->isChecked() ? gapOpenPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.gapExtensionPenalty = gapExtensionPenaltyCheckBox->isChecked() ? gapExtensionPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.terminalGapExtensionPenalty = terminalGapExtensionPenaltyCheckBox->isChecked() ? terminalGapExtensionPenaltySpinBox->value() : Kalign3Settings::VALUE_IS_NOT_SET;
    settings.nThreads = threadCountCheckBox->isChecked() ? threadCountSpinBox->value() : (int)Kalign3Settings::VALUE_IS_NOT_SET;
    QDialog::accept();
}

}  // namespace U2
