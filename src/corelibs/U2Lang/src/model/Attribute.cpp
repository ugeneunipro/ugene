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

#include <cassert>

#include <QStringList>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowUtils.h>
#include "Attribute.h"

namespace U2 {
using namespace WorkflowSerialize;

/*************************************
 *  Attribute
 *************************************/
Attribute::Attribute(const Descriptor &_descriptor, const DataTypePtr _type, const Flags _flags, const QVariant &_defaultValue)
    : Descriptor(_descriptor),
      type(_type),
      flags(_flags),
      defaultValue(_defaultValue)
{
    value = defaultValue;
    debugCheckAttributeId();
}

Attribute::Attribute(const Descriptor& d, const DataTypePtr t, bool req, const QVariant & defaultValue )
: Descriptor(d), type(t), defaultValue(defaultValue) {
    flags |= req ? Required : None;
    value = defaultValue;
    debugCheckAttributeId();
}

Attribute::~Attribute() {
    qDeleteAll(relations);
    qDeleteAll(portRelations);
}

void Attribute::debugCheckAttributeId() const {
    QString id = getId(); Q_UNUSED(id);
    assert(id != Constants::TYPE_ATTR);
    assert(id != Constants::NAME_ATTR);
    assert(id != Constants::SCRIPT_ATTR);
    assert(id != Constants::ELEM_ID_ATTR);
}

void Attribute::copy(const Attribute &other) {
    this->Descriptor::operator =(other);

    type = other.type;
    flags = other.flags;
    value = other.value;
    defaultValue = other.defaultValue;
    scriptData = other.scriptData;

    qDeleteAll(relations);
    relations.clear();
    foreach (const AttributeRelation *relation, other.relations) {
        relations << relation->clone();
    }

    qDeleteAll(portRelations);
    portRelations.clear();
    foreach(const PortRelationDescriptor* portRelation, other.portRelations) {
        portRelations << portRelation->clone();
    }

    slotRelations.clear();
    slotRelations = other.slotRelations;
}

Attribute::Attribute(const Attribute &other)
    : Descriptor(other)
{
    copy(other);
}

Attribute &Attribute::operator =(const Attribute &other) {
    CHECK(this != &other, *this);
    copy(other);
    return *this;
}

const DataTypePtr Attribute::getAttributeType()const {
    return type;
}

bool Attribute::isRequiredAttribute() const {
    return flags.testFlag(Required);
}

bool Attribute::canBeEmpty() const {
    return flags.testFlag(CanBeEmpty);
}

bool Attribute::needValidateEncoding() const {
    return flags.testFlag(NeedValidateEncoding);
}

Attribute::Flags Attribute::getFlags() const {
    return flags;
}

void Attribute::setAttributeValue(const QVariant & newVal) {
    if (QVariant() == newVal) {
        value = defaultValue;
    } else {
        value = newVal;
    }
}

const QVariant &Attribute::getAttributePureValue() const {
    return value;
}

const QVariant &Attribute::getDefaultPureValue() const {
    return defaultValue;
}

bool Attribute::isDefaultValue() const {
    return (value == defaultValue);
}

const AttributeScript & Attribute::getAttributeScript() const {
    return scriptData;
}

AttributeScript & Attribute::getAttributeScript() {
    return scriptData;
}

QVariant Attribute::toVariant() const {
    QVariantList res;
    res << value;
    res << qVariantFromValue<QString>(scriptData.getScriptText());
    QVariantList scriptVars;
    foreach( const Descriptor & varDesc, scriptData.getScriptVars().keys() ) {
        scriptVars << qVariantFromValue<QString>(varDesc.getId());
    }
    res << QVariant(scriptVars);
    return res;
}

bool Attribute::fromVariant(const QVariant& variant) {
    if( !variant.canConvert( QVariant::List ) ) {
        return false;
    }
    QVariantList args = variant.toList();
    if( args.size() != 3 ) {
        return false;
    }
    value = args.at(0);
    QVariant scriptTextVal = args.at(1);
    QString scriptText;
    if( scriptTextVal.canConvert(QVariant::String) ) {
        scriptText = scriptTextVal.toString();
    }
    scriptData.setScriptText(scriptText);

    QVariant descs = args.at(2);
    if( descs.canConvert(QVariant::List) ) {
        QVariantList descList = descs.toList();
        for( int i = 0; i < descList.size(); ++i ) {
            scriptData.setScriptVar(Descriptor(descList.at(i).value<QString>(), "", "" ), QVariant());
        }
    }
    return true;
}

bool Attribute::isEmpty() const {
    return !value.isValid() || value.isNull();
}

bool Attribute::isEmptyString() const {
    return value.type() == QVariant::String && getAttributeValueWithoutScript<QString>().isEmpty();
}

void Attribute::addRelation(const AttributeRelation *relation) {
    relations.append(relation);
}

QVector<const AttributeRelation*> &Attribute::getRelations() {
    return relations;
}

void Attribute::addPortRelation(PortRelationDescriptor* relationDesc) {
    portRelations << relationDesc;
}

const QList<PortRelationDescriptor*>& Attribute::getPortRelations() const {
    return portRelations;
}

void Attribute::addSlotRelation(const SlotRelationDescriptor& relationDesc) {
    slotRelations << relationDesc;
}

const QList<SlotRelationDescriptor>& Attribute::getSlotRelations() const {
    return slotRelations;
}

Attribute *Attribute::clone() {
    return new Attribute(*this);
}

AttributeGroup Attribute::getGroup() {
    return COMMON_GROUP;
}

void Attribute::updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap) {
    Q_UNUSED(actorIdsMap);
}

bool Attribute::validate(ProblemList &problemList) {
    if(!isRequiredAttribute() || canBeEmpty()) {
        return true;
    }
    if( (isEmpty() || isEmptyString()) && getAttributeScript().isEmpty()) {
        problemList.append(Problem(U2::WorkflowUtils::tr("Required parameter is not set: %1").arg(getDisplayName())));
        return false;
    }
    return true;
}

/*************************************
*  AttributeScript
*************************************/
AttributeScript::AttributeScript(const QString & t) : text(t) {}

bool AttributeScript::isEmpty() const {
    return text.isEmpty();
}

void AttributeScript::setScriptText(const QString & t) {
    text = t;
}

const QString & AttributeScript::getScriptText() const {
    return text;
}

const QMap<Descriptor, QVariant> & AttributeScript::getScriptVars() const {
    return vars;
}

void AttributeScript::clearScriptVars() {
    vars.clear();
}

void AttributeScript::setScriptVar(const Descriptor & desc, const QVariant & val) {
    vars.insert(desc, val);
}

bool AttributeScript::hasVarWithId(const QString & varName) const {
    foreach(const Descriptor & varDesc, vars.keys()) {
        if( varDesc.getId() == varName ) {
            return true;
        }
    }
    return false;
}

bool AttributeScript::hasVarWithDesc(const QString & varName) const {
    foreach(const Descriptor & varDesc, vars.keys()) {
        if( varDesc.getDisplayName() == varName ) {
            return true;
        }
    }
    return false;
}

void AttributeScript::setVarValueWithId(const QString & varName, const QVariant & value) {
    foreach(const Descriptor & varDesc, vars.keys()) {
        if( varDesc.getId() == varName ) {
            vars[varDesc] = value;
            break;
        }
    }
}

} // U2
