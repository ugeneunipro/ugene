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

#include "SequenceReadingModeSelectorDialogFiller.h"
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::SequenceReadingModeSelectorDialogFiller"

SequenceReadingModeSelectorDialogFiller::SequenceReadingModeSelectorDialogFiller(HI::GUITestOpStatus &_os, CustomScenario *c) :
    Filler(_os, "SequenceReadingModeSelectorDialog", c),
    cancel(false)
{

}

#define GT_METHOD_NAME "commonScenario"
void SequenceReadingModeSelectorDialogFiller::commonScenario()
{   GTGlobals::sleep(1000);
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");
    QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
    GT_CHECK(buttonBox != NULL, "buttonBox not found");
    if (cancel) {
        QPushButton *button = buttonBox->button(QDialogButtonBox::Cancel);
        GT_CHECK(button != NULL, "standard button not found");
        GTWidget::click(os, button);
        return;
    }
    if (readingMode == Separate) {
        QRadioButton *separateRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("separateRB"));
        GT_CHECK(separateRB != NULL, "radio button not found");
        GTRadioButton::click(os, separateRB);
    }
    if (readingMode == Merge) {
        QRadioButton *mergeRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("mergeRB"));
        GT_CHECK(mergeRB != NULL, "radio button not found");
        GTRadioButton::click(os, mergeRB);

        QSpinBox *mergeSpinBox = dialog->findChild<QSpinBox*>(QString::fromUtf8("mergeSpinBox"));
        GT_CHECK(mergeSpinBox != NULL, "merge spin box not found");
        GTSpinBox::setValue(os, mergeSpinBox, bases, GTGlobals::UseKeyBoard);
    }
    if (readingMode == Join) {
        QRadioButton *malignmentRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("malignmentRB"));
        GT_CHECK(malignmentRB != NULL, "radio button not found");
        GTRadioButton::click(os, malignmentRB);
    }
    if (readingMode == Align) {
        QRadioButton *refalignmentRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("refalignmentRB"));
        GT_CHECK(refalignmentRB != NULL, "radio button not found");
        GTRadioButton::click(os, refalignmentRB);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
