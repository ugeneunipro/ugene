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

#ifndef _U2_QD_SCHEDULER_H_
#define _U2_QD_SCHEDULER_H_

#include <U2Lang/QDScheme.h>

#include <U2Core/Task.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>

namespace U2 {

class AnnotationTableObject;
class LoadUnloadedDocumentTask;

class QDRunSettings {
public:
    QDRunSettings():  annotationsObj(NULL), scheme(NULL),
    offset(0), outputType(Group) {}

    DNASequence             dnaSequence;
    AnnotationTableObject *   annotationsObj;
    GObjectReference        annotationsObjRef;
    QString                 groupName;
    QString                 annDescription;
    U2Region                region;
    QDScheme*               scheme;
    QString                 viewName;
    int                     offset;

    enum OutputType { Group, Single };
    OutputType outputType;
};

class QDStep {
public:
    QDStep(QDScheme* _scheme);
    void next();
    bool hasNext() const;
    bool hasPrev() const;
    QDActor* getActor() const { return actor; }
    const QList<QDActor*>& getLinkedActors() const { return linkedActors; }
    QList<QDConstraint*> getConstraints(QDSchemeUnit* subj, QDSchemeUnit* linked) const;
private:
    void initTotalMap();
private:
    QDScheme* scheme;
    QDActor* actor;
    QList<QDActor*> linkedActors;
    QMap< QPair<QDSchemeUnit*, QDSchemeUnit*>, QList<QDConstraint*> > constraintsMap;
};

class QDScheduler;

class QDResultLinker {
public:
    QDResultLinker(QDScheduler* _sched);
    QVector<U2Region> findLocation(QDStep* step);
    void updateCandidates(QDStep* step, int& progress);
    QDScheduler* getScheduler() const { return sched; }
    int getCandidatesNumber() const { return candidates.size(); }
    bool isCancelled() const { return cancelled; }
    const QString& getCancelMessage() const { return cancelMeassage; }
    void prepareAnnotations();
    void createAnnotations(const QString& groupPrefix);
    void createMergedAnnotations(const QString& groupPrefix);
    void pushToTable();
private:
    void formGroupResults();
    void processNewResults(int& progress);
    void initCandidates(int& progress);
    void updateCandidates(int& progress);
    void cleanupCandidates();
    bool canAdd(QDResultGroup* actorResult, QDResultGroup* candidate, bool complement) const;
    QDStrandOption findResultStrand(QDResultGroup* actorRes);
    //inverts repeat pair if any for complement search
    QList<QDResultUnit> prepareComplResults(QDResultGroup* src) const;
    static QString prepareAnnotationName(const QDResultUnit& res);
private:
    QDScheme*                                   scheme;
    QDScheduler*                                sched;
    bool                                        cancelled;
    QString                                     cancelMeassage;
    QDStep*                                     currentStep;
    bool                                        needInit;
    QList<QDResultGroup*>                       candidates;
    QMap<QDResultUnit, SharedAnnotationData>    result2annotation;
    QList<QDResultGroup*>                       currentResults;
    QMap< QDActor*, QList<QDResultGroup*> >     currentGroupResults;
    QMap<QString, QList<SharedAnnotationData> > annotations;
    int                                         maxMemorySizeInMB;
};

class QDFindLocationTask : public Task {
    Q_OBJECT
public:
    QDFindLocationTask(QDStep* _step, QDResultLinker* _linker)
        : Task(tr("Find resultLocation"), TaskFlag_None), step(_step), linker(_linker) {}

    void run();
    QVector<U2Region> getSearchLocation() { return searchLocation; }
    QVector<U2Region> getResultLocation() { return resultLocation; }
public:
    static const int REGION_DELTA;
private:
    QDStep* step;
    QDResultLinker* linker;
    QVector<U2Region> resultLocation;
    QVector<U2Region> searchLocation;
};

class QDLinkResultsTask : public Task {
    Q_OBJECT
public:
    QDLinkResultsTask(QDStep* _step, QDResultLinker* _linker):
    Task(tr("Link results"), TaskFlag_None), step(_step), linker(_linker)
    { tpm = Progress_Manual; }

    void run() { linker->updateCandidates(step, stateInfo.progress); }
private:
    QDStep* step;
    QDResultLinker* linker;
};

class QDTask : public Task {
    Q_OBJECT
public:
    QDTask(QDStep* _step, QDResultLinker* _linker);
    QList<Task*> onSubTaskFinished(Task* subTask);
private slots:
    void sl_updateProgress();
private:
    QDStep* step;
    QDResultLinker* linker;
    QDFindLocationTask* findLocationTask;
    Task* runTask;
    QVector<U2Region> curActorLocation;
};

class U2DESIGNER_EXPORT QDCreateAnnotationsTask : public Task {
    Q_OBJECT
public:
    QDCreateAnnotationsTask(QDResultLinker* linker)
        : Task(tr("Prepare annotations task"), TaskFlag_None), linker(linker) {}

    void run() { linker->prepareAnnotations(); }

private:
    QDResultLinker* linker;
};

class U2DESIGNER_EXPORT QDScheduler : public Task {
    Q_OBJECT
public:
    QDScheduler(const QDRunSettings& settings);
    ~QDScheduler();
    QList<Task*> onSubTaskFinished(Task* subTask);
    const QDRunSettings& getSettings() const { return settings; }
    QDResultLinker* getLinker() const { return linker; }
    Task::ReportResult report();
private slots:
    void sl_updateProgress();
private:
    QDRunSettings settings;
    QDResultLinker* linker;
    LoadUnloadedDocumentTask* loadTask;
    QDCreateAnnotationsTask* createAnnsTask;
    QDStep* currentStep;
    int progressDelta;
};

}//namespace

#endif
