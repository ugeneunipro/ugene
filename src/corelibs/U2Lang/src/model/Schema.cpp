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

#include <U2Lang/ActorModel.h>
#include <U2Lang/IntegralBusType.h>
#include <U2Lang/Wizard.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>

#include "Schema.h"

namespace U2 {
namespace Workflow {

/**************************
 * Schema
 **************************/
Schema::Schema() : deepCopy(false) {
}

Schema::~Schema() {
    reset();
}

Schema::Schema( const Schema & other ) {
    *this = other;
}

Schema & Schema::operator =( const Schema & other ) {
    procs = other.procs;
    domain = other.domain;
    graph = ActorBindingsGraph(other.graph);
    deepCopy = false;
    portAliases = other.portAliases;
    includedTypeName = other.includedTypeName;
    return *this;
}

void Schema::reset() {
    if (deepCopy) {
        qDeleteAll(procs);
        procs.clear();
    }
    graph.clear();
    qDeleteAll(wizards);
    wizards.clear();
}

void Schema::applyConfiguration(const QMap<ActorId, QVariantMap> &cfg) {
    foreach(Actor* a, procs) {
        if (cfg.contains(a->getId())) {
            a->setParameters(cfg[a->getId()]);
        }
    }
}


Actor * Schema::actorById( ActorId id) const {
    return WorkflowUtils::actorById(procs, id);
}

QList<Actor*> Schema::actorsByOwnerId(ActorId id) const {
    QList<Actor*> res;
    foreach (Actor *proc, procs) {
        if (proc->getOwner() == id) {
            res.append(proc);
        }
    }
    return res;
}

QString Schema::getDomain() const {
    return domain;
}

void Schema::setDomain(const QString & d) {
    domain = d;
}

const ActorBindingsGraph & Schema::getActorBindingsGraph() const {
    return graph;
}

const QList<Actor*> & Schema::getProcesses() const {
    return procs;
}

void Schema::addProcess(Actor * a) {
    assert(a != NULL);
    procs.append(a);
}

QList<Link*> Schema::getFlows() const {
    return graph.getFlows();
}

void Schema::addFlow(Link* l) {
    assert(l != NULL);
    graph.addBinding(l->source(), l->destination());
}

void Schema::setDeepCopyFlag(bool flag) {
    deepCopy = flag;
}

bool Schema::hasParamAliases() const {
    foreach(Actor * actor, procs) {
        if(actor->hasParamAliases()) {
            return true;
        }
    }
    return false;
}

bool Schema::hasAliasHelp() const {
    foreach(Actor * a, procs) {
        if(a->hasAliasHelp()) {
            return true;
        }
    }
    return false;
}

bool Schema::hasPortAliases() const {
    return !portAliases.isEmpty();
}

const QList<PortAlias> &Schema::getPortAliases() const {
    return portAliases;
}

bool Schema::addPortAlias(const PortAlias &newAlias) {
    foreach (const PortAlias &alias, portAliases) {
        if (alias.getAlias() == newAlias.getAlias()) {
            return false;
        }
        if (alias.getSourcePort() == newAlias.getSourcePort()) {
            return false;
        }
    }

    portAliases.append(newAlias);
    return true;
}

void Schema::setPortAliases(const QList<PortAlias> &aliases) {
    portAliases = aliases;
}

QString Schema::getTypeName() const {
    return includedTypeName;
}

void Schema::setTypeName(const QString &typeName) {
    this->includedTypeName = typeName;
}

using namespace std;

bool Schema::recursiveExpand(QList<QString> &schemaIds) {
    QMap<Actor*, Schema*> subSchemas;

    // Expand all processes
    foreach (Actor *proc, procs) {
        ActorPrototype *proto = proc->getProto();
        if (!proto->isSchemaFlagSet()) {
            continue;
        }

        if (schemaIds.contains(proto->getId())) {
            return false;
        }

        Schema *schema = WorkflowEnv::getSchemaActorsRegistry()->getSchema(proto->getId());
        if (NULL == schema) {
            return false;
        }

        QList<QString> newIdList(schemaIds);
        newIdList.append(proto->getId());
        bool res = schema->recursiveExpand(newIdList);
        if (!res) {
            return false;
        }

        subSchemas.insert(proc, schema);
    }

    // Everything is all right after expanding. So replace expanded processes
    foreach (Actor *proc, procs) {
        if (!proc->getProto()->isSchemaFlagSet()) {
            continue;
        }
        Schema *schema = subSchemas.value(proc);

        // set owner
        foreach (Actor *subProc, schema->getProcesses()) {
            subProc->setOwner(proc->getId());
        }

        // replace parameters
        foreach (Actor *subProc, schema->getProcesses()) {
            if (subProc->hasParamAliases()) {
                setAliasedAttributes(proc, subProc);
            }
        }

        // replace ports and slots
        foreach (const PortAlias &subPortAlias, schema->getPortAliases()) {
            if (subPortAlias.isInput()) {
                replaceInLinksAndSlots(proc, subPortAlias);
            } else {
                replaceOutLinks(proc, subPortAlias);
                replaceOutSlots(proc, subPortAlias);
            }

            if (this->hasPortAliases()) {
                replacePortAliases(subPortAlias);
            }
        }

        graph.getBindings().unite(schema->graph.getBindings());

        // replace procs
        procs.removeOne(proc);
        procs.append(schema->getProcesses());
    }

    return true;
}

bool Schema::expand() {
    QList<QString> emptyList;
    return this->recursiveExpand(emptyList);
}

void Schema::setAliasedAttributes(Actor *proc, Actor *subProc) {
    QMap<QString, QString> newParamAliases;

    foreach (QString subAttrId, subProc->getParamAliases().keys()) {
        QString alias = subProc->getParamAliases().value(subAttrId);

        QVariant value = proc->getParameter(alias)->getAttributePureValue();
        subProc->getParameter(subAttrId)->setAttributeValue(value);
        AttributeScript script = proc->getParameter(alias)->getAttributeScript();
        subProc->getParameter(subAttrId)->getAttributeScript() = script;

        if (proc->getParamAliases().keys().contains(alias)) {
            newParamAliases.insert(subAttrId, proc->getParamAliases().value(alias));
        }
    }
    subProc->getParamAliases() = newParamAliases;
}

typedef QPair<QString, QString> SlotPair;

void Schema::replaceInLinksAndSlots(Actor *proc, const PortAlias &portAlias) {
    Port *port = proc->getPort(portAlias.getAlias());
    Actor *subProc = portAlias.getSourcePort()->owner();
    Port *subPort = subProc->getPort(portAlias.getSourcePort()->getId());

    foreach (Link *link, this->getFlows()) {
        if (link->destination() == port) {
            // replace ports link
            removeFlow(link);
            link->connect(link->source(), subPort);
            addFlow(link);

            // replace slots links and paths
            Attribute *b = port->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID);
            Attribute *p = port->getParameter(IntegralBusPort::PATHS_ATTR_ID);
            StrStrMap busMap = b->getAttributeValueWithoutScript<StrStrMap>();
            SlotPathMap pathMap = p->getAttributeValueWithoutScript<SlotPathMap>();
            StrStrMap subBusMap;
            SlotPathMap subPathMap;

            foreach (const SlotAlias &slotAlias, portAlias.getSlotAliases()) {
                subBusMap[slotAlias.getSourceSlotId()] = busMap[slotAlias.getAlias()];

                foreach (const SlotPair &slotPair, pathMap.keys()) {
                    if (slotAlias.getAlias() == slotPair.first) {
                        SlotPair subPair(slotAlias.getSourceSlotId(), slotPair.second);
                        foreach (const QStringList &p, pathMap.values(slotPair)) {
                            subPathMap.insertMulti(subPair, p);
                        }
                    }
                }
            }
            subPort->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->setAttributeValue(qVariantFromValue(subBusMap));
            subPort->getParameter(IntegralBusPort::PATHS_ATTR_ID)->setAttributeValue(qVariantFromValue(subPathMap));
        }
    }
}

void Schema::replaceOutLinks(Actor *origProc, const PortAlias &portAlias) {
    Port *port = origProc->getPort(portAlias.getAlias());
    Actor *subProc = portAlias.getSourcePort()->owner();
    Port *subPort = subProc->getPort(portAlias.getSourcePort()->getId());

    foreach (Link *link, this->getFlows()) {
        if (link->source() == port) {
            // replace only ports link
            removeFlow(link);
            link->connect(subPort, link->destination());
            addFlow(link);
        }
    }
}

void Schema::replaceOutSlots(Actor *origProc, const PortAlias &portAlias) {
    // replace slots links
    foreach (Actor *proc, procs) {
        foreach (Port *p, proc->getInputPorts()) {
            Attribute *a = p->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID);
            StrStrMap busMap = a->getAttributeValueWithoutScript<StrStrMap>();
            StrStrMap newMap;

            QMapIterator<QString, QString> it(busMap);
            while (it.hasNext()) {
                it.next();
                // replace ids at slots' values
                QString value = it.value();
                foreach (const SlotAlias &slotAlias, portAlias.getSlotAliases()) {
                    QString origSlotId = slotAlias.getAlias();
                    QString subSlotId = slotAlias.getSourceSlotId();

                    QString slotString = origProc->getId() + ":" + origSlotId;
                    int idPos = value.indexOf(slotString);
                    while (idPos >= 0) {
                        Actor *subProc = slotAlias.getSourcePort()->owner();
                        value.remove(idPos, slotString.length());
                        value.insert(idPos, subProc->getId()+":"+subSlotId);
                        idPos = value.indexOf(slotString);
                    }
                }
                newMap.insert(it.key(), value);
            }

            a->setAttributeValue(qVariantFromValue(newMap));
        }
    }
}

