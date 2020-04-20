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

#ifndef _U2_RAWDATA_H_
#define _U2_RAWDATA_H_

#include <U2Core/U2Type.h>

namespace U2 {

/**
 * Raw data representation. The data itself must be packed using
 * a serializer. The serializer's identifier is kept by this structure.
 */
class U2CORE_EXPORT U2RawData : public U2Object {
public:
    U2RawData() : U2Object() {}
    U2RawData(const U2DbiRef &dbiRef) : U2Object(U2DataId(), dbiRef.dbiId, 0) {}

    /** The identifier of a serializer which the data is packed by */
    QString serializer;
};

} // U2

#endif // _U2_RAWDATA_H_
