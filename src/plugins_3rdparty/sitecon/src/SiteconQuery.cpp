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

#include "SiteconQuery.h"
#include "SiteconIO.h"

#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/BaseTypes.h>
#include <U2Designer/DelegateEditors.h>

#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FailTask.h>

#include <QApplication>
#include <QFileInfo>

namespace U2 {

static const QString SCORE_ATTR("score");
static const QString E1_ATTR("err1");
static const QString E2_ATTR("err2");
static const QString MODEL_ATTR("profile");
    
QDSiteconActor::QDSiteconActor(QDActorPrototype const* proto) : QDActor(proto) {
    units["sitecon"] = new QDSchemeUnit(this);
}

QString QDSiteconActor::getText() const {
    QString modelName;
    const QString& urlStr = cfg->getParameter(MODEL_ATTR)->getAttributeValueWithoutScript<QString>();
    const QStringList& urls = WorkflowUtils::expandToUrls(urlStr);

    QString modelHyp = QString("<a href=%1>").arg(MODEL_ATTR);
    if (urls.isEmpty() || urlStr.isEmpty()) {
        modelName = "unset";
    }
    else if (urls.size()==1) {
        QFileInfo fileInfo(urls.first());
        modelName = fileInfo.fileName();
    }
    else {
        modelName = tr("%1").arg(urls.size());
    }

    if (urls.isEmpty() || urls.size()==1) {
        modelName = tr("with profile provided by %1 %2</a>").arg(modelHyp).arg(modelName);
    }
    else {
        modelName = tr("with all %1 %2 profiles</a>").arg(modelHyp).arg(modelName);
    }

    QString scoreStr = tr("similarity %1%").arg(cfg->getParameter(SCORE_ATTR)->getAttributeValueWithoutScript<int>());
    scoreStr = QString("<a href=%1>%2</a>").arg(SCORE_ATTR).arg(scoreStr);
    
    QString strandName;
    switch (strand) {
    case QDStrand_Both: strandName = QDSiteconActor::tr("both strands"); break;
    case QDStrand_DirectOnly: strandName = QDSiteconActor::tr("direct strand"); break;
    case QDStrand_ComplementOnly: strandName = QDSiteconActor::tr("complement strand"); break;
    }

    QString doc = tr("Searches transcription factor binding sites (TFBS) %1."
        "<br>Recognize sites with %2, process %3.")
        .arg(modelName)
        .arg(scoreStr)
        .arg(strandName);

    return doc;
    return QString();
}

Task* QDSiteconActor::getAlgorithmTask(const QVector<U2Region>& location) {
    Task* t = NULL;
    assert(!location.isEmpty());

    const QString& urlStr = cfg->getParameter(MODEL_ATTR)->getAttributeValueWithoutScript<QString>();
    const QStringList& urls = WorkflowUtils::expandToUrls(urlStr);

    settings.minPSUM = cfg->getParameter(SCORE_ATTR)->getAttributeValueWithoutScript<int>();
    settings.minE1 = cfg->getParameter(E1_ATTR)->getAttributeValueWithoutScript<double>();
    settings.maxE2 = cfg->getParameter(E2_ATTR)->getAttributeValueWithoutScript<double>();
    
    if(urls.isEmpty()) {
        QString err = tr("%1: incorrect sitecon model url(s)").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    if(settings.minPSUM > 100 || settings.minPSUM < 60){
        QString err = tr("%1: min score can not be less 60% or more 100%").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    if(settings.minE1 > 1 || settings.minE1 < 0){
        QString err = tr("%1: min Err1 can not be less 0 or more 1").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    if(settings.maxE2 > 1 || settings.maxE2 < 0){
        QString err = tr("%1: max Err2 can not be less 0 or more 1").arg(getParameters()->getLabel());
        return new FailTask(err);
    }
    /*if(!(strand == 1 || strand == 0 || strand == -1)){
        QString err = tr("%1: search in strand can only be 0(both) or 1(direct) or -1(complement)")
            .arg(getParameters()->getLabel());
        return new FailTask(err);
    }*/

    const DNASequence& dnaSeq = scheme->getSequence();
    QDStrandOption stOp = getStrandToRun();
    if (stOp == QDStrand_ComplementOnly || stOp == QDStrand_Both) {
        DNATranslation* compTT = AppContext::getDNATranslationRegistry()->
            lookupComplementTranslation(dnaSeq.alphabet);
        if (compTT != NULL) {
            settings.complTT = compTT;
        }
    }
    
    t = new QDSiteconTask(urls, settings, dnaSeq, location);
    connect(new TaskSignalMapper(t),SIGNAL(si_taskFinished(Task*)),SLOT(sl_onAlgorithmTaskFinished(Task*)));
    return t;
}

void QDSiteconActor::sl_onAlgorithmTaskFinished(Task* t) {
    QDSiteconTask* st = qobject_cast<QDSiteconTask*>(t);
    assert(st);
    foreach(const SiteconSearchResult& res, st->getResults()) {
        const SharedAnnotationData ad = res.toAnnotation("");
        QDResultUnit ru(new QDResultUnitData);
        ru->strand = ad->getStrand();
        ru->quals = ad->qualifiers;
        ru->region = ad->location->regions.first();
        ru->owner = units.values().first();
        QDResultGroup::buildGroupFromSingleResult(ru, results);
    }
}

QDSiteconActorPrototype::QDSiteconActorPrototype() {
    descriptor.setId("sitecon");
    descriptor.setDisplayName(QDSiteconActor::tr("Sitecon"));
    descriptor.setDocumentation(QDSiteconActor::tr("Searches for transcription factor binding sites significantly similar to specified SITECON profiles."
        " In case several profiles were supplied, searches with all profiles one by one and outputs merged set of annotations."));

    {
        Descriptor scd(SCORE_ATTR, QDSiteconActor::tr("Min score"),
            QApplication::translate("SiteconSearchDialog", "Recognition quality percentage threshold. If you need to switch off this filter choose <b>the lowest</b> value</i></p>.", 0));
        Descriptor e1d(E1_ATTR, QDSiteconActor::tr("Min Err1"),
            QDSiteconActor::tr("Alternative setting for filtering results, minimal value of Error type I."
            "<br>Note that all thresholds (by score, by err1 and by err2) are applied when filtering results."));
        Descriptor e2d(E2_ATTR, QDSiteconActor::tr("Max Err2"),
            QDSiteconActor::tr("Alternative setting for filtering results, max value of Error type II."
            "<br>Note that all thresholds (by score, by err1 and by err2) are applied when filtering results."));
        Descriptor md(MODEL_ATTR, QDSiteconActor::tr("Model"), QDSiteconActor::tr("Profile data to search with."));

        attributes << new Attribute(scd, BaseTypes::NUM_TYPE(), false, 85);
        attributes << new Attribute(e1d, BaseTypes::NUM_TYPE(), false, 0.);
        attributes << new Attribute(e2d, BaseTypes::NUM_TYPE(), false, 0.001);
        attributes << new Attribute(md, BaseTypes::STRING_TYPE(), true);
    }

    QMap<QString, PropertyDelegate*> delegates;

    {
        QVariantMap m; m["minimum"] = 60; m["maximum"] = 100; m["suffix"] = "%";
        delegates[SCORE_ATTR] = new SpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = 0.; m["maximum"] = 1.; m["singleStep"] = 0.1;
        delegates[E1_ATTR] = new DoubleSpinBoxDelegate(m);
    }
    {
        QVariantMap m; m["minimum"] = 0.000001; m["maximum"] = 1.; m["singleStep"] = 0.001; m["decimals"] = 6;
        delegates[E2_ATTR] = new DoubleSpinBoxDelegate(m);
    }

    delegates[MODEL_ATTR] = new URLDelegate(SiteconIO::getFileFilter(), SiteconIO::SITECON_ID, true);

    editor = new DelegateEditor(delegates);
}

//Task
//////////////////////////////////////////////////////////////////////////
QDSiteconTask::QDSiteconTask( const QStringList& urls, const SiteconSearchCfg& _cfg,
                             const DNASequence& _dna, const QVector<U2Region>& _searchRegion )
                             : Task( tr("Sitecon Query"), TaskFlag_NoRun ),
                             cfg(_cfg), dnaSeq(_dna), searchRegion(_searchRegion) 
{
    loadModelsTask = new SiteconReadMultiTask(urls);
    addSubTask(loadModelsTask);
}

QList<Task*> QDSiteconTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> st;
    if (subTask == loadModelsTask) {
        QList<SiteconModel> models = loadModelsTask->getResult();
        foreach(const U2Region& r, searchRegion) {
            QByteArray seq = dnaSeq.seq.mid(r.startPos, r.length);
            foreach(const SiteconModel& m, models) {
                st.append(new SiteconSearchTask(m, seq, cfg, r.startPos));
            }
        }
    }
    else {
        SiteconSearchTask* searchTask = qobject_cast<SiteconSearchTask*>(subTask);
        assert(searchTask);
        results.append(searchTask->takeResults());
    }
    return st;
}

SiteconReadMultiTask::SiteconReadMultiTask(const QStringList& urls)
: Task(tr("Load sitecon models task"), TaskFlag_NoRun) {
    foreach(const QString& url, urls) {
        addSubTask(new SiteconReadTask(url));
    }
}

QList<Task*> SiteconReadMultiTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> stub;
    SiteconReadTask* rt = qobject_cast<SiteconReadTask*>(subTask);
    assert(rt);
    models.append(rt->getResult());
    return stub;
}

}//namespace
