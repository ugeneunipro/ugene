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

#include "ORFQuery.h"

#include <U2Designer/DelegateEditors.h>
#include <U2Lang/BaseTypes.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>

#include <U2Algorithm/ORFAlgorithmTask.h>

#include <QApplication>

namespace U2 {

static const QString ID_ATTR("trans-id");
static const QString LEN_ATTR("min-length");
static const QString INIT_ATTR("starts-with-init");
static const QString FIT_ATTR("stop-codon");
static const QString ALT_ATTR("alt-start");
static const QString MAX_LENGTH_ATTR("max-length");
static const QString RES_ATTR("max-result-attribute");
static const QString LIMIT_ATTR("limit-results");

QDORFActor::QDORFActor(QDActorPrototype const* proto) : QDActor(proto) {
    units["orf"] = new QDSchemeUnit(this);
    cfg->setAnnotationKey("ORF");
}

QString QDORFActor::getText() const {
    QMap<QString, Attribute*> params = cfg->getParameters();

    QString strandName;
    switch (strand) {
        case QDStrand_Both: strandName = QDORFActor::tr("both strands");
            break;
        case QDStrand_DirectOnly: strandName = QDORFActor::tr("direct strand");
            break;
        case QDStrand_ComplementOnly: strandName = QDORFActor::tr("complement strand");
            break;
    }

    const QString& transId = cfg->getParameters().value(ID_ATTR)->getAttributeValueWithoutScript<QString>();
    QString ttName = AppContext::getDNATranslationRegistry()->
        lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()), DNATranslationType_NUCL_2_AMINO, transId)->getTranslationName();
    ttName = QString("<a href=%1>%2</a>").arg(ID_ATTR).arg(ttName);

    bool mustInit = params.value(INIT_ATTR)->getAttributePureValue().toBool();
    bool allowAltStart = params.value(ALT_ATTR)->getAttributePureValue().toBool();
    bool mustFit = params.value(FIT_ATTR)->getAttributePureValue().toBool();
    QString extra;
    if (!mustInit) {
        extra += QDORFActor::tr(", allow ORFs <u>starting with any codon</u> other than terminator");
    } else if (allowAltStart) {
        extra += QDORFActor::tr(", take into account <u>alternative start codons</u>");
    }
    if (mustFit) {
        extra += QDORFActor::tr(", <u>ignore non-terminated</u> ORFs");
    }

    int minLen = cfg->getParameter(LEN_ATTR)->getAttributePureValue().toInt();
    QString minLenStr = QString("<a href=%1>%2 bps</a>").arg(LEN_ATTR).arg(minLen);
    int maxLen = cfg->getParameter(MAX_LENGTH_ATTR)->getAttributeValueWithoutScript<int>();
    QString maxLenStr = QString("<a href=%1>%2 bps</a>").arg(MAX_LENGTH_ATTR).arg(maxLen);

    QString doc = QDORFActor::tr("Finds ORFs in <u>%1</u> using the <u>%2</u>."
        "<br>Detects only ORFs <u>not shorter than %3, not longer than %4</u>%5.")
        .arg(strandName) //both strands
        .arg(ttName) //Standard Genetic Code
        .arg(minLenStr) //100
        .arg(maxLenStr)
        .arg(extra); //  take into account alternative start codons.

    return doc;
}

Task* QDORFActor::getAlgorithmTask(const QVector<U2Region>& searchLocation) {
    Task* t = NULL;
    const DNASequence& dnaSeq = scheme->getSequence();
    QMap<QString, Attribute*> params = cfg->getParameters();

    switch (getStrandToRun()) {
        case QDStrand_Both:
            settings.strand = ORFAlgorithmStrand_Both;
            break;
        case QDStrand_DirectOnly:
            settings.strand = ORFAlgorithmStrand_Direct;
            break;
        case QDStrand_ComplementOnly:
            settings.strand = ORFAlgorithmStrand_Complement;
            break;
    }
    settings.minLen = params.value(LEN_ATTR)->getAttributePureValue().toInt();
    settings.mustFit = params.value(FIT_ATTR)->getAttributePureValue().toBool();
    settings.mustInit = params.value(INIT_ATTR)->getAttributePureValue().toBool();
    settings.allowAltStart = params.value(ALT_ATTR)->getAttributePureValue().toBool();
    settings.maxResult2Search = params.value(RES_ATTR)->getAttributePureValue().toInt();

    settings.searchRegion = U2Region(0, dnaSeq.length());

    if (settings.strand != ORFAlgorithmStrand_Direct) {
        DNATranslation* compTT = NULL;
        if (dnaSeq.alphabet->isNucleic()) {
            compTT = AppContext::getDNATranslationRegistry()->
                lookupComplementTranslation(dnaSeq.alphabet);
        }
        if (compTT != NULL) {
            settings.complementTT = compTT;
        } else {
            settings.strand = ORFAlgorithmStrand_Direct;
        }
    }

    const QString& transId = params.value(ID_ATTR)->getAttributeValueWithoutScript<QString>();
    settings.proteinTT = AppContext::getDNATranslationRegistry()->
        lookupTranslation(dnaSeq.alphabet, DNATranslationType_NUCL_2_AMINO, transId);

    if (!settings.proteinTT) {
        return new FailTask(tr("Bad sequence"));
    }

    t = new Task(tr("ORF find"), TaskFlag_NoRun);
    assert(orfTasks.isEmpty());
    foreach(const U2Region& r, searchLocation) {
        ORFAlgorithmSettings stngs(settings);
        stngs.searchRegion = r;
        ORFFindTask* sub = new ORFFindTask(stngs, scheme->getEntityRef());
        orfTasks.append(sub);
        t->addSubTask(sub);
    }
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)),
        SLOT(sl_onAlgorithmTaskFinished(Task*)));
    return t;
}

