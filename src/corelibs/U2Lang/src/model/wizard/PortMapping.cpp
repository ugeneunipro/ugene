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

#include <QSet>

#include <U2Core/U2SafePoints.h>

#include "PortMapping.h"

namespace U2 {

PortMapping::PortMapping(const QString &srcPortId, const QString &dstPortId)
: IdMapping(srcPortId, dstPortId)
{

}

void PortMapping::addSlotMapping(const SlotMapping &value) {
    slotList << value;
}

const QList<SlotMapping> & PortMapping::getMappings() const {
    return slotList;
}

void PortMapping::validate(const QMap<Descriptor, DataTypePtr> &srcType,
    const QMap<Descriptor, DataTypePtr> &dstType, U2OpStatus &os) const {
    validateSlotsCount(srcType, dstType, os);
    CHECK_OP(os, );

    QSet<QString> srcIds;
    QSet<QString> dstIds;
    foreach (const SlotMapping &mapping, slotList) {
        tryAddId(mapping.getSrcId(), srcIds, os);
        CHECK_OP(os, );
        tryAddId(mapping.getDstId(), dstIds, os);
        CHECK_OP(os, );
        DataTypePtr srcSlotType = validateSlotId(srcId, mapping.getSrcId(), srcType, os);
        CHECK_OP(os, );
        DataTypePtr dstSlotType = validateSlotId(dstId, mapping.getDstId(), dstType, os);
        CHECK_OP(os, );
        mapping.validate(srcSlotType, dstSlotType, os);
        CHECK_OP(os, );
    }

    validateMappingsCount(srcType, os);
    CHECK_OP(os, );
}

void PortMapping::validateSlotsCount(const QMap<Descriptor, DataTypePtr> &srcType,
    const QMap<Descriptor, DataTypePtr> &dstType, U2OpStatus &os) const{
    if (srcType.size() != dstType.size()) {
        os.setError(QObject::tr("Ports can not be mapped: %1, %2. Slots count is different").arg(srcId).arg(dstId));
    }
}

DataTypePtr PortMapping::validateSlotId(const QString &portId, const QString &slotId,
    const QMap<Descriptor, DataTypePtr> &type, U2OpStatus &os) const {
    if (!type.contains(slotId)) {
        os.setError(QObject::tr("%1 port does not contain a slot with id: %2").arg(portId).arg(slotId));
        return DataTypePtr();
    }
    return type[slotId];
}

void PortMapping::tryAddId(const QString &id,
    QSet<QString> &idSet, U2OpStatus &os) const {
    if (idSet.contains(id)) {
        os.setError(QObject::tr("Duplicated mapping of slot with id: %1").arg(id));
        return;
    }
    idSet << id;
}

void PortMapping::validateMappingsCount(const QMap<Descriptor, DataTypePtr> &srcType,
    U2OpStatus &os) const {
    if (slotList.count() < srcType.count()) {
        os.setError(QObject::tr("Not all slots are mapped"));
    }
}

PortMapping PortMapping::getMappingBySrcPort(const QString &srcPortId, const QList<PortMapping> &mappings, U2OpStatus &os) {
    foreach (const PortMapping &m, mappings) {
        if (m.getSrcId() == srcPortId) {
            return m;
        }
    }
    os.setError(QString("No mapping for port: %1").arg(srcPortId));
    return PortMapping("", "");
}

QString PortMapping::getDstSlotId(const QString &srcSlotId, U2OpStatus &os) const {
    foreach (const SlotMapping &m, slotList) {
        if (m.getSrcId() == srcSlotId) {
            return m.getDstId();
        }
    }
    os.setError(QString("No mapping for slot: %1").arg(srcSlotId));
    return "";
}

} // U2
