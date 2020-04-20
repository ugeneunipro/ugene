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

#ifndef _U2_SQLITEBLOBINPUTSTREAM_H_
#define _U2_SQLITEBLOBINPUTSTREAM_H_

#include <U2Core/InputStream.h>
#include <U2Core/U2SqlHelpers.h>

struct sqlite3_blob;

namespace U2 {

class SQLiteBlobStream {
    Q_DISABLE_COPY(SQLiteBlobStream)
public:
    SQLiteBlobStream();
    virtual ~SQLiteBlobStream();

protected:
    void init(int accessType, DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, U2OpStatus &os);
    void releaseHandle();

protected:
    sqlite3_blob *handle;
    qint64 size;
    qint64 offset;

    static const int READ_ONLY;
    static const int READ_WRITE;
};

class SQLiteBlobInputStream : public InputStream, public SQLiteBlobStream {
    Q_DISABLE_COPY(SQLiteBlobInputStream)
public:
    SQLiteBlobInputStream(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, U2OpStatus &os);

    qint64 available();
    void close();
    int read(char *buffer, int length, U2OpStatus &os);
    qint64 skip(qint64 n, U2OpStatus &os);
};

} // U2

#endif // _U2_SQLITEBLOBINPUTSTREAM_H_
