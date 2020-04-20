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

#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "RepeatWorker.h"

namespace U2 {
namespace LocalWorkflow {

/******************************
 * RepeatWorkerFactory
 ******************************/
static const QString NAME_ATTR("result-name");
static const QString LEN_ATTR("min-length");
static const QString IDENTITY_ATTR("identity");
static const QString MIN_DIST_ATTR("min-distance");
static const QString MAX_DIST_ATTR("max-distance");
static const QString INVERT_ATTR("inverted");
static const QString NESTED_ATTR("filter-algorithm");
static const QString ALGO_ATTR("algorithm");
static const QString THREADS_ATTR("threads");
static const QString TANMEDS_ATTR("exclude-tandems");
static const QString USE_MAX_DISTANCE_ATTR("use-maxdistance");
static const QString USE_MIN_DISTANCE_ATTR("use-mindistance");

const QString RepeatWorkerFactory::ACTOR_ID("repeats-search");

FindRepeatsTaskSettings RepeatWorkerFactory::defaultSettings() {
    FindRepeatsTaskSettings res;
    res.minLen = 5;
    res.setIdentity(100);
    res.minDist = 0;
    res.maxDist = 5000;
    res.inverted = false;
    res.excludeTandems = false;
    res.filter = DisjointRepeats;
    return res;
}

void RepeatWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> a;