void Schema::replacePortAliases(const PortAlias &subPortAlias) {
    // replace port aliases
    QList<PortAlias> newPortAliases;
    foreach (PortAlias origPortAlias, this->portAliases) {
        if (origPortAlias.getSourcePort()->getId() == subPortAlias.getAlias()) {
            origPortAlias.setNewSourcePort(subPortAlias.getSourcePort());
        }

        // replace slot aliases
        QList<SlotAlias> newSlotAliases;
        foreach (const SlotAlias &origSlotAlias, origPortAlias.getSlotAliases()) {
            if (origSlotAlias.getSourcePort()->getId() == subPortAlias.getAlias()) {
                foreach (const SlotAlias &subSlotAlias, subPortAlias.getSlotAliases()) {
                    if (subSlotAlias.getAlias() == origSlotAlias.getSourceSlotId()) {
                        SlotAlias newSlotAlias(subSlotAlias.getSourcePort(), subSlotAlias.getSourceSlotId(), origSlotAlias.getAlias());
                        newSlotAliases.append(newSlotAlias);
                        break;
                    }
                }
            } else {
                newSlotAliases.append(origSlotAlias);
            }
        }
        origPortAlias.setNewSlotAliases(newSlotAliases);
        newPortAliases.append(origPortAlias);
    }
    this->portAliases = newPortAliases;
}

