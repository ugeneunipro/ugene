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
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "SequenceQualityTrimWorker.h"
#include "tasks/SequenceQualityTrimTask.h"

namespace U2 {
namespace LocalWorkflow {

static const QString QUALITY_ID("qual-id");
static const QString LEN_ID("len-id");
static const QString BOTH_ID("both-ends");

SequenceQualityTrimPrompter::SequenceQualityTrimPrompter(Actor *actor)
    : SequenceQualityTrimBase(actor)
{

}

QString SequenceQualityTrimPrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort *>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    const Actor *producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    const QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    const QString producerName = tr("from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);
    const QString trimSide = getHyperlink(BOTH_ID, tr(getParameter(BOTH_ID).toBool() ? "the both ends" : "the end"));
    return tr("Trim input sequence %1 from %2, using the quality threshold.").arg(producerName).arg(trimSide);
}

SequenceQualityTrimWorker::SequenceQualityTrimWorker(Actor *actor)
    : BaseThroughWorker(actor, BasePorts::IN_SEQ_PORT_ID(), BasePorts::OUT_SEQ_PORT_ID())
{

}

Task *SequenceQualityTrimWorker::createTask(const Message &message, U2OpStatus &os) {
    SequenceQualityTrimTaskSettings settings;
    settings.qualityTreshold = getValue<int>(QUALITY_ID);
    settings.minSequenceLength = getValue<int>(LEN_ID);
    settings.trimBothEnds = getValue<bool>(BOTH_ID);

    const QVariantMap dataMap = message.getData().toMap();
    const SharedDbiDataHandler sequenceHandler = dataMap[BaseSlots::DNA_SEQUENCE_SLOT().getId()].value<SharedDbiDataHandler>();
    settings.sequenceObject = StorageUtils::getSequenceObject(context->getDataStorage(), sequenceHandler);
    CHECK_EXT(NULL != settings.sequenceObject, os.setError(tr("There is no sequence object in the message")), NULL);

    return new SequenceQualityTrimTask(settings);
}

QList<Message> SequenceQualityTrimWorker::fetchResult(Task *task, U2OpStatus &os) {
    QList<Message> messages;
    SequenceQualityTrimTask *trimTask = qobject_cast<SequenceQualityTrimTask *>(task);
    SAFE_POINT_EXT(NULL != trimTask, os.setError(tr("An unexpected task type")), messages);

    QScopedPointer<U2SequenceObject> trimmedSequenceObject(trimTask->takeTrimmedSequence());
    SAFE_POINT_EXT(NULL != trimmedSequenceObject, os.setError("Sequence trim task didn't produce any object without any errors"), messages);
    if (0 == trimmedSequenceObject->getSequenceLength()) {
        monitor()->addError(tr("Sequence was filtered out by quality"), actor->getId(), WorkflowNotification::U2_WARNING);
        return messages;
    }
    SharedDbiDataHandler trimmedSequenceHandler = context->getDataStorage()->putSequence(trimmedSequenceObject.data());

    QVariantMap data;
    data[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = QVariant::fromValue<SharedDbiDataHandler>(trimmedSequenceHandler);
    messages << Message(output->getBusType(), data);

    return messages;
}

const QString SequenceQualityTrimWorkerFactory::ACTOR_ID = "SequenceQualityTrim";

SequenceQualityTrimWorkerFactory::SequenceQualityTrimWorkerFactory() :
    DomainFactory(ACTOR_ID)
{

}

void SequenceQualityTrimWorkerFactory::init() {
    Descriptor desc(ACTOR_ID, SequenceQualityTrimWorker::tr("Sequence Quality Trimmer"),
        SequenceQualityTrimWorker::tr("The workflow scans each input sequence from the end to find the first position where the quality is greater or equal to the minimum quality threshold. "
                              "Then it trims the sequence to that position. If a the whole sequence has quality less than the threshold or the length of the output sequence less than "
                              "the minimum length threshold then the sequence is skipped."));

    QList<PortDescriptor *> ports;
    {
        Descriptor inPortDescriptor(BasePorts::IN_SEQ_PORT_ID(), SequenceQualityTrimWorker::tr("Input Sequence"),
            SequenceQualityTrimWorker::tr("Set of sequences to trim by quality"));
        Descriptor outPortDescriptor(BasePorts::OUT_SEQ_PORT_ID(), SequenceQualityTrimWorker::tr("Output Sequence"),
            SequenceQualityTrimWorker::tr("Trimmed sequences"));

        QMap<Descriptor, DataTypePtr> inSlot;
        inSlot[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        DataTypePtr inType(new MapDataType(BasePorts::IN_SEQ_PORT_ID(), inSlot));
        ports << new PortDescriptor(inPortDescriptor, inType, true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        DataTypePtr outType(new MapDataType(BasePorts::OUT_SEQ_PORT_ID(), inSlot));
        ports << new PortDescriptor(outPortDescriptor, outType, false, true);
    }

    QList<Attribute *> attributes;
    {
        Descriptor qualityTreshold(QUALITY_ID, SequenceQualityTrimWorker::tr("Trimming quality threshold"),
                                   SequenceQualityTrimWorker::tr("Quality threshold for trimming."));

        Descriptor minSequenceLength(LEN_ID, SequenceQualityTrimWorker::tr("Min length"),
                                     SequenceQualityTrimWorker::tr("Too short reads are discarded by the filter."));

        Descriptor trimBothEnds(BOTH_ID, SequenceQualityTrimWorker::tr("Trim both ends"),
                                SequenceQualityTrimWorker::tr("Trim the both ends of a read or not. Usually, you need to set <b>True</b> for <b>Sanger</b> sequencing and <b>False</b> for <b>NGS</b>"));

        attributes << new Attribute(qualityTreshold, BaseTypes:: NUM_TYPE(), false, QVariant(30));
        attributes << new Attribute(minSequenceLength, BaseTypes::NUM_TYPE(), false, QVariant(0));
        attributes << new Attribute(trimBothEnds, BaseTypes::BOOL_TYPE(), false, true);
    }

    QMap<QString, PropertyDelegate *> delegates;
    {
        QVariantMap intLimitsMap;
        intLimitsMap["minimum"] = 0;
        intLimitsMap["maximum"] = INT_MAX;
        delegates[QUALITY_ID] = new SpinBoxDelegate(intLimitsMap);
        delegates[LEN_ID] = new SpinBoxDelegate(intLimitsMap);
        delegates[BOTH_ID] = new ComboBoxWithBoolsDelegate();
    }

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attributes);
    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new SequenceQualityTrimPrompter());

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new SequenceQualityTrimWorkerFactory());
}

Worker *SequenceQualityTrimWorkerFactory::createWorker(Actor *actor) {
    return new SequenceQualityTrimWorker(actor);
}

}   // namespace LocalWorkflow
}   // namespace U2
