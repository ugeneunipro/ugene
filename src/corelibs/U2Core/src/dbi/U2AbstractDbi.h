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

/**
    Default DBI implementations
*/

#ifndef _U2_ABSTRACT_DBI_H_
#define _U2_ABSTRACT_DBI_H_

#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2CrossDatabaseReferenceDbi.h>
#include <U2Core/U2DbiUpgrader.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/U2ModDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2VariantDbi.h>
#include <U2Core/UdrDbi.h>

namespace U2 {

/** Default (empty) implementation for optional DBI methods */
class U2AbstractDbi : public U2Dbi {
protected:
    U2AbstractDbi(const U2DbiFactoryId& fid) {
        state = U2DbiState_Void;
        factoryId = fid;
    }

    ~U2AbstractDbi() {
        qDeleteAll(upgraders);
    }

public:
    virtual bool flush(U2OpStatus&) {return true;}

    virtual U2DbiState getState() const {return state;}

    virtual U2DbiId getDbiId() const {return dbiId;}

    virtual U2DbiFactoryId getFactoryId() const  {return factoryId;}

    virtual const QSet<U2DbiFeature>& getFeatures() const {return features;}

    virtual QHash<QString, QString> getInitProperties() const {return initProperties;}

    virtual QHash<QString, QString> getDbiMetaInfo(U2OpStatus&) {return metaInfo;}

    virtual U2SequenceDbi* getSequenceDbi() {return NULL;}

    virtual U2FeatureDbi* getFeatureDbi() {return NULL;}

    virtual U2MsaDbi* getMsaDbi() {return NULL;}

    virtual U2AssemblyDbi* getAssemblyDbi() {return NULL;}

    virtual U2AttributeDbi* getAttributeDbi()  {return NULL;}

    virtual U2ObjectDbi* getObjectDbi()  {return NULL;}

    virtual U2ObjectRelationsDbi* getObjectRelationsDbi() {return NULL;}

    virtual U2VariantDbi* getVariantDbi()  {return NULL;}

    virtual U2ModDbi* getModDbi()  {return NULL;}

    virtual U2CrossDatabaseReferenceDbi* getCrossDatabaseReferenceDbi()  {return NULL;}

    virtual UdrDbi* getUdrDbi() {return NULL;}

    virtual U2DataType getEntityTypeById(const U2DataId&) const {return U2Type::Unknown;}

    virtual QString getProperty(const QString&, const QString& defaultValue, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ReadProperties, this, os);
        return defaultValue;
    }

    virtual void setProperty(const QString&, const QString&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteProperties, this, os);
    }

    virtual void upgrade(U2OpStatus &os) {
        qSort(upgraders);
        foreach (const U2DbiUpgrader *upgrader, upgraders) {
            if (upgrader->isAppliable(Version::parseVersion(getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "0.0.0", os)))) {
                upgrader->upgrade(os);
                CHECK_OP(os, );
            }
        }
    }

    bool isTransactionActive() const { return false; }

protected:
    U2DbiState                  state;
    U2DbiId                     dbiId;
    U2DbiFactoryId              factoryId;
    QSet<U2DbiFeature>          features;
    QHash<QString, QString>     initProperties;
    QHash<QString, QString>     metaInfo;
    QList<U2DbiUpgrader *>      upgraders;
};

/** Default no-op implementation for write  methods of U2ObjectDbi */
class U2SimpleObjectDbi : public U2ObjectDbi {
protected:
    U2SimpleObjectDbi(U2Dbi* rootDbi) : U2ObjectDbi(rootDbi) {}

public:
    virtual void getObject(U2Object&, const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
    }

    virtual U2DataId getObject(qint64 objectId, U2OpStatus& os) {
        Q_UNUSED(objectId)
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return U2DataId();
    }

    virtual QHash<U2Object, QString> getObjectFolders(U2OpStatus &os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return QHash<U2Object, QString>();
    }

    virtual QStringList getObjectFolders(const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return QStringList();
    }

    virtual bool removeObject(const U2DataId&, bool, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return false;
    }

    virtual bool removeObjects(const QList<U2DataId>&, bool, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return false;
    }

    virtual void createFolder(const QString&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    virtual bool removeFolder(const QString&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return false;
    }

    virtual void renameFolder(const QString &, const QString &, U2OpStatus &os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    virtual QString getFolderPreviousPath(const QString &, U2OpStatus &os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return "";
    }

    virtual void addObjectsToFolder(const QList<U2DataId>&, const QString&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    virtual void moveObjects(const QList<U2DataId>&, const QString&, const QString&, U2OpStatus& os, bool) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    virtual QStringList restoreObjects(const QList<U2DataId> &, U2OpStatus &os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return QStringList();
    }

    virtual U2TrackModType getTrackModType(const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_ReadModifications, getRootDbi(), os);
        return NoTrack;
    }

    virtual void setTrackModType(const U2DataId&, U2TrackModType, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteModifications, getRootDbi(), os);
    }

    virtual void undo(const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
    }

    virtual void redo(const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
    }

    virtual bool canUndo(const U2DataId& /*msaId*/, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
        return false;
    }

    virtual bool canRedo(const U2DataId& /*objId*/, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
        return false;
    }
};

/** Default no-op implementation for write  methods of U2AssemblyDbi */
class U2SimpleAssemblyDbi: public U2AssemblyDbi{
protected:
    U2SimpleAssemblyDbi(U2Dbi* rootDbi) : U2AssemblyDbi(rootDbi) {}

public:
    virtual void createAssemblyObject(U2Assembly&, const QString&,  U2DbiIterator<U2AssemblyRead>*, U2AssemblyReadsImportInfo&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    virtual void finalizeAssemblyObject(U2Assembly & /*assembly*/, U2OpStatus &os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
        }

    virtual void removeAssemblyData(const U2DataId&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    virtual void updateAssemblyObject(U2Assembly&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    virtual void removeReads(const U2DataId&, const QList<U2DataId>&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    virtual void addReads(const U2DataId&, U2DbiIterator<U2AssemblyRead>*, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    virtual void pack(const U2DataId&, U2AssemblyPackStat&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_AssemblyReadsPacking, getRootDbi(), os);
    }

    virtual void calculateCoverage(const U2DataId&, const U2Region&, U2AssemblyCoverageStat&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_AssemblyCoverageStat, getRootDbi(), os);
    }
};

/** Default no-op implementation for write  methods of U2AttributeDbi */
class U2SimpleAttributeDbi: public U2AttributeDbi {
protected:
    U2SimpleAttributeDbi(U2Dbi* rootDbi) : U2AttributeDbi(rootDbi) {}

public:
    virtual void removeAttributes(const QList<U2DataId>&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    virtual void removeObjectAttributes(const U2DataId&, U2OpStatus& os)  {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    virtual void createIntegerAttribute(U2IntegerAttribute&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    virtual void createRealAttribute(U2RealAttribute&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    virtual void createStringAttribute(U2StringAttribute&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    virtual void createByteArrayAttribute(U2ByteArrayAttribute&, U2OpStatus& os) {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }
};

}//namespace

#endif
