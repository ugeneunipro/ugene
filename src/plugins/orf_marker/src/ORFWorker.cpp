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

#include "ORFWorker.h"

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/BaseAttributes.h>

#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Algorithm/ORFAlgorithmTask.h>
#include <U2Core/FailTask.h>
#include <QApplication>

/* TRANSLATOR U2::LocalWorkflow::ORFWorker */

namespace U2 {
namespace LocalWorkflow {

/*****************************
 * ORFWorkerFactory
 *****************************/
static const QString NAME_ATTR("result-name");
static const QString ID_ATTR("genetic-code");
static const QString LEN_ATTR("min-length");
static const QString FIT_ATTR("require-stop-codon");
static const QString INIT_ATTR("require-init-codon");
static const QString ALT_ATTR("allow-alternative-codons");
static const QString ISC_ATTR("include-stop-codon");
static const QString RES_ATTR("max-result-attribute");
static const QString LIMIT_ATTR("limit-results");

const QString ORFWorkerFactory::ACTOR_ID("orf-search");

void ORFWorkerFactory::init() {
    QList<PortDescriptor*> p; QList<Attribute*> a;

    {
        Descriptor id(BasePorts::IN_SEQ_PORT_ID(), ORFWorker::tr("Input sequences"),
            ORFWorker::tr("A nucleotide sequence to search ORFs in. Protein sequences are skipped."));
        Descriptor od(BasePorts::OUT_ANNOTATIONS_PORT_ID(), ORFWorker::tr("ORF annotations"),
            ORFWorker::tr("A set of annotations marking ORFs found in the sequence."));

        QMap<Descriptor, DataTypePtr> inM;
        inM[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        p << new PortDescriptor(id, DataTypePtr(new MapDataType("orf.sequence", inM)), true /*input*/);
        QMap<Descriptor, DataTypePtr> outM;
        outM[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        p << new PortDescriptor(od, DataTypePtr(new MapDataType("orf.annotations", outM)), false /*input*/, true /*multi*/);
    }
    {
        Descriptor nd(NAME_ATTR, ORFWorker::tr("Annotate as"), ORFWorker::tr("Name of the result annotations marking found ORFs."));
        Descriptor ttd(ID_ATTR, ORFWorker::tr("Genetic code"), ORFWorker::tr("Which genetic code should be used for translating the input nucleotide sequence."));
        Descriptor ld(LEN_ATTR, ORFWorker::tr("Min length, bp:"), QApplication::translate("ORFDialogBase", "Ignore ORFs shorter than the specified length.", 0));
        Descriptor fd(FIT_ATTR, ORFWorker::tr("Require stop codon"), QApplication::translate("ORFDialogBase", "Require stop codon", 0));
        Descriptor ind(INIT_ATTR, ORFWorker::tr("Require init codon"), QApplication::translate("ORFDialogBase", "\n""Ignore boundary ORFs which last beyond the search region\n""(i.e. have no stop codon within the range).\n", 0));
        Descriptor ad(ALT_ATTR, ORFWorker::tr("Allow alternative codons"), QApplication::translate("ORFDialogBase", "\n"
            "               Allow ORFs starting with alternative initiation codons,\n"
            "               accordingly to the current translation table.\n", 0));
        Descriptor isc(ISC_ATTR, ORFWorker::tr("Include stop codon"), ORFWorker::tr("The result annotation will includes stop codon if this option is set."));
        Descriptor mr(RES_ATTR,ORFWorker::tr("Max result"),ORFWorker::tr("Find results not achieved by specified count."));
        Descriptor lr(LIMIT_ATTR,ORFWorker::tr("Limit results"),ORFWorker::tr("The amount of results will be limited id that option is setted."));

        a << new Attribute(nd, BaseTypes::STRING_TYPE(), true, QVariant("ORF"));
        a << new Attribute(ttd, BaseTypes::STRING_TYPE(), false, QVariant(DNATranslationID(1)));
        a << new Attribute(ld, BaseTypes::NUM_TYPE(), false, QVariant(100));
        a << new Attribute(BaseAttributes::STRAND_ATTRIBUTE(), BaseTypes::STRING_TYPE(), false, BaseAttributes::STRAND_BOTH());
        a << new Attribute(fd, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute(ind, BaseTypes::BOOL_TYPE(), false, QVariant(true));
        a << new Attribute(ad, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute(isc, BaseTypes::BOOL_TYPE(), false, QVariant(false));
        a << new Attribute(mr,BaseTypes::NUM_TYPE(),false,100000);
        a << new Attribute(lr,BaseTypes::BOOL_TYPE(),false,QVariant(true));
    }

    Descriptor desc(ACTOR_ID, ORFWorker::tr("ORF Marker"),
        ORFWorker::tr("Finds Open Reading Frames (ORFs) in each supplied nucleotide sequence, stores found regions as annotations."
        "<p>Protein sequences are skipped if any supplied to input."
        "<p><dfn>ORFs are DNA sequence regions that could potentially encode a protein,"
        " and usually give a good indication of the presence of a gene in the surrounding sequence.</dfn></p>"
        "<p>In the sequence, ORFs are located between a start-code sequence (initiation codon) and a stop-code sequence (termination codon),"
        " defined by the selected genetic code.</p>")
       );
    ActorPrototype* proto = new IntegralBusActorPrototype(desc, p, a);
    QMap<QString, PropertyDelegate*> delegates;

    QVariantMap lenMap; lenMap["minimum"] = QVariant(0); lenMap["maximum"] = QVariant(INT_MAX);
    lenMap["suffix"] = L10N::suffixBp();
    delegates[LEN_ATTR] = new SpinBoxDelegate(lenMap);
    delegates[BaseAttributes::STRAND_ATTRIBUTE().getId()] = new ComboBoxDelegate(BaseAttributes::STRAND_ATTRIBUTE_VALUES_MAP());

    QList<ComboItem> idMap;
    QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->
        lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()),
        DNATranslationType_NUCL_2_AMINO);
    foreach(DNATranslation* tt, TTs) {
        idMap.append(qMakePair(tt->getTranslationName(), tt->getTranslationId()));
    }
    delegates[ID_ATTR] = new ComboBoxDelegate(idMap);

    proto->setPrompter(new ORFPrompter());
    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":orf_marker/images/orf_marker.png");
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_BASIC(), proto);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalDomainFactory::ID);
    localDomain->registerEntry(new ORFWorkerFactory());
}

static ORFAlgorithmStrand getStrand(const QString & s) {
    QString str = s.toLower();
    if(BaseAttributes::STRAND_BOTH().startsWith(str)) {
        return ORFAlgorithmStrand_Both;
    } else if(BaseAttributes::STRAND_DIRECT().startsWith(str)) {
        return ORFAlgorithmStrand_Direct;
    } else if(BaseAttributes::STRAND_COMPLEMENTARY().startsWith(str)) {
        return ORFAlgorithmStrand_Complement;
    } else {
        bool ok = false;
        int num = str.toInt(&ok);
        if(ok && num >= 0) {
            return ORFAlgorithmStrand(num);
        } else {
            return ORFAlgorithmStrand_Both;
        }
    }
}

/*****************************
 * ORFPrompter
 *****************************/
QString ORFPrompter::composeRichDoc() {
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(BasePorts::IN_SEQ_PORT_ID()));
    Actor* producer = input->getProducer(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString producerName = tr(" from <u>%1</u>").arg(producer ? producer->getLabel() : unsetStr);

    ORFAlgorithmSettings cfg;
    cfg.strand = getStrand(getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId()).value<QString>());
    cfg.minLen = getParameter(LEN_ATTR).toInt();
    cfg.mustFit = getParameter(FIT_ATTR).toBool();
    cfg.mustInit = getParameter(INIT_ATTR).toBool();
    cfg.allowAltStart = getParameter(ALT_ATTR).toBool();
    cfg.includeStopCodon = getParameter(ISC_ATTR).toBool();

