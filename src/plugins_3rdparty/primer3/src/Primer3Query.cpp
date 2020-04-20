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

#include "Primer3Query.h"
#include "Primer3Task.h"
#include "Primer3Dialog.h"

#include <U2Designer/DelegateEditors.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/Log.h>
#include <U2Lang/BaseTypes.h>

namespace U2 {

static const QString LEFT_PRIMER_ID = "left";
static const QString RIGHT_PRIMER_ID = "right";

static const QString EXCLUDED_REGIONS_ATTR("excluded_regions");
static const QString TARGETS_ATTR("targets");
static const QString SIZE_RANGES_ATTR("size_ranges");
static const QString START_CODON_ATTR("start_codon_pos");
static const QString NUM_RETURN_ATTR("num_return");
static const QString MAX_MISPRIMING_ATTR("max_mispriming");
static const QString MAX_TEMPLATE_MISPRIMING_ATTR("max_template_mispriming");
static const QString STABILITY_ATTR("stability");
static const QString PAIR_MAX_MISPRIMING_ATTR("pair_max_mispriming");
static const QString PAIR_MAX_TEMPLATE_MISPRIMING_ATTR("pair_max_template_mispriming");

QDPrimerActor::QDPrimerActor(QDActorPrototype const* proto) : QDActor(proto) {
    cfg->setAnnotationKey("top primers");
    units[LEFT_PRIMER_ID] = new QDSchemeUnit(this);
    units[RIGHT_PRIMER_ID] = new QDSchemeUnit(this);
    setDefaultSettings();
}

QString QDPrimerActor::getText() const {
    return "Searches primers";
}

Task* QDPrimerActor::getAlgorithmTask(const QVector<U2Region>& /*location*/ ) {
    Task* t = NULL;
    const DNASequence& dnaSeq = scheme->getSequence();
    settings.setSequence(dnaSeq.seq);

    U2Region seqRange(0, dnaSeq.length());
    settings.setIncludedRegion(seqRange.startPos + settings.getFirstBaseIndex(), seqRange.length);

    QList< U2Region > list;
    const QString& excludedRegsStr = cfg->getParameter(EXCLUDED_REGIONS_ATTR)->getAttributeValueWithoutScript<QString>();
    bool ok = Primer3Dialog::parseIntervalList(excludedRegsStr, ",", &list);
    if (ok) {
        settings.setExcludedRegion(list);
    } else {
        algoLog.error(tr("%1 invalid input. Excluded regions.").arg(cfg->getLabel()));
        return NULL;
    }

    const QString& targetsStr = cfg->getParameter(TARGETS_ATTR)->getAttributeValueWithoutScript<QString>();
    ok = Primer3Dialog::parseIntervalList(targetsStr, ",", &list);
    if (ok) {
        settings.setTarget(list);
    } else {
        algoLog.error(tr("%1 invalid input. Targets.").arg(cfg->getLabel()));
        return NULL;
    }

    const QString& sizeRangesAttr = cfg->getParameter(SIZE_RANGES_ATTR)->getAttributeValueWithoutScript<QString>();
    ok = Primer3Dialog::parseIntervalList(sizeRangesAttr, "-", &list, Primer3Dialog::Start_End);
    if (ok) {
        settings.setProductSizeRange(list);
    } else {
        algoLog.error(tr("%1 invalid input. Product size ranges.").arg(cfg->getLabel()));
    }

    int numRet = cfg->getParameter(NUM_RETURN_ATTR)->getAttributeValueWithoutScript<int>();
    settings.setIntProperty("PRIMER_NUM_RETURN", numRet);

    qreal maxMispriming = cfg->getParameter(MAX_MISPRIMING_ATTR)->getAttributeValueWithoutScript<double>();
    settings.setAlignProperty("PRIMER_MAX_MISPRIMING", maxMispriming);
    assert(settings.getAlignPropertyList().contains("PRIMER_MAX_MISPRIMING"));

    qreal maxTemplateMispriming = cfg->getParameter(MAX_TEMPLATE_MISPRIMING_ATTR)->getAttributeValueWithoutScript<double>()*100;
    settings.setAlignProperty("PRIMER_MAX_TEMPLATE_MISPRIMING", maxTemplateMispriming);
    assert(settings.getAlignPropertyList().contains("PRIMER_MAX_TEMPLATE_MISPRIMING"));

    qreal stability = cfg->getParameter(STABILITY_ATTR)->getAttributeValueWithoutScript<double>();
    settings.setDoubleProperty("PRIMER_MAX_END_STABILITY", stability);
    assert(settings.getDoublePropertyList().contains("PRIMER_MAX_END_STABILITY"));

    qreal pairMispriming = cfg->getParameter(PAIR_MAX_MISPRIMING_ATTR)->getAttributeValueWithoutScript<double>();
    settings.setAlignProperty("PRIMER_PAIR_MAX_MISPRIMING", pairMispriming);
    assert(settings.getAlignPropertyList().contains("PRIMER_PAIR_MAX_MISPRIMING"));

    qreal pairtemplateMispriming = cfg->getParameter(PAIR_MAX_TEMPLATE_MISPRIMING_ATTR)->getAttributeValueWithoutScript<double>()*100;
    settings.setAlignProperty("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING", pairtemplateMispriming);
    assert(settings.getAlignPropertyList().contains("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING"));

    t = new Primer3SWTask(settings);
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished(Task*)));

    return t;
}

