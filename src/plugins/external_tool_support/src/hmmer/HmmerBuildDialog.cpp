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
#include <U2Core/MultipleSequenceAlignment.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "HmmerBuildDialog.h"
#include "HmmerBuildFromFileTask.h"
#include "HmmerBuildFromMsaTask.h"

namespace U2 {

UHMM3BuildDialogModel::UHMM3BuildDialogModel()
    : alignmentUsing(false)
{

}

const QString HmmerBuildDialog::MA_FILES_DIR_ID     = "uhmmer3_build_ma_files_dir";
const QString HmmerBuildDialog::HMM_FILES_DIR_ID    = "uhmmer3_build_hmm_files_dir";

void HmmerBuildDialog::setSignalsAndSlots() {
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(maOpenFileButton,       SIGNAL(clicked()),      SLOT(sl_maOpenFileButtonClicked()));
    connect(okButton,               SIGNAL(clicked()),      SLOT(sl_buildButtonClicked()));
    connect(cancelButton,           SIGNAL(clicked()),      SLOT(sl_cancelButtonClicked()));
    connect(mcFastRadioButton,      SIGNAL(toggled(bool)),  SLOT(sl_fastMCRadioButtonChanged(bool)));
    connect(wblosumRSWRadioButton,  SIGNAL(toggled(bool)),  SLOT(sl_wblosumRSWRadioButtonChanged(bool)));
    connect(eentESWRadioButton,     SIGNAL(toggled(bool)),  SLOT(sl_eentESWRadioButtonChanged(bool)));
    connect(eclustESWRadioButton,   SIGNAL(toggled(bool)),  SLOT(sl_eclustESWRadioButtonChanged(bool)));
    connect(esetESWRadioButton,     SIGNAL(toggled(bool)),  SLOT(sl_esetESWRadioButtonChanged(bool)));
    
    //temporary disabling of strange label/spinbox
    fragThreshDoubleSpinBox->setVisible(false);
    fragthreshLabel->setVisible(false);
}

void HmmerBuildDialog::initialize() {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748984");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Build"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();
    setModelValues(); // build settings are default here
    setSignalsAndSlots();
}

void HmmerBuildDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = HMM_FILES_DIR_ID;
    config.defaultFormatId = "hmm";
    config.fileDialogButton = outHmmfileToolButton;
    config.fileNameEdit = outHmmfileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select hmm file to create");

    SaveDocumentController::SimpleFormatsInfo formatsInfo;
    formatsInfo.addFormat("hmm", "HMM profile", QStringList() << "hmm");

    saveController = new SaveDocumentController(config, formatsInfo, this);
}

HmmerBuildDialog::HmmerBuildDialog(const MultipleSequenceAlignment &ma, QWidget * parent)
    : QDialog(parent),
      saveController(NULL)
{
    initialize();
    model.alignment = ma->getCopy();
    model.alignmentUsing = !model.alignment->isEmpty();
    
    if (model.alignmentUsing) {
        maLoadFromFileEdit->hide();
        maLoadFromFileLabel->hide();
        maOpenFileButton->hide();
    }
}

void HmmerBuildDialog::setModelValues() {
    symfracDoubleSpinBox->setValue(model.buildSettings.symfrac);
    widRSWDoubleSpinBox->setValue(model.buildSettings.wid);
    eidESWDoubleSpinBox->setValue(model.buildSettings.eid);
    esetESWDoubleSpinBox->setValue(model.buildSettings.eset);
    emlSpinBox->setValue(model.buildSettings.eml);
    emnSpinBox->setValue(model.buildSettings.emn);
    evlSpinBox->setValue(model.buildSettings.evl);
    evnSpinBox->setValue(model.buildSettings.evn);
    eflSpinBox->setValue(model.buildSettings.efl);
    efnSpinBox->setValue(model.buildSettings.efn);
    eftDoubleSpinBox->setValue(model.buildSettings.eft);
    seedSpinBox->setValue(model.buildSettings.seed);
    esigmaDoubleSpinBox->setValue(model.buildSettings.esigma);
    fragThreshDoubleSpinBox->setValue(model.buildSettings.fragtresh);
}

void HmmerBuildDialog::sl_maOpenFileButtonClicked() {
    LastUsedDirHelper helper(MA_FILES_DIR_ID);
    helper.url = U2FileDialog::getOpenFileName(this, tr("Select multiple alignment file"),
        helper, DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT , true));
    if (!helper.url.isEmpty()) {
        maLoadFromFileEdit->setText(helper.url);
    }
}

