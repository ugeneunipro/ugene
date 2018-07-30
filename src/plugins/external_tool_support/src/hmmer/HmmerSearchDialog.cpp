/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <math.h>

#include <QMessageBox>
#include <QPushButton>
#include <QButtonGroup>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "HmmerSearchDialog.h"
#include "HmmerSearchTask.h"

namespace U2 {

const QString HmmerSearchDialog::DOM_E_PLUS_PREFIX          = "1E+";
const QString HmmerSearchDialog::DOM_E_MINUS_PREFIX         = "1E";
const QString HmmerSearchDialog::HMM_FILES_DIR_ID           = "uhmmer3_search_dlg_impl_hmm_dir";
const QString HmmerSearchDialog::ANNOTATIONS_DEFAULT_NAME   = "hmm_signal";

HmmerSearchDialog::HmmerSearchDialog(U2SequenceObject *seqObj, QWidget *parent)
    : QDialog(parent), seqCtx(NULL)
{
    init(seqObj);
}

HmmerSearchDialog::HmmerSearchDialog(ADVSequenceObjectContext *seqCtx, QWidget *parent)
    : QDialog(parent), seqCtx(seqCtx)
{
    init(seqCtx->getSequenceObject());
}

void HmmerSearchDialog::init(U2SequenceObject *seqObj){
    setupUi(this);
    SAFE_POINT(NULL != seqObj, L10N::nullPointerError("sequence object"), );

    new HelpButton(this, buttonBox, "21433395");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    useScoreTresholdGroup.addButton(useExplicitScoreTresholdButton);
    useScoreTresholdGroup.addButton(useGATresholdsButton);
    useScoreTresholdGroup.addButton(useNCTresholdsButton);
    useScoreTresholdGroup.addButton(useTCTresholdsButton);
    useExplicitScoreTresholdButton->setChecked(true);

    model.sequence = QPointer<U2SequenceObject>(seqObj);
    setModelValues(); // default settings here

    // Annotations widget
    CreateAnnotationModel annModel;
    annModel.hideLocation = true;
    annModel.sequenceObjectRef = seqObj;
    annModel.useAminoAnnotationTypes = seqObj->getAlphabet()->isAmino();
    annModel.data->type = U2FeatureTypes::MiscSignal;
    annModel.data->name = ANNOTATIONS_DEFAULT_NAME;
    annModel.sequenceLen = seqObj->getSequenceLength();
    annotationsWidgetController = new CreateAnnotationWidgetController(annModel, this);

    QWidget *firstTab = tabWidget->widget(0);
    assert(NULL != firstTab);
    QVBoxLayout *curLayout = qobject_cast<QVBoxLayout *>(firstTab->layout());
    assert(NULL != curLayout);
    QWidget *aw = annotationsWidgetController->getWidget();
    curLayout->insertWidget(1, aw);

    QPushButton *searchButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(cancelButton,                   SIGNAL(clicked()),          SLOT(reject()));
    connect(searchButton,                   SIGNAL(clicked()),          SLOT(sl_okButtonClicked()));
    connect(useEvalTresholdsButton,         SIGNAL(toggled(bool)),      SLOT(sl_useEvalTresholdsButtonChanged(bool)));
    connect(useScoreTresholdsButton,        SIGNAL(toggled(bool)),      SLOT(sl_useScoreTresholdsButtonChanged(bool)));
    connect(useExplicitScoreTresholdButton, SIGNAL(toggled(bool)),      SLOT(sl_useExplicitScoreTresholdButton(bool)));
    connect(maxCheckBox,                    SIGNAL(stateChanged(int)),  SLOT(sl_maxCheckBoxChanged(int)));
    connect(domESpinBox,                    SIGNAL(valueChanged(int)),  SLOT(sl_domESpinBoxChanged(int)));
    connect(queryHmmFileToolButton,         SIGNAL(clicked()),          SLOT(sl_queryHmmFileToolButtonClicked()));
    connect(domZCheckBox,                   SIGNAL(stateChanged(int)),  SLOT(sl_domZCheckBoxChanged(int)));
}

void HmmerSearchDialog::setModelValues() {
    domESpinBox->setValue(1);
    scoreTresholdDoubleSpin->setValue(0); // because default is OPTION_NOT_SET
    domZDoubleSpinBox->setValue(0); // because default is OPTION_NOT_SET
    nobiasCheckBox->setChecked(model.searchSettings.noBiasFilter);
    nonull2CheckBox->setChecked(model.searchSettings.noNull2);
    maxCheckBox->setChecked(model.searchSettings.doMax);
    f1DoubleSpinBox->setValue(model.searchSettings.f1);
    f2DoubleSpinBox->setValue(model.searchSettings.f2);
    f3DoubleSpinBox->setValue(model.searchSettings.f3);
    seedSpinBox->setValue(model.searchSettings.seed);
}

void HmmerSearchDialog::getModelValues() {
    if (useEvalTresholdsButton->isChecked()) {
        model.searchSettings.domE = pow(10.0, domESpinBox->value());
        model.searchSettings.domT = HmmerSearchSettings::OPTION_NOT_SET;
    } else if (useScoreTresholdsButton->isChecked()) {
        model.searchSettings.domE = HmmerSearchSettings::OPTION_NOT_SET;
        if (useExplicitScoreTresholdButton->isChecked()) {
            model.searchSettings.domT = scoreTresholdDoubleSpin->value();
        } else if (useGATresholdsButton->isChecked()) {
            model.searchSettings.useBitCutoffs = HmmerSearchSettings::p7H_GA;
        } else if (useNCTresholdsButton->isChecked()) {
            model.searchSettings.useBitCutoffs = HmmerSearchSettings::p7H_NC;
        } else if(useTCTresholdsButton->isChecked()) {
            model.searchSettings.useBitCutoffs = HmmerSearchSettings::p7H_TC;
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }

    if (domZCheckBox->isChecked()) {
        model.searchSettings.domZ = domZDoubleSpinBox->value();
    } else {
        model.searchSettings.domZ = HmmerSearchSettings::OPTION_NOT_SET;
    }

    model.searchSettings.noBiasFilter = nobiasCheckBox->isChecked();
    model.searchSettings.noNull2 = nonull2CheckBox->isChecked();
    model.searchSettings.doMax = maxCheckBox->isChecked();

    model.searchSettings.f1 = f1DoubleSpinBox->value();
    model.searchSettings.f2 = f2DoubleSpinBox->value();
    model.searchSettings.f3 = f3DoubleSpinBox->value();

    model.searchSettings.seed = seedSpinBox->value();

    const CreateAnnotationModel &annModel = annotationsWidgetController->getModel();
    model.searchSettings.pattern = annotationsWidgetController->getAnnotationPattern();
    model.searchSettings.hmmProfileUrl = queryHmmFileEdit->text();
    model.searchSettings.sequence = model.sequence;
    model.searchSettings.annotationTable = annModel.getAnnotationObject();
}

QString HmmerSearchDialog::checkModel() {
    QString ret;

    if (model.searchSettings.hmmProfileUrl.isEmpty()) {
        ret = tr("HMM profile is not set");
        queryHmmFileEdit->setFocus();
        return ret;
    }

    if (!model.searchSettings.validate()) {
        ret = tr("Settings are invalid");
        return ret;
    }

    ret = annotationsWidgetController->validate();
    return ret;
}

void HmmerSearchDialog::sl_okButtonClicked() {
    bool objectPrepared = annotationsWidgetController->prepareAnnotationObject();
    if (!objectPrepared) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }

    SAFE_POINT(!model.sequence.isNull(), L10N::nullPointerError("sequence object"), );
    getModelValues();
    QString err = checkModel();
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Error: bad arguments!"), err);
        return;
    }
    if(seqCtx != NULL){
        seqCtx->getAnnotatedDNAView()->tryAddObject(annotationsWidgetController->getModel().getAnnotationObject());
    }

