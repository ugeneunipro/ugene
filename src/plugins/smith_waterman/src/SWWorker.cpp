/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/SWResultFilterRegistry.h>
#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAInfo.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/PluginModel.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

#include "SWWorker.h"

namespace U2 {
namespace LocalWorkflow {

/**************************
 * SWWorkerFactory
 **************************/
static const QString NAME_ATTR("result-name");
static const QString PATTERN_ATTR("pattern");
static const QString SCORE_ATTR("min-score");
static const QString MATRIX_ATTR("matrix");
static const QString AMINO_ATTR("amino");
static const QString ALGO_ATTR("algorithm");
static const QString FILTER_ATTR("filter-strategy");
static const QString GAPOPEN_ATTR("gap-open-score");
static const QString GAPEXT_ATTR("gap-ext-score");
static const QString USE_PATTERN_NAME_ATTR("use-pattern-names");
static const QString PATTERN_NAME_QUAL_ATTR("pattern-name-qual");

const QString SWWorkerFactory::ACTOR_ID("ssearch");

static const QString PATTERN_PORT("pattern");
static const QString PATTERN_DELIMITER(";");

void SWAlgoEditor::populate() {
    QStringList algoLst = AppContext::getSmithWatermanTaskFactoryRegistry()->getListFactoryNames();
    if (algoLst.isEmpty()) {
        return;
    }
    foreach(const QString& n, algoLst) {
        comboItems.append(qMakePair(n,n));
    }
    QList<Attribute*> lst = proto->getAttributes();
    foreach(Attribute* a, lst) {
        if (a->getId() == ALGO_ATTR) {
            a->setAttributeValue(algoLst.first());
            break;
        }
    }
}

void SWWorkerFactory::init() {
    QList<PortDescriptor*> p;
    {
        Descriptor patd(PATTERN_PORT,
                       SWWorker::tr("Pattern Data"),
                       SWWorker::tr("An input pattern sequence to search for."));

        Descriptor ind(BasePorts::IN_SEQ_PORT_ID(),
            SWWorker::tr("Input Data"),
            SWWorker::tr("An input reference sequence to search in."));

        Descriptor oud(BasePorts::OUT_ANNOTATIONS_PORT_ID(),
            SWWorker::tr("Pattern Annotations"),
            SWWorker::tr("The regions found."));

        QMap<Descriptor, DataTypePtr> patM;
        patM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(patd, DataTypePtr(new MapDataType("sw.pattern", patM)), true);

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(ind, DataTypePtr(new MapDataType("sw.sequence", inM)), true);

        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(oud, DataTypePtr(new MapDataType("sw.annotations", outM)), false, true);
    }

    QStringList filterLst = AppContext::getSWResultFilterRegistry()->getFiltersIds();

    QList<Attribute*> a;
    {
        Descriptor nd(NAME_ATTR,
            SWWorker::tr("Annotate as"),
            SWWorker::tr("Name of the result annotations."));

        Descriptor scd(SCORE_ATTR,
            SWWorker::tr("Min Score"),
            SWWorker::tr("Minimal percent similarity between a sequence and a pattern."));

        Descriptor ald(ALGO_ATTR,
            SWWorker::tr("Algorithm"),
            SWWorker::tr("Version of the Smith-Waterman algorithm. You can use the optimized"
                " versions of the algorithm (SSE, CUDA and OpenCL) if your hardware supports"
                " these capabilities."));

        Descriptor amd(AMINO_ATTR,
            SWWorker::tr("Search in Translation"),
            SWWorker::tr("Translates a supplied nucleotide sequence to protein"
                " and searches in the translated sequence."));

        Descriptor mxd(MATRIX_ATTR,
            SWWorker::tr("Substitution Matrix"),
            SWWorker::tr("Describes the rate at which one character in a sequence changes"
                " to other character states over time."));

        Descriptor frd(FILTER_ATTR,
            SWWorker::tr("Filter Results"),
            SWWorker::tr("Specifies either to filter the intersected results or to return all"
                " the results."));

        Descriptor god(GAPOPEN_ATTR,
            SWWorker::tr("Gap Open Score"),
            SWWorker::tr("Penalty for opening a gap."));

        Descriptor ged(GAPEXT_ATTR,
            SWWorker::tr("Gap Extension Score"),
            SWWorker::tr("Penalty for extending a gap."));

        Descriptor pnd(USE_PATTERN_NAME_ATTR,
                       SWWorker::tr("Use Pattern Names"),
                       SWWorker::tr("Use a pattern name as an annotation name."));

        Descriptor patternNameQualifier(PATTERN_NAME_QUAL_ATTR,
                        SWWorker::tr("Qualifier name for pattern name"),
                        SWWorker::tr("Name of qualifier in result annotations which is containing "
                        "a pattern name."));

        a << new Attribute(mxd, BaseTypes::STRING_TYPE(), true, QString("Auto"));
        a << new Attribute(ald, BaseTypes::STRING_TYPE(), true);
        a << new Attribute(frd, BaseTypes::STRING_TYPE(), false, filterLst.isEmpty() ? QString() : filterLst.first());
        a << new Attribute(scd, BaseTypes::NUM_TYPE(), false, 90);
        a << new Attribute(BaseAttributes::STRAND_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, BaseAttributes::STRAND_BOTH());
        a << new Attribute(amd, BaseTypes::BOOL_TYPE(), false, false);
        a << new Attribute(god, BaseTypes::NUM_TYPE(), false, -10.);
        a << new Attribute(ged, BaseTypes::NUM_TYPE(), false, -1.);
        a << new Attribute(pnd, BaseTypes::BOOL_TYPE(), false, true);
        a << new Attribute(nd, BaseTypes::STRING_TYPE(), false, "misc_feature");
        a << new Attribute(patternNameQualifier, BaseTypes::STRING_TYPE(), false, "pattern_name");
    }

    Descriptor desc(ACTOR_ID,
        SWWorker::tr("Smith-Waterman Search"),
        SWWorker::tr("Searches regions in a sequence similar to a pattern"
            " sequence. Outputs a set of annotations. <p>Under the hood is"
            " the well-known Smith-Waterman algorithm for performing local"
            " sequence alignment."));

    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    // delegates
    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m; m["minimum"] = 1; m["maximum"] = 100; m["suffix"] = "%";
        delegates[SCORE_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["maximum"] = -0.; m["minimum"]=-10000000.;
        delegates[GAPOPEN_ATTR] = new DoubleSpinBoxDelegate(m);
        m["maximum"] = -1.;
        delegates[GAPEXT_ATTR] = new DoubleSpinBoxDelegate(m);
    }
    delegates[BaseAttributes::STRAND_ATTRIBUTE().getId()] = new ComboBoxDelegate(BaseAttributes::STRAND_ATTRIBUTE_VALUES_MAP());
    {
        QVariantMap m;
        foreach(const QString& n, filterLst) {
            m.insert(n,n);
        }
        delegates[FILTER_ATTR] = new ComboBoxDelegate(m);
    }
    {
        QVariantMap m; m.insert(SWWorker::tr("Auto"), QString("Auto"));
        QStringList lst = AppContext::getSubstMatrixRegistry()->getMatrixNames();
        foreach(const QString& n, lst) {
            m.insert(n,n);
        }
        delegates[MATRIX_ATTR] = new ComboBoxDelegate(m);
    }

    SWAlgoEditor* aled = new SWAlgoEditor(proto);
    aled->connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(populate()));
    delegates[ALGO_ATTR] = aled;
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":core/images/sw.png");
    proto->setPrompter(new SWPrompter());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new SWWorkerFactory());
}

