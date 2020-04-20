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

#include "SNPDatabaseUtils.h"

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <QFile>


namespace U2 {

#define S3_DATABASE_KEY "s3-database"

Database* SNPDatabaseUtils::openDatabase( const QString& path ){
    Database* res = NULL;

    if (QFile::exists(path)){
        U2OpStatusImpl os;
        res = Database::loadDatabase(path, os);
        CHECK_OP(os, res);
    }

    return res;
}

U2DataId SNPDatabaseUtils::getSequenceId( const QString& sequenceName, U2ObjectDbi* objectDbi ){
    U2DataId seqId;
    if (sequenceName.isEmpty()){
        return seqId;
    }
    SAFE_POINT(objectDbi != NULL, "object Dbi is NULL", seqId);

    U2OpStatusImpl os;
    QScopedPointer< U2DbiIterator<U2DataId> > it(objectDbi->getObjectsByVisualName(sequenceName, U2Type::Sequence, os));
    SAFE_POINT(it->hasNext(), "no sequence found", seqId );
    seqId = it->next();

    return seqId;

}

} // U2
