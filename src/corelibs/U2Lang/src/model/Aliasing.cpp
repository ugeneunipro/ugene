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

#include "Aliasing.h"

namespace U2 {
namespace Workflow {

SlotAlias::SlotAlias(const Port *port, const QString &slotId, const QString &alias)
: port(port), slotId(slotId), alias(alias)
{

}

const Port *SlotAlias::getSourcePort() const {
    return port;
}

QString SlotAlias::getSourceSlotId() const {
    return slotId;
}

QString SlotAlias::getAlias() const {
    return alias;
}

PortAlias::PortAlias(const Port *sourcePort, const QString &alias, const QString &description)
: port(sourcePort), alias(alias), description(description)
{

}

bool PortAlias::operator ==(const PortAlias &another) {
    return (alias == another.getAlias());
}

bool PortAlias::addSlot(const SlotAlias &newSlot) {
    foreach (const SlotAlias &slot, slotAliases) {
        if (slot.getSourcePort() == newSlot.getSourcePort()) {
            if (slot.getAlias() == newSlot.getAlias() ||
                slot.getSourceSlotId() == newSlot.getSourceSlotId()) {
                return false;
            }
        }
    }

    slotAliases.append(newSlot);
    return true;
}

bool PortAlias::addSlot(const Port *sourcePort, const QString &slotId, const QString &alias) {
    SlotAlias newSlot(sourcePort, slotId, alias);

    return this->addSlot(newSlot);
}

const Port *PortAlias::getSourcePort() const {
    return port;
}

QString PortAlias::getAlias() const {
    return alias;
}

QString PortAlias::getDescription() const {
    return description;
}

const QList<SlotAlias> &PortAlias::getSlotAliases() const {
    return slotAliases;
}

void PortAlias::setNewSlotAliases(const QList<SlotAlias> &newSlotAliases) {
    slotAliases = newSlotAliases;
}

void PortAlias::setNewSourcePort(const Port *sourcePort) {
    port = sourcePort;
}

bool PortAlias::isInput() const {
    return port->isInput();
}

} // Workflow
} // U2
