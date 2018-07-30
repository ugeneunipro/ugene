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

#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorPrototype.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Lang/GrouperSlotAttribute.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>

#include "Actor.h"
#include "ActorDocument.h"
#include "support/IntegralBusUtils.h"

namespace U2 {
namespace Workflow {

Actor::Actor(const ActorId &actorId, ActorPrototype* proto, AttributeScript * _script)
: id(actorId), proto(proto), doc(NULL), script(_script), condition(new AttributeScript()) {
    if(script == NULL) {
        if(proto->isScriptFlagSet()) {
            script = new AttributeScript();
            script->setScriptText("");
        }
        else {
            script = NULL;
        }
    }

    if(script != NULL) {
        setupVariablesForPort(script);
        setupVariablesForAttribute(script);
    }
    setupVariablesForPort(condition, true);
    setupVariablesForAttribute(condition);
}

Actor::Actor(const Actor&) : QObject(), Configuration(), Peer() {
    assert(false);
}

Actor::~Actor() {
    qDeleteAll(ports.values());
    delete doc;
    delete script;
    delete condition;
}

void Actor::setupVariablesForPort(AttributeScript *_script, bool inputOnly) {
    foreach(const PortDescriptor *descr, proto->getPortDesciptors()) {
        QString prefix;
        if(descr->isInput()) {
            prefix = "in_";
        }
        else if (!inputOnly) {
            prefix = "out_";
        } else {
            continue;
        }

        DataTypePtr dataTypePtr = descr->getType();
        if(dataTypePtr->isMap()) {
            QMap<Descriptor, DataTypePtr> map = dataTypePtr->getDatatypesMap();
            foreach(const Descriptor & d, map.keys()) {
                Descriptor var(prefix + d.getId(), d.getDisplayName(), d.getDocumentation());
                _script->setScriptVar(var, QVariant());
            }
        }
        else  if(dataTypePtr->isList()) {
            foreach(const Descriptor & typeDescr, dataTypePtr->getAllDescriptors()) {
                Descriptor var(prefix + typeDescr.getId(), typeDescr.getDisplayName(), typeDescr.getDocumentation());
                _script->setScriptVar(var, QVariant());
            }
        }
        else {
            QString id = prefix + dataTypePtr->getId();
            QString displayName = dataTypePtr->getDisplayName();
            QString doc = prefix + dataTypePtr->getDocumentation();
            _script->setScriptVar(Descriptor(id,displayName,doc), QVariant());
        }
    }
}

void Actor::setupVariablesForAttribute(AttributeScript *_script) {
    foreach( Attribute * attribute, proto->getAttributes() ) {
        assert(attribute != NULL);
        QString attrVarName = attribute->getDisplayName();
        _script->setScriptVar(Descriptor(attribute->getId().replace(".","_"), attrVarName.replace(".", "_"), attribute->getDocumentation()), QVariant());
    }
}

AttributeScript * Actor::getScript() const {
    return script;
}

void Actor::setScript(AttributeScript* _script) {
    script->setScriptText(_script->getScriptText());
}

AttributeScript *Actor::getCondition() const {
    return condition;
}

ActorId Actor::getOwner() const {
    return owner;
}

void Actor::setOwner(const ActorId &owner) {
    this->owner = owner;
}

void Actor::updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap) {
    if (actorIdsMap.contains(owner)) {
        owner = actorIdsMap[owner];
    }

    foreach (Attribute *a, this->getAttributes()) {
        a->updateActorIds(actorIdsMap);
    }
}

ActorPrototype * Actor::getProto() const {
    return proto;
}

ActorId Actor::getId() const {
    return id;
}

void Actor::setId(const ActorId &id) {
    this->id = id;
}

QString Actor::getLabel() const {
    if( label.isEmpty() ) {
        return QString("%1 %2").arg(getProto()->getDisplayName()).arg(getId());
    } else {
        return label;
    }
}

void Actor::setLabel(const QString& l) {
    label = l;
    emit si_labelChanged();
}

Port * Actor::getPort(const QString& id) const {
    return ports.value(id);
}

QList<Port*> Actor::getPorts() const {
    return ports.values();
}

QList<Port*> Actor::getInputPorts() const {
    QList<Port*> l;
    foreach (Port* p, ports.values()) {
        if (p->isInput()) {
            l<<p;
        }
    }
    return l;
}

QList<Port*> Actor::getOutputPorts() const {
    QList<Port*> l;
    foreach (Port* p, ports.values()) {
        if (p->isOutput()) {
            l<<p;
        }
    }
    return l;
}

QList<Port*> Actor::getEnabledPorts() const {
    QList<Port*> l;
    foreach (Port* p, ports.values()) {
        if(p->isEnabled()) {
            l<<p;
        }
    }
    return l;
}

QList<Port*> Actor::getEnabledInputPorts() const {
    QList<Port*> l;
    foreach (Port* p, ports.values()) {
        if (p->isEnabled() && p->isInput()) {
            l<<p;
        }
    }
    return l;
}

QList<Port*> Actor::getEnabledOutputPorts() const {
    QList<Port*> l;
    foreach (Port* p, ports.values()) {
        if (p->isEnabled() && p->isOutput()) {
            l<<p;
        }
    }
    return l;
}

void Actor::setParameter(const QString& name, const QVariant& val) {
    Configuration::setParameter(name, val);
    updateItemsAvailability(getParameter(name));
    emit si_modified();
}

ActorDocument * Actor::getDescription() const {
    return doc;
}

void Actor::setDescription(ActorDocument* d) {
    assert(d != NULL);
    doc = d;
}

const QMap<QString, QString> & Actor::getParamAliases() const {
    return paramAliases;
}

QMap<QString, QString> & Actor::getParamAliases() {
    return paramAliases;
}

bool Actor::hasParamAliases() const {
    return !paramAliases.isEmpty();
}

const QMap<QString, QString> & Actor::getAliasHelp() const {
    return aliasHelpDescs;
}

QMap<QString, QString> & Actor::getAliasHelp() {
    return aliasHelpDescs;
}

bool Actor::hasAliasHelp() const {
    foreach(const QString & alias, paramAliases.values()) {
        if(aliasHelpDescs.contains(alias)) {
            return true;
        }
    }
    return false;
}

void Actor::remap(const QMap<ActorId, ActorId>& m) {
    foreach(Port* p, ports) {
        p->remap(m);
    }
}

void Actor::update(const QMap<ActorId, ActorId> &actorsMapping) {
    foreach (Port *p, getPorts()) {
        p->updateBindings(actorsMapping);
    }
    if (CoreLibConstants::GROUPER_ID == proto->getId()) {
        updateGrouperSlots(actorsMapping);
    }
}

void Actor::updateGrouperSlots(const QMap<ActorId, ActorId> &actorsMapping) {
    SAFE_POINT(1 == getOutputPorts().size(), "Grouper port error 1", );
    SAFE_POINT(1 == getInputPorts().size(), "Grouper port error 2", );
    Port *outPort = getOutputPorts().first();
    SAFE_POINT(outPort->getOutputType()->isMap(), "Grouper port error 3", );
    QMap<Descriptor, DataTypePtr> outBusMap = outPort->getOutputType()->getDatatypesMap();
    QMap<Descriptor, DataTypePtr> inBusMap;
    {
        Port *inPort = getInputPorts().first();
        inBusMap = WorkflowUtils::getBusType(inPort);
    }

    // update in slot attribute
    {
        Attribute *attr = getParameter(CoreLibConstants::GROUPER_SLOT_ATTR);
        QString groupSlot = attr->getAttributeValueWithoutScript<QString>();
        if (!groupSlot.isEmpty()) {
            groupSlot = GrouperOutSlot::readable2busMap(groupSlot);
            U2OpStatus2Log logOs;
            IntegralBusSlot slot = IntegralBusSlot::fromString(groupSlot, logOs);
            foreach (const ActorId &oldId, actorsMapping.keys()) {
                slot.replaceActorId(oldId, actorsMapping[oldId]);
            }
            groupSlot = slot.toString();
            bool found = false;
            foreach (const Descriptor &d, inBusMap.keys()) {
                if (d.getId() == groupSlot) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                groupSlot = "";
            }
            attr->setAttributeValue(GrouperOutSlot::busMap2readable(groupSlot));
        }
    }
    // update out slots
    {
        GrouperOutSlotAttribute *attr = dynamic_cast<GrouperOutSlotAttribute*>(getParameter(CoreLibConstants::GROUPER_OUT_SLOTS_ATTR));
        QList<GrouperOutSlot> &outSlots = attr->getOutSlots();
        QList<GrouperOutSlot>::iterator i = outSlots.begin();
        while (i != outSlots.end()) {
            QString in = i->getBusMapInSlotId();
            U2OpStatus2Log logOs;
            IntegralBusSlot slot = IntegralBusSlot::fromString(in, logOs);
            foreach (const ActorId &oldId, actorsMapping.keys()) {
                slot.replaceActorId(oldId, actorsMapping[oldId]);
            }
            in = slot.toString();
            i->setBusMapInSlotStr(in);
            bool found = false;
            foreach (const Descriptor &d, inBusMap.keys()) {
                if (d.getId() == in) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                outBusMap.remove(i->getOutSlotId());
                i = outSlots.erase(i);
            } else {
                ++i;
            }
        }
    }

    DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outBusMap));
    outPort->setNewType(newType);
}

