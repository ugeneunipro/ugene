/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

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

    ~U2AbstractDbi() override {
        qDeleteAll(upgraders);
    }

public:
    bool flush(U2OpStatus&) override {
        return true;
    }

    U2DbiState getState() const override {
        return state;
    }

    U2DbiId getDbiId() const override {
        return dbiId;
    }

    U2DbiFactoryId getFactoryId() const override {
        return factoryId;
    }

    const QSet<U2DbiFeature>& getFeatures() const override {
        return features;
    }

    QHash<QString, QString> getInitProperties() const override {
        return initProperties;
    }

    QHash<QString, QString> getDbiMetaInfo(U2OpStatus&) override {
        return metaInfo;
    }

    U2SequenceDbi* getSequenceDbi() override {
        return nullptr;
    }

    U2FeatureDbi* getFeatureDbi() override {
        return nullptr;
    }

    U2MsaDbi* getMsaDbi() override {
        return nullptr;
    }

    U2AssemblyDbi* getAssemblyDbi() override {
        return nullptr;
    }

    U2AttributeDbi* getAttributeDbi() override {
        return nullptr;
    }

    U2ObjectDbi* getObjectDbi() override {
        return nullptr;
    }

    U2ObjectRelationsDbi* getObjectRelationsDbi() override {
        return nullptr;
    }

    U2VariantDbi* getVariantDbi() override {
        return nullptr;
    }

    U2ModDbi* getModDbi() override {
        return nullptr;
    }

    U2CrossDatabaseReferenceDbi* getCrossDatabaseReferenceDbi() override {
        return nullptr;
    }

    UdrDbi* getUdrDbi() override {
        return nullptr;
    }

    U2DataType getEntityTypeById(const U2DataId&) const override {
        return U2Type::Unknown;
    }

    QString getProperty(const QString&, const QString& defaultValue, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ReadProperties, this, os);
        return defaultValue;
    }

    void setProperty(const QString&, const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteProperties, this, os);
    }

    virtual void upgrade(U2OpStatus& os) {
        std::sort(upgraders.begin(), upgraders.end());
        for (const U2DbiUpgrader* upgrader : qAsConst(upgraders)) {
            QString dbVersionString = getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "0.0.0", os);
            Version dbVersion = Version::parseVersion(dbVersionString);
            CHECK_OP(os, );
            if (upgrader->isApplicable(dbVersion)) {
                upgrader->upgrade(os);
                CHECK_OP(os, );
            }
        }
    }

    bool isTransactionActive() const override {
        return false;
    }

protected:
    U2DbiState state;
    U2DbiId dbiId;
    U2DbiFactoryId factoryId;
    QSet<U2DbiFeature> features;
    QHash<QString, QString> initProperties;
    QHash<QString, QString> metaInfo;
    QList<U2DbiUpgrader*> upgraders;
};

/** Default no-op implementation for write  methods of U2ObjectDbi */
class U2SimpleObjectDbi : public U2ObjectDbi {
protected:
    U2SimpleObjectDbi(U2Dbi* rootDbi)
        : U2ObjectDbi(rootDbi) {
    }

public:
    U2DataType getObject(U2Object&, const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return {};
    }

    U2DataId getObject(qint64 objectId, U2OpStatus& os) override {
        Q_UNUSED(objectId)
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return U2DataId();
    }

    QHash<U2Object, QString> getObjectFolders(U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return QHash<U2Object, QString>();
    }

    QStringList getObjectFolders(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return QStringList();
    }

    bool removeObject(const U2DataId&, bool, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return false;
    }

    bool removeObjects(const QList<U2DataId>&, bool, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_RemoveObjects, getRootDbi(), os);
        return false;
    }

    void createFolder(const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    bool removeFolder(const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return false;
    }

    void renameFolder(const QString&, const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    QString getFolderPreviousPath(const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return "";
    }

    void addObjectsToFolder(const QList<U2DataId>&, const QString&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    void moveObjects(const QList<U2DataId>&, const QString&, const QString&, U2OpStatus& os, bool) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
    }

    QStringList restoreObjects(const QList<U2DataId>&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ChangeFolders, getRootDbi(), os);
        return QStringList();
    }

    U2TrackModType getTrackModType(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_ReadModifications, getRootDbi(), os);
        return NoTrack;
    }

    void setTrackModType(const U2DataId&, U2TrackModType, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteModifications, getRootDbi(), os);
    }

    void undo(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
    }

    void redo(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
    }

    bool canUndo(const U2DataId& /*msaId*/, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
        return false;
    }

    bool canRedo(const U2DataId& /*objId*/, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_UndoRedo, getRootDbi(), os);
        return false;
    }
};

/** Default no-op implementation for write  methods of U2AssemblyDbi */
class U2SimpleAssemblyDbi : public U2AssemblyDbi {
protected:
    U2SimpleAssemblyDbi(U2Dbi* rootDbi)
        : U2AssemblyDbi(rootDbi) {
    }

public:
    void createAssemblyObject(U2Assembly&, const QString&, U2DbiIterator<U2AssemblyRead>*, U2AssemblyReadsImportInfo&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void finalizeAssemblyObject(U2Assembly& /*assembly*/, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void removeAssemblyData(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void updateAssemblyObject(U2Assembly&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void removeReads(const U2DataId&, const QList<U2DataId>&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void addReads(const U2DataId&, U2DbiIterator<U2AssemblyRead>*, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAssembly, getRootDbi(), os);
    }

    void pack(const U2DataId&, U2AssemblyPackStat&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_AssemblyReadsPacking, getRootDbi(), os);
    }

    void calculateCoverage(const U2DataId&, const U2Region&, U2AssemblyCoverageStat&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_AssemblyCoverageStat, getRootDbi(), os);
    }
};

/** Default no-op implementation for write  methods of U2AttributeDbi */
class U2SimpleAttributeDbi : public U2AttributeDbi {
protected:
    U2SimpleAttributeDbi(U2Dbi* rootDbi)
        : U2AttributeDbi(rootDbi) {
    }

public:
    void removeAttributes(const QList<U2DataId>&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void removeObjectAttributes(const U2DataId&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void createIntegerAttribute(U2IntegerAttribute&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void createRealAttribute(U2RealAttribute&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void createStringAttribute(U2StringAttribute&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void createByteArrayAttribute(U2ByteArrayAttribute&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }

    void updateByteArrayAttribute(U2ByteArrayAttribute&, U2OpStatus& os) override {
        U2DbiUtils::logNotSupported(U2DbiFeature_WriteAttributes, getRootDbi(), os);
    }
};

}  // namespace U2
