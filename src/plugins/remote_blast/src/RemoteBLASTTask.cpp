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

#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Formats/GenbankPlainTextFormat.h>

#include "RemoteBLASTTask.h"

namespace U2 {

class BaseIOAdapters;
class BaseDocumentFormats;

RemoteBLASTToAnnotationsTask::RemoteBLASTToAnnotationsTask(const RemoteBLASTTaskSettings & _cfg, int _qoffs,
                                AnnotationTableObject* _ao, const QString &_url, const QString & _group, const QString &annDescription):
Task(tr("RemoteBLASTTask"), TaskFlags_NR_FOSCOE), offsInGlobalSeq(_qoffs), aobj(_ao), group(_group), annDescription(annDescription), url(_url) {
    GCOUNTER(cvar, tvar, "RemoteBLASTToAnnotationsTask");

    queryTask = new RemoteBLASTTask(_cfg);
    addSubTask(queryTask);
}


QList<Task*> RemoteBLASTToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }

    if (isCanceled()) {
        return res;
    }

    if (aobj.isNull()) {
        stateInfo.setError( tr("The object was removed\n"));
        return res;
    }

    if (subTask != queryTask) {
        return res;
    }

    // Query was finished

    RemoteBLASTTask * rrTask = qobject_cast<RemoteBLASTTask *>(queryTask);
    SAFE_POINT(NULL != rrTask, "Invalid remote BLAST task!", res);
    QList<SharedAnnotationData> anns = rrTask->getResultedAnnotations();
    if (anns.isEmpty()) {
        return res;
    }

    if (aobj->getDocument() == NULL && !url.isEmpty()) { // create new document if object has no document and url is not empty
        Document *d = AppContext::getProject()->findDocumentByURL(url);
        if (d == NULL) {
            IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
            DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
            d = df->createNewLoadedDocument(iof, url, stateInfo);
            CHECK_OP(stateInfo, res);
            d->addObject(aobj);
            AppContext::getProject()->addDocument(d);
        } else {
            setError(tr("File %1 already exists").arg(url));
            return res;
        }
    }

    // Add annotations to aobj: shift annotations according to offset first
    QList<SharedAnnotationData> annotations;
    for (QMutableListIterator<SharedAnnotationData> it_ad(anns); it_ad.hasNext();) {
        SharedAnnotationData &ad = it_ad.next();
        U2Region::shift(offsInGlobalSeq, ad->location->regions);
        annotations << ad;
    }
    U1AnnotationUtils::addDescriptionQualifier(annotations, annDescription);
    res << new CreateAnnotationsTask(aobj, annotations, group);
    return res;
}


RemoteBLASTTask::RemoteBLASTTask(const RemoteBLASTTaskSettings & cfg_)
    : Task(tr("RemoteBLASTTask"), TaskFlags_NR_FOSE_COSC),
      cfg(cfg_),
      httpBlastTask(NULL),
      createAnnotTask(NULL)
{
    httpBlastTask = new RemoteBlastHttpRequestTask(cfg);
    addSubTask(httpBlastTask);
}

QList<Task*> RemoteBLASTTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> res;

    if (subTask->hasError() && (subTask == httpBlastTask || subTask == createAnnotTask)) {
        stateInfo.setError(subTask->getError());
        return res;
    }

    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == httpBlastTask) {
        createAnnotTask = new CreateAnnotationsFromHttpBlastResultTask(cfg,
                                                                       httpBlastTask->getResults());
        res << createAnnotTask;
    }

    if (subTask == createAnnotTask) {
        resultAnnotations = createAnnotTask->getResultedAnnotations();
    }

    return res;
}

QByteArray RemoteBLASTTask::getOutputFile() const {
    return httpBlastTask->getOutputFile();
}

const QList<SharedAnnotationData> & RemoteBLASTTask::getResultedAnnotations() const {
    return resultAnnotations;
}

RemoteBlastHttpRequestTask::RemoteBlastHttpRequestTask(const RemoteBLASTTaskSettings& cfg)
    : Task(tr("Http Blast requests task"), TaskFlags_FOSE_COSC),
      cfg(cfg),
      timeout(false)
{

}

