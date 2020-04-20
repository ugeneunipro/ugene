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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchemaRegistry.h>

#include "UdrSchema.h"

namespace U2 {

const QByteArray UdrSchema::RECORD_ID_FIELD_NAME("record_id");
const QByteArray UdrSchema::OBJECT_FIELD_NAME("object");
const int UdrSchema::OBJECT_FIELD_NUM = 0;

UdrSchema::FieldDesc::FieldDesc(const QByteArray &name, UdrSchema::DataType dataType, UdrSchema::IndexType indexType)
: name(name), dataType(dataType), indexType(indexType)
{

}

const QByteArray UdrSchema::FieldDesc::getName() const {
    return name;
}

UdrSchema::DataType UdrSchema::FieldDesc::getDataType() const {
    return dataType;
}

UdrSchema::IndexType UdrSchema::FieldDesc::getIndexType() const {
    return indexType;
}

UdrSchema::UdrSchema(const UdrSchemaId &id, bool useObjectReference)
: id(id), withObjectReference(useObjectReference)
{
    if (useObjectReference) {
        U2OpStatusImpl os;
        addField(FieldDesc(OBJECT_FIELD_NAME, ID, INDEXED), os);
        SAFE_POINT_OP(os, );
    }
}

bool UdrSchema::contains(const QByteArray &name) const {
    CHECK(RECORD_ID_FIELD_NAME != name, true);

    foreach (const FieldDesc &field, fields) {
        if (name == field.getName()) {
            return true;
        }
    }
    return false;
}

void UdrSchema::addField(const FieldDesc &desc, U2OpStatus &os) {
    CHECK_EXT(!contains(desc.getName()), os.setError("Duplicate name"), );
    if (BLOB == desc.getDataType()) {
        CHECK_EXT(NOT_INDEXED == desc.getIndexType(), os.setError("BLOB data can not be indexed"), );
    }
    CHECK_EXT(UdrSchemaRegistry::isCorrectName(desc.getName()), os.setError("Incorrect field name"), );
    fields << desc;
}

void UdrSchema::addMultiIndex(const QList<int> &multiIndex, U2OpStatus &os) {
    CHECK_EXT(multiIndex.toSet().size() == multiIndex.size(), os.setError("Not unique fields"), );
    foreach (int fieldNum, multiIndex) {
        FieldDesc field = getField(fieldNum, os);
        CHECK_OP(os, );
        CHECK_EXT(BLOB != field.getDataType(), os.setError("BLOB data can not be indexed"), );
    }
    CHECK_EXT(!multiIndexes.contains(multiIndex), os.setError("Duplicate multi index"), );
    multiIndexes << multiIndex;
}

const QList< QList<int> > & UdrSchema::getMultiIndexes() const {
    return multiIndexes;
}

const UdrSchemaId & UdrSchema::getId() const {
    return id;
}

int UdrSchema::size() const {
    return fields.size();
}

UdrSchema::FieldDesc UdrSchema::getField(int fieldNum, U2OpStatus &os) const {
    CHECK_EXT(0 <= fieldNum && fieldNum < size(), os.setError("Out of range"), FieldDesc("", INTEGER));
    return fields[fieldNum];
}

QStringList UdrSchema::fieldNames(const UdrSchema *schema, U2OpStatus &os, const QList<int> &nums) {
    QStringList result;
    QList<int> target = nums;
    if (nums.isEmpty()) {
        for (int i=0; i<schema->size(); i++) {
            target << i;
        }
    }
    foreach (int fieldNum, target) {
        UdrSchema::FieldDesc field = schema->getField(fieldNum, os);
        CHECK_OP(os, result);
        result << field.getName();
    }
    return result;
}

QList<int> UdrSchema::notBinary(const UdrSchema *schema, U2OpStatus &os) {
    QList<int> result;
    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, result);
        if (UdrSchema::BLOB != field.getDataType()) {
            result << i;
        }
    }
    return result;
}

UdrSchema::FieldDesc UdrSchema::getBlobField(const UdrSchema *schema, int fieldNum, U2OpStatus &os) {
    UdrSchema::FieldDesc field = schema->getField(fieldNum, os);
    CHECK_OP(os, field);

    if (UdrSchema::BLOB != field.getDataType()) {
        os.setError("Only BLOB fields can be used");
    }
    return field;
}

bool UdrSchema::hasObjectReference() const {
    return withObjectReference;
}

} // U2
