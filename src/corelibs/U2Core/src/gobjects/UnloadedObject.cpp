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

#include <U2Core/GHints.h>
#include <U2Core/U2SafePoints.h>

#include "UnloadedObject.h"

namespace U2 {

UnloadedObject::UnloadedObject(const QString& objectName, const GObjectType& lot, const U2EntityRef &_entityRef, const QVariantMap& hintsMap)
    : GObject(GObjectTypes::UNLOADED, objectName, hintsMap)
{
    setLoadedObjectType(lot);
    entityRef = _entityRef;
}

UnloadedObject::UnloadedObject(const UnloadedObjectInfo& info)
    : GObject(GObjectTypes::UNLOADED, info.name, info.hints)
{
    setLoadedObjectType(info.type);
    entityRef = info.entityRef;
}

GObject* UnloadedObject::clone(const U2DbiRef &/*dstDbiRef*/, U2OpStatus &/*os*/, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    UnloadedObject* cln = new UnloadedObject(getGObjectName(), getLoadedObjectType(), getEntityRef(), gHints.getMap());
    cln->setIndexInfo(getIndexInfo());
    return cln;
}

void UnloadedObject::setLoadedObjectType(const GObjectType& lot) {
    SAFE_POINT(lot!=GObjectTypes::UNLOADED, "Unloaded object can't be a reference to another unloaded object!",);
    loadedObjectType = lot;
}

UnloadedObjectInfo::UnloadedObjectInfo(GObject* obj) {
    CHECK(NULL != obj, );

    name = obj->getGObjectName();
    hints = obj->getGHintsMap();
    entityRef = obj->getEntityRef();

    if (obj->isUnloaded()) {
        UnloadedObject* uo = qobject_cast<UnloadedObject*>(obj);
        type = uo->getLoadedObjectType();
    } else {
        type = obj->getGObjectType();
    }
}

}//namespace