const QList<Wizard*> & Schema::getWizards() const {
    return wizards;
}

void Schema::setWizards(const QList<Wizard*> &value) {
    qDeleteAll(wizards);
    wizards = value;
}

QList<Wizard*> Schema::takeWizards() {
    QList<Wizard*> result = wizards;
    wizards.clear();
    return result;
}

void Schema::removeProcess(Actor *actor) {
    // remove actors flows
    foreach (Port *p, actor->getPorts()) {
        foreach (Link *l, p->getLinks()) {
            removeFlow(l);
        }
    }

    // remove actor from port aliases
    QList<Port*> ports = actor->getPorts();
    QList<PortAlias>::iterator i = portAliases.begin();
    while (i != portAliases.end()) {
        Port *p = const_cast<Port*>(i->getSourcePort());
        if (ports.contains(p)) {
            i = portAliases.erase(i);
        } else {
            ++i;
        }
    }

    procs.removeOne(actor);
    update();
}

void Schema::update() {
    update(QMap<ActorId, ActorId>());
}

void Schema::update(const QMap<ActorId, ActorId> &actorsMapping) {
    // update actors from the first level of the graph to the last one
    QMap<int, QList<Actor*> > top = graph.getTopologicalSortedGraph(procs);
    int beginLevel = top.size() - 1;
    for (int level=beginLevel; level>=0; level--) {
        foreach (Actor *a, top[level]) {
            a->update(actorsMapping);
        }
    }
}

