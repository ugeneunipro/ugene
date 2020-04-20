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

#include "BlastPlusWorker.h"
#include "TaskLocalStorage.h"
#include "BlastPlusSupport.h"
#include "BlastNPlusSupportTask.h"
#include "BlastPPlusSupportTask.h"
#include "BlastXPlusSupportTask.h"
#include "TBlastNPlusSupportTask.h"
#include "TBlastXPlusSupportTask.h"

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/FailTask.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {
namespace LocalWorkflow {

/****************************
 * BlastAllWorkerFactory
 ****************************/
const QString BlastPlusWorkerFactory::ACTOR_ID("blast-plus");

QString BlastPlusWorkerFactory::getHitsName() {
    return BlastPlusWorker::tr("Culling limit");
}

QString BlastPlusWorkerFactory::getHitsDescription() {
    return BlastPlusWorker::tr("If the query range of a hit is enveloped by that of at least this many higher-scoring hits, delete the hit");
}

#define BLASTPLUS_PROGRAM_NAME  "blast-type"
#define BLASTPLUS_DATABASE_PATH "db-path"
#define BLASTPLUS_DATABASE_NAME "db-name"
#define BLASTPLUS_EXPECT_VALUE  "e-val"
#define BLASTPLUS_COMP_STATS "comp-based-stats"
#define BLASTPLUS_MAX_HITS "max-hits"
#define BLASTPLUS_GROUP_NAME    "result-name"
#define BLASTPLUS_EXT_TOOL_PATH "tool-path"
#define BLASTPLUS_TMP_DIR_PATH  "temp-dir"
#define BLASTPLUS_GAP_COSTS_VALUE       "gap-costs"
#define BLASTPLUS_MATCH_SCORES_VALUE    "match-scores"

//Additional options
#define BLASTPLUS_ORIGINAL_OUT  "blast-output"  //path for output file
#define BLASTPLUS_OUT_TYPE      "type-output"   //original option -m 0-11
#define BLASTPLUS_GAPPED_ALN    "gapped-aln"    //Perform gapped alignment (not available with tblastx)

void BlastPlusWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), BlastPlusWorker::tr("Input sequence"),
        BlastPlusWorker::tr("Sequence for which annotations is searched."));
    Descriptor oud(BasePorts::OUT_ANNOTATIONS_PORT_ID(), BlastPlusWorker::tr("Annotations"), BlastPlusWorker::tr("Found annotations."));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("blast.plus.seq", inM)), true /*input*/);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
    p << new PortDescriptor(oud, DataTypePtr(new MapDataType("blast.plus.annotations", outM)), false /*input*/, true /*multi*/);

    Descriptor pn(BLASTPLUS_PROGRAM_NAME, BlastPlusWorker::tr("Search type"),
                   BlastPlusWorker::tr("Select type of BLAST+ searches."));
    Descriptor dp(BLASTPLUS_DATABASE_PATH, BlastPlusWorker::tr("Database Path"),
                   BlastPlusWorker::tr("Path with database files."));
    Descriptor dn(BLASTPLUS_DATABASE_NAME, BlastPlusWorker::tr("Database Name"),
                   BlastPlusWorker::tr("Base name for BLAST+ DB files."));
    Descriptor ev(BLASTPLUS_EXPECT_VALUE, BlastPlusWorker::tr("Expected value"),
                   BlastPlusWorker::tr("This setting specifies the statistical significance threshold for reporting matches against database sequences."));
    Descriptor cbs(BLASTPLUS_COMP_STATS, BlastPlusWorker::tr("Composition-based statistics"), BlastPlusWorker::tr("Composition-based statistics."));
    Descriptor mh(BLASTPLUS_MAX_HITS, getHitsName(), getHitsDescription());
    Descriptor gn(BLASTPLUS_GROUP_NAME, BlastPlusWorker::tr("Annotate as"),
                   BlastPlusWorker::tr("Name for annotations."));
    Descriptor etp(BLASTPLUS_EXT_TOOL_PATH, BlastPlusWorker::tr("Tool Path"),
                   BlastPlusWorker::tr("External tool path."));
    Descriptor tdp(BLASTPLUS_TMP_DIR_PATH, BlastPlusWorker::tr("Temporary folder"),
                   BlastPlusWorker::tr("Folder for temporary files."));
    Descriptor output(BLASTPLUS_ORIGINAL_OUT, BlastPlusWorker::tr("BLAST output"),
                   BlastPlusWorker::tr("Location of BLAST output file."));
    Descriptor outtype(BLASTPLUS_OUT_TYPE, BlastPlusWorker::tr("BLAST output type"),
                   BlastPlusWorker::tr("Type of BLAST output file."));
    Descriptor ga(BLASTPLUS_GAPPED_ALN, BlastPlusWorker::tr("Gapped alignment"),
                   BlastPlusWorker::tr("Perform gapped alignment."));

    Descriptor gc(BLASTPLUS_GAP_COSTS_VALUE, BlastPlusWorker::tr("Gap costs"),
                   BlastPlusWorker::tr("Cost to create and extend a gap in an alignment."));
    Descriptor ms(BLASTPLUS_MATCH_SCORES_VALUE, BlastPlusWorker::tr("Match scores"),
                   BlastPlusWorker::tr("Reward and penalty for matching and mismatching bases."));

    a << new Attribute(pn, BaseTypes::STRING_TYPE(), true, QVariant("blastn"));
    a << new Attribute(dp, BaseTypes::STRING_TYPE(), true, QVariant(""));
    a << new Attribute(dn, BaseTypes::STRING_TYPE(), true, QVariant(""));
    a << new Attribute(etp, BaseTypes::STRING_TYPE(), true, QVariant("default"));
    a << new Attribute(tdp, BaseTypes::STRING_TYPE(), true, QVariant("default"));
    a << new Attribute(ev, BaseTypes::NUM_TYPE(), false, QVariant(10.00));
    Attribute *cbsAttr = new Attribute(cbs, BaseTypes::STRING_TYPE(), false, "D");
    QVariantList cbsVisibilitylist;
    cbsVisibilitylist << "blastp" << "blastx" << "tblastn";
    cbsAttr->addRelation(new VisibilityRelation(BLASTPLUS_PROGRAM_NAME, cbsVisibilitylist));
    a << cbsAttr;
    a << new Attribute(mh, BaseTypes::NUM_TYPE(), false, QVariant(0));
    a << new Attribute(gn, BaseTypes::STRING_TYPE(), false, QVariant("blast_result"));

    Attribute* gaAttr= new Attribute(ga, BaseTypes::BOOL_TYPE(), false, QVariant(true));
    QVariantList gaVisibilitylist;
    gaVisibilitylist << "blastn" << "blastp" << "blastx" << "tblastn";
    gaAttr->addRelation(new VisibilityRelation(BLASTPLUS_PROGRAM_NAME, gaVisibilitylist));
    a << gaAttr;

    a << new Attribute(gc, BaseTypes::STRING_TYPE(), false, "2 2");

    Attribute* msAttr = new Attribute(ms, BaseTypes::STRING_TYPE(), false, "1 -3");
    QVariantMap scoresGapDependency = ExternalToolSupportUtils::getScoresGapDependencyMap();
    msAttr->addRelation(new ValuesRelation(BLASTPLUS_GAP_COSTS_VALUE, scoresGapDependency));
    a << msAttr;

    a << new Attribute(output, BaseTypes::STRING_TYPE(), false, QVariant(""));
    a << new Attribute(outtype, BaseTypes::STRING_TYPE(), false, QVariant("5"));

    Descriptor desc(ACTOR_ID, BlastPlusWorker::tr("Local BLAST+ Search"),
        BlastPlusWorker::tr("Finds annotations for DNA sequence in local database."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap m;
        m["blastn"] = "blastn";
        m["blastp"] = "blastp";
        m["blastx"] = "blastx";
        m["tblastn"] = "tblastn";
        m["tblastx"] = "tblastx";
        delegates[BLASTPLUS_PROGRAM_NAME] = new ComboBoxDelegate(m);
    }

    {
        QVariantMap m;
        m["minimum"] = 0.000001;
        m["maximum"] = 100000;
        m["singleStep"] = 1.0;
        m["decimals"] = 6;
        delegates[BLASTPLUS_EXPECT_VALUE] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["D or d: default (equivalent to 2)"] = "D";
        m["0 or F or f: No composition-based statistics"] = "0";
        m["1: Composition-based statistics as in NAR 29:2994-3005, 2001"] = "1";
        m["2 or T or t : Composition-based score adjustment as in Bioinformatics 21:902-911, 2005"] = "2";
        m["3: Composition-based score adjustment as in Bioinformatics 21:902-911, 2005"] = "3";
        delegates[BLASTPLUS_COMP_STATS] = new ComboBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = INT_MAX;
        delegates[BLASTPLUS_MAX_HITS] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["use"] = true;
        m["not use"] = false;
        delegates[BLASTPLUS_GAPPED_ALN] = new ComboBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["traditional pairwise (-outfmt 0)"] = 0;
//        m["query-anchored showing identities"] = 1;
//        m["query-anchored no identities"] = 2;
//        m["flat query-anchored, show identities"] = 3;
//        m["flat query-anchored, no identities"] = 4;
        m["XML (-outfmt 5)"] = 5;
        m["tabular (-outfmt 6)"] = 6;
//        m["tabular with comment lines"] = 7;
//        m["Text ASN.1"] = 8;
//        m["Binary ASN.1"] = 9;
//        m["Comma-separated values"] = 10;
//        m["BLAST archive format (ASN.1)"] = 11;
        delegates[BLASTPLUS_OUT_TYPE] = new ComboBoxDelegate(m);
    }

    {
        QVariantMap m;
        const QList <QString> matchValues = scoresGapDependency.keys();
        for (int i = 0; i < matchValues.size(); i++) {
            m[matchValues.at(i)] = matchValues.at(i);
        }
        delegates[BLASTPLUS_MATCH_SCORES_VALUE] = new ComboBoxDelegate(m);
    }

    {
        const QVariantMap m = scoresGapDependency.value("1 -3").toMap();
        delegates[BLASTPLUS_GAP_COSTS_VALUE] = new ComboBoxDelegate(m);
    }

    delegates[BLASTPLUS_ORIGINAL_OUT] = new URLDelegate("", "out file", false);
    delegates[BLASTPLUS_DATABASE_PATH] = new URLDelegate("", "Database Folder", false, true, false);
    delegates[BLASTPLUS_EXT_TOOL_PATH] = new URLDelegate("", "executable", false, false, false);
    delegates[BLASTPLUS_TMP_DIR_PATH] = new URLDelegate("", "TmpDir", false, true);

    proto->setEditor(new DelegateEditor(delegates));
    proto->setPrompter(new BlastPlusPrompter());
    proto->setIconPath(":external_tool_support/images/ncbi.png");
    proto->setValidator(new ToolsValidator());
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new BlastPlusWorkerFactory());
}

