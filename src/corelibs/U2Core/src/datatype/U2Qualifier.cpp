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

#include <U2Core/TextUtils.h>

#include "U2Qualifier.h"

namespace U2 {

U2Qualifier::U2Qualifier() {

}

U2Qualifier::U2Qualifier(const QString &name, const QString &value)
    : name(name),
    value(value) {
    //    SAFE_POINT(isValid(), "An attempt to create an invalid qualifier", );
}

bool U2Qualifier::isValid() const {
    return isValidQualifierName(name) && isValidQualifierValue(value);
}

bool U2Qualifier::operator==(const U2Qualifier &q) const {
    return q.name == name && q.value == value;
}

bool U2Qualifier::operator!=(const U2Qualifier &q) const {
    return !(*this == q);
}

bool U2Qualifier::isValidQualifierName(const QString &name) {
    return !name.isEmpty() && TextUtils::fits(TextUtils::QUALIFIER_NAME_CHARS, name.toLocal8Bit().data(), name.length());
}

bool U2Qualifier::isValidQualifierValue(const QString &) {
    return true;
}

}   // namespace U2