void RemoteBlastHttpRequestTask::prepare() {
    prepareQueries();
    algoLog.trace("Sequences prepared");
    for (QList<Query>::iterator it = queries.begin(),end = queries.end();it!=end;it++) {
        DataBaseFactory *dbf = AppContext::getDataBaseRegistry()->getFactoryById(cfg.dbChoosen);
        if(dbf == NULL) {
            stateInfo.setError(tr("Incorrect database"));
            return;
        }
        HttpRequest *tmp = dbf->getRequest(this);
        httpRequest.append(tmp);
    }
    algoLog.trace("Requests formed");
    connect(&timer,SIGNAL(timeout()),SLOT(sl_timeout()));
    timeout = true;
    timer.setSingleShot(true);
    int mult = cfg.aminoT ? 6 : 1; //if 6 requests - 6 times more wait
    timer.start(cfg.retries*1000*60*mult);
}

void RemoteBlastHttpRequestTask::run() {
    for(int i = 0;i < queries.count();i++) {
        if(isCanceled()) {
            return;
        }

        httpRequest[i]->sendRequest(cfg.params,QString(queries[i].seq.data()));
        if (!httpRequest[i]->getError().isEmpty()) {
            stateInfo.setError(httpRequest[i]->getError());
            return;
        }

        resultList << HttpBlastRequestTaskResult(httpRequest[i], queries[i]);
    }
}

void RemoteBlastHttpRequestTask::updateProgress() {
    if(stateInfo.progress >= 99) {
        return;
    }
    int timeoutInSecs = timer.interval() / 1000;
    int elapsedTime = GTimer::secsBetween(timeInfo.startTime, GTimer::currentTimeMicros());
    int taskProgress = elapsedTime * 99 / timeoutInSecs;
    stateInfo.progress = qMin(taskProgress, 99);
    emit si_progressChanged();
}

void RemoteBlastHttpRequestTask::prepareQueries() {
    Query q;
    if(cfg.aminoT) {
        q.amino = true;
        QByteArray complQuery(cfg.query.size(), 0);
        cfg.complT->translate(cfg.query.data(), cfg.query.size(), complQuery.data(), complQuery.size());
        TextUtils::reverse(complQuery.data(), complQuery.size());
        for(int i = 0; i < 3; ++i) {
            QByteArray aminoQuery(cfg.query.size() / 3, 0);
            cfg.aminoT->translate(cfg.query.data()+i, cfg.query.size()-i, aminoQuery.data(), aminoQuery.size());
            q.seq = aminoQuery;
            q.offs = i;
            q.complement = false;
            queries.push_back(q);
            QByteArray aminoQueryCompl(cfg.query.size() / 3, 0);
            cfg.aminoT->translate(complQuery.data()+i, complQuery.size()-i, aminoQueryCompl.data(), aminoQueryCompl.size());
            q.seq = aminoQueryCompl;
            q.offs = i;
            q.complement = true;
            queries.push_back(q);
        }
    }
    else {
        q.seq = cfg.query;
        queries.push_back(q);
    }

}

CreateAnnotationsFromHttpBlastResultTask::CreateAnnotationsFromHttpBlastResultTask(const RemoteBLASTTaskSettings& cfg,
                                                                                   const QList<RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult>& results)
    : Task(tr("Create annotations from BLAST results"), TaskFlags_NR_FOSE_COSC),
      cfg(cfg),
      httpBlastResults(results)
{
    seqLen = cfg.query.size();
    SAFE_POINT_EXT(!httpBlastResults.isEmpty(), setError(tr("HttpBlastResult list is empty")),);
}

void CreateAnnotationsFromHttpBlastResultTask::prepare() {
    foreach (const RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult& r, httpBlastResults) {
        createAnnotations(r);
    }
    mergeNeighbourResults();
}

