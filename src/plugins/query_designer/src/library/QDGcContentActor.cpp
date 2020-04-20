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

#include "QDGcContentActor.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2Region.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/FailTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/TextUtils.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Lang/BaseTypes.h>


namespace U2 {

void FindGcRegionsTask::find(const char* seq,
    qint64 seqLen,
    U2Region gcRangeInPercents,
    qint64 len,
    QVector<U2Region>& result)
{
    assert(len <= seqLen);

    U2Region lastFound;

    qint64 gcCount = 0;
    qreal maxGcCount = qreal(gcRangeInPercents.endPos() * len) / 100;
    qreal minGcCount = qreal(gcRangeInPercents.startPos * len) / 100;
    for (qint64 i=0; i<len; i++) {
        if (seq[i] == 'C' || seq[i] == 'G') {
            ++gcCount;
        }
    }

    if (gcCount >= minGcCount && gcCount <= maxGcCount) {
        lastFound.startPos = 0;
        lastFound.length = len;
    }

    for (qint64 i=len; i<seqLen; i++) {
        qint64 startPos = i - len + 1;
        if ((seq[startPos - 1]=='C' || seq[startPos - 1]=='G') && gcCount > 0) {
            --gcCount;
        }
        if (seq[i] == 'C' || seq[i] == 'G') {
            ++gcCount;
        }
        if (gcCount >= minGcCount && gcCount <= maxGcCount) {
            if (lastFound.endPos()>=startPos) {
                lastFound.length = i + 1 - lastFound.startPos;
            } else {
                result.append(lastFound);
                lastFound.startPos = startPos;
                lastFound.length = len;
            }
        }
    }

    result.append(lastFound);

    //remove (0,0) region located in the beginning of the array
    if (!result.isEmpty() && result.first().isEmpty()) {
        result.remove(0);
    }
}

void FindGcRegionsTask::run() {
    if (settings_.strand == QDStrand_DirectOnly || settings_.strand == QDStrand_Both) {
        find(sequence_.seq.constData(), sequence_.seq.length(),
            settings_.gcRangeInPercents, settings_.minLen, directResults);
    }

    if(settings_.strand == QDStrand_ComplementOnly || settings_.strand == QDStrand_Both) {
        assert(settings_.complTT);
        //TextUtils::translate(settings_.complTT->getOne2OneMapper(), &ch, 1);
        find(sequence_.seq.constData(), sequence_.seq.length(),
            settings_.gcRangeInPercents, settings_.minLen, compResults);
    }
}

QList<SharedAnnotationData> FindGcRegionsTask::getResultAsAnnotations() const {
    QList<SharedAnnotationData> res;
    res += createAnnotations(directResults, settings_.offset, U2Strand::Direct);
    res += createAnnotations(compResults, settings_.offset, U2Strand::Complementary);
    return res;
}

QList<SharedAnnotationData> FindGcRegionsTask::createAnnotations(const QVector<U2Region>& regions, qint64 offset, U2Strand::Direction strand) {
    QList<SharedAnnotationData> res;
    foreach(U2Region r, regions) {
        SharedAnnotationData d(new AnnotationData());
        r.startPos += offset;
        d->location->regions.append(r);
        d->location->strand = strand;
        res.append(d);
    }
    return res;
}

static const QString UNIT_ID("find-gc");

static const QString REGION_START_ATTR("region-start");
static const QString REGION_END_ATTR("region-end");
static const QString MIN_LEN_ATTR("min-len");
static const QString MAX_LEN_ATTR("max-len");

QDFindGcRegionsActor::QDFindGcRegionsActor( QDActorPrototype const* proto ) : QDActor(proto) {
    units[UNIT_ID] = new QDSchemeUnit(this);
}

int QDFindGcRegionsActor::getMinResultLen() const {
    return cfg->getParameter(MIN_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

int QDFindGcRegionsActor::getMaxResultLen() const {
    return cfg->getParameter(MAX_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

QString QDFindGcRegionsActor::getText() const {
    return tr("Searches regions in a sequence with GC content in the specified range.");
}

Task* QDFindGcRegionsActor::getAlgorithmTask( const QVector<U2Region>& location ) {
    const DNASequence& sequence = scheme->getSequence();
    FindGcRegionsSettings settings;

    settings.strand = getStrandToRun();
    if (settings.strand != QDStrand_DirectOnly) {
        DNATranslation* complTT = NULL;
        if (scheme->getSequence().alphabet->isNucleic()) {
            complTT = AppContext::getDNATranslationRegistry()->
                lookupComplementTranslation(scheme->getSequence().alphabet);
        }
        if (complTT != NULL) {
            settings.complTT = complTT;
        } else {
            QString err = tr("Could not find complement translation");
            return new FailTask(err);
        }
    }

    int regionStart = cfg->getParameter(REGION_START_ATTR)->getAttributeValueWithoutScript<int>();

    if (regionStart < 0 || regionStart > 100) {
        QString err = tr("'%1' error. Region start parameter should be not less than 0% not higher than 100%. Setting up default value: 20%.");
        settings.gcRangeInPercents.startPos = 20;
        regionStart = 20;
    }
    else {
        settings.gcRangeInPercents.startPos = regionStart;
    }

    int regionEnd = cfg->getParameter(REGION_END_ATTR)->getAttributeValueWithoutScript<int>();
    if (regionEnd < regionStart && regionEnd > 100) {
        algoLog.error(tr("'%1' error. Region end parameter should be not less than 'region start' not higher than 100%. Setting up default value: 40%."));
        settings.gcRangeInPercents.length = 20;
    } else {
        settings.gcRangeInPercents.length = regionEnd - regionStart;
    }

    int minLen = cfg->getParameter(MIN_LEN_ATTR)->getAttributeValueWithoutScript<int>();
    if (minLen<5 || minLen>sequence.length()) {
        QString err = tr("'%1' error. Min length should be not less than 5 and not higher than sequence length.").arg(cfg->getLabel());
        return new FailTask(err);
    }

    settings.minLen = minLen;

    Task* t = new Task(tr("Search GC regions QD task"), TaskFlag_NoRun);

    foreach(U2Region r, location) {
        FindGcRegionsSettings stngs(settings);
        stngs.offset = r.startPos;
        FindGcRegionsTask* sub = new FindGcRegionsTask(stngs, sequence);
        t->addSubTask(sub);
        connect(new TaskSignalMapper(sub), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)));
    }

    return t;
}

void QDFindGcRegionsActor::sl_onTaskFinished(Task* t) {
    FindGcRegionsTask* fprt = qobject_cast<FindGcRegionsTask*>(t);
    QList<SharedAnnotationData> annotations = fprt->getResultAsAnnotations();
    foreach(SharedAnnotationData d, annotations) {
        if (d->location->regions.first().length > getMaxResultLen()) {
            continue;
        }
        QDResultUnit ru(new QDResultUnitData);
        ru->region = d->location->regions.first();
        ru->strand = d->location->strand;
        ru->owner = units.value(UNIT_ID);
        QDResultGroup* g = new QDResultGroup(QDStrand_DirectOnly);
        g->add(ru);
        results.append(g);
    }
}

QDFindGcActorPrototype::QDFindGcActorPrototype() {
    descriptor.setId("gc-content");
    descriptor.setDisplayName(QDFindGcRegionsActor::tr("GC Content"));
    descriptor.setDocumentation(QDFindGcRegionsActor::tr(
        "Searches regions in a sequence with GC content in the specified range."));

    Descriptor minGcD(REGION_START_ATTR,
        QDFindGcRegionsActor::tr("Min GC content"),
        QDFindGcRegionsActor::tr("Min GC content value in percents."));

    Descriptor maxGcD(REGION_END_ATTR,
        QDFindGcRegionsActor::tr("Max GC content"),
        QDFindGcRegionsActor::tr("Max GC content value in percents."));

    Descriptor mind(MIN_LEN_ATTR,
        QDFindGcRegionsActor::tr("Min Length"),
        QDFindGcRegionsActor::tr("Minimum length of a region."));

    Descriptor maxd(MAX_LEN_ATTR,
        QDFindGcRegionsActor::tr("Max Length"),
        QDFindGcRegionsActor::tr("Maximum length of a region."));

    attributes << new Attribute(minGcD, BaseTypes::NUM_TYPE(), true, QVariant(20));
    attributes << new Attribute(maxGcD, BaseTypes::NUM_TYPE(), true, QVariant(40));
    attributes << new Attribute(mind, BaseTypes::NUM_TYPE(), false, QVariant(50));
    attributes << new Attribute(maxd, BaseTypes::NUM_TYPE(), false, QVariant(1000));

    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap bm; bm["minimum"] = 0; bm["maximum"] = 100; bm["suffix"] = "%";
        delegates[REGION_START_ATTR] = new SpinBoxDelegate(bm);

        QVariantMap m; m["minimum"] = 0; m["maximum"] = 100; m["suffix"] = "%";
        delegates[REGION_END_ATTR] = new SpinBoxDelegate(m);

        QVariantMap lenMap; lenMap["minimum"] = 5; lenMap["maximum"]=INT_MAX; lenMap["suffix"] = "bp";
        delegates[MIN_LEN_ATTR] = new SpinBoxDelegate(lenMap);
        delegates[MAX_LEN_ATTR] = new SpinBoxDelegate(lenMap);
    }

    editor = new DelegateEditor(delegates);
}

} //namespace
