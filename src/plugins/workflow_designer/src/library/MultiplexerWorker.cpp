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
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "MultiplexerWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString MultiplexerWorkerFactory::ACTOR_ID("multiplexer");

static const QString EMPTY_TYPESET_ID("empty");
static const QString INPUT_PORT_1("input-data-1");
static const QString INPUT_PORT_2("input-data-2");
static const QString OUTPUT_PORT("output-data");

static const QString RULE_ID("multiplexing-rule");

/*******************************
 * MultiplexerWorker
 *******************************/
MultiplexerWorker::MultiplexerWorker(Actor *p)
: BaseWorker(p, false), inChannel1(NULL), inChannel2(NULL), outChannel(NULL), rule(ONE_TO_ONE),
hasMultiData(false), multiMetadataId(-1), messagesInited(false)
{
}

void MultiplexerWorker::init() {
    rule = actor->getParameter(RULE_ID)->getAttributeValue<uint>(context);
    inChannel1 = ports.value(INPUT_PORT_1);
    inChannel2 = ports.value(INPUT_PORT_2);
    outChannel = ports.value(OUTPUT_PORT);
    hasMultiData = false;
    messagesInited = false;
}

bool MultiplexerWorker::isReady() const {
    if (isDone()) {
        return false;
    }

    int hasMsg1 = inChannel1->hasMessage();
    int hasMsg2 = inChannel2->hasMessage();
    bool ended1 = inChannel1->isEnded();
    bool ended2 = inChannel2->isEnded();

    if (ONE_TO_MANY == rule) {
        if (hasMsg1 && hasMsg2) {
            return true;
        } else if (hasMsg1) {
            return ended2;
        } else if (hasMultiData || ended1) {
            return hasMsg2 || ended2;
        }
    } else if (ONE_TO_ONE == rule) {
        if (hasMsg1 && hasMsg2) {
            return true;
        } else if (hasMsg1) {
            return ended2;
        } else if (hasMsg2) {
            return ended1;
        }
    }

    return ended1 && ended2;
}

void MultiplexerWorker::shutDown() {
    outChannel->setEnded();
    setDone();
}

bool MultiplexerWorker::checkIfEnded() {
    if (inChannel1->isEnded() && inChannel2->isEnded()) {
        shutDown();
        return true;
    }
    return false;
}

bool MultiplexerWorker::hasDataFotMultiplexing() const {
    return inChannel1->hasMessage() || hasMultiData;
}

inline void MultiplexerWorker::sendUnitedMessage(const QVariantMap &m1, QVariantMap &m2, int metadataId) {
    m2.unite(m1);
    outChannel->putWithoutContext(Message(outChannel->getBusType(), m2, metadataId));
}

Task *MultiplexerWorker::tick() {
    if (ONE_TO_MANY == rule) {
        multiplexManyMode();
    } else if (ONE_TO_ONE == rule) {
        if (checkIfEnded()) {
            return NULL;
        }

        bool bothData = inChannel1->hasMessage() && inChannel2->hasMessage();
        if (!bothData) {
            if (inChannel1->hasMessage() || inChannel2->hasMessage()) {
                monitor()->addError(getMessagesMismatchError(), getActorId(), Problem::U2_INFO);
            }
            shutDown();
            return NULL;
        }

        QVariantMap m1, m2;
        int metadataId = -1;
        if (inChannel1->hasMessage()) {
            Message m = inChannel1->look();
            m1 = m.getData().toMap();
            metadataId = m.getMetadataId();
            inChannel1->get();
        }
        if (inChannel2->hasMessage()) {
            m2 = inChannel2->look().getData().toMap();
            inChannel2->get();
        }

        sendUnitedMessage(m1, m2, metadataId);
        checkIfEnded();
    } else {
        QVariantMap m1, m2;
        int metadataId = -1;
        bool hasMessage = inChannel1->hasMessage() || inChannel2->hasMessage();
        if (inChannel1->hasMessage()) {
            Message m = inChannel1->look();
            m1 = m.getData().toMap();
            metadataId = m.getMetadataId();
            inChannel1->get();
        } else if (inChannel2->hasMessage()) {
            m2 = inChannel2->look().getData().toMap();
            inChannel2->get();
        }

        if (hasMessage) {
            sendUnitedMessage(m1, m2, metadataId);
        }
        checkIfEnded();
    }
    return NULL;
}

