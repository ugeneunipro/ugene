/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "Port.h"

namespace U2 {

namespace Workflow {

/**************************
* PortDescriptor
**************************/
PortDescriptor::PortDescriptor(const Descriptor& desc, DataTypePtr t, bool input, bool multi, uint f )
    : Descriptor(desc), type(t), input(input), multi(multi), flags(f) {
    assert(type->isMap());
    defaultType = type;
    if(input) {
        assert(!multi && "currently wd model cannot handle multi input ports");
    }
}

bool PortDescriptor::isInput() const {
    return input;
}

bool PortDescriptor::isOutput() const {
    return !input;
}

bool PortDescriptor::isMulti() const {
    return multi;
}

uint PortDescriptor::getFlags() const {
    return flags;
}

DataTypePtr PortDescriptor::getType() const {
    return type;
}

DataTypePtr PortDescriptor::getOutputType() const {
    return type;
}

void PortDescriptor::setNewType(const DataTypePtr &newType) {
    type = newType;
}

QMap<Descriptor, DataTypePtr> PortDescriptor::getOwnTypeMap() const {
    if (type->isMap()) {
        return type->getDatatypesMap();
    } else {
        QMap<Descriptor, DataTypePtr> result;
        Descriptor d = (Descriptor &)(*this);
        result[d] = type;

        return result;
    }
}

void PortDescriptor::setVisibleSlot(const QString& slotId, const bool isVisible) {
    QMap<Descriptor, DataTypePtr> newMap = type->getDatatypesMap();
    if (isVisible) {
        QMap<Descriptor, DataTypePtr> defaultMap = defaultType->getDatatypesMap();
        foreach(const Descriptor& desc, defaultMap.keys()) {
            if (slotId == desc.getId()) {
                newMap[desc] = defaultMap[desc];
            }
        }
    } else {
        foreach(const Descriptor& desc, newMap.keys()) {
            if (slotId == desc.getId()) {
                newMap.remove(desc);
            }
        }
    }
    const QString id = type.data()->getId();
    type = DataTypePtr(new MapDataType(id, newMap));
}

QString PortDescriptor::getSlotNameById(const QString& id) const {
    QMap<Descriptor, DataTypePtr> map = getOwnTypeMap();
    QString result;
    foreach(const Descriptor& descriptor, map.keys()) {
        CHECK_CONTINUE(descriptor.getId() == id);

        result = descriptor.getDisplayName();
        break;
    }
    return result;
}

/**************************
* Port
**************************/
Port::Port(const PortDescriptor& d, Actor* p) : PortDescriptor(d), proc(p), enabled(true) {
}

Actor * Port::owner() const {
    return proc;
}

QMap<Port*, Link*> Port::getLinks() const {
    return bindings;
}

int Port::getWidth() const {
    return bindings.size();
}

void Port::setParameter(const QString& name, const QVariant& val) {
    Configuration::setParameter(name, val);
    emit bindingChanged();
}

void Port::remap(const QMap<ActorId, ActorId>&) {
    // nothing to do with the port
}

void Port::updateBindings(const QMap<ActorId, ActorId>&) {
    // nothing to do with the port
}

void Port::replaceActor(Actor * /*oldActor*/, Actor * /*newActor*/, const QList<PortMapping> & /*mappings*/) {
    // nothing to do with the port
}

bool Port::canBind(const Port* other) const {
    if (this == other || proc == other->proc || isInput() == other->isInput()) {
        return false;
    }
    bool thisMulty = isMulti();
    int thisWidth = getWidth();
    bool otherMulty = other->isMulti();
    int otherWidth = other->getWidth();
    if ((!thisMulty && thisWidth != 0) || (!otherMulty && otherWidth != 0)) {
        return false;
    }
    return !bindings.contains(const_cast<Port*>(other));
}

void Port::addLink(Link* b) {
    Port* peer = isInput()? b->source() : b->destination();
    assert(this == (isInput()? b->destination() : b->source()));
    //assert(canBind(peer));
    assert(!bindings.contains(peer));
    bindings[peer] = b;
    emit bindingChanged();
}

void Port::removeLink(Link* b) {
    Port* peer = isInput()? b->source() : b->destination();
    assert(this == (isInput()? b->destination() : b->source()));
    assert(bindings.contains(peer));
    bindings.remove(peer);
    emit bindingChanged();
}

void Port::setEnabled(bool _enabled) {
    if(enabled != _enabled) {
        enabled = _enabled;
        emit si_enabledChanged(enabled);
    }
}

/**************************
* Link
**************************/
Link::Link()  : src(NULL), dest(NULL) {
}

Link::Link(Port* p1, Port* p2) {
    connect(p1, p2);
}

Port * Link::source() const {
    return src;
}

Port * Link::destination() const {
    return dest;
}

void Link::connect(Port* p1, Port* p2) {
    assert(p1->canBind(p2));
    if (p1->isInput()) {
        dest = p1;
        src = p2;
    } else {
        dest = p2;
        src = p1;
    }
    p1->addLink(this);
    p2->addLink(this);
}

void Link::disconnect() {
    if (NULL != src && NULL != dest) {
        src->removeLink(this);
        dest->removeLink(this);
    }
}

}

} // U2