void Actor::replaceActor(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings) {
    foreach (Port *p, getPorts()) {
        p->replaceActor(oldActor, newActor, mappings);
    }
    if (CoreLibConstants::GROUPER_ID == proto->getId()) {
        {
            Attribute *attr = getParameter(CoreLibConstants::GROUPER_SLOT_ATTR);
            QString groupSlot = attr->getAttributeValueWithoutScript<QString>();
            groupSlot = GrouperOutSlot::readable2busMap(groupSlot);
            foreach (const PortMapping &pm, mappings) {
                IntegralBusUtils::remapPathedSlotString(groupSlot, oldActor->getId(), newActor->getId(), pm);
            }
            attr->setAttributeValue(GrouperOutSlot::busMap2readable(groupSlot));
        }

        {
            GrouperOutSlotAttribute *attr = dynamic_cast<GrouperOutSlotAttribute*>(getParameter(CoreLibConstants::GROUPER_OUT_SLOTS_ATTR));
            QList<GrouperOutSlot>::iterator i = attr->getOutSlots().begin();
            for (; i!=attr->getOutSlots().end(); i++) {
                QString in = i->getBusMapInSlotId();
                foreach (const PortMapping &pm, mappings) {
                    IntegralBusUtils::remapPathedSlotString(in, oldActor->getId(), newActor->getId(), pm);
                }
                i->setBusMapInSlotStr(in);
            }
        }
    }
}

