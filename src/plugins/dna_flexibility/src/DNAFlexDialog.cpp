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

#include "DNAFlexDialog.h"
#include "DNAFlexTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <QPushButton>
#include <QMessageBox>

namespace U2 {

DNAFlexDialog::DNAFlexDialog(ADVSequenceObjectContext* _ctx)
  : QDialog(_ctx->getAnnotatedDNAView()->getWidget())
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748939");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ctx = _ctx;

    // Get the sequence length
    int sequenceLength = ctx->getSequenceObject()->getSequenceLength();

    // Creating and initializing the annotation model
    CreateAnnotationModel annotModel;
    annotModel.hideAnnotationType = true;
    annotModel.hideLocation = true;  // hides location field and does not check it in validate()
    annotModel.data->name = "dna_flex";
    annotModel.sequenceObjectRef = ctx->getSequenceObject();
    annotModel.sequenceLen = sequenceLength;

    // Initializing and adding the annotations widget
    annotController = new CreateAnnotationWidgetController(annotModel, this);
    QWidget* annotWidget = annotController->getWidget();
    tabOutput->layout()->addWidget(annotWidget);

    // Setting the dialog icon to the standard UGENE icon
    setWindowIcon(QIcon(":/ugene/images/ugene_16.png"));

    // Setting the bounds for the parameters
    spinBoxWindowSize->setMaximum(sequenceLength);
    spinBoxWindowStep->setMaximum(sequenceLength - 2); // Approximate value. In real life the step should be 1 or small enough.

    // "-2" is added as the first window should be at least 2 nucleotides.
    if(settings.windowSize >  sequenceLength){
        settings.windowSize = sequenceLength;
    }

    // Setting the values
    updateHighFlexValues();

    // Connecting the GUI settings
    connect(spinBoxWindowSize, SIGNAL(valueChanged(int)), SLOT(sl_spinWindowSizeChanged(int)));
    connect(spinBoxWindowStep, SIGNAL(valueChanged(int)), SLOT(sl_spinWindowStepChanged(int)));
    connect(doubleSpinBoxThreshold, SIGNAL(valueChanged(double)), SLOT(sl_spinThresholdChanged(double)));
    connect(btnRemember, SIGNAL(clicked()), SLOT(sl_rememberSettings()));
    connect(btnDefaults, SIGNAL(clicked()), SLOT(sl_defaultSettings()));
    connect(tabWidget,SIGNAL(currentChanged(int)),this, SLOT(sl_updateSizes(int)));

    sl_updateSizes(0);
}

void DNAFlexDialog::accept()
{
    // Verifying and passing the settings
    // TODO

    // Preparing the annotations object and other annotations parameters
    QString err = annotController->validate();
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), err);
        return;
    }
    bool objectPrepared = annotController->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    const CreateAnnotationModel& annotModel = annotController->getModel();
    QString annotName = annotModel.data->name;
    QString annotGroup = annotModel.groupName;

    if(ctx != NULL){
        ctx->getAnnotatedDNAView()->tryAddObject(annotModel.getAnnotationObject());
    }

    // Creating the task
    U2OpStatusImpl os;
    QByteArray seqData = ctx->getSequenceObject()->getWholeSequenceData(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
    DNAFlexTask* task = new DNAFlexTask(
        settings,
        annotModel.getAnnotationObject(),
        annotName,
        annotGroup,
        annotModel.description,
        seqData);

    // Registering the task
    AppContext::getTaskScheduler()->registerTopLevelTask(task);

    // Accepting the dialog
    QDialog::accept();
}

void DNAFlexDialog::sl_spinWindowSizeChanged(int newValue)
{
    settings.windowSize = newValue;
}

void DNAFlexDialog::sl_spinWindowStepChanged(int newValue)
{
    settings.windowStep = newValue;
}

void DNAFlexDialog::sl_spinThresholdChanged(double newValue)
{
    settings.threshold = newValue;
}

void DNAFlexDialog::sl_rememberSettings()
{
    settings.rememberSettings();
}

void DNAFlexDialog::sl_defaultSettings()
{
    settings.restoreDefaults();
    updateHighFlexValues();
}

void DNAFlexDialog::updateHighFlexValues()
{
    spinBoxWindowSize->setValue(settings.windowSize);
    spinBoxWindowStep->setValue(settings.windowStep);
    doubleSpinBoxThreshold->setValue(settings.threshold);
}

void DNAFlexDialog::sl_updateSizes(int index) {
    Q_UNUSED(index);

    for (int i=0; i < tabWidget->count(); i++) {
        tabWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    QWidget *widget = tabWidget->currentWidget();
    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    widget->resize(widget->minimumSizeHint());
    widget->adjustSize();
    tabWidget->resize(tabWidget->minimumSizeHint());
    tabWidget->adjustSize();
}

} // namespace