    QString extra;
    if (!cfg.mustInit) {
        QString anyLink = getHyperlink(INIT_ATTR, tr("starting with any codon"));
        extra += tr(", allow ORFs %1 other than terminator").arg(anyLink);
    } else if (cfg.allowAltStart) {
        QString altLink = getHyperlink(ALT_ATTR, tr("alternative start codons"));
        extra += tr(", take into account %1").arg(altLink);
    }
    if (cfg.mustFit) {
        QString mustFitLink = getHyperlink(FIT_ATTR, tr("ignore non-terminated"));
        extra += tr(", %1 ORFs").arg(mustFitLink);
    }

    QString strandName;
    switch (cfg.strand) {
    case ORFAlgorithmStrand_Both: strandName = ORFWorker::tr("both strands"); break;
    case ORFAlgorithmStrand_Direct: strandName = ORFWorker::tr("direct strand"); break;
    case ORFAlgorithmStrand_Complement: strandName = ORFWorker::tr("complement strand"); break;
    }
    strandName = getHyperlink(BaseAttributes::STRAND_ATTRIBUTE().getId(), strandName);

    QString resultName = getHyperlink(NAME_ATTR, getRequiredParam(NAME_ATTR));

    QString transId = getParameter(ID_ATTR).toString();
    QString ttName = AppContext::getDNATranslationRegistry()->
        lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()), DNATranslationType_NUCL_2_AMINO, transId)->getTranslationName();
    ttName = getHyperlink(ID_ATTR, ttName);


    QString doc = tr("For each nucleotide sequence%1, find ORFs in <u>%2</u> using the <u>%3</u>."
        "<br>Detect only ORFs <u>not shorter than %4 bps</u>%5."
        "<br>Output the list of found regions annotated as <u>%6</u>.")
        .arg(producerName) //sequence from Read Fasta 1
        .arg(strandName) //both strands
        .arg(ttName) //Standard Genetic Code
        .arg(getHyperlink(LEN_ATTR, cfg.minLen)) //100
        .arg(extra) //  take into account alternative start codons.
        .arg(resultName);

    return doc;
}