void Actor::updateDelegateTags() {
    CHECK(NULL != editor, );
    foreach (Attribute *influencing, getAttributes()) {
        foreach (const AttributeRelation *rel, influencing->getRelations()) {
            PropertyDelegate *dependentDelegate = editor->getDelegate(rel->getRelatedAttrId());
            if (NULL == dependentDelegate) {
                continue;
            }
            rel->updateDelegateTags(influencing->getAttributePureValue(), dependentDelegate->tags());
        }
    }
}

void Actor::updateItemsAvailability() {
    foreach (Attribute *influencing, getAttributes()) {
        updateItemsAvailability(influencing);
    }
}

void Actor::updateItemsAvailability(const Attribute* influencingAttribute) {
    foreach(PortRelationDescriptor* rel, influencingAttribute->getPortRelations()) {
        Port* dependentPort = getPort(rel->getPortId());
        CHECK_CONTINUE(dependentPort != NULL);

        dependentPort->setEnabled(rel->isPortEnabled(influencingAttribute->getAttributePureValue()));
    }

    foreach(const SlotRelationDescriptor& rel, influencingAttribute->getSlotRelations()) {
        Port* dependentPort = getPort(rel.portId);
        CHECK_CONTINUE(dependentPort != NULL);

        const bool isEnabled = rel.isSlotEnabled(influencingAttribute->getAttributePureValue().toString());
        dependentPort->setVisibleSlot(rel.slotId, isEnabled);
    }
}

void Actor::addCustomValidator(const ValidatorDesc &desc) {
    customValidators << desc;
}

const QList<ValidatorDesc> & Actor::getCustomValidators() const {
    return customValidators;
}

/**
 * Validates an attribute with URL(s), considering its type.
 * Attributes with scripts are ignored (the method returns "true").
 * Attributes with value "Default" (case-insensitive) are ignored.
 */
