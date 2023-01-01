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

SequenceReadingModeSelectorDialogFiller::SequenceReadingModeSelectorDialogFiller(HI::GUITestOpStatus& _os, CustomScenario* c)
    : Filler(_os, "SequenceReadingModeSelectorDialog", c),
      cancel(false) {
}

#define GT_METHOD_NAME "commonScenario"
void SequenceReadingModeSelectorDialogFiller::commonScenario() {
    GTGlobals::sleep(1000);
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    auto buttonBox = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);
    if (cancel) {
        QPushButton* button = buttonBox->button(QDialogButtonBox::Cancel);
        GT_CHECK(button != nullptr, "standard button not found");
        GTWidget::click(os, button);
        return;
    }
    if (readingMode == Separate) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "separateRB", dialog));
    }
    if (readingMode == Merge) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "mergeRB", dialog));

        auto mergeSpinBox = GTWidget::findSpinBox(os, "mergeSpinBox", dialog);
        GTSpinBox::setValue(os, mergeSpinBox, bases, GTGlobals::UseKeyBoard);
    }
    if (readingMode == Join) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "malignmentRB", dialog));
    }
    if (readingMode == Align) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "refalignmentRB", dialog));
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