void Schema::removeFlow(Link* l) {
    if (graph.contains(l->source(), l->destination())) {
        graph.removeBinding(l->source(), l->destination());
        l->disconnect();
        // TODO: delete l;
    }
}

ActorId Schema::uniqueActorId(const QString &id, const QList<Actor*> &procs) {
    QStringList uniqueIds;
    foreach (Actor *a, procs) {
        uniqueIds << aid2str(a->getId());
    }
    QString result = WorkflowUtils::createUniqueString(id, "-", uniqueIds);
    return str2aid(result);
}

void Schema::renameProcess(const ActorId &oldId, const ActorId &newId) {
    Actor *actor = actorById(oldId);
    CHECK(NULL != actor, );

    actor->setId(newId);
    QMap<ActorId, ActorId> m;
    m[oldId] = newId;
    foreach (Port *p, actor->getPorts()) {
        p->remap(m);
    }
    update(m);
}

namespace {
    QStringList removeAliasesDupliucates(const QList<Actor*> &actors, Actor *newActor) {
        QStringList removed;
        QStringList allAliases;
        foreach (Actor *actor, actors) {
            allAliases << actor->getParamAliases().values();
        }
        QMap<QString, QString> newAliases = newActor->getParamAliases();
        foreach (const QString &key, newAliases.keys()) {
            QString alias = newAliases.value(key);
            if (allAliases.contains(alias)) {
                newActor->getParamAliases().remove(key);
                newActor->getAliasHelp().remove(alias);
                removed << alias;
            }
        }
        return removed;
    }
}

void Schema::merge(Schema &other) {
    foreach (Actor *newActor, other.procs) {
        QStringList removed = removeAliasesDupliucates(procs, newActor);
        foreach (const QString &alias, removed) {
            coreLog.error(QObject::tr("Duplicate alias '%1'. It has been removed").arg(alias));
        }
        procs << newActor;
    }
    graph.getBindings().unite(other.graph.getBindings());
    portAliases << other.portAliases;
}

void Schema::replaceProcess(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings) {
    CHECK(procs.contains(oldActor), );
    CHECK(!procs.contains(newActor), );
    QMap<int, QList<Actor*> > top = graph.getTopologicalSortedGraph(procs);

    // replace actors flows
    foreach (Port *p, oldActor->getPorts()) {
        U2OpStatus2Log os;
        PortMapping pm = PortMapping::getMappingBySrcPort(p->getId(), mappings, os);
        if (os.hasError()) {
            continue;
        }
        foreach (Link *l, p->getLinks()) {
            Port *p1 = l->source() == p ? l->destination() : l->source();
            Port *p2 = newActor->getPort(pm.getDstId());
            removeFlow(l);
            Link *newLink = new Link(p1, p2);
            addFlow(newLink);
            if (p2->isInput()) {
                IntegralBusPort *oldPort = dynamic_cast<IntegralBusPort*>(p);
                IntegralBusPort *newPort = dynamic_cast<IntegralBusPort*>(p2);
                newPort->copyInput(oldPort, pm);
            }
        }
    }

    int beginLevel = top.size() - 1;
    for (int level=beginLevel; level>=0; level--) {
        foreach (Actor *a, top[level]) {
            if (a != oldActor) {
                a->replaceActor(oldActor, newActor, mappings);
            }
        }
    }

    procs.removeOne(oldActor);
    procs.append(newActor);
}

/************************************************************************/
/* ActorVisualData */
/************************************************************************/
ActorVisualData::ActorVisualData() {
    initialize();
}

ActorVisualData::ActorVisualData(const ActorId &_actorId)
: actorId(_actorId)
{
    initialize();
}

