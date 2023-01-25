/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "RemoteBLASTWorker.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>

namespace U2 {
namespace LocalWorkflow {

const QString RemoteBLASTWorkerFactory::ACTOR_ID("blast-ncbi");

const QString ANNOTATION_NAME("result-name");
const QString ORIGINAL_OUT("blast-output");

void RemoteBLASTWorkerFactory::init() {
    QList<PortDescriptor*> p;
    QList<Attribute*> a;
    Descriptor ind(BasePorts::IN_SEQ_PORT_ID(), RemoteBLASTWorker::tr("Input sequence"), RemoteBLASTWorker::tr("The sequence to search the annotations for"));
    Descriptor outd(BasePorts::OUT_ANNOTATIONS_PORT_ID(), RemoteBLASTWorker::tr("Annotations"), RemoteBLASTWorker::tr("Found annotations"));

    QMap<Descriptor, DataTypePtr> inM;
    inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
    p << new PortDescriptor(ind, DataTypePtr(new MapDataType("blast.ncbi.sequence", inM)), true);
    QMap<Descriptor, DataTypePtr> outM;
    outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
    p << new PortDescriptor(outd, DataTypePtr(new MapDataType("blast.ncbi.annotations", outM)), false, true);

    Descriptor programDescriptor(PROGRAM_ATTR, RemoteBLASTWorker::tr("Program"), RemoteBLASTWorker::tr("Select program. Available programs are 'ncbi-blastn', 'ncbi-blastp' and 'ncbi-cdd'."));
    Descriptor databaseDescriptor(DATABASE_ATTR, RemoteBLASTWorker::tr("Database"), RemoteBLASTWorker::tr("Select database to search. Example: 'nr', 'est_human', 'refseq_protein'"));
    Descriptor evalueDescriptor(EVALUE_ATTR, RemoteBLASTWorker::tr("Expected value"), RemoteBLASTWorker::tr("This parameter specifies the statistical significance threshold of reporting matches against the database sequences."));
    Descriptor hitsDescriptor(HITS_ATTR, RemoteBLASTWorker::tr("Results limit"), RemoteBLASTWorker::tr("Maximum number of results."));
    Descriptor mbDescriptor(MEGABLAST_ATTR, RemoteBLASTWorker::tr("Megablast"), RemoteBLASTWorker::tr("Enables megablast search."));
    Descriptor shortSeqDescriptor(SHORTSEQ_ATTR, RemoteBLASTWorker::tr("Short sequence"), RemoteBLASTWorker::tr("Optimize search for short sequences. Valid values: 'true', 'false', '1', '0'"));
    Descriptor entrezQueryDescriptor(ENTREZ_QUERY_ATTR, RemoteBLASTWorker::tr("Entrez query"), RemoteBLASTWorker::tr("An Entrez query to limit search."));
    Descriptor annotateAsDescriptor(ANNOTATION_NAME, RemoteBLASTWorker::tr("Annotate as"), RemoteBLASTWorker::tr("Result annotation name."));
    Descriptor outputDescriptor(ORIGINAL_OUT, RemoteBLASTWorker::tr("BLAST output"), RemoteBLASTWorker::tr("Location of the BLAST output file. Not used for ncbi-cdd search."));

    Descriptor gapCosts(GAP_ATTR, RemoteBLASTWorker::tr("Gap costs"), RemoteBLASTWorker::tr("Cost to create and extend a gap in an alignment."));
    Descriptor matchScores(MATCHSCORE_ATTR, RemoteBLASTWorker::tr("Match scores"), RemoteBLASTWorker::tr("Reward and penalty for matching and mismatching bases."));

    auto programNameAttr = new Attribute(programDescriptor, BaseTypes::STRING_TYPE(), true, "ncbi-blastn");
    QVariantMap matchscoreRelations;
    addParametersSetToMap(matchscoreRelations, "ncbi-blastn", ParametersLists::blastn_scores);
    addParametersSetToMap(matchscoreRelations, "ncbi-blastp", QStringList("Unavailable"));
    addParametersSetToMap(matchscoreRelations, "ncbi-cdd", QStringList("Unavailable"));
    programNameAttr->addRelation(new ValuesRelation(MATCHSCORE_ATTR, matchscoreRelations));

    QVariantMap databaseRelations;
    addParametersSetToMap(databaseRelations, "ncbi-blastn", ParametersLists::blastn_dataBase);
    addParametersSetToMap(databaseRelations, "ncbi-blastp", ParametersLists::blastp_dataBase);
    addParametersSetToMap(databaseRelations, "ncbi-cdd", ParametersLists::cdd_dataBase);
    programNameAttr->addRelation(new ValuesRelation(DATABASE_ATTR, databaseRelations));

    a << programNameAttr;

    const int defaultDatabaseNumber = 2;
    SAFE_POINT(defaultDatabaseNumber < ParametersLists::blastn_dataBase.size(), QObject::tr("Incorrect list of Blastn databases"), );
    a << new Attribute(databaseDescriptor, BaseTypes::STRING_TYPE(), true, ParametersLists::blastn_dataBase.at(defaultDatabaseNumber));

    a << new Attribute(evalueDescriptor, BaseTypes::STRING_TYPE(), false, 10);

    QVariantList notCddTools;
    notCddTools << "ncbi-blastn"
                << "ncbi-blastp";

    a << new Attribute(hitsDescriptor, BaseTypes::NUM_TYPE(), false, 10);
    auto mbAttr = new Attribute(mbDescriptor, BaseTypes::BOOL_TYPE(), false, false);
    mbAttr->addRelation(new VisibilityRelation(PROGRAM_ATTR, "ncbi-blastn"));
    a << mbAttr;

    auto shortSeqAttr = new Attribute(shortSeqDescriptor, BaseTypes::BOOL_TYPE(), true, false);
    shortSeqAttr->addRelation(new VisibilityRelation(PROGRAM_ATTR, notCddTools));
    a << shortSeqAttr;

    auto entrezQueryAttr = new Attribute(entrezQueryDescriptor, BaseTypes::STRING_TYPE(), false);
    entrezQueryAttr->addRelation(new VisibilityRelation(PROGRAM_ATTR, notCddTools));
    a << entrezQueryAttr;
    a << new Attribute(annotateAsDescriptor, BaseTypes::STRING_TYPE(), false);
    a << new Attribute(outputDescriptor, BaseTypes::STRING_TYPE(), false);

    auto gapAttr = new Attribute(gapCosts, BaseTypes::STRING_TYPE(), false, "2 2");
    gapAttr->addRelation(new VisibilityRelation(PROGRAM_ATTR, notCddTools));
    a << gapAttr;

    auto msAttr = new Attribute(matchScores, BaseTypes::STRING_TYPE(), false, "1 -3");
    QVariantMap scoresGapDependency = ExternalToolSupportUtils::getScoresGapDependencyMap();
    addParametersSetToMap(scoresGapDependency, "Unavailable", ParametersLists::blastp_gapCost);

    msAttr->addRelation(new ValuesRelation(GAP_ATTR, scoresGapDependency));
    msAttr->addRelation(new VisibilityRelation(PROGRAM_ATTR, "ncbi-blastn"));
    a << msAttr;

    Descriptor desc(ACTOR_ID, RemoteBLASTWorker::tr("Remote BLAST"), RemoteBLASTWorker::tr("Finds annotations for DNA sequence in remote database."));
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap m;
        m["minimum"] = 1;
        m["maximum"] = 5000;
        delegates[HITS_ATTR] = new SpinBoxDelegate(m);
    }

