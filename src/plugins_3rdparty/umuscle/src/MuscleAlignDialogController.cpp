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

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "MuscleAlignDialogController.h"

namespace U2 {

MuscleAlignDialogController::MuscleAlignDialogController(QWidget* w, const MultipleSequenceAlignment& _ma, MuscleTaskSettings& _settings) 
    : QDialog(w), ma(_ma->getCopy()), settings(_settings)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748988");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    translateCheckBox->setEnabled(ma->getAlphabet()->isNucleic());
    inputGroupBox->setVisible(false);
    this->adjustSize();

    rangeEndSB->setMaximum(ma->getLength());
    rangeEndSB->setValue(ma->getLength());

    if (settings.alignRegion) {
        customRangeRB->setChecked(true);
        rangeStartSB->setValue(settings.regionToAlign.startPos);
        rangeEndSB->setValue(settings.regionToAlign.endPos());
    }
    connect(confBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onPresetChanged(int)));
    initPresets();
    foreach(const MuscleAlignPreset* p, presets.qlist) {
        confBox->addItem(p->name);
    }
    
    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    assert(!aminoTs.empty());
    foreach(DNATranslation* t, aminoTs) {
        translationTableBox->addItem(t->getTranslationName());
    }

}

void MuscleAlignDialogController::accept() {
    int n = confBox->currentIndex();
    assert(n >=0 && n < presets.qlist.size());
    const MuscleAlignPreset* p = presets.qlist[n];
    p->apply(settings);
    if(dynamic_cast<const DefaultModePreset*>(p) != NULL) {
        settings.mode = Default;
    } else if(dynamic_cast<const LargeModePreset*>(p) != NULL) {
        settings.mode = Large;
    } else if(dynamic_cast<const RefineModePreset*>(p) != NULL) {
        settings.mode = Refine;
    } else {
        assert(false);
        settings.mode = Default;
    }
    
    settings.stableMode = stableCB->isChecked();

    if (wholeRangeRB->isChecked()) {
        settings.regionToAlign = U2Region(0, ma->getLength());
        settings.alignRegion = false;
    } else {
        int startPos = rangeStartSB->value() - 1;
        int endPos = rangeEndSB->value() - 1;
        if (endPos - startPos < 2) {
            QMessageBox::critical(NULL, tr("Error"), tr("Illegal alignment region"));
            rangeStartSB->setFocus();
            return;
        }
        settings.alignRegion = true;
        settings.regionToAlign = U2Region(startPos,  endPos - startPos + 1);
    }

    if (maxItersCheckBox->isChecked()) {
        settings.maxIterations = maxItersSpinBox->value();
        assert(settings.maxIterations >= 2);
    }
    if (maxMinutesCheckBox->isChecked()) {
        settings.maxSecs = maxMinutesSpinBox->value() * 60; 
        assert(settings.maxSecs > 0);
    }
    QDialog::accept();
}

void MuscleAlignDialogController::sl_onPresetChanged(int newPreset) {
    assert(newPreset>=0 && newPreset < presets.qlist.size());
    const MuscleAlignPreset* p = presets.qlist[newPreset];
    confEdit->setText(p->desc);
}

////////////////////////////////////////
//MuscleAlignWithExtFileSpecifyDialogController
MuscleAlignWithExtFileSpecifyDialogController::MuscleAlignWithExtFileSpecifyDialogController(QWidget* w, MuscleTaskSettings& _settings)
    : QDialog(w),
      settings(_settings),
      saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748987");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();

    connect(inputFilePathButton, SIGNAL(clicked()), SLOT(sl_inputPathButtonClicked()));

    //we don`t know length of MA, need check this at task
    rangeStartSB->setValue(0);
    rangeEndSB->setValue(500);

    connect(confBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onPresetChanged(int)));
    initPresets();
    foreach(const MuscleAlignPreset* p, presets.qlist) {
        confBox->addItem(p->name);
    }
    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    assert(!aminoTs.empty());
    foreach(DNATranslation* t, aminoTs) {
        translationTableBox->addItem(t->getTranslationName());
    }
}

