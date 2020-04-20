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

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchema.h>

#include "util/MysqlHelpers.h"
#include "MysqlBlobInputStream.h"

namespace U2 {

MysqlBlobInputStream::MysqlBlobInputStream(MysqlDbRef *db, const QByteArray &tableId,
    const QByteArray &columnId, const U2DataId &rowId, U2OpStatus &os)
    : size(0), offset(0)
{
    CHECK_EXT(NULL != db, os.setError("Invalid database handler detected!"), );

    U2SqlQuery q(QString("SELECT %1 FROM %2 WHERE %3 = :%3").arg(QString(columnId), QString(tableId), QString(UdrSchema::RECORD_ID_FIELD_NAME)), db, os);

    q.bindDataId(":" + UdrSchema::RECORD_ID_FIELD_NAME, rowId);
    q.step();
    CHECK_OP(os, );
    blobData = q.getBlob(0);
    size = blobData.size();
}

qint64 MysqlBlobInputStream::available() {
    return size - offset;
}

void MysqlBlobInputStream::close() {

}

int MysqlBlobInputStream::read(char *buffer, int length, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != buffer, os.setError("Invalid data buffer detected!"), -1);

    int targetLength = (offset + length < size) ? length : (size - offset);
    if (0 == targetLength) {
        return -1;
    }

    const QByteArray result = blobData.mid(offset, targetLength);
    memcpy(buffer, result.constData(), targetLength);

    offset += targetLength;
    return targetLength;
}

qint64 MysqlBlobInputStream::skip(qint64 n, U2OpStatus &/*os*/) {
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
