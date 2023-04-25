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

#include <QComboBox>

#include "GTGlobals.h"

#include <core/GUITestOpStatus.h>

namespace U2 {

using namespace HI;

/*!
 * \brief The class for working with U2::ComboBoxWithCheckBoxes custom widget
 */
class GTComboBoxWithCheckBoxes {
public:
    GTComboBoxWithCheckBoxes() = delete;

    // Check all checkboxes from @indexes and uncheck all others
    static void selectItemByIndex(GUITestOpStatus& os, QComboBox* comboBox, const QList<int>& indexes, GTGlobals::UseMethod method = GTGlobals::UseKey);

    // Check all checkboxes from @texts and uncheck all others
    static void selectItemByText(GUITestOpStatus& os, QComboBox* comboBox, const QStringList& texts, GTGlobals::UseMethod method = GTGlobals::UseKey);
    static void selectItemByText(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent, const QStringList& text, GTGlobals::UseMethod method = GTGlobals::UseKey);

    static QStringList getCheckedItemsTexts(GUITestOpStatus& os, const QString& comboBoxName, QWidget* parent);



};

}