void ActorVisualData::initialize() {
    posInited = false;
    styleInited = false;
    colorInited = false;
    fontInited = false;
    rectInited = false;
}

ActorId ActorVisualData::getActorId() const {
    return actorId;
}

void ActorVisualData::setActorId(const ActorId &value) {
    actorId = value;
}

QPointF ActorVisualData::getPos(bool &contains) const {
    contains = posInited;
    return pos;
}

QString ActorVisualData::getStyle(bool &contains) const {
    contains = styleInited;
    return styleId;
}

QColor ActorVisualData::getColor(bool &contains) const {
    contains = colorInited;
    return color;
}

QFont ActorVisualData::getFont(bool &contains) const {
    contains = fontInited;
    return font;
}

QRectF ActorVisualData::getRect(bool &contains) const {
    contains = rectInited;
    return rect;
}

qreal ActorVisualData::getPortAngle(const QString &portId, bool &contains) const {
    contains = angleMap.contains(portId);
    return angleMap.value(portId, 0.0);
}

void ActorVisualData::setPos(const QPointF &value) {
    posInited = true;
    pos = value;
}

void ActorVisualData::setStyle(const QString &value) {
    styleInited = true;
    styleId = value;
}

void ActorVisualData::setColor(const QColor &value) {
    colorInited = true;
    color = value;
}

void ActorVisualData::setFont(const QFont &value) {
    fontInited = true;
    font = value;
}

void ActorVisualData::setRect(const QRectF &value) {
    rectInited = true;
    rect = value;
}

void ActorVisualData::setPortAngle(const QString &portId, qreal value) {
    angleMap[portId] = value;
}

QMap<QString, qreal> ActorVisualData::getAngleMap() const {
    return angleMap;
}

/**************************
 * Metadata
 **************************/
Metadata::Metadata() {
    reset();
}

void Metadata::reset() {
    name = QString();
    comment = QString();
    url = QString();
    scalePercent = 100;
    isSampleWorkflow = false;
    estimationsCode.clear();

    resetVisual();
}

void Metadata::resetVisual() {
    actorVisual.clear();
    textPosMap.clear();
}

ActorVisualData Metadata::getActorVisualData(const ActorId &actorId, bool &contains) const {
    contains = actorVisual.contains(actorId);
    return actorVisual.value(actorId, ActorVisualData());
}

void Metadata::setActorVisualData(const ActorVisualData &data) {
    actorVisual[data.getActorId()] = data;
}

QPointF Metadata::getTextPos(const ActorId &srcActorId, const QString &srcPortId,
    const ActorId &dstActorId, const QString &dstPortId, bool &contains) const {
    QString linkStr = getLinkString(srcActorId, srcPortId, dstActorId, dstPortId);
    contains = textPosMap.contains(linkStr);
    return textPosMap.value(linkStr, QPointF());
}

void Metadata::setTextPos(const ActorId &srcActorId, const QString &srcPortId,
    const ActorId &dstActorId, const QString &dstPortId,
    const QPointF &value) {
    QString linkStr = getLinkString(srcActorId, srcPortId, dstActorId, dstPortId);
    textPosMap[linkStr] = value;
}

void Metadata::removeActorMeta(const ActorId &actorId) {
    actorVisual.remove(actorId);

    foreach (const QString &linkStr, textPosMap.keys()) {
        if (isActorLinked(actorId, linkStr)) {
            textPosMap.remove(linkStr);
        }
    }
}

QString Metadata::getPortString(const ActorId &actorId, const QString &portId) const {
    return actorId + "." + portId;
}

ActorId Metadata::getActorId(const QString &portStr) const {
    QStringList tokens = portStr.split(".");
    CHECK(2 == tokens.size(), ActorId(""));
    return tokens[0];
}

QString Metadata::getLinkString(const ActorId &srcActorId, const QString &srcPortId,
    const ActorId &dstActorId, const QString &dstPortId) const {
    return getPortString(srcActorId, srcPortId) + "->" +
        getPortString(dstActorId, dstPortId);
}

