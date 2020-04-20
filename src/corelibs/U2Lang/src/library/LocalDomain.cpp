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

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/IntegralBusType.h>
#include <U2Lang/LastReadyScheduler.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowSettings.h>

#include "LocalDomain.h"

namespace U2 {
namespace LocalWorkflow {

const QString LocalDomainFactory::ID("domain.local.bio");

/*****************************
 * BaseWorker
 *****************************/
BaseWorker::BaseWorker(Actor* a, bool autoTransitBus)
: processDone(false), actor(a)
{
    foreach(Port* p, a->getPorts()) {
        if (qobject_cast<IntegralBusPort*>(p)) {
            IntegralBus* bus = new IntegralBus(p);
            ports.insert(p->getId(), bus);
            p->setPeer(bus);
        }
    }
    if (autoTransitBus) {
        foreach(Port* p, a->getInputPorts()) {
            IntegralBus* bus = p->castPeer<IntegralBus>();
            foreach(Port* op, a->getOutputPorts()) {
                if (p->isInput() != op->isInput()) {
                    IntegralBus* ob = op->castPeer<IntegralBus>();
                    ob->addComplement(bus);
                    bus->addComplement(ob);
                }
            }
        }
    }
    a->setPeer(this);
    //failFast = WorkflowSettings::failFast();
}

BaseWorker::~BaseWorker() {
    foreach(Port* p, actor->getPorts()) {
        if (qobject_cast<IntegralBusPort*>(p)) {
            p->setPeer(NULL);
        }
    }
    qDeleteAll(ports.values());
    actor->setPeer(NULL);
}

QStringList BaseWorker::getOutputFiles(){
    QStringList res;
    foreach(Attribute *attr, actor->getProto()->getAttributes()) {
        if(attr->getId() == BaseAttributes::URL_OUT_ATTRIBUTE().getId()) {
            QString str = actor->getParameter(BaseAttributes::URL_OUT_ATTRIBUTE().getId())->getAttributeValueWithoutScript<QString>();
            QUrl url(str);
            if(url.isValid()) {
                res << url.toString();
            }
        }
    }
    return res;
}

bool BaseWorker::addCommunication(const QString& id, CommunicationChannel* ch) {
    Q_UNUSED(id);
    Q_UNUSED(ch);
    assert(0);
    return false;
}

CommunicationChannel * BaseWorker::getCommunication(const QString& name) {
    return ports.value(name);
}

ActorId BaseWorker::getActorId() const {
    return actor->getId();
}

Message BaseWorker::getMessageAndSetupScriptValues( CommunicationChannel * channel ) {
    assert(channel != NULL);
    assert(channel->hasMessage());
    bindScriptValues();
    Message currentMessage = channel->get();
    currentMessage.isEmpty();
    messagesProcessedOnLastTick[channel].enqueue(currentMessage);

    return currentMessage;
}

void BaseWorker::bindScriptValues() {
    foreach( IntegralBus * bus, ports.values() ) {
        assert(bus != NULL);
        if(!bus->hasMessage()) { // means that it is bus for output port
            continue;
        }

        foreach( Attribute * attribute, actor->getParameters().values() ) {
            assert(attribute != NULL);
            setScriptVariableFromBus(&attribute->getAttributeScript(), bus);

            if(actor->getCondition()->hasVarWithId(attribute->getId())) {
                actor->getCondition()->setVarValueWithId(attribute->getId(), attribute->getAttributePureValue());
            }
        }

        QVariantMap busData = bus->lookMessage().getData().toMap();
        foreach(const QString & slotId, busData.keys()) {
            QString attrId = "in_" + slotId;
            if( actor->getCondition()->hasVarWithId(attrId)) {
                actor->getCondition()->setVarValueWithId(attrId, busData.value(slotId));
            }
        }
    }
}

void BaseWorker::setScriptVariableFromBus(AttributeScript *script, IntegralBus *bus) {
    QVariantMap busData = bus->look().getData().toMap();
    foreach(const QString & slotDesc, busData.keys()) {
        ActorId actorId = IntegralBusType::parseSlotDesc(slotDesc);
        QString attrId = IntegralBusType::parseAttributeIdFromSlotDesc(slotDesc);
        QString portId = bus->getPortId();
        IntegralBusPort * busPort = qobject_cast<IntegralBusPort*>(actor->getPort(portId));
        assert(busPort != NULL);

        Actor * bindedAttrOwner = busPort->getLinkedActorById(actorId);
        if(bindedAttrOwner == NULL) {
            continue;
        }
        //attrId.replace(".", "_");
        //attrId.replace("-", "_");
        if( !script->getScriptText().isEmpty() ) {
            //attrScript.setVarValueWithId(attrId, busData.value(slotDesc));
            script->setScriptVar(attrId, busData.value(slotDesc));
        }
    }
}

void BaseWorker::setDone() {
    processDone = true;
}

bool BaseWorker::isDone() const {
    return processDone;
}

bool BaseWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    QList<Port*> inPorts = actor->getInputPorts();
    if (inPorts.isEmpty()) {
        return true;
    } else if (1 == inPorts.size()) {
        IntegralBus *inChannel = ports.value(inPorts.first()->getId());
        int hasMsg = inChannel->hasMessage();
        bool ended = inChannel->isEnded();
        if (hasMsg || ended) {
            return true;
        }
    }

