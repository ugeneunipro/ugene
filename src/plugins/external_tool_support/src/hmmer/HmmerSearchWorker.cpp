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

#include <U2Core/AnnotationData.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
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

#include "HmmerSearchTask.h"
#include "HmmerSearchWorker.h"
#include "HmmerSupport.h"

namespace U2 {
namespace LocalWorkflow {

/*******************************
 * HMM3SearchWorkerFactory
 *******************************/
static const QString HMM_URL_PORT("in-hmm3");

static const QString NAME_ATTR("result-name");
static const QString DOM_E_ATTR("e-val");
static const QString DOM_T_ATTR("score");
static const QString SEED_ATTR("seed");
static const QString FILTER_BY_ATTR("filter-by");

static const QString FILTER_BY_E_VALUE_STRING ("E-value");
static const QString FILTER_BY_SCORE_STRING ("Score");
static const QString FILTER_BY_NONE_STRING("Do not filter results");

static const QString FILTER_BY_E_VALUE("evalue");
static const QString FILTER_BY_SCORE("score");
static const QString FILTER_BY_NONE("none");

const QString HmmerSearchWorkerFactory::ACTOR("hmm3-search");

void HmmerSearchWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> a;
    {
        Descriptor filterByDesc(FILTER_BY_ATTR,
            HmmerSearchWorker::tr("Filter by"),
            HmmerSearchWorker::tr("Parameter to filter results by."));
        Descriptor hd(HMM_URL_PORT, HmmerSearchWorker::tr("HMMER profile"), HmmerSearchWorker::tr("HMMER profile(s) URL(s) to search with."));
        Descriptor sd(BasePorts::IN_SEQ_PORT_ID(), HmmerSearchWorker::tr("Input sequence"),
            HmmerSearchWorker::tr("An input sequence (nucleotide or protein) to search in."));
        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(), HmmerSearchWorker::tr("HMMER annotations"),
            HmmerSearchWorker::tr("Annotations marking found similar sequence regions."));

        QMap<Descriptor, DataTypePtr> hmmM;
        hmmM[BaseSlots::URL_SLOT()] = BaseTypes::STRING_TYPE();
        p << new PortDescriptor(hd, DataTypePtr(new MapDataType("hmm.search.hmm", hmmM)), true /*input*/, false, IntegralBusPort::BLIND_INPUT);
        QMap<Descriptor, DataTypePtr> seqM;
        seqM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(sd, DataTypePtr(new MapDataType("hmm.search.sequence", seqM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("hmm.search.out", outM)), false /*input*/, true);

        Descriptor nd(NAME_ATTR, HmmerSearchWorker::tr("Result annotation"), HmmerSearchWorker::tr("A name of the result annotations."));
        Descriptor nsd(SEED_ATTR, HmmerSearchWorker::tr("Seed"), HmmerSearchWorker::tr("Random generator seed. 0 - means that one-time arbitrary seed will be used."));
        Descriptor ded(DOM_E_ATTR, HmmerSearchWorker::tr("Filter by high E-value"), HmmerSearchWorker::tr("Report domains with e-value less than."));
        Descriptor dtd(DOM_T_ATTR, HmmerSearchWorker::tr("Filter by low score"), HmmerSearchWorker::tr("Report domains with score greater than."));

        Attribute *evalue = new Attribute(ded, BaseTypes::NUM_TYPE(), false, QVariant((double)10.0));
        Attribute *score = new Attribute(dtd, BaseTypes::NUM_TYPE(), false, QVariant((double)0.0));

        Attribute *filterBy = new Attribute(filterByDesc, BaseTypes::STRING_TYPE(), true, FILTER_BY_NONE);
        a << new Attribute(nd, BaseTypes::STRING_TYPE(), true, QVariant("hmm_signal"));
        a << filterBy;
        a << new Attribute(nsd, BaseTypes::NUM_TYPE(), false, QVariant(42));
        a << evalue;
        a << score;


        evalue->addRelation(new VisibilityRelation(FILTER_BY_ATTR, FILTER_BY_E_VALUE));
        score->addRelation(new VisibilityRelation(FILTER_BY_ATTR, FILTER_BY_SCORE));
    }

