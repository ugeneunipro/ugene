/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Lang/Attribute.h>
#include <U2Lang/Marker.h>

#    define _MARKER_ATTRIBUTE_H_

namespace U2 {

class U2LANG_EXPORT MarkerAttribute : public Attribute {
public:
    MarkerAttribute(const Descriptor& d, const DataTypePtr type, bool required = false, const QVariant& defaultValue = QVariant());
    void setAttributeValue(const QVariant& newVal) override;
    const QVariant& getAttributePureValue() const override;
    bool isDefaultValue() const override;
    Attribute* clone() override;
    AttributeGroup getGroup() override;

    QList<Marker*>& getMarkers();
    bool contains(const QString& markerId) const;

private:
    QList<Marker*> markers;
};

}  // namespace U2
