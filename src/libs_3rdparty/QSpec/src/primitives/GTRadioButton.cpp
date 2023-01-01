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
#include "primitives/GTMainWindow.h"
#include "primitives/GTWidget.h"

namespace HI {
#define GT_CLASS_NAME "GTRadioButton"

#define GT_METHOD_NAME "click"
void GTRadioButton::click(GUITestOpStatus& os, QRadioButton* radioButton) {
    GT_CHECK(radioButton != NULL, "RadioButton is NULL");
    if (radioButton->isChecked() == true) {
        return;
    }

    QPoint buttonPos = radioButton->mapToGlobal(radioButton->rect().topLeft());
    if (Qt::RightToLeft != radioButton->layoutDirection()) {
        buttonPos = QPoint(buttonPos.x() + 10, buttonPos.y() + 10);  // moved to clickable area
    } else {
        buttonPos = QPoint(radioButton->mapToGlobal(QPoint(radioButton->rect().right(), 0)).x() - 10, buttonPos.y() + 10);  // moved to clickable area
    }

    GTMouseDriver::moveTo(buttonPos);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTRadioButton::click(GUITestOpStatus& os, const QString& radioButtonName, QWidget* parent) {
    click(os, GTWidget::findRadioButton(os, radioButtonName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRadioButtonByText"
QRadioButton* GTRadioButton::getRadioButtonByText(GUITestOpStatus& os, QString text, QWidget* parent) {
    QList<QRadioButton*> radioList = getAllButtonsByText(os, text, parent);
    GT_CHECK_RESULT(radioList.size() > 1, "Several radioButtons contain this text", NULL);
    GT_CHECK_RESULT(radioList.size() == 0, "No radioButtons with this text found", NULL);

    return radioList[0];
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAllButtonsByText"
QList<QRadioButton*> GTRadioButton::getAllButtonsByText(GUITestOpStatus& os, const QString& text, QWidget* parent) {
    return GTWidget::findChildren<QRadioButton>(os,
                                                parent,
                                                [text](auto button) { return button->text() == text; });
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIsChecked"
void GTRadioButton::checkIsChecked(GUITestOpStatus& os, QRadioButton* button, bool expectedState) {
    GT_CHECK(button != nullptr, "QRadioButton == NULL");
    bool state = button->isChecked();
    for (int time = 0; time <= GT_OP_WAIT_MILLIS && state != expectedState; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        state = button->isChecked();
    }
    GT_CHECK(state == expectedState, QString("Incorrect radio button state: expected '%1', got '%2'").arg(expectedState).arg(state));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