    Descriptor desc(HmmerSearchWorkerFactory::ACTOR, HmmerSearchWorker::tr("HMM3 Search"),
        HmmerSearchWorker::tr("Searches each input sequence for significantly similar sequence matches to all specified HMM profiles."
        " In case several profiles were supplied, searches with all profiles one by one and outputs united set of annotations for each sequence."));
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate *> delegates;

    {
        QVariantMap filterByValues;
        filterByValues[FILTER_BY_E_VALUE_STRING] = FILTER_BY_E_VALUE;
        filterByValues[FILTER_BY_SCORE_STRING] = FILTER_BY_SCORE;
        filterByValues[FILTER_BY_NONE_STRING] = FILTER_BY_NONE;
        delegates[FILTER_BY_ATTR] = new ComboBoxDelegate(filterByValues);
    }

    {
        QVariantMap eMap;
        eMap["decimals"]= (2);
        eMap["minimum"] = (1e-99);
        eMap["maximum"] = (1e+99);
        eMap["singleStep"] = (0.1);
        delegates[DOM_E_ATTR] = new DoubleSpinBoxDelegate(eMap);
    }
    {
        QVariantMap nMap;
        nMap["maximum"] = (INT_MAX);
        nMap["minimum"] = (0);
        delegates[SEED_ATTR] = new SpinBoxDelegate(nMap);
    }
    {
        QVariantMap tMap;
        tMap["decimals"]= (2);
        tMap["minimum"] = (-1e+09);
        tMap["maximum"] = (1e+09);
        tMap["singleStep"] = (0.1);
        delegates[DOM_T_ATTR] = new DoubleSpinBoxDelegate(tMap);
    }

    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":/external_tool_support/images/hmmer.png");
    proto->setPrompter(new HmmerSearchPrompter());
    proto->addExternalTool(HmmerSupport::SEARCH_TOOL_ID);
    WorkflowEnv::getProtoRegistry()->registerProto(Descriptor("hmmer3", HmmerSearchWorker::tr("HMMER3 Tools"), ""), proto);

    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new HmmerSearchWorkerFactory());
}

HmmerSearchWorkerFactory::HmmerSearchWorkerFactory()
    : DomainFactory(ACTOR)
{

}

Worker * HmmerSearchWorkerFactory::createWorker(Actor *a) {
    return new HmmerSearchWorker(a);
}

/*******************************
 * HMM3SearchPrompter
 *******************************/
HmmerSearchPrompter::HmmerSearchPrompter(Actor *p)
    : PrompterBase<HmmerSearchPrompter>(p)
{

}

QString HmmerSearchPrompter::composeRichDoc() {
    Actor *hmmProducer = qobject_cast<IntegralBusPort *>(target->getPort(HMM_URL_PORT))->getProducer(HMM_URL_PORT);
    Actor *seqProducer = qobject_cast<IntegralBusPort *>(target->getPort(BasePorts::IN_SEQ_PORT_ID()))->getProducer(BasePorts::IN_SEQ_PORT_ID());

    QString seqName = (seqProducer ? tr("For each sequence from <u>%1</u>,").arg(seqProducer->getLabel()) : "");
    QString hmmName = (hmmProducer ? tr("using all profiles provided by <u>%1</u>,").arg(hmmProducer->getLabel()) : "");

    QString resultName = getHyperlink(NAME_ATTR, getRequiredParam(NAME_ATTR));

    QString doc = tr("%1 search HMMER signals %2. "
        "<br>Output the list of found regions annotated as <u>%4</u>.")
        .arg(seqName)
        .arg(hmmName)
        .arg(resultName);

    return doc;
}

/*******************************
 * HMM3SearchWorker
 *******************************/
HmmerSearchWorker::HmmerSearchWorker(Actor *a)
    : BaseWorker(a, false),
      hmmPort(NULL),
      seqPort(NULL),
      output(NULL)
{

}

