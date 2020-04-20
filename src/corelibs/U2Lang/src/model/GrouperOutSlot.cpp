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

#include <QVariant>

#include <U2Lang/BaseTypes.h>

#include "GrouperOutSlot.h"

namespace U2 {

/************************************************************************/
/* ActionTypes */
/************************************************************************/
const QString ActionTypes::MERGE_SEQUENCE       = QString("merge-sequence");
const QString ActionTypes::SEQUENCE_TO_MSA      = QString("sequence-to-msa");
const QString ActionTypes::MERGE_MSA            = QString("merge-msa");
const QString ActionTypes::MERGE_STRING         = QString("merge-string");
const QString ActionTypes::MERGE_ANNS           = QString("merge-annotations");

bool ActionTypes::isValidType(const QString &type) {
    bool result = false;

    result |= (MERGE_SEQUENCE == type);
    result |= (SEQUENCE_TO_MSA == type);
    result |= (MERGE_MSA == type);
    result |= (MERGE_STRING == type);
    result |= (MERGE_ANNS == type);

    return result;
}

DataTypePtr ActionTypes::getDataTypeByAction(const QString &actionType) {
    if (MERGE_SEQUENCE == actionType) {
        return BaseTypes::DNA_SEQUENCE_TYPE();
    } else if (SEQUENCE_TO_MSA == actionType) {
        return BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    } else if (MERGE_MSA == actionType) {
        return BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    } else if (MERGE_STRING == actionType) {
        return BaseTypes::STRING_TYPE();
    } else if (MERGE_ANNS == actionType) {
        return BaseTypes::ANNOTATION_TABLE_TYPE();
    } else {
        assert(false);
        return DataTypePtr();
    }
}

/************************************************************************/
/* ActionParameters */
/************************************************************************/
const QString ActionParameters::GAP             = QString("gap");
const QString ActionParameters::UNIQUE          = QString("unique");
const QString ActionParameters::SEPARATOR       = QString("separator");
const QString ActionParameters::MSA_NAME        = QString("msa-name");
const QString ActionParameters::SEQ_NAME        = QString("seq-name");
const QString ActionParameters::SEQ_SLOT        = QString("seq-slot");

ActionParameters::ParameterType ActionParameters::getType(const QString &parameter) {
    if (GAP == parameter) {
        return INTEGER;
    } else if (UNIQUE == parameter) {
        return BOOLEAN;
    } else if (SEPARATOR == parameter) {
        return STRING;
    } else if (MSA_NAME == parameter) {
        return STRING;
    } else if (SEQ_NAME == parameter) {
        return STRING;
    } else if (SEQ_SLOT == parameter) {
        return STRING;
    }

    assert(false);
    return STRING;
}

bool ActionParameters::isValidParameter(const QString &actionType, const QString &parameter) {
    bool result = false;
    if (ActionTypes::MERGE_SEQUENCE == actionType) {
        result |= (GAP == parameter);
        result |= (SEQ_NAME == parameter);
    } else if (ActionTypes::SEQUENCE_TO_MSA == actionType) {
        result |= (UNIQUE == parameter);
        result |= (MSA_NAME == parameter);
    } else if (ActionTypes::MERGE_MSA == actionType) {
        result |= (UNIQUE == parameter);
        result |= (MSA_NAME == parameter);
    } else if (ActionTypes::MERGE_STRING == actionType) {
        result |= (SEPARATOR == parameter);
    } else if (ActionTypes::MERGE_ANNS == actionType) {
        result |= (UNIQUE == parameter);
        result |= (SEQ_SLOT == parameter);
    }

    return result;
}

/************************************************************************/
/* GrouperSlotAction */
/************************************************************************/
GrouperSlotAction::GrouperSlotAction(const QString &type)
: type(type)
{

}

GrouperSlotAction::GrouperSlotAction(const GrouperSlotAction &other)
: type(other.type), parameters(other.parameters)
{

}

QString GrouperSlotAction::getType() const {
    return type;
}

const QVariantMap &GrouperSlotAction::getParameters() const {
    return parameters;
}

bool GrouperSlotAction::hasParameter(const QString &parameterId) const {
    return parameters.contains(parameterId);
}

QVariant GrouperSlotAction::getParameterValue(const QString &parameterId) const {
    return parameters.value(parameterId, QVariant());
}

void GrouperSlotAction::setParameterValue(const QString &parameterId, const QVariant &value) {
    parameters[parameterId] = value;
}

/************************************************************************/
/* GroupOperations */
/************************************************************************/
Descriptor GroupOperations::BY_VALUE() {
    return Descriptor("by-value", QObject::tr("By value"), QObject::tr("By value"));
}

Descriptor GroupOperations::BY_NAME() {
    return Descriptor("by-name", QObject::tr("By name"), QObject::tr("By name"));
}

Descriptor GroupOperations::BY_ID() {
    return Descriptor("by-id", QObject::tr("By id"), QObject::tr("By id"));
}

/************************************************************************/
/* GrouperOutSlot */
/************************************************************************/
GrouperOutSlot::GrouperOutSlot(const QString &outSlotId, const QString &inSlotStr)
: outSlotId(outSlotId), inSlotStr(inSlotStr), action(NULL)
{

}

GrouperOutSlot::GrouperOutSlot(const GrouperOutSlot &another) {
    outSlotId = another.outSlotId;
    inSlotStr = another.inSlotStr;
    if (NULL == another.action) {
        action = NULL;
    } else {
        action = new GrouperSlotAction(*another.action);
    }
}

GrouperOutSlot::~GrouperOutSlot() {
    delete action;
}

bool GrouperOutSlot::operator==(const GrouperOutSlot &other) const {
    return this->outSlotId == other.outSlotId;
}

GrouperSlotAction *GrouperOutSlot::getAction() {
    return action;
}

GrouperSlotAction *GrouperOutSlot::getAction() const {
    return action;
}

void GrouperOutSlot::setAction(const GrouperSlotAction &action) {
    delete this->action;
    this->action = new GrouperSlotAction(action);
}

QString GrouperOutSlot::getOutSlotId() const {
    return outSlotId;
}

void GrouperOutSlot::setOutSlotId(const QString &outSlotId) {
    this->outSlotId = outSlotId;
}

QString GrouperOutSlot::getInSlotStr() const {
    return inSlotStr;
}

void GrouperOutSlot::setInSlotStr(const QString &slotStr) {
    this->inSlotStr = slotStr;
}

void GrouperOutSlot::setBusMapInSlotStr(const QString &busMapSlotStr) {
    QString result = busMapSlotStr;
    result.replace(":", ".");

    this->inSlotStr = result;
}

QString GrouperOutSlot::getBusMapInSlotId() const {
    QString result = inSlotStr;
    result.replace(".", ":");

    return result;
}

QString GrouperOutSlot::readable2busMap(const QString &readableSlotStr) {
    QString result = readableSlotStr;
    result.replace(".", ":");

    return result;
}

QString GrouperOutSlot::busMap2readable(const QString &busMapSlotStr) {
    QString result = busMapSlotStr;
    result.replace(":", ".");

    return result;
}

} // U2