    {
        QVariantMap m;
        m["ncbi-blastn"] = "ncbi-blastn";
        m["ncbi-blastp"] = "ncbi-blastp";
        m["ncbi-cdd"] = "ncbi-cdd";
        delegates[PROGRAM_ATTR] = new ComboBoxDelegate(m);
    }

    {
        QVariantMap m;
        foreach (const QString& curStr, ParametersLists::blastn_dataBase) {
            m[curStr] = curStr;
        }
        delegates[DATABASE_ATTR] = new ComboBoxDelegate(m);
    }

    {
        QVariantMap m;
        m["minimum"] = 0.000001;
        m["maximum"] = 100000;
        m["singleStep"] = 1.0;
        m["decimals"] = 6;
        delegates[EVALUE_ATTR] = new DoubleSpinBoxDelegate(m);
    }

    {
        QVariantMap m;
        const QList<QString> matchValues = scoresGapDependency.keys();
        for (const auto& matchValue : qAsConst(matchValues)) {
            m[matchValue] = matchValue;
        }
        delegates[MATCHSCORE_ATTR] = new ComboBoxDelegate(m);
        ;
    }

    {
        const QVariantMap m = scoresGapDependency.value("1 -3").toMap();
        delegates[GAP_ATTR] = new ComboBoxDelegate(m);
    }

    delegates[ORIGINAL_OUT] = new URLDelegate("(*.xml)", "xml file");

    proto->setPrompter(new RemoteBLASTPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":remote_blast/images/remote_db_request.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new RemoteBLASTWorkerFactory());
}

QString RemoteBLASTPrompter::composeRichDoc() {
    auto input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    SAFE_POINT(input != nullptr, "Not a IntegralBusPort", "Internal error");
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>" + tr("unset") + "</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    QString doc = tr("For sequence %1 find annotations in database <u>%2</u>.")
                      .arg(producerName)
                      .arg(getHyperlink(PROGRAM_ATTR, getRequiredParam(PROGRAM_ATTR)));
    return doc;
}

void RemoteBLASTWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* RemoteBLASTWorker::tick() {
    if (getValue<QString>(ANNOTATION_NAME).isEmpty()) {
        algoLog.details(tr("Annotations name is empty, default name used"));
    }

    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->transit();
            return nullptr;
        }
        cfg.dbChoosen = getValue<QString>(PROGRAM_ATTR).split("-").last().toLower();
        if (cfg.dbChoosen != "blastn" && cfg.dbChoosen != "blastp" && cfg.dbChoosen != "cdd") {
            return new FailTask(tr("Incorrect '%1' parameter value: %2").arg(PROGRAM_ATTR).arg(cfg.dbChoosen));
        }

