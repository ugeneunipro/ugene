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

#include "primitives/GTLineEdit.h"

#include "drivers/GTKeyboardDriver.h"
#include "primitives/GTWidget.h"
#include "system/GTClipboard.h"
#include "utils/GTKeyboardUtils.h"

namespace HI {
#define GT_CLASS_NAME "GTLineEdit"

#define GT_METHOD_NAME "setText"
void GTLineEdit::setText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString& text, bool noCheck /* = false*/, bool useCopyPaste) {
    GT_CHECK(lineEdit != nullptr, "lineEdit is NULL");
    GT_CHECK(!lineEdit->isReadOnly(), "lineEdit is read-only: " + lineEdit->objectName());
    if (lineEdit->text() == text) {
        return;
    }
    clear(os, lineEdit);

    if (useCopyPaste) {
        GTClipboard::setText(os, text);
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    } else {
        GTKeyboardDriver::keySequence(text);
    }
    if (noCheck) {
        GTGlobals::sleep(500);
        return;
    }

    // Wait up to 5 seconds for the text to be updated.
    QString currentText = lineEdit->text();
    for (int time = 0; time <= 5000 && currentText != text; time += 100) {
        GTGlobals::sleep(100);
        currentText = lineEdit->text();
    }
    GT_CHECK(currentText == text, QString("Can't set text, current text is different from a given input in lineEdit '%1'. "
                                          "Expected '%2', got '%3'")
                                      .arg(lineEdit->objectName())
                                      .arg(text)
                                      .arg(currentText));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setText"
void GTLineEdit::setText(GUITestOpStatus& os, const QString& lineEditName, const QString& text, QWidget* parent, bool noCheck, bool useCopyPaste) {
    setText(os, GTWidget::findLineEdit(os, lineEditName, parent), text, noCheck, useCopyPaste);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getText"
QString GTLineEdit::getText(GUITestOpStatus& os, QLineEdit* lineEdit) {
    GT_CHECK_RESULT(lineEdit != nullptr, "lineEdit is NULL", "");
    return lineEdit->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getText"
QString GTLineEdit::getText(GUITestOpStatus& os, const QString& lineEditName, QWidget* parent) {
    return getText(os, GTWidget::findLineEdit(os, lineEditName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clear"
void GTLineEdit::clear(GUITestOpStatus& os, QLineEdit* lineEdit) {
    GT_CHECK(lineEdit != nullptr, "lineEdit is NULL");
    GT_CHECK(!lineEdit->isReadOnly(), "lineEdit is read-only: " + lineEdit->objectName());

    GTWidget::setFocus(os, lineEdit);
    if (lineEdit->text().isEmpty()) {
        return;
    }
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Wait up to 5 seconds for the text to be cleaned.
    QString currentText = lineEdit->text();
    for (int time = 0; time <= 5000 && !currentText.isEmpty(); time += 100) {
        GTGlobals::sleep(100);
        currentText = lineEdit->text();
    }
    GT_CHECK(currentText.isEmpty(), "Can't clear text, lineEdit is not empty: " + currentText);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pasteClipboard"
void GTLineEdit::pasteClipboard(GUITestOpStatus& os, QLineEdit* lineEdit, PasteMethod pasteMethod) {
    clear(os, lineEdit);
    switch (pasteMethod) {
        case Mouse:
            os.setError("GTLineEdit::pasteClipboard: Not implemented: Paste by mouse");
            break;

        default:
        case Shortcut:
            GTKeyboardUtils::paste();
            break;
    }

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkText"
void GTLineEdit::checkText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString& expectedText) {
    GT_CHECK(lineEdit != nullptr, "Line edit is NULL");
    GT_CHECK(expectedText == lineEdit->text(), QString("The text differs: expect '%1', got '%2'").arg(expectedText).arg(lineEdit->text()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkText"
void GTLineEdit::checkText(GUITestOpStatus& os, const QString& lineEditName, QWidget* parent, const QString& expectedText) {
    checkText(os, GTWidget::findLineEdit(os, lineEditName, parent), expectedText);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copyText"
QString GTLineEdit::copyText(GUITestOpStatus& os, QLineEdit* lineEdit) {
    GT_CHECK_RESULT(lineEdit != nullptr, "lineEdit is NULL", QString());
    return lineEdit->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "tryToSetText"
bool GTLineEdit::tryToSetText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString& text) {
    clear(os, lineEdit);

    GTKeyboardDriver::keySequence(text);
    GTGlobals::sleep(500);

    QString currentText = lineEdit->text();
    return currentText == text;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
