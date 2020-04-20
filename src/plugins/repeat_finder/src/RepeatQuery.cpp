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

#include <U2Core/FailTask.h>
#include <U2Core/L10n.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/QDConstraint.h>

#include "FindRepeatsDialog.h"
#include "RepeatQuery.h"
#include "RepeatWorker.h"

namespace U2 {

static const QString LEN_ATTR("min-length");
static const QString IDENTITY_ATTR("identity");
static const QString MIN_DIST_ATTR("min_dist");
static const QString MAX_DIST_ATTR("max_dist");
static const QString INVERT_ATTR("invert");
static const QString NESTED_ATTR("filter-algorithm");
static const QString ALGO_ATTR("algorithm");
static const QString THREADS_ATTR("threads");
static const QString MAX_LEN_ATTR("max-length");
static const QString TANMEDS_ATTR("exclude-tandems");

QDRepeatActor::QDRepeatActor(QDActorPrototype const* proto) : QDActor(proto) {
    simmetric = true;
    cfg->setAnnotationKey("repeat_unit");
    units["left"] = new QDSchemeUnit(this);
    units["right"] = new QDSchemeUnit(this);
    paramConstraints << new QDDistanceConstraint(units.values(), E2S, 0, 5000);
}

static const QString ALGO_SUFFIX = "suffix";
static const QString ALGO_DIAG = "diagonals";
static const QString ALGO_AUTO = "auto";

static const QString FA_DISJOINT = "Disjoint repeats";
static const QString FA_NOFILTERING = "NoFiltering";
static const QString FA_UNIQUE = "UniqueRepeats";


QList< QPair<QString,QString> > QDRepeatActor::saveConfiguration() const
{
    QList< QPair<QString,QString> > res = QDActor::saveConfiguration();
    Attribute* a = cfg->getParameter(ALGO_ATTR);
    for (int i=0; i<res.size(); i++)
    {
        QPair<QString, QString>& attr = res[i];
        if (attr.first==a->getId())
        {
            RFAlgorithm alg = RFAlgorithm(a->getAttributeValueWithoutScript<int>());
            switch (alg)
            {
            case RFAlgorithm_Auto:
                attr.second = ALGO_AUTO;
                break;
            case RFAlgorithm_Diagonal:
                attr.second = ALGO_DIAG;
                break;
            case RFAlgorithm_Suffix:
                attr.second = ALGO_SUFFIX;
                break;
            default:
                break;
            }
        }
    }

    for (int i=0; i<res.size(); i++)
    {
        QPair<QString, QString>& attr = res[i];
        if (attr.first==a->getId())
        {
            RepeatsFilterAlgorithm falg = RepeatsFilterAlgorithm(a->getAttributeValueWithoutScript<int>());
            switch (falg)
            {
            case DisjointRepeats:
                attr.second = FA_DISJOINT;
                break;
            case NoFiltering:
                attr.second = FA_NOFILTERING;
                break;
            case UniqueRepeats:
                attr.second = FA_UNIQUE;
                break;
            default:
                break;
            }
        }
    }

    QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(paramConstraints.first());
    int minDist = dc->getMin();
    int maxDist = dc->getMax();

    res.append(qMakePair(MIN_DIST_ATTR, QString::number(minDist)));
    res.append(qMakePair(MAX_DIST_ATTR, QString::number(maxDist)));

    return res;
}

void QDRepeatActor::loadConfiguration(const QList< QPair<QString,QString> >& strMap)
{
    QDActor::loadConfiguration(strMap);
    QString minDistStr;
    QString maxDistStr;
    foreach(const StringAttribute& attr, strMap)
    {
        if (attr.first==ALGO_ATTR)
        {
            int alg;
            const QString& strandVal = attr.second;
            if (strandVal==ALGO_AUTO)
            {
                alg = 0;
            }
            else if (strandVal==ALGO_DIAG)
            {
                alg = 1;
            }
            else if (strandVal==ALGO_SUFFIX)
            {
                alg = 2;
            }
            cfg->setParameter(ALGO_ATTR, qVariantFromValue(alg));
        }
        else if (attr.first==NESTED_ATTR)
        {
            int falg;
            const QString& strandVal = attr.second;
            if (strandVal==FA_DISJOINT)
            {
                falg = 0;
            }
            else if (strandVal==FA_NOFILTERING)
            {
                falg = 1;
            }
            else if (strandVal==FA_UNIQUE)
            {
                falg = 2;
            }
            cfg->setParameter(NESTED_ATTR, qVariantFromValue(falg));
        }
        else if (attr.first == MIN_DIST_ATTR)
        {
            minDistStr = attr.second;
        }
        else if (attr.first == MAX_DIST_ATTR)
        {
            maxDistStr = attr.second;
        }
    }

    if(!minDistStr.isEmpty() && !maxDistStr.isEmpty()){
        bool ok = false;
        int minDist = minDistStr.toInt(&ok);
        if(!ok){
            return;
        }
        int maxDist = maxDistStr.toInt(&ok);
        if(!ok || minDist > maxDist || paramConstraints.size() != 1){
            return;
        }

        paramConstraints.clear();
        paramConstraints << new QDDistanceConstraint(units.values(), E2S, minDist, maxDist);
    }
}

int QDRepeatActor::getMinResultLen() const {
    return cfg->getParameter(LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

int QDRepeatActor::getMaxResultLen() const {
    return cfg->getParameter(MAX_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

QString QDRepeatActor::getText() const {
    QString inverted = cfg->getParameter(INVERT_ATTR)->getAttributePureValue().toBool() ?
        QDRepeatActor::tr("inverted") : QDRepeatActor::tr("direct");

    inverted = QString("<a href=%1>%2</a>").arg(INVERT_ATTR).arg(inverted);

    int identity = cfg->getParameter(IDENTITY_ATTR)->getAttributeValueWithoutScript<int>();
    QString identityStr = QString("<a href=%1>%2%</a>").arg(IDENTITY_ATTR).arg(identity);

    int minLen = cfg->getParameter(LEN_ATTR)->getAttributeValueWithoutScript<int>();
    QString minLenStr = QString("<a href=%1>%2</a>").arg(LEN_ATTR).arg(minLen);

    QString doc = QDRepeatActor::tr("Finds <u>%1</u> repeats."
        "<br>Detects <u>%2 identical</u> repeats <u>not shorter than %3 bps</u>.")
        .arg(inverted)
        .arg(identityStr)
        .arg(minLenStr);

    return doc;
}

Task* QDRepeatActor::getAlgorithmTask(const QVector<U2Region>& location) {
    assert(scheme);
    Task* t = NULL;

    settings.algo = RFAlgorithm(cfg->getParameter(ALGO_ATTR)->getAttributeValueWithoutScript<int>());
    settings.minLen = cfg->getParameter(LEN_ATTR)->getAttributeValueWithoutScript<int>();
    int identity = cfg->getParameter(IDENTITY_ATTR)->getAttributeValueWithoutScript<int>();
    settings.setIdentity(identity);
    settings.nThreads = cfg->getParameter(THREADS_ATTR)->getAttributeValueWithoutScript<int>();
    settings.inverted = cfg->getParameter(INVERT_ATTR)->getAttributeValueWithoutScript<bool>();
    settings.filter = RepeatsFilterAlgorithm(cfg->getParameter(NESTED_ATTR)->getAttributeValueWithoutScript<int>());
    settings.excludeTandems = cfg->getParameter(TANMEDS_ATTR)->getAttributeValueWithoutScript<bool>();

    QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(paramConstraints.first());
    settings.minDist = dc->getMin();
    settings.maxDist = dc->getMax();

    assert(identity <= 100 && identity >= 0);
    assert(settings.minDist>=0);
    const DNASequence& dnaSeq = scheme->getSequence();
    if (!dnaSeq.alphabet->isNucleic()) {
        return new FailTask(tr("Sequence should be nucleic"));
    }

    t = new Task(tr("RepeatQDTask"), TaskFlag_NoRun);
    foreach(const U2Region& r, location) {
        FindRepeatsTaskSettings stngs(settings);
        stngs.seqRegion = r;
        FindRepeatsToAnnotationsTask* st = new FindRepeatsToAnnotationsTask(stngs, dnaSeq,
                                                            "repeat unit", QString(), "", GObjectReference());
        t->addSubTask(st);
        repTasks.append(st);
    }
    connect(new TaskSignalMapper(t), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onAlgorithmTaskFinished()));
    return t;
}

void QDRepeatActor::sl_onAlgorithmTaskFinished() {
    QList<SharedAnnotationData> res;
    foreach (FindRepeatsToAnnotationsTask *frt, repTasks) {
        res = frt->importAnnotations();
    }
    repTasks.clear();
    foreach(const SharedAnnotationData &ad, res) {
        if (ad->location->regions[0].length > getMaxResultLen()) {
            continue;
        }

        //////////////////////////////////////////////////////////////////////////
        QDResultUnit ru1(new QDResultUnitData);
        QDResultUnit ru2(new QDResultUnitData);
        ru1->quals = ad->qualifiers;
        ru2->quals = ad->qualifiers;
        ru1->region = ad->location->regions[0];
        ru2->region = ad->location->regions[1];
        ru1->owner = units.value("left");
        ru2->owner = units.value("right");
        ru1->strand = U2Strand::Direct;
        ru2->strand = U2Strand::Direct;
        QDResultGroup* g = new QDResultGroup(QDStrand_Both);
        g->add(ru1);
        g->add(ru2);
        results.append(g);
    }
}

QDRepeatActorPrototype::QDRepeatActorPrototype() {
    descriptor.setId("repeats");
    descriptor.setDisplayName(QDRepeatActor::tr("Repeats"));
    descriptor.setDocumentation(QDRepeatActor::tr("Finds repeats in supplied sequence, stores found regions as annotations."));

    Descriptor idd(IDENTITY_ATTR, QDRepeatActor::tr("Identity"), QDRepeatActor::tr("Repeats identity."));
    Descriptor ld(LEN_ATTR, QDRepeatActor::tr("Min length"), QDRepeatActor::tr("Minimum length of repeats."));
    Descriptor ind(INVERT_ATTR, QDRepeatActor::tr("Inverted"), QDRepeatActor::tr("Search for inverted repeats."));
    Descriptor nsd(NESTED_ATTR, QDRepeatActor::tr("Filter algorithm"), QDRepeatActor::tr("Filter nested repeats algorithm."));
    Descriptor ald(ALGO_ATTR, QDRepeatActor::tr("Algorithm"), QDRepeatActor::tr("Control over variations of algorithm."));
    Descriptor thd(THREADS_ATTR, QDRepeatActor::tr("Parallel threads"), QDRepeatActor::tr("Number of parallel threads used for the task."));
    Descriptor mld(MAX_LEN_ATTR, QDRepeatActor::tr("Max length"), QDRepeatActor::tr("Maximum length of repeats."));
    Descriptor tan(TANMEDS_ATTR, QDRepeatActor::tr("Exclude tandems"), QDRepeatActor::tr("Exclude tandems areas before find repeat task is run."));

    FindRepeatsTaskSettings stngs = LocalWorkflow::RepeatWorkerFactory::defaultSettings();

    attributes << new Attribute(ld, BaseTypes::NUM_TYPE(), true, stngs.minLen);
    attributes << new Attribute(idd, BaseTypes::NUM_TYPE(), false, stngs.getIdentity());
    attributes << new Attribute(ind, BaseTypes::BOOL_TYPE(), false, stngs.inverted);
    attributes << new Attribute(nsd, BaseTypes::NUM_TYPE(), false, stngs.filter);
    attributes << new Attribute(ald, BaseTypes::NUM_TYPE(), false, stngs.algo);
    attributes << new Attribute(thd, BaseTypes::NUM_TYPE(), false, stngs.nThreads);
    attributes << new Attribute(mld, BaseTypes::NUM_TYPE(), true, QDActor::DEFAULT_MAX_RESULT_LENGTH);
    attributes << new Attribute(tan, BaseTypes::BOOL_TYPE(), false, stngs.excludeTandems);

    QMap<QString, PropertyDelegate*> delegates;
    {
        QVariantMap m;
        m["minimum"] = 2;
        m["maximum"] = INT_MAX;
        m["suffix"] = L10N::suffixBp();
        delegates[LEN_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["minimum"] = 0;
        m["maximum"] = INT_MAX;
        m["suffix"] = L10N::suffixBp();
        delegates[MAX_LEN_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["minimum"] = 50;
        m["maximum"] = 100;
        m["suffix"] = "%";
        delegates[IDENTITY_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["specialValueText"] = "Auto";
        delegates[THREADS_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["Auto"] = RFAlgorithm_Auto;
        m["Diagonals"] = RFAlgorithm_Diagonal;
        m["Suffix index"] = RFAlgorithm_Suffix;
        delegates[ALGO_ATTR] = new ComboBoxDelegate(m);
    }
    {
        QVariantMap m;
        m["Disjoint repeats"] = DisjointRepeats;
        m["No filtering"] = NoFiltering;
        m["Unique repeats"] = UniqueRepeats;
        delegates[NESTED_ATTR] = new ComboBoxDelegate(m);
    }

    editor = new DelegateEditor(delegates);
}

}//namespace
