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

#include <cmath>

#include <QBitArray>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "U2DbiUtils.h"

namespace U2 {

#define DB_ID_OFFSET    0
#define TYPE_OFFSET     8
#define DB_EXTRA_OFFSET 10
#define DATAID_MIN_LEN  10

static U2DataId emptyId;

const QString U2DbiUtils::PUBLIC_DATABASE_NAME = "UGENE public database";
const QString U2DbiUtils::PUBLIC_DATABASE_LOGIN = "public";
const QString U2DbiUtils::PUBLIC_DATABASE_PASSWORD = "public";
const QString U2DbiUtils::PUBLIC_DATABASE_URL = U2DbiUtils::createFullDbiUrl(PUBLIC_DATABASE_LOGIN, "5.9.139.103", 3306, "public_ugene_1_25");

void U2DbiUtils::logNotSupported(U2DbiFeature f, U2Dbi* dbi, U2OpStatus& os) {
    QString msg = tr("Feature is not supported: %1, dbi: %2").arg(int(f)).arg(dbi == NULL ? QString("<unknown>") : dbi->getDbiId());
    coreLog.error(msg);
    if (!os.hasError()) {
        os.setError(msg);
    }

#ifdef _DEBUG
    if (dbi->getFeatures().contains(f)) {
        coreLog.error("Calling not-supported method while features is listed in supported features list!");
    }
    assert(0);
#endif
}

U2DbiRef U2DbiUtils::toRef(U2Dbi* dbi) {
    if (dbi == NULL) {
        return U2DbiRef();
    }
    return U2DbiRef(dbi->getFactoryId(), dbi->getDbiId());
}

void U2DbiUtils::addLimit(QString& sql, qint64 offset, qint64 count) {
    if (count == -1) {
        return;
    }
    sql = sql + QString(" LIMIT %1, %2").arg(offset).arg(count).toLatin1();
}

U2DataId U2DbiUtils::toU2DataId(qint64 id, U2DataType type, const QByteArray& dbExtra) {
    assert(sizeof(U2DataType) == 2);

    if (0 == id) {
        return emptyId;
    }

    int extraLen = dbExtra.size();
    int len = DATAID_MIN_LEN + extraLen;
    QByteArray res(len, Qt::Uninitialized);
    char* data = res.data();
    ((qint64*)(data + DB_ID_OFFSET))[0] = id;
    ((U2DataType*)(data + TYPE_OFFSET))[0] = type;
    if (extraLen > 0) {
        memcpy(data + DB_EXTRA_OFFSET, dbExtra.constData(), dbExtra.size());
    }

    return res;
}

quint64 U2DbiUtils::toDbiId(const U2DataId& id) {
    if (id.size() < DATAID_MIN_LEN) {
        return 0;
    }
    return *(qint64*)(id.constData() + DB_ID_OFFSET);
}

U2DataType U2DbiUtils::toType(const U2DataId& id) {
    if (id.size() < DATAID_MIN_LEN) {
        return 0;
    }
    return *(U2DataType*)(id.constData() + TYPE_OFFSET);
}

QByteArray U2DbiUtils::toDbExtra(const U2DataId& id) {
    if (id.size() < DATAID_MIN_LEN) {
        return emptyId;
    }
    return QByteArray(id.constData() + DB_EXTRA_OFFSET, id.length() - DB_EXTRA_OFFSET);
}

QString U2DbiUtils::text(const U2DataId& id) {
    QString res = QString("[Id: %1, Type: %2, Extra: %3]").arg(toDbiId(id)).arg(int(toType(id))).arg(toDbExtra(id).constData());
    return res;
}

QString U2DbiUtils::ref2Url(const U2DbiRef& dbiRef) {
    U2DbiFactory* dbiFactory = AppContext::getDbiRegistry()->getDbiFactoryById(dbiRef.dbiFactoryId);
    SAFE_POINT(NULL != dbiFactory, QString("Invalid database type: %1").arg(dbiRef.dbiFactoryId), "");
    return dbiFactory->id2Url(dbiRef.dbiId).getURLString();
}

QString U2DbiUtils::createDbiUrl(const QString &host, int port, const QString &dbName ) {
    QString portString = (port >= 0 ? QString::number(port) : "");
    return host + ":" + portString + "/" + dbName;
}

QString U2DbiUtils::createFullDbiUrl(const QString &userName, const QString &host, int port, const QString &dbName) {
    return createFullDbiUrl(userName, createDbiUrl(host, port, dbName));
}

QString U2DbiUtils::createFullDbiUrl(const QString &userName, const QString &dbiUrl) {
    return userName + "@" + dbiUrl;
}

bool U2DbiUtils::parseDbiUrl(const QString &dbiUrl, QString& host, int& port, QString& dbName) {
    int sepIndex = dbiUrl.indexOf(":");
    if (sepIndex < 0) {
        return false;
    }

    host = dbiUrl.left(sepIndex);

    sepIndex = dbiUrl.indexOf("/", sepIndex);
    if (sepIndex < 0) {
        return false;
    }

    QString portString = dbiUrl.mid(host.length() + 1, sepIndex - host.length() - 1);
    if (portString.isEmpty()) {
        port = -1;
    } else {
        bool ok = false;
        port = portString.toInt(&ok);
        if (!ok) {
            return false;
        }
    }

    dbName = dbiUrl.right(dbiUrl.length() - sepIndex - 1);

    return true;
}

bool U2DbiUtils::parseFullDbiUrl(const QString &dbiUrl, QString &userName, QString &host, int &port, QString &dbName) {
    return parseDbiUrl(full2shortDbiUrl(dbiUrl, userName), host, port, dbName);
}

QString U2DbiUtils::full2shortDbiUrl(const QString &fullDbiUrl) {
    QString unusedUserName;
    return full2shortDbiUrl(fullDbiUrl, unusedUserName);
}

QString U2DbiUtils::full2shortDbiUrl(const QString &fullDbiUrl, QString &userName) {
    int sepIndex = fullDbiUrl.indexOf("@");
    if (-1 == sepIndex) {
        return fullDbiUrl;
    }

    userName = fullDbiUrl.left(sepIndex);
    return fullDbiUrl.right(fullDbiUrl.length() - sepIndex - 1);
}

bool U2DbiUtils::isFullDbiUrl(const QString &dbiUrl) {
    return dbiUrl.contains('@');
}

QString U2DbiUtils::makeFolderCanonical(const QString& folder) {
    if (U2ObjectDbi::ROOT_FOLDER == folder) {
        return folder;
    }

    QString result = folder.startsWith(U2ObjectDbi::ROOT_FOLDER + U2ObjectDbi::PATH_SEP) ?
                folder :
                U2ObjectDbi::ROOT_FOLDER + U2ObjectDbi::PATH_SEP + folder;
    result.replace(QRegExp(U2ObjectDbi::PATH_SEP + "+"), U2ObjectDbi::PATH_SEP);

    if (U2ObjectDbi::ROOT_FOLDER != result &&
            result.endsWith(U2ObjectDbi::ROOT_FOLDER)) {
        result.chop(U2ObjectDbi::ROOT_FOLDER.size());
    }

    return result;
}

bool U2DbiUtils::isDbiReadOnly(const U2DbiRef &dbiRef) {
    U2OpStatusImpl os;
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, true);