        cfg.aminoT = nullptr;

        bool optimizeForShortSequences = getValue<bool>(SHORTSEQ_ATTR);

        int evalue = getValue<int>(EVALUE_ATTR);
        if (evalue <= 0) {
            return new FailTask(tr("Incorrect '%1' parameter value: %2").arg(EVALUE_ATTR).arg(evalue));
        }

        int maxHits = getValue<int>(HITS_ATTR);
        if (maxHits <= 0) {
            return new FailTask(tr("Incorrect '%1' parameter value: %2").arg(HITS_ATTR).arg(maxHits));
        }

        cfg.params = "CMD=Put";
        if (cfg.dbChoosen == "cdd") {
            cfg.dbChoosen = "blastp";
            addParametr(cfg.params, ReqParams::program, cfg.dbChoosen);
            addParametr(cfg.params, ReqParams::service, "rpsblast");
            QString usedDB = getValue<QString>(DATABASE_ATTR);
            addParametr(cfg.params, ReqParams::database, usedDB.toLower());
            addParametr(cfg.params, ReqParams::hits, maxHits);
            addParametr(cfg.params, ReqParams::expect, evalue);
        } else {
            addParametr(cfg.params, ReqParams::program, cfg.dbChoosen);
            QString usedDatabase = getValue<QString>(DATABASE_ATTR);
            addParametr(cfg.params, ReqParams::database, usedDatabase.split(" ").last());
            QString filter;
            if (optimizeForShortSequences) {
                evalue = 1000;
                if (cfg.dbChoosen == "blastn") {
                    addParametr(cfg.params, ReqParams::wordSize, 7);
                }
            } else {
                addParametr(cfg.params, ReqParams::filter, "L");
            }
            QString entrezQueryStr = getValue<QString>(ENTREZ_QUERY_ATTR);
            if (false == entrezQueryStr.isEmpty()) {
                addParametr(cfg.params, ReqParams::entrezQuery, entrezQueryStr);
            }
            addParametr(cfg.params, ReqParams::expect, evalue);

            addParametr(cfg.params, ReqParams::hits, maxHits);
            if (getValue<bool>(MEGABLAST_ATTR)) {
                addParametr(cfg.params, ReqParams::megablast, "true");
            }
        }
        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));
        if (seqObj.isNull()) {
            return new FailTask(tr("Failed to read sequence"));
        }
        U2OpStatusImpl os;
        DNASequence seq = seqObj->getWholeSequence(os);
        CHECK_OP(os, new FailTask(os.getError()));

        seq.info.clear();
        const DNAAlphabet* alp = U2AlphabetUtils::findBestAlphabet(seq.seq);
        /*if(seq.length()>MAX_BLAST_SEQ_LEN) {
            log.error(tr("The sequence is too long"));
            return NULL;
        }*/
        if (alp->isAmino() && cfg.dbChoosen == "blastn") {
            return new FailTask(tr("Error: using amino sequence with blastn"));
        } else if (alp->isNucleic() && cfg.dbChoosen != "blastn") {
            return new FailTask(tr("Error: using nucleic sequence with blastp"));
        }
        cfg.query = seq.seq;
        cfg.isCircular = seq.circular;
        cfg.retries = 60;
        cfg.filterResult = 0;

        if (cfg.dbChoosen == "blastn") {
            addParametr(cfg.params, ReqParams::gapCost, getValue<QString>(GAP_ATTR));
            QString matchScores = getValue<QString>(MATCHSCORE_ATTR);
            addParametr(cfg.params, ReqParams::matchScore, matchScores.split(" ").first());
            addParametr(cfg.params, ReqParams::mismatchScore, matchScores.split(" ").last());
        }

        auto t = new RemoteBLASTTask(cfg);
        connect(t, &Task::si_stateChanged, this, &RemoteBLASTWorker::sl_taskFinished);
        return t;
    } else if (input->isEnded()) {
        setDone();
        output->setEnded();
    }
    return nullptr;
}

void RemoteBLASTWorker::sl_taskFinished() {
    auto t = qobject_cast<RemoteBLASTTask*>(sender());
    SAFE_POINT(t != nullptr, "Not a RemoteBLASTTask", );
    if (t->getState() != Task::State_Finished || t->hasError() || t->isCanceled()) {
        return;
    }

    if (output) {
        if (getValue<QString>(PROGRAM_ATTR) != "ncbi-cdd") {
            QString url = getValue<QString>(ORIGINAL_OUT);
            if (!url.isEmpty()) {
                IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
                IOAdapter* io = iof->createIOAdapter();
                if (io->open(url, IOAdapterMode_Write)) {
                    QByteArray output = t->getOutputFile();
                    io->writeBlock(output);
                    io->close();
                }
            }
        }

        QList<SharedAnnotationData> res = t->getResultedAnnotations();
        const QString annName = getValue<QString>(ANNOTATION_NAME);
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

}  // namespace LocalWorkflow
}  // namespace U2
