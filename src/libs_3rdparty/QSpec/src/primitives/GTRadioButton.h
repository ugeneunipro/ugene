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
#include <QRadioButton>

#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class for working with QRadioButton primitive
 */
class HI_EXPORT GTRadioButton {
public:
    // fails if the radioButton is NULL or can't click
    static void click(QRadioButton* radioButton);
    static void click(const QString& radioButtonName, QWidget* parent = nullptr);

    static QRadioButton* getRadioButtonByText(QString text, QWidget* parent = nullptr);
    static QList<QRadioButton*> getAllButtonsByText(const QString& text, QWidget* parent = nullptr);

    /** Checks if the radio button is checked. */
    static void checkIsChecked(QRadioButton* button, bool expectedState = true);
};

}  // namespace HI
