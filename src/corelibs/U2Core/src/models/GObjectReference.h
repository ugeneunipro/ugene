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

#ifndef _U2_GOBJECT_REFERENCE_H_
#define _U2_GOBJECT_REFERENCE_H_

#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/U2Type.h>

#include <QDataStream>

namespace U2 {

class GObject;

class U2CORE_EXPORT GObjectReference {
public:
    GObjectReference(){}

    GObjectReference(const QString& _docUrl, const QString& _objName, const GObjectType& _type, const U2EntityRef& _entityRef = U2EntityRef())
        : docUrl(_docUrl), objName(_objName), entityRef(_entityRef), objType(_type) {}

    GObjectReference(const GObject* obj, bool deriveLoadedType = true);

    bool isValid() const {return !docUrl.isEmpty() && !objName.isEmpty() && !objType.isEmpty();}

    bool operator ==(const GObjectReference& o) const;

    bool operator <(const GObjectReference& o) const;

    /** GObject reference keeps only string path of the document url.
        This must be enough to find document in the project
        while allows not to keep a complete url data here (for example username/password, etc...)
    */
    QString         docUrl;

    /** The name of the object */
    QString         objName;

    /** Object reference to DB
     *  It can be empty
     */
    U2EntityRef     entityRef;

    /** The type of the object */
    GObjectType     objType;

private:
    static bool registerMeta;
};

inline uint qHash(const GObjectReference& key) {
    return ::qHash(key.docUrl) + ::qHash(key.objName) + ::qHash(key.objType);
}

inline uint qHash(const GObjectReference* key) {
    return ::qHash(key->docUrl) + ::qHash(key->objName) + ::qHash(key->objType);
}

//TODO: add constraints on relation roles

class U2CORE_EXPORT GObjectRelation {
public:
    GObjectRelation(){}

    GObjectRelation(const GObjectReference& _ref, const GObjectRelationRole& _role)
        : ref(_ref), role(_role) {}

    bool isValid() const {return ref.isValid();}

    bool operator ==(const GObjectRelation& o) const;

    const QString& getDocURL() const {return ref.docUrl;}

    GObjectReference ref;
    GObjectRelationRole role;
private:
    static bool registerMeta;
};

inline uint qHash(const GObjectRelation& key) {
    const uint h1 = qHash(key.ref);
    const uint h2 = ::qHash(static_cast<int>(key.role));
    return ((h1 << 16) | (h1 >> 16)) ^ h2;
}


QDataStream &operator<<(QDataStream &out, const GObjectReference &myObj);
QDataStream &operator>>(QDataStream &in, GObjectReference &myObj);
QDataStream &operator<<(QDataStream &out, const GObjectRelation &myObj);
QDataStream &operator>>(QDataStream &in, GObjectRelation &myObj);


}//namespace

Q_DECLARE_METATYPE( U2::GObjectReference )
Q_DECLARE_METATYPE( U2::GObjectRelation )
Q_DECLARE_METATYPE( QList< U2::GObjectReference > )
Q_DECLARE_METATYPE( QList< U2::GObjectRelation > )

#endif