    return false;
}

void BaseWorker::saveCurrentChannelsStateAndRestorePrevious() {
    foreach(CommunicationChannel *channel, messagesProcessedOnLastTick.keys()) {
        assert(ports.values().contains(dynamic_cast<IntegralBus *>(channel)));

        QQueue<Message> currentMessagesBackup;
        while(channel->hasMessage()) {
            currentMessagesBackup.enqueue(channel->get());
        }
        addMessagesFromBackupToAppropriratePort(channel);

        messagesProcessedOnLastTick[channel] = currentMessagesBackup;
    }
}

WorkflowMonitor * BaseWorker::monitor() const {
    CHECK(NULL != context, NULL);
    return context->getMonitor();
}

void BaseWorker::reportError(const QString &message) {
    CHECK(NULL != monitor(), );
    monitor()->addError(message, getActorId());
}

void BaseWorker::restoreActualChannelsState() {
    foreach(CommunicationChannel *channel, messagesProcessedOnLastTick.keys()) {
        assert(!channel->hasMessage());
        addMessagesFromBackupToAppropriratePort(channel);
    }
}

QList<ExternalToolListener*> BaseWorker::createLogListeners(int listenersNumber) const {
    return context->getMonitor()->createWorkflowListeners(actor->getId(), actor->getLabel(), listenersNumber);
}

void BaseWorker::addMessagesFromBackupToAppropriratePort(CommunicationChannel *channel) {
    while(!messagesProcessedOnLastTick[channel].isEmpty()) {
        channel->put(messagesProcessedOnLastTick[channel].dequeue(), true);
    }
}

bool BaseWorker::canTaskBeCanceled(Task * /*workerTask*/) const {
    return false;
}

Task * BaseWorker::tick(bool &canResultBeCanceled) {
    Task *result = tick();
    if(NULL != result) {
        canResultBeCanceled = canTaskBeCanceled(result);
    }

    return result;
}

/*****************************
 * SimpleQueue
 *****************************/
SimpleQueue::SimpleQueue() : ended(false), takenMsgs(0) {
}

Message SimpleQueue::get() {
    assert(hasMessage());
    takenMsgs++;
    return que.dequeue();
}

Message SimpleQueue::look() const {
    assert(hasMessage());
    return que.head();
}

void SimpleQueue::put(const Message& m, bool isMessageRestored) {
    que.enqueue(m);
    if(isMessageRestored) {
        --takenMsgs;
    }
}

int SimpleQueue::hasMessage() const {
    return que.size();
}

int SimpleQueue::takenMessages() const {
    return takenMsgs;
}

int SimpleQueue::hasRoom(const DataType* ) const {
    return 1000;
}

bool SimpleQueue::isEnded() const {
    return ended && que.isEmpty();
}

void SimpleQueue::setEnded() {
    ended = true;
}

int SimpleQueue::capacity() const {
    return INT_MAX;
}

void SimpleQueue::setCapacity(int) {
}

QQueue<Message> SimpleQueue::getMessages(int startIndex, int endIndex) const {
    if(-1 == endIndex) {
        endIndex = hasMessage() - 1;
    }
    Q_ASSERT(0 <= startIndex && que.size() >= startIndex
        && 0 <= endIndex && que.size() >= endIndex);
    QQueue<Message> result;
    foreach(Message message, que.mid(startIndex, endIndex - startIndex + 1)) {
        result.enqueue(message);
    }
    return result;
}

/*****************************
 * LocalDomainFactory
 *****************************/
LocalDomainFactory::LocalDomainFactory() : DomainFactory(ID) {
}

// TODO: this function should be moved to WorkflowRunFromCMDLine.cpp
// It must be called only once and save its result to some registry
static QMap<QString, QMap<QString, QList<QString> > > getSlotsForPrint() {
    QMap<QString, QMap<QString, QList<QString> > > forPrint;
    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();

    int printOpIdx = CMDLineRegistryUtils::getParameterIndex("print");
    while (printOpIdx != -1) {
        QString printSlot = cmdLineRegistry->getParameterValue("print", printOpIdx); // TODO: "print" == WorkflowDesignerPlugin::PRINT
        printOpIdx++;
        if (!printSlot.isEmpty()) {
            QStringList tokens = printSlot.split(".");
            if (3 == tokens.size()) {
                QMap<QString, QList<QString> > ports = forPrint.value(tokens[0], QMap<QString, QList<QString> >());
                QList<QString> slotS = ports.value(tokens[1], QList<QString>());
                slotS.append(tokens[2]);
                ports.insert(tokens[1], slotS);
                forPrint.insert(tokens[0], ports);
            }
        } else {
            printOpIdx = -1;
        }
    }
    return forPrint;
}

static void addPrintSLots(IntegralBus *bus, Port *p) {
    QMap<QString, QMap<QString, QList<QString> > > forPrint = getSlotsForPrint();
    QString actorId = p->owner()->getId();
    if (forPrint.contains(actorId)) {
        QMap<QString, QList<QString> > ports = forPrint.value(actorId);
        if (ports.contains(p->getId())) {
            QList<QString> slotS = ports.value(p->getId());
            bus->setPrintSlots(p->isInput(), slotS);
        }
    }
}

static CommunicationSubject* setupBus(Port* p) {
    QString id = p->getId();
    BaseWorker* worker = p->owner()->castPeer<BaseWorker>();
    assert(worker);
    CommunicationSubject* subj = worker;
    IntegralBus* bus = qobject_cast<IntegralBus*>(p->castPeer<QObject>());
    if (bus) {
        assert(subj->getCommunication(id) == dynamic_cast<CommunicationChannel*>(bus));
        subj = bus;
    } else if (subj) {
        assert(0);
        bus = new IntegralBus(p);
        p->setPeer(bus);
        subj->addCommunication(id, bus);
        subj = bus;
        foreach(Port* op, p->owner()->getPorts()) {
            if (p->isInput() != op->isInput()) {
                IntegralBus* ob = qobject_cast<IntegralBus*>(op->castPeer<QObject>());
                if (ob) {
                    ob->addComplement(bus);
                    bus->addComplement(ob);
                }
            }
        }
    }
    addPrintSLots(bus, p);
    return subj;
}

Worker* LocalDomainFactory::createWorker(Actor* a) {
    Worker* w = NULL;
    DomainFactory* f = getById(a->getProto()->getId());
    if (f) {
        w = f->createWorker(a);
#ifdef _DEBUG
        assert(w);
        BaseWorker* bw = dynamic_cast<BaseWorker*>(w);
        assert(qobject_cast<BaseWorker*>(bw));
        assert(bw == a->getPeer());
#endif
    }

    return w;
}

CommunicationChannel* LocalDomainFactory::createConnection(Link* l) {
    SimpleQueue* cc = NULL;
    QString srcId = l->source()->getId();
    QString dstId = l->destination()->getId();
    CommunicationSubject* src = setupBus(l->source());
    CommunicationSubject* dst = setupBus(l->destination());
    if (src && dst) {
        cc = new SimpleQueue();
        src->addCommunication(srcId, cc);
        dst->addCommunication(dstId, cc);
    }
    l->setPeer(cc);
    return cc;
}

Scheduler* LocalDomainFactory::createScheduler(Schema* sh) {
    Scheduler *sc = new LastReadyScheduler(sh);
    return sc;
}

void LocalDomainFactory::destroy( Scheduler* sh, Schema* schema) {
    foreach(Link* l, schema->getFlows()) {
        delete l->castPeer<SimpleQueue>();
        l->setPeer(NULL);
    }

    foreach(Actor* a, schema->getProcesses()) {
        delete a->castPeer<BaseWorker>();
    }

    delete sh;
}

} // Workflow namespace
} // U2 namespace
