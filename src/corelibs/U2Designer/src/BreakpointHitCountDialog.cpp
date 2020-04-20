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

#include <limits>

#include <QIntValidator>
#include <QMessageBox>
#include <QPushButton>

#include <U2Gui/HelpButton.h>

#include "BreakpointHitCountDialog.h"
#include "ui_BreakpointHitCountDialog.h"

const int LOWER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER = 1;
const int UPPER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER = std::numeric_limits<int>::max();
const int HIT_COUNT_AFTER_RESET = 0;

const char *WRONG_INPUT_MESSAGE_BOX_TITLE = "Incorrect input";
const char *WRONG_INPUT_MESSAGE_BOX_CONTENT = "The specified hit count target is not valid";

namespace U2 {

BreakpointHitCountDialog::BreakpointHitCountDialog(const QStringList &hitCountConditions,
    const QString &conditionOnLaunch, quint32 hitCountParameterOnLaunch, quint32 hitCountOnLaunch,
    const QStringList &hitCountersListWithoutParameter, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      initialCondition(conditionOnLaunch),
      initialParameter(hitCountParameterOnLaunch),
      hitCountersConditionsWithoutParameter(hitCountersListWithoutParameter),
      chosenCondition(conditionOnLaunch),
      hitCounterParameter(LOWER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER),
      isHitCounterReset(false)
{
    ui = new Ui_BreakpointHitCountDialog();
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "28967093");

    ui->hitConditionCombo->addItems(hitCountConditions);
    ui->hitConditionCombo->setCurrentIndex(hitCountConditions.indexOf(conditionOnLaunch));
    sl_hitConditionChanged(conditionOnLaunch);
    ui->hitParameterEdit->setValidator(new QIntValidator(LOWER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER,
        UPPER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER, this));
    ui->currentHitCountValueLabel->setText(QString::number(hitCountOnLaunch));
    ui->hitParameterEdit->setText(QString::number(hitCountParameterOnLaunch));

    QPushButton *resetButton = ui->buttonBox->button(QDialogButtonBox::Reset);
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    resetButton->setText(tr("Reset"));
    okButton->setText(tr("OK"));
    cancelButton->setText(tr("Cancel"));

    connect(resetButton, SIGNAL(clicked()), this, SLOT(sl_resetHitCount()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(sl_dialogAccepted()));
    connect(ui->hitConditionCombo, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_hitConditionChanged(const QString &)));

}

BreakpointHitCountDialog::~BreakpointHitCountDialog() {
    delete ui;
}

void BreakpointHitCountDialog::sl_dialogAccepted() {
    bool conversionResult = true;
    hitCounterParameter = ui->hitParameterEdit->text().toInt(&conversionResult);
    if ( ( !conversionResult
        && !hitCountersConditionsWithoutParameter.contains(
        ui->hitConditionCombo->currentText( ) ) )
        || 1 > hitCounterParameter )
    {
        QMessageBox::critical(this, tr(WRONG_INPUT_MESSAGE_BOX_TITLE),
            tr(WRONG_INPUT_MESSAGE_BOX_CONTENT));
        return;
    }

    if (isHitCounterReset) {
        emit si_resetHitCount();
    }

    if (initialParameter != hitCounterParameter || initialCondition != chosenCondition) {
        emit si_hitCounterAssigned(chosenCondition, hitCounterParameter);
    }
    accept();
}

void BreakpointHitCountDialog::sl_resetHitCount() {
    isHitCounterReset = true;
    ui->currentHitCountValueLabel->setText(QString::number(HIT_COUNT_AFTER_RESET));
}

void BreakpointHitCountDialog::sl_hitConditionChanged(const QString &text) {
    chosenCondition = text;
    if(hitCountersConditionsWithoutParameter.contains(ui->hitConditionCombo->currentText())) {
        ui->hitParameterEdit->hide();
    } else if(!ui->hitParameterEdit->isVisible()) {
        ui->hitParameterEdit->show();
    }
}

} // namespace U2
