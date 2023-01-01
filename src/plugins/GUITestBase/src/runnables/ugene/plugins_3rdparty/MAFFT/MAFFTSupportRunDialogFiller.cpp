/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>

#include "MAFFTSupportRunDialogFiller.h"

namespace U2 {

MAFFTSupportRunDialogFiller::Parameters::Parameters()
    : checkBox_gapOpenCheckBox_checked(0),
      doubleSpin_gapOpenSpinBox_value(1.53),
      checkBox_gapExtCheckBox_checked(0),
      doubleSpin_gapExtSpinBox_value(0),
      checkBox_maxNumberIterRefinementCheckBox_checked(0),
      spin_maxNumberIterRefinementSpinBox_value(0) {
}

#define GT_CLASS_NAME "GTUtilsDialog::MAFFTSupportRunDialogFiller"

MAFFTSupportRunDialogFiller::MAFFTSupportRunDialogFiller(GUITestOpStatus& os, MAFFTSupportRunDialogFiller::Parameters* parameters)
    : Filler(os, "MAFFTSupportRunDialog"),
      parameters(parameters) {
    CHECK_SET_ERR(parameters, "Invalid filler parameters: NULL pointer");
}

MAFFTSupportRunDialogFiller::MAFFTSupportRunDialogFiller(GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "MAFFTSupportRunDialog", scenario),
      parameters(nullptr) {
}

#define GT_METHOD_NAME "commonScenario"
void MAFFTSupportRunDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    auto gapOpenCheckBox = GTWidget::findCheckBox(os, "gapOpenCheckBox", dialog);
    GTCheckBox::setChecked(os, gapOpenCheckBox, parameters->checkBox_gapOpenCheckBox_checked);

    auto gapOpenSpinBox = GTWidget::findDoubleSpinBox(os, "gapOpenSpinBox", dialog);
    GTDoubleSpinbox::setValue(os, gapOpenSpinBox, parameters->doubleSpin_gapOpenSpinBox_value);

    auto gapExtCheckBox = GTWidget::findCheckBox(os, "gapExtCheckBox", dialog);
    GTCheckBox::setChecked(os, gapExtCheckBox, parameters->checkBox_gapExtCheckBox_checked);

    auto gapExtSpinBox = GTWidget::findDoubleSpinBox(os, "gapExtSpinBox", dialog);
    GTDoubleSpinbox::setValue(os, gapExtSpinBox, parameters->doubleSpin_gapExtSpinBox_value);

    auto maxNumberIterRefinementCheckBox = GTWidget::findCheckBox(os, "maxNumberIterRefinementCheckBox", dialog);
    GTCheckBox::setChecked(os, maxNumberIterRefinementCheckBox, parameters->checkBox_maxNumberIterRefinementCheckBox_checked);

    auto maxNumberIterRefinementSpinBox = GTWidget::findSpinBox(os, "maxNumberIterRefinementSpinBox", dialog);
    GTSpinBox::setValue(os, maxNumberIterRefinementSpinBox, parameters->spin_maxNumberIterRefinementSpinBox_value);

    GTWidget::click(os, GTWidget::findButtonByText(os, "Align", dialog));
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
