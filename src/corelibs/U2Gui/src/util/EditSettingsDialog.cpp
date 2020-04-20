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

#include <QPushButton>

#include "EditSettingsDialog.h"

#include "ui_EditSettingsDialog.h"

#include <U2Core/U2SafePoints.h>
#include <U2Gui/HelpButton.h>



namespace U2 {

EditSettingsDialog::EditSettingsDialog(const EditSettings& settings, QWidget* parent)
    : QDialog(parent) {
    ui = new Ui_EditSettingDialogForm;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748769");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->recalculateQuals->setChecked(settings.recalculateQualifiers);

    switch (settings.annotationStrategy) {
    case U1AnnotationUtils::AnnotationStrategyForResize_Resize:
        ui->resizeRadioButton->setChecked(true);
        break;
    case U1AnnotationUtils::AnnotationStrategyForResize_Remove:
        ui->removeRadioButton->setChecked(true);
        break;
    case U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Joined:
        ui->splitRadioButton->setChecked(true);
        break;
    case U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate:
        ui->split_separateRadioButton->setChecked(true);
        break;
    default:
        FAIL("Unexpected enum value", );
    }
}

EditSettingsDialog::~EditSettingsDialog() {
    delete ui;
}

EditSettings EditSettingsDialog::getSettings() const {
    EditSettings s;
    s.recalculateQualifiers = ui->recalculateQuals->isChecked();

    if (ui->resizeRadioButton->isChecked()) {
        s.annotationStrategy = U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    }
    if (ui->removeRadioButton->isChecked()) {
        s.annotationStrategy = U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }
    if (ui->splitRadioButton->isChecked()) {
        s.annotationStrategy = U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Joined;
    }
    if (ui->split_separateRadioButton->isChecked()) {
        s.annotationStrategy = U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate;
    }

    return s;
}

} // namespace