static StrandOption getStrand(const QString & s) {
    QString str = s.toLower();
    if(BaseAttributes::STRAND_BOTH().startsWith(str)) {
        return StrandOption_Both;
    } else if(BaseAttributes::STRAND_DIRECT().startsWith(str)) {
        return StrandOption_DirectOnly;
    } else if(BaseAttributes::STRAND_COMPLEMENTARY().startsWith(str)) {
        return StrandOption_ComplementOnly;
    } else {
        bool ok = false;
        int num = str.toInt(&ok);
        if(ok && num >= 0) {
            return StrandOption(num);
        } else {
            return StrandOption_Both;
        }
    }
}

/**************************
 * SWPrompter
 **************************/
QString SWPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* seqProducer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    IntegralBusPort* patternPort = qobject_cast<IntegralBusPort*>(target->getPort(PATTERN_PORT));
    Actor* ptrnProducer = patternPort->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";

    QString seqName;
    seqProducer ? (seqName = seqProducer->getLabel()) : (seqName = unsetStr);

    QString ptrnName;
    ptrnProducer ? (ptrnName = ptrnProducer->getLabel()) : (ptrnName = unsetStr);

    SmithWatermanSettings cfg;
    cfg.strand = getStrand(getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId()).value<QString>());
    cfg.percentOfScore = getParameter(SCORE_ATTR).toInt();

    QString strandName;
    switch (cfg.strand) {
        case StrandOption_Both:
            strandName = SWWorker::tr("both strands");
            break;
        case StrandOption_DirectOnly:
            strandName = SWWorker::tr("direct strand");
            break;
        case StrandOption_ComplementOnly:
            strandName = SWWorker::tr("complementary strand");
            break;
        default:
            assert(false);
    }

    QString searchInTranslationSelected = "";
    if (getParameter(AMINO_ATTR).toBool()) {
        searchInTranslationSelected = "<u>" + tr("translated") + "</u>" + " ";
    }

    QString resultName = getParameter(NAME_ATTR).toString();
    if (resultName == "") {
        resultName = "misc_feature";
    }

    bool usePatternNames = getParameter(USE_PATTERN_NAME_ATTR).toBool();

    QString doc = tr("Searches regions in each sequence from <u>%1</u>"
        " similar to all pattern(s) taken from <u>%2</u>. <br/>Percent similarity between"
        " a sequence and a pattern is %3. <br/>Seaches in %4"
        " of a %5 sequence. <br/>Outputs the regions found annotated as %6.")
        .arg(seqName)
        .arg(ptrnName)
        .arg(getHyperlink(SCORE_ATTR, QString::number(cfg.percentOfScore) + "%"))
        .arg(getHyperlink(BaseAttributes::STRAND_ATTRIBUTE().getId(), strandName))
        .arg(getHyperlink(AMINO_ATTR, searchInTranslationSelected))
        .arg(usePatternNames ? "pattern names" : getHyperlink(NAME_ATTR, resultName));

    return doc;
}

