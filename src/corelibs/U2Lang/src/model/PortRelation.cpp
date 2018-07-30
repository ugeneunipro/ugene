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

#include "PortRelation.h"

namespace U2 {

PortRelationDescriptor::PortRelationDescriptor(const QString& portId, const QVariantList& valuesWithEnabledPort)
    : portId(portId), valuesWithEnabledPort(valuesWithEnabledPort) {}

PortRelationDescriptor::~PortRelationDescriptor() {}

bool PortRelationDescriptor::isPortEnabled(const QVariant& attrValue) const {
    return valuesWithEnabledPort.contains(attrValue);
}

PortRelationDescriptor* PortRelationDescriptor::clone() const {
    return new PortRelationDescriptor(*this);
}

const QVariantList& PortRelationDescriptor::getValuesWithEnabledPort() const {
    return valuesWithEnabledPort;
}

const QString& PortRelationDescriptor::getPortId() const {
    return portId;
}


} // U2 namespace
