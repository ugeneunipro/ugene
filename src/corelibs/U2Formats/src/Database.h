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
#ifndef _U2_DATABASE_H_
#define _U2_DATABASE_H_

#include <U2Core/global.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

#define U2_DATABASE_SETTINGS_DIR QString("database/")
#define U2_RECENT_DATABASES_LIST QString("recent/")
#define U2_LAST_USED_DATABASE QString("last/")

#define U2_DEFAULT_DATABASE_DESCR QString("Database with\nhuman genome sequences\ngene annotations\ndamage effect matrix")
#define U2_HG19_DATABASE_DESCR QString("Database with\nhuman genome sequences(hg19): \nhttp://hgdownload.cse.ucsc.edu/goldenPath/hg19/database/ \ngene annotations (protein coding, tRNA, predicted genes)\ndamage effect matrix (improved SIFT)")
#define U2_HG18_DATABASE_DESCR QString("Database with\nhuman genome sequences(hg18)\ngene annotations\ndamage effect matrix")

class U2Dbi;
class U2OpStatus;
/**
    Global genome database: genome sequences and annotations
*/

class U2FORMATS_EXPORT Database : public QObject {
    Q_OBJECT
protected:
    Database(const DbiConnection& dbi) : databaseDbi(dbi) {}

public:
    const DbiConnection& getDbi() const {return databaseDbi;}

    static Database* loadDatabase(const QString& url, U2OpStatus& os);

    static DbiConnection createEmptyDbi(const QString& url, U2OpStatus& os);

private:
    DbiConnection databaseDbi;
};

} //namespace

#endif
