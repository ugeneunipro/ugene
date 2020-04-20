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

#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchema.h>

#include "util/MysqlHelpers.h"
#include "MysqlBlobOutputStream.h"

namespace U2 {

MysqlBlobOutputStream::MysqlBlobOutputStream(MysqlDbRef *db, const QByteArray &tableId,
    const QByteArray &columnId, const U2DataId &rowId, int /*size*/, U2OpStatus &os)
    : db(db), tableId(tableId), columnId(columnId), rowId(rowId), wasUsed(false)
{
    SAFE_POINT_EXT(NULL != db, os.setError("Invalid database reference detected!"), );
}

void MysqlBlobOutputStream::close() {

}

void MysqlBlobOutputStream::write(const char *buffer, int length, U2OpStatus &os) {
    SAFE_POINT_EXT(NULL != buffer, os.setError("Invalid data buffer detected!"), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray blobData;
    if (Q_LIKELY(wasUsed)) {
        U2SqlQuery getBlobQuery(QString("SELECT %1 FROM %2 WHERE %3 = :%3")
            .arg(QString(columnId), QString(tableId), QString(UdrSchema::RECORD_ID_FIELD_NAME)), db, os);

        getBlobQuery.bindDataId(":" + UdrSchema::RECORD_ID_FIELD_NAME, rowId);
        getBlobQuery.step();
        CHECK_OP(os, );
        blobData = getBlobQuery.getBlob(0);
    } else {
        wasUsed = true;
    }
    blobData += QByteArray(buffer, length);

    U2SqlQuery updateBlobQuery(QString("UPDATE %1 SET %2 = :%2 WHERE %3 = :%3")
        .arg(QString(tableId), QString(columnId), QString(UdrSchema::RECORD_ID_FIELD_NAME)), db, os);

    updateBlobQuery.bindBlob(":" + columnId, blobData);
    updateBlobQuery.bindDataId(":" + UdrSchema::RECORD_ID_FIELD_NAME, rowId);

    updateBlobQuery.update();
}

} // U2