void CreateAnnotationsFromHttpBlastResultTask::createAnnotations(const RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult& result) {
    HttpRequest* t = result.request;
    SAFE_POINT_EXT(t != NULL, setError(tr("HttpRequest is NULL!")),);
    RemoteBlastHttpRequestTask::Query q = result.query;
    QList<SharedAnnotationData> annotations = t->getAnnotations();
    {
        QRegExp regExp("&" + ReqParams::hits + "=([^&]*)");
        if(cfg.params.contains(regExp)) {
            bool ok = false;
            int maxHits = regExp.cap(1).toInt(&ok);
            if(ok) {
                annotations = annotations.mid(0, maxHits);
            }
        }
    }
    if(annotations.isEmpty()) {
        return;
    }

    if(cfg.filterResult) {
        annotations = filterAnnotations(annotations);
    }

    for (int i = 0; i < annotations.size(); i++) {
        SharedAnnotationData &d = annotations[i];
        for (QVector<U2Region>::iterator jt = d->location->regions.begin(), eend = d->location->regions.end(); eend != jt; ++jt) {
            qint64& s = jt->startPos;
            qint64& l = jt->length;

            if (q.complement) {
                s = q.seq.size() - s - l;
                d->setStrand(d->getStrand().isCompementary() ? U2Strand::Direct : U2Strand::Complementary);
            }
            if (q.amino) {
                s = s * 3 + (q.complement ? 2 - q.offs : q.offs);
                l = l * 3;
            }
        }
    }

    resultAnnotations << annotations;
}

QList<SharedAnnotationData> CreateAnnotationsFromHttpBlastResultTask::filterAnnotations(QList<SharedAnnotationData> &annotations) {
    QString selectiveQual = cfg.useEval ? "e-value" : "score";
    QList<SharedAnnotationData> resultList;

    if (cfg.filterResult & FilterResultByAccession) {
        QStringList accessions;
        foreach (const SharedAnnotationData &ann, annotations) {
            QString acc = ann->findFirstQualifierValue("accession");
            if (accessions.contains(acc)) {
                QString eval = ann->findFirstQualifierValue(selectiveQual);
                foreach (const SharedAnnotationData &a, resultList) {
                    if (a->findFirstQualifierValue("accession") == acc) {
                        if (cfg.useEval
                            ? a->findFirstQualifierValue(selectiveQual).toDouble() < eval.toDouble()
                            : a->findFirstQualifierValue(selectiveQual).toDouble() > eval.toDouble())
                        {
                            resultList.removeOne(a);
                            resultList << ann;
                        }
                        break;
                    }
                }
            } else {
                resultList << ann;
                accessions << acc;
            }
        }
        annotations = resultList;
    }

    if(cfg.filterResult & FilterResultByDef) {
        resultList.clear();
        QStringList defs;
        foreach (const SharedAnnotationData &ann, annotations) {
            QString def = ann->findFirstQualifierValue("def");
            if(defs.contains(def)) {
                QString eval = ann->findFirstQualifierValue(selectiveQual);
                foreach (const SharedAnnotationData &a, resultList) {
                    if (a->findFirstQualifierValue("def") == def) {
                        if (cfg.useEval
                            ? a->findFirstQualifierValue(selectiveQual).toDouble() < eval.toDouble()
                            : a->findFirstQualifierValue(selectiveQual).toDouble() > eval.toDouble())
                        {
                            resultList.removeOne(a);
                            resultList << ann;
                        }
                        break;
                    }
                }
            } else {
                resultList << ann;
                defs << def;
            }
        }
        annotations = resultList;
    }

    if(cfg.filterResult & FilterResultById) {
        resultList.clear();
        QStringList ids;
        foreach (const SharedAnnotationData &ann, annotations) {
            QString id = ann->findFirstQualifierValue("id");
            if (ids.contains(id)) {
                QString eval = ann->findFirstQualifierValue(selectiveQual);
                foreach (const SharedAnnotationData &a, resultList) {
                    if (a->findFirstQualifierValue("id") == id) {
                        if (cfg.useEval
                            ? a->findFirstQualifierValue(selectiveQual).toDouble() < eval.toDouble()
                            : a->findFirstQualifierValue(selectiveQual).toDouble() > eval.toDouble())
                        {
                            resultList.removeOne(a);
                            resultList << ann;
                        }
                        break;
                    }
                }
            } else {
                resultList << ann;
                ids << id;
            }
        }
    }
    return resultList;
}

