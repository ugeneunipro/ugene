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

#ifndef _HI_GT_LINEEDIT_H_
#define _HI_GT_LINEEDIT_H_

#include "GTGlobals.h"
#include <QLineEdit>

namespace HI {
/*!
 * \brief The class for working with QLineEdit primitive
 */
class HI_EXPORT GTLineEdit {
public:
    enum PasteMethod {Shortcut, Mouse};

    // fails if lineEdit is NULL, GTLineEdit::clear fails
    // or a set text differs from a given string
#ifdef Q_OS_MAC
    static void setText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString &str, bool noCheck = false, bool useCopyPaste = true);
    static void setText(GUITestOpStatus& os, const QString &lineEditName, const QString &text, const QWidget * const parent, bool noCheck = false, bool useCopyPaste = true);
#else
    static void setText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString &str, bool noCheck = false, bool useCopyPaste = false);
    static void setText(GUITestOpStatus& os, const QString &lineEditName, const QString &text, QWidget const * const parent, bool noCheck = false, bool useCopyPaste = false);
#endif
    static QString getText(GUITestOpStatus &os, QLineEdit *lineEdit);
    static QString getText(GUITestOpStatus &os, const QString &lineEditName, QWidget *parent = NULL);

    // fails if lineEdit is NULL, or lineEdit's text wasn't cleared
    static void clear(GUITestOpStatus& os, QLineEdit* lineEdit);

    // fails if GTLineEdit::clear fails
    static void pasteClipboard(GUITestOpStatus& os, QLineEdit* lineEdit, PasteMethod pasteMethod = Shortcut);

    // fails if lineEdit is NULL or lineEdit text is not in lineEdit's rect
    // considering lineEdit's fontMetrics and textMargins
    static void checkTextSize(GUITestOpStatus& os, QLineEdit* lineEdit);
    static void checkText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString &expectedText);
    static void checkText(GUITestOpStatus& os, const QString &lineEditName, QWidget const * const parent, const QString &expectedText);

    static QString copyText(GUITestOpStatus& os, QLineEdit* lineEdit);

    // fails if lineEdit is NULL
    // checks if str can be pasted in lineEdit
    static bool tryToSetText(GUITestOpStatus& os, QLineEdit* lineEdit, const QString& str);
};

}   // namespace

#endif // _HI_GT_LINEEDIT_H_
