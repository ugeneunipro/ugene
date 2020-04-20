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

#include "IntegralBus.h"
#include "IntegralBusType.h"
#include "IntegralBusModel.h"

#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {
namespace Workflow {

IntegralBusType::IntegralBusType(const Descriptor& d, const QMap<Descriptor, DataTypePtr>& m) : MapDataType(d, m) {
}

Descriptor IntegralBusType::assignSlotDesc(const Descriptor& d, const Port* p) {
    QString id = QString("%1:%2").arg(p->owner()->getId()).arg(d.getId());
    QString name = U2::Workflow::IntegralBusPort::tr("%1 (by %2)").arg(d.getDisplayName()).arg(p->owner()->getLabel());
    QString doc = d.getDocumentation();
    return Descriptor(id, name, doc);
}

ActorId IntegralBusType::parseSlotDesc(const QString& id) {
    U2OpStatus2Log os;
    IntegralBusSlot slot = IntegralBusSlot::fromString(id, os);
    return slot.actorId();
}

QString IntegralBusType::parseAttributeIdFromSlotDesc(const QString & str) {
    U2OpStatus2Log os;
    IntegralBusSlot slot = IntegralBusSlot::fromString(str, os);
    return slot.getId();
}

void IntegralBusType::remapSlotString(QString &slotStr, const QMap<ActorId, ActorId> &actorIdsMap) {
    U2OpStatus2Log os;
    IntegralBusSlot slot = IntegralBusSlot::fromString(slotStr, os);
    SAFE_POINT_OP(os, );

    ActorId oldId = slot.actorId();
    if (actorIdsMap.contains(oldId)) {
        ActorId newId = actorIdsMap[oldId];
        slot.replaceActorId(oldId, newId);
        QString newSlotStr = slot.toString();
        coreLog.trace("remapping old="+slotStr+" to new="+newSlotStr);
        slotStr = newSlotStr;
    }
}

void IntegralBusType::remap(StrStrMap& busMap, const QMap<ActorId, ActorId>& m) {
    foreach(QString key, busMap.uniqueKeys()) {
        QStringList newValList;
        foreach(QString val, busMap.value(key).split(";")) {
            remapSlotString(val, m);
            newValList.append(val);
        }
        busMap.insert(key, newValList.join(";"));
    }
}

void IntegralBusType::remapPaths(SlotPathMap &pathsMap, const QMap<ActorId, ActorId> &actorIdsMap) {
    SlotPathMap newPathsMap;
    foreach (const SlotPair &slotsPair, pathsMap.keys()) {
        QStringList oldPath = pathsMap.value(slotsPair, QStringList());
        QStringList newPath;
        foreach (const QString &idStr, oldPath) {
            ActorId oldId = str2aid(idStr);
            if (actorIdsMap.contains(oldId)) {
                const ActorId &newId = actorIdsMap[oldId];
                newPath << aid2str(newId);
            } else {
                newPath << aid2str(oldId);
            }
        }

        SlotPair newSlotPair(slotsPair);
        remapSlotString(newSlotPair.second, actorIdsMap);

        newPathsMap[newSlotPair] = newPath;
    }
    pathsMap = newPathsMap;
}

inline static QString getNewDisplayName(const QString &oldName, const QString &procName, const QString &slotStr) {
    QString slotId;
    QStringList path;
    QString result = oldName;
    BusMap::parseSource(slotStr, slotId, path);
    if (path.isEmpty()) {
        result += " through " + procName;
    } else {
        result += ", " + procName;
    }

    return result;
}

void IntegralBusType::addInputs(const Port* p, bool addPaths) {
    if (p->isInput()) {
        Actor *proc = p->owner();
        ActorPrototype *proto = proc->getProto();
        QString grouperSlot;
        if (proto->getId() == CoreLibConstants::GROUPER_ID) {
            QString slotStr = proc->getParameter(CoreLibConstants::GROUPER_SLOT_ATTR)->getAttributePureValue().toString();
            grouperSlot = GrouperOutSlot::readable2busMap(slotStr);
        }

        foreach(Port* peer, p->getLinks().uniqueKeys()) {
            DataTypePtr pt = peer->getType();
            if (qobject_cast<IntegralBusPort*>(peer)) {
                assert(pt->isMap());
                QMap<Descriptor, DataTypePtr> types = pt->getDatatypesMap();
                foreach (Descriptor d, types.keys()) {
                    DataTypePtr typePtr = types.value(d);
                    if (addPaths) {
                        QString newName = getNewDisplayName(d.getDisplayName(), proc->getLabel(), d.getId());
                        QString newId = BusMap::getNewSourceId(d.getId(), proc->getId());
                        d.setId(newId);
                        d.setDisplayName(newName);
                    }
                    if (proto->getId() == CoreLibConstants::GROUPER_ID) {
                        if (grouperSlot == d.getId()) {
                            map[d] = typePtr;
                        } else {
                            continue;
                        }
                    }
                    map[d] = typePtr;
                }
            } else {
                addOutput(pt, peer);
            }
        }
    }
}

void IntegralBusType::addOutput(DataTypePtr t, const Port* producer) {

    if (t->isMap()) {
        foreach(Descriptor d, t->getAllDescriptors()) {
            map[assignSlotDesc(d, producer)] = t->getDatatypeByDescriptor(d);
        }
    } else {
        map[assignSlotDesc(*producer, producer)] = t;
    }

}

}//Workflow namespace
}//GB2namespace