void MuscleAlignWithExtFileSpecifyDialogController::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir,
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true));
    if (lod.url.isEmpty()) {
        return;
    }
    inputFileLineEdit->setText(lod.url);
}

void MuscleAlignWithExtFileSpecifyDialogController::initSaveController() {
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

void MuscleAlignWithExtFileSpecifyDialogController::accept() {
    int n = confBox->currentIndex();
    assert(n >=0 && n < presets.qlist.size());
    const MuscleAlignPreset* p = presets.qlist[n];
    p->apply(settings);
    if(dynamic_cast<const DefaultModePreset*>(p) != NULL) {
        settings.mode = Default;
    } else if(dynamic_cast<const LargeModePreset*>(p) != NULL) {
        settings.mode = Large;
    } else if(dynamic_cast<const RefineModePreset*>(p) != NULL) {
        settings.mode = Refine;
    } else {
        assert(false);
        settings.mode = Default;
    }
    settings.stableMode = stableCB->isChecked();
    
    if (wholeRangeRB->isChecked()) {
        settings.alignRegion = false;
    } else {
        int startPos = rangeStartSB->value() - 1;
        int endPos = rangeEndSB->value();
        if (endPos - startPos < 2) {
            QMessageBox::critical(NULL, tr("Error"), tr("Illegal alignment region"));
            rangeStartSB->setFocus();
            return;
        }
        settings.alignRegion = true;
        settings.regionToAlign = U2Region(startPos,  endPos - startPos);
    }

    if (maxItersCheckBox->isChecked()) {
        settings.maxIterations = maxItersSpinBox->value();
        assert(settings.maxIterations >= 2);
    }
    if (maxMinutesCheckBox->isChecked()) {
        settings.maxSecs = maxMinutesSpinBox->value() * 60;
        assert(settings.maxSecs > 0);
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

void MuscleAlignWithExtFileSpecifyDialogController::sl_onPresetChanged(int newPreset) {//???
    assert(newPreset>=0 && newPreset < presets.qlist.size());
    const MuscleAlignPreset* p = presets.qlist[newPreset];
    confEdit->setText(p->desc);
}
void MuscleAlignWithExtFileSpecifyDialogController::initPresets() {
    presets.qlist.append(new DefaultModePreset());
    presets.qlist.append(new LargeModePreset());
    presets.qlist.append(new RefineModePreset());
}

//////////////////////////////////////////////////////////////////////////
// presets
DefaultModePreset::DefaultModePreset() {
    name = MuscleAlignDialogController::tr("MUSCLE default");
    desc = MuscleAlignDialogController::tr("<p>The default settings are designed to give the best accuracy");
    desc+= MuscleAlignDialogController::tr("<p><b>Command line:</b> muscle <no-parameters>");
}

LargeModePreset::LargeModePreset() {
    name = MuscleAlignDialogController::tr("Large alignment");
    desc = MuscleAlignDialogController::tr("<p>If you have a large number of sequences (a few thousand), or they are very long, then the default settings may be too slow for practical use. A good compromise between speed and accuracy is to run just the first two iterations of the algorithm");
    desc+= MuscleAlignDialogController::tr("<p><b>Command line:</b> muscle <i>-maxiters 2</i>");
}

RefineModePreset::RefineModePreset() {
    name = MuscleAlignDialogController::tr("Refine only");
    desc = MuscleAlignDialogController::tr("<p>Improves existing alignment without complete realignment");
    desc+= MuscleAlignDialogController::tr("<p><b>Command line:</b> muscle <i>-refine</i>");
}


void MuscleAlignDialogController::initPresets() {
    presets.qlist.append(new DefaultModePreset());
    presets.qlist.append(new LargeModePreset());
    presets.qlist.append(new RefineModePreset());
}

bool MuscleAlignDialogController::translateToAmino()
{
    return translateCheckBox->isChecked();
}

QString MuscleAlignDialogController::getTranslationId() {
    
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QStringList ids = tr->getDNATranslationIds(translationTableBox->currentText());
    assert(!ids.empty());
    
    return ids.first();

}


}//namespace