    {
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(), RepeatWorker::tr("Input sequences"),
                        RepeatWorker::tr("A nucleotide sequence to search repeats in."));
        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(), RepeatWorker::tr("Repeat annotations"),
                        RepeatWorker::tr("A set of annotations marking repeats found in the sequence."));
        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("repeat.seq", inM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("repeat.annotations", outM)), false /*input*/, true /*multi*/);
    }
    {
        Descriptor nd(NAME_ATTR, RepeatWorker::tr("Annotate as"), RepeatWorker::tr("Name of the result annotations marking found repeats."));
        Descriptor idd(IDENTITY_ATTR, RepeatWorker::tr("Identity"), RepeatWorker::tr("Repeats identity."));
        Descriptor ld(LEN_ATTR, RepeatWorker::tr("Min length"), RepeatWorker::tr("Minimum length of repeats."));
        Descriptor mid(MIN_DIST_ATTR, RepeatWorker::tr("Min distance"), RepeatWorker::tr("Minimum distance between repeats."));
        Descriptor mad(MAX_DIST_ATTR, RepeatWorker::tr("Max distance"), RepeatWorker::tr("Maximum distance between repeats."));
        Descriptor ind(INVERT_ATTR, RepeatWorker::tr("Inverted"), RepeatWorker::tr("Search for inverted repeats."));
        Descriptor nsd(NESTED_ATTR, RepeatWorker::tr("Filter algorithm"), RepeatWorker::tr("Filter repeats algorithm."));
        Descriptor ald(ALGO_ATTR, RepeatWorker::tr("Algorithm"), RepeatWorker::tr("Control over variations of algorithm."));
        Descriptor thd(THREADS_ATTR, RepeatWorker::tr("Parallel threads"), RepeatWorker::tr("Number of parallel threads used for the task."));
        Descriptor tan(TANMEDS_ATTR, RepeatWorker::tr("Exclude tandems"), RepeatWorker::tr("Exclude tandems areas before find repeat task is run."));
        Descriptor umaxd(USE_MAX_DISTANCE_ATTR, RepeatWorker::tr("Apply 'Max distance' attribute"), RepeatWorker::tr("Apply 'Max distance' attribute."));
        Descriptor umind(USE_MIN_DISTANCE_ATTR, RepeatWorker::tr("Apply 'Min distance' attribute"), RepeatWorker::tr("Apply 'Min distance' attribute."));

        FindRepeatsTaskSettings cfg = RepeatWorkerFactory::defaultSettings();
        Attribute *aa;
        a << new Attribute(nd, BaseTypes::STRING_TYPE(), true, "repeat_unit");
        aa = new Attribute(ld, BaseTypes::NUM_TYPE(), false, cfg.minLen);
        a << aa;
        aa = new Attribute(idd, BaseTypes::NUM_TYPE(), false, cfg.getIdentity());
        a << aa;
        a << new Attribute(umind, BaseTypes::BOOL_TYPE(), false, true);
        aa = new Attribute(mid, BaseTypes::NUM_TYPE(), false, cfg.minDist);
        aa->addRelation(new VisibilityRelation(USE_MIN_DISTANCE_ATTR, true));
        a << aa;
        a << new Attribute(umaxd, BaseTypes::BOOL_TYPE(), false, true);
        aa = new Attribute(mad, BaseTypes::NUM_TYPE(), false, cfg.maxDist);
        aa->addRelation(new VisibilityRelation(USE_MAX_DISTANCE_ATTR, true));
        a << aa;
        aa = new Attribute(ind, BaseTypes::BOOL_TYPE(), false, cfg.inverted);
        a << aa;
        aa = new Attribute(nsd, BaseTypes::NUM_TYPE(), false, cfg.filter);
        a << aa;
        aa = new Attribute(ald, BaseTypes::NUM_TYPE(), false, cfg.algo);
        a << aa;
        aa = new Attribute(thd, BaseTypes::NUM_TYPE(), false, cfg.nThreads);
        a << aa;
        aa = new Attribute(tan, BaseTypes::BOOL_TYPE(), false, cfg.excludeTandems);
        a << aa;
    }

    Descriptor desc(ACTOR_ID, RepeatWorker::tr("Find Repeats"),
        RepeatWorker::tr("Finds repeats in each supplied sequence, stores found regions as annotations.")
       );
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;
    delegates[USE_MIN_DISTANCE_ATTR] = new ComboBoxWithBoolsDelegate();
    delegates[USE_MAX_DISTANCE_ATTR] = new ComboBoxWithBoolsDelegate();

    QVariantMap minDistProperties;
    minDistProperties["minimum"] = 0;
    minDistProperties["maximum"] = INT_MAX;
    minDistProperties["suffix"] = L10N::suffixBp();
    delegates[MIN_DIST_ATTR] = new SpinBoxDelegate(minDistProperties);

    QVariantMap maxDistProperties;
    maxDistProperties["minimum"] = 0;
    maxDistProperties["maximum"] = INT_MAX;
    maxDistProperties["suffix"] = L10N::suffixBp();
    delegates[MAX_DIST_ATTR] = new SpinBoxDelegate(maxDistProperties);

    QVariantMap lengthProperties;
    lengthProperties["minimum"] = 2;
    lengthProperties["maximum"] = INT_MAX;
    lengthProperties["suffix"] = L10N::suffixBp();
    lengthProperties["specialValueText"] = RepeatWorker::tr("Any");
    delegates[LEN_ATTR] = new SpinBoxDelegate(lengthProperties);

    QVariantMap identityProperties;
    identityProperties["minimum"] = 50;
    identityProperties["maximum"] = 100;
    identityProperties["suffix"] = "%";
    delegates[IDENTITY_ATTR] = new SpinBoxDelegate(identityProperties);

    QVariantMap threadProperties;
    threadProperties["specialValueText"] = "Auto";
    delegates[THREADS_ATTR] = new SpinBoxDelegate(threadProperties);

    QVariantMap algoProperties;
    algoProperties["Auto"] = RFAlgorithm_Auto;
    algoProperties["Diagonals"] = RFAlgorithm_Diagonal;
    algoProperties["Suffix index"] = RFAlgorithm_Suffix;
    delegates[ALGO_ATTR] = new ComboBoxDelegate(algoProperties);

    QVariantMap nestedProperties;
    nestedProperties["Disjoint repeats"] = DisjointRepeats;
    nestedProperties["No filtering"] = NoFiltering;
    nestedProperties["Unique repeats"] = UniqueRepeats;
    delegates[NESTED_ATTR] = new ComboBoxDelegate(nestedProperties);

    proto->setPrompter(new RepeatPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":repeat_finder/images/repeats.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new RepeatWorkerFactory());
}

