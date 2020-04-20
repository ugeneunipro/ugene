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


#include "EditFragmentDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTRadioButton.h>
#include <QApplication>
#include <QGroupBox>
#include <QComboBox>

namespace U2 {

EditFragmentDialogFiller::EditFragmentDialogFiller(HI::GUITestOpStatus &os, const Parameters &parameters)
: Filler(os, "EditFragmentDialog"), parameters(parameters)
{

}


#define GT_CLASS_NAME "GTUtilsDialog::EditFragmentDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void EditFragmentDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    // GUITest_regression_scenarios_test_0574
    if (parameters.checkRComplText) {
        GTRadioButton::click(os, qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rStickyButton", dialog)));
        QGroupBox *rCustomOverhangBox = qobject_cast<QGroupBox*>(GTWidget::findWidget(os, "rCustomOverhangBox", dialog));
        rCustomOverhangBox->setChecked(true);
        GTRadioButton::click(os, qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rComplRadioButton", dialog)));
        QLineEdit *rComplOverhangEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "rComplOverhangEdit", dialog));
        GT_CHECK(rComplOverhangEdit->text() == parameters.rComplText, "Wrong rComplTextEdit text");
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (parameters.lSticky) {
        QRadioButton *lStickyButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "lStickyButton", dialog));
        GT_CHECK(lStickyButton, "lStickyButton not found");
        GTRadioButton::click(os, lStickyButton);

        QGroupBox *lCustomOverhangBox = qobject_cast<QGroupBox*>(GTWidget::findWidget(os, "lCustomOverhangBox", dialog));
        lCustomOverhangBox->setChecked(parameters.lCustom);

        if (parameters.lCustom) {
            if (parameters.lDirect) {
                QRadioButton *lDirectRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "lDirectRadioButton", dialog));
                GT_CHECK(lDirectRadioButton, "lDirectRadioButton not found");
                GTRadioButton::click(os, lDirectRadioButton);

                QLineEdit *lDirectOverhangEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "lDirectOverhangEdit", dialog));
                GT_CHECK(lDirectOverhangEdit, "lDirectOverhangEdit is NULL");
                GTLineEdit::setText(os, lDirectOverhangEdit, parameters.lDirectText);
            } else {
                QRadioButton *lComplRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "lComplRadioButton", dialog));
                GT_CHECK(lComplRadioButton, "lComplRadioButton not found");
                GTRadioButton::click(os, lComplRadioButton);

                QLineEdit *lComplOverhangEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "lComplOverhangEdit", dialog));
                GT_CHECK(lComplOverhangEdit, "lComplOverhangEdit is NULL");
                GTLineEdit::setText(os, lComplOverhangEdit, parameters.lComplText);
            }
        }
    } else {
        QRadioButton *lBluntButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "lBluntButton", dialog));
        GT_CHECK(lBluntButton, "lBluntButton not found");
        GTRadioButton::click(os, lBluntButton);
    }

    if (parameters.rSticky) {
        QRadioButton *rStickyButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rStickyButton", dialog));
        GT_CHECK(rStickyButton, "rStickyButton not found");
        GTRadioButton::click(os, rStickyButton);

        QGroupBox *rCustomOverhangBox = qobject_cast<QGroupBox*>(GTWidget::findWidget(os, "rCustomOverhangBox", dialog));
        rCustomOverhangBox->setChecked(parameters.rCustom);

        if (parameters.rCustom) {
            if (parameters.rDirect) {
                QRadioButton *rDirectRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rDirectRadioButton", dialog));
                GT_CHECK(rDirectRadioButton, "rDirectRadioButton not found");
                GTRadioButton::click(os, rDirectRadioButton);

                QLineEdit *rDirectOverhangEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "rDirectOverhangEdit", dialog));
                GT_CHECK(rDirectOverhangEdit, "rDirectOverhangEdit is NULL");
                GTLineEdit::setText(os, rDirectOverhangEdit, parameters.rDirectText);
            } else {
                QRadioButton *rComplRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rComplRadioButton", dialog));
                GT_CHECK(rComplRadioButton, "rComplRadioButton not found");
                GTRadioButton::click(os, rComplRadioButton);

                QLineEdit *rComplOverhangEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "rComplOverhangEdit", dialog));
                GT_CHECK(rComplOverhangEdit, "rComplOverhangEdit is NULL");
                GTLineEdit::setText(os, rComplOverhangEdit, parameters.rComplText);
            }
        }
    } else {
        QRadioButton *rBluntButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rBluntButton", dialog));
        GT_CHECK(rBluntButton, "rBluntButton not found");
        GTRadioButton::click(os, rBluntButton);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} // U2
