/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "DeleteGapsDialogFiller.h"
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QAbstractButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DeleteGapsDialogFiller"
void DeleteGapsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (radioButNum == 1) {
        auto allColumnsOfGaps = GTWidget::findRadioButton("allRadioButton", dialog);
        GTRadioButton::click(allColumnsOfGaps);
    } else {
        auto withNumberOfGaps = GTWidget::findRadioButton("absoluteRadioButton", dialog);
        GTRadioButton::click(withNumberOfGaps);
    }
    GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::RemoveGapColsDialogFiller"
void RemoveGapColsDialogFiller::commonScenario() {
    GTGlobals::sleep(1000);
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto radio = GTWidget::findRadioButton(radioMap[button], dialog);
    GTRadioButton::click(radio);

    if (button == Number) {
        auto box = GTWidget::findSpinBox("absoluteSpinBox", dialog);
        GTSpinBox::setValue(box, spinValue);
    } else if (button == Percent) {
        auto box = GTWidget::findSpinBox("relativeSpinBox", dialog);
        GTSpinBox::setValue(box, spinValue);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME
}  // namespace U2
