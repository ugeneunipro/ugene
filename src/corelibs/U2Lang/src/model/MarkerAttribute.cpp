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

#include "MarkerAttribute.h"

namespace U2 {

MarkerAttribute::MarkerAttribute(const Descriptor& d, const DataTypePtr type, bool required, const QVariant & defaultValue)
: Attribute(d, type, required, defaultValue)
{

}

void MarkerAttribute::setAttributeValue(const QVariant &/*newVal*/) {
    FAIL("marker set value", );
}

const QVariant &MarkerAttribute::getAttributePureValue() const {
    QStringList names;
    foreach (Marker *marker, markers) {
        names << marker->getName();
    }
    const_cast<QVariant&>(value) = names.join(",");
    return value;
}

bool MarkerAttribute::isDefaultValue() const {
    return (defaultValue == getAttributePureValue());
}

Attribute *MarkerAttribute::clone() {
    return new MarkerAttribute(*this);
}

AttributeGroup MarkerAttribute::getGroup() {
    return MARKER_GROUP;
}

QList<Marker*> & MarkerAttribute::getMarkers() {
    return markers;
}

bool MarkerAttribute::contains(const QString &markerId) const {
    foreach (Marker *marker, markers) {
        if (NULL == marker) {
            continue;
        }
        if (marker->getName() == markerId) {
            return true;
        }
    }
    return false;
}

} //U2
