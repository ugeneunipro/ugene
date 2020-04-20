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

#ifndef _U2_SQLITEBLOBOUTPUTSTREAM_H_
#define _U2_SQLITEBLOBOUTPUTSTREAM_H_

#include <U2Core/OutputStream.h>
#include <U2Core/U2SqlHelpers.h>

#include "SQLiteBlobInputStream.h"

namespace U2 {

class SQLiteBlobOutputStream : public OutputStream, public SQLiteBlobStream {
public:
    SQLiteBlobOutputStream(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, int size, U2OpStatus &os);

    void close();
    void write(const char *buffer, int length, U2OpStatus &os);

private:
    void update(DbRef *db, const QByteArray &tableId, const QByteArray &columnId, const U2DataId &rowId, int size, U2OpStatus &os);
};

} // U2

#endif // _U2_SQLITEBLOBOUTPUTSTREAM_H_
