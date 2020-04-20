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
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "KalignDialogController.h"

namespace U2 {

class QClearableDoubleSpinBox : public QDoubleSpinBox {
public:	
    QString textFromValue(double val) const {
        return isEnabled() ? QDoubleSpinBox::textFromValue(val) : "";
    }

    bool event(QEvent *ev) {
        if(ev->type() == QEvent::EnabledChange) {
            this->setValue(this->value());
        }
        return QDoubleSpinBox::event(ev);
    }

};

KalignDialogController::KalignDialogController(QWidget* w, const MultipleSequenceAlignment& _ma, KalignTaskSettings& _settings) 
: QDialog(w), ma(_ma->getCopy()), settings(_settings)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749026");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    setupUiExt();
    inputGroupBox->setVisible(false);
    this->adjustSize();
    translateCheckBox->setEnabled(ma->getAlphabet()->isNucleic());
    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    assert(!aminoTs.empty());
    foreach(DNATranslation* t, aminoTs) {
        translationTableBox->addItem(t->getTranslationName());
        }

}

void KalignDialogController::setupUiExt() {    
    gapOpenSpinBox->setEnabled(false);
    gapExtensionPenaltySpinBox->setEnabled(false);
    terminalGapSpinBox->setEnabled(false);
    bonusScoreSpinBox->setEnabled(false);

    if(ma->getAlphabet()->isAmino()) {
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

    QObject::connect(gapOpenCheckBox, SIGNAL(clicked(bool)), gapOpenSpinBox, SLOT(setEnabled(bool)));
    QObject::connect(gapExtensionPenaltyCheckBox, SIGNAL(clicked(bool)), gapExtensionPenaltySpinBox, SLOT(setEnabled(bool)));
    QObject::connect(terminalGapCheckBox, SIGNAL(clicked(bool)), terminalGapSpinBox, SLOT(setEnabled(bool)));
    QObject::connect(bonusScoreCheckBox, SIGNAL(clicked(bool)), bonusScoreSpinBox, SLOT(setEnabled(bool)));

}

void KalignDialogController::accept() {

    if(gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if(gapExtensionPenaltyCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtensionPenaltySpinBox->value();
    }
    if(terminalGapCheckBox->isChecked()) {
        settings.termGapPenalty = terminalGapSpinBox->value();
    }
    QDialog::accept();
}

//KalignAlignWithExtFileSpecifyDialogController
KalignAlignWithExtFileSpecifyDialogController::KalignAlignWithExtFileSpecifyDialogController(QWidget* w, KalignTaskSettings& _settings)
    : QDialog(w),
      settings(_settings),
      saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749053");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();

    connect(inputFilePathButton, SIGNAL(clicked()), SLOT(sl_inputPathButtonClicked()));

    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    assert(!aminoTs.empty());
    foreach(DNATranslation* t, aminoTs) {
        translationTableBox->addItem(t->getTranslationName());
    }
}

void KalignAlignWithExtFileSpecifyDialogController::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir,
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true));
    if (lod.url.isEmpty()) {
        return;
    }
    inputFileLineEdit->setText(lod.url);
}

void KalignAlignWithExtFileSpecifyDialogController::initSaveController() {
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

bool KalignDialogController::translateToAmino() {
    return translateCheckBox->isChecked();
}

QString KalignDialogController::getTranslationId() {
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QStringList ids = tr->getDNATranslationIds(translationTableBox->currentText());
    assert(!ids.empty());

    return ids.first();

}

void KalignAlignWithExtFileSpecifyDialogController::accept() {

    if(gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if(gapExtensionPenaltyCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtensionPenaltySpinBox->value();
    }
    if(terminalGapCheckBox->isChecked()) {
        settings.termGapPenalty = terminalGapSpinBox->value();
    }
    if(inputFileLineEdit->text().isEmpty()){
        QMessageBox::information(this, tr("Kalign with Align"),
                                 tr("Input file is not set!") );
    }else if(saveController->getSaveFileName().isEmpty()){
        QMessageBox::information(this, tr("Kalign with Align"),
                                 tr("Output file is not set!") );
    }
    else{
        settings.outputFilePath = saveController->getSaveFileName();
        settings.inputFilePath = inputFileLineEdit->text();
        QDialog::accept();
    }

}

}//namespace

