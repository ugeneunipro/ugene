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

#ifndef _U2_ATTRIBUTE_UTILS_H_
#define _U2_ATTRIBUTE_UTILS_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Attribute.h>

namespace U2 {

class U2AttributeDbi;
class U2OpStatus;

/**
    U2Attribute utility functions

    All functions return empty attribute if it is not found or error occurred.
    Use U2Entity::hasValidId() to check for valid value.
*/
class U2CORE_EXPORT U2AttributeUtils : public QObject {
    Q_OBJECT
private:
    U2AttributeUtils(){}
public:

    static U2IntegerAttribute findIntegerAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& name, U2OpStatus& os);

    static U2RealAttribute findRealAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& name, U2OpStatus& os);

    static U2ByteArrayAttribute findByteArrayAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& name, U2OpStatus& os);

    static U2StringAttribute findStringAttribute(U2AttributeDbi* adbi, const U2DataId& objectId, const QString& name, U2OpStatus& os);

    static void init(U2Attribute &attr, const U2Object &obj, const QString &name);

    static void removeAttribute(U2AttributeDbi* adbi, const U2DataId& attrId, U2OpStatus& os);

    static void copyObjectAttributes(const U2DataId &srcObjId, const U2DataId &dstObjId,
        U2AttributeDbi *srcAttributeDbi, U2AttributeDbi *dstAttributeDbi,
        U2OpStatus &os);

    static void copyObjectAttributes(const U2EntityRef &srcObjRef, const U2EntityRef &dstObjRef, U2OpStatus &os);
};

} //namespace


#endif
