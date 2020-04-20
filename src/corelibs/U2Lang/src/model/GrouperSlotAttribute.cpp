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

#include <U2Lang/IntegralBusType.h>

#include "GrouperSlotAttribute.h"

namespace U2 {

GrouperOutSlotAttribute::GrouperOutSlotAttribute(const Descriptor &d, const DataTypePtr type, bool required, const QVariant &defaultValue)
: Attribute(d, type, required, defaultValue)
{

}

GrouperOutSlotAttribute::~GrouperOutSlotAttribute() {
}

Attribute *GrouperOutSlotAttribute::clone() {
    return new GrouperOutSlotAttribute(*this);
}

AttributeGroup GrouperOutSlotAttribute::getGroup() {
    return GROUPER_SLOT_GROUP;
}

QList<GrouperOutSlot> &GrouperOutSlotAttribute::getOutSlots() {
    return outSlots;
}

const QList<GrouperOutSlot> &GrouperOutSlotAttribute::getOutSlots() const {
    return outSlots;
}

void GrouperOutSlotAttribute::addOutSlot(const GrouperOutSlot &outSlot) {
    outSlots.append(outSlot);
}

void GrouperOutSlotAttribute::updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap) {
    QList<GrouperOutSlot> newOutSlots;
    foreach (const GrouperOutSlot &gSlot, outSlots) {
        QString slotStr = gSlot.getInSlotStr();
        slotStr = GrouperOutSlot::readable2busMap(slotStr);
        Workflow::IntegralBusType::remapSlotString(slotStr, actorIdsMap);
        slotStr = GrouperOutSlot::busMap2readable(slotStr);

        GrouperOutSlot newGSlot(gSlot);
        newGSlot.setInSlotStr(slotStr);
        newOutSlots << newGSlot;
    }

    outSlots = newOutSlots;
}

GroupSlotAttribute::GroupSlotAttribute(const Descriptor &d, const DataTypePtr type, bool required, const QVariant &defaultValue)
: Attribute(d, type, required, defaultValue)
{

}

Attribute *GroupSlotAttribute::clone() {
    return new GroupSlotAttribute(*this);
}

void GroupSlotAttribute::updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap) {
    QString slotStr = this->getAttributeValueWithoutScript<QString>();
    slotStr = GrouperOutSlot::readable2busMap(slotStr);
    Workflow::IntegralBusType::remapSlotString(slotStr, actorIdsMap);
    slotStr = GrouperOutSlot::busMap2readable(slotStr);

    this->setAttributeValue(slotStr);
}

void GroupSlotAttribute::setAttributeValue(const QVariant &newVal) {
    QString slotStr = newVal.toString();
    Attribute::setAttributeValue(GrouperOutSlot::busMap2readable(slotStr));
}

} // U2
