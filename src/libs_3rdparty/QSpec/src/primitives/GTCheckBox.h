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

#include <QCheckBox>

#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class for working with QCheckBox primitive
 */
class HI_EXPORT GTCheckBox {
public:
    static void setChecked(QCheckBox* checkBox, bool checked = true);
    static void setChecked(const QString& checkBoxName, bool checked = true, QWidget* parent = nullptr);
    static void setChecked(const QString& checkBoxName, QWidget* parent = nullptr);

    static bool getState(QCheckBox* checkBox);
    static bool getState(const QString& checkBoxName, QWidget* parent = nullptr);

    static void checkState(QCheckBox* checkBox, bool expectedState);
    static void checkState(const QString& checkBoxName, bool expectedState, QWidget* parent = nullptr);
};

}  // namespace HI
