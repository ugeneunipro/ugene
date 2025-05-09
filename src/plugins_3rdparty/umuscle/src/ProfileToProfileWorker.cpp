/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ProfileToProfileWorker.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/MsaObject.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "MuscleTask.h"

namespace U2 {
namespace LocalWorkflow {

const QString ProfileToProfileWorkerFactory::ACTOR_ID("align-profile-to-profile");

static const QString IN_PROFILES_PORT_ID("in-profiles");

static const QString MASTER_PROFILE_SLOT_ID("master-msa");
static const QString SECOND_PROFILE_SLOT_ID("second-msa");

/************************************************************************/
/* Worker */
/************************************************************************/
ProfileToProfileWorker::ProfileToProfileWorker(Actor* a)
    : BaseWorker(a), inPort(nullptr), outPort(nullptr) {
}

void ProfileToProfileWorker::init() {
    inPort = ports[IN_PROFILES_PORT_ID];
    outPort = ports[BasePorts::OUT_MSA_PORT_ID()];
}

Task* ProfileToProfileWorker::tick() {
    if (inPort->hasMessage()) {
        Message m = getMessageAndSetupScriptValues(inPort);

        QVariantMap qm = m.getData().toMap();
        SharedDbiDataHandler masterMsaId = qm.value(MASTER_PROFILE_SLOT_ID).value<SharedDbiDataHandler>();
        QScopedPointer<MsaObject> masterMsaObj(StorageUtils::getMsaObject(context->getDataStorage(), masterMsaId));
        SAFE_POINT(!masterMsaObj.isNull(), "NULL MSA Object!", nullptr);
        const Msa masterMsa = masterMsaObj->getAlignment();

        SharedDbiDataHandler secondMsaId = qm.value(SECOND_PROFILE_SLOT_ID).value<SharedDbiDataHandler>();
        QScopedPointer<MsaObject> secondMsaObj(StorageUtils::getMsaObject(context->getDataStorage(), secondMsaId));
        SAFE_POINT(!secondMsaObj.isNull(), "NULL MSA Object!", nullptr);
        const Msa secondMsa = secondMsaObj->getAlignment();

        Task* t = new ProfileToProfileTask(masterMsa, secondMsa);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (inPort->isEnded()) {
        setDone();
        outPort->setEnded();
    }
    return nullptr;
}

void ProfileToProfileWorker::cleanup() {
    foreach (MsaObject* obj, objects) {
        delete obj;
    }
    objects.clear();
}

void ProfileToProfileWorker::sl_taskFinished() {
    auto t = dynamic_cast<ProfileToProfileTask*>(sender());
    if (t->isCanceled()) {
        return;
    }
    if (!t->isFinished() || t->hasError()) {
        return;
    }

    if (outPort) {
        Msa resultAl = t->getResult();
        resultAl->setName("Aligned");
        SharedDbiDataHandler msaId = context->getDataStorage()->putAlignment(resultAl);
        QVariantMap msgData;
        msgData[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()] = QVariant::fromValue<SharedDbiDataHandler>(msaId);
        outPort->put(Message(BaseTypes::MULTIPLE_ALIGNMENT_TYPE(), msgData));
        algoLog.info(tr("Aligned profile to profile with MUSCLE").arg(t->getResult()->getName()));
    }
}

/************************************************************************/
/* Task */
/************************************************************************/
ProfileToProfileTask::ProfileToProfileTask(const Msa& masterMsa, const Msa& secondMsa)
    : Task(tr("Align profile to profile with MUSCLE"), TaskFlag_NoRun),
      masterMsa(masterMsa->getCopy()),
      secondMsa(secondMsa->getCopy()),
      seqIdx(0),
      subtaskCount(0) {
}

ProfileToProfileTask::~ProfileToProfileTask() = default;

void ProfileToProfileTask::prepare() {
    int maxThreads = 1;  // AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
    setMaxParallelSubtasks(maxThreads);

    foreach (const MsaRow& row, masterMsa->getRows()) {
        result->addRow(row->getRowDbInfo(), row->getSequence(), stateInfo);
        CHECK_OP(stateInfo, );
    }

    QList<Task*> tasks = createAlignTasks();
    foreach (Task* t, tasks) {
        addSubTask(t);
    }
}

QList<Task*> ProfileToProfileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> tasks;
    if (subTask->hasError()) {
        return tasks;
    }

    if (subTask->isCanceled()) {
        return tasks;
    }

    appendResult(subTask);
    tasks << createAlignTasks();

    return tasks;
}

const Msa& ProfileToProfileTask::getResult() {
    U2AlphabetUtils::assignAlphabet(result);
    return result;
}

void ProfileToProfileTask::appendResult(Task* task) {
    subtaskCount--;
    auto t = dynamic_cast<MuscleTask*>(task);
    SAFE_POINT(t != nullptr, "NULL Muscle task!", );

    const QVector<MsaRow>& newRows = t->resultMA->getRows();
    if (newRows.size() == masterMsa->getRows().size() + 1) {
        U2OpStatus2Log os;
        result->addRow(newRows.last()->getRowDbInfo(), newRows.last()->getSequence(), os);
    }
}

QList<Task*> ProfileToProfileTask::createAlignTasks() {
    QList<Task*> tasks;
    while (canCreateTask()) {
        U2OpStatus2Log os;
        MuscleTaskSettings cfg;
        cfg.op = MuscleTaskOp_ProfileToProfile;
        cfg.profile->addRow(secondMsa->getRow(seqIdx)->getRowDbInfo(), secondMsa->getRow(seqIdx)->getSequence(), os);
        cfg.profile->setAlphabet(secondMsa->getAlphabet());

        tasks << new MuscleTask(masterMsa, cfg);

        subtaskCount++;
        seqIdx++;
    }

    return tasks;
}

bool ProfileToProfileTask::canCreateTask() const {
    return (seqIdx < secondMsa->getRows().size()) && (subtaskCount < getMaxParallelSubtasks());
}

/************************************************************************/
/* Factory */
/************************************************************************/
void ProfileToProfileWorkerFactory::init() {
    QList<PortDescriptor*> portDescs;
    {
        Descriptor masterProfileD(MASTER_PROFILE_SLOT_ID,
                                  ProfileToProfileWorker::tr("Master profile"),
                                  ProfileToProfileWorker::tr("The main alignment which will be aligned on."));
        Descriptor secondProfileD(SECOND_PROFILE_SLOT_ID,
                                  ProfileToProfileWorker::tr("Second profile"),
                                  ProfileToProfileWorker::tr("Alignment which will be aligned to the master alignment."));

        QMap<Descriptor, DataTypePtr> inMap;
        inMap[masterProfileD] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        inMap[secondProfileD] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        portDescs << new PortDescriptor(IN_PROFILES_PORT_ID, DataTypePtr(new MapDataType("in.profiles", inMap)), true);

        QMap<Descriptor, DataTypePtr> outMap;
        outMap[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        portDescs << new PortDescriptor(BasePorts::OUT_MSA_PORT_ID(), DataTypePtr(new MapDataType("out.msa", outMap)), false, true);
    }

    Descriptor protoD(ACTOR_ID,
                      ProfileToProfileWorker::tr("Align Profile to Profile With MUSCLE"),
                      ProfileToProfileWorker::tr("Aligns second profile to master profile with MUSCLE aligner."));

    ActorPrototype* proto = new IntegralBusActorPrototype(protoD, portDescs, QList<Attribute*>());
    proto->setEditor(new DelegateEditor(QMap<QString, PropertyDelegate*>()));
    proto->setPrompter(new ProfileToProfilePrompter());
    proto->setIconPath(":umuscle/images/muscle_16.png");

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ProfileToProfileWorkerFactory());
}

Worker* ProfileToProfileWorkerFactory::createWorker(Actor* a) {
    return new ProfileToProfileWorker(a);
}

/************************************************************************/
/* Prompter */
/************************************************************************/
QString ProfileToProfilePrompter::composeRichDoc() {
    return "Aligns second profile to master profile with MUSCLE aligner.";
}

}  // namespace LocalWorkflow
}  // namespace U2
