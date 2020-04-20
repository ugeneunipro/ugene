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

#ifndef _U2_REMOTE_QUERY_TASK_H_
#define _U2_REMOTE_QUERY_TASK_H_

#include <QPointer>
#include <QTimer>

#include <U2Algorithm/CDSearchTaskFactory.h>

#include <U2Core/AnnotationData.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Task.h>

#include "HttpRequest.h"

namespace U2 {

class DNATranslation;
class AnnotationTableObject;

enum SendSelectionAlphabet
{
    SendSelectionAlphabet_Any,
    SendSelectionAlphabet_Nucleo,
    SendSelectionAlphabet_Amino
};

enum FilterResult {
    FilterResultByAccession = 1,
    FilterResultByDef = 2,
    FilterResultById = 4
};

struct RemoteBLASTTaskSettings {
    QString             dbChoosen;
    QString             params;
    int                 retries;
    DNATranslation *    aminoT;
    DNATranslation *    complT;
    QByteArray          query;
    bool                isCircular;
    int                 filterResult;
    bool                useEval;
    RemoteBLASTTaskSettings() : retries(0), aminoT(NULL), complT(NULL),
        isCircular(false), filterResult(0), useEval(false) {}
};

class RemoteBLASTToAnnotationsTask : public Task {
    Q_OBJECT
public:
    RemoteBLASTToAnnotationsTask( const RemoteBLASTTaskSettings &cfg, int qoffs, AnnotationTableObject *ao, const QString &url, const QString & group, const QString &annDescription);

    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    int                 offsInGlobalSeq;
    QPointer<AnnotationTableObject>  aobj;
    QString             group;
    const QString       annDescription;
    Task*               queryTask;
    QString             url;
};

class CreateAnnotationsFromHttpBlastResultTask;
class RemoteBlastHttpRequestTask;

class RemoteBLASTTask : public Task {
    Q_OBJECT
public:
    RemoteBLASTTask( const RemoteBLASTTaskSettings & cfg );
    QList<Task*> onSubTaskFinished(Task *subTask);

    QByteArray getOutputFile() const;

    const QList<SharedAnnotationData> & getResultedAnnotations() const;

private:
    RemoteBLASTTaskSettings                     cfg;
    RemoteBlastHttpRequestTask*                 httpBlastTask;
    CreateAnnotationsFromHttpBlastResultTask*   createAnnotTask;
    QList<SharedAnnotationData>                 resultAnnotations;
};

class RemoteBlastHttpRequestTask : public Task {
    Q_OBJECT
public:
    RemoteBlastHttpRequestTask(const RemoteBLASTTaskSettings& cfg);
    void prepare();
    void run();

    void updateProgress();
    void resetProgress() {stateInfo.progress = 0; emit si_progressChanged();}

    QByteArray getOutputFile() const {return httpRequest.first()->getOutputFile();}


    struct Query {
        Query()
            : amino(false),
              complement(false),
              offs(0) {}
        QByteArray seq;
        bool amino;
        bool complement;
        int offs;       //translation frame offset
    };
    struct HttpBlastRequestTaskResult {
        HttpBlastRequestTaskResult(HttpRequest* r,
                                   Query& q)
            : request(r),
              query(q) {}
        HttpRequest*    request;
        Query           query;
    };

    const QList<HttpBlastRequestTaskResult>& getResults() const { return resultList; }
    bool isTimeOut() const { return timeout; }

private slots:
    void sl_timeout() {timeout = false;}

private:
    void  prepareQueries();

    RemoteBLASTTaskSettings     cfg;

    QList<Query>                queries;
    QList<HttpRequest*>         httpRequest;

    QList<HttpBlastRequestTaskResult> resultList;
    QTimer timer;
    bool timeout;
};

class CheckNCBISequenceCircularityTask : public Task {
    Q_OBJECT
public:
    CheckNCBISequenceCircularityTask(const QString& id);
    QList<Task*> onSubTaskFinished(Task *subTask);

    bool getResult() const { return result; }

private:
    QString seqId;
    Task*   loadTask;
    QString tempUrl;
    bool    result;
};

class CreateAnnotationsFromHttpBlastResultTask : public Task {
    Q_OBJECT
public:
    CreateAnnotationsFromHttpBlastResultTask(const RemoteBLASTTaskSettings &cfg,
                                             const QList<RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult> &results);
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    const QList<SharedAnnotationData> & getResultedAnnotations() const { return resultAnnotations; }

private:
    void createAnnotations(const RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult &result);
    QList<SharedAnnotationData> filterAnnotations(QList<SharedAnnotationData> &annotations);

    void mergeNeighbourResults();

    SharedAnnotationData merge(const SharedAnnotationData& start, const SharedAnnotationData& end);
    static bool annotationsReferToTheSameSeq(const SharedAnnotationData& start, const SharedAnnotationData& end);
    bool annotationsAreNeighbours(SharedAnnotationData& start, SharedAnnotationData& end);
    void orderNeighbors(SharedAnnotationData& start, SharedAnnotationData& end);
    void createCheckTask(const SharedAnnotationData& start, const SharedAnnotationData& end);

    RemoteBLASTTaskSettings     cfg;
    int                         seqLen;
    QList<SharedAnnotationData> resultAnnotations;

    QList<RemoteBlastHttpRequestTask::HttpBlastRequestTaskResult>   httpBlastResults;
    QVector<CheckNCBISequenceCircularityTask *>                    circCheckTasks;
    QVector<QPair<SharedAnnotationData, SharedAnnotationData> >    mergeCandidates;
};

//used by CDSearchWorker
class RemoteCDSearch : public CDSearchResultListener {
public:
    RemoteCDSearch(const CDSearchSettings& settings);

    virtual Task* getTask() const { return task; }
    virtual QList<SharedAnnotationData> getCDSResults() const { return task->getResultedAnnotations(); }
private:
    RemoteBLASTTask* task;
};

class RemoteCDSearchFactory : public CDSearchFactory {
    virtual CDSearchResultListener* createCDSearch(const CDSearchSettings& settings) const { return new RemoteCDSearch(settings); }
};

class Merge : public QObject {
    Q_OBJECT
public:
    static U2Qualifier equalQualifiers(const QString &qualName, const SharedAnnotationData& first, const SharedAnnotationData& second);

    static U2Qualifier percentQualifiers(const QString &qualName, const SharedAnnotationData& first, const SharedAnnotationData& second);

    static U2Qualifier hitFromQualifier(const SharedAnnotationData& first, const SharedAnnotationData& second);
    static U2Qualifier hitToQualifier(const SharedAnnotationData& first, const SharedAnnotationData& second);

    static U2Qualifier sumQualifiers(const QString &qualName, const SharedAnnotationData& first, const SharedAnnotationData& second);

    static U2Qualifier eValueQualifier(int seqLen, const SharedAnnotationData& first, const SharedAnnotationData& second);
};

} // namespace

#endif
