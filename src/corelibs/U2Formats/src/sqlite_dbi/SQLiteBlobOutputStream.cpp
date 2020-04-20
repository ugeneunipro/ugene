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

#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchema.h>

#include "SQLiteBlobOutputStream.h"

namespace U2 {

SQLiteBlobOutputStream::SQLiteBlobOutputStream(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, int size, U2OpStatus &os)
: OutputStream(), SQLiteBlobStream()
{
    SAFE_POINT_EXT(NULL != db, os.setError("NULL db ref"), );
    SAFE_POINT_EXT(NULL != db->handle, os.setError("NULL db handle"), );

    update(db, tableId, columnId, rowId, size, os);
    CHECK_OP(os, );
    init(SQLiteBlobStream::READ_WRITE, db, tableId, columnId, rowId, os);
}

void SQLiteBlobOutputStream::close() {
    releaseHandle();
}

void SQLiteBlobOutputStream::write(const char *buffer, int length, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != handle, os.setError("blob handle is not opened"), );
    int status = sqlite3_blob_write(handle, (void*)buffer, length, offset);
    if (SQLITE_OK != status) {
        os.setError(QObject::tr("Can not write data. The database is closed or the data were changed."));
        return;
    }
    offset += length;
}

void SQLiteBlobOutputStream::update(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, int size, U2OpStatus &os) {
    SQLiteWriteQuery q("UPDATE " + tableId + " SET " + columnId + " = ?1 WHERE " + UdrSchema::RECORD_ID_FIELD_NAME + " = ?2", db, os);
    CHECK_OP(os, );

    q.bindZeroBlob(1, size);
    q.bindDataId(2, rowId);
    q.update(1);
}

} // U2
