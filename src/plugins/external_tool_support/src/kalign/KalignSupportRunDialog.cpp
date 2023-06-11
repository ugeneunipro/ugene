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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FileFilters.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "KalignSupportRunDialog.h"

namespace U2 {

Kalign3DialogWithMsaInput::Kalign3DialogWithMsaInput(QWidget* w, const MultipleSequenceAlignment& _ma, Kalign3Settings& _settings)
    : QDialog(w), ma(_ma->getExplicitCopy()), settings(_settings) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930935");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    gapOpenSpinBox->setEnabled(false);
    gapExtensionPenaltySpinBox->setEnabled(false);
    terminalGapSpinBox->setEnabled(false);
    bonusScoreSpinBox->setEnabled(false);

    if (ma->getAlphabet()->isAmino()) {
        gapOpenSpinBox->setValue(53.9);
        gapExtensionPenaltySpinBox->setValue(8.52);
        terminalGapSpinBox->setValue(4.42);
        bonusScoreSpinBox->setValue(0.02);
    } else {
        gapOpenSpinBox->setValue(217);
        gapExtensionPenaltySpinBox->setValue(39.4);
        terminalGapSpinBox->setValue(292.6);
        bonusScoreSpinBox->setValue(28.3);
    }

    QObject::connect(gapOpenCheckBox, &QCheckBox::clicked, gapOpenSpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(gapExtensionPenaltyCheckBox, &QCheckBox::clicked, gapExtensionPenaltySpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(terminalGapCheckBox, &QCheckBox::clicked, terminalGapSpinBox, &QDoubleSpinBox::setEnabled);
    QObject::connect(bonusScoreCheckBox, &QCheckBox::clicked, bonusScoreSpinBox, &QDoubleSpinBox::setEnabled);

    inputGroupBox->setVisible(false);
    this->adjustSize();
    translateCheckBox->setEnabled(ma->getAlphabet()->isNucleic());
    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    for (DNATranslation* t : qAsConst(aminoTs)) {
        translationTableBox->addItem(t->getTranslationName());
    }
}

void Kalign3DialogWithMsaInput::accept() {
    if (gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if (gapExtensionPenaltyCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtensionPenaltySpinBox->value();
    }
    if (terminalGapCheckBox->isChecked()) {
        settings.termGapPenalty = terminalGapSpinBox->value();
    }
    QDialog::accept();
}

bool Kalign3DialogWithMsaInput::isTranslateToAmino() const {
    return translateCheckBox->isChecked();
}

QString Kalign3DialogWithMsaInput::getTranslationId() const {
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QStringList ids = tr->getDNATranslationIds(translationTableBox->currentText());
    SAFE_POINT(!ids.empty(), "No translations found!", "");
    return ids.first();
}

// KalignAlignWithExtFileSpecifyDialogController
Kalign3DialogWithFileInput::Kalign3DialogWithFileInput(QWidget* w, Kalign3Settings& _settings)
    : QDialog(w),
      settings(_settings) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930983");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();

    connect(inputFilePathButton, &QToolButton::clicked, this, &Kalign3DialogWithFileInput::sl_inputPathButtonClicked);

    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    for (DNATranslation* t : qAsConst(aminoTs)) {
        translationTableBox->addItem(t->getTranslationName());
    }
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
    if (gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if (gapExtensionPenaltyCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtensionPenaltySpinBox->value();
    }
    if (terminalGapCheckBox->isChecked()) {
        settings.termGapPenalty = terminalGapSpinBox->value();
    }
    if (inputFileLineEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("Align with Kalign"), tr("Input file is not set!"));
    } else if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::information(this, tr("Align with Kalign"), tr("Output file is not set!"));
    } else {
        settings.outputFilePath = saveController->getSaveFileName();
        settings.inputFilePath = inputFileLineEdit->text();
        QDialog::accept();
    }
}

}  // namespace U2
