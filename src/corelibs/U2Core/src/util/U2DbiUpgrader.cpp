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

#include "U2DbiUpgrader.h"

namespace U2 {

U2DbiUpgrader::U2DbiUpgrader(const Version &versionFrom, const Version &versionTo) :
    versionFrom(versionFrom),
    versionTo(versionTo)
{
}

U2DbiUpgrader::~U2DbiUpgrader() {

}

bool U2DbiUpgrader::isAppliable(const Version &dbVersion) const {
    return versionFrom == dbVersion;
}

bool U2DbiUpgrader::operator < (const U2DbiUpgrader &other) const {
    SAFE_POINT((versionFrom < other.versionFrom && versionTo < other.versionTo && versionTo <= other.versionFrom) ||
               (versionFrom > other.versionFrom && versionTo > other.versionTo && versionFrom >= other.versionTo),
                "Upgrader versions intersect, which will be used first is undefined", false);
    return versionFrom < other.versionFrom && versionTo < other.versionTo;
}

}   // namespace U2
