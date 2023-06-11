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

#include "SequenceReadingModeSelectorDialogFiller.h"
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::SequenceReadingModeSelectorDialogFiller"

SequenceReadingModeSelectorDialogFiller::SequenceReadingModeSelectorDialogFiller(CustomScenario* c)
    : Filler("SequenceReadingModeSelectorDialog", c),
      cancel(false) {
}

#define GT_METHOD_NAME "commonScenario"
void SequenceReadingModeSelectorDialogFiller::commonScenario() {
    GTGlobals::sleep(1000);
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);
    if (cancel) {
        QPushButton* button = buttonBox->button(QDialogButtonBox::Cancel);
        GT_CHECK(button != nullptr, "standard button not found");
        GTWidget::click(button);
        return;
    }
    if (readingMode == Separate) {
        GTRadioButton::click(GTWidget::findRadioButton("separateRB", dialog));
    }
    if (readingMode == Merge) {
        GTRadioButton::click(GTWidget::findRadioButton("mergeRB", dialog));

        auto mergeSpinBox = GTWidget::findSpinBox("mergeSpinBox", dialog);
        GTSpinBox::setValue(mergeSpinBox, bases, GTGlobals::UseKeyBoard);
    }
    if (readingMode == Join) {
        GTRadioButton::click(GTWidget::findRadioButton("malignmentRB", dialog));
    }
    if (readingMode == Align) {
        GTRadioButton::click(GTWidget::findRadioButton("refalignmentRB", dialog));
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
