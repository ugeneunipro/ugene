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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/IntegralBus.h>

#include "IntegralBusModel.h"
#include "IntegralBusType.h"
#include "support/IntegralBusUtils.h"

/* TRANSLATOR U2::Workflow::IntegralBusPort */
namespace U2 {
namespace Workflow {

/*******************************
 * IntegralBusPort
 *******************************/
static Actor* getLinkedActor(ActorId id, Port* output, QList<Actor*> visitedActors) {
    if (visitedActors.contains(output->owner())) {
        return NULL;
    }
    visitedActors << output->owner();
    if (output->owner()->getId() == id) {
        return output->owner();
    }
    foreach(Port* transit, output->owner()->getInputPorts()) {
        foreach(Port* p, transit->getLinks().uniqueKeys()) {
            Actor* a = getLinkedActor(id, p, visitedActors);
            if (a) return a;
        }
    }
    return NULL;
}

static QMap<QString, QStringList> getListSlotsMappings(const StrStrMap& bm, const Port* p) {
    assert(p->isInput());
    DataTypePtr dt = p->getType();
    QMap<QString, QStringList> res;
    if (dt->isList()) {
        QString val = bm.value(p->getId());
        if (!val.isEmpty()) {
            res.insert(p->getId(), val.split(";"));
        }
    } else if (dt->isMap()) {
        foreach(Descriptor d, dt->getAllDescriptors()) {
            QString val = bm.value(d.getId());
            if (dt->getDatatypeByDescriptor(d)->isList() && !val.isEmpty()) {
                res.insert(d.getId(), val.split(";"));
            }
        }
    }
    return res;
}

const QString IntegralBusPort::BUS_MAP_ATTR_ID = "bus-map";
const QString IntegralBusPort::PATHS_ATTR_ID = "paths-through";

IntegralBusPort::IntegralBusPort(const PortDescriptor& d, Actor* p) : Port(d,p), recursing(false) {
    addParameter(BUS_MAP_ATTR_ID, new Attribute(Descriptor(BUS_MAP_ATTR_ID), DataTypePtr()));
    addParameter(PATHS_ATTR_ID, new Attribute(Descriptor(PATHS_ATTR_ID), DataTypePtr()));
}

DataTypePtr IntegralBusPort::getType() const {
    return isInput() ? type : getBusType();
}

DataTypePtr IntegralBusPort::getBusType() const {
    if (recursing) {
        return DataTypePtr(new IntegralBusType(Descriptor(), QMap<Descriptor, DataTypePtr>()));
    }
    recursing = true;
    IntegralBusType* t = new IntegralBusType(Descriptor(*this), QMap<Descriptor, DataTypePtr>());
    bool addPath = owner()->getProto()->getInfluenceOnPathFlag();
    foreach (Port* p, owner()->getInputPorts()) {
        if ((p->getFlags()&BLIND_INPUT) == 0){
            t->addInputs(p, addPath);
        }
    }
    t->addOutput(type, this);
    recursing = false;
    return DataTypePtr(t);
}

Actor* IntegralBusPort::getProducer(const QString& slot) {
    QList<Actor*> l = getProducers(slot);
    if (l.size() == 1) {
        return l.first();
    } else {
        return NULL;
    }
}

QList<Actor*> IntegralBusPort::getProducers(const QString& slot) {
    QList<Actor*> res;
    Attribute* at = getParameter(BUS_MAP_ATTR_ID);
    if(at == NULL) {
        return res;
    }
    StrStrMap busMap = at->getAttributeValueWithoutScript<StrStrMap>();
    QString slotValue = busMap.value(slot);
    QStringList vals = slotValue.split(";");
    foreach(QString val, vals) {
        ActorId id = IntegralBusType::parseSlotDesc(val);
        Actor * a = getLinkedActorById(id);
        if(a != NULL) {
            res << a;
        }
    }
    return res;
}

Actor* IntegralBusPort::getLinkedActorById(ActorId id) const {
    QList<Actor*> res;
    foreach(Port* peer, getLinks().uniqueKeys()) {
        Actor* ac = getLinkedActor(id, peer, QList<Actor*>());
        if(ac != NULL) {
            res << ac;
        }
    }

    Actor * ret = NULL;
    if( res.size() == 0 ) {
        ret = NULL;
    } else if( res.size() > 1 ) {
        ret = res.first();
        //assert(false);
    } else {
        ret = res.first();
    }
    return ret;
}

SlotPathMap IntegralBusPort::getPaths() const {
    Attribute *a = this->getParameter(PATHS_ATTR_ID);
    SlotPathMap map = a->getAttributeValueWithoutScript<SlotPathMap>();

    return map;
}

QList<QStringList> IntegralBusPort::getPathsBySlotsPair(const QString& dest, const QString& src) const {
    SlotPathMap map = getPaths();
    QList<QStringList> list = map.values(QPair<QString, QString>(dest, src));

    return list;
}

void IntegralBusPort::setPathsBySlotsPair(const QString& dest, const QString& src, const QList<QStringList> &paths) {
    SlotPathMap map = getPaths();
    QPair<QString, QString> key(dest, src);
    map.remove(key);

    foreach (const QStringList &path, paths) {
        map.insertMulti(key, path);
    }

    this->setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(map));
}

void IntegralBusPort::addPathBySlotsPair(const QString& dest, const QString& src, const QStringList &path) {
    SlotPathMap map = getPaths();
    QPair<QString, QString> key(dest, src);
    map.insertMulti(key, path);

    this->setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(map));
}

