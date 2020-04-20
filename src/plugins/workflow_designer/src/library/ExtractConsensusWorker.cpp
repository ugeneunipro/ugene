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

#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>
#include <U2Algorithm/BuiltInAssemblyConsensusAlgorithms.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/FailTask.h>
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

#include <U2View/AssemblyModel.h>
#include <U2View/ExportConsensusTask.h>

#include "ExtractConsensusWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ExtractConsensusWorkerFactory::ACTOR_ID("extract-consensus");

namespace {
    const QString ALGO_ATTR_ID("algorithm");
    const QString GAPS_ATTR_ID("keep-gaps");
}

ExtractConsensusWorker::ExtractConsensusWorker(Actor *actor)
: BaseWorker(actor)
{

}

void ExtractConsensusWorker::init() {

}

Task * ExtractConsensusWorker::tick() {
    if (hasAssembly()) {
        U2OpStatusImpl os;
        const U2EntityRef assembly = takeAssembly(os);
        CHECK_OP(os, new FailTask(os.getError()));

        return createTask(assembly);
    } else {
        finish();
        return NULL;
    }
}

void ExtractConsensusWorker::sl_taskFinished() {
    ExtractConsensusTaskHelper *t = dynamic_cast<ExtractConsensusTaskHelper*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished() && !t->hasError(), );
    CHECK(!t->isCanceled(), );

    sendResult(context->getDataStorage()->getDataHandler(t->getResult()));
}

void ExtractConsensusWorker::cleanup() {

}

bool ExtractConsensusWorker::hasAssembly() const {
    const IntegralBus *port = ports[BasePorts::IN_ASSEMBLY_PORT_ID()];
    SAFE_POINT(NULL != port, "NULL assembly port", false);
    return port->hasMessage();
}

U2EntityRef ExtractConsensusWorker::takeAssembly(U2OpStatus &os) {
    const Message m = getMessageAndSetupScriptValues(ports[BasePorts::IN_ASSEMBLY_PORT_ID()]);
    const QVariantMap data = m.getData().toMap();
    if (!data.contains(BaseSlots::ASSEMBLY_SLOT().getId())) {
        os.setError(tr("Empty assembly slot"));
        return U2EntityRef();
    }
    const SharedDbiDataHandler dbiId = data[BaseSlots::ASSEMBLY_SLOT().getId()].value<SharedDbiDataHandler>();
    const AssemblyObject *obj = StorageUtils::getAssemblyObject(context->getDataStorage(), dbiId);
    if (NULL == obj) {
        os.setError(tr("Error with assembly object"));
        return U2EntityRef();
    }
    return obj->getEntityRef();
}

Task * ExtractConsensusWorker::createTask(const U2EntityRef &assembly) {
    const QString algoId = getValue<QString>(ALGO_ATTR_ID);
    const bool keepGaps = getValue<bool>(GAPS_ATTR_ID);
    Task *t = new ExtractConsensusTaskHelper(algoId, keepGaps, assembly, context->getDataStorage()->getDbiRef());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return t;
}

void ExtractConsensusWorker::finish() {
    IntegralBus *inPort = ports[BasePorts::IN_ASSEMBLY_PORT_ID()];
    SAFE_POINT(NULL != inPort, "NULL assembly port", );
    SAFE_POINT(inPort->isEnded(), "The assembly is not ended", );
    IntegralBus *outPort = ports[BasePorts::OUT_SEQ_PORT_ID()];
    SAFE_POINT(NULL != outPort, "NULL sequence port", );

    outPort->setEnded();
    setDone();
}