    HmmerSearchTask *searchTask = new HmmerSearchTask(model.searchSettings);
    AppContext::getTaskScheduler()->registerTopLevelTask(searchTask);

    QDialog::accept();
}

void HmmerSearchDialog::sl_useEvalTresholdsButtonChanged(bool checked) {
    domESpinBox->setEnabled(checked);
}

void HmmerSearchDialog::sl_useScoreTresholdsButtonChanged(bool checked) {
    useExplicitScoreTresholdButton->setEnabled(checked);
    useGATresholdsButton->setEnabled(checked);
    useNCTresholdsButton->setEnabled(checked);
    useTCTresholdsButton->setEnabled(checked);
    if (!checked) {
        scoreTresholdDoubleSpin->setEnabled(false);
    } else {
        scoreTresholdDoubleSpin->setEnabled(useExplicitScoreTresholdButton->isChecked());
    }
}

void HmmerSearchDialog::sl_useExplicitScoreTresholdButton(bool checked) {
    scoreTresholdDoubleSpin->setEnabled(checked);
}

void HmmerSearchDialog::sl_maxCheckBoxChanged(int state) {
    assert(Qt::PartiallyChecked != state);
    bool unchecked = Qt::Unchecked == state;
    f1Label->setEnabled(unchecked);
    f2Label->setEnabled(unchecked);
    f3Label->setEnabled(unchecked);
    f1DoubleSpinBox->setEnabled(unchecked);
    f2DoubleSpinBox->setEnabled(unchecked);
    f3DoubleSpinBox->setEnabled(unchecked);
}

void HmmerSearchDialog::sl_domESpinBoxChanged(int newVal) {
    const QString &prefix = (0 <= newVal ? DOM_E_PLUS_PREFIX : DOM_E_MINUS_PREFIX);
    domESpinBox->setPrefix(prefix);
}

void HmmerSearchDialog::sl_queryHmmFileToolButtonClicked() {
    LastUsedDirHelper helper(HMM_FILES_DIR_ID);
    const QString fileFilter = DialogUtils::prepareFileFilter(tr("HMM profile"), QStringList() << "hmm", true, QStringList());

    helper.url = U2FileDialog::getOpenFileName(this, tr("Select query HMM profile"), helper, fileFilter);
    if (!helper.url.isEmpty()) {
        queryHmmFileEdit->setText(helper.url);
    }
}

void HmmerSearchDialog::sl_domZCheckBoxChanged(int state) {
    assert(Qt::PartiallyChecked != state);
    bool checked = Qt::Checked == state;
    domZDoubleSpinBox->setEnabled(checked);
}

}   // namespace U2