/****************************
* BlastPlusPrompter
****************************/
BlastPlusPrompter::BlastPlusPrompter(Actor* p) : PrompterBase<BlastPlusPrompter>(p) {
}
QString BlastPlusPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);
    QString doc = tr("For sequence <u>%1</u> find annotations in database <u>%2</u>.")
        .arg(producerName).arg(getHyperlink(BLASTPLUS_DATABASE_NAME, getRequiredParam(BLASTPLUS_DATABASE_NAME)));

    return doc;
}
/****************************
* BlastPlusWorker
****************************/
BlastPlusWorker::BlastPlusWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void BlastPlusWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* BlastPlusWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return NULL;
        }
        cfg.programName = getValue<QString>(BLASTPLUS_PROGRAM_NAME);
        cfg.databaseNameAndPath = getValue<QString>(BLASTPLUS_DATABASE_PATH) + "/"+ getValue<QString>(BLASTPLUS_DATABASE_NAME);
        cfg.isDefaultCosts = true;
        cfg.isDefaultMatrix = true;
        cfg.isDefautScores = true;
        cfg.expectValue = getValue<double>(BLASTPLUS_EXPECT_VALUE);
        if (actor->isAttributeVisible(actor->getParameter(BLASTPLUS_COMP_STATS))) {
            cfg.compStats = getValue<QString>(BLASTPLUS_COMP_STATS);
        }
        cfg.numberOfHits = getValue<int>(BLASTPLUS_MAX_HITS);
        cfg.groupName = getValue<QString>(BLASTPLUS_GROUP_NAME);
        if(cfg.groupName.isEmpty()){
            cfg.groupName="blast result";
        }
        cfg.wordSize=0;
        cfg.isGappedAlignment = getValue<bool>(BLASTPLUS_GAPPED_ALN);

        QString path = actor->getParameter(BLASTPLUS_EXT_TOOL_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            if(cfg.programName == "blastn"){
                AppContext::getExternalToolRegistry()->getById(BlastPlusSupport::ET_TBLASTN_ID)->setPath(path);
            }else if(cfg.programName == "blastp"){
                AppContext::getExternalToolRegistry()->getById(BlastPlusSupport::ET_BLASTP_ID)->setPath(path);
            }else if(cfg.programName == "blastx"){
                AppContext::getExternalToolRegistry()->getById(BlastPlusSupport::ET_BLASTX_ID)->setPath(path);
            }else if(cfg.programName == "tblastn"){
                AppContext::getExternalToolRegistry()->getById(BlastPlusSupport::ET_TBLASTN_ID)->setPath(path);
            }else if(cfg.programName == "tblastx"){
                AppContext::getExternalToolRegistry()->getById(BlastPlusSupport::ET_TBLASTX)->setPath(path);
            }
        }
        path = actor->getParameter(BLASTPLUS_TMP_DIR_PATH)->getAttributeValue<QString>(context);
        if(QString::compare(path, "default", Qt::CaseInsensitive) != 0){
            AppContext::getAppSettings()->getUserAppsSettings()->setUserTemporaryDirPath(path);
        }

        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return NULL;
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        if(seq.length() < 1) {
            return new FailTask(tr("Empty sequence supplied to BLAST"));
        }
        cfg.querySequence=seq.seq;
        cfg.isSequenceCircular = seq.circular;

        const DNAAlphabet *alp = U2AlphabetUtils::findBestAlphabet(seq.seq);
        cfg.alphabet=alp;
        //TO DO: Check alphabet
        if(seq.alphabet->isAmino()) {
            if(cfg.programName == "blastn" || cfg.programName == "blastx" || cfg.programName == "tblastx") {
                return new FailTask(tr("Selected BLAST search with nucleotide input sequence"));
            }
        }
        else {
            if(cfg.programName == "blastp" || cfg.programName == "tblastn") {
                return new FailTask(tr("Selected BLAST search with amino acid input sequence"));
            }
        }
        cfg.needCreateAnnotations = false;
        cfg.outputType = getValue<int>(BLASTPLUS_OUT_TYPE);
        cfg.outputOriginalFile = getValue<QString>(BLASTPLUS_ORIGINAL_OUT);
        if(cfg.outputType != 5 && cfg.outputOriginalFile.isEmpty()){
            return new FailTask(tr("Not selected BLAST output file"));
        }

        if(cfg.programName == "blastn"){
            cfg.megablast = true;
            cfg.wordSize = 28;
            cfg.windowSize = 0;
        }else{
            cfg.megablast = false;
            cfg.wordSize = 3;
            cfg.windowSize  = 40;
        }
        //set X dropoff values
        if(cfg.programName == "blastn"){
            cfg.xDropoffFGA = 100;
            cfg.xDropoffGA = 20;
            cfg.xDropoffUnGA = 10;
        }else if (cfg.programName == "tblastx"){
            cfg.xDropoffFGA = 0;
            cfg.xDropoffGA = 0;
            cfg.xDropoffUnGA = 7;
        }else{
            cfg.xDropoffFGA = 25;
            cfg.xDropoffGA = 15;
            cfg.xDropoffUnGA = 7;
        }

        QString gapCosts = getValue<QString>(BLASTPLUS_GAP_COSTS_VALUE);
        cfg.gapOpenCost = gapCosts.split(" ").at(0).toInt();
        cfg.gapExtendCost = gapCosts.split(" ").at(1).toInt();
        QString matchScores = getValue<QString>(BLASTPLUS_MATCH_SCORES_VALUE);
        cfg.matchReward = matchScores.split(" ").at(0).toInt();
        cfg.mismatchPenalty = matchScores.split(" ").at(1).toInt();

        ExternalToolSupportTask *task = nullptr;
        if (cfg.programName == "blastn") {
            task = new BlastNPlusSupportTask(cfg);
        } else if (cfg.programName == "blastp") {
            task = new BlastPPlusSupportTask(cfg);
        } else if (cfg.programName == "blastx") {
            task = new BlastXPlusSupportTask(cfg);
        } else if (cfg.programName == "tblastn") {
            task = new TBlastNPlusSupportTask(cfg);
        } else if (cfg.programName == "tblastx") {
            task = new TBlastXPlusSupportTask(cfg);
        }
        SAFE_POINT(nullptr != task, QString("An unknown program name: %1").arg(cfg.programName), new FailTask(QString("An unknown program name: %1").arg(cfg.programName)));
        task->addListeners(createLogListeners());
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return task;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return NULL;
}