void HmmerSearchWorker::init() {
    cfg = HmmerSearchSettings();

    hmmPort = ports.value(HMM_URL_PORT);
    seqPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
    seqPort->addComplement(output);
    output->addComplement(seqPort);

    QString filterBy = actor->getParameter(FILTER_BY_ATTR)->getAttributeValue<QString>(context);
    if (filterBy == FILTER_BY_E_VALUE) {
        cfg.domE = actor->getParameter(DOM_E_ATTR)->getAttributeValue<double>(context);
        cfg.domT = HmmerSearchSettings::OPTION_NOT_SET;
    } else if (filterBy == FILTER_BY_SCORE) {
        cfg.domT = actor->getParameter(DOM_T_ATTR)->getAttributeValue<double>(context);
        cfg.domE = HmmerSearchSettings::OPTION_NOT_SET;
    } else {
        cfg.domE = HmmerSearchSettings::OPTION_NOT_SET;
        cfg.domT = HmmerSearchSettings::OPTION_NOT_SET;
    }

    cfg.seed = actor->getParameter(SEED_ATTR)->getAttributeValue<int>(context);
    resultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
    if (resultName.isEmpty()) {
        algoLog.details(tr("Value for attribute name is empty, default name used"));
        resultName = "hmm_signal";
    }
}

bool HmmerSearchWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    bool seqEnded = seqPort->isEnded();
    bool hmmEnded = hmmPort->isEnded();
    int seqHasMes = seqPort->hasMessage();
    int hmmHasMes = hmmPort->hasMessage();
    return hmmHasMes || (hmmEnded && (seqHasMes || seqEnded));
}

Task * HmmerSearchWorker::tick() {
    while (hmmPort->hasMessage()) {
        hmms << hmmPort->get().getData().toMap().value(BaseSlots::URL_SLOT().getId()).toString();
    }
    if (!hmmPort->isEnded()) { //  || hmms.isEmpty() || !seqPort->hasMessage()
        return NULL;
    }

    if (seqPort->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(seqPort);
        if (inputMessage.isEmpty() || hmms.isEmpty()) {
            output->transit();
            return NULL;
        }
        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (NULL == seqObj) {
            return NULL;
        }

        if (seqObj->getAlphabet()->getType() != DNAAlphabet_RAW) {
            QList<Task *> subtasks;
            HmmerSearchSettings settings = cfg;
            foreach (const QString &hmmProfileUrl, hmms) {
                settings.workingDir = monitor()->outputDir() + "hmmer_search";
                settings.hmmProfileUrl = hmmProfileUrl;
                settings.sequence = seqObj.data();
                settings.pattern.annotationName = resultName;
                settings.annotationTable = new AnnotationTableObject("Annotation table", context->getDataStorage()->getDbiRef());
                HmmerSearchTask *searchTask = new HmmerSearchTask(settings);
                settings.annotationTable->setParent(searchTask);
                searchTask->addListeners(createLogListeners());
                subtasks << searchTask;
            }

            Task *multiTask = new MultiTask(tr("Find HMMER signals in %1").arg(seqObj->getGObjectName()), subtasks);
            connect(new TaskSignalMapper(multiTask), SIGNAL(si_taskFinished(Task *)), SLOT(sl_taskFinished(Task *)));
            seqObj.take()->setParent(multiTask);
            return multiTask;
        }
        QString err = tr("Bad sequence supplied to input: %1").arg(seqObj->getGObjectName());
        return new FailTask(err);
    } if (seqPort->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void HmmerSearchWorker::sl_taskFinished(Task *task) {
    SAFE_POINT(NULL != task, "Invalid task is encountered", );
    if (task->isCanceled()) {
        return;
    }
    if (NULL != output) {
        QList<SharedAnnotationData> list;

        foreach(const QPointer<Task> &sub, task->getSubtasks()) {
            HmmerSearchTask *searchTask = qobject_cast<HmmerSearchTask *>(sub.data());
            if (searchTask == NULL){
                continue;
            }
            list << searchTask->getAnnotations();
        }

        CHECK(!list.isEmpty(), );

        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(list);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(tableId)));
        algoLog.info(tr("Found %1 HMMER signals").arg(list.size()));
    }
}

void HmmerSearchWorker::cleanup() {

}

}   // namespace LocalWorkflow
}   // namespace U2
