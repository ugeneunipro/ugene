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

#ifndef _U2_TYPE_H_
#define _U2_TYPE_H_

#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class U2Dbi;

/**
    UGENE built-in data-types
    Note: unsigned used to reveal all possible math ops with it.

    U2DataType - data types supported by U2Dbi
    U2DataId - object ids associated with Data by U2Dbi.
    Note that U2DataId == empty check must be suppoted by DBI to check empty fields.
*/
typedef quint16 U2DataType;
typedef QByteArray U2DataId;

/**
    ID of the DBI Factory. Defines type of the DBI driver.
    Examples: 'sqlite', 'mysql', 'fasta'....
*/
typedef QString U2DbiFactoryId;

/**
    ID of the DBI. Points to DBI instance inside of the given DBI factory
    Usually is an URL or the DBI file/resource
*/

typedef QString U2DbiId;


/**
    Built in types
    Note: Maximum value used for type must be <=4096
*/

class U2CORE_EXPORT U2Type {
public:
    /** Type is unknown. Default value. */
    static const U2DataType Unknown                     = 0;

    /** Object types */
    static const U2DataType Sequence                    = 1;
    static const U2DataType Msa                         = 2;
    static const U2DataType Assembly                    = 4;
    static const U2DataType VariantTrack                = 5;
    static const U2DataType VariantType                 = 6;
    static const U2DataType FilterTableType             = 7;
    static const U2DataType KnownMutationsTrackType     = 8;
    static const U2DataType AnnotationTable             = 10;
    static const U2DataType Mca                         = 11;
    /** UDR object types 101..199 */
    static const U2DataType Text                        = 101;
    static const U2DataType PhyTree                     = 102;
    static const U2DataType Chromatogram                = 103;
    static const U2DataType BioStruct3D                 = 104;
    static const U2DataType PFMatrix                    = 105;
    static const U2DataType PWMatrix                    = 106;

    static const U2DataType CrossDatabaseReference      = 999;

    /** SCO (non-object, non-root) types */
    static const U2DataType Annotation                  = 1000;
    static const U2DataType AnnotationGroup             = 1001;

    /**  Assembly read */
    static const U2DataType AssemblyRead                = 1100;

    /** Sequence feature */
    static const U2DataType Feature                     = 1300;

    static const U2DataType UdrRecord                   = 1400;

    /**  Attribute types */
    static const U2DataType AttributeInteger            = 2001;
    static const U2DataType AttributeReal               = 2002;
    static const U2DataType AttributeString             = 2003;
    static const U2DataType AttributeByteArray          = 2004;

    static bool isObjectType(U2DataType type) {return type > 0 && type <= 999;}
    static bool isUdrObjectType(U2DataType type) {return type > 100 && type < 200;}

    static bool isAttributeType(U2DataType type) {return type >=2000 && type < 2100;}

};

enum U2TrackModType {
    NoTrack = 0,
    TrackOnUpdate = 1,
    TRACK_MOD_TYPE_NR_ITEMS
};

/** Dbi reference: dbiURL & DBI type */
class U2CORE_EXPORT U2DbiRef {
public:
    U2DbiRef(const U2DbiFactoryId &dbiFactoryId = U2DbiFactoryId(), const U2DbiId &dbiId = U2DbiId());

    bool isValid() const;

    bool operator!=(const U2DbiRef &r2) const;
    bool operator==(const U2DbiRef &r2) const;
    bool operator <(const U2DbiRef &r2) const;

    U2DbiFactoryId  dbiFactoryId;
    U2DbiId         dbiId;

private:
    static bool metaInfoRegistered;
};

QDataStream & operator <<(QDataStream &out, const U2DbiRef &dbiRef);
QDataStream & operator >>(QDataStream &in, U2DbiRef &dbiRef);

/**
    Cross database data reference
*/
class U2CORE_EXPORT U2EntityRef {
public:
                    U2EntityRef();
                    U2EntityRef(const U2DbiRef &dbiRef, const U2DataId &entityId);
    bool            isValid() const;

    bool            operator ==(const U2EntityRef &other) const;
    bool            operator !=(const U2EntityRef &other) const;
    bool            operator <(const U2EntityRef &other) const;

    /** database  id */
    U2DbiRef         dbiRef;

    /** DB local data reference */
    U2DataId        entityId;

    /** Object version number this reference is valid for */
    qint64          version;
};

/**
    Base class for all data types that can be referenced by some ID
*/
class U2CORE_EXPORT U2Entity {
public:
                    U2Entity(const U2DataId &id = U2DataId());
                    U2Entity(const U2Entity &other);

    virtual         ~U2Entity();

    bool            hasValidId() const;

    U2Entity        operator =(const U2Entity &other);

    bool            operator ==(const U2Entity &other) const;
    bool            operator !=(const U2Entity &other) const;
    bool            operator <(const U2Entity &other) const;

    U2DataId        id;
};


/**
    Base marker class for all First-class-objects stored in the database
*/
class U2CORE_EXPORT U2Object : public U2Entity {
public:
    U2Object() : version(0), trackModType(NoTrack) {}
    U2Object(U2DataId id, const U2DbiId& _dbId, qint64 v) : U2Entity(id), dbiId(_dbId), version(v), trackModType(NoTrack) {}

    /** Source of the object: database id */
    U2DbiId         dbiId;

    /** Version of the object. Same as modification count of the object */
    qint64          version;

    /** The name of the object shown to user. Any reasonably short text */
    QString         visualName;

    /** Specifies whether modifications of the object must be tracked or not */
    U2TrackModType  trackModType;

    /** The type of the object. Should be overridden in subclasses */
    virtual U2DataType getType() const { return U2Type::Unknown; }
};

inline uint qHash(const U2Object &obj) {
    return qHash(QPair<U2DataId, U2DbiId>(obj.id, obj.dbiId));
}

/**
    If database keeps annotations/attributes for data entity stored in another database
    U2CrossDatabaseReference used as a parent object for all local data
*/
class U2CORE_EXPORT U2CrossDatabaseReference : public U2Object {
public:
    U2CrossDatabaseReference() {}
    U2CrossDatabaseReference(U2DataId id, QString dbId, qint64 version) : U2Object(id, dbId, version) {}


    // remote data element id;
    U2EntityRef   dataRef;

    U2DataType getType() const { return U2Type::CrossDatabaseReference; }
};


/** Template class for DBI iterators */
template<class T> class U2DbiIterator {
public:
    virtual ~U2DbiIterator(){}

    /** returns true if there are more reads to iterate*/
    virtual bool hasNext() = 0;

    /** returns next read and shifts one element*/
    virtual T next() = 0;

    /** returns next read without shifting*/
    virtual T peek() = 0;
};

class U2Strand {
public:
    enum Direction {
        Direct = 1,
        Complementary = -1
    };

    U2Strand() : value(Direct) { }
    U2Strand(Direction val) : value(val) { }

    bool isDirect() const { return value == Direct; }

    bool isCompementary() const { return value == Complementary; }

    bool operator==(const U2Strand &s) const { return value == s.value; }

    bool operator!=(const U2Strand &s) const { return value != s.value; }

    Direction getDirection() const { return static_cast<U2Strand::Direction>(value); }

    int getDirectionValue() const { return value; }

private:
    int value;
};

} //namespace

Q_DECLARE_METATYPE(QList<U2::U2DataId>)
Q_DECLARE_METATYPE(U2::U2DbiRef)

#endif