bool Metadata::isActorLinked(const ActorId &actorId, const QString &linkStr) const {
    QStringList tokens = linkStr.split("->");
    CHECK(2 == tokens.size(), false);

    QStringList srcTokens = tokens[0].split(".");
    CHECK(2 == srcTokens.size(), false);
    QStringList dstTokens = tokens[1].split(".");
    CHECK(2 == dstTokens.size(), false);

    return (srcTokens[0] == actorId) || (dstTokens[0] == actorId);
}

QList<ActorVisualData> Metadata::getActorsVisual() const {
    return actorVisual.values();
}

QMap<QString, QPointF> Metadata::getTextPosMap() const {
    return textPosMap;
}

void Metadata::setSampleMark(bool isSample) {
    isSampleWorkflow = isSample;
}

bool Metadata::isSample() const {
    return isSampleWorkflow;
}

void Metadata::renameActors(const QMap<ActorId, ActorId> &actorsMapping) {
    foreach (const ActorId &oldId, actorsMapping.keys()) {
        if (actorVisual.contains(oldId)) {
            ActorId newId = actorsMapping[oldId];
            ActorVisualData visual = actorVisual.take(oldId);
            visual.setActorId(newId);
            actorVisual[newId] = visual;
        }
    }

    foreach (const QString &oldLinkStr, textPosMap.keys()) {
        QString newLinkStr = renameLink(oldLinkStr, actorsMapping);
        if (newLinkStr != oldLinkStr) {
            textPosMap[newLinkStr] = textPosMap[oldLinkStr];
            textPosMap.remove(oldLinkStr);
        }
    }
}

QString Metadata::renameLink(const QString &linkStr, const QMap<ActorId, ActorId> &actorsMapping) const {
    QStringList tokens = linkStr.split("->");
    CHECK(2 == tokens.size(), linkStr);

    QStringList srcTokens = tokens[0].split(".");
    CHECK(2 == srcTokens.size(), linkStr);
    QStringList dstTokens = tokens[1].split(".");
    CHECK(2 == dstTokens.size(), linkStr);

    foreach (const ActorId &oldId, actorsMapping.keys()) {
        if (srcTokens[0] == oldId) {
            srcTokens[0] = actorsMapping[oldId];
        }
        if (dstTokens[0] == oldId) {
            dstTokens[0] = actorsMapping[oldId];
        }
    }
    return getLinkString(srcTokens[0], srcTokens[1], dstTokens[0], dstTokens[1]);
}

QString Metadata::renameLink(const QString &linkStr, const ActorId &oldId, const ActorId &newId, const QList<PortMapping> &mappings) const {
    QStringList tokens = linkStr.split("->");
    CHECK(2 == tokens.size(), linkStr);

    QStringList srcTokens = tokens[0].split(".");
    CHECK(2 == srcTokens.size(), linkStr);
    QStringList dstTokens = tokens[1].split(".");
    CHECK(2 == dstTokens.size(), linkStr);

    if (srcTokens[0] == oldId) {
        U2OpStatus2Log os;
        PortMapping m = PortMapping::getMappingBySrcPort(srcTokens[1], mappings, os);
        srcTokens[0] = newId;
        srcTokens[1] = m.getDstId();
    }
    if (dstTokens[0] == oldId) {
        U2OpStatus2Log os;
        PortMapping m = PortMapping::getMappingBySrcPort(dstTokens[1], mappings, os);
        dstTokens[0] = newId;
        dstTokens[1] = m.getDstId();
    }
    return getLinkString(srcTokens[0], srcTokens[1], dstTokens[0], dstTokens[1]);
}

void Metadata::mergeVisual(const Metadata &other) {
    actorVisual.unite(other.actorVisual);
    textPosMap.unite(other.textPosMap);
}

