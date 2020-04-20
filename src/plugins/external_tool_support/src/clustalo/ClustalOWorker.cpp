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

#include "ClustalOWorker.h"
#include "TaskLocalStorage.h"
#include "ClustalOSupport.h"

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/NoFailTaskWrapper.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/Log.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/FailTask.h>

namespace U2 {
namespace LocalWorkflow {

/****************************
 * ClustalOWorkerFactory
 ****************************/
const QString ClustalOWorkerFactory::ACTOR_ID("ClustalO");
const QString NUM_ITERATIONS("num-iterations");
const QString MAX_GT_ITERATIONS("max-guidetree-iterations");
const QString MAX_HMM_ITERATIONS("max-hmm-iterations");
const QString SET_AUTO("set-auto");
const QString NUM_PROCESSORS("num-processors");
const QString EXT_TOOL_PATH("path");
const QString TMP_DIR_PATH("temp-dir");

void ClustalOWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_MSA_PORT_ID(), ClustalOWorker::tr("Input MSA"), ClustalOWorker::tr("Input MSA to process."));
    Descriptor oud(BasePorts::OUT_MSA_PORT_ID(), ClustalOWorker::tr("ClustalO result MSA"),
        ClustalOWorker::tr("The result of the ClustalO alignment."));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("clustal.in.msa", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("clustal.out.msa", outM)), false /*input*/, true /*multi*/);

    Descriptor ni(NUM_ITERATIONS, ClustalOWorker::tr("Number of iterations"),
                    ClustalOWorker::tr("Number of (combined guide-tree/HMM) iterations."));
    Descriptor ngti(MAX_GT_ITERATIONS, ClustalOWorker::tr("Number of guidetree iterations"),
                    ClustalOWorker::tr("Maximum number guidetree iterations."));
    Descriptor nhmmi(MAX_HMM_ITERATIONS, ClustalOWorker::tr("Number of HMM iterations"),
                    ClustalOWorker::tr("Maximum number of HMM iterations."));
    Descriptor sa(SET_AUTO, ClustalOWorker::tr("Set auto options"),
                    ClustalOWorker::tr("Set options automatically (might overwrite some of your options)."));

    Descriptor etp(EXT_TOOL_PATH, ClustalOWorker::tr("Tool path"),
                    ClustalOWorker::tr("Path to the ClustalO tool."
                        "<p>The default path can be set in the UGENE application settings."));

    Descriptor tdp(TMP_DIR_PATH, ClustalOWorker::tr("Temporary folder"),
                    ClustalOWorker::tr("Folder to store temporary files."));

    a << new Attribute(ni, BaseTypes::NUM_TYPE(), false, QVariant(1));
    a << new Attribute(ngti, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(nhmmi, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(sa, BaseTypes::BOOL_TYPE(), false, QVariant(false));

    a << new Attribute(etp, BaseTypes::STRING_TYPE(), true, QVariant("Default"));
    a << new Attribute(tdp, BaseTypes::STRING_TYPE(), true, QVariant("Default"));

    Descriptor desc(ACTOR_ID, ClustalOWorker::tr("Align with ClustalO"),
        ClustalOWorker::tr("Aligns multiple sequence alignments (MSAs) supplied with ClustalO."
        "<p>ClustalO is a general purpose multiple sequence alignment program for proteins."
        "Visit <a href=\"http://www.clustal.org/omega\">http://www.clustal.org/omega</a> to learn more about it."));

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);

    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap m; m["minimum"] = int(1); m["maximum"] = int(1000);
        delegates[NUM_ITERATIONS] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = int(0); m["maximum"] = int(1000);
        delegates[MAX_GT_ITERATIONS] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = int(0); m["maximum"] = int(1000);
        delegates[MAX_HMM_ITERATIONS] = new SpinBoxDelegate(m);
    }

    delegates[EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new ClustalOPrompter());
    proto->setIconPath(":external_tool_support/images/clustalo.png");
    proto->addExternalTool(ClustalOSupport::ET_CLUSTALO_ID, EXT_TOOL_PATH);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ClustalOWorkerFactory());
}

/****************************
* ClustalOPrompter
****************************/
ClustalOPrompter::ClustalOPrompter(Actor* p) : PrompterBase<ClustalOPrompter>(p) {
}
QString ClustalOPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor* producer = input->getProducer(BasePorts::IN_MSA_PORT_ID());
    QString producerName = producer ? tr(" from %1").arg(producer->getLabel()) : "";
    QString doc = tr("Aligns each MSA supplied <u>%1</u> with \"<u>ClustalO</u>\".")
        .arg(producerName);

    return doc;
}
/****************************
* ClustalOWorker
****************************/
ClustalOWorker::ClustalOWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void ClustalOWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(BasePorts::OUT_MSA_PORT_ID());
}

Task* ClustalOWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.numIterations=actor->getParameter(NUM_ITERATIONS)->getAttributeValue<int>(context);
        cfg.maxGuidetreeIterations=actor->getParameter(MAX_GT_ITERATIONS)->getAttributeValue<int>(context);
        cfg.maxHMMIterations=actor->getParameter(MAX_HMM_ITERATIONS)->getAttributeValue<int>(context);
        cfg.setAutoOptions=actor->getParameter(SET_AUTO)->getAttributeValue<bool>(context);
        cfg.numberOfProcessors=AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();

        QString path=actor->getParameter(EXT_TOOL_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID)->setPath(path);
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
            algoLog.error(tr("An empty MSA '%1' has been supplied to ClustalO.").arg(msa->getName()));
            return NULL;
        }
        ClustalOSupportTask* supportTask = new ClustalOSupportTask(msa, GObjectReference(), cfg);
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

void ClustalOWorker::sl_taskFinished() {
    NoFailTaskWrapper *wrapper = qobject_cast<NoFailTaskWrapper*>(sender());
    CHECK(wrapper->isFinished(), );
    ClustalOSupportTask* t = qobject_cast<ClustalOSupportTask*>(wrapper->originalTask());
    if(t->isCanceled()){
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
    algoLog.info(tr("Aligned %1 with ClustalO").arg(t->resultMA->getName()));
}

void ClustalOWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
