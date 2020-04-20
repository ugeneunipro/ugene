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

#include "IntegralBus.h"
#include "IntegralBusType.h"

#include <U2Core/L10n.h>
#include <U2Core/Log.h>

#include <U2Lang/WorkflowUtils.h>

#include <limits.h>
#include <QMutexLocker>

namespace U2 {
namespace Workflow {

/************************************************************************/
/* BusMap */
/************************************************************************/
static const QString PATH_SEPARATOR = QString(">");
static const QString PATH_LIST_SEPARATOR = QString(",");

BusMap::BusMap(const StrStrMap &busMap, const QMap<QString, QStringList> &listMap, const SlotPathMap &paths)
: input(true), busMap(busMap), listMap(listMap), paths(paths)
{
}

BusMap::BusMap(const StrStrMap &busMap, bool breaksDataflow, const QString &actorId)
: input(false), busMap(busMap), breaksDataflow(breaksDataflow), actorId(actorId)
{
}

void BusMap::parseSource(const QString &src, QString &srcId, QStringList &path) {
    int sepPos = src.indexOf(PATH_SEPARATOR);
    path.clear();
    if (-1 != sepPos) {
        srcId = src.left(sepPos);
        foreach (const QString &procId, src.mid(sepPos + 1).split(PATH_LIST_SEPARATOR)) {
            path << procId.trimmed();
        }
    } else {
        srcId = src;
    }
}

inline bool equalPaths(const SlotPathMap &allPaths, const QStringList &ipath, const QString &dest, const QString &src) {
    QPair<QString, QString> slotPair(dest, src);
    QList<QStringList> paths = allPaths.values(slotPair);

    if (paths.isEmpty()) {
        return ipath.isEmpty();
    }

    foreach (const QStringList &path, paths) {
        if (path.size() == ipath.size()) {
            foreach (const QString &id, ipath) {
                if (!path.contains(id)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

Message BusMap::takeMessageMap(CommunicationChannel *ch, QVariantMap &context) {
    assert(input);
    Q_UNUSED(input);

    Message m = ch->get();
    assert(m.getData().type() == QVariant::Map);
    context.unite(m.getData().toMap());

    return Message(m.getType(), getMessageData(m), m.getMetadataId());
}

Message BusMap::lookMessageMap(CommunicationChannel *ch) {
    Message m = ch->look();
    return Message(m.getType(), getMessageData(m), m.getMetadataId());
}

QVariantMap BusMap::getMessageData(const Message &m) const {
    assert(m.getData().type() == QVariant::Map);
    QVariantMap imap = m.getData().toMap();
    QStringList ipath;
    QString ikey;

    QVariantMap result;
    foreach(QString src, imap.uniqueKeys()) {
        QVariant ival = imap.value(src);

        parseSource(src, ikey, ipath);
        foreach(QString rkey, busMap.keys(ikey)) {
            if (equalPaths(paths, ipath, rkey, ikey)) {
                coreLog.trace("reducing bus from key="+ikey+" to="+rkey);
                result[rkey] = ival;
            }
        }

        QMapIterator<QString,QStringList> lit(listMap);
        while (lit.hasNext())
        {
            lit.next();
            QString rkey = lit.key();
            assert(!lit.value().isEmpty());
            if (lit.value().contains(src)) {
                QVariantList vl = result[rkey].toList();
                if (m.getType()->getDatatypeByDescriptor(src)->isList()) {
                    vl += ival.toList();
                    coreLog.trace("reducing bus key="+src+" to list of "+rkey);
                } else {
                    vl.append(ival);
                    coreLog.trace("reducing bus key="+src+" to list element of "+rkey);
                }
                result[rkey] = vl;
            }
        }
    }

    return result;
}

QString BusMap::getNewSourceId(const QString &srcId, const QString &actorId) {
    int sepPos = srcId.indexOf(PATH_SEPARATOR);
    QString path = actorId;
    QString res;
    if (-1 != sepPos) {
        path.prepend(srcId.mid(sepPos + 1) + PATH_LIST_SEPARATOR);
        res = srcId.left(sepPos);
    } else {
        res = srcId;
    }
    return res + PATH_SEPARATOR + path;
}

QVariantMap BusMap::composeMessageMap(const Message &m, const QVariantMap &context) {
    QVariantMap data;
    if (breaksDataflow) {
        foreach (const QString &key, context.keys()) {
            data[getNewSourceId(key, actorId)] = context.value(key);
        }
    } else {
        data = context;
    }
    if (m.getData().type() == QVariant::Map) {
        QMapIterator<QString, QVariant> it(m.getData().toMap());
        while (it.hasNext()) {
            it.next();
            QString key = busMap.value(it.key());
            coreLog.trace("putting key="+key+" remapped from="+it.key());
            data.insert(key, it.value());
        }
    } else {
        assert(busMap.size() == 1);
        data.insert(busMap.values().first(), m.getData());
    }
    return data;
}

/************************************************************************/
/* IntegralBus */
/************************************************************************/

static QMap<QString, QStringList> getListMappings(const StrStrMap& busMap, const SlotPathMap &pathMap, const Port* p) {
    StrStrMap bm = busMap;
    WorkflowUtils::applyPathsToBusMap(bm, pathMap);
    assert(p->isInput());
    DataTypePtr dt = p->getType();
    QMap<QString, QStringList> res;
    if (dt->isList()) {
        if (bm.contains(p->getId())) {
            res.insert(p->getId(), bm.value(p->getId()).split(";"));
        }
    } else if (dt->isMap()) {
        foreach(Descriptor d, dt->getAllDescriptors()) {
            if (dt->getDatatypeByDescriptor(d)->isList() && bm.contains(d.getId())) {
                res.insert(d.getId(), bm.value(d.getId()).split(";"));
            }
        }
    }
    return res;
}


IntegralBus::IntegralBus(Port* p)
: busType(p->getType()), contextMetadataId(-1), complement(NULL), portId(p->getId()), takenMsgs(0), workflowContext(NULL) {
    actorId = p->owner()->getId();
    QString name = p->owner()->getLabel() + "[" + p->owner()->getId()+"]";
    contextMutex = new QMutex();
    if (p->isInput()) {
        Attribute* a = p->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID);
        if(a == NULL) {
            assert(false);
            return;
        }

        StrStrMap map = a->getAttributeValueWithoutScript<StrStrMap>();
        if (map.isEmpty()) {
            ActorPrototype *proto = p->owner()->getProto();
            assert(proto->isAllowsEmptyPorts());
            Q_UNUSED(proto);
        }
        QMapIterator<QString, QString> it(map);
        while (it.hasNext()) {
            it.next();
            coreLog.trace(QString("%1 - input bus map key=%2 val=%3").arg(name).arg(it.key()).arg(it.value()));
        }

        IntegralBusPort *busPort = qobject_cast<IntegralBusPort*>(p);
        SlotPathMap pathMap = busPort->getPaths();
        QMap<QString, QStringList> listMap = getListMappings(map, pathMap, p);
        busMap = new BusMap(map, listMap, pathMap);
    } else { // p is output
        StrStrMap map;
        IntegralBusPort* bp = qobject_cast<IntegralBusPort*>(p);
        DataTypePtr t = bp ? bp->getOwnType() : p->getType();
        if (t->isMap()) {
            foreach(Descriptor d, t->getAllDescriptors()) {
                QString key = d.getId();
                QString val = IntegralBusType::assignSlotDesc(d, p).getId();
                map.insert(key, val);
            }
        } else {
            QString key = p->getId();
            QString val = IntegralBusType::assignSlotDesc(*p, p).getId();
            map.insert(key, val);
        }
        QMapIterator<QString, QString> it(map);
        while (it.hasNext()) {
            it.next();
            coreLog.trace(QString("%1 - output bus map key=%2 val=%3").arg(name).arg(it.key()).arg(it.value()));
        }

        Actor *proc = p->owner();
        busMap = new BusMap(map, proc->getProto()->getInfluenceOnPathFlag(), proc->getId());
    }
}

bool IntegralBus::addCommunication(const QString& id, CommunicationChannel* ch) {
    outerChannels.insertMulti(id, ch);
    return true;
}

CommunicationChannel * IntegralBus::getCommunication(const QString& id) {
    return outerChannels.value(id);
}

Message IntegralBus::get() {
    QVariantMap result;
    int metadataId = MessageMetadata::INVALID_ID;
    lastMessageContext.clear();
    foreach (CommunicationChannel *ch, outerChannels) {
        Message message = busMap->takeMessageMap(ch, lastMessageContext);
        QVariantMap data = message.getData().toMap();
        result.unite(data);

        if (1 == outerChannels.size()) {
            metadataId = message.getMetadataId();
        } else {
            // Actually there is always 1 item in the list because only one connection for one input port is allowed.
            // But if there are several connections in the future then metadata will be lost as it is in the multiplexer.
            // If you support several input connections remove this branch.
            coreLog.error(L10N::internalError("Several input channels"));
            assert(0);
        }
    }

    if (!printSlots.isEmpty()) {
        foreach (const QString &key, result.keys()) {
            if (printSlots.contains(key)) {
                QString slotString = actorId + "." + portId + "." + key;
                DataTypePtr type = busType->getDatatypesMap().value(key, DataTypePtr());
                if (NULL != type.data()) {
                    WorkflowUtils::print(slotString, result.value(key), type, workflowContext);
                }
            }
        }
    }
    QVariant data;
    if (busType->isMap()) {
        data.setValue(result);
    } else if (result.size() == 1) {
        data = result.values().at(0);
    }
    if (complement) {
        complement->setContext(lastMessageContext, metadataId);
    }

    takenMsgs++;
    return Message(busType, data, metadataId);
}

QQueue<Message> IntegralBus::getMessages(int startIndex, int endIndex) const {
    QQueue<Message> result;

    QMap<CommunicationChannel *, QQueue<Message> > messagesFromChannels;
    foreach(CommunicationChannel* channel, outerChannels) {
        assert(channel != NULL);
        QQueue<Message> channelMessages = channel->getMessages(startIndex, endIndex);
        messagesFromChannels[channel] = channelMessages;
    }

    for(qint32 messageCount = 0; messageCount
        < messagesFromChannels[messagesFromChannels.keys().first()].size(); ++messageCount)
    {
        QVariantMap resultingMessageMap;
        int metadataId = -1;
        foreach(CommunicationChannel *channel, messagesFromChannels.keys()) {
            Message message = messagesFromChannels[channel][messageCount];
            if (message.getData().type() != QVariant::Map) {
                coreLog.error(L10N::internalError("No message map"));
                assert(0);
            }
            resultingMessageMap.unite(message.getData().toMap());
            if (1 == outerChannels.size()) {
                metadataId = message.getMetadataId();
            }
        }
        result.enqueue(Message(busType, resultingMessageMap, metadataId));
    }

    return result;
}

Message IntegralBus::look() const {
    QVariantMap result;
    int metadataId = -1;
    foreach(CommunicationChannel* channel, outerChannels) {
        assert(channel != NULL);
        Message message = channel->look();
        assert(message.getData().type() == QVariant::Map);
        result.unite(message.getData().toMap());
        if (1 == outerChannels.size()) {
            metadataId = message.getMetadataId();
        }
    }
    return Message(busType, result, metadataId);
}

Message IntegralBus::lookMessage() const {
    QVariantMap result;
    int metadataId = -1;
    foreach (CommunicationChannel* ch, outerChannels) {
        Message message = busMap->lookMessageMap(ch);
        result.unite(message.getData().toMap());
        if (1 == outerChannels.size()) {
            metadataId = message.getMetadataId();
        }
    }
    QVariant data;
    if (busType->isMap()) {
        data.setValue(result);
    } else if (1 == result.size()) {
        data = result.values().first();
    }
    return Message(busType, data, metadataId);
}

Message IntegralBus::composeMessage(const Message& m) {
    QVariantMap data(busMap->composeMessageMap(m, getContext()));
    context.clear();
    int metadataId = m.getMetadataId();
    if (-1 != contextMetadataId) {
        metadataId =  contextMetadataId;
    }
    return Message(busType, data, metadataId);
}

void IntegralBus::put(const Message& m, bool isMessageRestored) {
    Message busMessage = composeMessage(m);
    foreach(CommunicationChannel* ch, outerChannels) {
        ch->put(busMessage, isMessageRestored);
    }
    if ( !printSlots.isEmpty() && (m.getData().type() == QVariant::Map) ) {
        QVariantMap map = m.getData().toMap();
        foreach (const QString &key, map.keys()) {
            if (printSlots.contains(key)) {
                QString slotString = actorId + "." + portId + "." + key;
                IntegralBusSlot slot(key, portId, actorId);
                DataTypePtr type = busType->getDatatypesMap().value(slot.toString(), DataTypePtr());
                if (NULL != type.data()) {
                    WorkflowUtils::print(slotString, map.value(key), type, workflowContext);
                }
            }
        }
    }

    if(isMessageRestored) {
        --takenMsgs;
    }
}

void IntegralBus::transit() {
    this->put(Message::getEmptyMapMessage());
}

void IntegralBus::putWithoutContext(const Message& m) {
    foreach(CommunicationChannel* ch, outerChannels) {
        ch->put(m);
    }
}

int IntegralBus::hasMessage() const {
    if (outerChannels.isEmpty()) {
        return 0;
    }
    int num = INT_MAX;
    foreach(CommunicationChannel* ch, outerChannels) {
        num = qMin(num, ch->hasMessage());
    }
    return num;
}

int IntegralBus::takenMessages() const {
    return takenMsgs;
}

int IntegralBus::hasRoom(const DataType*) const {
    if (outerChannels.isEmpty()) {
        return 0;
    }
    int num = INT_MAX;
    foreach(CommunicationChannel* ch, outerChannels) {
        num = qMin(num, ch->hasRoom());
    }
    return num;
}

bool IntegralBus::isEnded() const {
    foreach(CommunicationChannel* ch, outerChannels) {
        if (ch->isEnded()) {
#ifdef _DEBUG
            foreach(CommunicationChannel* dbg, outerChannels) {
                assert(dbg->isEnded());
            }
#endif
            return true;
        }
    }
    return false;
}

void IntegralBus::setEnded() {
    foreach(CommunicationChannel* ch, outerChannels) {
        ch->setEnded();
    }
}

void IntegralBus::setPrintSlots(bool in, const QList<QString> &printSlots) {
    Q_UNUSED(in);
    this->printSlots = printSlots;
}

void IntegralBus::setWorkflowContext(WorkflowContext *context) {
    workflowContext = context;
}

IntegralBus::~IntegralBus() {
    delete contextMutex;
    delete busMap;
}

void IntegralBus::setContext(const QVariantMap &m, int metadataId) {
    QMutexLocker lock(contextMutex);
    context.unite(m);
    contextMetadataId = metadataId;
}

int IntegralBus::getContextMetadataId() const {
    return contextMetadataId;
}

}//namespace Workflow
}//namespace U2
