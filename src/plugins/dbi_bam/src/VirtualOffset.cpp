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

#include <cassert>
#include "VirtualOffset.h"

namespace U2 {
namespace BAM {

VirtualOffset::VirtualOffset(quint64 packedOffset):
        packedOffset(packedOffset)
{
}

VirtualOffset::VirtualOffset(quint64 coffset, int uoffset):
        packedOffset((coffset << 16) | uoffset)
{
    assert(coffset <= 0xffffffffffffLL);
    assert(uoffset >= 0);
    assert(uoffset <= 0xffff);
}

quint64 VirtualOffset::getPackedOffset()const {
    return packedOffset;
}

quint64 VirtualOffset::getCoffset()const {
    return packedOffset >> 16;
}

int VirtualOffset::getUoffset()const {
    return (int)packedOffset & 0xffff;
}

bool VirtualOffset::operator==(VirtualOffset right)const {
    return packedOffset == right.packedOffset;
}

bool VirtualOffset::operator!=(VirtualOffset right)const {
    return packedOffset != right.packedOffset;
}

bool VirtualOffset::operator<(VirtualOffset right)const {
    return packedOffset < right.packedOffset;
}

bool VirtualOffset::operator>(VirtualOffset right)const {
    return packedOffset > right.packedOffset;
}

bool VirtualOffset::operator<=(VirtualOffset right)const {
    return packedOffset <= right.packedOffset;
}

bool VirtualOffset::operator>=(VirtualOffset right)const {
    return packedOffset >= right.packedOffset;
}

} // namespace BAM
} // namespace U2
