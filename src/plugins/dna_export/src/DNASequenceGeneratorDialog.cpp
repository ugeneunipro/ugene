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
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "DNASequenceGenerator.h"
#include "DNASequenceGeneratorDialog.h"

namespace U2 {

#define ROOT_SETTING QString("dna_export/")
#define SELECTED_OPTION QString("selected_option")

#define OPTION_REFERENCE QString("reference")
#define OPTION_BASE_CONTENT QString("base_content")
#define OPTION_SKEW QString("skew")

static QMap<char, qreal> initContent() {
    QMap<char, qreal> res;
    res['A'] = 0.25;
    res['C'] = 0.25;
    res['G'] = 0.25;
    res['T'] = 0.25;
    return res;
}

QMap<char, qreal> DNASequenceGeneratorDialog::content = initContent();

DNASequenceGeneratorDialog::DNASequenceGeneratorDialog(QWidget* p)
    : QDialog(p),
      saveController(NULL),
      generateButton(NULL),
      cancelButton(NULL),
      percentMap(content),
      gcSkew(0)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749018");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Generate"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    seedSpinBox->setEnabled(false);
    referenceButton->setChecked(true);

    percentASpin->setValue(percentMap.value('A')*100.0);
    percentCSpin->setValue(percentMap.value('C')*100.0);
    percentGSpin->setValue(percentMap.value('G')*100.0);
    percentTSpin->setValue(percentMap.value('T')*100.0);
    gcSkew = ((float)((int)(percentMap.value('G')*100) - (int)(percentMap.value('C')*100)))
        /((int)(percentMap.value('G')*100) + (int)(percentMap.value('C')*100));
    int iGCSkew = (int)(gcSkew * 100);
    gcSkew = (float(iGCSkew))/100.0;
    percentGCSpin->setValue(gcSkew);

    initSaveController();

    generateButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(inputButton, SIGNAL(clicked()), SLOT(sl_browseReference()));
    connect(generateButton, SIGNAL(clicked()), SLOT(sl_generate()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(seedCheckBox, SIGNAL(stateChanged (int)), SLOT(sl_seedStateChanged(int)));
    connect(referenceButton, SIGNAL(clicked()), SLOT(sl_enableRefMode()));
    connect(baseContentRadioButton, SIGNAL(clicked()), SLOT(sl_enableBaseMode()));
    connect(gcSkewRadioButton, SIGNAL(clicked()), SLOT(sl_enableGCSkewMode()));

    QString lastUsedMode = AppContext::getSettings()->getValue(ROOT_SETTING + SELECTED_OPTION).toString();
    if (lastUsedMode == OPTION_BASE_CONTENT) {
        sl_enableBaseMode();
    } else if (lastUsedMode == OPTION_SKEW) {
        sl_enableGCSkewMode();
    } else {
        sl_enableRefMode();
    }
}

void DNASequenceGeneratorDialog::sl_seedStateChanged(int state) {
    seedSpinBox->setEnabled(state == Qt::Checked);
}

void DNASequenceGeneratorDialog::initSaveController(){
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::FASTA;
    config.fileDialogButton = outputButton;
    config.fileNameEdit = outputEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;
    config.saveTitle = tr("Save sequences");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::SEQUENCE
                                           << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    formatConstraints.allowPartialTypeMapping = true;

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

void DNASequenceGeneratorDialog::sl_browseReference() {
    LastUsedDirHelper lod;
    QString filter = DNASequenceGenerator::prepareReferenceFileFilter();
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open file"), lod.dir, filter);
    inputEdit->setText(lod.url);
}


void DNASequenceGeneratorDialog::sl_generate() {
    DNASequenceGeneratorConfig cfg;
    cfg.addToProj = addToProjCBox->isChecked();
    cfg.length = lengthSpin->value();
    cfg.numSeqs = seqNumSpin->value();
    cfg.useRef = referenceButton->isChecked();
    cfg.refUrl = inputEdit->text();
    cfg.outUrl = saveController->getSaveFileName();
    cfg.sequenceName = "Sequence ";
    cfg.formatId = saveController->getFormatIdToSave();
    cfg.window = windowSpinBox->value();
    cfg.alphabet = cfg.useRef ? NULL : AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    cfg.seed = seedCheckBox->isChecked() ? seedSpinBox->value() : -1;

    if (cfg.window > cfg.length) {
        QMessageBox::critical(this, tr("DNA Sequence Generator"), tr("Windows size bigger than sequence length"));
        return;
    }

    if (cfg.refUrl.isEmpty() && cfg.useRef) {
        QMessageBox::critical(this, tr("DNA Sequence Generator"), tr("Reference url is not specified."));
        return;
    }

    if (cfg.outUrl.isEmpty()) {
        QMessageBox::critical(this, tr("DNA Sequence Generator"), tr("Output file is no specified."));
        return;
    }

    Settings *s = AppContext::getSettings();
    if (baseContentRadioButton->isChecked()) {
        s->setValue(ROOT_SETTING + SELECTED_OPTION, OPTION_BASE_CONTENT);
        int percentA = percentASpin->value();
        int percentC = percentCSpin->value();
        int percentG = percentGSpin->value();
        int percentT = percentTSpin->value();
        int total = percentA + percentC + percentG + percentT;
        if (total != 100) {
            QMessageBox::critical(this, tr("Base content"), tr("Total percentage must be 100%"));
            return;
        }
        percentMap['A'] = percentA / 100.0;
        percentMap['C'] = percentC / 100.0;
        percentMap['G'] = percentG / 100.0;
        percentMap['T'] = percentT / 100.0;
        cfg.content = percentMap;
    } else if (gcSkewRadioButton->isChecked()){
        s->setValue(ROOT_SETTING + SELECTED_OPTION, OPTION_SKEW);
        gcSkew = percentGCSpin->value();
        qreal percentA = qrand();
        qreal percentC = qrand();
        qreal percentT = qrand();
        qreal percentG = qrand();
        qreal sum = percentA + percentC + percentG + percentT;
        percentA = percentA / sum * 100;
        percentG = percentG / sum * 100;
        percentC = percentC / sum * 100;
        percentT = percentT / sum * 100;
        qreal GC = percentG + percentC;

        percentC = (1 - gcSkew)* GC / 2;
        percentG = percentC + gcSkew * GC;
        if (percentC < 0 || percentC > 100 || percentG < 0 || percentG > 100) {
            QMessageBox::critical(this, tr("Base content"), tr("Incorrect GC Skew value"));
            return;
        }
        sum = percentA + percentC + percentG + percentT;
        percentA += 100 - sum;

        percentASpin->setValue(qRound(percentA));
        percentCSpin->setValue(qRound(percentC));
        percentGSpin->setValue(qRound(percentG));
        percentTSpin->setValue(qRound(percentT));

        percentMap['A'] = percentA / 100;
        percentMap['C'] = percentC / 100;
        percentMap['G'] = percentG / 100;
        percentMap['T'] = percentT / 100;
        cfg.content = percentMap;
    } else {
        s->setValue(ROOT_SETTING + SELECTED_OPTION, OPTION_REFERENCE);
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(new DNASequenceGeneratorTask(cfg));
    accept();

}

void DNASequenceGeneratorDialog::sl_enableRefMode() {
    referenceButton->setChecked(true);
    inputEdit->setEnabled(true);
    inputButton->setEnabled(true);
    percentASpin->setEnabled(false);
    percentCSpin->setEnabled(false);
    percentTSpin->setEnabled(false);
    percentGSpin->setEnabled(false);
    percentGCSpin->setEnabled(false);
}

void DNASequenceGeneratorDialog::sl_enableBaseMode() {
    baseContentRadioButton->setChecked(true);
    inputEdit->setEnabled(false);
    inputButton->setEnabled(false);
    percentASpin->setEnabled(true);
    percentCSpin->setEnabled(true);
    percentTSpin->setEnabled(true);
    percentGSpin->setEnabled(true);
    percentGCSpin->setEnabled(false);
}

void DNASequenceGeneratorDialog::sl_enableGCSkewMode() {
    gcSkewRadioButton->setChecked(true);
    inputEdit->setEnabled(false);
    inputButton->setEnabled(false);
    percentASpin->setEnabled(false);
    percentCSpin->setEnabled(false);
    percentTSpin->setEnabled(false);
    percentGSpin->setEnabled(false);
    percentGCSpin->setEnabled(true);
}


} //namespace
