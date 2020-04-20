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

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/Marker.h>
#include <U2Lang/WorkflowEnv.h>

#include "PassFilterWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString PassFilterWorkerFactory::ACTOR_ID("filter-by-values");

/*******************************
 * FilterSequenceWorker
 *******************************/
PassFilterWorker::PassFilterWorker(Actor *p)
: BaseWorker(p), inChannel(NULL), outChannel(NULL)
{
}

void PassFilterWorker::init() {
    inChannel = ports.value("in-data");
    outChannel = ports.value("filtered-data");
    mtype = ports.value("filtered-data")->getBusType();

    foreach (QString val, actor->getAttributes().first()->getAttributePureValue().toString().split(",")) {
        passedValues << val.trimmed();
    }
}

Task *PassFilterWorker::tick() {
    while (inChannel->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(inChannel);
        if (inputMessage.isEmpty()) {
            continue;
        }
        QVariantMap data = inputMessage.getData().toMap();
        QString value = data.value(BaseSlots::TEXT_SLOT().getId()).toString();

        if (passedValues.contains(value)) {
            outChannel->put(Message::getEmptyMapMessage());
        }
    }
    if (inChannel->isEnded()) {
        outChannel->setEnded();
        setDone();
    }
    return NULL;
}

void PassFilterWorker::cleanup() {
}

/*******************************
 * FilterSequenceWorkerFactory
 *******************************/
void PassFilterWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attrs;

    QMap<Descriptor, DataTypePtr> inTypeMap;
    QMap<Descriptor, DataTypePtr> outTypeMap;
    Descriptor passDesc(BaseSlots::TEXT_SLOT().getId(), PassFilterWorker::tr("Input values"), PassFilterWorker::tr("Input values."));
    Descriptor outDesc("filtered_data", PassFilterWorker::tr("Filtered data"), PassFilterWorker::tr("Filtered data"));
    inTypeMap[passDesc] = BaseTypes::STRING_TYPE();
    DataTypePtr inTypeSet(new MapDataType(BaseSlots::TEXT_SLOT(), inTypeMap));
    DataTypePtr outTypeSet(new MapDataType(outDesc, outTypeMap));

    portDescs << new PortDescriptor("in-data", inTypeSet, true);
    portDescs << new PortDescriptor("filtered-data", outTypeSet, false);

    Descriptor passVals(BaseSlots::TEXT_SLOT().getId(),
        PassFilterWorker::tr("Filter by value(s)"),
        PassFilterWorker::tr("Comma-separated list of values used to filter the input data."));
    attrs << new Attribute(passVals, BaseTypes::STRING_TYPE(), true);

    Descriptor protoDesc(PassFilterWorkerFactory::ACTOR_ID,
        PassFilterWorker::tr("Filter"),
        PassFilterWorker::tr("Passes through only data that matches the input filter value (or values)."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
    proto->setPrompter(new PassFilterPrompter());
    proto->setInfluenceOnPathFlag(true);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_DATAFLOW(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new PassFilterWorkerFactory());
}

Worker *PassFilterWorkerFactory::createWorker(Actor* a) {
    return new PassFilterWorker(a);
}

/*******************************
 * FilterSequencePrompter
 *******************************/
QString PassFilterPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort("in-data"));
    Actor* producer = input->getProducer(BaseSlots::TEXT_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr("<u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);
    QString passVals = getRequiredParam(BaseSlots::TEXT_SLOT().getId());
    passVals = getHyperlink(BaseSlots::TEXT_SLOT().getId(), passVals);

    QString res = tr("Filters input data from %1 by value(s) %2.").arg(producerName).arg(passVals);
    return res;
}


} // LocalWorkflow
} // U2
