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

#include <math.h>

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
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

#include "PhmmerSearchTask.h"
#include "PhmmerSearchDialog.h"

namespace U2 {

const QString PhmmerSearchDialog::QUERY_FILES_DIR            = "uhmm3_phmmer_query_files_dir";
const QString PhmmerSearchDialog::DOM_E_PLUS_PREFIX          = "1E+";
const QString PhmmerSearchDialog::DOM_E_MINUS_PREFIX         = "1E";
const QString PhmmerSearchDialog::ANNOTATIONS_DEFAULT_NAME   = "signal";

PhmmerSearchDialog::PhmmerSearchDialog(U2SequenceObject *seqObj, QWidget *parent)
    : QDialog(parent), seqCtx(NULL)
{
    init(seqObj);
}

PhmmerSearchDialog::PhmmerSearchDialog(ADVSequenceObjectContext *seqCtx, QWidget *parent)
    : QDialog(parent), seqCtx(seqCtx)
{
    init(seqCtx->getSequenceObject());
}

void PhmmerSearchDialog::init(U2SequenceObject *seqObj){
    assert(NULL != seqObj);
    setupUi(this);

    new HelpButton(this, buttonBox, "24748986");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    U2OpStatusImpl os;
    model.dbSequence = seqObj;
    SAFE_POINT_EXT(!os.hasError(), QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );
    setModelValues(); // default model here

    // Annotations widget
    CreateAnnotationModel annModel;
    annModel.hideLocation = true;
    annModel.sequenceObjectRef = seqObj;
    annModel.useAminoAnnotationTypes = seqObj->getAlphabet()->isAmino();
    annModel.data->type = U2FeatureTypes::MiscSignal;
    annModel.data->name = ANNOTATIONS_DEFAULT_NAME;
    annModel.sequenceLen = seqObj->getSequenceLength();
    annotationsWidgetController = new CreateAnnotationWidgetController(annModel, this);

    QWidget *firstTab = mainTabWidget->widget(0);
    assert(NULL != firstTab);
    QVBoxLayout *curLayout = qobject_cast<QVBoxLayout *>(firstTab->layout());
    assert(NULL != curLayout);
    curLayout->insertWidget(ANNOTATIONS_WIDGET_LOCATION, annotationsWidgetController->getWidget());

    connect(queryToolButton, SIGNAL(clicked()), SLOT(sl_queryToolButtonClicked()));
    connect(useEvalTresholdsButton, SIGNAL(toggled(bool)), SLOT(sl_useEvalTresholdsButtonChanged(bool)));
    connect(useScoreTresholdsButton, SIGNAL(toggled(bool)), SLOT(sl_useScoreTresholdsButtonChanged(bool)));
    connect(domZCheckBox, SIGNAL(stateChanged(int)), SLOT(sl_domZCheckBoxChanged(int)));
    connect(maxCheckBox, SIGNAL(stateChanged(int)), SLOT(sl_maxCheckBoxChanged(int)));
    connect(domESpinBox, SIGNAL(valueChanged(int)), SLOT(sl_domESpinBoxChanged(int)));

    adjustSize();
}

void PhmmerSearchDialog::setModelValues() {
    domESpinBox->setValue(1);
    assert(10.0 == model.phmmerSettings.domE);
    scoreTresholdDoubleSpin->setValue(model.phmmerSettings.domT);
    f1DoubleSpinBox->setValue(model.phmmerSettings.f1);
    f2DoubleSpinBox->setValue(model.phmmerSettings.f2);
    f3DoubleSpinBox->setValue(model.phmmerSettings.f3);
    seedSpinBox->setValue(model.phmmerSettings.seed);
    emlSpinBox->setValue(model.phmmerSettings.eml);
    emnSpinBox->setValue(model.phmmerSettings.emn);
    evlSpinBox->setValue(model.phmmerSettings.evl);
    evnSpinBox->setValue(model.phmmerSettings.evn);
    eflSpinBox->setValue(model.phmmerSettings.efl);
    efnSpinBox->setValue(model.phmmerSettings.efn);
    eftDoubleSpinBox->setValue(model.phmmerSettings.eft);
    popenDoubleSpinBox->setValue(model.phmmerSettings.popen);
    pextendDoubleSpinBox->setValue(model.phmmerSettings.pextend);
}

void PhmmerSearchDialog::sl_queryToolButtonClicked() {
    LastUsedDirHelper helper(QUERY_FILES_DIR);
    helper.url = U2FileDialog::getOpenFileName(this, tr("Select query sequence file"),
        helper, DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true));
    if (!helper.url.isEmpty()) {
        queryLineEdit->setText(helper.url);
    }
}

