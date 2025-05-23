/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2Assembly.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2Region.h>

#pragma once

namespace U2 {

class U2CORE_EXPORT U2AssemblyDbiUtils {
public:    
    //calculates min distance for reads in given region, from start to leftmost read and leength between leftmost and rightmost longest reads
    static QPair<qint64, qint64> calculateLeftShiftAndLength(U2AssemblyDbi* srcAssemblyDbi, U2DataId srcObjId, 
                                                                                const U2Region& desiredRegion, U2OpStatus& os);
};

}
