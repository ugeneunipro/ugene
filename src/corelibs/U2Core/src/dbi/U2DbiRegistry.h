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

#ifndef _U2_DBI_REGISTRY_H_
#define _U2_DBI_REGISTRY_H_

#include <U2Core/U2Dbi.h>

#include <QHash>
#include <QMutex>
#include <QTimer>

namespace U2 {

class DbiConnection;
class U2DbiPool;

#define SQLITE_DBI_ID "SQLiteDbi"
#define MYSQL_DBI_ID "MysqlDbi"
#define BAM_DBI_ID "SamtoolsBasedDbi"
#define DEFAULT_DBI_ID SQLITE_DBI_ID
#define WORKFLOW_SESSION_TMP_DBI_ALIAS "workflow_session"


/**
    Keep all DBI types registered in the system
*/
class U2CORE_EXPORT U2DbiRegistry : public QObject {
    Q_OBJECT
    class TmpDbiRef {
    public:
        TmpDbiRef(const QString& _alias = QString(), const U2DbiRef& _dbiRef = U2DbiRef(), int _nUsers = 0)
            : alias(_alias), dbiRef(_dbiRef), nUsers(_nUsers){}

        QString     alias;
        U2DbiRef    dbiRef;
        int         nUsers;
    };
public:
    U2DbiRegistry(QObject *parent = NULL);
    ~U2DbiRegistry();

    virtual bool registerDbiFactory(U2DbiFactory *factory);

    virtual QList<U2DbiFactoryId> getRegisteredDbiFactories() const;

    virtual U2DbiFactory *getDbiFactoryById(const U2DbiFactoryId &id) const;

    U2DbiPool* getGlobalDbiPool() const {return pool;}

    /**
    * Increases the "number of users"-counter for the dbi, if it exists.
    * Otherwise, allocates the dbi and sets the counter to 1.
    */
    U2DbiRef attachTmpDbi(const QString &alias, U2OpStatus &os, const U2DbiFactoryId &factoryId);

    /**
    * Decreases the "number of users"-counter.
    * Deallocates the dbi, if it becomes equal to 0.
    */
    void detachTmpDbi(const QString& alias, U2OpStatus& os);

    QList<U2DbiRef> listTmpDbis() const;

    /**
    * Returns the reference to the tmp session dbi.
    * If the last is not created yet then it would be created.
    */
    U2DbiRef getSessionTmpDbiRef(U2OpStatus &os);

    /**
    * WARNING: must be used only in crash handler.
    * Closes the session database connection and returns the path to the database file
    */
    QString shutdownSessionDbi(U2OpStatus &os);

private:
    /** Creates the session connection and increases the counter for the dbi */
    void initSessionDbi(TmpDbiRef& tmpDbiRef);

    U2DbiRef allocateTmpDbi(const QString& alias, U2OpStatus& os, const U2DbiFactoryId &factoryId);

    void deallocateTmpDbi(const TmpDbiRef& ref, U2OpStatus& os);

    QHash<U2DbiFactoryId, U2DbiFactory *>   factories;
    U2DbiPool*                              pool;
    QList<TmpDbiRef>                        tmpDbis;
    QMutex                                  lock;

    /** this connection is opened during the whole ugene session*/
    DbiConnection*                          sessionDbiConnection;
    bool                                    sessionDbiInitDone;
};

/**
    Class to access to DBI connections
    Roles:
        creates new DBIs on user request
        track connection live range (using ref counters)
        closes unused DBIs
*/
class U2CORE_EXPORT U2DbiPool : public QObject {
    Q_OBJECT
public:
    U2DbiPool(QObject* p = NULL);
    virtual ~U2DbiPool();

    U2Dbi* openDbi(const U2DbiRef& ref, bool create, U2OpStatus& os, const QHash<QString, QString> &properties = (QHash<QString, QString>()));
    void addRef(U2Dbi * dbi, U2OpStatus & os);
    void releaseDbi(U2Dbi* dbi, U2OpStatus& os);
    void closeAllConnections(const U2DbiRef& ref, U2OpStatus& os);

private slots:
    void sl_checkDbiPoolExpiration();

private:
    QStringList getIds(const U2DbiRef& ref, U2OpStatus &os) const;
    int getCountOfConnectionsInPool(const QString &url) const;
    U2Dbi * getDbiFromPool(const QString &id);
    void removeDbiRecordFromPool(const QString &id);
    void flushPool(const QString &url = QString(), bool removeAll = false);

    static QHash<QString, QString> getInitProperties(const QString& url, bool create);
    static QString getId(const U2DbiRef& ref, U2OpStatus &os);
    static bool isDbiFromMainThread(const QString &dbiId);
    static U2Dbi * createDbi(const U2DbiRef &ref, bool create, U2OpStatus &os, const QHash<QString, QString> &properties);
    static void deallocateDbi(U2Dbi *dbi, U2OpStatus &os);
    static QString id2Url(const QString &id);

    static const int DBI_POOL_EXPIRATION_TIME_MSEC;
    static const int MAX_CONNECTIONS_PER_DBI;

    QHash<QString, U2Dbi *> dbiById;
    QHash<QString, int> dbiCountersById;
    QHash<QString, U2Dbi *> suspendedDbis;
    QHash<U2Dbi *, qint64> dbiSuspendStartTime;
    QTimer expirationTimer;
    QMutex lock;
};

} // namespace U2

#endif // _U2_DBI_REGISTRY_H_
