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

#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTWidget.h>

#include "primitives/GTTextEdit.h"
#include "utils/GTKeyboardUtils.h"

namespace HI {

#define GT_CLASS_NAME "GTTextEdit"

#define GT_METHOD_NAME "setText"
void GTTextEdit::setText(QTextEdit* textEdit, const QString& text) {
    GT_CHECK(textEdit != nullptr, "textEdit is NULL");

    if (textEdit->toPlainText() == text) {
        return;
    }

    clear(textEdit);
    GTWidget::setFocus(textEdit);

    GTKeyboardDriver::keySequence(text);
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getText"
QString GTTextEdit::getText(QTextEdit* textEdit) {
    GT_CHECK_RESULT(textEdit != nullptr, "textEdit is NULL", "");
    return textEdit->toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "containsString"
bool GTTextEdit::containsString(QTextEdit* textEdit, const QString& string) {
    const QString text = getText(textEdit);
    return text.contains(string);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clear"
void GTTextEdit::clear(QTextEdit* textEdit) {
    GT_CHECK(textEdit != NULL, "textEdit is NULL");

    GTWidget::setFocus(textEdit);

    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(1000);

    QString s = textEdit->toPlainText();
    GT_CHECK(s.isEmpty(), "Can't clear text, textEdit is not empty");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
