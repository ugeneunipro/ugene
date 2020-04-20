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
#include "primitives/GTPlainTextEdit.h"
#include "primitives/GTWidget.h"
#include "utils/GTKeyboardUtils.h"

namespace HI {

#define GT_CLASS_NAME "GTPlainTextEdit"

#define GT_METHOD_NAME "setPlainText"
void GTPlainTextEdit::setPlainText(GUITestOpStatus& os, QPlainTextEdit* plainTextEdit, const QString &text) {

    GT_CHECK(plainTextEdit != NULL, "plainTextEdit is NULL");
    if(plainTextEdit->toPlainText() == text){
        return;
    }

    GTWidget::setFocus(os, plainTextEdit);

    GTKeyboardDriver::keySequence(text);
    GTGlobals::sleep(500);

}
#undef GT_METHOD_NAME

//#define GT_METHOD_NAME "clear"
//void GTPlainTextEdit::clear(GUITestOpStatus& os, QPlainTextEdit* plainTextEdit) {
//
//    GT_CHECK(plainTextEdit != NULL, "plainTextEdit is NULL");
//
//    GTWidget::setFocus(os, plainTextEdit);
//
//    GTKeyboardUtils::selectAll(os);
//    GTGlobals::sleep(100);
//    GTKeyboardDriver::keyClick( Qt::Key_Delete);
//    GTGlobals::sleep(1000);
//
//    QString s = plainTextEdit->toPlainText();
//    GT_CHECK(s.isEmpty() == true, "Can't clear text, lineEdit is not empty");
//}
//#undef GT_METHOD_NAME



}
