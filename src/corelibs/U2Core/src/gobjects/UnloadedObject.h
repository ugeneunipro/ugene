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

#ifndef _U2_UNLOADED_OBJECT_H_
#define _U2_UNLOADED_OBJECT_H_

#include <U2Core/GObject.h>
#include "GObjectTypes.h"

namespace U2 {

class U2CORE_EXPORT UnloadedObjectInfo {
public:
    UnloadedObjectInfo(GObject* obj = NULL);
    QString         name;
    GObjectType     type;
    QVariantMap     hints;
    U2EntityRef     entityRef;

    bool isValid() const {return !name.isEmpty() && !type.isEmpty() && type != GObjectTypes::UNLOADED;}
};

class U2CORE_EXPORT UnloadedObject: public GObject {
    Q_OBJECT
public:
    UnloadedObject(const QString& objectName, const GObjectType& loadedObjectType,
        const U2EntityRef &entityRef, const QVariantMap& hintsMap = QVariantMap());
    UnloadedObject(const UnloadedObjectInfo& info);

    virtual GObject* clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

    GObjectType getLoadedObjectType() const {return loadedObjectType;}
    void setLoadedObjectType(const GObjectType& lot);

protected:
    GObjectType loadedObjectType;
};

}//namespace

#endif
