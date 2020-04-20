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

#ifndef _HI_GT_CHECKBOX_H_
#define _HI_GT_CHECKBOX_H_

#include "GTGlobals.h"
#include <QCheckBox>

namespace HI {
/*!
 * \brief The class for working with QCheckBox primitive
 */
class HI_EXPORT GTCheckBox {
public:
    static void setChecked(GUITestOpStatus &os, QCheckBox *checkBox, bool checked = true);
    static void setChecked(GUITestOpStatus &os, const QString &checkBoxName, bool checked = true, QWidget const * const parent = NULL);
    static void setChecked(GUITestOpStatus &os, const QString &checkBoxName, QWidget const * const parent = NULL);

    static bool getState(GUITestOpStatus &os, QCheckBox const * const checkBox);
    static bool getState(GUITestOpStatus &os, const QString &checkBoxName, QWidget * const parent = NULL);

    static void checkState(GUITestOpStatus &os, QCheckBox const * const checkBox, const bool expectedState);
    static void checkState(GUITestOpStatus &os, const QString &checkBoxName, bool expectedState, QWidget const * const parent = NULL);
};

}
#endif
