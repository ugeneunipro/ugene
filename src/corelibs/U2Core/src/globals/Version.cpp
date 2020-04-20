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

#include "Version.h"

#ifndef U2_APP_VERSION
#error U2_APP_VERSION is not set!
#endif

#ifndef UGENE_MIN_VERSION_SQLITE
#error UGENE_MIN_VERSION_SQLITE is not set!
#else
#define U2_APP_MIN_VERSION_SQLITE_STRING TOSTRING(UGENE_MIN_VERSION_SQLITE)
#endif

#ifndef UGENE_MIN_VERSION_MYSQL
#error UGENE_MIN_VERSION_MYSQL is not set!
#else
#define U2_APP_MIN_VERSION_MYSQL_STRING TOSTRING(UGENE_MIN_VERSION_MYSQL)
#endif

#ifndef U2_DISTRIBUTION_INFO
#define U2_DISTRIBUTION_INFO "sources"
#endif

namespace U2 {

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define U2_APP_VERSION_STRING TOSTRING(U2_APP_VERSION)
#define VERSION_DEV_SUFFIX "dev"

const QString Version::buildDate = __DATE__;
const int Version::appArchitecture = QT_POINTER_SIZE * 8;
const QString Version::distributionInfo = QString(TOSTRING(U2_DISTRIBUTION_INFO)).replace("_"," ");

Version::Version() {
    major = minor = patch = 0;
    debug = false;
    text = "unknown";
}

Version Version::parseVersion(const QString& text) {
    Version v;
    v.text = text;

    //parse sub-numbers and suffix
    int versionType = 0;
    QString currentNum;
    int textPos = 0;
    for (; textPos < v.text.length(); textPos++ ) {
        QChar c = v.text.at(textPos);
        if (c.isNumber()) {
            currentNum += c;
        } else {
            bool ok;
            int val = currentNum.toInt(&ok);
            if (!ok) {
                break;
            }
            if (versionType == 0) {
                v.major = val;
            } else if (versionType == 1) {
                v.minor = val;
            } else {
                v.patch = val;
                break;
            }
            versionType++;
            currentNum.clear();
        }
    }
    v.suffix = v.text.mid(textPos);
    v.isDevVersion = v.suffix.contains(VERSION_DEV_SUFFIX);
    if(v.suffix.isEmpty()){//See issue UGENE-870 (https://ugene.net/tracker/browse/UGENE-870)
        bool ok;
        int val = currentNum.toInt(&ok);
        if (ok) {
            v.patch = val;
        }
    }

#ifdef _DEBUG
    v.debug = true;
#else
    v.debug = false;
#endif

    return v;
}

Version Version::appVersion() {
    return parseVersion(U2_APP_VERSION_STRING);
}

Version Version::qtVersion() {
    return parseVersion(QT_VERSION_STR);
}

Version Version::minVersionForSQLite() {
    return parseVersion(U2_APP_MIN_VERSION_SQLITE_STRING);
}

Version Version::minVersionForMySQL() {
    return parseVersion(U2_APP_MIN_VERSION_MYSQL_STRING);
}


bool Version::operator  >  (const Version& v) const {
    return v < *this;
}

bool Version::operator  >= (const Version& v) const {
    return v <= *this;
}

bool Version::operator  <  (const Version& v) const {
    if (v.major != major) {
        return v.major > major;
    }

    if (v.minor != minor) {
        return v.minor > minor;
    }

    if (v.patch != patch) {
        return v.patch > patch;
    }

    return false;

}

bool Version::operator  <= (const Version& v) const {
    return *this < v || *this == v;
}

bool Version::operator  == (const Version& v) const {
    return major == v.major && minor == v.minor && patch == v.patch;
}


} //namespace

