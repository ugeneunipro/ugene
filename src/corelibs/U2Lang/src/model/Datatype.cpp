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

#include "Datatype.h"

namespace U2 {

/*****************************
 * DataType
 *****************************/
const QString DataType::EMPTY_TYPESET_ID = QString("empty");

DataType::DataType(const QString& id, const QString& name, const QString& desc) : Descriptor(id, name, desc){
}

DataType::DataType(const Descriptor& d) : Descriptor(d) {
}

DataType::Kind DataType::kind() const {
    return Single;
}

QList<Descriptor> DataType::getAllDescriptors()const {
    return QList<Descriptor>();
}

QMap<Descriptor, DataTypePtr> DataType::getDatatypesMap() const {
    return QMap<Descriptor, DataTypePtr>();
}

DataTypePtr DataType::getDatatypeByDescriptor(const Descriptor& idd) const {
    Q_UNUSED(idd);
    return DataTypePtr();
}

Descriptor DataType::getDatatypeDescriptor(const QString& id) const {
    QList<Descriptor> lst = getAllDescriptors();
    int idx = lst.indexOf(id);
    return idx == -1 ? "" : lst.at(idx);
}

/*****************************
 * MapDataType
 *****************************/
MapDataType::MapDataType(const Descriptor& d, const QMap<Descriptor, DataTypePtr>& m) : DataType(d), map(m) {
}

DataType::Kind MapDataType::kind() const {
    return DataType::Map;
}

DataTypePtr MapDataType::getDatatypeByDescriptor(const Descriptor& d) const {
    return map.value(d);
}

QList<Descriptor> MapDataType::getAllDescriptors() const {
    return map.uniqueKeys();
}

QMap<Descriptor, DataTypePtr> MapDataType::getDatatypesMap() const {
    return map;
}

/*****************************
 * ListDataType
 *****************************/
ListDataType::ListDataType(const Descriptor& d, DataTypePtr el) : DataType(d), listElementDatatype(el) {
}

DataType::Kind ListDataType::kind() const {
    return DataType::List;
}

DataTypePtr ListDataType::getDatatypeByDescriptor(const Descriptor& idd) const {
    Q_UNUSED(idd);
    return listElementDatatype;
}

/*****************************
 * DatatypeRegistry
 *****************************/
DataTypeRegistry::~DataTypeRegistry() {
    registry.clear();
}

DataTypePtr DataTypeRegistry::getById(const QString& id) const {
    return registry.value(id);
}

bool DataTypeRegistry::registerEntry(DataTypePtr t) {
    if (registry.contains(t->getId())) {
        return false;
    } else {
        registry.insert(t->getId(), t);
        return true;
    }
}

DataTypePtr DataTypeRegistry::unregisterEntry(const QString& id) {
    return registry.take(id);
}

QList<DataTypePtr> DataTypeRegistry::getAllEntries() const {
    return registry.values();
}

QList<QString> DataTypeRegistry::getAllIds() const {
    return registry.uniqueKeys();
}

} // U2
