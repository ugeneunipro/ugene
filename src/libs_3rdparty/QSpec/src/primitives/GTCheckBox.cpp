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

#include "primitives/GTCheckBox.h"

#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTCheckBox"

#define GT_METHOD_NAME "setChecked"
void GTCheckBox::setChecked(GUITestOpStatus& os, QCheckBox* checkBox, bool checked) {
    GT_CHECK(checkBox != nullptr, "QCheckBox == NULL");
    if (checkBox->isChecked() == checked) {  // TODO: this should not be used this way: setChecked() must not be called on the disabled checkbox.
        return;
    }
    GT_CHECK(checkBox->isEnabled(), "QCheckBox is disabled: " + checkBox->objectName());
    if (checked != checkBox->isChecked()) {
        GTWidget::click(os, checkBox, Qt::LeftButton, {5, checkBox->rect().height() / 2});
        checkState(os, checkBox, checked);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setChecked"
void GTCheckBox::setChecked(GUITestOpStatus& os, const QString& checkBoxName, bool checked, QWidget* parent) {
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, checkBoxName, parent), checked);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setChecked"
void GTCheckBox::setChecked(GUITestOpStatus& os, const QString& checkBoxName, QWidget* parent) {
    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, checkBoxName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getState"
bool GTCheckBox::getState(GUITestOpStatus& os, QCheckBox* checkBox) {
    GT_CHECK_RESULT(checkBox != NULL, "QCheckBox == NULL", false);
    return checkBox->isChecked();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getState"
bool GTCheckBox::getState(GUITestOpStatus& os, const QString& checkBoxName, QWidget* parent) {
    return getState(os, GTWidget::findCheckBox(os, checkBoxName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIsChecked"
void GTCheckBox::checkState(GUITestOpStatus& os, QCheckBox* checkBox, bool expectedState) {
    GT_CHECK(checkBox != nullptr, "QCheckBox == NULL");
    bool state = checkBox->isChecked();
    for (int time = 0; time <= GT_OP_WAIT_MILLIS && state != expectedState; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        state = checkBox->isChecked();
    }
    GT_CHECK(state == expectedState, QString("Incorrect checkbox state: expected '%1', got '%2'").arg(expectedState).arg(state));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkIsChecked"
void GTCheckBox::checkState(GUITestOpStatus& os, const QString& checkBoxName, bool expectedState, QWidget* parent) {
    checkState(os, GTWidget::findCheckBox(os, checkBoxName, parent), expectedState);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