void Metadata::replaceProcess(const ActorId &oldId, const ActorId &newId, const QList<PortMapping> &mappings) {
    bool contains = false;
    if (actorVisual.contains(oldId)) {
        ActorVisualData oldV = actorVisual[oldId];
        ActorVisualData newV(newId);
        QPointF p = oldV.getPos(contains);
        if (contains) {
            newV.setPos(p);
        }
        QString s = oldV.getStyle(contains);
        if (contains) {
            newV.setStyle(s);
        }
        QColor c = oldV.getColor(contains);
        if (contains) {
            newV.setColor(c);
        }
        QFont f = oldV.getFont(contains);
        if (contains) {
            newV.setFont(f);
        }
        QRectF r = oldV.getRect(contains);
        if (contains) {
            newV.setRect(r);
        }
        actorVisual.remove(oldId);
        actorVisual[newId] = newV;
    }
    foreach (const QString &linkStr, textPosMap.keys()) {
        QString newLinkStr = renameLink(linkStr, oldId, newId, mappings);
        if (newLinkStr != linkStr) {
            textPosMap[newLinkStr] = textPosMap[linkStr];
            textPosMap.remove(linkStr);
        }
    }
}

/**************************
 * ActorBindingGraph
 **************************/
bool ActorBindingsGraph::validateGraph(QString &) const {
    return true;
}

bool ActorBindingsGraph::addBinding(Port *source, Port *dest) {
    QList<Port*> ports;
    if (bindings.contains(source)) {
        ports = bindings.value(source);
        if (ports.contains(dest)) {
            return false;
        }
    }
    ports.append(dest);
    bindings.insert(source, ports);
    return true;
}

bool ActorBindingsGraph::contains(Port *source, Port *dest) const {
    if (bindings.contains(source)) {
        const QList<Port*> &ports = bindings[source];
        return ports.contains(dest);
    }
    return false;
}

void ActorBindingsGraph::removeBinding(Port *source, Port *dest) {
    if (bindings.contains(source)) {
        QList<Port*> &ports = bindings[source];
        ports.removeOne(dest);
        if (ports.isEmpty()) {
            bindings.remove(source);
        }
    }
}

const QMap<Port*, QList<Port*> > &ActorBindingsGraph::getBindings() const {
    return bindings;
}

QMap<Port*, QList<Port*> > &ActorBindingsGraph::getBindings() {
    return bindings;
}

QMap<int, QList<Actor*> > ActorBindingsGraph::getTopologicalSortedGraph(QList<Actor*> actors) const {
    QMap<Actor*, QList<Port*> > graph;
    foreach (Port *source, bindings.keys()) {
        if (graph.contains(source->owner())) {
            graph[source->owner()].append(bindings.value(source));
        } else {
            graph.insert(source->owner(), bindings.value(source));
        }
    }
    QMap<int, QList<Actor*> > result;

    int vertexLabel = 0;
    while (!graph.isEmpty()) {
        QList<Actor*> endVertexes;
        {
            foreach (Actor *a, actors) {
                if (!graph.keys().contains(a)) { // so, there is no arcs from this actor
                    endVertexes.append(a);
                }
            }
        }
        result.insert(vertexLabel, endVertexes);

        foreach (Actor *a, graph.keys()) {
            QList<Port*> ports = graph.value(a);
            foreach (Port *p, ports) {
                if (endVertexes.contains(p->owner())) {
                    ports.removeOne(p);
                }
            }
            if (ports.isEmpty()) {
                graph.remove(a);
            } else {
                graph.insert(a, ports);
            }
        }

        foreach (Actor *a, endVertexes) {
            actors.removeOne(a);
        }
        vertexLabel++;
    }
    result.insert(vertexLabel, actors);

    return result;
}

void ActorBindingsGraph::clear() {
    bindings.clear();
}

bool ActorBindingsGraph::isEmpty() const {
    return bindings.isEmpty();
}

QList<Link*> ActorBindingsGraph::getFlows() const {
    QList<Link*> result;
    foreach (Port *src, bindings.keys()) {
        foreach (Link *l, src->getLinks()) {
            SAFE_POINT(l->source() == src, "Link's source port mismatch", result);
            Port *dst = l->destination();
            SAFE_POINT(bindings[src].contains(dst), "Link's destination port mismatch", result);
            result << l;
        }
    }
    return result;
}

}//Workflow namespace

}//GB2namespace