static bool validateUrlAttribute(Attribute *attr, UrlAttributeType urlType, ProblemList &infoList) {
    SAFE_POINT(NULL != attr, "NULL attribute!", false);
    SAFE_POINT(NotAnUrl != urlType, "Can't pass not an URL to the method!", false);

    if (!attr->getAttributeScript().isEmpty()) {
        return true;
    }

    QString urls = attr->getAttributePureValue().toString();
    if (urls.toLower() == "default") {
        return true;
    }

    bool res;
    switch (urlType) {
        case DatasetAttr:
            res = WorkflowUtils::validateDatasets(attr->getAttributePureValue().value< QList<Dataset> >(), infoList);
            break;
        case InputFile:
            res = WorkflowUtils::validateInputFiles(urls, infoList);
            break;
        case InputDir:
            res = WorkflowUtils::validateInputDirs(urls, infoList);
            break;
        case OutputFile:
            res = WorkflowUtils::validateOutputFile(urls, infoList);
            break;
        case OutputDir:
            res = WorkflowUtils::validateOutputDir(urls, infoList);
            break;
        default:
            FAIL("Unexpected value of the URL attribute!", false);
    }
    return res;
}

static bool validateCodePage(const QString &url) {
    QString url2 = QString::fromLocal8Bit(url.toLocal8Bit());
    if (url.compare(url2, Qt::CaseSensitive) != 0) {
        return false;
    }
    return true;
}

/**
 * UGENE-5595
 * The following validation is very simple:
 *   - convert attr's value from QString to 8Bite byte array
 *   - then, convert back 8Bit to QString
 *   - both QString must be equal
 */
static bool validateCodePage(Attribute *attr, ProblemList &infoList) {
    SAFE_POINT(NULL != attr, "NULL attribute!", false);

    QStringList urlsList = WorkflowUtils::getAttributeUrls(attr);
    if (urlsList.isEmpty()) {
        return true;
    }

    // Verify each URL
    bool res = true;
    foreach(const QString &url, urlsList) {
        if (!validateCodePage(url)) {
            res = false;
            infoList << Problem(L10N::warningCharactersCodePage(attr->getDisplayName()), "", Problem::U2_WARNING);
        }
    }
    return res;
}

bool Actor::validate(ProblemList &problemList) const {
    bool result = Configuration::validate(problemList);
    foreach (const ValidatorDesc &desc, customValidators) {
        ActorValidator *v = WorkflowEnv::getActorValidatorRegistry()->findValidator(desc.type);
        if (NULL != v) {
            result &= v->validate(this, problemList, desc.options);
        }
    }

    // Validate URL and Numeric parameters
    bool urlsRes = true;
    foreach (Attribute *attr, getParameters()) {
        SAFE_POINT(NULL != attr, "NULL attribute!", false);
        if (!isAttributeVisible(attr)) {
            continue;
        }
        UrlAttributeType urlType = WorkflowUtils::isUrlAttribute(attr, this);

        if (urlType != NotAnUrl) {
            bool urlAttrValid = validateUrlAttribute(attr, urlType, problemList);
            urlsRes = urlsRes && urlAttrValid;
        }

        if (urlType != NotAnUrl || attr->getFlags().testFlag(Attribute::NeedValidateEncoding)) {
            // UGENE-5595
            bool urlAttrValid = validateCodePage(attr, problemList);
            Q_UNUSED(urlAttrValid)
            // we think that this is a warning, so I commented the following line
            //urlsRes = urlsRes && urlAttrValid;
        }

        if (attr->getAttributeType() == BaseTypes::NUM_TYPE() && !attr->getAttributePureValue().toString().isEmpty()) {
            bool ok;
            attr->getAttributePureValue().toString().toDouble((&ok));
            result &= ok;
            if (!ok) {
                problemList << Problem(L10N::badArgument(attr->getAttributePureValue().toString()));
            }
        }

        if (WorkflowUtils::isSharedDbUrlAttribute(attr, this)) {
            result &= WorkflowUtils::validateSharedDbUrl(attr->getAttributePureValue().toString(), problemList);
        }
    }
    result = result && urlsRes;

    return result;
}

} // Workflow
} // U2
