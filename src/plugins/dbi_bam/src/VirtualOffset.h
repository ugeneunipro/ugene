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

#ifndef _U2_BAM_VIRTUAL_OFFSET_H_
#define _U2_BAM_VIRTUAL_OFFSET_H_

#include <QtGlobal>

namespace U2 {
namespace BAM {

class VirtualOffset
{
public:
    VirtualOffset(quint64 packedOffset);
    VirtualOffset(quint64 coffset, int uoffset);
    quint64 getPackedOffset()const;
    quint64 getCoffset()const;
    int getUoffset()const;
    bool operator==(VirtualOffset right)const;
    bool operator!=(VirtualOffset right)const;
    bool operator<(VirtualOffset right)const;
    bool operator>(VirtualOffset right)const;
    bool operator<=(VirtualOffset right)const;
    bool operator>=(VirtualOffset right)const;
private:
    quint64 packedOffset;
};

} // namespace BAM
} // namespace U2

#endif // _VIRTUAL_OFFSET_H_
