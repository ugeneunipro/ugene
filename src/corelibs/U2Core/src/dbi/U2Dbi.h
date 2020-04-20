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

#ifndef _U2_DBI_H_
#define _U2_DBI_H_

#include <U2Core/U2FormatCheckResult.h>
#include <U2Core/U2Feature.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2Msa.h>
#include <U2Core/GUrl.h>

#include <QHash>
#include <QSet>

class QMutex;

namespace U2 {

// For the classes below, see description in class definition
class U2AssemblyDbi;
class U2AttributeDbi;
class U2CrossDatabaseReferenceDbi;
class U2Dbi;
class U2FeatureDbi;
class U2ModDbi;
class U2MsaDbi;
class U2ObjectDbi;
class U2ObjectRelationsDbi;
class U2OpStatus;
class U2SequenceDbi;
class U2VariantDbi;
class UdrDbi;
class Version;

class U2CORE_EXPORT U2DbiOptions {
public:
    /** Application version number that is supposed to be minimum version required to make use of the database */
    static const QString APP_MIN_COMPATIBLE_VERSION;

    /** A constant to retrieve all available data. */
    static const int U2_DBI_NO_LIMIT;

    /** Init time DBI parameter name to specify URL of the database */
    static const QString U2_DBI_OPTION_URL;

    /** Init time DBI parameter name to specify that database must be created if not exists */
    static const QString U2_DBI_OPTION_CREATE;

    /** Init time DBI parameter name to specify user password for the database. */
    static const QString U2_DBI_OPTION_PASSWORD;

    /** Init time DBI parameter value. Indicates boolean 'Yes' or 'true'. */
    static const QString U2_DBI_VALUE_ON;

    /** SQLite only: "exclusive" (default) or "normal" mode. */
    static const QString U2_DBI_LOCKING_MODE;
};

/**
    Operational state of the database.
    */
enum U2DbiState {
    U2DbiState_Void = 1,
    U2DbiState_Starting = 2,
    U2DbiState_Ready = 3,
    U2DbiState_Stopping = 4
};

/**
    DBI feature flags
    */
enum U2DbiFeature {

    /** DBI supports sequence reading methods */
    U2DbiFeature_ReadSequence = 1,
    /** DBI supports MSA reading methods */
    U2DbiFeature_ReadMsa = 2,
    /** DBI supports Assembly reading methods */
    U2DbiFeature_ReadAssembly = 3,
    /** DBI supports sequence annotations reading methods */
    U2DbiFeature_ReadFeatures = 4,
    /** DBI supports read methods for attributes */
    U2DbiFeature_ReadAttributes = 5,
    /** DBI supports read methods for remote objects  */
    U2DbiFeature_ReadCrossDatabaseReferences = 6,
    /** DBI supports readings Variants and VariantTracks */
    U2DbiFeature_ReadVariant = 7,
    /** DBI supports readings of custom properties */
    U2DbiFeature_ReadProperties = 8,
    /** DBI supports reading of objects modification tracks */
    U2DbiFeature_ReadModifications = 9,
    /** DBI supports UDR reading methods */
    U2DbiFeature_ReadUdr = 10,
    /** DBI supports reading of object relations */
    U2DbiFeature_ReadRelations = 11,

    /** DBI supports changing/storing sequences */
    U2DbiFeature_WriteSequence = 101,
    /** DBI supports changing/storing multiple sequence alignments */
    U2DbiFeature_WriteMsa = 102,
    /** DBI supports changing/storing assemblies */
    U2DbiFeature_WriteAssembly = 103
    /** DBI supports changing/storing sequence annotations */,
    U2DbiFeature_WriteFeatures = 104,
    /** DBI supports changing/storing attributes */
    U2DbiFeature_WriteAttributes = 105,
    /** DBI supports cross database references */
    U2DbiFeature_WriteCrossDatabaseReferences = 106,
    /** DBI supports changing/storing Variants and VariantTracks */
    U2DbiFeature_WriteVariant = 107,
    /** DBI supports changing/storing custom properties */
    U2DbiFeature_WriteProperties = 108,
    /** DBI supports changing/storing of objects modification tracks */
    U2DbiFeature_WriteModifications = 109,
    /** DBI supports changing/storing UDR */
    U2DbiFeature_WriteUdr = 110,
    /** DBI supports writing of object relations */
    U2DbiFeature_WriteRelations = 111,

