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

#include <U2Core/AppContext.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DatabaseConnectionAdapter.h"

namespace U2 {

DatabaseConnectionAdapterFactory::DatabaseConnectionAdapterFactory(QObject *parent) :
    IOAdapterFactory(parent),
    name(tr("Database connection"))
{
}

IOAdapter* DatabaseConnectionAdapterFactory::createIOAdapter() {
    return new DatabaseConnectionAdapter(this);
}

IOAdapterId DatabaseConnectionAdapterFactory::getAdapterId() const {
    return BaseIOAdapters::DATABASE_CONNECTION;
}

const QString& DatabaseConnectionAdapterFactory::getAdapterName() const {
    return name;
}

bool DatabaseConnectionAdapterFactory::isIOModeSupported(IOAdapterMode m) const {
    // read only
    return (m == IOAdapterMode_Read) || (m == IOAdapterMode_Write);
}

TriState DatabaseConnectionAdapterFactory::isResourceAvailable(const GUrl&) const {
    return TriState_Unknown;
}

DatabaseConnectionAdapter::DatabaseConnectionAdapter(DatabaseConnectionAdapterFactory* factory, QObject* parent) :
    IOAdapter(factory, parent)
{
}

bool DatabaseConnectionAdapter::open(const GUrl& url, IOAdapterMode m) {
    if (IOAdapterMode_Read != m) {
        // In current state you can only read from shared database
        return false;
    }

    CHECK(!url.isEmpty(), false);

    U2DbiRef dbiRef(MYSQL_DBI_ID, url.getURLString());

    U2OpStatus2Log os;
    connection = DbiConnection(dbiRef, os);
    CHECK_OP(os, false);

    return connection.isOpen();
}

bool DatabaseConnectionAdapter::isOpen() const {
    return connection.isOpen();
}

void DatabaseConnectionAdapter::close() {
    U2OpStatus2Log os;
    connection.close(os);
}

qint64 DatabaseConnectionAdapter::readUntil(char*, qint64, const QBitArray&, TerminatorHandling, bool*) {
    FAIL("Operation is not supported", 0);
    return 0;
}

qint64 DatabaseConnectionAdapter::readBlock(char*, qint64) {
    FAIL("Operation is not supported", 0);
    return 0;
}

qint64 DatabaseConnectionAdapter::writeBlock(const char*, qint64) {
    FAIL("Operation is not supported", 0);
    return 0;
}

bool DatabaseConnectionAdapter::skip(qint64) {
    FAIL("Operation is not supported", false);
    return false;
}

qint64 DatabaseConnectionAdapter::left() const {
    FAIL("Operation is not supported", -1);
    return -1;
}

DbiConnection DatabaseConnectionAdapter::getConnection() const {
    return connection;
}

int DatabaseConnectionAdapter::getProgress() const {
    return -1;
}

bool DatabaseConnectionAdapter::isEof() {
    return true;
}

GUrl DatabaseConnectionAdapter::getURL() const {
    return GUrl(connection.dbi->getDbiId(), GUrl_Network);
}

QString DatabaseConnectionAdapter::errorString() const {
    return "";
}

}    // namespace U2
