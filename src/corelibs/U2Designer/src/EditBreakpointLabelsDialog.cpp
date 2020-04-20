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

#include <QStringList>
#include <QCheckBox>
#include <QKeyEvent>

#include "ui_EditBreakpointLabelsDialog.h"
#include "EditBreakpointLabelsDialog.h"
#include <U2Gui/HelpButton.h>

const QString LABEL_LEFT_SIDE_OFFSET = "    ";

namespace U2 {

EditBreakpointLabelsDialog::EditBreakpointLabelsDialog(const QStringList &existingLabels,
    const QStringList &initCallingBreakpointLabels, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      applienceControlsForLabels(),
      callingBreakpointLabels(initCallingBreakpointLabels),
      newLabelsAdded(),
      ui(new Ui_EditBreakpointLabelsDialog())
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "28967093");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->addLabelButton->setEnabled(false);
    initExistingLabelsList(existingLabels);

    connect(ui->newLabelEdit, SIGNAL(textChanged(const QString &)),
        SLOT(sl_newLabelEditChanged(const QString &)));
    connect(ui->addLabelButton, SIGNAL(clicked()), SLOT(sl_newLabelAdded()));

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_dialogAccepted()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

}

EditBreakpointLabelsDialog::~EditBreakpointLabelsDialog() {
    delete ui;
}

void EditBreakpointLabelsDialog::initExistingLabelsList(const QStringList &existingLabels) {
    foreach(QString label, existingLabels) {
        addNewLabelToList(label, callingBreakpointLabels.contains(label));
    }
}

void EditBreakpointLabelsDialog::sl_labelApplianceStateChanged(int state) {
    QObject *activator = sender();
    QCheckBox *labelApplianceController = qobject_cast<QCheckBox *>(activator);
    Q_ASSERT(NULL != labelApplianceController);

    switch(state) {
    case Qt::Checked:
        callingBreakpointLabels.append(applienceControlsForLabels[labelApplianceController]);
        break;
    case Qt::Unchecked:
        callingBreakpointLabels.removeAll(applienceControlsForLabels[labelApplianceController]);
        break;
    default:
        Q_ASSERT(false);
    }
}

void EditBreakpointLabelsDialog::sl_newLabelEditChanged(const QString &text) {
    ui->addLabelButton->setEnabled(!text.isEmpty());
}

void EditBreakpointLabelsDialog::sl_newLabelAdded() {
    const QString newLabel = ui->newLabelEdit->text();
    Q_ASSERT(!newLabel.isEmpty());
    if(ui->labelList->findItems(LABEL_LEFT_SIDE_OFFSET + newLabel, Qt::MatchExactly).isEmpty()) {
        addNewLabelToList(newLabel, true);
        callingBreakpointLabels.append(newLabel);
        newLabelsAdded.append(newLabel);
    }
    ui->newLabelEdit->setText(QString());
}

void EditBreakpointLabelsDialog::addNewLabelToList(const QString &newLabel,
    bool appliedToCallingBreakpoint)
{
    QListWidgetItem *itemWithCurrentLabel = new QListWidgetItem(LABEL_LEFT_SIDE_OFFSET + newLabel,
        ui->labelList);

    QCheckBox *labelApplianceController = new QCheckBox(ui->labelList);
    labelApplianceController->setChecked(appliedToCallingBreakpoint);
    connect(labelApplianceController, SIGNAL(stateChanged(int)),
        SLOT(sl_labelApplianceStateChanged(int)));
    applienceControlsForLabels[labelApplianceController] = newLabel;

    ui->labelList->setItemWidget(itemWithCurrentLabel, labelApplianceController);
}

void EditBreakpointLabelsDialog::sl_dialogAccepted() {
    if(!newLabelsAdded.isEmpty()) {
        emit si_labelsCreated(newLabelsAdded);
    }
    emit si_labelAddedToCallingBreakpoint(callingBreakpointLabels);
}

void EditBreakpointLabelsDialog::keyPressEvent(QKeyEvent *event) {
    if((Qt::Key_Enter == event->key() || Qt::Key_Return == event->key())
        && ui->addLabelButton->isEnabled()) {
        sl_newLabelAdded();
    }
    QDialog::keyPressEvent(event);
}

} // namespace U2
