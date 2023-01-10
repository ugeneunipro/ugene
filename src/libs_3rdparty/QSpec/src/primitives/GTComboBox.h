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

#ifndef _HI_GT_COMBOBOX_H_
#define _HI_GT_COMBOBOX_H_

#include <QComboBox>

#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class for working with QComboBox primitive
 */
class HI_EXPORT GTComboBox {
public:
    // Fails if the comboBox is NULL, index is not in a comboBox range
    // or a comboBox's index differs from a given index in the end of method's execution
    static void selectItemByIndex(GUITestOpStatus& os, QComboBox* comboBox, int index, GTGlobals::UseMethod method = GTGlobals::UseKey);

    // Fails if the comboBox is NULL, combobox doesn't contain an item with text
    // or a comboBox current item's text differs from a given text in the end of method's execution
    static void selectItemByText(GUITestOpStatus& os, QComboBox* comboBox, const QString& text, GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void selectItemByText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent, const QString& text, GTGlobals::UseMethod method = GTGlobals::UseKey);

    static QString getCurrentText(GUITestOpStatus& os, QComboBox* comboBox);
    static QString getCurrentText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent = nullptr);

    static QStringList getValues(GUITestOpStatus& os, QComboBox* comboBox);
    static void checkValues(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& values);

    static void checkValuesPresence(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& values);
    static void checkCurrentValue(GUITestOpStatus& os, QComboBox* comboBox, const QString& expectedText);
    static void checkCurrentUserDataValue(GUITestOpStatus& os, QComboBox* comboBox, const QString& expectedValue);
};

}  // namespace HI
#endif
