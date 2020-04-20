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

#include <3rdparty/sqlite3/sqlite3.h>

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include "SQLiteBlobInputStream.h"

namespace U2 {

/************************************************************************/
/* SQLiteBlobStream */
/************************************************************************/
const int SQLiteBlobStream::READ_ONLY = 0;
const int SQLiteBlobStream::READ_WRITE = 1;

SQLiteBlobStream::SQLiteBlobStream()
: handle(NULL), size(0), offset(0)
{

}

SQLiteBlobStream::~SQLiteBlobStream() {
    releaseHandle();
}

void SQLiteBlobStream::init(int accessType, DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != db, os.setError("NULL db ref"), );
    SAFE_POINT_EXT(NULL != db->handle, os.setError("NULL db handle"), );

    int status = sqlite3_blob_open(
        db->handle,
        "main",
        tableId.constData(),
        columnId.constData(),
        U2DbiUtils::toDbiId(rowId),
        accessType,
        &handle
    );
    if (SQLITE_OK != status) {
        os.setError(sqlite3_errmsg(db->handle));
    } else {
        size = sqlite3_blob_bytes(handle);
    }
}

void SQLiteBlobStream::releaseHandle() {
    if (NULL != handle) {
        sqlite3_blob_close(handle);
        handle = NULL;
    }
}

/************************************************************************/
/* SQLiteBlobInputStream */
/************************************************************************/
SQLiteBlobInputStream::SQLiteBlobInputStream(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, U2OpStatus &os)
: InputStream(), SQLiteBlobStream()
{
    init(SQLiteBlobStream::READ_ONLY, db, tableId, columnId, rowId, os);
}

qint64 SQLiteBlobInputStream::available() {
    return size - offset;
}

void SQLiteBlobInputStream::close() {
    releaseHandle();
}

int SQLiteBlobInputStream::read(char *buffer, int length, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != handle, os.setError("blob handle is not opened"), 0);
    int targetLength = (offset + length < size) ? length : (size - offset);
    if (0 == targetLength) {
        return -1;
    }

    int status = sqlite3_blob_read(handle, (void*)buffer, targetLength, offset);
    if (SQLITE_OK != status) {
        os.setError(QObject::tr("Can not read data. The database is closed or the data were changed."));
        return 0;
    }
    offset += targetLength;
    return targetLength;
}

qint64 SQLiteBlobInputStream::skip(qint64 n, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != handle, os.setError("blob handle is not opened"), 0);
    if (offset + n >= size) {
        int oldOffset = offset;
        offset = size;
        return size - oldOffset;
    }
    if (offset + n < 0) {
        int oldOffset = offset;
        offset = 0;
        return -oldOffset;
    }
    offset += n;
    return n;
}

} // U2