/******************************
 * RepeatPrompter
 ******************************/
QString RepeatPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    // TODO extend ?
    QString resultName = getRequiredParam(NAME_ATTR);
    QString inverted = getParameter(INVERT_ATTR).toBool() ? tr("inverted") : tr("direct");

    QString doc = tr("For each sequence%1, find <u>%2</u> repeats."
        "<br>Detect <u>%3% identical</u> repeats <u>not shorter than %4 bps</u>."
        "<br>Output the list of found regions annotated as <u>%5</u>.")
        .arg(producerName) //sequence from Read Fasta 1
        .arg(getHyperlink(INVERT_ATTR, inverted))
        .arg(getHyperlink(IDENTITY_ATTR, getParameter(IDENTITY_ATTR).toInt()))
        .arg(getHyperlink(LEN_ATTR, getParameter(LEN_ATTR).toInt()))
        .arg(getHyperlink(NAME_ATTR, resultName));

    return doc;
}

/******************************
 * RepeatWorker
 ******************************/
RepeatWorker::RepeatWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void RepeatWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* RepeatWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.algo = RFAlgorithm(actor->getParameter(ALGO_ATTR)->getAttributeValue<int>(context));
        cfg.minLen = actor->getParameter(LEN_ATTR)->getAttributeValue<int>(context);
        if(actor->getParameter(USE_MIN_DISTANCE_ATTR)->getAttributeValue<bool>(context)){
            cfg.minDist = actor->getParameter(MIN_DIST_ATTR)->getAttributeValue<int>(context);
        }else{
            cfg.minDist = 0;
        }
        if(actor->getParameter(USE_MAX_DISTANCE_ATTR)->getAttributeValue<bool>(context)){
            cfg.maxDist = actor->getParameter(MAX_DIST_ATTR)->getAttributeValue<int>(context);
        }else{
            cfg.maxDist = INT_MAX;
        }
        int identity = actor->getParameter(IDENTITY_ATTR)->getAttributeValue<int>(context);
        cfg.setIdentity(identity);
        cfg.nThreads = actor->getParameter(THREADS_ATTR)->getAttributeValue<int>(context);
        cfg.inverted = actor->getParameter(INVERT_ATTR)->getAttributeValue<bool>(context);
        cfg.filter = RepeatsFilterAlgorithm(actor->getParameter(NESTED_ATTR)->getAttributeValue<int>(context));
        cfg.excludeTandems = actor->getParameter(TANMEDS_ATTR)->getAttributeValue<bool>(context);
        resultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
        if(resultName.isEmpty()){
            resultName = "repeat_unit";
            algoLog.error(tr("result name is empty, default name used"));
        }
        if(identity > 100 || identity < 0){
            algoLog.error(tr("Incorrect value: identity value must be between 0 and 100"));
            return new FailTask(tr("Incorrect value: identity value must be between 0 and 100"));
        }
        QVariantMap map = inputMessage.getData().toMap();
        SharedDbiDataHandler seqId = map.value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if(cfg.minDist < 0){
            algoLog.error(tr("Incorrect value: minimal distance must be greater then zero"));
            return new FailTask(tr("Incorrect value: minimal distance must be greater then zero"));
        }

        if (!seq.alphabet->isNucleic()) {
            QString err = tr("Sequence alphabet is not nucleic!");
            return new FailTask(err);
        }
        Task* t = new FindRepeatsToAnnotationsTask(cfg, seq, resultName, QString(), "", GObjectReference());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void RepeatWorker::sl_taskFinished() {
    FindRepeatsToAnnotationsTask *t = qobject_cast<FindRepeatsToAnnotationsTask *>(sender());
    if (t->getState() != Task::State_Finished || t->hasError() || t->isCanceled()) {
        return;
    }
    if (NULL != output) {
        const QList<SharedAnnotationData> res = t->importAnnotations();
        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(res);
        const QVariant v = qVariantFromValue<SharedDbiDataHandler>(tableId);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
        algoLog.info(tr("Found %1 repeats").arg(res.size()));
    }
}

void RepeatWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
