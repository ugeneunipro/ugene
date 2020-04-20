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

#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/U2SafePoints.h>

#include "GObjectReference.h"

namespace U2 {

GObjectReference::GObjectReference(const GObject* obj, bool deriveLoadedType) {
    SAFE_POINT(obj!=NULL && obj->getDocument()!=NULL, "GObjectReference:: no object and annotation", );
    docUrl = obj->getDocument()->getURLString();
    objName = obj->getGObjectName();
    entityRef = obj->getEntityRef();
    if (obj->isUnloaded() && deriveLoadedType) {
        const UnloadedObject* uo = qobject_cast<const UnloadedObject*>(obj);
        SAFE_POINT(uo!=NULL, "GObjectReference:: cannot cast UnloadedObject", );
        objType = uo->getLoadedObjectType();
    } else {
        objType = obj->getGObjectType();
    }
}

bool GObjectReference::operator ==(const GObjectReference& r) const {
    return objName == r.objName && docUrl == r.docUrl && objType == r.objType
        && (!r.entityRef.isValid() || !entityRef.isValid() || r.entityRef == entityRef);
}

bool GObjectRelation::operator ==(const GObjectRelation& r) const {
    return ref == r.ref && role == r.role;
}

bool GObjectReference::operator < (const GObjectReference& r) const {
    return U2::qHash(this) < U2::qHash(r);
}

QDataStream &operator<<(QDataStream &out, const GObjectReference &myObj) {
    out << myObj.docUrl << myObj.objName << myObj.objType;
    return out;
}

QDataStream &operator>>(QDataStream &in, GObjectReference &myObj) {
    in >> myObj.docUrl;
    in >> myObj.objName;
    in >> myObj.objType;
    return in;
}

QDataStream &operator<<(QDataStream &out, const GObjectRelation &myObj) {
    QString data; // for compatibility
    out << myObj.ref << GObjectRelationRoleCompatibility::toString(myObj.role) << data;
    return out;
}

QDataStream &operator>>(QDataStream &in, GObjectRelation &myObj) {
    QString roleString;
    QString data; // for compatibility
    in >> myObj.ref;
    in >> roleString;
    in >> data;
    myObj.role = GObjectRelationRoleCompatibility::fromString(roleString);
    return in;
}

static bool registerMetas1()  {
    qRegisterMetaType<GObjectReference>("GObjectReference");
    qRegisterMetaTypeStreamOperators<GObjectReference>("U2::GObjectReference");

    qRegisterMetaType<QList<GObjectReference> >("QList<U2::GObjectReference>");
    qRegisterMetaTypeStreamOperators< QList<GObjectReference> >("QList<U2::GObjectReference>");

    return true;
}

static bool registerMetas2()  {
    qRegisterMetaType<GObjectRelation>("GObjectRelation");
    qRegisterMetaTypeStreamOperators<GObjectRelation>("U2::GObjectRelation");

    qRegisterMetaType<QList<GObjectRelation> >("QList<U2::GObjectRelation>");
    qRegisterMetaTypeStreamOperators< QList<GObjectRelation> >("QList<U2::GObjectRelation>");

    return true;
}


bool GObjectReference::registerMeta  = registerMetas1();
bool GObjectRelation::registerMeta  = registerMetas2();

}   // namespace U2