void CreateAnnotationsFromHttpBlastResultTask::mergeNeighbourResults() {
    const QVector<SharedAnnotationData> resultVectorAnnotations = resultAnnotations.toVector();
    for (int i = 0; i < resultVectorAnnotations.size(); i++) {
        for (int j = 0; j < i; j++) {
            if (i == j) {
                continue;
            }

            SharedAnnotationData adStart = resultVectorAnnotations[i];
            SharedAnnotationData adEnd = resultVectorAnnotations[j];

            if (annotationsAreNeighbours(adStart, adEnd) && annotationsReferToTheSameSeq(adStart, adEnd)) {
                orderNeighbors(adStart, adEnd);

                bool linearNeighbours = (adStart->findFirstQualifierValue("hit-to").toInt() + 1 == adEnd->findFirstQualifierValue("hit-from").toInt());
                if (linearNeighbours) {
                    resultAnnotations << merge(adStart, adEnd);
                    resultAnnotations.removeOne(adStart);
                    resultAnnotations.removeOne(adEnd);
                }

                bool circularNeighbours = (adEnd->findFirstQualifierValue("hit-from") == "1"
                    && adStart->findFirstQualifierValue("hit-to") == adStart->findFirstQualifierValue("hit_len"));
                if (circularNeighbours) {
                    createCheckTask(adStart, adEnd);
                }
            }
        }
    }
}