/**************************
 * SWWorker
 **************************/
SWWorker::SWWorker(Actor* a) : BaseWorker(a, false), input(NULL), output(NULL) {
}

void SWWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    patternPort = ports.value(PATTERN_PORT);
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());

    input->addComplement(output);
    output->addComplement(input);
}

bool SWWorker::isReady() const {
    if (isDone()) {
        return false;
    }
    bool inputEnded = input->isEnded();
    bool patternEnded = patternPort->isEnded();
    int inputHasMes = input->hasMessage();
    int patternHasMes = patternPort->hasMessage();
    return patternHasMes || (patternEnded && (inputHasMes || inputEnded));
}

Task* SWWorker::tick() {
    while (patternPort->hasMessage()) {
        SharedDbiDataHandler ptrnId = patternPort->get().getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> ptrnObj(StorageUtils::getSequenceObject(context->getDataStorage(), ptrnId));
        if (NULL == ptrnObj.data()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence ptrn = ptrnObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if(ptrn.isNull()) {
            return new FailTask(tr("Null pattern supplied to Smith-Waterman: %1").arg(ptrn.getName()));
        }
        patternList << ptrn.constData();
        patternNames.insert(ptrn.constData(), ptrn.getName());
    }
    if (!patternPort->isEnded()) {
        return NULL;
    }

    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        SmithWatermanSettings cfg;

        // sequence
        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (NULL == seqObj.data()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if(seq.isNull()) {
            return new FailTask(tr("Null sequence supplied to Smith-Waterman: %1").arg(seq.getName()));
        }
        // scoring matrix
        QString mtrx = actor->getParameter(MATRIX_ATTR)->getAttributeValue<QString>(context);
        if(mtrx.isEmpty()){
            mtrx = "Auto";
        }
        cfg.pSm = AppContext::getSubstMatrixRegistry()->getMatrix(mtrx);
        if(cfg.pSm.getName().isEmpty() && mtrx.toLower() != "auto") {
            algoLog.details(tr("Invalid value: weight matrix with given name not exists"));
            return new FailTask(tr("Invalid value: weight matrix with given name not exists"));
        }
        if (cfg.pSm.isEmpty()) {
            QString matrixName;
            QStringList lst = AppContext::getSubstMatrixRegistry()->selectMatrixNamesByAlphabet(seq.alphabet);
            if (!lst.isEmpty()) {
                matrixName = lst.first();
                cfg.pSm = AppContext::getSubstMatrixRegistry()->getMatrix(matrixName);
            }
            if(cfg.pSm.isEmpty()) {
                return new FailTask(tr("Can't find weight matrix name: '%1'!").arg(matrixName.isEmpty() ? tr("<empty>") : matrixName));
            }
        }

        if(mtrx.toLower() != "auto") {
            QByteArray alphChars = seq.alphabet->getAlphabetChars();
            if(!cfg.pSm.getAlphabet()->containsAll(alphChars.constData(), alphChars.length())) {
                return new FailTask(tr("Wrong matrix selected. Alphabets do not match"));
            }
        }

        cfg.sqnc = QByteArray(seq.constData(), seq.length());
        cfg.globalRegion.length = seq.length();

        // score and gap model
        cfg.percentOfScore = actor->getParameter(SCORE_ATTR)->getAttributeValue<int>(context);
        if(cfg.percentOfScore < 0 || cfg.percentOfScore > 100){
            algoLog.error(tr("Incorrect value: score value must lay between 0 and 100"));
            return new FailTask(tr("Incorrect value: score value must lay between 0 and 100"));
        }

        cfg.gapModel.scoreGapExtd = actor->getParameter(GAPEXT_ATTR)->getAttributeValue<double>(context);
        cfg.gapModel.scoreGapOpen = actor->getParameter(GAPOPEN_ATTR)->getAttributeValue<double>(context);

        // filter
        QString filter = actor->getParameter(FILTER_ATTR)->getAttributeValue<QString>(context);
        cfg.resultFilter = AppContext::getSWResultFilterRegistry()->getFilter(filter);
        if(cfg.resultFilter == NULL){
            algoLog.error(tr("Incorrect value:  filter name incorrect, default value used")); //details level won't work
            cfg.resultFilter = AppContext::getSWResultFilterRegistry()->getFilter("none");
        }

        // annotation name
        QString defaultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
        if(defaultName.isEmpty()){
            algoLog.error(tr("Incorrect value: result name is empty, default value used")); //details level won't work
            defaultName = "misc_feature";
        }

        // translations
        cfg.strand = getStrand(actor->getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
        if (cfg.strand != StrandOption_DirectOnly) {
            DNATranslation* compTT = NULL;
            if (seq.alphabet->isNucleic()) {
                compTT = AppContext::getDNATranslationRegistry()->
                    lookupComplementTranslation(seq.alphabet);
            }
            if (compTT != NULL) {
                cfg.complTT = compTT;
            } else {
                algoLog.error(tr("Could not find complement translation for %1, searching only direct strand").arg(seq.getName()));
                cfg.strand = StrandOption_DirectOnly;
            }
        }
        if (actor->getParameter(AMINO_ATTR)->getAttributeValue<bool>(context)) {
            DNATranslationType tt = seq.alphabet->getType() == DNAAlphabet_NUCL ? DNATranslationType_NUCL_2_AMINO : DNATranslationType_RAW_2_AMINO;
            QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->lookupTranslation(seq.alphabet, tt);
            if (!TTs.isEmpty()) { //FIXME let user choose or use hints ?
                    cfg.aminoTT = AppContext::getDNATranslationRegistry()->getStandardGeneticCodeTranslation(seq.alphabet);
            }
        }

        // algorithm
        QString algName = actor->getParameter(ALGO_ATTR)->getAttributeValue<QString>(context);
        SmithWatermanTaskFactory* algo = AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(algName);
        if (!algo) {
            return new FailTask(tr("SmithWaterman algorithm not found: %1").arg(algName));
        }

        // for each pattern run smith-waterman
        if(patternList.isEmpty()) {
            algoLog.error(tr("Incorrect value: search pattern, pattern is empty"));
            return new FailTask(tr("Incorrect value: search pattern, pattern is empty"));
        }
        QList<Task*> subs;
        foreach(QByteArray p, patternList) {
            if(!cfg.pSm.getAlphabet()->containsAll(p.constData(), p.length())) {
                algoLog.error(tr("Incorrect value: pattern alphabet doesn't match sequence alphabet "));
                return new FailTask(tr("Pattern symbols not matching to alphabet"));
            }

            assert(!p.isEmpty());
            SmithWatermanSettings config(cfg);
            config.ptrn = p;

            QString resultName = actor->getParameter(USE_PATTERN_NAME_ATTR)
                ->getAttributeValue<bool>(context) ? patternNames.value(p, defaultName)
                : defaultName;
            SmithWatermanReportCallbackAnnotImpl* rcb = new SmithWatermanReportCallbackAnnotImpl(
                NULL, U2FeatureTypes::MiscFeature, resultName, QString(), "", false);
            config.resultCallback = rcb;
            config.resultListener = new SmithWatermanResultListener();

            Task * swTask = algo->getTaskInstance(config, tr("smith_waterman_task"));
            callbacks.insert(swTask, rcb);
            patterns.insert(swTask, config.ptrn);
            subs << swTask;
        }
        assert(!subs.isEmpty());

        MultiTask * multiSw = new MultiTask(tr("Smith waterman subtasks"), subs);
        connect(new TaskSignalMapper(multiSw), SIGNAL(si_taskFinished(Task*)), SLOT(sl_taskFinished(Task*)));
        return multiSw;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void SWWorker::sl_taskFinished(Task* t) {
    QList<SharedAnnotationData> annData;
    MultiTask * multiSw = qobject_cast<MultiTask*>(t);
    SAFE_POINT(NULL != t, "Invalid task is encountered",);
    QList<Task*> subs = multiSw->getTasks();
    SAFE_POINT(!subs.isEmpty(), "Invalid task is encountered",);
    QStringList ptrns;
    foreach(Task * sub, subs) {
        SAFE_POINT(NULL != sub, "Invalid task is encountered",);
        if (sub->isCanceled()) {
            return;
        }
        SmithWatermanReportCallbackAnnotImpl* rcb = callbacks.take(sub);
        assert(rcb != NULL);
        if(rcb) {
            // crop long names
            const QString qualifierName = actor->getParameter(PATTERN_NAME_QUAL_ATTR)->getAttributeValue<QString>(context);
            foreach(SharedAnnotationData a, rcb->getAnotations()) {
                QString pattern = patterns.value(sub);
                if(!patternNames[pattern].isEmpty()) {
                    a->qualifiers.push_back(U2Qualifier(qualifierName, patternNames[pattern]));
                }
                annData << a;
            }
        }
        ptrns << patterns.value(sub);
    }

    assert(output != NULL);
    if (NULL != output) {
        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(annData);
        const QVariant v = qVariantFromValue<SharedDbiDataHandler>(tableId);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
        algoLog.info(tr("Found %1 matches of pattern '%2'").arg(annData.size()).arg(ptrns.join(PATTERN_DELIMITER)));
    }
}

void SWWorker::cleanup() {
}

} //namespace LocalWorkflow
} //namespace U2