void PhmmerSearchDialog::getModelValues() {
    if (useEvalTresholdsButton->isChecked()) {
        model.phmmerSettings.domE = pow(10.0, domESpinBox->value());
        model.phmmerSettings.domT = PhmmerSearchSettings::OPTION_NOT_SET;
    } else if (useScoreTresholdsButton->isChecked()) {
        model.phmmerSettings.domT = scoreTresholdDoubleSpin->value();
    } else {
        assert(false);
    }

    model.phmmerSettings.popen = popenDoubleSpinBox->value();
    model.phmmerSettings.pextend = pextendDoubleSpinBox->value();

    model.phmmerSettings.noBiasFilter = nobiasCheckBox->isChecked();
    model.phmmerSettings.noNull2 = nonull2CheckBox->isChecked();
    model.phmmerSettings.doMax = maxCheckBox->isChecked();
    model.phmmerSettings.f1 = f1DoubleSpinBox->value();
    model.phmmerSettings.f2 = f2DoubleSpinBox->value();
    model.phmmerSettings.f3 = f3DoubleSpinBox->value();

    model.phmmerSettings.eml = emlSpinBox->value();
    model.phmmerSettings.emn = emnSpinBox->value();
    model.phmmerSettings.evl = evlSpinBox->value();
    model.phmmerSettings.evn = evnSpinBox->value();
    model.phmmerSettings.efl = eflSpinBox->value();
    model.phmmerSettings.efn = efnSpinBox->value();
    model.phmmerSettings.eft = eftDoubleSpinBox->value();
    model.phmmerSettings.seed = seedSpinBox->value();

    const CreateAnnotationModel &annModel = annotationsWidgetController->getModel();
    model.phmmerSettings.pattern = annotationsWidgetController->getAnnotationPattern();
    model.phmmerSettings.annotationTable = annModel.getAnnotationObject();
    model.phmmerSettings.querySequenceUrl = queryLineEdit->text();
    model.phmmerSettings.targetSequence = model.dbSequence;
    model.phmmerSettings.pattern.groupName = annModel.groupName;
}

QString PhmmerSearchDialog::checkModel() {
    QString ret;

    if (model.phmmerSettings.querySequenceUrl.isEmpty()) {
        ret = tr("Query sequence file path is empty");
        queryLineEdit->setFocus();
        return ret;
    }

    ret = annotationsWidgetController->validate();
    if (!ret.isEmpty()) {
        return ret;
    }

    assert(model.phmmerSettings.validate());

    return ret;
}

void PhmmerSearchDialog::accept() {
    bool objectPrepared = annotationsWidgetController->prepareAnnotationObject();
    if (!objectPrepared) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }

    getModelValues();
    QString err = checkModel();
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Error: bad arguments!"), err);
        return;
    }
    if(seqCtx != NULL){
        seqCtx->getAnnotatedDNAView()->tryAddObject(annotationsWidgetController->getModel().getAnnotationObject());
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(new PhmmerSearchTask(model.phmmerSettings));

    QDialog::accept();
}

void PhmmerSearchDialog::sl_useEvalTresholdsButtonChanged(bool checked) {
    domESpinBox->setEnabled(checked);
}

void PhmmerSearchDialog::sl_useScoreTresholdsButtonChanged(bool checked) {
    scoreTresholdDoubleSpin->setEnabled(checked);
}

void PhmmerSearchDialog::sl_domZCheckBoxChanged(int state) {
    assert(Qt::PartiallyChecked != state);
    bool checked = Qt::Checked == state;
    domZDoubleSpinBox->setEnabled(checked);
}

void PhmmerSearchDialog::sl_maxCheckBoxChanged(int state) {
    assert(Qt::PartiallyChecked != state);
    bool unchecked = Qt::Unchecked == state;
    f1Label->setEnabled(unchecked);
    f2Label->setEnabled(unchecked);
    f3Label->setEnabled(unchecked);
    f1DoubleSpinBox->setEnabled(unchecked);
    f2DoubleSpinBox->setEnabled(unchecked);
    f3DoubleSpinBox->setEnabled(unchecked);
}

void PhmmerSearchDialog::sl_domESpinBoxChanged(int newVal) {
    const QString & prefix = (0 <= newVal ? DOM_E_PLUS_PREFIX : DOM_E_MINUS_PREFIX);
    domESpinBox->setPrefix(prefix);
}

}   // namespace U2
