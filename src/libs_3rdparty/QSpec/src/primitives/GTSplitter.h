/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_SPLITTER_H_
#define _HI_GT_SPLITTER_H_

#include "GTGlobals.h"

#include <QSplitter>

namespace HI {

/*!
 * \brief The class for working with QSplitter primitive
 */
class GTSplitter {
public:
    static QRect getHandleRect(GUITestOpStatus &os, QSplitter *splitter, int handleNumber = 0);
    static void moveHandle(GUITestOpStatus &os, QSplitter *splitter, int pixels, int handleNumber = 0);
};

}   // namespace HI

#endif // _HI_GT_SPLITTER_H_
