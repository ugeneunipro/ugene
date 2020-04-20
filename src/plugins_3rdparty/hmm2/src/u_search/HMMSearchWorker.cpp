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

#include <QApplication>

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

#include "HMMSearchWorker.h"
#include "HMMIOWorker.h"
#include "HMMSearchTask.h"
#include "hmmer2/funcs.h"

namespace U2 {
namespace LocalWorkflow {

/*******************************
 * HMMSearchWorkerFactory
 *******************************/
static const QString HMM_PORT("in-hmm2");

static const QString NAME_ATTR("result-name");
static const QString NSEQ_ATTR("seqs-num");
static const QString DOM_E_ATTR("e-val");
static const QString DOM_T_ATTR("score");

const QString HMMSearchWorkerFactory::ACTOR("hmm2-search");

void HMMSearchWorkerFactory::init() {

    QList<PortDescriptor*> p; QList<Attribute*> a;
    {
        Descriptor hd(HMM_PORT, HMMSearchWorker::tr("HMM profile"), HMMSearchWorker::tr("HMM profile(s) to search with."));
        Descriptor sd(BasePorts::IN_SEQ_PORT_ID(), HMMSearchWorker::tr("Input sequence"),
            HMMSearchWorker::tr("An input sequence (nucleotide or protein) to search in."));
        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(), HMMSearchWorker::tr("HMM annotations"),
            HMMSearchWorker::tr("Annotations marking found similar sequence regions."));

        QMap<Descriptor, DataTypePtr> hmmM;
        hmmM[HMMLib::HMM2_SLOT()] = HMMLib::HMM_PROFILE_TYPE();
        p << new PortDescriptor(hd, DataTypePtr(new MapDataType("hmm.search.hmm", hmmM)), true /*input*/, false, IntegralBusPort::BLIND_INPUT);
        QMap<Descriptor, DataTypePtr> seqM;
        seqM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(sd, DataTypePtr(new MapDataType("hmm.search.sequence", seqM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("hmm.search.out", outM)), false /*input*/, true);
    }

    {
        Descriptor nd(NAME_ATTR, HMMSearchWorker::tr("Result annotation"), HMMSearchWorker::tr("A name of the result annotations."));
        Descriptor nsd(NSEQ_ATTR, HMMSearchWorker::tr("Number of seqs"), QApplication::translate("HMMSearchDialog", "Calculate the E-value scores as if we had seen a sequence database of &lt;n&gt; sequences.", 0));
        Descriptor ded(DOM_E_ATTR, HMMSearchWorker::tr("Filter by high E-value"), QApplication::translate("HMMSearchDialog", "E-value filtering can be used to exclude low-probability hits from result.", 0));
        Descriptor dtd(DOM_T_ATTR, HMMSearchWorker::tr("Filter by low score"), QApplication::translate("HMMSearchDialog", "Score based filtering is an alternative to E-value filtering to exclude low-probability hits from result.", 0));

        a << new Attribute(nd, BaseTypes::STRING_TYPE(), true, QVariant("hmm_signal"));
        a << new Attribute(nsd, BaseTypes::NUM_TYPE(), false, QVariant(1));
        a << new Attribute(ded, BaseTypes::NUM_TYPE(), false, QVariant(-1));
        a << new Attribute(dtd, BaseTypes::NUM_TYPE(), false, QVariant((double)-1e+09));
    }

    Descriptor desc(HMMSearchWorkerFactory::ACTOR, HMMSearchWorker::tr("HMM2 Search"),
        HMMSearchWorker::tr("Searches each input sequence for significantly similar sequence matches to all specified HMM profiles."
        " In case several profiles were supplied, searches with all profiles one by one and outputs united set of annotations for each sequence."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap eMap; eMap["prefix"]= ("1e"); eMap["minimum"] = (-99); eMap["maximum"] = (0);
        delegates[DOM_E_ATTR] = new SpinBoxDelegate(eMap);
    }
    {
        QVariantMap nMap; nMap["maximum"] = (INT_MAX);
        delegates[NSEQ_ATTR] = new SpinBoxDelegate(nMap);
    }
    {
        QVariantMap tMap; tMap["decimals"]= (1); tMap["minimum"] = (-1e+09); tMap["maximum"] = (1e+09);
        tMap["singleStep"] = (0.1);
        delegates[DOM_T_ATTR] = new DoubleSpinBoxDelegate(tMap);
    }

    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":/hmm2/images/hmmer_16.png");
    proto->setPrompter(new HMMSearchPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(HMMLib::HMM_CATEGORY(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new HMMSearchWorkerFactory());
}

static bool isDefaultCfg(PrompterBaseImpl* actor) {
    return 1 == actor->getParameter(NSEQ_ATTR).toInt()
        && -1 == actor->getParameter(DOM_E_ATTR).toInt()
        && double(-1e+09) == actor->getParameter(DOM_T_ATTR).toDouble();
}

/*******************************
 * HMMSearchPrompter
 *******************************/
QString HMMSearchPrompter::composeRichDoc() {
    Actor* hmmProducer = qobject_cast<IntegralBusPort*>(target->getPort(HMM_PORT))->getProducer(HMM_PORT);
    Actor* seqProducer = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()))->getProducer(BasePorts::IN_SEQ_PORT_ID());

    QString seqName = seqProducer ? tr("For each sequence from <u>%1</u>,").arg(seqProducer->getLabel()) : "";
    QString hmmName = hmmProducer ? tr("using all profiles provided by <u>%1</u>,").arg(hmmProducer->getLabel()) : "";

    QString resultName = getHyperlink(NAME_ATTR, getRequiredParam(NAME_ATTR));
    QString cfg = isDefaultCfg(this) ? tr("Use <u>default</u> settings.") : tr("Use <u>custom</u> settings.");

    QString doc = tr("%1 HMM signals%2. %3"
        "<br>Output the list of found regions annotated as <u>%4</u>.")
        .arg(seqName.isEmpty() ? "Search" : seqName + " search")
        .arg(hmmName.isEmpty() ? "" : " " + hmmName)
        .arg(cfg)
        .arg(resultName);

    return doc;
}

/*******************************
 * HMMSearchWorker
 *******************************/
HMMSearchWorker::HMMSearchWorker(Actor* a) : BaseWorker(a, false), hmmPort(NULL), seqPort(NULL), output(NULL) {
}

void HMMSearchWorker::init() {
    hmmPort = ports.value(HMM_PORT);
    seqPort = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
    seqPort->addComplement(output);
    output->addComplement(seqPort);

    float domENum = actor->getParameter(DOM_E_ATTR)->getAttributeValue<int>(context);
    if(domENum > 0) {
        algoLog.details(tr("Power of e-value must be less or equal to zero. Using default value: 1e-1"));
        domENum = -1;
    }
    cfg.domE = pow(10, domENum);

    cfg.domT = (float)actor->getParameter(DOM_T_ATTR)->getAttributeValue<double>(context);
    cfg.eValueNSeqs = actor->getParameter(NSEQ_ATTR)->getAttributeValue<int>(context);
    resultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
    if(resultName.isEmpty()){
        algoLog.details(tr("Value for attribute name is empty, default name used"));
        resultName = "hmm_signal";
    }
}

bool HMMSearchWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    bool seqEnded = seqPort->isEnded();
    bool hmmEnded = hmmPort->isEnded();
    int seqHasMes = seqPort->hasMessage();
    int hmmHasMes = hmmPort->hasMessage();
    return hmmHasMes || (hmmEnded && (seqHasMes || seqEnded));
}

Task* HMMSearchWorker::tick() {
    while (hmmPort->hasMessage()) {
        hmms << hmmPort->get().getData().toMap().value(HMM2_SLOT_ID).value<plan7_s*>();
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
        if (seqObj.isNull()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence dnaSequence = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if (dnaSequence.alphabet->getType() != DNAAlphabet_RAW) {
            QList<Task*> subtasks;
            foreach(plan7_s* hmm, hmms) {
                subtasks << new HMMSearchTask(hmm, dnaSequence, cfg);
            }
            Task* searchTask = new MultiTask(tr("Find HMM signals in %1").arg(dnaSequence.getName()), subtasks);
            connect(new TaskSignalMapper(searchTask), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
            return searchTask;
        }
        QString err = tr("Bad sequence supplied to input: %1").arg(dnaSequence.getName());
        return new FailTask(err);
    } if (seqPort->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void HMMSearchWorker::sl_taskFinished(Task *t) {
    SAFE_POINT(NULL != t, "Invalid task is encountered",);
    if (t->isCanceled()) {
        return;
    }
    if (NULL != output) {
        QList<SharedAnnotationData> list;
        foreach (const QPointer<Task> &sub, t->getSubtasks()) {
            HMMSearchTask *hst = qobject_cast<HMMSearchTask *>(sub.data());
            list += hst->getResultsAsAnnotations(U2FeatureTypes::MiscSignal, resultName);
        }

        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(list);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(tableId)));
        algoLog.info(tr("Found %1 HMM signals").arg(list.size()));
    }
}

void HMMSearchWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