void ExtractConsensusWorker::sendResult(const SharedDbiDataHandler &seqId) {
    QVariantMap data;
    data[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(seqId);
    IntegralBus *outPort = ports[BasePorts::OUT_SEQ_PORT_ID()];
    SAFE_POINT(NULL != outPort, "NULL sequence port", );

    outPort->put(Message(outPort->getBusType(), data));
}

/************************************************************************/
/* ExtractConsensusTaskHelper */
/************************************************************************/
ExtractConsensusTaskHelper::ExtractConsensusTaskHelper(const QString &algoId, bool keepGaps, const U2EntityRef &assembly, const U2DbiRef &targetDbi)
: Task(tr("Extract consensus"), TaskFlags_NR_FOSCOE),
  algoId(algoId),
  keepGaps(keepGaps),
  assembly(assembly),
  targetDbi(targetDbi),
  exportTask(NULL)
{

}

void ExtractConsensusTaskHelper::prepare() {
    ExportConsensusTaskSettings settings;

    settings.consensusAlgorithm = QSharedPointer<AssemblyConsensusAlgorithm>(createAlgorithm());
    CHECK_OP(stateInfo, );
    settings.model = QSharedPointer<AssemblyModel>(createModel());
    CHECK_OP(stateInfo, );

    settings.region = settings.model->getGlobalRegion();
    settings.seqObjName = settings.model->getAssembly().visualName;

    settings.saveToFile = false;
    settings.targetDbi = targetDbi;
    settings.addToProject = false;
    settings.keepGaps = keepGaps;

    exportTask = new ExportConsensusTask(settings);
    addSubTask(exportTask);
}

U2EntityRef ExtractConsensusTaskHelper::getResult() const {
    SAFE_POINT(NULL != exportTask, "NULL export task", U2EntityRef());
    const U2Sequence seq = exportTask->getResult();
    const U2EntityRef ref(targetDbi, seq.id);
    return ref;
}

AssemblyConsensusAlgorithm * ExtractConsensusTaskHelper::createAlgorithm() {
    AssemblyConsensusAlgorithmRegistry *reg = AppContext::getAssemblyConsensusAlgorithmRegistry();
    SAFE_POINT_EXT(NULL != reg, setError("NULL registry"), NULL);

    AssemblyConsensusAlgorithmFactory *f = reg->getAlgorithmFactory(algoId);
    if (NULL == f) {
        setError(tr("Unknown consensus algorithm: ") + algoId);
        return NULL;
    }

    return f->createAlgorithm();
}

AssemblyModel * ExtractConsensusTaskHelper::createModel() {
    const DbiConnection con(assembly.dbiRef, stateInfo);
    CHECK_OP(stateInfo, NULL);

    U2AssemblyDbi *dbi = con.dbi->getAssemblyDbi();
    SAFE_POINT_EXT(NULL != dbi, setError("NULL assembly dbi"), NULL);

    const U2Assembly object = dbi->getAssemblyObject(assembly.entityId, stateInfo);
    CHECK_OP(stateInfo, NULL);

    AssemblyModel *model = new AssemblyModel(con);
    model->setAssembly(dbi, object);

    return model;
}

/************************************************************************/
/* ExtractConsensusWorkerFactory */
/************************************************************************/
ExtractConsensusWorkerFactory::ExtractConsensusWorkerFactory()
: DomainFactory(ACTOR_ID)
{

}

Worker * ExtractConsensusWorkerFactory::createWorker(Actor *actor) {
    return new ExtractConsensusWorker(actor);
}

void ExtractConsensusWorkerFactory::init() {
    AssemblyConsensusAlgorithmRegistry *reg = AppContext::getAssemblyConsensusAlgorithmRegistry();
    SAFE_POINT(NULL != reg, "NULL registry", );

    const Descriptor desc(ACTOR_ID,
        QObject::tr("Extract Consensus from Assembly"),
        QObject::tr("Extract the consensus sequence from the incoming assembly."));

    QList<PortDescriptor*> ports;
    {
        QMap<Descriptor, DataTypePtr> inData;
        inData[BaseSlots::ASSEMBLY_SLOT()] = BaseTypes::ASSEMBLY_TYPE();
        DataTypePtr inType(new MapDataType(BasePorts::IN_ASSEMBLY_PORT_ID(), inData));
        ports << new PortDescriptor(BasePorts::IN_ASSEMBLY_PORT_ID(), inType, true);

        QMap<Descriptor, DataTypePtr> outData;
        outData[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        DataTypePtr outType(new MapDataType(BasePorts::OUT_SEQ_PORT_ID(), outData));
        ports << new PortDescriptor(BasePorts::OUT_SEQ_PORT_ID(), outType, false, true);
    }

    QList<Attribute*> attrs;
    {
        const Descriptor algoDesc(ALGO_ATTR_ID,
            QObject::tr("Algorithm"),
            QObject::tr("The algorithm of consensus extracting."));
        const Descriptor gapsDesc(GAPS_ATTR_ID,
            QObject::tr("Keep gaps"),
            QObject::tr("Set this parameter if the result consensus must keep the gaps."));
        attrs << new Attribute(algoDesc, BaseTypes::STRING_TYPE(), true, BuiltInAssemblyConsensusAlgorithms::DEFAULT_ALGO);
        attrs << new Attribute(gapsDesc, BaseTypes::BOOL_TYPE(), true, true);
    }

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap algos;
        foreach (const QString algoId, reg->getAlgorithmIds()) {
            AssemblyConsensusAlgorithmFactory *f = reg->getAlgorithmFactory(algoId);
            algos[f->getName()] = algoId;
        }
        delegates[ALGO_ATTR_ID] = new ComboBoxDelegate(algos);
    }

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new ExtractConsensusWorkerPrompter());
    proto->setEditor(new DelegateEditor(delegates));

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_NGS_BASIC(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ExtractConsensusWorkerFactory());
}

/************************************************************************/
/* ExtractConsensusWorkerPrompter */
/************************************************************************/
ExtractConsensusWorkerPrompter::ExtractConsensusWorkerPrompter(Actor *actor)
: PrompterBase<ExtractConsensusWorkerPrompter>(actor)
{

}

QString ExtractConsensusWorkerPrompter::composeRichDoc() {
    QString algorithm = getParameter(ALGO_ATTR_ID).toString();
    QString link = getHyperlink(ALGO_ATTR_ID, algorithm);
    return tr("Extracts the consensus sequence from the incoming assembly using the %1 algorithm.").arg(link);
}

} // LocalWorkflow
} // U2
