/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Lang/Attribute.h>
#include <U2Lang/GrouperOutSlot.h>

namespace U2 {

class U2LANG_EXPORT GrouperOutSlotAttribute : public Attribute {
public:
    GrouperOutSlotAttribute(const Descriptor& d, const DataTypePtr type, bool required = false, const QVariant& defaultValue = QVariant());
    ~GrouperOutSlotAttribute();

    Attribute* clone() override;
    AttributeGroup getGroup() override;
    void updateActorIds(const QMap<ActorId, ActorId>& actorIdsMap) override;

    QList<GrouperOutSlot>& getOutSlots();
    const QList<GrouperOutSlot>& getOutSlots() const;
    void addOutSlot(const GrouperOutSlot& outSlot);

private:
    QList<GrouperOutSlot> outSlots;
};

class U2LANG_EXPORT GroupSlotAttribute : public Attribute {
public:
    GroupSlotAttribute(const Descriptor& d, const DataTypePtr type, bool required = false, const QVariant& defaultValue = QVariant());

    Attribute* clone() override;
    void updateActorIds(const QMap<ActorId, ActorId>& actorIdsMap) override;
    void setAttributeValue(const QVariant& newVal) override;
};

}  // namespace U2
