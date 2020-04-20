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

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowEnv.h>

#include "SelectorValue.h"

namespace U2 {

SelectorValue::SelectorValue(const QString &_value, const QString &_replaceProtoId)
: value(_value), replaceProtoId(_replaceProtoId)
{

}

void SelectorValue::addPortMapping(const PortMapping &value) {
    portList << value;
}

void SelectorValue::setName(const QString &value) {
    name = value;
}

const QString & SelectorValue::getValue() const {
    return value;
}

const QString & SelectorValue::getProtoId() const {
    return replaceProtoId;
}

const QString & SelectorValue::getName() const {
    return name;
}

const QList<PortMapping> & SelectorValue::getMappings() const {
    return portList;
}

void SelectorValue::validate(Actor *actor, U2OpStatus &os) const {
    ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(replaceProtoId);
    if (NULL == proto) {
        os.setError(QObject::tr("Unknown actor prototype: %1").arg(replaceProtoId));
        return;
    }

    if (proto->getId() == actor->getProto()->getId()) { // no mapping
        return;
    }

    validatePortsCount(actor->getPorts(), proto->getPortDesciptors(), os);
    CHECK_OP(os, );

    QSet<QString> srcIdSet;
    foreach (const PortMapping &mapping, portList) {
        validateDuplicates(mapping, srcIdSet, os);
        CHECK_OP(os, );
        srcIdSet << mapping.getSrcId();
        PortDescriptor *srcPort = validateSrcPort(mapping, actor, os);
        CHECK_OP(os, );
        PortDescriptor *dstPort = validateDstPort(mapping, proto->getPortDesciptors(), os);
        CHECK_OP(os, );
        mapping.validate(srcPort->getOwnTypeMap(), dstPort->getOwnTypeMap(), os);
        CHECK_OP(os, );
    }

    validateMappingsCount(actor->getPorts(), os);
    CHECK_OP(os, );
}

Port * SelectorValue::validateSrcPort(const PortMapping &mapping, Actor *actor, U2OpStatus &os) const {
    Port *result = actor->getPort(mapping.getSrcId());
    if (NULL == result) {
        os.setError(QObject::tr("The actor does not contain a port with this id: %1").arg(mapping.getSrcId()));
    }
    return result;
}

PortDescriptor * SelectorValue::validateDstPort(const PortMapping &mapping,
    const QList<PortDescriptor*> &descs, U2OpStatus &os) const {
    PortDescriptor *result = NULL;
    foreach (PortDescriptor *desc, descs) {
        if (desc->getId() == mapping.getDstId()) {
            result = desc;
        }
    }
    if (NULL == result) {
        os.setError(QObject::tr("The actor prototype does not contain a port with this id: %1").arg(mapping.getDstId()));
    }
    return result;
}

void SelectorValue::validateDuplicates(const PortMapping &mapping,
    const QSet<QString> &srcIdSet, U2OpStatus &os) const {
    if (srcIdSet.contains(mapping.getSrcId())) {
        os.setError(QObject::tr("Duplicated mapping of a port: %1").arg(mapping.getSrcId()));
    }
}

void SelectorValue::validateMappingsCount(const QList<Port*> &srcPorts, U2OpStatus &os) const {
    if (portList.count() < srcPorts.count()) {
        os.setError(QObject::tr("Not all ports are mapped"));
    }
}

void SelectorValue::validatePortsCount(const QList<Port*> &src,
    const QList<PortDescriptor*> &dst, U2OpStatus &os) const {
    if (src.count() != dst.count()) {
        os.setError(QObject::tr("The actor can not be mapped with a proto: %1. Ports count is different").arg(replaceProtoId));
    }
}

} // U2
