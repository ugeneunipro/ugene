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

#include "MAFFTSupport.h"
#include "MAFFTWorker.h"
#include "TaskLocalStorage.h"

namespace U2 {
namespace LocalWorkflow {

/****************************
 * MAFFTWorkerFactory
 ****************************/
const QString MAFFTWorkerFactory::ACTOR_ID("mafft");
const QString GAP_OPEN_PENALTY("gap-open-penalty");
const QString GAP_EXT_PENALTY("gap-ext-penalty");
const QString NUM_ITER("iterations-max-num");
const QString EXT_TOOL_PATH("path");
const QString TMP_DIR_PATH("temp-dir");

void MAFFTWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), MAFFTWorker::tr("Input MSA"),
        MAFFTWorker::tr("Multiple sequence alignment to be processed."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), MAFFTWorker::tr("Multiple sequence alignment"), MAFFTWorker::tr("Result of alignment."));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("mafft.in.msa", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("mafft.out.msa", outM)), false /*input*/, true /*multi*/);

    Descriptor gop(GAP_OPEN_PENALTY, MAFFTWorker::tr("Gap Open Penalty"),
                   MAFFTWorker::tr("Gap Open Penalty."));
    Descriptor gep(GAP_EXT_PENALTY, MAFFTWorker::tr("Offset"),
                   MAFFTWorker::tr("Works like gap extension penalty."));
    Descriptor tgp(NUM_ITER, MAFFTWorker::tr("Max Iteration"),
                   MAFFTWorker::tr("Maximum number of iterative refinement."));
    Descriptor etp(EXT_TOOL_PATH, MAFFTWorker::tr("Tool Path"),
                   MAFFTWorker::tr("External tool path."));
    Descriptor tdp(TMP_DIR_PATH, MAFFTWorker::tr("Temporary folder"),
                   MAFFTWorker::tr("Folder for temporary files."));

    a << new Attribute(gop, BaseTypes::NUM_TYPE(), false, QVariant(1.53));
    a << new Attribute(gep, BaseTypes::NUM_TYPE(), false, QVariant(0.00));
    a << new Attribute(tgp, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(etp, BaseTypes::STRING_TYPE(), true, QVariant("default"));
    a << new Attribute(tdp, BaseTypes::STRING_TYPE(), true, QVariant("default"));

    Descriptor desc(ACTOR_ID, MAFFTWorker::tr("Align with MAFFT"),
        MAFFTWorker::tr("MAFFT is a multiple sequence alignment program for unix-like operating systems. "
                        "<p><dfn>It offers a range of multiple alignment methods, "
                        "L-INS-i (accurate; for alignment of &lt;&#126;200 sequences), "
                        "FFT-NS-2 (fast; for alignment of &lt;&#126;10,000 sequences), etc. </dfn></p>"));
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
        QVariantMap m; m["minimum"] = int(0); m["maximum"] = int(1000);
        delegates[NUM_ITER] = new SpinBoxDelegate(m);
    }
    delegates[EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new MAFFTPrompter());
    proto->setIconPath(":external_tool_support/images/cmdline.png");
    proto->addExternalTool(MAFFTSupport::ET_MAFFT_ID, EXT_TOOL_PATH);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new MAFFTWorkerFactory());
}

/****************************
* MAFFTPrompter
****************************/
MAFFTPrompter::MAFFTPrompter(Actor* p) : PrompterBase<MAFFTPrompter>(p) {
}
QString MAFFTPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";

    QString doc = tr("Aligns each MSA supplied <u>%1</u> with <u>\"MAFFT\"</u>.")
        .arg(producerName);

    return doc;
}
/****************************
* MAFFTWorker
****************************/
MAFFTWorker::MAFFTWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void MAFFTWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* MAFFTWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.gapOpenPenalty=actor->getParameter(GAP_OPEN_PENALTY)->getAttributeValue<float>(context);
        cfg.gapExtenstionPenalty=actor->getParameter(GAP_EXT_PENALTY)->getAttributeValue<float>(context);
        cfg.maxNumberIterRefinement=actor->getParameter(NUM_ITER)->getAttributeValue<int>(context);
        QString path=actor->getParameter(EXT_TOOL_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            AppContext::getExternalToolRegistry()->getById(MAFFTSupport::ET_MAFFT_ID)->setPath(path);
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
            algoLog.error(tr("An empty MSA '%1' has been supplied to MAFFT.").arg(msa->getName()));
            return NULL;
        }
        MAFFTSupportTask* supportTask = new MAFFTSupportTask(msa, GObjectReference(), cfg);
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

void MAFFTWorker::sl_taskFinished() {
    NoFailTaskWrapper *wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    MAFFTSupportTask* t = qobject_cast<MAFFTSupportTask*>(wrapper->originalTask());
    if(t->isCanceled()){
        return;
    }
    if (t->hasError()) {
        coreLog.error(t->getError());
        return;
    }

    SAFE_POINT(NULL != output, "NULL output!", );
    send(t->resultMA);
    algoLog.info(tr("Aligned %1 with MAFFT").arg(t->resultMA->getName()));
}

void MAFFTWorker::cleanup() {
}

void MAFFTWorker::send(const MultipleSequenceAlignment &msa) {
    SAFE_POINT(NULL != output, "NULL output!", );
    SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(msa);
    QVariantMap m;
    m[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(msaId);
    output->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), m));
}

} //namespace LocalWorkflow
} //namespace U2
