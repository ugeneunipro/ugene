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

#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/NoFailTaskWrapper.h>
#include <U2Lang/WorkflowEnv.h>

#include "KalignConstants.h"
#include "KalignDialogController.h"
#include "KalignWorker.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {

/****************************
 * KalignWorkerFactory
 ****************************/
const QString KalignWorkerFactory::ACTOR_ID("kalign");
const QString GAP_OPEN_PENALTY("gap-open-penalty");
const QString GAP_EXT_PENALTY("gap-ext-penalty");
const QString TERM_GAP_PENALTY("terminal-gap-penalty");
const QString BONUS_SCORE("bonus-score");

void KalignWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), KalignWorker::tr("Input MSA"), KalignWorker::tr("Input MSA to process."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), KalignWorker::tr("Kalign result MSA"), 
        KalignWorker::tr("The result of the Kalign alignment."));
    
    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("kalign.in.msa", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("kalign.out.msa", outM)), false /*input*/, true /*multi*/);
    
    Descriptor gop(GAP_OPEN_PENALTY, KalignWorker::tr("Gap open penalty"),
                   KalignWorker::tr("The penalty for opening/closing a gap. Half the value will be subtracted from the alignment score when opening, and half when closing a gap."));
    Descriptor gep(GAP_EXT_PENALTY, KalignWorker::tr("Gap extension penalty"),
                   KalignWorker::tr("The penalty for extending a gap."));
    Descriptor tgp(TERM_GAP_PENALTY, KalignWorker::tr("Terminal gap penalty"),
                   KalignWorker::tr("The penalty to extend gaps from the N/C terminal of protein or 5'/3' terminal of nucleotide sequences."));
	Descriptor secret(BONUS_SCORE, KalignWorker::tr("Bonus score"),
				   KalignWorker::tr("A bonus score that is added to each pair of aligned residues."));

    a << new Attribute(gop, BaseTypes::NUM_TYPE(), false, QVariant(54.90));
    a << new Attribute(gep, BaseTypes::NUM_TYPE(), false, QVariant(8.52));
    a << new Attribute(tgp, BaseTypes::NUM_TYPE(), false, QVariant(4.42));
    a << new Attribute(secret, BaseTypes::NUM_TYPE(), false, QVariant(0.02));

    Descriptor desc(ACTOR_ID, KalignWorker::tr("Align with Kalign"),
        KalignWorker::tr("Aligns multiple sequence alignments (MSAs) supplied with Kalign."
        "<p>Kalign is a fast and accurate multiple sequence alignment tool. The original version of the tool can be found on <a href=\"http://msa->sbc.su.se\">http://msa->sbc.su.se</a>."));

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m; m["minimum"] = double(.00); m["maximum"] = double(100.00); m["decimals"] = 2;
        delegates[GAP_OPEN_PENALTY] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = double(.00); m["maximum"] = double(10.00); m["decimals"] = 2;
        delegates[GAP_EXT_PENALTY] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = double(.00); m["maximum"] = double(99.99); m["decimals"] = 2;
        delegates[TERM_GAP_PENALTY] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = double(.00); m["maximum"] = double(99.99); m["decimals"] = 2;
        delegates[BONUS_SCORE] = new DoubleSpinBoxDelegate(m);
    }

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new KalignPrompter());
    proto->setIconPath(":kalign/images/kalign_16.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new KalignWorkerFactory());
}

/****************************
* KalignPrompter
****************************/
QString KalignPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";

    QString doc = tr("Aligns each MSA supplied <u>%1</u> with \"<u>Kalign</u>\".")
        .arg(producerName);

    return doc;
}

/****************************
* KalignWorker
****************************/
KalignWorker::KalignWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void KalignWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* KalignWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.gapOpenPenalty=actor->getParameter(GAP_OPEN_PENALTY)->getAttributeValue<float>(context);
        cfg.gapExtenstionPenalty=actor->getParameter(GAP_EXT_PENALTY)->getAttributeValue<float>(context);
        cfg.termGapPenalty=actor->getParameter(TERM_GAP_PENALTY)->getAttributeValue<float>(context);
	    cfg.secret=actor->getParameter(BONUS_SCORE)->getAttributeValue<float>(context);

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        if (msa->isEmpty()) {
            algoLog.error(tr("An empty MSA '%1' has been supplied to Kalign.").arg(msa->getName()));
            return NULL;
        }
        Task *t = new NoFailTaskWrapper(new KalignTask(msa, cfg));
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void KalignWorker::sl_taskFinished() {
    NoFailTaskWrapper *wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    KalignTask *t = qobject_cast<KalignTask*>(wrapper->originalTask());
    if (t->hasError()) {
        coreLog.error(t->getError());
        return;
    }

    if (t->isCanceled()) {
        return;
    }

    SAFE_POINT(NULL != output, "NULL output!", );
    send(t->resultMA);
    algoLog.info(tr("Aligned %1 with Kalign").arg(t->resultMA->getName()));
}

void KalignWorker::cleanup() {
}

void KalignWorker::send(const MultipleSequenceAlignment &msa) {
    SAFE_POINT(NULL != output, "NULL output!", );
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(msa);
    QVariantMap m;
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(msaId);
    output->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), m));
}

} //namespace LocalWorkflow
} //namespace U2