    /** DBI supports removal of objects */
    U2DbiFeature_RemoveObjects = 200,
    /** DBI supports set folder modification operations */
    U2DbiFeature_ChangeFolders = 201,

    /** DBI provides optimized algorithm for assembly reads packing */
    U2DbiFeature_AssemblyReadsPacking = 300,
    /** DBI provides optimized algorithm for coverage calculation */
    U2DbiFeature_AssemblyCoverageStat = 301,

    /** DBI provides optimized algorithm for sorting attributes */
    U2DbiFeature_AttributeSorting = 400,

    /** DBI supports undo/redo of changing operations */
    U2DbiFeature_UndoRedo = 500,

    U2DbiFeature_GlobalReadOnly = 600
};

/** Indicates object life-cycle and storage location */
enum U2DbiObjectRank {
    /** Object  is stored in this database and is top-level (included into some folder) */
    U2DbiObjectRank_TopLevel = 1,
    /** Object  is stored in this database and is not top-level, it is a child of another object */
    U2DbiObjectRank_Child = 2,
    /** Object  is stored in another database, see CrossDbiReference table for details */
    U2DbiObjectRank_Remote = 3
};

/**
    DBI factory provides functions to create new DBI instances
    and check file content to ensure that file is a valid database file
    */
class U2CORE_EXPORT U2DbiFactory {
public:
    U2DbiFactory();
    virtual ~U2DbiFactory();

    /** Creates new DBI instance */
    virtual U2Dbi *createDbi() = 0;

    /** Returns DBI type ID */
    virtual U2DbiFactoryId getId() const = 0;

    /**
        Checks that data pointed by properties is a valid DBI resource
        rawData param is used for compatibility with UGENE 1.x format detection
        and can be used by factory directly to check database header
        */
    virtual FormatCheckResult isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& os) const = 0;

    virtual GUrl id2Url(const U2DbiId& id) const = 0;

    virtual bool isDbiExists(const U2DbiId& id) const = 0;
};


/**
    Database access interface.
    Database examples: fasta file, genbank file, BAM file, SQLite file
    */
class U2CORE_EXPORT U2Dbi {
public:
    virtual ~U2Dbi();
    //////////////////////////////////////////////////////////////////////////
    // base methods that any DBI must support


    /**
        Boots the database up to functional state.
        Some property names are reserved:
        'url' - url of the DBI
        'create'  - '1' or '0'- if DBI does not exist, asks to create instance if possible
        'user', 'password' - user and password to access to the DBI
        The 'persistentData' parameter provides database state saved from previous session in a project or workspace, if any.

        @see shutdown()
        */
    virtual void init(const QHash<QString, QString>& properties, const QVariantMap& persistentData, U2OpStatus& os) = 0;

    /** Stops the database and frees up used resources.
    Returns persistent database state for external storage in a project or workspace, if any.
    For example, plain-file-based DBI can store certain settings or preferences between sessions,
    which may not fit into particular file format.

    Note: this method will call flush()
    @see init()
    */
    virtual QVariantMap shutdown(U2OpStatus& os) = 0;

    /**
        Ensures that dbi state is synchronized with storage
        Return 'true' of operation is successful
        */
    virtual bool flush(U2OpStatus& os) = 0;

    /**  Unique database id. Usually is an URL of the database */
    virtual U2DbiId getDbiId() const = 0;

    /** Return factory instance for this DBI */
    virtual U2DbiFactoryId getFactoryId() const = 0;

    virtual U2DbiRef getDbiRef() const;

    /** Returning result specifies whether the database internal structure is ready for work,
        namely, the database schema is created correctly.
        */
    virtual bool isInitialized(U2OpStatus &);

    /** Performs initialization of the database schema. The database is supposed to be available
        for use after this function returns if @os has no error.
        */
    virtual void populateDefaultSchema(U2OpStatus &os);