    return con.dbi->isReadOnly();
}

Version U2DbiUtils::getDbMinRequiredVersion(const U2DbiRef &dbiRef, U2OpStatus &os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, Version());
    return Version::parseVersion(con.dbi->getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os));
}

bool U2DbiUtils::isDatabaseTooNew(const U2DbiRef &dbiRef, const Version &ugeneVersion, QString &minRequiredVersionString, U2OpStatus &os) {
    const Version minRequiredVersion = getDbMinRequiredVersion(dbiRef, os);
    CHECK_OP(os, false);
    minRequiredVersionString = minRequiredVersion.text;
    return minRequiredVersion > ugeneVersion;
}

bool U2DbiUtils::isDatabaseTooOld(const U2DbiRef &dbiRef, const Version &ugeneVersion, U2OpStatus &os) {
    const Version minRequiredVersion = getDbMinRequiredVersion(dbiRef, os);
    CHECK_OP(os, false);
    return minRequiredVersion < ugeneVersion;
}

//////////////////////////////////////////////////////////////////////////
// TmpDbiHandle

TmpDbiHandle::TmpDbiHandle() {
}

TmpDbiHandle::TmpDbiHandle(const QString& _alias, U2OpStatus& os, const U2DbiFactoryId &factoryId)
    : alias(_alias)
{
    dbiRef = AppContext::getDbiRegistry()->attachTmpDbi(alias, os, factoryId);
}

TmpDbiHandle::TmpDbiHandle(const TmpDbiHandle& dbiHandle) {
    if (dbiHandle.isValid()) {
        alias = dbiHandle.getAlias();
        dbiRef = dbiHandle.getDbiRef();

        U2OpStatus2Log os;
        AppContext::getDbiRegistry()->attachTmpDbi(dbiHandle.getAlias(), os, dbiRef.dbiFactoryId);
    }
}

TmpDbiHandle& TmpDbiHandle::operator=(const TmpDbiHandle& dbiHandle) {
    if (this != &dbiHandle) {
        if (dbiHandle.isValid()) {
            alias = dbiHandle.getAlias();
            dbiRef = dbiHandle.getDbiRef();

            U2OpStatus2Log os;
            AppContext::getDbiRegistry()->attachTmpDbi(dbiHandle.getAlias(), os, dbiRef.dbiFactoryId);
        }
    }

    return *this;
}

TmpDbiHandle::~TmpDbiHandle () {
    if (isValid())
    {
        U2OpStatus2Log os;
        AppContext::getDbiRegistry()->detachTmpDbi(alias, os);
    }
}

//////////////////////////////////////////////////////////////////////////
// TmpDbiObjects
TmpDbiObjects::~TmpDbiObjects() {
    if (os.isCoR()) {
        foreach(const U2DataId& id, objects) {
            if (!id.isEmpty()) {
                U2OpStatus2Log os2log;
                DbiConnection con(dbiRef, os2log);
                con.dbi->getObjectDbi()->removeObject(id, os2log);
            }
        }
    }
}

DbiOperationsBlock::DbiOperationsBlock(const U2DbiRef &_dbiRef, U2OpStatus &os) :
    dbiRef(_dbiRef),
    os(os)
{
    connection = new DbiConnection(dbiRef, os);
    CHECK_OP(os, );
    connection->dbi->startOperationsBlock(os);
}

DbiOperationsBlock::~DbiOperationsBlock() {
    if (NULL != connection->dbi) {
        connection->dbi->stopOperationBlock(os);
    }
    delete connection;
}

}   // namespace U2