void QDPrimerActor::sl_onAlgorithmTaskFinished(Task* t) {
    Primer3SWTask* primerTask = qobject_cast<Primer3SWTask*>(t);
    assert(primerTask);
    QList<PrimerPair> bestPairs = primerTask->getBestPairs();
    foreach(PrimerPair pair, bestPairs) {
        QList<SharedAnnotationData> annotations;
        Primer* leftPrimer = pair.getLeftPrimer();
        Primer* rightPrimer = pair.getRightPrimer();
        if (leftPrimer!=NULL && rightPrimer!=NULL) {
            QDResultUnit ru1(new QDResultUnitData);
            ru1->strand = U2Strand::Direct;
            ru1->region = U2Region(leftPrimer->getStart(), leftPrimer->getLength());
            ru1->owner = units.value(LEFT_PRIMER_ID);
            QDResultUnit ru2(new QDResultUnitData);
            ru2->strand = U2Strand::Complementary;
            ru2->region = U2Region(rightPrimer->getStart()-rightPrimer->getLength()-1,
                rightPrimer->getLength());
            ru2->owner = units.value(RIGHT_PRIMER_ID);
            QDResultGroup* g = new QDResultGroup;
            g->add(ru1);
            g->add(ru2);
            results.append(g);
        }
    }
}

void QDPrimerActor::setDefaultSettings() {
    {
        QList< U2Region > sizeRange;
        sizeRange.append(U2Region(150, 101));  // 150-250
        sizeRange.append(U2Region(100, 201));  // 100-300
        sizeRange.append(U2Region(301, 100));  // 301-400
        sizeRange.append(U2Region(401, 100));  // 401-500
        sizeRange.append(U2Region(501, 100));  // 501-600
        sizeRange.append(U2Region(601, 100));  // 601-700
        sizeRange.append(U2Region(701, 150));  // 701-850
        sizeRange.append(U2Region(851, 150));  // 851-1000
        settings.setProductSizeRange(sizeRange);
    }
    settings.setDoubleProperty("PRIMER_MAX_END_STABILITY",9.0);
    settings.setAlignProperty("PRIMER_MAX_TEMPLATE_MISPRIMING",1200);
    settings.setAlignProperty("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING",2400);
    settings.setIntProperty("PRIMER_LIBERAL_BASE",1);
    settings.setDoubleProperty("PRIMER_WT_POS_PENALTY",0.0);
    settings.setIntProperty("PRIMER_FIRST_BASE_INDEX",1);
}

