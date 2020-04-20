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

#include "QDFindPolyRegionsActor.h"

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

void FindPolyRegionsTask::find(const char* seq,
                               qint64 seqLen,
                               char ch,
                               int percent,
                               qint64 len,
                               QVector<U2Region>& result)
{
    assert(len <= seqLen);
    qreal reqChNumReal = len*percent/100.0;
    qint64 reqChNum = reqChNumReal;
    if ( reqChNumReal > reqChNum ) {
        ++reqChNum;
    }

    U2Region lastFound;

    qint64 chNum = 0;
    for (qint64 i=0; i<len; i++) {
        if (seq[i]==ch) {
            ++chNum;
        }
    }

    if (chNum>=reqChNum) {
        lastFound.startPos = 0;
        lastFound.length = len;
    }

    for (qint64 i=len; i<seqLen; i++) {
        qint64 startPos = i - len + 1;
        if (seq[startPos - 1]==ch && chNum > 0) {
            --chNum;
        }
        if (seq[i]==ch) {
            ++chNum;
        }
        if (chNum>=reqChNum) {
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

void FindPolyRegionsTask::run() {
    if (settings_.strand == QDStrand_DirectOnly || settings_.strand == QDStrand_Both) {
        find(sequence_.seq.constData(), sequence_.seq.length(), settings_.ch,
            settings_.percent, settings_.minLen, directResults);
    }

    if(settings_.strand == QDStrand_ComplementOnly || settings_.strand == QDStrand_Both) {
        assert(settings_.complTT);
        char ch = settings_.ch;
        TextUtils::translate(settings_.complTT->getOne2OneMapper(), &ch, 1);
        find(sequence_.seq.constData(), sequence_.seq.length(), ch,
            settings_.percent, settings_.minLen, compResults);
    }
}

QList<SharedAnnotationData> FindPolyRegionsTask::getResultAsAnnotations() const {
    QList<SharedAnnotationData> res;
    res += createAnnotations(directResults, settings_.offset, U2Strand::Direct);
    res += createAnnotations(compResults, settings_.offset, U2Strand::Complementary);
    return res;
}

QList<SharedAnnotationData> FindPolyRegionsTask::createAnnotations(const QVector<U2Region>& regions, qint64 offset, U2Strand::Direction strand) {
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

static const QString UNIT_ID("find-poly");

static const QString BASE_ATTR("base");
static const QString PERCENT_ATTR("percent");
static const QString MIN_LEN_ATTR("min-len");
static const QString MAX_LEN_ATTR("max-len");

QDFindPolyActor::QDFindPolyActor( QDActorPrototype const* proto ) : QDActor(proto) {
    units[UNIT_ID] = new QDSchemeUnit(this);
}

int QDFindPolyActor::getMinResultLen() const {
    return cfg->getParameter(MIN_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

int QDFindPolyActor::getMaxResultLen() const {
    return cfg->getParameter(MAX_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

QString QDFindPolyActor::getText() const {
    return tr("Searches regions in a sequence that contain a specified percentage of a certain base.");
}

Task* QDFindPolyActor::getAlgorithmTask( const QVector<U2Region>& location ) {
    const DNASequence& sequence = scheme->getSequence();
    FindPolyRegionsSettings settings;

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

    QString baseStr = cfg->getParameter(BASE_ATTR)->getAttributeValueWithoutScript<QString>();

    if (baseStr.size()!=1) {
        QString err = tr("'%1' error. Incorrect value of 'Base' parameter.").arg(cfg->getLabel());
        return new FailTask(err);
    }

    settings.ch = baseStr.at(0).toLatin1();

    int percent = cfg->getParameter(PERCENT_ATTR)->getAttributeValueWithoutScript<int>();
    if (percent < 50 || percent > 100) {
        algoLog.error(tr("'%1' error. Percent parameter should be not less than 50% not higher than 100%. Setting up default value: 90%."));
        settings.percent = 90;
    } else {
        settings.percent = percent;
    }

    int minLen = cfg->getParameter(MIN_LEN_ATTR)->getAttributeValueWithoutScript<int>();
    if (minLen<5 || minLen>sequence.length()) {
        QString err = tr("'%1' error. Min length should be not less than 5 and not higher than sequence length.").arg(cfg->getLabel());
        return new FailTask(err);
    }

    settings.minLen = minLen;

    Task* t = new Task(tr("Search poly regions QD task"), TaskFlag_NoRun);

    foreach(U2Region r, location) {
        FindPolyRegionsSettings stngs(settings);
        stngs.offset = r.startPos;
        FindPolyRegionsTask* sub = new FindPolyRegionsTask(stngs, sequence);
        t->addSubTask(sub);
        connect(new TaskSignalMapper(sub), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onTaskFinished(Task*)));
    }

    return t;
}

void QDFindPolyActor::sl_onTaskFinished(Task* t) {
    FindPolyRegionsTask* fprt = qobject_cast<FindPolyRegionsTask*>(t);
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

QDFindPolyActorPrototype::QDFindPolyActorPrototype() {
    descriptor.setId("base-content");
    descriptor.setDisplayName(QDFindPolyActor::tr("Base Content"));
    descriptor.setDocumentation(QDFindPolyActor::tr(
        "Searches regions in a sequence that contain a specified percentage"
        " of a certain base."));

    Descriptor bd(BASE_ATTR,
        QDFindPolyActor::tr("Base"),
        QDFindPolyActor::tr("Specifies the base."));

    Descriptor pd(PERCENT_ATTR,
        QDFindPolyActor::tr("Percentage"),
        QDFindPolyActor::tr("Percentage of the base in a region."));

    Descriptor mind(MIN_LEN_ATTR,
        QDFindPolyActor::tr("Min Length"),
        QDFindPolyActor::tr("Minimum length of a region."));

    Descriptor maxd(MAX_LEN_ATTR,
        QDFindPolyActor::tr("Max Length"),
        QDFindPolyActor::tr("Maximum length of a region."));

    attributes << new Attribute(bd, BaseTypes::STRING_TYPE(), true);
    attributes << new Attribute(pd, BaseTypes::NUM_TYPE(), true, QVariant(90));
    attributes << new Attribute(mind, BaseTypes::NUM_TYPE(), false, QVariant(50));
    attributes << new Attribute(maxd, BaseTypes::NUM_TYPE(), false, QVariant(1000));

    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap bm;
        bm.insert("A","A"); bm.insert("G","G"); bm.insert("C","C"); bm.insert("T","T");
        delegates[BASE_ATTR] = new ComboBoxDelegate(bm);

        QVariantMap m; m["minimum"] = 50; m["maximum"] = 100; m["suffix"] = "%";
        delegates[PERCENT_ATTR] = new SpinBoxDelegate(m);

        QVariantMap lenMap; lenMap["minimum"] = 5; lenMap["maximum"]=INT_MAX; lenMap["suffix"] = " bp";
        delegates[MIN_LEN_ATTR] = new SpinBoxDelegate(lenMap);
        delegates[MAX_LEN_ATTR] = new SpinBoxDelegate(lenMap);
    }

    editor = new DelegateEditor(delegates);
}

} //namespace
