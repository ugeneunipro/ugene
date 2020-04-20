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

#ifndef _HI_GT_SPINBOX_H_
#define _HI_GT_SPINBOX_H_

#include "GTGlobals.h"
#include <QSpinBox>

namespace HI {
/*!
 * \brief The class for working with QSpinBox primitive
 */
class HI_EXPORT GTSpinBox {
public:
    static int getValue(GUITestOpStatus &os, QSpinBox *spinBox);
    static int getValue(GUITestOpStatus &os, const QString &spinBoxName, QWidget *parent = NULL);

    static void setValue(GUITestOpStatus &os, QSpinBox *spinBox, int v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse);
    static void setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse, QWidget *parent = NULL);
    static void setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, QWidget *parent = NULL);

    static void checkLimits(GUITestOpStatus &os, QSpinBox *spinBox, int min, int max);
    static void checkLimits(GUITestOpStatus &os, const QString &spinBoxName, int min, int max, QWidget *parent = NULL);
};

}   // namespace

#endif