QDPrimerActorPrototype::QDPrimerActorPrototype() {
    descriptor.setId("primer");
    descriptor.setDisplayName(QObject::tr("Primer"));
    descriptor.setDocumentation(QObject::tr("PCR primer design"));

    Descriptor erd(EXCLUDED_REGIONS_ATTR, QObject::tr("Excluded regions"), QObject::tr("Excluded regions"));
    Descriptor td(TARGETS_ATTR, QObject::tr("Targets"), QObject::tr("Targets"));
    Descriptor srd(SIZE_RANGES_ATTR, QObject::tr("Product size ranges"), QObject::tr("Product size ranges"));
    Descriptor nrd(NUM_RETURN_ATTR, QObject::tr("Number to return"), QObject::tr("Number to return"));
    //Descriptor scd(START_CODON_ATTR, QObject::tr("Start codon position"), QObject::tr("Start codon position"));
    Descriptor mmd(MAX_MISPRIMING_ATTR, QObject::tr("Max repeat mispriming"), QObject::tr("Max repeat mispriming"));
    Descriptor mtmd(MAX_TEMPLATE_MISPRIMING_ATTR, QObject::tr("Max template mispriming"), QObject::tr("Max template mispriming"));
    Descriptor sd(STABILITY_ATTR, QObject::tr("Max 3' stability"), QObject::tr("Max 3' stability"));
    Descriptor pmmd(PAIR_MAX_MISPRIMING_ATTR, QObject::tr("Pair max repeat mispriming"), QObject::tr("Pair max repeat mispriming"));
    Descriptor pmtmd(PAIR_MAX_TEMPLATE_MISPRIMING_ATTR, QObject::tr("Pair max template mispriming"), QObject::tr("Pair max template mispriming"));

    attributes << new Attribute(erd, BaseTypes::STRING_TYPE(), false);
    attributes << new Attribute(td, BaseTypes::STRING_TYPE(), false);
    QString sizeRangeDefVal = "150-200 100-300 301-400 401-500 501-600 601-700 701-850 851-1000";
    attributes << new Attribute(srd, BaseTypes::STRING_TYPE(), true, sizeRangeDefVal);
    //a << new Attribute(scd, BaseTypes::STRING_TYPE(), false);
    attributes << new Attribute(nrd, BaseTypes::NUM_TYPE(), false, 5);
    attributes << new Attribute(mmd, BaseTypes::NUM_TYPE(), false, 12.0);
    attributes << new Attribute(mtmd, BaseTypes::NUM_TYPE(), false, 12.0);
    attributes << new Attribute(sd, BaseTypes::NUM_TYPE(), false, 9.0);
    attributes << new Attribute(pmmd, BaseTypes::NUM_TYPE(), false, 24.0);
    attributes << new Attribute(pmtmd, BaseTypes::NUM_TYPE(), false, 24.0);

    QMap<QString, PropertyDelegate*> delegates;
    QVariantMap lenMap;
    lenMap["minimum"] = QVariant(0);
    lenMap["maximum"] = QVariant(9999);
    delegates[NUM_RETURN_ATTR] = new SpinBoxDelegate(lenMap);
    delegates[MAX_MISPRIMING_ATTR] = new DoubleSpinBoxDelegate(lenMap);
    delegates[MAX_TEMPLATE_MISPRIMING_ATTR] = new DoubleSpinBoxDelegate(lenMap);
    delegates[STABILITY_ATTR] = new DoubleSpinBoxDelegate(lenMap);
    delegates[PAIR_MAX_MISPRIMING_ATTR] = new DoubleSpinBoxDelegate(lenMap);
    delegates[PAIR_MAX_TEMPLATE_MISPRIMING_ATTR] = new DoubleSpinBoxDelegate(lenMap);

    editor = new DelegateEditor(delegates);
}

}//namespace