void IntegralBusPort::clearPaths() {
    SlotPathMap map;
    this->setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(map));
}

void IntegralBusPort::remap(const QMap<ActorId, ActorId>& m) {
    Attribute* busAttr = getParameter(BUS_MAP_ATTR_ID);
    if (busAttr) {
        StrStrMap busMap = busAttr->getAttributeValueWithoutScript<StrStrMap>();
        IntegralBusType::remap(busMap, m);
        setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(busMap));
    }

    Attribute* pathsAttr = getParameter(PATHS_ATTR_ID);
    if (pathsAttr) {
        SlotPathMap pathsMap = pathsAttr->getAttributeValueWithoutScript<SlotPathMap>();
        IntegralBusType::remapPaths(pathsMap, m);
        setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(pathsMap));
    }
}

void IntegralBusPort::updateBindings(const QMap<ActorId, ActorId> &actorsMapping) {
    Port::updateBindings(actorsMapping);
    if (isOutput()) {
        return;
    }
    U2OpStatusImpl os;
    QMap<Descriptor, DataTypePtr> incomingType = WorkflowUtils::getBusType(this);
    StrStrMap busMap = getBusMap();
    SlotPathMap pathMap = getPathsMap();
    IntegralBusType::remap(busMap, actorsMapping);
    IntegralBusType::remapPaths(pathMap, actorsMapping);

    foreach (const QString &dstSlot, busMap.keys()) {
        QList<IntegralBusSlot> srcs = IntegralBusSlot::listFromString(busMap.value(dstSlot), os);
        QList<IntegralBusSlot> validSrcs;

        foreach (const IntegralBusSlot srcSlot, srcs) {
            SlotPair slotPair(dstSlot, srcSlot.toString());
            bool hasOneValidPath = false;

            if (pathMap.contains(slotPair)) {
                QList<QStringList> validPaths;
                foreach (const QStringList &path, pathMap.values(slotPair)) {
                    QString slotStr = srcSlot.toString() + ">" + path.join(",");
                    bool valid = WorkflowUtils::isBindingValid(slotStr, incomingType, dstSlot, getOwnTypeMap());
                    if (valid) {
                        validPaths << path;
                        hasOneValidPath = true;
                    }
                }
                pathMap.remove(slotPair);
                foreach (const QStringList &p, validPaths) {
                    pathMap.insertMulti(slotPair, p);
                }
            } else {
                hasOneValidPath = WorkflowUtils::isBindingValid(srcSlot.toString(), incomingType, dstSlot, getOwnTypeMap());
            }
            if (hasOneValidPath) {
                validSrcs << srcSlot;
            }
        }

        busMap[dstSlot] = IntegralBusSlot::listToString(validSrcs);
    }

    setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(busMap));
    setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(pathMap));
}

