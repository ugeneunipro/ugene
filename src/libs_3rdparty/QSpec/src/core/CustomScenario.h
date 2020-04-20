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

#ifndef _HI_CUSTOM_SCENARIO_H_
#define _HI_CUSTOM_SCENARIO_H_

#include <core/global.h>
#include <core/GUITestOpStatus.h>

namespace HI {


class HI_EXPORT CustomScenario {
public:
    virtual ~CustomScenario();
    virtual void run(HI::GUITestOpStatus &os) = 0;
};

}   // namespace

#endif // _HI_CUSTOM_SCENARIO_H_