/*****************************
 * ORFWorker
 *****************************/
ORFWorker::ORFWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {
}

void ORFWorker::init() {
    input = ports.value(BasePorts::IN_SEQ_PORT_ID());
    output = ports.value(BasePorts::OUT_ANNOTATIONS_PORT_ID());
}

Task* ORFWorker::tick() {
    if (input->hasMessage()) {
        Message inputMessage = getMessageAndSetupScriptValues(input);
        if (inputMessage.isEmpty()) {
            output->put(Message::getEmptyMapMessage());
        }
        cfg.strand = getStrand(actor->getParameter(BaseAttributes::STRAND_ATTRIBUTE().getId())->getAttributeValue<QString>(context));
        cfg.minLen = actor->getParameter(LEN_ATTR)->getAttributeValue<int>(context);
        cfg.mustFit = actor->getParameter(FIT_ATTR)->getAttributeValue<bool>(context);
        cfg.mustInit = actor->getParameter(INIT_ATTR)->getAttributeValue<bool>(context);
        cfg.allowAltStart = actor->getParameter(ALT_ATTR)->getAttributeValue<bool>(context);
        cfg.includeStopCodon = actor->getParameter(ISC_ATTR)->getAttributeValue<bool>(context);
        cfg.maxResult2Search = actor->getParameter(RES_ATTR)->getAttributeValue<int>(context);
        resultName = actor->getParameter(NAME_ATTR)->getAttributeValue<QString>(context);
        if(resultName.isEmpty()){
            algoLog.error(tr("ORF: result name is empty, default name used"));
            resultName = "misc_feature";
        }
        transId = actor->getParameter(ID_ATTR)->getAttributeValue<QString>(context);
        if (cfg.minLen < 0){
            algoLog.error(tr("ORF: Incorrect value: min-length must be greater then zero"));
            return new FailTask(tr("Incorrect value: min-length must be greater then zero"));
        }

        SharedDbiDataHandler seqId = inputMessage.getData().toMap().value(BaseSlots::DNA_SEQUENCE_SLOT().getId()).value<SharedDbiDataHandler>();
        QScopedPointer<U2SequenceObject> seqObj(StorageUtils::getSequenceObject(context->getDataStorage(), seqId));

        if (seqObj.isNull()) {
            return NULL;
        }

        const DNAAlphabet* alphabet = seqObj->getAlphabet();
        if (alphabet && alphabet->getType() == DNAAlphabet_NUCL) {
            ORFAlgorithmSettings config(cfg);
            config.searchRegion.length = seqObj->getSequenceLength();
            if (config.strand != ORFAlgorithmStrand_Direct) {
                DNATranslation* compTT = AppContext::getDNATranslationRegistry()->
                    lookupComplementTranslation(alphabet);
                if (compTT != NULL) {
                    config.complementTT = compTT;
                } else {
                    config.strand = ORFAlgorithmStrand_Direct;
                }
            }
            config.proteinTT = AppContext::getDNATranslationRegistry()->
                lookupTranslation(alphabet, DNATranslationType_NUCL_2_AMINO, transId);
            if (config.proteinTT) {
                Task* t = new ORFFindTask(config,seqObj->getEntityRef());
                connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
                return t;
            }
        }
        QString err = tr("Bad sequence supplied to ORFWorker: %1").arg(seqObj->getSequenceName());

        return new FailTask(err);
    } else if (input->isEnded()) {
        output->setEnded();
        setDone();
    }
    return NULL;
}

void ORFWorker::sl_taskFinished() {
    ORFFindTask *t = qobject_cast<ORFFindTask *>(sender());
    if (t->getState() != Task::State_Finished || t->isCanceled() || t->hasError()) {
        return;
    }
    QList<ORFFindResult> res = t->popResults();
    if (NULL != output) {
        const QList<SharedAnnotationData> annsList = ORFFindResult::toTable(res, resultName);

        const SharedDbiDataHandler tableId = context->getDataStorage()->putAnnotationTable(annsList);
        output->put(Message(BaseTypes::ANNOTATION_TABLE_TYPE(), qVariantFromValue<SharedDbiDataHandler>(tableId)));
        algoLog.info(tr("Found %1 ORFs").arg(res.size()));
    }
}

void ORFWorker::cleanup() {

}

} //namespace LocalWorkflow
} //namespace U2
