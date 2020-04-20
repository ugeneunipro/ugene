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

#ifndef _HI_GT_GROUP_BOX_H_
#define _HI_GT_GROUP_BOX_H_

#include "GTGlobals.h"
#include <QGroupBox>

namespace HI {
/*!
 * \brief The class for working with QGroupBox primitive
 */
class HI_EXPORT GTGroupBox {
public:
    static bool getChecked(GUITestOpStatus &os, QGroupBox *groupBox);
    static bool getChecked(GUITestOpStatus& os, const QString &groupBoxName, QWidget *parent = NULL);

    static void setChecked(GUITestOpStatus &os, QGroupBox *groupBox, bool checked = true);
    static void setChecked(GUITestOpStatus& os, const QString &groupBoxName, bool checked = true, QWidget *parent = NULL);
    static void setChecked(GUITestOpStatus& os, const QString &groupBoxName, QWidget *parent = NULL);

private:
    static QRect getCheckBoxRect(QGroupBox *groupBox);
};

}

#endif
