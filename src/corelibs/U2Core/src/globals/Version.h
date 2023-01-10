/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
    Version(int major = 0, int minor = 0, int patch = 0);

    // Current UGENE instance version
    static Version appVersion();

    // Current QT version
    static Version qtVersion();

    static Version parseVersion(const QString& versionText);

    /**
     * Checks that build and runtime versions are compatible.
     * Logs error message to stdout if versions are not compatible.
     * Returns false if versions are not compatible or true otherwise.
     */
    static bool checkBuildAndRuntimeVersions();

    /** String representation of the version: Example: 42.1-dev. This text can be parsed by parseVersion. */
    QString toString() const;

    /** Returns true if the version is not '0.0.0' (created with a default constructor). */
    bool isValid() const;

    bool operator>(const Version& v) const;
    bool operator>=(const Version& v) const;
    bool operator<(const Version& v) const;
    bool operator<=(const Version& v) const;
    bool operator==(const Version& v) const;
    bool operator!=(const Version& v) const {
        return !(v == *this);
    }

    int major;
    int minor;
    int patch;

    bool debug = false;
    QString suffix;
    bool isDevVersion = false;
    const static int appArchitecture;
    const static QString buildDate;
};

}  // namespace U2

#endif
