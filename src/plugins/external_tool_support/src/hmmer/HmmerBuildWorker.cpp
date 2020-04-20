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
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "HmmerBuildWorker.h"
#include "HmmerBuildFromMsaTask.h"
#include "HmmerSupport.h"

namespace U2 {
namespace LocalWorkflow {

/******************************
 * HmmerBuildWorkerFactory
 ******************************/
const QString HmmerBuildWorkerFactory::ACTOR("hmm3-build");
static const QString OUT_HMM_URL_PORT_ID("out-hmm3");

static const QString SEED_ATTR("seed");

static const QString HMM3_PROFILE_DEFAULT_NAME("hmm3_profile");

void HmmerBuildWorkerFactory::init() {
    QList<PortDescriptor *> p;
    QList<Attribute *> a;
    {
        Descriptor id(BasePorts::IN_MSA_PORT_ID(), HmmerBuildWorker::tr("Input MSA"),
            HmmerBuildWorker::tr("Input multiple sequence alignment for building statistical model."));
        Descriptor od(OUT_HMM_URL_PORT_ID, HmmerBuildWorker::tr("HMM3 profile"), HmmerBuildWorker::tr("Produced HMM3 profile URL"));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("hmm3.build.in", inM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("hmm3.build", outM)), false /*input*/, true /*multi*/);
    }

    Descriptor sed(SEED_ATTR, HmmerBuildWorker::tr("Random seed"), HmmerBuildWorker::tr("Random generator seed. 0 - means that one-time arbitrary seed will be used."));

    a << new Attribute(sed, BaseTypes::NUM_TYPE(), false, QVariant(42));

    Descriptor desc(HmmerBuildWorkerFactory::ACTOR, HmmerBuildWorker::tr("HMM3 Build"), HmmerBuildWorker::tr("Builds a HMM3 profile from a multiple sequence alignment."
        "<p>The HMM3 profile is a statistical model which captures position-specific information"
        " about how conserved each column of the alignment is, and which residues are likely."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate *> delegates;

    {
        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = INT_MAX;
        delegates[SEED_ATTR] = new SpinBoxDelegate(m);
    }
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":/external_tool_support/images/hmmer.png");
    proto->setPrompter(new HmmerBuildPrompter());
    proto->addExternalTool(HmmerSupport::BUILD_TOOL_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(Descriptor("hmmer3", HmmerBuildWorker::tr("HMMER3 Tools"), ""), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new HmmerBuildWorkerFactory());
}

void HmmerBuildWorkerFactory::cleanup() {
    delete WorkflowEnv::getProtoRegistry()->unregisterProto(ACTOR);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(ACTOR);
}

HmmerBuildWorkerFactory::HmmerBuildWorkerFactory()
    : DomainFactory(ACTOR)
{

}

Worker * HmmerBuildWorkerFactory::createWorker(Actor *a) {
    return new HmmerBuildWorker(a);
}

/******************************
 * HmmerBuildPrompter
 ******************************/
HmmerBuildPrompter::HmmerBuildPrompter(Actor *p)
    : PrompterBase<HmmerBuildPrompter>(p)
{

}

QString HmmerBuildPrompter::composeRichDoc() {
    IntegralBusPort *input = qobject_cast<IntegralBusPort *>(target->getPort(BasePorts::IN_MSA_PORT_ID()));
    Actor *msaProducer = input->getProducer(BasePorts::IN_MSA_PORT_ID());

    QString msaName = (msaProducer ? tr("For each MSA from <u>%1</u>,").arg(msaProducer->getLabel()) : "");

    QString doc = tr("%1 builds a HMMER profile.").arg(msaName);

    return doc;
}

/******************************
* HmmerBuildWorker
******************************/
HmmerBuildWorker::HmmerBuildWorker(Actor *a)
    : BaseWorker(a),
      input(NULL),
      output(NULL)
{
}

void HmmerBuildWorker::init() {
    input = ports.value(BasePorts::IN_MSA_PORT_ID());
    output = ports.value(OUT_HMM_URL_PORT_ID);
    cfg = HmmerBuildSettings();
}

bool HmmerBuildWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    return input->hasMessage() || input->isEnded();
}

Task * HmmerBuildWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.seed = actor->getParameter(SEED_ATTR)->getAttributeValue<int>(context);

        QVariantMap qm = inputMessage.getData().toMap();
        SharedDbiDataHandler msaId = qm.value(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<MultipleSequenceAlignmentObject> msaObj(StorageUtils::getMsaObject(context->getDataStorage(), msaId));
        SAFE_POINT(!msaObj.isNull(), "NULL MSA Object!", NULL);
        const MultipleSequenceAlignment msa = msaObj->getMultipleAlignment();

        cfg.profileUrl = monitor()->outputDir() + "hmmer_build/" + QFileInfo(context->getMetadataStorage().get(inputMessage.getMetadataId()).getFileUrl()).baseName() + ".hmm";
        HmmerBuildFromMsaTask *task = new HmmerBuildFromMsaTask(cfg, msa);
        task->addListeners(createLogListeners());
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
        return task;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void HmmerBuildWorker::sl_taskFinished(Task* task) {
    HmmerBuildFromMsaTask *buildTask = qobject_cast<HmmerBuildFromMsaTask *>(task);
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (task->isCanceled()) {
        return;
    }
    const QString hmmUrl = buildTask->getHmmUrl();
    monitor()->addOutputFile(hmmUrl, actor->getId(), true);
    output->put(Message(BaseTypes::STRING_TYPE(), hmmUrl));
    algoLog.info(tr("Built HMMER profile"));
}

void HmmerBuildWorker::cleanup() {

}

}   // namespace LocalWorkflow
}   // namespace U2