void IntegralBusPort::replaceActor(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings) {
    Port::replaceActor(oldActor, newActor, mappings);
    if (isOutput()) {
        return;
    }

    StrStrMap busMap = getBusMap();
    foreach (Port *p, oldActor->getEnabledOutputPorts()) {
        U2OpStatus2Log os;
        PortMapping pm = PortMapping::getMappingBySrcPort(p->getId(), mappings, os);
        if (os.hasError()) {
            continue;
        }
        IntegralBusUtils::remapBus(busMap, oldActor->getId(), newActor->getId(), pm);
    }
    setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(busMap));

    SlotPathMap pathMap = getPathsMap();
    QMap<ActorId, ActorId> actorsMapping;
    actorsMapping[oldActor->getId()] = newActor->getId();
    IntegralBusType::remapPaths(pathMap, actorsMapping);
    setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(pathMap));
}

void IntegralBusPort::setVisibleSlot(const QString& slotId, const bool isVisible) {
    PortDescriptor::setVisibleSlot(slotId, isVisible);

    if (isVisible) {
        restoreBusMapKey(slotId);
    } else {
        removeBusMapKey(slotId);
    }
}

void IntegralBusPort::copyInput(IntegralBusPort *port, const PortMapping &mapping) {
    CHECK(isInput(), );
    CHECK(port->isInput(), );
    StrStrMap myBusMap;
    StrStrMap busMap = port->getBusMap();
    foreach (const QString &slotId, busMap.keys()) {
        U2OpStatus2Log os;
        myBusMap[mapping.getDstSlotId(slotId, os)] = busMap[slotId];
    }
    setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(myBusMap));

    SlotPathMap myPathMap;
    SlotPathMap pathMap = port->getPathsMap();
    foreach (const SlotPair &pair, pathMap.keys()) {
        U2OpStatus2Log os;
        SlotPair myPair(mapping.getDstSlotId(pair.first, os), pair.second);
        myPathMap[myPair] = pathMap[pair];
    }
    setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(myPathMap));
}

StrStrMap IntegralBusPort::getBusMap() const {
    Attribute *busAttr = getParameter(BUS_MAP_ATTR_ID);
    CHECK(NULL != busAttr, StrStrMap());
    return busAttr->getAttributeValueWithoutScript<StrStrMap>();
}

SlotPathMap IntegralBusPort::getPathsMap() const {
    Attribute *pathsAttr = getParameter(PATHS_ATTR_ID);
    CHECK(NULL != pathsAttr, SlotPathMap());
    return pathsAttr->getAttributeValueWithoutScript<SlotPathMap>();
}

void IntegralBusPort::setBusMapValue(const QString & slotId, const QString & value) {
    if( !isInput() ) {
        return;
    }
    StrStrMap busMap = getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
    if(busMap[slotId].isEmpty()) {
        busMap[slotId] = value;
    } else {
        busMap[slotId] = busMap[slotId] + ";" + value;
    }
    setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(busMap));
}

void IntegralBusPort::removeBusMapKey(const QString& slotId) {
    CHECK(!removedBusMap.contains(slotId), );

    StrStrMap attributeBusMap = getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
    CHECK(attributeBusMap.contains(slotId), );

    removedBusMap.insert(slotId, attributeBusMap[slotId]);
    attributeBusMap.remove(slotId);
    setParameter(BUS_MAP_ATTR_ID, QVariant::fromValue<StrStrMap>(attributeBusMap));
}

void IntegralBusPort::restoreBusMapKey(const QString& slotId) {
    StrStrMap attributeBusMap = getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
    CHECK(!attributeBusMap.contains(slotId), );

    CHECK(removedBusMap.contains(slotId), );

    attributeBusMap.insert(slotId, removedBusMap[slotId]);
    removedBusMap.remove(slotId);
    setParameter(BUS_MAP_ATTR_ID, QVariant::fromValue<StrStrMap>(attributeBusMap));
}

