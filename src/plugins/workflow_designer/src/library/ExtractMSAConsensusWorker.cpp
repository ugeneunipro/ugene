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

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusUtils.h>

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

#include "ExtractMSAConsensusWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ExtractMSAConsensusSequenceWorkerFactory::ACTOR_ID("extract-msa-consensus-sequence");
const QString ExtractMSAConsensusStringWorkerFactory::ACTOR_ID("extract-msa-consensus-string");

namespace {
    const QString ALGO_ATTR_ID("algorithm");
    const QString THRESHOLD_ATTR_ID("threshold");
    const QString GAPS_ATTR_ID("keep-gaps");
}

ExtractMSAConsensusWorker::ExtractMSAConsensusWorker(Actor *actor)
    : BaseWorker(actor),
      extractMsaConsensus(NULL)
{

}

void ExtractMSAConsensusWorker::init() {

}

Task* ExtractMSAConsensusWorker::tick() {
    if (hasMsa()) {
        U2OpStatusImpl os;
        MultipleSequenceAlignment msa = takeMsa(os);
        CHECK_OP(os, new FailTask(os.getError()));
        extractMsaConsensus = createTask(msa);
        return extractMsaConsensus;
    } else {
        finish();
        return NULL;
    }
}

void ExtractMSAConsensusWorker::sl_taskFinished() {
    ExtractMSAConsensusTaskHelper *t = dynamic_cast<ExtractMSAConsensusTaskHelper*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished() && !t->hasError(), );
    CHECK(!t->isCanceled(), );

    sendResult(context->getDataStorage()->getDataHandler(t->getResult()));
}

void ExtractMSAConsensusWorker::cleanup() {

}

bool ExtractMSAConsensusWorker::hasMsa() const {
    const IntegralBus *port = ports[BasePorts::IN_MSA_PORT_ID()];
    SAFE_POINT(NULL != port, "NULL msa port", false);
    return port->hasMessage();
}

