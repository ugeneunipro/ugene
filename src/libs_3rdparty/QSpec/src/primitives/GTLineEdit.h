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

#pragma once
#include <QLineEdit>

#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class for working with QLineEdit primitive
 */
class HI_EXPORT GTLineEdit {
public:
    enum PasteMethod {
        Shortcut,
        Mouse
    };

    static void setText(QLineEdit* lineEdit, const QString& text, bool noCheck = false, bool useCopyPaste = false);
    static void setText(const QString& lineEditName, const QString& text, QWidget* parent = nullptr, bool noCheck = false, bool useCopyPaste = false);
    static QString getText(QLineEdit* lineEdit);
    static QString getText(const QString& lineEditName, QWidget* parent = nullptr);

    // fails if lineEdit is NULL, or lineEdit's text wasn't cleared
    static void clear(QLineEdit* lineEdit);

    // fails if lineEdit is NULL or lineEdit text is not in lineEdit's rect
    // considering lineEdit's fontMetrics and textMargins
    static void checkText(QLineEdit* lineEdit, const QString& expectedText);
    static void checkText(const QString& lineEditName, QWidget* parent, const QString& expectedText);

    static QString copyText(QLineEdit* lineEdit);

    // fails if lineEdit is NULL
    // checks if str can be pasted in lineEdit
    static bool tryToSetText(QLineEdit* lineEdit, const QString& text);
};

}  // namespace HI
