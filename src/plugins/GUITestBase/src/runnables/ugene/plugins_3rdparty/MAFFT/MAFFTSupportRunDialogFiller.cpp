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

MAFFTSupportRunDialogFiller::Parameters::Parameters():
    ckeckBox_gapOpenCheckBox_checked(0),
    doubleSpin_gapOpenSpinBox_value(1.53),
    ckeckBox_gapExtCheckBox_checked(0),
    doubleSpin_gapExtSpinBox_value(0),
    ckeckBox_maxNumberIterRefinementCheckBox_checked(0),
    spin_maxNumberIterRefinementSpinBox_value(0)
{

}

#define GT_CLASS_NAME "GTUtilsDialog::MAFFTSupportRunDialogFiller"

MAFFTSupportRunDialogFiller::MAFFTSupportRunDialogFiller(GUITestOpStatus &os, MAFFTSupportRunDialogFiller::Parameters *parameters)
    : Filler(os, "MAFFTSupportRunDialog"),
      parameters(parameters)
{
    CHECK_SET_ERR(parameters, "Invalid filler parameters: NULL pointer");
}

MAFFTSupportRunDialogFiller::MAFFTSupportRunDialogFiller(GUITestOpStatus &os, CustomScenario *scenario)
    : Filler(os, "MAFFTSupportRunDialog", scenario),
      parameters(NULL)
{

}

#define GT_METHOD_NAME "commonScenario"
void MAFFTSupportRunDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");
    
    QCheckBox* gapOpenCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "gapOpenCheckBox", dialog));
    GT_CHECK(gapOpenCheckBox, "gapOpenCheckBox is NULL");
    GTCheckBox::setChecked(os, gapOpenCheckBox, parameters->ckeckBox_gapOpenCheckBox_checked);

    QDoubleSpinBox* gapOpenSpinBox = qobject_cast<QDoubleSpinBox*>(GTWidget::findWidget(os, "gapOpenSpinBox", dialog));
    GT_CHECK(gapOpenSpinBox, "gapOpenSpinBox is NULL");
    GTDoubleSpinbox::setValue(os, gapOpenSpinBox, parameters->doubleSpin_gapOpenSpinBox_value);

    QCheckBox* gapExtCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "gapExtCheckBox", dialog));
    GT_CHECK(gapExtCheckBox, "gapExtCheckBox is NULL");
    GTCheckBox::setChecked(os, gapExtCheckBox, parameters->ckeckBox_gapExtCheckBox_checked);

    QDoubleSpinBox* gapExtSpinBox = qobject_cast<QDoubleSpinBox*>(GTWidget::findWidget(os, "gapExtSpinBox", dialog));
    GT_CHECK(gapExtSpinBox, "gapExtSpinBox is NULL");
    GTDoubleSpinbox::setValue(os, gapExtSpinBox, parameters->doubleSpin_gapExtSpinBox_value);

    QCheckBox* maxNumberIterRefinementCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "maxNumberIterRefinementCheckBox", dialog));
    GT_CHECK(maxNumberIterRefinementCheckBox, "maxNumberIterRefinementCheckBox is NULL");
    GTCheckBox::setChecked(os, maxNumberIterRefinementCheckBox, parameters->ckeckBox_maxNumberIterRefinementCheckBox_checked);

    QSpinBox* maxNumberIterRefinementSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "maxNumberIterRefinementSpinBox", dialog));
    GT_CHECK(maxNumberIterRefinementSpinBox, "maxNumberIterRefinementSpinBox is NULL");
    GTSpinBox::setValue(os, maxNumberIterRefinementSpinBox, parameters->spin_maxNumberIterRefinementSpinBox_value);

    GTWidget::click(os, GTWidget::findButtonByText(os, "Align", dialog));
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}   // namespace U2
