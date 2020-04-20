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
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "CoreLib.h"
#include "ReverseComplementWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString RCWorkerFactory::ACTOR_ID = QString("reverse-complement");
const QString OP_TYPE("op-type");

enum OpType {
    Reverse,
    Complement,
    ReverseComplement
};

void RCWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> attrs;
    Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), RCWorker::tr("Input sequence"),
        RCWorker::tr("The sequence to be complemented"));
    Descriptor outd(BasePorts::OUT_SEQ_PORT_ID(), RCWorker::tr("Output sequence"),
        RCWorker::tr("Reverse-complement sequence"));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("rc.input.sequence", inM)), true);
    QMap<Descriptor, DataTypePtr> outM;
    //outM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(outd, DataTypePtr(new MapDataType("rc.outpur.sequence", inM)), false, true);

    Descriptor opType(OP_TYPE,RCWorker::tr("Operation type"),
        RCWorker::tr("Select what to do with sequence."));
    attrs << new Attribute(opType, BaseTypes::STRING_TYPE(),true,"reverse-complement");

    Descriptor desc(ACTOR_ID, RCWorker::tr("Reverse Complement"),
        RCWorker::tr("Converts input sequence into its reverse, complement or reverse-complement counterpart")
        );
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, attrs);

    QMap<QString, PropertyDelegate*> delegates;
    QVariantMap m;
    m["Reverse Complement"] = "reverse-complement";
    m["Reverse"] = "nocompl";
    m["Complement"] = "norev";
    delegates[OP_TYPE] = new ComboBoxDelegate(m);

    proto->setPrompter(new RCWorkerPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_CONVERTERS(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new RCWorkerFactory());
}

QString RCWorkerPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);
    QString type = getRequiredParam(OP_TYPE);
    QString op = type == "norev" ? "complement" : type == "nocompl" ? "reverse" : "reverse-complement";
    op = getHyperlink(OP_TYPE, op);

    QString res = tr("Converts each input sequence %1 into its %2 counterpart.").arg(producerName).arg(op);
    return res;
}

void RCWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_SEQ_PORT_ID());
}

Task* RCWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = qm.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return new FailTask(tr("Null sequence object supplied to FindWorker"));
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));
        if(seq.isNull()) {
            return new FailTask(tr("Null sequence supplied to FindWorker: %1").arg(seq.getName()));
        }

        QString type = actor->getParameter(OP_TYPE)->getAttributeValue<QString>(context);

        DNATranslation *complTT;
        if(!seq.alphabet->isNucleic()) {
            coreLog.info(tr("Can't complement amino sequence"));
            if (input->isEnded()) {
                output->setEnded();
            }
            return NULL;
        }
        if(type == "reverse-complement") {
            complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(seq.alphabet);
            if(complTT == NULL) {
                coreLog.info(tr("Can't find complement translation"));
                if (input->isEnded()) {
                    output->setEnded();
                }
                return NULL;
            }
            complTT->translate(seq.seq.data(),seq.seq.size(), seq.seq.data(), seq.seq.size());
            TextUtils::reverse(seq.seq.data(), seq.seq.size());
        } else if(type == "norev") {
            complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(seq.alphabet);
            if(complTT == NULL) {
                coreLog.info(tr("Can't find complement translation"));
                if (input->isEnded()) {
                    output->setEnded();
                }
                return NULL;
            }
            complTT->translate(seq.seq.data(),seq.seq.size(), seq.seq.data(), seq.seq.size());
        } else {
            TextUtils::reverse(seq.seq.data(), seq.seq.size());
        }

        if (output) {
            const SharedDbiDataHandler seqId = context->getDataStorage()->putSequence(seq);
            const QVariant v = qVariantFromValue<SharedDbiDataHandler>(seqId);
            output->put(Message(BaseTypes::DNA_SEQUENCE_TYPE(), v));
            if (input->isEnded()) {
                output->setEnded();
            }
        }
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

}
}