    /** Returns all features supported by this DBI instance */
    virtual const QSet<U2DbiFeature>& getFeatures() const = 0;

    /** Returns properties used to initialize the database */
    virtual QHash<QString, QString> getInitProperties() const = 0;

    /** Returns database meta-info. Any set of properties to be shown to user */
    virtual QHash<QString, QString> getDbiMetaInfo(U2OpStatus&) = 0;

    /** Returns type of the entity referenced by the given ID */
    virtual U2DataType getEntityTypeById(const U2DataId& id) const = 0;

    /** Returns current DBI state */
    virtual U2DbiState getState() const = 0;

    /**
        Database interface to access objects
        All dbi implementations must support a subset of this interface
        */
    virtual U2ObjectDbi* getObjectDbi() = 0;

    /**
        Database interface to access object relations.
        */
    virtual U2ObjectRelationsDbi* getObjectRelationsDbi();

    /**
        U2Sequence related DBI routines
        Not NULL only if U2DbiFeature_ReadSequences supported
        */
    virtual U2SequenceDbi* getSequenceDbi() = 0;

    /**
        U2Feature related DBI routines
        Not NULL only if U2DbiFeature_ReadFeatures supported
        */
    virtual U2FeatureDbi* getFeatureDbi() = 0;

    /**
        U2Annotation related DBI routines
        Not NULL only if U2DbiFeature_ReadMsa supported
        */
    virtual U2MsaDbi* getMsaDbi() = 0;

    /**
        U2Annotation related DBI routines
        Not NULL only if U2DbiFeature_ReadAssembly supported
        */
    virtual U2AssemblyDbi* getAssemblyDbi() = 0;

    /**
        U2Attribute related DBI routines
        Not NULL only if U2DbiFeature_ReadAttributes supported
        */
    virtual U2AttributeDbi* getAttributeDbi() = 0;

    /**
        U2Variant related DBI routines
        Not NULL only if U2DbiFeature_ReadVariants supported
        */
    virtual U2VariantDbi* getVariantDbi() = 0;

    /**
        Cross database references handling routines
        Not NULL only if U2DbiFeature_ReadCrossDatabaseReferences supported
        */
    virtual U2CrossDatabaseReferenceDbi* getCrossDatabaseReferenceDbi() = 0;

    /**
        U2Mod related DBI routines
        Not NULL only if U2DbiFeature_ReadModifications supported
        */
    virtual U2ModDbi* getModDbi() = 0;

    /**
        UdrRecord related DBI routines
        Not NULL only if U2DbiFeature_ReadUdr supported
        */
    virtual UdrDbi* getUdrDbi() = 0;

    /**
        Reads database global properties.
        Requires U2DbiFeature_ReadProperties support
        */
    virtual QString getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os) = 0;

    /**
        Sets database global properties.
        Requires U2DbiFeature_WriteProperties support
        */
    virtual void setProperty(const QString& name, const QString& value, U2OpStatus& os) = 0;

    /**
        Initializes execution of the block of operation through the one transaction
        */
    virtual void startOperationsBlock(U2OpStatus &os);

    virtual void stopOperationBlock(U2OpStatus &os);

    /** Returns a mutex that synchronizes access to internal database handler.
        This method is supposed to be used by caching DBIs
        in order to prevent cache inconsistency. */
    virtual QMutex * getDbMutex() const;

    virtual bool isReadOnly() const = 0;

    virtual bool isTransactionActive() const = 0;

protected:
    /** Stores to database the following properties:
        U2DbiOptions::APP_MIN_COMPATIBLE_VERSION */
    void setVersionProperties(const Version &minVersion, U2OpStatus &os);
};

/**
    Base class for all *Dbi classes provided by U2Dbi
    Contains reference to root-level dbi
    */
class U2CORE_EXPORT U2ChildDbi {
protected:
    U2ChildDbi(U2Dbi* _rootDbi);

public:
    virtual ~U2ChildDbi();

    U2Dbi* getRootDbi() const;

private:
    U2Dbi* rootDbi;
};

} //namespace

#endif
