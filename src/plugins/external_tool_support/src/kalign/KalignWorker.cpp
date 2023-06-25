/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "KalignWorker.h"

#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/NoFailTaskWrapper.h>
#include <U2Lang/WorkflowEnv.h>

#include "KalignSupportRunDialog.h"
#include "KalignSupportTask.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {

/****************************
 * KalignWorkerFactory
 ****************************/
const QString Kalign3WorkerFactory::ACTOR_ID("kalign");
const QString GAP_OPEN_PENALTY("gap-open-penalty");
const QString GAP_EXT_PENALTY("gap-ext-penalty");
const QString TERM_GAP_PENALTY("terminal-gap-penalty");
const QString THREAD_COUNT("nthreads");

void Kalign3WorkerFactory::init() {
    QList<PortDescriptor*> portDescriptors;
    QList<Attribute*> attributes;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), Kalign3Worker::tr("Input MSA"), Kalign3Worker::tr("Input MSA to process."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), Kalign3Worker::tr("Kalign result MSA"), Kalign3Worker::tr("The result of Kalign alignment."));

    QMap<Descriptor, DataTypePtr> inputPortMap;
    inputPortMap[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    portDescriptors << new PortDescriptor(ind, DataTypePtr(new MapDataType("kalign.in.msa", inputPortMap)), true /*input*/);

    QMap<Descriptor, DataTypePtr> outputPortMap;
    outputPortMap[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    portDescriptors << new PortDescriptor(oud, DataTypePtr(new MapDataType("kalign.out.msa", outputPortMap)), false /*input*/, true /*multi*/);

    Descriptor gop(GAP_OPEN_PENALTY, Kalign3Worker::tr("Gap open penalty"), Kalign3Worker::tr("The penalty for opening/closing a gap. Kalign parameter '--gpo'."));
    Descriptor gep(GAP_EXT_PENALTY, Kalign3Worker::tr("Gap extension penalty"), Kalign3Worker::tr("The penalty for extending a gap. Kalign parameter '--gpe'."));
    Descriptor tgp(TERM_GAP_PENALTY, Kalign3Worker::tr("Terminal gap penalty"), Kalign3Worker::tr("The penalty to extend gaps from the N/C terminal of protein or 5'/3' terminal of nucleotide sequences. Kalign parameter '--tgpe'."));
    Descriptor threadCount(THREAD_COUNT, Kalign3Worker::tr("Number of threads"), Kalign3Worker::tr("Number of threads to use by the kalign algorithm. Kalign parameter '--nthreads'."));

    attributes << new Attribute(gop, BaseTypes::NUM_TYPE(), false, QVariant(0.0));
    attributes << new Attribute(gep, BaseTypes::NUM_TYPE(), false, QVariant(0.0));
    attributes << new Attribute(tgp, BaseTypes::NUM_TYPE(), false, QVariant(0.0));
    attributes << new Attribute(threadCount, BaseTypes::NUM_TYPE(), false, QVariant(4));

    Descriptor actorDescription(ACTOR_ID,
                                Kalign3Worker::tr("Align with Kalign"),
                                Kalign3Worker::tr("Aligns multiple sequence alignments (MSAs) with Kalign."
                                                  "<p>Kalign is a fast and accurate multiple sequence alignment tool. The original version of the tool can be found on <a href=\"https://github.com/TimoLassmann/kalign\">https://github.com/TimoLassmann/kalign</a>."));

    auto actorPrototype = new IntegralBusActorPrototype(actorDescription, portDescriptors, attributes);

    QSharedPointer<DoubleFormatter> defaultValueFormatter(new DefaultDoubleFormatter(0, Kalign3Worker::tr("auto"), ""));

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m;
        m["minimum"] = double(0);
        m["maximum"] = double(1000);
        m["decimals"] = 2;
        delegates[GAP_OPEN_PENALTY] = new DoubleSpinBoxDelegate(m, nullptr, defaultValueFormatter);
    }
    {
        QVariantMap m;
        m["minimum"] = double(0);
        m["maximum"] = double(1000);
        m["decimals"] = 2;
        delegates[GAP_EXT_PENALTY] = new DoubleSpinBoxDelegate(m, nullptr, defaultValueFormatter);
    }
    {
        QVariantMap m;
        m["minimum"] = double(0);
        m["maximum"] = double(1000);
        m["decimals"] = 2;
        delegates[TERM_GAP_PENALTY] = new DoubleSpinBoxDelegate(m, nullptr, defaultValueFormatter);
    }
    {
        QVariantMap m;
        m["minimum"] = 1;
        m["maximum"] = 128;
        delegates[THREAD_COUNT] = new SpinBoxDelegate(m);
    }

    actorPrototype->setEditor(new DelegateEditor(delegates));
    actorPrototype->setPrompter(new Kalign3Prompter());
    actorPrototype->setIconPath(":kalign/images/kalign_16.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), actorPrototype);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new Kalign3WorkerFactory());
}

/****************************
 * KalignPrompter
 ****************************/
QString Kalign3Prompter::composeRichDoc() {
    auto input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";
    return tr("Aligns each input MSA <u>%1</u> with \"<u>Kalign</u>\".").arg(producerName);
}

/****************************
 * KalignWorker
 ****************************/
Kalign3Worker::Kalign3Worker(Actor* a)
    : BaseWorker(a) {
}

void Kalign3Worker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* Kalign3Worker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return nullptr;
        }
        cfg.gapOpenPenalty = actor->getParameter(GAP_OPEN_PENALTY)->getAttributeValue<double>(context);
        if (cfg.gapOpenPenalty <= 0) {
            cfg.gapOpenPenalty = Kalign3Settings::VALUE_IS_NOT_SET;
        }
        cfg.gapExtensionPenalty = actor->getParameter(GAP_EXT_PENALTY)->getAttributeValue<double>(context);
        if (cfg.gapExtensionPenalty <= 0) {
            cfg.gapExtensionPenalty = Kalign3Settings::VALUE_IS_NOT_SET;
        }
        cfg.terminalGapExtensionPenalty = actor->getParameter(TERM_GAP_PENALTY)->getAttributeValue<double>(context);
        if (cfg.terminalGapExtensionPenalty <= 0) {
            cfg.terminalGapExtensionPenalty = Kalign3Settings::VALUE_IS_NOT_SET;
        }
        cfg.nThreads = actor->getParameter(TERM_GAP_PENALTY)->getAttributeValue<int>(context);
        if (cfg.nThreads == 0) {
            cfg.nThreads = 4;
        }

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        if (msa->isEmpty()) {
            algoLog.error(tr("An empty MSA '%1' has been supplied to Kalign.").arg(msa->getName()));
            return nullptr;
        }
        auto t = new NoFailTaskWrapper(new Kalign3SupportTask(msa, GObjectReference(), cfg));
        connect(t, &Task::si_stateChanged, this, &Kalign3Worker::sl_taskFinished);
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return nullptr;
}

void Kalign3Worker::sl_taskFinished() {
    auto wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    auto t = qobject_cast<Kalign3SupportTask*>(wrapper->originalTask());
    if (t->hasError()) {
        coreLog.error(t->getError());
        return;
    }
    CHECK(!t->isCanceled(), );
    SAFE_POINT(output != nullptr, "NULL output!", );
    send(t->resultMA);
    algoLog.info(tr("Aligned %1 with Kalign").arg(t->resultMA->getName()));
}

void Kalign3Worker::cleanup() {
}

void Kalign3Worker::send(const MultipleSequenceAlignment& msa) {
    SAFE_POINT(output != nullptr, "NULL output!", );
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(msa);
    QVariantMap m;
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(msaId);
    output->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), m));
}

Kalign3WorkerFactory::Kalign3WorkerFactory()
    : DomainFactory(ACTOR_ID) {
}

Worker* Kalign3WorkerFactory::createWorker(Actor* a) {
    return new Kalign3Worker(a);
}

}  // namespace LocalWorkflow
}  // namespace U2
