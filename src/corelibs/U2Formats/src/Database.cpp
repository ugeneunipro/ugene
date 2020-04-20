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


#include "Database.h"

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/U2Type.h>
#include <U2Formats/SQLiteDbi.h>

#include <QFileInfo>

namespace U2 {

#define S3_DATABASE_KEY "s3-database"

Database* Database::loadDatabase(const QString& url, U2OpStatus& os) {

    U2DbiRef dbiRef (SQLiteDbiFactory::ID, url);
    DbiConnection dbHandle (dbiRef, os);
    SAFE_POINT_OP(os, NULL);

    // ensure that DBI is a valid Session DBI
    QString val = dbHandle.dbi->getProperty(S3_DATABASE_KEY, "", os);
    SAFE_POINT_OP(os, NULL);

    if (val != U2DbiOptions::U2_DBI_VALUE_ON) {
        os.setError(tr("Not a valid S3-database file: %1").arg(url));
        return NULL;
    }

    return new Database(dbHandle);
}

DbiConnection Database::createEmptyDbi(const QString& url, U2OpStatus& os) {
    QFileInfo fi(url);
    if (fi.exists()) {
        os.setError(tr("File already exists: %1").arg(url));
        return DbiConnection();
    }

    U2DbiRef dbiRef (SQLiteDbiFactory::ID, url);
    DbiConnection databaseDbi (dbiRef, true, os);
    SAFE_POINT_OP(os, DbiConnection());

    databaseDbi.dbi->setProperty(S3_DATABASE_KEY, U2DbiOptions::U2_DBI_VALUE_ON, os);
    SAFE_POINT_OP(os, DbiConnection());

    return databaseDbi;
}

} //namespace
