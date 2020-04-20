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

#include <cassert>

#include "Descriptor.h"

namespace U2 {

/**************************
 * Descriptor
 **************************/
Descriptor::Descriptor(const QString& id, const QString& name, const QString& doc) : id(id), name(name), desc(doc) {
}

Descriptor::Descriptor(const QString& _id) : id(_id), name(_id), desc(_id) {
}

Descriptor::Descriptor(const char* _id) : id(_id), name(_id), desc(_id) {
}

Descriptor::Descriptor() {
}

QString Descriptor::getId() const {
    return id;
}

QString Descriptor::getDisplayName() const {
    return name;
}

QString Descriptor::getDocumentation() const {
    return desc;
}

void Descriptor::setId(const QString& i) {
    id = i;
}

void Descriptor::setDocumentation(const QString& d) {
    desc = d;
}

void Descriptor::setDisplayName(const QString& n) {
    name = n;
}

/**************************
 * VisualDescriptor
 **************************/
VisualDescriptor::VisualDescriptor(const Descriptor& d, const QString & _iconPath) : Descriptor(d), iconPath(_iconPath) {
}

void VisualDescriptor::setIconPath( const QString & ip ) {
    iconPath = ip;
}

QIcon VisualDescriptor::getIcon() {
    if( icon.isNull() && !iconPath.isEmpty() ) {
        icon = QIcon(iconPath);
    }
    return icon;
}

void VisualDescriptor::setIcon( QIcon i ) {
    assert( iconPath.isEmpty() );
    icon = i;
}

} // U2