SharedAnnotationData CreateAnnotationsFromHttpBlastResultTask::merge(const SharedAnnotationData &start, const SharedAnnotationData &end) {
    SharedAnnotationData result(new AnnotationData);

    result->name = start->name;
    if (start->getRegions().first().endPos() == seqLen && end->getRegions().first().startPos == 0) {
        result->location->regions << start->getRegions() << end->getRegions();
        result->location->op = U2LocationOperator_Join;
    } else {
        result->location->regions << U2Region(start->getRegions().first().startPos, end->getRegions().last().endPos());
    }

    U2Qualifier q;
    q = Merge::equalQualifiers("accession", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::equalQualifiers("def", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::equalQualifiers("id", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::equalQualifiers("hit_len", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::equalQualifiers("source_frame", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::hitFromQualifier(start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::hitToQualifier(start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::percentQualifiers("identities", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::percentQualifiers("gaps", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }
    q = Merge::sumQualifiers("score", start, end);
    if (q.isValid()) {
        result->qualifiers.push_back(q);
    }

//!    No Bit-Score and E-value qualifiers!
//!    Such parameters can not be recalculated without knowing additional constants from BLAST: K and Alpha
//!    K and Alpha can vary.

    return result;
}

bool CreateAnnotationsFromHttpBlastResultTask::annotationsReferToTheSameSeq(const SharedAnnotationData &start, const SharedAnnotationData &end) {
    bool annsHaveTheSameId = start->findFirstQualifierValue("accession") == end->findFirstQualifierValue("accession")
            && start->findFirstQualifierValue("id") == end->findFirstQualifierValue("id");
    bool annsAreOnTheSameStrand = start->findFirstQualifierValue("source_frame") == end->findFirstQualifierValue("source_frame");
    return (annsHaveTheSameId && annsAreOnTheSameStrand);
}

bool CreateAnnotationsFromHttpBlastResultTask::annotationsAreNeighbours(SharedAnnotationData &start, SharedAnnotationData &end) {
    SAFE_POINT(start->getRegions().size() == 1, tr("Wrong number of annotations"), false);
    SAFE_POINT(end->getRegions().size() == 1, tr("Wrong number of annotations"), false);

    int sStart = start->getRegions().first().startPos;
    int sEnd = start->getRegions().first().endPos();

    int eStart = end->getRegions().first().startPos;
    int eEnd = end->getRegions().first().endPos();

    if (sEnd == eStart || (sEnd == seqLen && eStart == 0 && cfg.isCircular)) {
        return true;
    }

    if (sStart == eEnd || (eEnd == seqLen && sStart == 0 && cfg.isCircular)) {
        return true;
    }

    return false;
}

void CreateAnnotationsFromHttpBlastResultTask::orderNeighbors(SharedAnnotationData &start, SharedAnnotationData &end) {
    int sStart = start->getRegions().first().startPos;
    int eEnd = end->getRegions().first().endPos();

    if (sStart == eEnd || (eEnd == seqLen && sStart == 0 && cfg.isCircular)) {
        qSwap(start, end);
    }
}

void CreateAnnotationsFromHttpBlastResultTask::createCheckTask(const SharedAnnotationData &adStart, const SharedAnnotationData &adEnd) {
    mergeCandidates.append(QPair<SharedAnnotationData, SharedAnnotationData>(adStart, adEnd));
    QString id = adStart->findFirstQualifierValue("accession");
    CheckNCBISequenceCircularityTask *checkTask = new CheckNCBISequenceCircularityTask(id);
    circCheckTasks.append(checkTask);
    addSubTask(checkTask);
}

QList<Task *> CreateAnnotationsFromHttpBlastResultTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }

    CheckNCBISequenceCircularityTask* checkCircTask = qobject_cast<CheckNCBISequenceCircularityTask*>(subTask);
    if (checkCircTask == NULL) {
        return res;
    }

    if (checkCircTask->getResult()) {
        // merge
        int idx = circCheckTasks.indexOf(checkCircTask);
        SAFE_POINT_EXT(idx != -1, setError(tr("Invalid subtask")), res);
        SAFE_POINT_EXT(idx < mergeCandidates.size(), setError(tr("No corresponding annotations")), res);
        resultAnnotations << merge(mergeCandidates[idx].first, mergeCandidates[idx].second);
        resultAnnotations.removeOne(mergeCandidates[idx].first);
        resultAnnotations.removeOne(mergeCandidates[idx].second);

        circCheckTasks.remove(idx);
        mergeCandidates.remove(idx);
    }

    return res;
}

CheckNCBISequenceCircularityTask::CheckNCBISequenceCircularityTask(const QString &id)
    : Task(tr("Check NCBI sequence circularity"), TaskFlags_NR_FOSE_COSC),
      seqId(id),
      loadTask(NULL),
      result(false)
{
    SAFE_POINT_EXT(!seqId.isEmpty(), setError(tr("ID is empty")),);

    U2OpStatusImpl os;
    tempUrl = GUrlUtils::prepareDirLocation(AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("blast_circ_check"),
                                            os);
    SAFE_POINT_OP(os,);

    QString dbId = "nucleotide"; // protein databases do not contain circular molecules

    loadTask = new LoadRemoteDocumentTask(seqId, dbId, tempUrl, "gb");
    addSubTask(loadTask);
}

QList<Task*> CheckNCBISequenceCircularityTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == loadTask) {
        LoadRemoteDocumentTask* task = qobject_cast<LoadRemoteDocumentTask*>(loadTask);
        GUrl loadedSeq = task->getLocalUrl();
        U2OpStatusImpl os;
        result = GenbankPlainTextFormat::checkCircularity(loadedSeq, os);
        if (os.isCoR()) {
            taskLog.info(tr("Can not check if sequence is circular! Sequence is considered to be linear."));
        }
        GUrlUtils::removeFile(loadedSeq.getURLString(), os);
        if (os.isCoR()) {
            taskLog.info(tr("Can not remove temporary file."));
        }
    }

    return res;
}

RemoteCDSearch::RemoteCDSearch(const CDSearchSettings& settings) {
    RemoteBLASTTaskSettings cfg;
    cfg.dbChoosen = "cdd";

    addParametr(cfg.params, ReqParams::cdd_hits, 500);
    addParametr(cfg.params, ReqParams::cdd_eValue, settings.ev);

    if (settings.dbName == CDDNames::CDD_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "CDD");
    } else if (settings.dbName == CDDNames::PFAM_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_pfam");
    } else if (settings.dbName == CDDNames::SMART_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_smart");
    } else if (settings.dbName == CDDNames::COG_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_cog");
    } else if (settings.dbName == CDDNames::KOG_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_kog");
    } else if (settings.dbName == CDDNames::PRK_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_prk");
    } else if (settings.dbName == CDDNames::TIGR_DB()) {
        addParametr(cfg.params, ReqParams::cdd_db, "oasis_tigr");
    } else {
        assert(0);
    }

    cfg.query = settings.query;
    cfg.retries = 60;
    cfg.filterResult = 0;

    task = new RemoteBLASTTask(cfg);
}

U2Qualifier Merge::equalQualifiers(const QString &qualName, const SharedAnnotationData &first, const SharedAnnotationData &second) {
    QString qualValue;
    qualValue = first->findFirstQualifierValue(qualName);

    SAFE_POINT(!qualValue.isEmpty(), tr("Qualifier %1 not found").arg(qualName), U2Qualifier());
    SAFE_POINT(qualValue == second->findFirstQualifierValue(qualName), tr("Can not merge %1 qualifiers: values are not the same.").arg(qualName), U2Qualifier());

    return U2Qualifier(qualName, qualValue);
}

U2Qualifier Merge::percentQualifiers(const QString &qualName, const SharedAnnotationData &first, const SharedAnnotationData &second) {
    QString tmp = first->findFirstQualifierValue(qualName);
    SAFE_POINT(!tmp.isEmpty(), tr("Can not find '%1' qualifier").arg(qualName), U2Qualifier());
    // parse
    int number = tmp.left(tmp.indexOf('/')).toInt();
    int total = tmp.mid(tmp.indexOf('/') + 1, tmp.indexOf(' ') - tmp.indexOf('/')).toInt();

    tmp = second->findFirstQualifierValue(qualName);
    SAFE_POINT(!tmp.isEmpty(), tr("Can not find '%1' qualifier").arg(qualName), U2Qualifier());
    number += tmp.left(tmp.indexOf('/')).toInt();
    total += tmp.mid(tmp.indexOf('/') + 1, tmp.indexOf(' ') - tmp.indexOf('/')).toInt();

    float percent = 100 * (float)number / total;
    QString qualValue = QString::number(number) + "/" + QString::number(total) + " (" + QString::number(percent, 'g', 4) + "%)";

    return U2Qualifier(qualName, qualValue);
}

U2Qualifier Merge::hitFromQualifier(const SharedAnnotationData &first, const SharedAnnotationData &second) {
    Q_UNUSED(second);
    QString qualValue = first->findFirstQualifierValue("hit-from");
    return U2Qualifier("hit-from", qualValue);
}

U2Qualifier Merge::hitToQualifier(const SharedAnnotationData &first, const SharedAnnotationData &second) {
    Q_UNUSED(first);
    QString qualValue = second->findFirstQualifierValue("hit-to");
    return U2Qualifier("hit-to", qualValue);
}

U2Qualifier Merge::sumQualifiers(const QString &qualName, const SharedAnnotationData &first, const SharedAnnotationData &second) {
    QString tmp = first->findFirstQualifierValue(qualName);
    SAFE_POINT(!tmp.isEmpty(), tr("Can not find '%1' qualifier").arg(qualName), U2Qualifier());
    bool ok;
    int res = tmp.toDouble(&ok);
    SAFE_POINT(ok, tr("Can not convert qualifier value '%1' to double").arg(tmp), U2Qualifier());

    tmp = second->findFirstQualifierValue(qualName);
    SAFE_POINT(!tmp.isEmpty(), tr("Can not find '%1' qualifier").arg(qualName), U2Qualifier());
    res += tmp.toDouble(&ok);
    SAFE_POINT(ok, tr("Can not convert qualifier value '%1' to double").arg(tmp), U2Qualifier());

    return U2Qualifier(qualName, QString::number(res));
}

U2Qualifier Merge::eValueQualifier(int seqLen, const SharedAnnotationData &first, const SharedAnnotationData &second) {
    QString tmp = first->findFirstQualifierValue("E-value");
    SAFE_POINT(!tmp.isEmpty(), tr("Can not find 'E-value' qualifier"), U2Qualifier());
    bool ok;
    double e1 = tmp.toDouble(&ok);
    SAFE_POINT(ok, tr("Can not convert qualifier value '%1' to double").arg(tmp), U2Qualifier());

    tmp = second->findFirstQualifierValue("E-value");
    double e2 = tmp.toDouble(&ok);
    SAFE_POINT(ok, tr("Can not convert qualifier value '%1' to double").arg(tmp), U2Qualifier());

    tmp = first->findFirstQualifierValue("hit-len");
    int refLen = tmp.toInt(&ok);
    SAFE_POINT(ok, tr("Can not convert qualifier value '%1' to int").arg(tmp), U2Qualifier());

    double eValue = e1*e2 / seqLen*refLen;

    return U2Qualifier("E-value", QString::number(eValue));
}

} // namespace