void BlastPlusWorker::sl_taskFinished() {
    BlastPlusSupportCommonTask *t = qobject_cast<BlastPlusSupportCommonTask *>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()) {
        return;
    }

    if (NULL != output) {
        QList<SharedAnnotationData> res = t->getResultedAnnotations();
        QString annName = actor->getParameter(BLASTPLUS_GROUP_NAME)->getAttributeValue<QString>(context);
        if (!annName.isEmpty()) {
            for (int i = 0; i < res.count(); i++) {
                res[i]->name = annName;
            }
        }
        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(res);
        const QVariant v = qVariantFromValue<SharedDbiDataHandler>(tableId);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), v));
    }
}

void BlastPlusWorker::cleanup() {

}

/************************************************************************/
/* Validator */
/************************************************************************/
bool ToolsValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &/*options*/) const {
    ExternalTool *tool = getTool(getValue<QString>(actor, BLASTPLUS_PROGRAM_NAME));
    SAFE_POINT(NULL != tool, "NULL blast plus tool", false);

    Attribute *attr = actor->getParameter(BLASTPLUS_EXT_TOOL_PATH);
    SAFE_POINT(NULL != attr, "NULL blastplus path attribute", false);

    bool valid = attr->isDefaultValue() ? !tool->getPath().isEmpty() : !attr->isEmpty();
    if (!valid) {
        notificationList << WorkflowNotification(WorkflowUtils::externalToolError(tool->getName()));
    }
    return valid;
}

ExternalTool * ToolsValidator::getTool(const QString &program) const {
    QString toolId = BlastPlusSupportCommonTask::toolIdByProgram(program);
    return AppContext::getExternalToolRegistry()->getById(toolId);
}

} //namespace LocalWorkflow
} //namespace U2
