/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
k
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

#ifndef GTUTILS_ESC_CLICKER_H_
#define GTUTILS_ESC_CLICKER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class GTUtilsEscClicker : public Filler {
public:
    GTUtilsEscClicker( HI::GUITestOpStatus &_os, const QString &menuObjectName, bool _mouse = false );
    void commonScenario();

private:
    bool mouse;
};

} // namespace U2

#endif // GTUTILS_ESC_CLICKER_H_