MultipleSequenceAlignment ExtractMSAConsensusWorker::takeMsa(U2OpStatus &os) {
    const Message m = getMessageAndSetupScriptValues(ports[BasePorts::IN_MSA_PORT_ID()]);
    const QVariantMap data = m.getData().toMap();
    if (!data.contains(BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId())) {
        os.setError(tr("Empty msa slot"));
        return MultipleSequenceAlignment();
    }
    const SharedDbiDataHandler dbiId = data[BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId()].value<SharedDbiDataHandler>();
    const MultipleSequenceAlignmentObject *obj = StorageUtils::getMsaObject(context->getDataStorage(), dbiId);
    if (NULL == obj) {
        os.setError(tr("Error with msa object"));
        return MultipleSequenceAlignment();
    }
    return obj->getMultipleAlignment();
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusStringWorker
ExtractMSAConsensusStringWorker::ExtractMSAConsensusStringWorker(Actor *actor):ExtractMSAConsensusWorker(actor){}

void ExtractMSAConsensusStringWorker::finish() {
    IntegralBus *inPort = ports[BasePorts::IN_MSA_PORT_ID()];
    SAFE_POINT(NULL != inPort, "NULL msa port", );
    SAFE_POINT(inPort->isEnded(), "The msa is not ended", );
    IntegralBus *outPort = ports[BasePorts::OUT_TEXT_PORT_ID()];
    SAFE_POINT(NULL != outPort, "NULL text port", );

    outPort->setEnded();
    setDone();
}

void ExtractMSAConsensusStringWorker::sendResult(const SharedDbiDataHandler & /*seqId*/) {
    QVariantMap data;
    data[BaseSlots::TEXT_SLOT().getId()] = extractMsaConsensus->getResultAsText();
    IntegralBus *outPort = ports[BasePorts::OUT_TEXT_PORT_ID()];

    SAFE_POINT(NULL != outPort, "NULL text port", );

    outPort->put(Message(outPort->getBusType(), data));
}

ExtractMSAConsensusTaskHelper* ExtractMSAConsensusStringWorker::createTask(const MultipleSequenceAlignment &msa){
    const QString algoId = getValue<QString>(ALGO_ATTR_ID);
    const int threshold = getValue<int>(THRESHOLD_ATTR_ID);
    extractMsaConsensus = new ExtractMSAConsensusTaskHelper(algoId, threshold, true, msa, context->getDataStorage()->getDbiRef());
    connect(extractMsaConsensus, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return extractMsaConsensus;
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusSequenceWorker
ExtractMSAConsensusSequenceWorker::ExtractMSAConsensusSequenceWorker(Actor *actor):ExtractMSAConsensusWorker(actor){}

void ExtractMSAConsensusSequenceWorker::finish() {
    IntegralBus *inPort = ports[BasePorts::IN_MSA_PORT_ID()];
    SAFE_POINT(NULL != inPort, "NULL msa port", );
    SAFE_POINT(inPort->isEnded(), "The msa is not ended", );
    IntegralBus *outPort = ports[BasePorts::OUT_SEQ_PORT_ID()];
    SAFE_POINT(NULL != outPort, "NULL sequence port", );

    outPort->setEnded();
    setDone();
}

void ExtractMSAConsensusSequenceWorker::sendResult(const SharedDbiDataHandler &seqId) {
    QVariantMap data;
    data[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue<SharedDbiDataHandler>(seqId);
    IntegralBus *outPort = ports[BasePorts::OUT_SEQ_PORT_ID()];
    SAFE_POINT(NULL != outPort, "NULL sequence port", );

    outPort->put(Message(outPort->getBusType(), data));
}

ExtractMSAConsensusTaskHelper* ExtractMSAConsensusSequenceWorker::createTask(const MultipleSequenceAlignment &msa){
    const QString algoId = getValue<QString>(ALGO_ATTR_ID);
    const int threshold = getValue<int>(THRESHOLD_ATTR_ID);
    const bool keepGaps = getValue<bool>(GAPS_ATTR_ID);
    extractMsaConsensus = new ExtractMSAConsensusTaskHelper(algoId, threshold, keepGaps, msa, context->getDataStorage()->getDbiRef());
    connect(extractMsaConsensus, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
    return extractMsaConsensus;
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusTaskHelper
ExtractMSAConsensusTaskHelper::ExtractMSAConsensusTaskHelper(const QString &algoId, int threshold, bool keepGaps, const MultipleSequenceAlignment &msa, const U2DbiRef &targetDbi)
: Task(ExtractMSAConsensusTaskHelper::tr("Extract consensus"), TaskFlags_NR_FOSCOE),
  algoId(algoId),
  threshold(threshold),
  keepGaps(keepGaps),
  msa(msa->getCopy()),
  targetDbi(targetDbi)
  //,resultText("")
{

}

QString ExtractMSAConsensusTaskHelper::getResultName () const {
    QString res;
    res = msa->getName();
    res+="_consensus";
    return res;
}

void ExtractMSAConsensusTaskHelper::prepare() {
    QSharedPointer<MSAConsensusAlgorithm> algo(createAlgorithm());
    SAFE_POINT_EXT(NULL != algo, setError("Wrong consensus algorithm"), );

    MSAConsensusUtils::updateConsensus(msa, resultText, algo.data());
    if(!keepGaps && algo->getFactory()->isSequenceLikeResult()){
        resultText.replace("-", "");
    }

    if (algo->getFactory()->isSequenceLikeResult()) {
        U2SequenceImporter seqImporter;
        seqImporter.startSequence(stateInfo, targetDbi, U2ObjectDbi::ROOT_FOLDER, getResultName(), false);
        seqImporter.addBlock(resultText.data(), resultText.length(), stateInfo);
        resultSequence = seqImporter.finalizeSequence(stateInfo);
    }
}

U2EntityRef ExtractMSAConsensusTaskHelper::getResult() const {
    const U2EntityRef ref(targetDbi, resultSequence.id);
    return ref;
}

MSAConsensusAlgorithm * ExtractMSAConsensusTaskHelper::createAlgorithm() {
    MSAConsensusAlgorithmRegistry *reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT_EXT(NULL != reg, setError("NULL registry"), NULL);

    MSAConsensusAlgorithmFactory *f = reg->getAlgorithmFactory(algoId);
    if (NULL == f) {
        setError(ExtractMSAConsensusTaskHelper::tr("Unknown consensus algorithm: ") + algoId);
        return NULL;
    }
    MSAConsensusAlgorithm* alg = f->createAlgorithm(msa);
    SAFE_POINT_EXT(NULL != alg, setError("NULL algorithm"), NULL);
    alg->setThreshold(threshold);

    return alg;
}

QByteArray ExtractMSAConsensusTaskHelper::getResultAsText() const {
    return resultText;
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusWorkerFactory
ExtractMSAConsensusSequenceWorkerFactory::ExtractMSAConsensusSequenceWorkerFactory()
: DomainFactory(ACTOR_ID)
{

}

Worker * ExtractMSAConsensusSequenceWorkerFactory::createWorker(Actor *actor) {
    return new ExtractMSAConsensusSequenceWorker(actor);
}

void ExtractMSAConsensusSequenceWorkerFactory::init() {
    MSAConsensusAlgorithmRegistry *reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT(NULL != reg, "NULL registry", );

    const Descriptor desc(ACTOR_ID,
        ExtractMSAConsensusSequenceWorker::tr("Extract Consensus from Alignment as Sequence"),
        ExtractMSAConsensusSequenceWorker::tr("Extract the consensus sequence from the incoming multiple sequence alignment."));

    QList<PortDescriptor*> ports;
    {
        Descriptor inD(BasePorts::IN_MSA_PORT_ID(),
            ExtractMSAConsensusStringWorker::tr("Input alignment"),
            ExtractMSAConsensusStringWorker::tr("A alignment which consensus should be extracted"));
        QMap<Descriptor, DataTypePtr> inData;
        inData[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        ports << new PortDescriptor(inD, DataTypePtr(new MapDataType(BasePorts::IN_MSA_PORT_ID(), inData)), true);

        Descriptor outD(BasePorts::OUT_SEQ_PORT_ID(),
            ExtractMSAConsensusSequenceWorker::tr("Consensus sequence"),
            ExtractMSAConsensusSequenceWorker::tr("Provides resulting consensus as a sequence"));

        QMap<Descriptor, DataTypePtr> outData;
        outData[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        ports << new PortDescriptor(outD, DataTypePtr(new MapDataType(BasePorts::OUT_SEQ_PORT_ID(), outData)), false, true);
    }

    QList<Attribute*> attrs;
    QMap<QString, PropertyDelegate*> delegates;
    {
        const Descriptor algoDesc(ALGO_ATTR_ID,
            ExtractMSAConsensusSequenceWorker::tr("Algorithm"),
            ExtractMSAConsensusSequenceWorker::tr("The algorithm of consensus extracting."));
        const Descriptor thresholdDesc(THRESHOLD_ATTR_ID,
            ExtractMSAConsensusSequenceWorker::tr("Threshold"),
            ExtractMSAConsensusSequenceWorker::tr("The threshold of the algorithm."));
        const Descriptor gapsDesc(GAPS_ATTR_ID,
            ExtractMSAConsensusSequenceWorker::tr("Keep gaps"),
            ExtractMSAConsensusSequenceWorker::tr("Set this parameter if the result consensus must keep the gaps."));

        Attribute *thr = new Attribute(thresholdDesc, BaseTypes::NUM_TYPE(), true, 100);
        Attribute *algo = new Attribute(algoDesc, BaseTypes::STRING_TYPE(), true, BuiltInConsensusAlgorithms::STRICT_ALGO);
        attrs << algo << thr << new Attribute(gapsDesc, BaseTypes::BOOL_TYPE(), true, true);

        QVariantMap algos;
        QVariantMap m;
        QVariantList visibleRelationList;
        m["minimum"] = 0;
        m["maximum"] = 100;
        SpinBoxDelegate *thrDelegate = new SpinBoxDelegate(m);
        foreach (const QString &algoId, reg->getAlgorithmIds()) {
            MSAConsensusAlgorithmFactory *f = reg->getAlgorithmFactory(algoId);
            if(f->isSequenceLikeResult()){
                algos[f->getName()] = algoId;
                if (f->supportsThreshold()) {
                    visibleRelationList.append(algoId);
                }
            }
        }
        thr->addRelation(new VisibilityRelation(ALGO_ATTR_ID, visibleRelationList));
        algo->addRelation(new SpinBoxDelegatePropertyRelation(THRESHOLD_ATTR_ID));
        delegates[ALGO_ATTR_ID] = new ComboBoxDelegate(algos);
        delegates[THRESHOLD_ATTR_ID] = thrDelegate;
    }

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new ExtractMSAConsensusWorkerPrompter());
    proto->setEditor(new DelegateEditor(delegates));

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ExtractMSAConsensusSequenceWorkerFactory());
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusStringWorkerFactory
ExtractMSAConsensusStringWorkerFactory::ExtractMSAConsensusStringWorkerFactory()
: DomainFactory(ACTOR_ID)
{

}

Worker * ExtractMSAConsensusStringWorkerFactory::createWorker(Actor *actor) {
    return new ExtractMSAConsensusStringWorker(actor);
}

void ExtractMSAConsensusStringWorkerFactory::init() {
    MSAConsensusAlgorithmRegistry *reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT(NULL != reg, "NULL registry", );

    const Descriptor desc(ACTOR_ID,
        ExtractMSAConsensusSequenceWorker::tr("Extract Consensus from Alignment as Text"),
        ExtractMSAConsensusSequenceWorker::tr("Extract the consensus string from the incoming multiple sequence alignment."));

    QList<PortDescriptor*> ports;
    {
        Descriptor inD(BasePorts::IN_MSA_PORT_ID(),
            ExtractMSAConsensusStringWorker::tr("Input alignment"),
            ExtractMSAConsensusStringWorker::tr("A alignment which consensus should be extracted"));

        QMap<Descriptor, DataTypePtr> inData;
        inData[BaseSlots::MULTIPLE_ALIGNMENT_SLOT()] = BaseTypes::MULTIPLE_ALIGNMENT_TYPE();
        ports << new PortDescriptor(inD, DataTypePtr(new MapDataType(BasePorts::IN_MSA_PORT_ID(), inData)), true);

        Descriptor outD(BasePorts::OUT_TEXT_PORT_ID(),
            ExtractMSAConsensusStringWorker::tr("Consensus"),
            ExtractMSAConsensusStringWorker::tr("Provides resulting consensus as a text"));

        QMap<Descriptor, DataTypePtr> outData;
        outData[BaseSlots::TEXT_SLOT()] = BaseTypes::STRING_TYPE();
        ports << new PortDescriptor(outD, DataTypePtr(new MapDataType(BasePorts::OUT_TEXT_PORT_ID(), outData)) , false, true);
    }

    QList<Attribute*> attrs;
    QMap<QString, PropertyDelegate*> delegates;
    {
        const Descriptor algoDesc(ALGO_ATTR_ID,
            ExtractMSAConsensusSequenceWorker::tr("Algorithm"),
            ExtractMSAConsensusSequenceWorker::tr("The algorithm of consensus extracting."));
        const Descriptor thresholdDesc(THRESHOLD_ATTR_ID,
            ExtractMSAConsensusSequenceWorker::tr("Threshold"),
            ExtractMSAConsensusSequenceWorker::tr("The threshold of the algorithm."));
        Attribute *thr = new Attribute(thresholdDesc, BaseTypes::NUM_TYPE(), true, 100);
        Attribute *algo = new Attribute(algoDesc, BaseTypes::STRING_TYPE(), true, BuiltInConsensusAlgorithms::DEFAULT_ALGO);
        attrs << algo << thr;

        QVariantList visibleRelationList;
        QVariantMap algos;
        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = 100;
        SpinBoxDelegate *thrDelegate = new SpinBoxDelegate(m);
        foreach (const QString &algoId, reg->getAlgorithmIds()) {
            MSAConsensusAlgorithmFactory *f = reg->getAlgorithmFactory(algoId);
            if(!f->isSequenceLikeResult()){
                algos[f->getName()] = algoId;
                if (f->supportsThreshold()) {
                    visibleRelationList.append(algoId);
                }
            }
        }
        thr->addRelation(new VisibilityRelation(ALGO_ATTR_ID, visibleRelationList));
        algo->addRelation(new SpinBoxDelegatePropertyRelation(THRESHOLD_ATTR_ID));
        delegates[ALGO_ATTR_ID] = new ComboBoxDelegate(algos);
        delegates[THRESHOLD_ATTR_ID] = thrDelegate;
    }

    ActorPrototype *proto = new IntegralBusActorPrototype(desc, ports, attrs);
    proto->setPrompter(new ExtractMSAConsensusWorkerPrompter());
    proto->setEditor(new DelegateEditor(delegates));

    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_ALIGNMENT(), proto);
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ExtractMSAConsensusStringWorkerFactory());
}

///////////////////////////////////////////////////////////////////////
//ExtractMSAConsensusWorkerPrompter
ExtractMSAConsensusWorkerPrompter::ExtractMSAConsensusWorkerPrompter(Actor *actor)
: PrompterBase<ExtractMSAConsensusWorkerPrompter>(actor)
{

}

QString ExtractMSAConsensusWorkerPrompter::composeRichDoc() {
    QString algorithm = getParameter(ALGO_ATTR_ID).toString();
    QString link = getHyperlink(ALGO_ATTR_ID, algorithm);
    return ExtractMSAConsensusSequenceWorker::tr("Extracts the consensus sequence from the incoming alignment(s) using the %1 algorithm.").arg(link);
}

SpinBoxDelegatePropertyRelation *SpinBoxDelegatePropertyRelation::clone() const {
    return new SpinBoxDelegatePropertyRelation(*this);
}

QVariant SpinBoxDelegatePropertyRelation::getAffectResult( const QVariant &influencingValue, const QVariant &dependentValue, DelegateTags * /*infTags*/, DelegateTags *depTags ) const {
    CHECK(depTags != NULL, dependentValue);
    updateDelegateTags(influencingValue, depTags);
    int res = qBound(depTags->get("minimum").toInt(), dependentValue.toInt(), depTags->get("maximum").toInt());
    return res;
}

void SpinBoxDelegatePropertyRelation::updateDelegateTags( const QVariant &influencingValue, DelegateTags *dependentTags ) const {
    MSAConsensusAlgorithmRegistry *reg = AppContext::getMSAConsensusAlgorithmRegistry();
    SAFE_POINT(NULL != reg, "NULL registry", );
    MSAConsensusAlgorithmFactory *consFactory = reg->getAlgorithmFactory(influencingValue.toString());
    if(!consFactory){
        return;
    }
    if (dependentTags != NULL) {
        dependentTags->set("minimum", consFactory->getMinThreshold());
        dependentTags->set("maximum", consFactory->getMaxThreshold());
    }
}

} // LocalWorkflow
} // U2
