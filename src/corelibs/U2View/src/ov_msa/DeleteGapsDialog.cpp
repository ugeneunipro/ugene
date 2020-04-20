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

#include "DeleteGapsDialog.h"
#include "ui_DeleteGapsDialog.h"

#include <U2Gui/HelpButton.h>

#include <QPushButton>

namespace U2 {

DeleteGapsDialog::DeleteGapsDialog(QWidget* parent, int rowNum): QDialog(parent), ui(new Ui_DeleteGapsDialog()) {
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748851");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Remove"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->allRadioButton->setChecked(true);
    ui->absoluteSpinBox->setMinimum(1);
    ui->absoluteSpinBox->setMaximum(rowNum);

    QPushButton *deleteButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    connect(ui->absoluteRadioButton, SIGNAL(clicked()), SLOT(sl_onRadioButtonClicked()));
    connect(ui->relativeRadioButton, SIGNAL(clicked()), SLOT(sl_onRadioButtonClicked()));
    connect(ui->allRadioButton, SIGNAL(clicked()), SLOT(sl_onRadioButtonClicked()));
    connect(deleteButton, SIGNAL(clicked()), SLOT(sl_onOkClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_onCancelClicked()));

    sl_onRadioButtonClicked();

}
DeleteGapsDialog::~DeleteGapsDialog(){
    delete ui;
}

void DeleteGapsDialog::sl_onRadioButtonClicked() {
    ui->absoluteSpinBox->setEnabled(ui->absoluteRadioButton->isChecked());
    ui->relativeSpinBox->setEnabled(ui->relativeRadioButton->isChecked());

    if (ui->absoluteRadioButton->isChecked()) {
        ui->absoluteSpinBox->setFocus();
    }
    if (ui->relativeRadioButton->isChecked()) {
        ui->relativeSpinBox->setFocus();
    }
}

void DeleteGapsDialog::sl_onOkClicked() {
    deleteMode = ui->allRadioButton->isChecked() ? DeleteAll : (ui->relativeRadioButton->isChecked() ? DeleteByRelativeVal : DeleteByAbsoluteVal);

    switch(deleteMode) {
        case DeleteByAbsoluteVal: value = ui->absoluteSpinBox->value();
            break;
        case DeleteByRelativeVal: value = ui->relativeSpinBox->value();
            break;
        default: value = 0;
    }

    accept();
}

void DeleteGapsDialog::sl_onCancelClicked() {
    reject();
}

}
