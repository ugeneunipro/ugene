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

#include "primitives/GTPlainTextEdit.h"

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"
#include "utils/GTKeyboardUtils.h"

namespace HI {

#define GT_CLASS_NAME "GTPlainTextEdit"

#define GT_METHOD_NAME "setText"
void GTPlainTextEdit::setText(GUITestOpStatus& os, QPlainTextEdit* textEdit, const QString& text) {
    GT_CHECK(textEdit != nullptr, "textEdit is NULL");
    GT_CHECK(!textEdit->isReadOnly(), "textEdit is read-only: " + textEdit->objectName());
    if (textEdit->toPlainText() == text) {
        return;
    }
    clear(os, textEdit);
    if (text.isEmpty()) {
        return;
    }
    GTWidget::setFocus(os, textEdit);
    GTKeyboardDriver::keySequence(text);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clear"
void GTPlainTextEdit::clear(GUITestOpStatus& os, QPlainTextEdit* textEdit) {
    GT_CHECK(textEdit != nullptr, "textEdit is NULL");
    GT_CHECK(!textEdit->isReadOnly(), "textEdit is read-only: " + textEdit->objectName());

    GTWidget::setFocus(os, textEdit);
    if (textEdit->toPlainText().isEmpty()) {
        return;
    }
    GTKeyboardUtils::selectAll();
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Wait up to 5 seconds for the text to be cleaned.
    QString currentText = textEdit->toPlainText();
    for (int time = 0; time <= 5000 && !currentText.isEmpty(); time += 100) {
        GTGlobals::sleep(100);
        currentText = textEdit->toPlainText();
    }
    GT_CHECK(currentText.isEmpty(), "Can't clear text, plainTextEdit is not empty: " + currentText);
}
#undef GT_METHOD_NAME

}  // namespace HI