void MultiplexerWorker::multiplexManyMode() {
    if (!hasDataFotMultiplexing()) {
        if (checkIfEnded()) {
            return;
        }
    }
    QVariantMap m1;
    int metadataId = -1;
    if (hasMultiData) {
        m1 = multiData;
        metadataId = multiMetadataId;
    } else {
        if (inChannel1->hasMessage()) {
            Message m = inChannel1->look();
            m1 = m.getData().toMap();
            metadataId = m.getMetadataId();
            inChannel1->get(); // pop last message
        } else {
            shutDown();
        }
        hasMultiData = true;
        multiData = m1;
        multiMetadataId = metadataId;
    }

    if (messagesInited) {
        if (messages.isEmpty()) {
            shutDown();
        } else {
            foreach (QVariantMap m2, messages) {
                sendUnitedMessage(m1, m2, metadataId);
            }
        }
        hasMultiData = false;
        multiData.clear();
        multiMetadataId = -1;
    } else {
        while (inChannel2->hasMessage()) {
            QVariantMap m2 = inChannel2->look().getData().toMap();
            inChannel2->get(); // pop last message
            messages << m2;

            sendUnitedMessage(m1, m2, metadataId);
        }
        if (inChannel2->isEnded()) {
            if (messages.isEmpty()) {
                shutDown();
            }
            messagesInited = true;
            hasMultiData = false;
            multiData.clear();
            multiMetadataId = -1;
        }
    }
    if (!hasMultiData && inChannel1->isEnded()) { // nothing else to multiplex
        outChannel->setEnded();
        setDone();
    }
}

QString MultiplexerWorker::getInputActorName(IntegralBus *bus) const {
    Port *port = actor->getPort(bus->getPortId());
    SAFE_POINT(NULL != port, L10N::nullPointerError("Port"), "");
    SAFE_POINT(!port->getLinks().isEmpty(), "No input links", "");
    Port *inputPort = port->getLinks().keys().first();
    return inputPort->owner()->getLabel();
}

QString MultiplexerWorker::getMessagesMismatchError() const {
    int messages1 = inChannel1->takenMessages() + inChannel1->hasMessage();
    int messages2 = inChannel2->takenMessages() + inChannel2->hasMessage();
    QString inputName1 = getInputActorName(inChannel1);
    QString inputName2 = getInputActorName(inChannel2);
    return tr("The number of messages, received from \"%1\" (%2), does not correspond to the number of messages, received from \"%3\" (%4). Redundant messages were skipped.")
        .arg(inputName1).arg(messages1).arg(inputName2).arg(messages2);
}

void MultiplexerWorker::cleanup() {
}

/*******************************
 * MultiplexerWorkerFactory
 *******************************/
void MultiplexerWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> emptyTypeMap;
        DataTypePtr emptyTypeSet(new MapDataType(Descriptor(DataType::EMPTY_TYPESET_ID), emptyTypeMap));

        // input ports
        Descriptor inputDesc1(INPUT_PORT_1, MultiplexerWorker::tr("First input port"), MultiplexerWorker::tr(
            "One of the two input ports of the <i>Multiplexer</i> element. When rule \"1 to many\" is set up,"
            " each message from this port is concatenated with messages from the other port."));
        Descriptor inputDesc2(INPUT_PORT_2, MultiplexerWorker::tr("Second input port"), MultiplexerWorker::tr(
            "One of the two input ports of the <i>Multiplexer</i> element. When rule \"1 to many\" is set up,"
            " each message from the other port is concatenated with messages from this port."));
        portDescs << new PortDescriptor(inputDesc1, emptyTypeSet, true);
        portDescs << new PortDescriptor(inputDesc2, emptyTypeSet, true);

        // output port
        Descriptor outputDesc(OUTPUT_PORT, MultiplexerWorker::tr("Multiplexed Output"),
            MultiplexerWorker::tr("The port outputs multiplexed messages."));
        portDescs << new PortDescriptor(outputDesc, emptyTypeSet, false, true);
    }

    QList<Attribute*> attrs;
    {
        // attributes
        Descriptor ruleDesc(RULE_ID, MultiplexerWorker::tr("Multiplexing rule"),
            MultiplexerWorker::tr("Specifies how to multiplex the input messages:"
                " <li><b>1 to 1</b> - the multiplexer gets one message from the first input port"
                " and one message from the second input port, joins them into a single message, and transfers it to the"
                " output. This procedure is repeated while there are available messages in both input ports.</li>"
                " <li><b>1 to many</b> - the multiplexer gets one message from the first input port, joins it with each"
                " message from the second input port, and transfers the joined messages to the output. This procedure"
                " is repeated for each message from the first input port.</li>"
                " <br/>"));

        attrs << new Attribute(ruleDesc, BaseTypes::STRING_TYPE(), true, ONE_TO_ONE);
    }

    QMap<QString, PropertyDelegate*> delegateMap;
    {
        // delegates
        QVariantMap rules;
        rules[MultiplexerWorker::tr("1 to many")] = ONE_TO_MANY;
        rules[MultiplexerWorker::tr("1 to 1")] = ONE_TO_ONE;

        delegateMap[RULE_ID] = new ComboBoxDelegate(rules);
    }

    Descriptor protoDesc(MultiplexerWorkerFactory::ACTOR_ID,
        MultiplexerWorker::tr("Multiplexer"),
        MultiplexerWorker::tr("The element allows one to join two data flows into a single data flow,"
            " i.e. to join messages from two input ports into concatenated messages and send them to the output."
            " The concatenation approach is determined by the <i>Multiplexing rule</i> parameter."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);

    proto->setEditor(new DelegateEditor(delegateMap));
    proto->setPrompter(new MultiplexerPrompter());
    proto->setAllowsEmptyPorts(true);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATAFLOW(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new MultiplexerWorkerFactory());
}

Worker *MultiplexerWorkerFactory::createWorker(Actor* a) {
    return new MultiplexerWorker(a);
}

/*******************************
 * MultiplexerPrompter
 *******************************/
QString MultiplexerPrompter::composeRichDoc() {
    uint rule = getParameter(RULE_ID).toUInt();

    IntegralBusPort* input1 = qobject_cast<IntegralBusPort*>(target->getPort(INPUT_PORT_1));
    IntegralBusPort* input2 = qobject_cast<IntegralBusPort*>(target->getPort(INPUT_PORT_2));

    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    QString inputName1 = unsetStr;
    if (input1->getLinks().size() > 0) {
        Port *p = input1->getLinks().keys().first();
        inputName1 = p->owner()->getLabel();
    }
    QString inputName2 = unsetStr;
    if (input2->getLinks().size() > 0) {
        Port *p = input2->getLinks().keys().first();
        inputName2 = p->owner()->getLabel();
    }

    if (ONE_TO_ONE == rule) {
        return tr("Gets one message from <u>%1</u> and one message from <u>%2</u>,"
            " joins them into a single message, and transfers it to the output."
            " Repeats this while there are available messages in both input ports.").arg(inputName1).arg(inputName2);
    } else {
        return tr("Gets one message from <u>%1</u>, joins it with each message from <u>%2</u>,"
            " and transfers the joined messages to the output."
            " Repeats this for each message from <u>%1</u>.").arg(inputName1).arg(inputName2);
    }
}

} // LocalWorkflow
} // U2