void IntegralBusPort::setupBusMap() {
    if( !isInput() || getWidth() != 1 ) {
        return;
    }

    DataTypePtr to = getType();
    assert(to->isMap()); // all port types made as map datatypes

    DataTypePtr from = bindings.uniqueKeys().first()->getType();
    QList<Descriptor> keys = to->getAllDescriptors();
    StrStrMap busMap = getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
    foreach(const Descriptor & key, keys) {
        // FIXME: hack for not binding 'Location' slot
        // 'Location' slot should NOT be binded for any writers to avoid writing to source of data
        // If there is only one slot - there are no objections for binding URL
        if(key == BaseSlots::URL_SLOT() && keys.size() > 1) {
            busMap.insert(key.getId(), "");
            continue;
        }

        U2OpStatus2Log os;
        DataTypePtr elementDatatype = to->getDatatypeByDescriptor(key);
        QList<Descriptor> allCandidates = WorkflowUtils::findMatchingTypes(from, elementDatatype);
        QList<Descriptor> candidates = IntegralBusUtils::splitCandidates(allCandidates, key, elementDatatype).mainDescs;
        if (elementDatatype->isList()) {
            candidates += WorkflowUtils::findMatchingTypes(from, elementDatatype->getDatatypeByDescriptor());
            candidates = IntegralBusUtils::splitCandidates(candidates, key, elementDatatype->getDatatypeByDescriptor()).mainDescs;
            QString res = WorkflowUtils::candidatesAsStringList(candidates).join(";");
            busMap.insert(key.getId(), res);
        } else if (candidates.size() == 0) {
            //no unambiguous match, reset
            busMap.insert(key.getId(), "");
        } else {
            bool fl = false;
            QMap<Port*,Link*> links = this->getLinks();
            Port *port = links.keys().first();
            DataTypePtr ptr = port->getOutputType();
            if(ptr->isMap()) {
                foreach(const Descriptor & desc, ptr->getAllDescriptors()) {
                    if(key.getId() == desc.getId()) {
                        foreach(const Descriptor &d, candidates) {
                            IntegralBusSlot slot = IntegralBusSlot::fromString(d.getId(), os);
                            if(slot.actorId() == port->owner()->getId()) {
                                busMap.insert(key.getId(), d.getId());
                                fl = true;
                            }
                        }
                    }
                }
            }
            else {
                if(key.getId() == ptr->getId()) {
                    foreach(const Descriptor &d, candidates) {
                        IntegralBusSlot slot = IntegralBusSlot::fromString(d.getId(), os);
                        if(slot.actorId() == port->owner()->getId()) {
                            busMap.insert(key.getId(), d.getId());
                            fl = true;
                        }
                    }
                }
            }
            if(!fl) {
                busMap.insert(key.getId(), candidates.first().getId());
            }
        }
    }

    SlotPathMap pathMap;
    WorkflowUtils::extractPathsFromBindings(busMap, pathMap);
    setParameter(BUS_MAP_ATTR_ID, qVariantFromValue<StrStrMap>(busMap));
    setParameter(PATHS_ATTR_ID, qVariantFromValue<SlotPathMap>(pathMap));
}

bool IntegralBusPort::validate(NotificationsList &notificationList) const {
    bool good = Configuration::validate(notificationList);
    if (isInput() && !validator) {
        good &= ScreenedSlotValidator::validate(QStringList(), this, notificationList);
    }
    return good;
}

