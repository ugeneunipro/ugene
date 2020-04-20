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

#ifndef _U2_VERSION_H_
#define _U2_VERSION_H_

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT Version {
public:
    // creates empty version structure
    Version();

    // Current UGENE instance version
    static Version appVersion();

    // Current QT version
    static Version qtVersion();

    // Minimum UGENE version whose SQLite databases are compatible with this version
    static Version minVersionForSQLite();

    // minimum UGENE version whose MySQL databases are compatible with this version
    static Version minVersionForMySQL();

    static Version parseVersion(const QString& text);

    bool operator  >  (const Version& v) const;
    bool operator  >= (const Version& v) const;
    bool operator  <  (const Version& v) const;
    bool operator  <= (const Version& v) const;
    bool operator  == (const Version& v) const;
    bool operator  != (const Version& v) const {return !(v == *this);}

    int         major;
    int         minor;
    int         patch;

    bool        debug;
    QString     text;
    QString     suffix;
    bool        isDevVersion;
    const static int         appArchitecture;
    const static QString     buildDate;
    const static QString     distributionInfo;
};

} //namespace

#endif
