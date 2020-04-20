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

#include <math.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FailTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/DialogUtils.h>

#include <U2Lang/BaseTypes.h>

#include "HMMIO.h"
#include "HMMSearchQDActor.h"
#include "HMMSearchTask.h"
#include "uhmmsearch.h"

namespace U2 {

static const QString PROFILE_ATTR("hmm-profile");
static const QString NSEQ_ATTR("seqs-num");
static const QString DOM_E_ATTR("e-val");
static const QString DOM_T_ATTR("score");
static const QString MIN_LEN("min-len");
static const QString MAX_LEN("max-len");

HMM2QDActor::HMM2QDActor( QDActorPrototype const* proto ) : QDActor(proto) {
    units["hmm"] = new QDSchemeUnit(this);
    cfg->setAnnotationKey("hmm_signal");
    CHECK(NULL != proto->getEditor(), );
    PropertyDelegate* evpd = proto->getEditor()->getDelegate(DOM_E_ATTR);
    connect(evpd, SIGNAL(si_valueChanged(int)), SLOT(sl_evChanged(int)));
}

int HMM2QDActor::getMinResultLen() const {
    return cfg->getParameter(MIN_LEN)->getAttributeValueWithoutScript<int>();
}

int HMM2QDActor::getMaxResultLen() const {
    return cfg->getParameter(MAX_LEN)->getAttributeValueWithoutScript<int>();
}

QString HMM2QDActor::getText() const {
    return tr("HMM2");
}

Task* HMM2QDActor::getAlgorithmTask( const QVector<U2Region>& location ) {
    const DNASequence& dnaSeq = scheme->getSequence();
    const char* seq = dnaSeq.seq.constData();

    QMap<QString, Attribute*> params = cfg->getParameters();
    QString hmmFileStr = params.value(PROFILE_ATTR)->getAttributeValueWithoutScript<QString>();
    QStringList hmmFiles = hmmFileStr.split(QRegExp("\\s*;\\s*"));

    Task* t = new Task(tr("QD HMM2 search"), TaskFlag_NoRun);

    UHMMSearchSettings stngs;

    float domENum = cfg->getParameter(DOM_E_ATTR)->getAttributeValueWithoutScript<int>();
    if(domENum > 0) {
        algoLog.details(tr("Power of e-value must be less or equal to zero. Using default value: 1e-1"));
        domENum = -1;
    }
    stngs.domE = pow(10, domENum);
    stngs.domT = (float)cfg->getParameter(DOM_T_ATTR)->getAttributeValueWithoutScript<double>();
    stngs.eValueNSeqs = cfg->getParameter(NSEQ_ATTR)->getAttributeValueWithoutScript<int>();
    
    foreach(QString hmmFile, hmmFiles) {
        foreach(U2Region r, location) {
            DNASequence sequence;
            sequence.seq = QByteArray(seq + r.startPos, r.length);
            sequence.alphabet = dnaSeq.alphabet;

            HMMSearchTask* st = new HMMSearchTask(hmmFile, sequence, stngs);
            t->addSubTask(st);
            offsets[st] = r.startPos;
        }
    }

    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)));

    return t;
}

void HMM2QDActor::sl_onTaskFinished(Task*) {
    QString aname = cfg->getAnnotationKey();
    foreach(HMMSearchTask* t, offsets.keys()) {
        QList<SharedAnnotationData> annotations = t->getResultsAsAnnotations(U2FeatureTypes::MiscSignal, aname);
        int offset = offsets.value(t);
        foreach (const SharedAnnotationData &d, annotations) {
            U2Region r = d->location->regions.first();
            if (r.length < getMinResultLen() || r.length > getMaxResultLen()) {
                continue;
            }
            r.startPos+=offset;
            QDResultUnit ru(new QDResultUnitData);
            ru->strand = d->getStrand();
            ru->quals = d->qualifiers;
            ru->region = r;
            ru->owner = units.value("hmm");
            QDResultGroup::buildGroupFromSingleResult(ru, results);
        }
    }
    offsets.clear();
}