/*******************************
* ScreenedSlotValidator
*******************************/
bool ScreenedSlotValidator::validate( const QStringList& screenedSlots, const IntegralBusPort* vport, NotificationsList& notificationList)
{
    bool good = true;
    {
        if (vport->getWidth() == 0) {
            notificationList.append(WorkflowNotification(IntegralBusPort::tr("No input data supplied")));
            return false;
        }
        StrStrMap bm = vport->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
        SlotPathMap pm = vport->getParameter(IntegralBusPort::PATHS_ATTR_ID)->getAttributeValueWithoutScript<SlotPathMap>();
        WorkflowUtils::applyPathsToBusMap(bm, pm);
        int busWidth = bm.size();
        QMap<QString, QStringList> listMap = getListSlotsMappings(bm, vport);
        // iterate over all producers and exclude valid mappings from bus bindings
        foreach(Port* p, vport->getLinks().uniqueKeys()) {
            assert(qobject_cast<IntegralBusPort*>(p));//TBD?
            DataTypePtr t = p->getType();
            assert(t->isMap());
            {
                foreach(Descriptor d, t->getAllDescriptors()) {
                    foreach(QString key, bm.keys(d.getId())) {
                        //log.debug("reducing bus from key="+ikey+" to="+rkey);
                        assert(!key.isEmpty());
                        bm.remove(key);
                    }
                    foreach(QString key, listMap.uniqueKeys()) {
                        QStringList& l = listMap[key];
                        l.removeAll(d.getId());
                        if (l.isEmpty()) {
                            listMap.remove(key);
                            bm.remove(key);
                        }
                    }
                }
            }
        }
        if (busWidth == bm.size()) {
            ActorPrototype *proto = vport->owner()->getProto();
            if (!(0 == busWidth && proto->isAllowsEmptyPorts())) {
                notificationList.append(WorkflowNotification(IntegralBusPort::tr("No input data supplied")));
                good = false;
            }
        }
        {
            QMapIterator<QString,QString> it(bm);
            while (it.hasNext())
            {
                it.next();
                const QString& slot = it.key();
                QString slotName = vport->getType()->getDatatypeDescriptor(slot).getDisplayName();
                //assert(!slotName.isEmpty());
                if (it.value().isEmpty()) {
                    if (!screenedSlots.contains(slot)) {
                        notificationList.append(WorkflowNotification(IntegralBusPort::tr("Empty input slot: %1").arg(slotName), "", WorkflowNotification::U2_WARNING));
                    }
                } else {
                    notificationList.append(WorkflowNotification(IntegralBusPort::tr("Bad slot binding: %1 to %2").arg(slotName).arg(it.value())));
                    good = false;
                }
            }
        }
        {
            QMapIterator<QString,QStringList> it(listMap);
            while (it.hasNext())
            {
                it.next();
                const QString& slot = it.key();
                QString slotName = vport->getType()->getDatatypeDescriptor(slot).getDisplayName();
                assert(!slotName.isEmpty());
                assert(!it.value().isEmpty());
                notificationList.append(WorkflowNotification(IntegralBusPort::tr("Bad slot binding: %1 to %2").arg(slotName).arg(it.value().join(","))));
                good = false;
            }
        }
    }
    return good;
}

bool ScreenedSlotValidator::validate(const Configuration* cfg, NotificationsList &notificationList ) const {
    return validate(screenedSlots, static_cast<const IntegralBusPort*>(cfg), notificationList);
}


/*******************************
* ScreenedParamValidator
*******************************/
ScreenedParamValidator::ScreenedParamValidator(const QString& id, const QString& port, const QString& slot)
: id(id), port(port), slot(slot) {}

bool ScreenedParamValidator::validate(const Configuration* cfg, NotificationsList &notificationList) const {
    QString err = validate(cfg);
    if( !err.isEmpty() ) {
        notificationList.append(WorkflowNotification(err));
        return false;
    }
    return true;
}

