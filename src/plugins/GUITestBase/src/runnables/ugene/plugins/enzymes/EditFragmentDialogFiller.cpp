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

#include "EditFragmentDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QGroupBox>

namespace U2 {

EditFragmentDialogFiller::EditFragmentDialogFiller(const Parameters& parameters)
    : Filler("EditFragmentDialog"), parameters(parameters) {
}

#define GT_CLASS_NAME "GTUtilsDialog::EditFragmentDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void EditFragmentDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    // GUITest_regression_scenarios_test_0574
    if (parameters.checkRComplText) {
        GTRadioButton::click(GTWidget::findRadioButton("rStickyButton", dialog));
        auto rCustomOverhangBox = GTWidget::findGroupBox("rCustomOverhangBox", dialog);
        GTGroupBox::setChecked(rCustomOverhangBox, true);
        GTRadioButton::click(GTWidget::findRadioButton("rComplRadioButton", dialog));
        GT_CHECK(GTLineEdit::getText("rComplOverhangEdit", dialog) == parameters.rComplText, "Wrong rComplTextEdit text");
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (parameters.lSticky) {
        auto lStickyButton = GTWidget::findRadioButton("lStickyButton", dialog);
        GTRadioButton::click(lStickyButton);

        auto lCustomOverhangBox = GTWidget::findGroupBox("lCustomOverhangBox", dialog);
        GTGroupBox::setChecked(lCustomOverhangBox, parameters.lCustom);

        if (parameters.lCustom) {
            if (parameters.lDirect) {
                auto lDirectRadioButton = GTWidget::findRadioButton("lDirectRadioButton", dialog);
                GTRadioButton::click(lDirectRadioButton);

                GTLineEdit::setText("lDirectOverhangEdit", parameters.lDirectText, dialog);
            } else {
                auto lComplRadioButton = GTWidget::findRadioButton("lComplRadioButton", dialog);
                GTRadioButton::click(lComplRadioButton);

                GTLineEdit::setText("lComplOverhangEdit", parameters.lComplText, dialog);
            }
        }
    } else {
        auto lBluntButton = GTWidget::findRadioButton("lBluntButton", dialog);
        GTRadioButton::click(lBluntButton);
    }

    if (parameters.rSticky) {
        auto rStickyButton = GTWidget::findRadioButton("rStickyButton", dialog);
        GTRadioButton::click(rStickyButton);

        auto rCustomOverhangBox = GTWidget::findGroupBox("rCustomOverhangBox", dialog);
        GTGroupBox::setChecked(rCustomOverhangBox, parameters.rCustom);

        if (parameters.rCustom) {
            if (parameters.rDirect) {
                auto rDirectRadioButton = GTWidget::findRadioButton("rDirectRadioButton", dialog);
                GTRadioButton::click(rDirectRadioButton);

                GTLineEdit::setText("rDirectOverhangEdit", parameters.rDirectText, dialog);
            } else {
                auto rComplRadioButton = GTWidget::findRadioButton("rComplRadioButton", dialog);
                GTRadioButton::click(rComplRadioButton);

                GTLineEdit::setText("rComplOverhangEdit", parameters.rComplText, dialog);
            }
        }
    } else {
        auto rBluntButton = GTWidget::findRadioButton("rBluntButton", dialog);
        GTRadioButton::click(rBluntButton);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