void HMM2QDActor::sl_evChanged(int i) {
    CHECK(NULL != proto->getEditor(), );
    PropertyDelegate* pd = proto->getEditor()->getDelegate(DOM_E_ATTR);
    SpinBoxDelegate* evpd = qobject_cast<SpinBoxDelegate*>(pd);
    assert(evpd);
    if (i>0) {
        evpd->setEditorProperty("prefix", "1e+");
    } else {
        evpd->setEditorProperty("prefix", "1e");
    }
}

void HMM2QDActor::updateEditor() {
    int val = cfg->getParameter(DOM_E_ATTR)->getAttributeValueWithoutScript<int>();
    sl_evChanged(val);
}

//Prototype
//////////////////////////////////////////////////////////////////////////

HMM2QDActorPrototype::HMM2QDActorPrototype() {
    descriptor.setId("hmm2");
    descriptor.setDisplayName(HMM2QDActor::tr("HMM2"));
    descriptor.setDocumentation(HMM2QDActor::tr(
        "Searches HMM signals in a sequence with one or more profile HMM2"
        " and saves the results as annotations."));

    {
        Descriptor pd(PROFILE_ATTR,
            HMM2QDActor::tr("Profile HMM"),
            HMM2QDActor::tr("Semicolon-separated list of input HMM files."));

        Descriptor mind(MIN_LEN,
            HMM2QDActor::tr("Min Length"),
            HMM2QDActor::tr("Minimum length of a result region."));

        Descriptor maxd(MAX_LEN,
            HMM2QDActor::tr("Max Length"),
            HMM2QDActor::tr("Maximum length of a result region."));

        Descriptor ded(DOM_E_ATTR,
            HMM2QDActor::tr("Filter by High E-value"),
            HMM2QDActor::tr("Reports domains &lt;= this E-value threshold"
            " in output."));

        Descriptor dtd(DOM_T_ATTR,
            HMM2QDActor::tr("Filter by Low Score"),
            HMM2QDActor::tr("Reports domains &gt;= this score cutoff in output."));

        Descriptor nsd(NSEQ_ATTR,
            HMM2QDActor::tr("Number of Sequences"),
            HMM2QDActor::tr("Specifies number of significant sequences."
            " It is used for domain E-value calculations."));

        attributes << new Attribute(pd, BaseTypes::STRING_TYPE(), true);
        attributes << new Attribute(mind, BaseTypes::NUM_TYPE(), false, QVariant(10));
        attributes << new Attribute(maxd, BaseTypes::NUM_TYPE(), false, QVariant(1000));
        attributes << new Attribute(ded, BaseTypes::NUM_TYPE(), false, QVariant(1));
        attributes << new Attribute(dtd, BaseTypes::NUM_TYPE(), false, QVariant(0.01));
        attributes << new Attribute(nsd, BaseTypes::NUM_TYPE(), false, QVariant(1));
    }

    QMap<QString, PropertyDelegate*> delegates;

    {
        delegates[PROFILE_ATTR] = new URLDelegate(
            HMMIO::getHMMFileFilter(),
            HMMIO::HMM_ID, true/*multi*/);
        
        QVariantMap eMap; eMap["prefix"]= ("1e+"); eMap["minimum"] = (-99); eMap["maximum"] = (1);
        delegates[DOM_E_ATTR] = new SpinBoxDelegate(eMap);

        QVariantMap nMap; nMap["maximum"] = 999999999.00; nMap["minimum"] = 0.00;
        delegates[NSEQ_ATTR] = new DoubleSpinBoxDelegate(nMap);

        QVariantMap tMap; tMap["decimals"]= (2); tMap["minimum"] = 0.01; tMap["maximum"] = 99.99;
        tMap["singleStep"] = (1.0);
        delegates[DOM_T_ATTR] = new DoubleSpinBoxDelegate(tMap);

        QVariantMap lenMap; lenMap["minimum"] = 2; lenMap["maximum"] = INT_MAX;
        delegates[MIN_LEN] = new SpinBoxDelegate(lenMap);
        delegates[MAX_LEN] = new SpinBoxDelegate(lenMap);
    }

    editor = new DelegateEditor(delegates);
}

} //namespace
