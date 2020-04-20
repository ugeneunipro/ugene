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

#ifndef _U2_BIOSTRUCT3D_OBJECT_H_
#define _U2_BIOSTRUCT3D_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/U2RawData.h>

namespace U2 {

class U2CORE_EXPORT U2BioStruct3D : public U2RawData {
public:
    U2BioStruct3D();
    U2BioStruct3D(const U2DbiRef &dbiRef);

    U2DataType getType() const;
};

class U2CORE_EXPORT BioStruct3DObject : public GObject {
    Q_OBJECT
public:
    static BioStruct3DObject * createInstance(const BioStruct3D &bioStruct3D, const QString &objectName, const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap = QVariantMap());
    BioStruct3DObject(const QString &objectName, const U2EntityRef &structRef, const QVariantMap &hintsMap = QVariantMap());

    const BioStruct3D & getBioStruct3D() const;
    GObject * clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

protected:
    void loadDataCore(U2OpStatus &os);

private:
    BioStruct3DObject(const BioStruct3D &bioStruct3D, const QString &objectName, const U2EntityRef &structRef, const QVariantMap &hintsMap);

    BioStruct3D     bioStruct3D;
};

} // U2


#endif //_U2_BIOSTRUCT3D_OBJECT_H_
