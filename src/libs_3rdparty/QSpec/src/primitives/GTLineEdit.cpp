/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTWidget.h"
#include "system/GTClipboard.h"
#include "utils/GTKeyboardUtils.h"
#include "utils/GTThread.h"

namespace HI {
#define GT_CLASS_NAME "GTLineEdit"

#define GT_METHOD_NAME "setText"
void GTLineEdit::setText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString &str, bool noCheck /* = false*/, bool useCopyPaste) {

    GT_CHECK(lineEdit != NULL, "lineEdit is NULL");
    if(lineEdit->text() == str){
        return;
    }

    GTWidget::setFocus(os, lineEdit);
    if (lineEdit->text() == str) {
        return;
    }
    if (!lineEdit->text().isEmpty()) {
        clear(os, lineEdit);
    }

    if(useCopyPaste){
        GTClipboard::setText(os, str);
        GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
    }else{
        GTKeyboardDriver::keySequence(str);
    }
    GTGlobals::sleep(500);

    if(noCheck){
        return;
    }

    QString s = lineEdit->text();
    for (int i = 0; i <= 10 && (s != str); i++) {
        GTGlobals::sleep(500);
        s = lineEdit->text();
    }
    GT_CHECK(s == str, QString("Can't set text, set text differs from a given string in lineEdit '%1'. "
                               "Expected '%2', got '%3'").arg(lineEdit->objectName()).arg(str).arg(s));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setText"
void GTLineEdit::setText(GUITestOpStatus &os, const QString &lineEditName, const QString &text, QWidget const * const parent, bool noCheck, bool useCopyPaste) {
    setText(os, GTWidget::findExactWidget<QLineEdit *>(os, lineEditName, parent), text, noCheck, useCopyPaste);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getText"
QString GTLineEdit::getText(GUITestOpStatus &os, QLineEdit *lineEdit) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != lineEdit, "lineEdit is NULL", "");
    return lineEdit->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getText"
QString GTLineEdit::getText(GUITestOpStatus &os, const QString &lineEditName, QWidget *parent) {
    return getText(os, GTWidget::findExactWidget<QLineEdit *>(os, lineEditName, parent));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clear"
void GTLineEdit::clear(GUITestOpStatus& os, QLineEdit* lineEdit) {

    GT_CHECK(lineEdit != NULL, "lineEdit is NULL");

    GTWidget::setFocus(os, lineEdit);

    GTKeyboardUtils::selectAll(os);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTThread::waitForMainThread();

    QString s = lineEdit->text();
    GT_CHECK(s.isEmpty() == true, "Can't clear text, lineEdit is not empty");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "pasteClipboard"
void GTLineEdit::pasteClipboard(GUITestOpStatus& os, QLineEdit* lineEdit, PasteMethod pasteMethod) {

    GT_CHECK(lineEdit != NULL, "lineEdit is NULL");

    clear(os, lineEdit);
    switch(pasteMethod) {
        case Mouse:
            os.setError("GTLineEdit::pasteClipboard: Not implemented: Paste by mouse");
            break;

        default:
        case Shortcut:
            GTKeyboardUtils::paste(os);
            break;
    }

    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTextSize"
void GTLineEdit::checkTextSize(GUITestOpStatus& os, QLineEdit* lineEdit) {

    GT_CHECK(lineEdit != NULL, "lineEdit is NULL");

    QMargins lineEditMargins = lineEdit->textMargins();
    QFontMetrics fontMetrics = lineEdit->fontMetrics();
    int textWidth = lineEditMargins.left() + lineEditMargins.right() + fontMetrics.width(lineEdit->text());
    int rectWidth = lineEdit->rect().width();

    GT_CHECK(textWidth <= rectWidth, "GTLineEdit::checkTextSize: Text is not inside LineEdit's rect");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkText"
void GTLineEdit::checkText(GUITestOpStatus &os, QLineEdit *lineEdit, const QString &expectedText) {
    Q_UNUSED(os);
    GT_CHECK(NULL != lineEdit, "Line edit is NULL");
    GT_CHECK(expectedText == lineEdit->text(), QString("The text differs: expect '%1', got '%2'").arg(expectedText).arg(lineEdit->text()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkText"
void GTLineEdit::checkText(GUITestOpStatus &os, const QString &lineEditName, const QWidget * const parent, const QString &expectedText) {
    checkText(os, GTWidget::findExactWidget<QLineEdit *>(os, lineEditName, parent), expectedText);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "copyText"
QString GTLineEdit::copyText(GUITestOpStatus& os, QLineEdit* lineEdit) {
    GT_CHECK_RESULT(lineEdit != NULL, "lineEdit is NULL", QString());
    return lineEdit->text();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "tryToSetText"
bool GTLineEdit::tryToSetText(GUITestOpStatus &os, QLineEdit *lineEdit, const QString &str) {
    GT_CHECK_RESULT(lineEdit != NULL, "lineEdit is NULL", false);
    GTWidget::setFocus(os, lineEdit);
    if (lineEdit->text() == str) {
        return true;
    }

    if (!lineEdit->text().isEmpty()) {
        clear(os, lineEdit);
    }

    GTKeyboardDriver::keySequence(str);
    GTGlobals::sleep(500);

    QString s = lineEdit->text();
    return s == str;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace
