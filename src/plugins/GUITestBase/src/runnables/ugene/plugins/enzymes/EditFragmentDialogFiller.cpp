/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "EditFragmentDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QGroupBox>

namespace U2 {

EditFragmentDialogFiller::EditFragmentDialogFiller(HI::GUITestOpStatus& os, const Parameters& parameters)
    : Filler(os, "EditFragmentDialog"), parameters(parameters) {
}

#define GT_CLASS_NAME "GTUtilsDialog::EditFragmentDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void EditFragmentDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    // GUITest_regression_scenarios_test_0574
    if (parameters.checkRComplText) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "rStickyButton", dialog));
        auto rCustomOverhangBox = GTWidget::findGroupBox(os, "rCustomOverhangBox", dialog);
        GTGroupBox::setChecked(os, rCustomOverhangBox, true);
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "rComplRadioButton", dialog));
        GT_CHECK(GTLineEdit::getText(os, "rComplOverhangEdit", dialog) == parameters.rComplText, "Wrong rComplTextEdit text");
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (parameters.lSticky) {
        auto lStickyButton = GTWidget::findRadioButton(os, "lStickyButton", dialog);
        GTRadioButton::click(os, lStickyButton);

        auto lCustomOverhangBox = GTWidget::findGroupBox(os, "lCustomOverhangBox", dialog);
        GTGroupBox::setChecked(os, lCustomOverhangBox, parameters.lCustom);

        if (parameters.lCustom) {
            if (parameters.lDirect) {
                auto lDirectRadioButton = GTWidget::findRadioButton(os, "lDirectRadioButton", dialog);
                GTRadioButton::click(os, lDirectRadioButton);

                GTLineEdit::setText(os, "lDirectOverhangEdit", parameters.lDirectText, dialog);
            } else {
                auto lComplRadioButton = GTWidget::findRadioButton(os, "lComplRadioButton", dialog);
                GTRadioButton::click(os, lComplRadioButton);

                GTLineEdit::setText(os, "lComplOverhangEdit", parameters.lComplText, dialog);
            }
        }
    } else {
        auto lBluntButton = GTWidget::findRadioButton(os, "lBluntButton", dialog);
        GTRadioButton::click(os, lBluntButton);
    }

    if (parameters.rSticky) {
        auto rStickyButton = GTWidget::findRadioButton(os, "rStickyButton", dialog);
        GTRadioButton::click(os, rStickyButton);

        auto rCustomOverhangBox = GTWidget::findGroupBox(os, "rCustomOverhangBox", dialog);
        GTGroupBox::setChecked(os, rCustomOverhangBox, parameters.rCustom);

        if (parameters.rCustom) {
            if (parameters.rDirect) {
                auto rDirectRadioButton = GTWidget::findRadioButton(os, "rDirectRadioButton", dialog);
                GTRadioButton::click(os, rDirectRadioButton);

                GTLineEdit::setText(os, "rDirectOverhangEdit", parameters.rDirectText, dialog);
            } else {
                auto rComplRadioButton = GTWidget::findRadioButton(os, "rComplRadioButton", dialog);
                GTRadioButton::click(os, rComplRadioButton);

                GTLineEdit::setText(os, "rComplOverhangEdit", parameters.rComplText, dialog);
            }
        }
    } else {
        auto rBluntButton = GTWidget::findRadioButton(os, "rBluntButton", dialog);
        GTRadioButton::click(os, rBluntButton);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