QString ScreenedParamValidator::validate(const Configuration * cfg) const {
    Attribute* param = cfg->getParameter(id);
    const bool paramIsVisible = cfg->isAttributeVisible(param);
    CHECK(paramIsVisible, QString());

    QVariant val = param->getAttributePureValue();
    const Workflow::Actor* a = dynamic_cast<const Workflow::Actor*>(cfg);
    assert(a);

    Workflow::Port* p = a->getPort(port);
    assert(p->isInput());

    QVariant busMap = p->getParameter(Workflow::IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributePureValue();
    QString slotVal = busMap.value<StrStrMap>().value(slot);
    const bool noParam = ( val.isNull() || val.toString().isEmpty() ) && param->getAttributeScript().isEmpty();
    const bool noSlot = slotVal.isNull() || slotVal.isEmpty();

    if (noParam && noSlot) {
        QString slotName = p->getType()->getDatatypeDescriptor(slot).getDisplayName();
        assert(!slotName.isEmpty());
        return U2::WorkflowUtils::tr("Either parameter '%1' or input slot '%2' must be set").arg(param->getDisplayName()).arg(slotName);//FIXME translator class
    }
    if (noParam == noSlot) {
        QString slotName = p->getType()->getDatatypeDescriptor(slot).getDisplayName();
        assert(!slotName.isEmpty());
        /*return U2::WorkflowUtils::tr("Warning, parameter '%1' overrides bus data slot '%2'")
            .arg(param->getDisplayName()).arg(slotName);//FIXME translator class*/
    }
    return QString();
}

/************************************************************************/
/* IntegralBusSlot */
/************************************************************************/
const QString IntegralBusSlot::SLOTS_SEP(";");
const QString IntegralBusSlot::INNER_SEP(":");

IntegralBusSlot::IntegralBusSlot() {

}

IntegralBusSlot::IntegralBusSlot(const QString &slotId, const QString &portId, const ActorId &actorId)
: id(slotId), port(portId), actor(actorId)
{

}

QString IntegralBusSlot::getId() const {
    return id;
}

QString IntegralBusSlot::portId() const {
    return port;
}

ActorId IntegralBusSlot::actorId() const {
    return actor;
}

void IntegralBusSlot::replaceActorId(const ActorId &oldId, const ActorId &newId) {
    if (oldId == actor) {
        actor = newId;
    }
}

QString IntegralBusSlot::toString() const {
    return actor + INNER_SEP + id;
}

QString IntegralBusSlot::listToString(const QList<IntegralBusSlot> &slotList) {
    QStringList result;
    foreach (const IntegralBusSlot &slot, slotList) {
        result << slot.toString();
    }
    return result.join(SLOTS_SEP);
}

IntegralBusSlot IntegralBusSlot::fromString(const QString &slotString, U2OpStatus &os) {
    if (slotString.isEmpty()) {
        return IntegralBusSlot();
    }

    QStringList tokens = slotString.split(INNER_SEP);
    if (2 != tokens.size()) {
        os.setError(QString("Can not parse slot from: %1").arg(slotString));
        return IntegralBusSlot();
    }
    return IntegralBusSlot(tokens[1], "", str2aid(tokens[0]));
}

QList<IntegralBusSlot> IntegralBusSlot::listFromString(const QString &slotsString, U2OpStatus &os) {
    QStringList strList = slotsString.split(SLOTS_SEP, QString::SkipEmptyParts);
    QList<IntegralBusSlot> result;
    foreach (const QString &slotStr, strList) {
        IntegralBusSlot slot = fromString(slotStr, os);
        CHECK_OP(os, result);
        result << slot;
    }
    return result;
}

bool IntegralBusSlot::operator==( const IntegralBusSlot& ibs) const{
    if (getId() == ibs.getId()
        && portId() == ibs.portId()
        && actorId() == ibs.actorId()){
            return true;
    }
    return false;
}

/************************************************************************/
/* PortValidator */
/************************************************************************/
bool PortValidator::validate(const Configuration *cfg, NotificationsList &notificationList) const {
    const IntegralBusPort *port = static_cast<const IntegralBusPort*>(cfg);
    SAFE_POINT(NULL != port, "NULL port", false);
    return validate(port, notificationList);
}

StrStrMap PortValidator::getBusMap(const IntegralBusPort *port) {
    return port->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
}

QString PortValidator::slotName(const IntegralBusPort *port, const QString &slotId) {
    return port->getType()->getDatatypeDescriptor(slotId).getDisplayName();
}

bool PortValidator::isBinded(const IntegralBusPort *port, const QString &slotId) {
    return isBinded(getBusMap(port), slotId);
}

bool PortValidator::isBinded(const StrStrMap &busMap, const QString &slotId) {
    return (!busMap.value(slotId, "").isEmpty());
}

}//Workflow namespace
}//GB2namespace
