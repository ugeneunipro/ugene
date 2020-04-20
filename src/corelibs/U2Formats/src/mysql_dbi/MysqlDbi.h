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

#ifndef _U2_MYSQL_DBI_H_
#define _U2_MYSQL_DBI_H_

#include <QStack>

#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2DbiRegistry.h>

namespace U2 {

class MysqlAttributeDbi;
class MysqlAssemblyDbi;
class MysqlCrossDatabaseReferenceDbi;
class MysqlDbRef;
class MysqlFeatureDbi;
class MysqlModDbi;
class MysqlMsaDbi;
class MysqlObjectDbi;
class MysqlObjectRelationsDbi;
class MysqlSequenceDbi;
class MysqlTransaction;
class MysqlUdrDbi;
class MysqlVariantDbi;

class U2FORMATS_EXPORT MysqlDbi : public U2AbstractDbi {
public:
    MysqlDbi();
    ~MysqlDbi();

    /**
    Boots the database up to functional state.
    Can be called again after successful shutdown, to re-load the database.
    */
    virtual void init(const QHash<QString, QString>& properties, const QVariantMap& persistentData, U2OpStatus& os);

    /** Stops the database and frees up used resources. */
    virtual QVariantMap shutdown(U2OpStatus& os);

    /**
    Ensures that dbi state is synchronized with storage
    Return 'true' of operation is successful
    */
    virtual bool flush(U2OpStatus& os);

    /** Unique database id. Used for cross-database references. */
    virtual QString getDbiId() const;

    virtual bool isInitialized(U2OpStatus &os);

    virtual void populateDefaultSchema(U2OpStatus& os);

    /** Returns database meta-info. Any set of properties to be shown to user */
    virtual QHash<QString, QString> getDbiMetaInfo(U2OpStatus& os);

    /** Returns type of the entity referenced by the given ID */
    virtual U2DataType getEntityTypeById(const U2DataId& id) const;

    virtual U2AssemblyDbi*                  getAssemblyDbi();
    virtual U2AttributeDbi*                 getAttributeDbi();
    virtual U2CrossDatabaseReferenceDbi*    getCrossDatabaseReferenceDbi();
    virtual U2FeatureDbi*                   getFeatureDbi();
    virtual U2ModDbi*                       getModDbi();
    virtual U2MsaDbi*                       getMsaDbi();
    virtual U2ObjectDbi*                    getObjectDbi();
    virtual U2ObjectRelationsDbi*           getObjectRelationsDbi();
    virtual U2SequenceDbi*                  getSequenceDbi();
    virtual UdrDbi*                         getUdrDbi();
    virtual U2VariantDbi*                   getVariantDbi();

    MysqlAssemblyDbi*                       getMysqlAssemblyDbi();
    MysqlAttributeDbi*                      getMysqlAttributeDbi();
    MysqlCrossDatabaseReferenceDbi*         getMysqlCrossDatabaseReferenceDbi();
    MysqlFeatureDbi*                        getMysqlFeatureDbi();
    MysqlModDbi*                            getMysqlModDbi();
    MysqlMsaDbi*                            getMysqlMsaDbi();
    MysqlObjectDbi*                         getMysqlObjectDbi();
    MysqlSequenceDbi*                       getMysqlSequenceDbi();
    MysqlUdrDbi*                            getMysqlUdrDbi();
    MysqlVariantDbi*                        getMysqlVariantDbi();

    MysqlDbRef* getDbRef();

    /** Returns properties used to initialized the database */
    virtual QHash<QString, QString> getInitProperties() const {return initProperties;}

    virtual QString getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os);

    virtual void setProperty(const QString& name, const QString& value, U2OpStatus& os);

    virtual void startOperationsBlock(U2OpStatus& os);

    virtual void stopOperationBlock(U2OpStatus &os);

    virtual bool isReadOnly() const;

    bool isTransactionActive() const;

    static const int BIND_PARAMETERS_LIMIT;

private:
    void createHandle(const QHash<QString, QString>& props);
    void open(const QHash<QString, QString>& props, U2OpStatus& os);

    void setState(U2DbiState state);

    void internalInit(const QHash<QString, QString>& props, U2OpStatus& os);

    void setupProperties(const QHash<QString, QString>& props, U2OpStatus& os);

    void checkVersion(U2OpStatus &os);

    void setupFeatures();

    void setupTransactions(U2OpStatus& os);

    void checkUserPermissions(U2OpStatus& os);

    static QString getConnectionName();

    MysqlDbRef*                        db;

    MysqlAssemblyDbi*                  assemblyDbi;
    MysqlAttributeDbi*                 attributeDbi;
    MysqlCrossDatabaseReferenceDbi*    crossDbi;
    MysqlFeatureDbi*                   featureDbi;
    MysqlModDbi*                       modDbi;
    MysqlMsaDbi*                       msaDbi;
    MysqlObjectDbi*                    objectDbi;
    MysqlObjectRelationsDbi*           objectRelationsDbi;
    MysqlSequenceDbi*                  sequenceDbi;
    MysqlUdrDbi*                       udrDbi;
    MysqlVariantDbi*                   variantDbi;

    QStack<MysqlTransaction*>          operationsBlockTransactions;

    bool                               tablesAreCreated;
    QString                            minCompatibleAppVersion;
};

class U2FORMATS_EXPORT MysqlDbiFactory : public U2DbiFactory {
public:
    MysqlDbiFactory();

    /** Creates new DBI instance */
    virtual U2Dbi *createDbi();

    /** Returns DBI type ID */
    virtual U2DbiFactoryId getId() const;

    /** Checks that data pointed by properties is a valid DBI resource */
    virtual FormatCheckResult isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& os) const;

    virtual GUrl id2Url(const U2DbiId& id) const {return GUrl(id, GUrl_Network);}

    virtual bool isDbiExists(const U2DbiId& id) const;

public:
    static const U2DbiFactoryId ID;
};

/** helper class, used as a base for all SQLite<child>Dbis */
class MysqlChildDbiCommon {
public:
    MysqlChildDbiCommon(MysqlDbi* dbi);
    virtual ~MysqlChildDbiCommon() {}

    virtual void initSqlSchema(U2OpStatus& os) = 0;
    virtual void shutdown(U2OpStatus&) {}

protected:
    MysqlDbi*   dbi;
    MysqlDbRef* db;
};

}   // namespace U2

#endif // _U2_MYSQL_DBI_H_
