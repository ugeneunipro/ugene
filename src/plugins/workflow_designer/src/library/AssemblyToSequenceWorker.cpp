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
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "AssemblyToSequenceWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString AssemblyToSequencesWorkerFactory::ACTOR_ID("assembly-to-sequences");

/************************************************************************/
/* Worker */
/************************************************************************/
AssemblyToSequencesWorker::AssemblyToSequencesWorker(Actor *p)
: BaseWorker(p), converter(NULL), inChannel(NULL), outChannel(NULL)
{

}

void AssemblyToSequencesWorker::init() {
    inChannel = ports.value(BasePorts::IN_ASSEMBLY_PORT_ID());
    outChannel = ports.value(BasePorts::OUT_SEQ_PORT_ID());
}

Task *AssemblyToSequencesWorker::tick() {
    SAFE_POINT(inChannel != NULL, "NULL input channel", NULL);
    SAFE_POINT(outChannel != NULL, "NULL output channel", NULL);

    if (inChannel->hasMessage()) {
        Message m = getMessageAndSetupScriptValues(inChannel);
        QVariantMap cxt = outChannel->getContext();
        converter = new AssemblyToSequencesTask(m, cxt, outChannel, context->getDataStorage());
        connect(converter, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    }

    return converter;
}

void AssemblyToSequencesWorker::sl_taskFinished() {
    SAFE_POINT(sender() == converter, "Wrong sender", );
    if (converter->isFinished()) {
        if (!inChannel->hasMessage() && inChannel->isEnded()) {
            outChannel->setEnded();
            this->setDone();
        }
    }
}

/************************************************************************/
/* Task */
/************************************************************************/
AssemblyToSequencesTask::AssemblyToSequencesTask(const Message &_message, const QVariantMap &_ctx,
                                                 IntegralBus *_channel, DbiDataStorage *_storage)
: Task(tr("Split Assembly into Sequences"), TaskFlag_None),
message(_message), ctx(_ctx), channel(_channel), storage(_storage)
{

}

void AssemblyToSequencesTask::run() {
    // 1. get data from the message
    CHECK(message.getType()->isMap(), );
    const QVariant &mData = message.getData();
    const QVariantMap data = mData.toMap();
    CHECK(data.contains(BaseSlots::ASSEMBLY_SLOT().getId()), );

    // 2. get assembly from the data
    SharedDbiDataHandler handler = data[BaseSlots::ASSEMBLY_SLOT().getId()].value<SharedDbiDataHandler>();
    QScopedPointer<AssemblyObject> assemblyObj(StorageUtils::getAssemblyObject(storage, handler));
    SAFE_POINT(NULL != assemblyObj.data(), "Error: assembly object error", );

    // 3. get assembly reads
    U2OpStatusImpl os;
    DbiConnection con(assemblyObj->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    U2AssemblyDbi *dbi = con.dbi->getAssemblyDbi();
    U2DataId assemblyId = assemblyObj->getEntityRef().entityId;
    qint64 length = dbi->getMaxEndPos(assemblyId, os) + 1;
    SAFE_POINT_OP(os, );
    U2Region wholeAssembly(0, length);
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter(dbi->getReads(assemblyId, wholeAssembly, os));
    SAFE_POINT_OP(os, );
    CHECK(NULL != iter.data(), );

    // 4. export reads to sequences and send messages
    const DNAAlphabet *alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    DataTypePtr busType = channel->getBusType();
    while (iter->hasNext()) {
        if (this->isCanceled()) {
            return;
        }
        U2AssemblyRead read = iter->next();
        DNASequence seq(read->name, read->readSequence, alphabet);
        seq.quality = read->quality;
        SharedDbiDataHandler seqHandler = storage->putSequence(seq);

        QVariantMap seqData;
        seqData[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(seqHandler);
        Message m(busType, seqData);
        // the context is needed! It keeps the corresponding data
        channel->setContext(ctx, message.getMetadataId());
        channel->put(m);
    }
}

void AssemblyToSequencesTask::cleanup() {

}

/************************************************************************/
/* Factory */
/************************************************************************/
void AssemblyToSequencesWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    {
        QMap<Descriptor, DataTypePtr> inTypeMap;
        inTypeMap[BaseSlots::ASSEMBLY_SLOT()] = BaseTypes::ASSEMBLY_TYPE();
        DataTypePtr inTypeSet(new MapDataType(BasePorts::IN_ASSEMBLY_PORT_ID(), inTypeMap));

        QMap<Descriptor, DataTypePtr> outTypeMap;
        outTypeMap[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        DataTypePtr outTypeSet(new MapDataType(BasePorts::OUT_SEQ_PORT_ID(), outTypeMap));

        portDescs << new PortDescriptor(BasePorts::IN_ASSEMBLY_PORT_ID(), inTypeSet, true);
        portDescs << new PortDescriptor(BasePorts::OUT_SEQ_PORT_ID(), outTypeSet, false);
    }

    QList<Attribute*> attrs;

    Descriptor protoDesc(AssemblyToSequencesWorkerFactory::ACTOR_ID,
        AssemblyToSequencesWorker::tr("Split Assembly into Sequences"),
        AssemblyToSequencesWorker::tr("Splits assembly into sequences(reads)."));

    ActorPrototype *proto = new IntegralBusActorPrototype(protoDesc, portDescs, attrs);
    proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
    proto->setPrompter(new AssemblyToSequencesPrompter());
    proto->setInfluenceOnPathFlag(true);

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CONVERTERS(), proto);
    WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID)->registerEntry(new AssemblyToSequencesWorkerFactory());
}

Worker *AssemblyToSequencesWorkerFactory::createWorker(Actor *a) {
    return new AssemblyToSequencesWorker(a);
}

/************************************************************************/
/* Prompter */
/************************************************************************/
QString AssemblyToSequencesPrompter::composeRichDoc() {
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";

    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_ASSEMBLY_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::ASSEMBLY_SLOT().getId());
    QString producerName = tr("<u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString res = tr("Split %1 assemblies into sequences(reads) and puts them to the output.").arg(producerName).arg(producerName);
    return res;
}

} // LocalWorkflow
} // U2