void HmmerBuildDialog::getModelValues() {
    model.buildSettings.symfrac     = symfracDoubleSpinBox->value();
    model.buildSettings.wid         = widRSWDoubleSpinBox->value();
    model.buildSettings.eid         = eidESWDoubleSpinBox->value();
    model.buildSettings.eset        = esetESWDoubleSpinBox->value();
    model.buildSettings.eml         = emlSpinBox->value();
    model.buildSettings.emn         = emnSpinBox->value();
    model.buildSettings.evl         = evlSpinBox->value();
    model.buildSettings.evn         = evnSpinBox->value();
    model.buildSettings.efl         = eflSpinBox->value();
    model.buildSettings.efn         = efnSpinBox->value();
    model.buildSettings.eft         = eftDoubleSpinBox->value();
    model.buildSettings.seed        = seedSpinBox->value();
    model.buildSettings.esigma      = esigmaDoubleSpinBox->value();
    model.buildSettings.fragtresh   = fragThreshDoubleSpinBox->value();
    if (0 != ereESWDoubleSpinBox->value()) {
        model.buildSettings.ere = ereESWDoubleSpinBox->value();
    }
    
    if (mcFastRadioButton->isChecked()) {
        model.buildSettings.modelConstructionStrategy = HmmerBuildSettings::p7_ARCH_FAST;
    } else {
        model.buildSettings.modelConstructionStrategy = HmmerBuildSettings::p7_ARCH_HAND;
    }
    
    if (wgscRSWRadioButton->isChecked()) {
        model.buildSettings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_GSC;
    } else if (wblosumRSWRadioButton->isChecked()) {
        model.buildSettings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_BLOSUM;
    } else if (wpbRSWRadioButton->isChecked()) {
        model.buildSettings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_PB;
    } else if (wnoneRSWRadioButton->isChecked()) {
        model.buildSettings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_NONE;
    } else if (wgivenRSWRadioButton->isChecked()) {
        model.buildSettings.relativeSequenceWeightingStrategy = HmmerBuildSettings::p7_WGT_GIVEN;
    } else {
        assert(false);
    }
    
    if (eentESWRadioButton->isChecked()) {
        model.buildSettings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_ENTROPY;
    } else if (eclustESWRadioButton->isChecked()) {
        model.buildSettings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_CLUST;
    } else if (enoneESWRadioButton->isChecked()) {
        model.buildSettings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_NONE;
    } else if (esetESWRadioButton->isChecked()) {
        model.buildSettings.effectiveSequenceWeightingStrategy = HmmerBuildSettings::p7_EFFN_SET;
    } else {
        assert(false);
    }
    
    model.buildSettings.profileUrl = saveController->getSaveFileName();
    model.inputFile = maLoadFromFileEdit->text();
}

QString HmmerBuildDialog::checkModel() {
//    assert(checkUHMM3BuildSettings(&model.buildSettings.inner));
    if (!model.alignmentUsing && model.inputFile.isEmpty()) {
        return tr("input file is empty");
    }
    if (model.buildSettings.profileUrl.isEmpty()) {
        return tr("output hmm file is empty");
    }
    return QString();
}

void HmmerBuildDialog::sl_buildButtonClicked() {
    getModelValues();
    QString err = checkModel();
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Error: bad arguments!"), err);
        return;
    }
    
    Task * buildTask = NULL;
    if (model.alignmentUsing) {
        buildTask = new HmmerBuildFromMsaTask(model.buildSettings, model.alignment);
    } else {
        buildTask = new HmmerBuildFromFileTask(model.buildSettings, model.inputFile);
    }
    assert(NULL != buildTask);
    
    AppContext::getTaskScheduler()->registerTopLevelTask(buildTask);
    QDialog::accept();
}

void HmmerBuildDialog::sl_cancelButtonClicked() {
    reject();
}

void HmmerBuildDialog::sl_fastMCRadioButtonChanged(bool checked) {
    mcFastSymfracLabel->setEnabled(checked);
    symfracDoubleSpinBox->setEnabled(checked);
}

void HmmerBuildDialog::sl_wblosumRSWRadioButtonChanged(bool checked) {
    widRSWLabel->setEnabled(checked);
    widRSWDoubleSpinBox->setEnabled(checked);
}

void HmmerBuildDialog::sl_eentESWRadioButtonChanged(bool checked) {
    ereESWDoubleSpinBox->setEnabled(checked);
    esigmaDoubleSpinBox->setEnabled(checked);
    esigmaLabel->setEnabled(checked);
    ereLabel->setEnabled(checked);
}

void HmmerBuildDialog::sl_eclustESWRadioButtonChanged(bool checked) {
    eidESWLabel->setEnabled(checked);
    eidESWDoubleSpinBox->setEnabled(checked);
}

void HmmerBuildDialog::sl_esetESWRadioButtonChanged(bool checked) {
    esetESWDoubleSpinBox->setEnabled(checked);
}

}   // namespace U2
