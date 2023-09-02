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

void GTLineEdit::setText(QLineEdit* lineEdit, const QString& text, bool noCheck /* = false*/, bool useCopyPaste) {
    GT_CHECK(lineEdit != nullptr, "lineEdit is NULL");
    GT_CHECK(!lineEdit->isReadOnly(), "lineEdit is read-only: " + lineEdit->objectName());
    if (lineEdit->text() == text) {
        return;
    }
    if (text.isEmpty()) {
        clear(lineEdit);
    } else {
        selectAll(lineEdit);
        if (useCopyPaste) {
            GTClipboard::setText(text);
            GTKeyboardUtils::paste();
        } else {
            GTKeyboardDriver::keySequence(text);
        }
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

void GTLineEdit::setText(const QString& lineEditName, const QString& text, QWidget* parent, bool noCheck, bool useCopyPaste) {
    setText(GTWidget::findLineEdit(lineEditName, parent), text, noCheck, useCopyPaste);
}

QString GTLineEdit::getText(QLineEdit* lineEdit) {
    GT_CHECK_RESULT(lineEdit != nullptr, "lineEdit is NULL", "");
    return lineEdit->text();
}

QString GTLineEdit::getText(const QString& lineEditName, QWidget* parent) {
    return getText(GTWidget::findLineEdit(lineEditName, parent));
}

void GTLineEdit::selectAll(QLineEdit* lineEdit) {
    GT_CHECK(lineEdit != nullptr, "lineEdit is NULL");
    GT_CHECK(!lineEdit->isReadOnly(), "lineEdit is read-only: " + lineEdit->objectName());

    if (lineEdit != QApplication::focusWidget()) {
        GTWidget::setFocus(lineEdit);
    }
    if (lineEdit->text().isEmpty()) {
        return;
    }
    GTKeyboardUtils::selectAll();
}

void GTLineEdit::clear(QLineEdit* lineEdit) {
    selectAll(lineEdit);
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);

    // Wait up to 5 seconds for the text to be cleaned.
    QString currentText = lineEdit->text();
    for (int time = 0; time <= 5000 && !currentText.isEmpty(); time += 100) {
        GTGlobals::sleep(100);
        currentText = lineEdit->text();
    }
    GT_CHECK(currentText.isEmpty(), "Can't clear text, lineEdit is not empty: " + currentText);
}

void GTLineEdit::checkText(QLineEdit* lineEdit, const QString& expectedText) {
    GT_CHECK(lineEdit != nullptr, "Line edit is NULL");
    GT_CHECK(expectedText == lineEdit->text(), QString("The text differs: expect '%1', got '%2'").arg(expectedText).arg(lineEdit->text()));
}

void GTLineEdit::checkText(const QString& lineEditName, QWidget* parent, const QString& expectedText) {
    checkText(GTWidget::findLineEdit(lineEditName, parent), expectedText);
}

QString GTLineEdit::copyText(QLineEdit* lineEdit) {
    GT_CHECK_RESULT(lineEdit != nullptr, "lineEdit is NULL", QString());
    return lineEdit->text();
}

bool GTLineEdit::tryToSetText(QLineEdit* lineEdit, const QString& text) {
    clear(lineEdit);

    GTKeyboardDriver::keySequence(text);
    GTGlobals::sleep(500);

    QString currentText = lineEdit->text();
    return currentText == text;
}

#undef GT_CLASS_NAME

}  // namespace HI