void QDORFActor::sl_onAlgorithmTaskFinished(Task*) {
    QList<ORFFindResult> res;
    foreach(ORFFindTask* oft, orfTasks) {
        res << oft->popResults();
    }
    QList<SharedAnnotationData> dataList = ORFFindResult::toTable(res, "1");
    foreach (const SharedAnnotationData &ad, dataList) {
        const U2Region &first = ad->location->regions[0];
        if (first.length > getMaxResultLen()) {
            continue;
        }
        QDResultUnit ru(new QDResultUnitData);
        ru->strand = ad->getStrand();
        ru->quals = ad->qualifiers;
        ru->region = first;
        ru->owner = units.value("orf");
        QDResultGroup::buildGroupFromSingleResult(ru, results);
    }
    orfTasks.clear();
}

int QDORFActor::getMinResultLen() const {
    return cfg->getParameter(LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

int QDORFActor::getMaxResultLen() const {
    return cfg->getParameter(MAX_LENGTH_ATTR)->getAttributeValueWithoutScript<int>();
}
QDORFActorPrototype::QDORFActorPrototype() {
     descriptor.setId("orf");
     descriptor.setDisplayName(QDORFActor::tr("ORF"));
     descriptor.setDocumentation(QDORFActor::tr("Finds Open Reading Frames (ORFs) in supplied nucleotide sequence, stores found regions as annotations."
         "<p>Protein sequences are skipped if any."
         "<p><dfn>ORFs are DNA sequence regions that could potentially encode a protein,"
         " and usually give a good indication of the presence of a gene in the surrounding sequence.</dfn></p>"
         "<p>In the sequence, ORFs are located between a start-code sequence (initiation codon) and a stop-code sequence (termination codon),"
         " defined by the selected genetic code.</p>")
         );

     Descriptor ttd(ID_ATTR, QDORFActor::tr("Genetic code"), QDORFActor::tr("Which genetic code should be used for translating the input nucleotide sequence."));
     Descriptor ld(LEN_ATTR, QDORFActor::tr("Min length, bp:"), QApplication::translate("ORFDialogBase", "Ignore ORFs shorter than the specified length", 0));
     Descriptor ind(INIT_ATTR, QDORFActor::tr("Require init codon"), QApplication::translate("ORFDialogBase", "\n""Ignore boundary ORFs which last beyond the search region\n""(i.e. have no stop codon within the range).\n", 0));
     Descriptor ad(ALT_ATTR, QDORFActor::tr("Allow alternative codons"), QApplication::translate("ORFDialogBase", "\n"
     "               Allow ORFs starting with alternative initiation codons,\n"
     "               accordingly to the current translation table.\n", 0));
     Descriptor mld(MAX_LENGTH_ATTR, QDORFActor::tr("Max length"), QDORFActor::tr("Maximum length of annotation allowed."));
     Descriptor fd(FIT_ATTR, QDORFActor::tr("Require stop codon"), QDORFActor::tr("Require stop codon."));
     Descriptor mr(RES_ATTR,QDORFActor::tr("Max result"),QDORFActor::tr("Find results not achieved by specified count."));
     Descriptor lr(LIMIT_ATTR,QDORFActor::tr("Limit results"),QDORFActor::tr("The amount of results will be limited id that option is set"));

     attributes << new Attribute(ttd, BaseTypes::STRING_TYPE(), false, QVariant(DNATranslationID(1)));
     attributes << new Attribute(ld, BaseTypes::NUM_TYPE(), true, QVariant(100));
     attributes << new Attribute(ind, BaseTypes::BOOL_TYPE(), false, QVariant(true));
     attributes << new Attribute(ad, BaseTypes::BOOL_TYPE(), false, QVariant(false));
     attributes << new Attribute(mld, BaseTypes::NUM_TYPE(), true, QVariant(QDActor::DEFAULT_MAX_RESULT_LENGTH));
     attributes << new Attribute(fd, BaseTypes::BOOL_TYPE(), false, QVariant(false));
     attributes << new Attribute(mr,BaseTypes::NUM_TYPE(),false,100000);
     attributes << new Attribute(lr,BaseTypes::BOOL_TYPE(),false,QVariant(true));

     QMap<QString, PropertyDelegate*> delegates;

     QVariantMap lenMap;
     lenMap["minimum"] = QVariant(0);
     lenMap["maximum"] = QVariant(INT_MAX);
     lenMap["suffix"] = L10N::suffixBp();
     delegates[LEN_ATTR] = new SpinBoxDelegate(lenMap);
     delegates[MAX_LENGTH_ATTR] = new SpinBoxDelegate(lenMap);

     QVariantMap idMap;
     QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->
         lookupTranslation(AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()),
         DNATranslationType_NUCL_2_AMINO);
     foreach(DNATranslation* tt, TTs) {
         idMap[tt->getTranslationName()] = tt->getTranslationId();
     }
     delegates[ID_ATTR] = new ComboBoxDelegate(idMap);

     editor = new DelegateEditor(delegates);
}

}//namespace
