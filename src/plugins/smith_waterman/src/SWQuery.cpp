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

#include "SWQuery.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/PluginModel.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>

#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>
#include <U2Algorithm/SWResultFilterRegistry.h>

#include <U2Lang/Attribute.h>
#include <U2Lang/ConfigurationEditor.h>
#include <U2Lang/BaseTypes.h>

namespace U2 {

static const QString PATTERN_ATTR("pattern");
static const QString SCORE_ATTR("min-score");
static const QString MATRIX_ATTR("matrix");
static const QString AMINO_ATTR("translate");
static const QString ALGO_ATTR("algorithm");
static const QString FILTER_ATTR("filter");
static const QString GAPOPEN_ATTR("gap-open-score");
static const QString GAPEXT_ATTR("gap-ext-score");

void SWAlgoEditor::populate() {
    QStringList algoLst = AppContext::getSmithWatermanTaskFactoryRegistry()->getListFactoryNames();
    if (algoLst.isEmpty()) {
        return;
    }
    foreach(const QString& n, algoLst) {
        comboItems.append(qMakePair(n,n));
    }

    algAttr->setAttributeValue(algoLst.first());
}

/************************************************************************/
/* QDSWActor                                                              */
/************************************************************************/

QDSWActor::QDSWActor(QDActorPrototype const* proto) : QDActor(proto), algo(0) {
    units["sw"] = new QDSchemeUnit(this);
    algo = NULL;
}

int QDSWActor::getMinResultLen() const {
    return cfg->getParameter(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().toLatin1().size()/2;
}

int QDSWActor::getMaxResultLen() const {
    return 4*getMinResultLen();
}

QString QDSWActor::getText() const {
    QMap<QString, Attribute*> params = cfg->getParameters();

    QString pattern = params.value(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().toLatin1().toUpper();
    if (pattern.isEmpty()) {
        pattern = "unset";
    }

    pattern = QString("<a href=%1>%2</a>").arg(PATTERN_ATTR).arg(pattern);

    int percentOfScore = params.value(SCORE_ATTR)->getAttributeValueWithoutScript<int>();
    QString percentOfScoreStr = QString("<a href=%1>%2%</a>").arg(SCORE_ATTR).arg(percentOfScore);
    QString match = percentOfScore < 100 ?
        QDSWActor::tr("matches with <u>at least %1 score</u>").arg(percentOfScoreStr) : QDSWActor::tr("exact matches");

    QString strandName;
    switch (strand) {
        case QDStrand_Both: strandName = QDSWActor::tr("both strands"); break;
        case QDStrand_DirectOnly: strandName = QDSWActor::tr("direct strand"); break;
        case QDStrand_ComplementOnly: strandName = QDSWActor::tr("complement strand"); break;
        default: break;
    }

    QString doc = QDSWActor::tr("Finds pattern <u>%1</u>."
        "<br>Looks for <u>%2</u> in <u>%3</u>.")
        .arg(pattern)
        .arg(match)
        .arg(strandName);
    return doc;
}

Task* QDSWActor::getAlgorithmTask(const QVector<U2Region>& searchLocation) {
    Task* task = NULL;
    assert(scheme);
    QMap<QString, Attribute*> params = cfg->getParameters();

    settings.aminoTT = NULL;
    settings.complTT = NULL;
    switch(getStrandToRun()) {
        case QDStrand_Both:
            settings.strand = StrandOption_Both;
            break;
        case QDStrand_DirectOnly:
            settings.strand = StrandOption_DirectOnly;
            break;
        case QDStrand_ComplementOnly:
            settings.strand = StrandOption_ComplementOnly;
            break;
    }
    settings.percentOfScore = params.value(SCORE_ATTR)->getAttributeValueWithoutScript<int>();
    if(settings.percentOfScore<0&&settings.percentOfScore>100) {
        QString err = tr("%1: percent of score out of bounds.").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    settings.gapModel.scoreGapExtd = params.value(GAPEXT_ATTR)->getAttributeValueWithoutScript<double>();
    settings.gapModel.scoreGapOpen = params.value(GAPOPEN_ATTR)->getAttributeValueWithoutScript<double>();
    mtrx = params.value(MATRIX_ATTR)->getAttributeValueWithoutScript<QString>();
    settings.pSm = AppContext::getSubstMatrixRegistry()->getMatrix(mtrx);
    QString filter = params.value(FILTER_ATTR)->getAttributeValueWithoutScript<QString>();
    settings.resultFilter = AppContext::getSWResultFilterRegistry()->getFilter(filter);
    if(!settings.resultFilter) {
        QString err = tr("%1: incorrect result filter.").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    settings.ptrn = params.value(PATTERN_ATTR)->getAttributeValueWithoutScript<QString>().toLatin1().toUpper();
    if(settings.ptrn.isEmpty()) {
        QString err = tr("%1: pattern is empty.").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    QString algName = params.value(ALGO_ATTR)->getAttributeValueWithoutScript<QString>();
    algo = AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(algName);
    if(!algo) {
        QString err = tr("%1: can not find %2.")
            .arg(getParameters()->getLabel())
            .arg(algName);
        return new FailTask(err);
    }

    const DNASequence& dnaSeq = scheme->getSequence();
    settings.sqnc = dnaSeq.seq;

    if (settings.strand != StrandOption_DirectOnly) {
        DNATranslation* compTT = NULL;
        if (dnaSeq.alphabet->isNucleic()) {
            compTT = AppContext::getDNATranslationRegistry()->
                lookupComplementTranslation(dnaSeq.alphabet);
        }
        if (compTT != NULL) {
            settings.complTT = compTT;
        } else {
            //Could not find complement translation, searching only direct strand
            settings.strand = StrandOption_DirectOnly;
        }
    }

    if (params.value(AMINO_ATTR)->getAttributeValueWithoutScript<bool>()) {
        DNATranslationType tt = (dnaSeq.alphabet->getType() == DNAAlphabet_NUCL) ? DNATranslationType_NUCL_2_AMINO : DNATranslationType_RAW_2_AMINO;
        QList<DNATranslation*> TTs = AppContext::getDNATranslationRegistry()->lookupTranslation(dnaSeq.alphabet, tt);
        if (!TTs.isEmpty()) { //FIXME let user choose or use hints ?
                settings.aminoTT = AppContext::getDNATranslationRegistry()->getStandardGeneticCodeTranslation(dnaSeq.alphabet);
        }
    }
    assert(settings.pSm.getName().isEmpty() && mtrx.toLower() != "auto");

    if (settings.pSm.isEmpty()) {
        QString matrixName;
        QStringList lst = AppContext::getSubstMatrixRegistry()->selectMatrixNamesByAlphabet(dnaSeq.alphabet);
        if (!lst.isEmpty()) {
            matrixName = lst.first();
            settings.pSm = AppContext::getSubstMatrixRegistry()->getMatrix(matrixName);
        }
        assert(!settings.pSm.isEmpty());
    }

    settings.globalRegion = U2Region(0, dnaSeq.length());

    task = new Task(tr("SSearch"), TaskFlag_NoRun);
    foreach(const U2Region& r, searchLocation) {
        SmithWatermanSettings stngs(settings);
        SmithWatermanReportCallbackAnnotImpl* rcb = new SmithWatermanReportCallbackAnnotImpl( NULL,
            U2FeatureTypes::MiscFeature, QString(), QString(), "", false );
        stngs.resultCallback = rcb;
        stngs.resultListener = new SmithWatermanResultListener();
        stngs.globalRegion = r;
        Task* sub = algo->getTaskInstance(stngs, tr("smith_waterman_task"));
        rcb->setParent(sub);
        task->addSubTask(sub);
        callbacks.insert(sub, rcb);
    }
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished(Task*)));
    return task;
}

void QDSWActor::sl_onAlgorithmTaskFinished(Task*) {
    QList<SharedAnnotationData> res;
    QMapIterator<Task*, SmithWatermanReportCallbackAnnotImpl*> iter(callbacks);
    while (iter.hasNext()) {
        iter.next();
        res << iter.value()->getAnotations();
    }

    foreach (const SharedAnnotationData &ad, res) {
        QDResultUnit ru(new QDResultUnitData);
        ru->strand = ad->getStrand();
        ru->quals = ad->qualifiers;
        ru->region = ad->location->regions.first();
        ru->owner = units.value("sw");
        QDResultGroup::buildGroupFromSingleResult(ru, results);
    }
    callbacks.clear();
}

SWQDActorFactory::SWQDActorFactory() {
    descriptor.setId("ssearch");
    descriptor.setDisplayName(QDSWActor::tr("Smith-Waterman"));
    descriptor.setDocumentation(QDSWActor::tr("Finds regions of similarity to the specified pattern in each input sequence (nucleotide or protein one). "
        "<p>Under the hood is the well-known Smith-Waterman algorithm for performing local sequence alignment."));

    QStringList filterLst = AppContext::getSWResultFilterRegistry()->getFiltersIds();
    QString defFilter = SWRF_EmptyFilter().getId();
    if(!filterLst.contains(defFilter)) {
        defFilter = AppContext::getSWResultFilterRegistry()->getDefaultFilterId();
    }

    Descriptor pd(PATTERN_ATTR, QDSWActor::tr("Pattern"), QDSWActor::tr("A subsequence pattern to look for."));
    Descriptor scd(SCORE_ATTR, QDSWActor::tr("Min score"), QDSWActor::tr("The search stringency."));
    Descriptor ald(ALGO_ATTR, QDSWActor::tr("Algorithm"), QDSWActor::tr("Algorithm version."));
    Descriptor amd(AMINO_ATTR, QDSWActor::tr("Search in translation"), QDSWActor::tr("Translate a supplied nucleotide sequence to protein then search in the translated sequence."));
    Descriptor mxd(MATRIX_ATTR, QDSWActor::tr("Scoring matrix"), QDSWActor::tr("The scoring matrix."));
    Descriptor frd(FILTER_ATTR, QDSWActor::tr("Filter results"), QDSWActor::tr("Result filtering strategy."));
    Descriptor god(GAPOPEN_ATTR, QDSWActor::tr("Gap open score"), QDSWActor::tr("Gap open score."));
    Descriptor ged(GAPEXT_ATTR, QDSWActor::tr("Gap ext score"), QDSWActor::tr("Gap extension score."));

    attributes << new Attribute(pd, BaseTypes::STRING_TYPE(), true);
    attributes << new Attribute(mxd, BaseTypes::STRING_TYPE(), true, QString("---"));
    Attribute* algAttr = new Attribute(ald, BaseTypes::STRING_TYPE(), true);
    attributes << algAttr;
    attributes << new Attribute(frd, BaseTypes::STRING_TYPE(), false, defFilter);
    attributes << new Attribute(scd, BaseTypes::NUM_TYPE(), false, 90);
    attributes << new Attribute(amd, BaseTypes::BOOL_TYPE(), false, false);
    attributes << new Attribute(god, BaseTypes::NUM_TYPE(), false, -10.);
    attributes << new Attribute(ged, BaseTypes::NUM_TYPE(), false, -1.);

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
    {
        QVariantMap m;
        foreach(const QString& n, filterLst) {
            m.insert(n,n);
        }
        delegates[FILTER_ATTR] = new ComboBoxDelegate(m);
    }
    {
        QVariantMap m; m.insert(QDSWActor::tr("Auto"), QString("---"));
        QStringList lst = AppContext::getSubstMatrixRegistry()->getMatrixNames();
        foreach(const QString& n, lst) {
            m.insert(n,n);
        }
        delegates[MATRIX_ATTR] = new ComboBoxDelegate(m);
    }

    SWAlgoEditor* aled = new SWAlgoEditor(algAttr);
    aled->connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(populate()));
    aled->populate();
    delegates[ALGO_ATTR] = aled;

    editor = new DelegateEditor(delegates);
}

}//namespace
