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
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

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

#include "TCoffeeSupport.h"
#include "TCoffeeWorker.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {

/****************************
 * TCoffeeWorkerFactory
 ****************************/
const QString TCoffeeWorkerFactory::ACTOR_ID("tcoffee");
const QString GAP_OPEN_PENALTY("gap-open-penalty");
const QString GAP_EXT_PENALTY("gap-ext-penalty");
const QString NUM_ITER("iterations-max-num");
const QString EXT_TOOL_PATH("path");
const QString TMP_DIR_PATH("temp-dir");

void TCoffeeWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), TCoffeeWorker::tr("Input MSA"), TCoffeeWorker::tr("Multiple sequence alignment to be processed."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), TCoffeeWorker::tr("Multiple sequence alignment"), TCoffeeWorker::tr("Result of alignment."));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("tcoffee.in.msa", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("tcoffee.out.msa", outM)), false /*input*/, true /*multi*/);

    Descriptor gop(GAP_OPEN_PENALTY, TCoffeeWorker::tr("Gap Open Penalty"),
                   TCoffeeWorker::tr("Gap Open Penalty. Must be negative, best matches get a score of 1000."));
    Descriptor gep(GAP_EXT_PENALTY, TCoffeeWorker::tr("Gap Extension Penalty"),
                   TCoffeeWorker::tr("Gap Extension Penalty. Positive values give rewards to gaps and prevent the alignment of unrelated segments."));
    Descriptor tgp(NUM_ITER, TCoffeeWorker::tr("Max Iteration"),
                   TCoffeeWorker::tr("Number of iteration on the progressive alignment.<br>"
                                     "0 - no iteration, -1 - Nseq iterations."));
    Descriptor etp(EXT_TOOL_PATH, TCoffeeWorker::tr("Tool Path"),
                   TCoffeeWorker::tr("External tool path."));
    Descriptor tdp(TMP_DIR_PATH, TCoffeeWorker::tr("Temporary folder"),
                   TCoffeeWorker::tr("folder for temporary file.s"));

    a << new Attribute(gop, BaseTypes::NUM_TYPE(), false, QVariant(-50));
    a << new Attribute(gep, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(tgp, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(etp, BaseTypes::STRING_TYPE(), true, QVariant("default"));
    a << new Attribute(tdp, BaseTypes::STRING_TYPE(), true, QVariant("default"));

    Descriptor desc(ACTOR_ID, TCoffeeWorker::tr("Align with T-Coffee"),
        TCoffeeWorker::tr("T-Coffee is a multiple sequence alignment package. "));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m; m["minimum"] = int(-10000); m["maximum"] = int(0); m["singleStep"] = int(50);
        delegates[GAP_OPEN_PENALTY] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = int(-5000); m["maximum"] = int(5000); m["singleStep"] = int(10);
        delegates[GAP_EXT_PENALTY] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = int(-1); m["maximum"] = int(100);
        delegates[NUM_ITER] = new SpinBoxDelegate(m);
    }
    delegates[EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new TCoffeePrompter());
    proto->setIconPath(":external_tool_support/images/tcoffee.png");
    proto->addExternalTool(TCoffeeSupport::ET_TCOFFEE_ID, EXT_TOOL_PATH);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new TCoffeeWorkerFactory());
}

/****************************
* TCoffeePrompter
****************************/
TCoffeePrompter::TCoffeePrompter(Actor* p) : PrompterBase<TCoffeePrompter>(p) {
}
QString TCoffeePrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";

    QString doc = tr("For each MSA<u>%1</u>, build the alignment using <u>\"T-Coffee\"</u> and send it to output.")
        .arg(producerName);

    return doc;
}
/****************************
* TCoffeeWorker
****************************/
TCoffeeWorker::TCoffeeWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void TCoffeeWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* TCoffeeWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.gapOpenPenalty=actor->getParameter(GAP_OPEN_PENALTY)->getAttributeValue<float>(context);
        cfg.gapExtenstionPenalty=actor->getParameter(GAP_EXT_PENALTY)->getAttributeValue<float>(context);
        cfg.numIterations=actor->getParameter(NUM_ITER)->getAttributeValue<int>(context);

        QString path=actor->getParameter(EXT_TOOL_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            AppContext::getExternalToolRegistry()->getById(TCoffeeSupport::ET_TCOFFEE_ID)->setPath(path);
        }
        path=actor->getParameter(TMP_DIR_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(path);
        }

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        if (msa->isEmpty()) {
            algoLog.error(tr("An empty MSA '%1' has been supplied to T-Coffee.").arg(msa->getName()));
            return NULL;
        }
        TCoffeeSupportTask* supportTask = new TCoffeeSupportTask(msa, GObjectReference(), cfg);
        supportTask->addListeners(createLogListeners());
        Task *t = new NoFailTaskWrapper(supportTask);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void TCoffeeWorker::sl_taskFinished() {
    NoFailTaskWrapper *wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    TCoffeeSupportTask* t = qobject_cast<TCoffeeSupportTask*>(wrapper->originalTask());
    if (t->isCanceled()) {
        return;
    }
    if (t->hasError()) {
        coreLog.error(t->getError());
        return;
    }

    SAFE_POINT(NULL != output, "NULL output!", );
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(t->resultMA);
    QVariantMap msgData;
    msgData[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(msaId);
    output->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), msgData));
    algoLog.info(tr("Aligned %1 with T-Coffee").arg(t->resultMA->getName()));
}

void TCoffeeWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
