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

#include "primitives/GTRadioButton.h"

#include <QApplication>
#include <QMainWindow>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"

namespace HI {
#define GT_CLASS_NAME "GTRadioButton"

#define GT_METHOD_NAME "click"
void GTRadioButton::click(QRadioButton* radioButton) {
    GT_CHECK(radioButton != nullptr, "RadioButton is NULL");
    if (radioButton->isChecked()) {
        return;
    }
    QPoint buttonPos = radioButton->layoutDirection() != Qt::RightToLeft
                           ? radioButton->mapToGlobal(radioButton->rect().topLeft()) + QPoint(10, 10)
                           : radioButton->mapToGlobal(radioButton->rect().topRight()) + QPoint(-10, 10);
    GTMouseDriver::moveTo(buttonPos);
    GTMouseDriver::click();
    checkIsChecked(radioButton, true);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTRadioButton::click(const QString& radioButtonName, QWidget* parent) {
    click(GTWidget::findRadioButton(radioButtonName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIsChecked"
void GTRadioButton::checkIsChecked(QRadioButton* button, bool expectedState) {
    GT_CHECK(button != nullptr, "QRadioButton == NULL");
    bool state = button->isChecked();
    for (int time = 0; time <= 5000 && state != expectedState; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        state = button->isChecked();
    }
    GT_CHECK(state == expectedState, QString("Incorrect radio button state: expected '%1', got '%2', button: %3").arg(expectedState).arg(state).arg(button->objectName()));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
