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

#ifndef _U2_PORTMAPPING_H_
#define _U2_PORTMAPPING_H_

#include <U2Core/U2OpStatus.h>

#include "SlotMapping.h"

namespace U2 {

class U2LANG_EXPORT PortMapping : public IdMapping {
public:
    PortMapping(const QString &srcPortId, const QString &dstPortId);

    void addSlotMapping(const SlotMapping &value);
    const QList<SlotMapping> & getMappings() const;
    QString getDstSlotId(const QString &srcSloId, U2OpStatus &os) const;

    void validate(const QMap<Descriptor, DataTypePtr> &srcType,
        const QMap<Descriptor, DataTypePtr> &dstType, U2OpStatus &os) const;

    static PortMapping getMappingBySrcPort(const QString &srcPortId, const QList<PortMapping> &mappings, U2OpStatus &os);

private:
    QList<SlotMapping> slotList;

    void validateSlotsCount(const QMap<Descriptor, DataTypePtr> &srcType,
        const QMap<Descriptor, DataTypePtr> &dstType, U2OpStatus &os) const;
    /** Returns found datatype */
    DataTypePtr validateSlotId(const QString &portId, const QString &slotId,
        const QMap<Descriptor, DataTypePtr> &type, U2OpStatus &os) const;
    void tryAddId(const QString &id,
        QSet<QString> &idSet, U2OpStatus &os) const;
    void validateMappingsCount(const QMap<Descriptor, DataTypePtr> &srcType,
        U2OpStatus &os) const;
};

} // U2

#endif // _U2_PORTMAPPING_H_
