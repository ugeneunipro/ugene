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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2Lang/QDConstraint.h>

#include "QDScheduler.h"

namespace U2 {
//QDScheduler
//////////////////////////////////////////////////////////////////////////
static int PROCESSING_PROGRESS_WEIGHT(80);

QDScheduler::QDScheduler(const QDRunSettings& _settings)
: Task(tr("QDScheduler"), TaskFlags_NR_FOSCOE), settings(_settings) {
    GCOUNTER(cvar, tvar, "QueryDesignerScheduler");
    loadTask = NULL;
    createAnnsTask = NULL;
    linker = new QDResultLinker(this);
    settings.scheme->adaptActorsOrder();
    currentStep = new QDStep(settings.scheme);

    tpm = Progress_Manual;
    stateInfo.progress = 0;
    // if annotation table is not added to project
    // annotations will be added in subtask thread
    // => leave some progress bar space for it
    Document* annObjDoc = settings.annotationsObj == NULL ? NULL : settings.annotationsObj->getDocument();
    if (annObjDoc) {
        progressDelta = 100 / settings.scheme->getActors().size();
    } else {
        progressDelta = PROCESSING_PROGRESS_WEIGHT / settings.scheme->getActors().size();
    }

    if (settings.annotationsObj == NULL) {
        GObject* ao = GObjectUtils::selectObjectByReference(settings.annotationsObjRef, UOF_LoadedAndUnloaded);
        if (ao == NULL) {
            setError(tr("Can't find annotation object: %1 in document: %2").arg(settings.annotationsObjRef.objName).arg(settings.annotationsObjRef.docUrl));
            return;
        }
        LoadDocumentTaskConfig cfg(false, settings.annotationsObjRef);
        loadTask = new LoadUnloadedDocumentTask(ao->getDocument(), cfg);
        addSubTask(loadTask);
    }
    QDTask* qdt = new QDTask(currentStep, linker);
    addSubTask(qdt);
}

QDScheduler::~QDScheduler() {
    delete linker;
    delete currentStep;
}

QList<Task*> QDScheduler::onSubTaskFinished(Task* subTask) {
    QList<Task*> subs;
    propagateSubtaskError();
    CHECK_OP(stateInfo, subs);
    if (linker->isCancelled()){
        setError(linker->getCancelMessage());
        return subs;
    }
    if (subTask == createAnnsTask) {
        return subs;
    }

    if (subTask == loadTask) {
        settings.annotationsObj = qobject_cast<AnnotationTableObject *>(loadTask->getDocument()->findGObjectByName(settings.annotationsObjRef.objName));
        return subs;
    }
    if (settings.annotationsObj == NULL) {
        setError(tr("Annotation object is not available"));
        return subs;
    }

    if (currentStep->hasNext()) {
        currentStep->next();
        QDTask* t = new QDTask(currentStep, linker);
        connect(t, SIGNAL(si_progressChanged()), SLOT(sl_updateProgress()));
        subs.append(t);
    } else {
        createAnnsTask = new QDCreateAnnotationsTask(linker);
        subs.append(createAnnsTask);
    }
    return subs;
}

#define PUSH_WEIGTH (1.0f - PROCESSING_PROGRESS_WEIGHT / 100.0f)
void QDScheduler::sl_updateProgress() {
    Task* sub = qobject_cast<Task*>(sender());
    int numProcessed = currentStep->getLinkedActors().size();
    if (numProcessed < settings.scheme->getActors().size()) {
        stateInfo.progress = progressDelta * (numProcessed + sub->getProgress()/100.0f);
    } else {
        stateInfo.progress = PROCESSING_PROGRESS_WEIGHT + PUSH_WEIGTH * sub->getProgress();
    }
}

Task::ReportResult QDScheduler::report() {
    if (stateInfo.isCanceled() || stateInfo.hasError()) {
        return ReportResult_Finished;
    }
    linker->pushToTable();
    // last task is finished, add annotation table object to view if needed
    if (!settings.viewName.isEmpty()) {
        GObjectViewWindow* viewWindow = GObjectViewUtils::findViewByName(settings.viewName);
        if (viewWindow != NULL) {
            viewWindow->getObjectView()->addObject(settings.annotationsObj);
        }
    }

    return ReportResult_Finished;
}

//QDResultLinker
//////////////////////////////////////////////////////////////////////////
QDResultLinker::QDResultLinker(QDScheduler* _sched)
: scheme(_sched->getSettings().scheme), sched(_sched), cancelled(false), currentStep(NULL),
needInit(true), maxMemorySizeInMB(-1) {
    const AppSettings* appSettings = AppContext::getAppSettings();
    SAFE_POINT_EXT(NULL != appSettings, taskLog.error(QDScheduler::tr("Invalid applications settings detected")), );

    AppResourcePool* appResourcePool = appSettings->getAppResourcePool();
    SAFE_POINT_EXT(NULL != appResourcePool, taskLog.error(QDScheduler::tr("Invalid users applications settings detected")), );
    maxMemorySizeInMB=AppContext::getAppSettings()->getAppResourcePool()->getMaxMemorySizeInMB();
}

QString QDResultLinker::prepareAnnotationName(const QDResultUnit& res) {
    QString aname = res->owner->getActor()->annotateAs();
    if (aname=="<rsite>") {
        foreach(const U2Qualifier& qual, res->quals) {
            if (qual.name == "id") {
                return qual.value;
            }
        }
    }
    return aname;
}

// for 1..3, 5..7 returns 1..7
U2Region uniteRegions(const QVector<U2Region>& regions) {
    assert(!regions.isEmpty());
    qint64 startPos = regions.first().startPos;
    qint64 endPos = regions.first().endPos();
    for (int i=1, n=regions.size(); i<n; i++) {
        const U2Region& r = regions.at(i);
        startPos = qMin(startPos, r.startPos);
        endPos = qMax(endPos, r.endPos());
    }
    return U2Region(startPos, endPos-startPos);
}

QVector<U2Region> joinRegions(QVector<U2Region>& regions) {
    QVector<U2Region> result;
    qSort(regions);
    for(int i=0, size=regions.size(); i<size;) {
        U2Region joined = regions[i];
        ++i;
        while (i<size && joined.intersects(regions.at(i))) {
            const U2Region& next = regions.at(i);
            joined = U2Region::containingRegion(joined, next);
            ++i;
        }
        result << joined;
    }
    return result;
}

QVector<U2Region> QDResultLinker::findLocation(QDStep* step) {
    QVector<U2Region> res;
    if (candidates.isEmpty() || cancelled) {
        res << U2Region(0, scheme->getSequence().length());
        return res;
    }
    QDActor* actor = step->getActor();
    const QList<QDSchemeUnit*>& units = actor->getSchemeUnits();
    foreach(QDResultGroup* candidate, candidates) {
        bool complement = candidate->strand == QDStrand_ComplementOnly;
        QVector<U2Region> actorLocation;
        foreach(QDSchemeUnit* su, units) {
            U2Region suRegion(0, scheme->getSequence().length());
            foreach(const QDResultUnit& ru, candidate->getResultsList()) {
                foreach(QDConstraint* c, step->getConstraints(su, ru->owner)) {
                    QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(c);
                    const U2Region& reg = QDConstraintController::matchLocation(dc, ru, complement);
                    suRegion = suRegion.intersect(reg);
                }
            }
            if (suRegion.isEmpty()) {
                actorLocation.clear();
                break;
            } else {
                actorLocation.append(suRegion);
            }
        }
        if (!actorLocation.isEmpty()) {
            const U2Region& region4candidate = uniteRegions(actorLocation);
            res.append(region4candidate);
        }
    }
    res = joinRegions(res);
    return res;
}

void QDResultLinker::updateCandidates(QDStep* step, int& progress) {
    currentStep = step;
    QDActor* currentActor = step->getActor();
    currentResults = currentActor->popResults();

    QString actorGroup = scheme->getActorGroup(currentActor);
    if (!actorGroup.isEmpty()) {
        QList<QDActor*> grpMembers = scheme->getActors(actorGroup);
        int unlinkedGroupMembersLeft = grpMembers.size()-1;
        foreach(QDActor* a, grpMembers) {
            if (step->getLinkedActors().contains(a)) {
                unlinkedGroupMembersLeft--;
            }
        }
        assert(unlinkedGroupMembersLeft>=0);

        currentGroupResults.insertMulti(currentActor, currentResults);

        if (unlinkedGroupMembersLeft==0) {
            formGroupResults();
            processNewResults(progress);
            QMapIterator< QDActor*, QList<QDResultGroup*> > i(currentGroupResults);
            while(i.hasNext()) {
                i.next();
                const QList<QDResultGroup*>& res = i.value();
                qDeleteAll(res);
            }
            currentGroupResults.clear();
            qDeleteAll(currentResults);
            currentResults.clear();
        }
    } else {
        processNewResults(progress);
        qDeleteAll(currentResults);
        currentResults.clear();
    }
}

template<class T>
QList<T> addNextSelection(const QList<T>& prev, const QList<T>& source, QList< QList<T> >& result) {
    int idx = prev.size()-1;
    while (idx>=0) {
        const T& item = prev.at(idx);
        int itemIdx = source.indexOf(item);
        if (itemIdx<source.size()-prev.size()+idx) {
            QList<T> newSelection(prev);
            for (int i=idx; i<prev.size(); i++) {
                newSelection[i] = source.at(++itemIdx);
            }
            result.append(newSelection);
            return newSelection;
        } else {
            --idx;
        }
    }
    return QList<T>();
}

template<class T>
void buildSelections(const QList<T>& source, int len, QList< QList<T> >& result) {
    QList<T> newSelection;
    for (int i=0; i<len; i++) {
        newSelection.append(source.at(i));
    }
    result.append(newSelection);
    while (!newSelection.isEmpty()) {
        newSelection = addNextSelection(newSelection, source, result);
    }
}

void QDResultLinker::formGroupResults() {
    QDActor* currentActor = currentStep->getActor();
    QString grp = scheme->getActorGroup(currentActor);
    const QList<QDActor*>& grpMembers = scheme->getActors(grp);
    assert(grpMembers.size()==currentGroupResults.size());

    QList< QList<QDActor*> > groupSelections;
    int reqNum = scheme->getRequiredNumber(grp);
    for (int resNum=reqNum, n=grpMembers.size(); resNum<=n; resNum++) {
        buildSelections(grpMembers, resNum, groupSelections);
    }

    currentResults.clear();
    foreach(const QList<QDActor*>& selection, groupSelections) {
        assert(currentGroupResults.keys().contains(selection.first()));
        QList<QDResultGroup*> results = currentGroupResults.value(selection.first());
        for(int i=1; i<selection.size(); i++) {
            QList<QDResultGroup*> newResults;
            assert(currentGroupResults.keys().contains(selection.at(i)));
            QList<QDResultGroup*> nextResults = currentGroupResults.value(selection.at(i));
            foreach(QDResultGroup* res, results) {
                foreach(QDResultGroup* nextRes, nextResults) {
                    QDResultGroup* newRes = new QDResultGroup(*res);
                    newRes->add(nextRes->getResultsList());
                    newResults.append(newRes);
                }
            }
            results = newResults;
        }
        currentResults.append(results);
    }
    currentGroupResults.clear();
}
void QDResultLinker::cleanupCandidates(){
    qDeleteAll(candidates);
    candidates.clear();
}
void QDResultLinker::processNewResults(int& progress) {
    if(needInit) {
        initCandidates(progress);
        needInit = false;
    } else {
        perfLog.details(QString("Finding matches %1 candidates x %2 results")
            .arg(candidates.size()).arg(currentResults.size()));
        qint64 start, end;
        start = GTimer::currentTimeMicros();

        updateCandidates(progress);

        end = GTimer::currentTimeMicros();
        perfLog.details(QString("Updating groups finished in %1 ms").arg(GTimer::millisBetween(start,end)));
    }

    if (candidates.isEmpty() && !cancelled) {
        cancelMeassage = QDScheduler::tr("No results have been found for this scheme");
        cancelled = true;
    }
    if (!candidates.isEmpty() && cancelled) {
        cleanupCandidates();
    }
}

//void QDResultLinker::filterActorResults(QList<QDResultGroup*>&) {
//    foreach(QDResultGroup* actorResult, currentResults) {
//        for(int i=0, n=actorResult.size(); i<n-1; i++) {
//            for(int j=1; j<n; j++) {
//                const QDResultUnit& res1 = actorResult.at(i);
//                const QDResultUnit& res2 = actorResult.at(j);
//                const QList<QDConstraint*>& sharedConstraints = constraintsMap.value(qMakePair(res1->owner, res2->owner));
//                foreach(QDConstraint* c, sharedConstraints) {
//                    if (!QDConstraintController::match(c, res1, res2)) {
//                        currentResults.removeOne(actorResult);
//                    }
//                }
//            }
//        }
//    }
//}

void QDResultLinker::initCandidates(int& progress) {
    int i = 0;
    foreach(QDResultGroup* actorRes, currentResults) {
        QDStrandOption candidateStrand = findResultStrand(actorRes);
        QDResultGroup* newCandidate = new QDResultGroup(candidateStrand);
        newCandidate->add(actorRes->getResultsList());
        candidates.append(newCandidate);
        progress = 100 * ++i / currentResults.size();
    }
}

QDStrandOption QDResultLinker::findResultStrand(QDResultGroup* actorRes) {
    QDStrandOption curActorStrand = currentStep->getActor()->getStrand();
    QDStrandOption resStrand = QDStrand_Both;
    if (curActorStrand==QDStrand_DirectOnly) {
        resStrand = actorRes->strand;
    } else if (curActorStrand==QDStrand_ComplementOnly) {
        if (actorRes->strand==QDStrand_DirectOnly) {
            resStrand = QDStrand_ComplementOnly;
        } else if (actorRes->strand==QDStrand_ComplementOnly) {
            resStrand = QDStrand_DirectOnly;
        } else {
            resStrand = QDStrand_Both;
        }
    } else {
        resStrand = scheme->getStrand();
    }
    return resStrand;
}

void QDResultLinker::updateCandidates(int& progress) {
    QList<QDResultGroup*> newCandidates;
    int i = 0;

    foreach(QDResultGroup* candidate, candidates) {
        foreach(QDResultGroup* actorRes, currentResults) {
            if (sched->isCanceled()) {
                cleanupCandidates();
                qDeleteAll(newCandidates);
                newCandidates.clear();
                return;
            }

            bool matches = false;
            //define for what schema strand result is
            QDStrandOption resStrand = findResultStrand(actorRes);
            if (resStrand!=QDStrand_Both && candidate->strand!=QDStrand_Both && resStrand!=candidate->strand) {
                continue;
            }
            //
            bool complement;
            if (scheme->getStrand()==QDStrand_DirectOnly) {
                matches = canAdd(actorRes, candidate, false);
                complement = false;
            } else if (scheme->getStrand()==QDStrand_ComplementOnly) {
                matches = canAdd(actorRes, candidate, true);
                complement = true;
            } else {
                if (resStrand==QDStrand_DirectOnly) {
                    matches = canAdd(actorRes, candidate, false);
                    complement = false;
                } else if (resStrand==QDStrand_ComplementOnly) {
                    matches = canAdd(actorRes, candidate, true);
                    complement = true;
                } else {
                    matches = canAdd(actorRes, candidate, false);
                    if (!matches) {
                        matches = canAdd(actorRes, candidate, true);
                        complement = true;
                    } else {
                        complement = false;
                    }
                }
            }

            if (matches) {
                QDResultGroup* newCandidate = new QDResultGroup(*candidate);
                newCandidate->add(actorRes->getResultsList());
#ifdef DEBUG
                if (newCandidate->strand==QDStrand_DirectOnly) {
                    assert(!complement);
                }
                if (newCandidate->strand==QDStrand_ComplementOnly) {
                    assert(complement);
                }
#endif
                //if strand was not defined
                if (newCandidate->strand == QDStrand_Both) {
                    newCandidate->strand = complement ? QDStrand_ComplementOnly : QDStrand_DirectOnly;
                }
                newCandidates.append(newCandidate);
                if (maxMemorySizeInMB <= (candidates.size()+newCandidates.size())*0.00025){ //0.0002 is empirically calculated coefficient
                    cancelMeassage = QDScheduler::tr("Too many results have been found for this scheme. Try to set stricter search conditions.").arg(newCandidates.size());
                    taskLog.error(cancelMeassage);
                    qDeleteAll(newCandidates);
                    newCandidates.clear();
                    cancelled = true;
                    return;
                }
            }
        }
        candidates.replace(candidates.indexOf(candidate), NULL);
        delete candidate;
        progress = 100 * ++i / candidates.size();
    }
    candidates = newCandidates;
}

bool QDResultLinker::canAdd(QDResultGroup* actorResult, QDResultGroup* candidate, bool complement) const {
    QList<QDResultUnit> actorResults;
    QList<QDResultUnit> candidateResults;
    if (complement) {
        actorResults = prepareComplResults(actorResult);
        candidateResults = prepareComplResults(candidate);
    } else {
        actorResults = actorResult->getResultsList();
        candidateResults = candidate->getResultsList();
    }
    foreach(const QDResultUnit& actorResUnit, actorResults) {
        foreach(const QDResultUnit& candidateResUnit, candidateResults) {
            const QList<QDConstraint*>& cl = currentStep->getConstraints(actorResUnit->owner, candidateResUnit->owner);
            foreach(QDConstraint* c, cl) {
                if(!QDConstraintController::match(c, actorResUnit, candidateResUnit, complement)) {
                    return false;
                }
            }
        }
    }
    return true;
}

QList<QDResultUnit> QDResultLinker::prepareComplResults(QDResultGroup* src) const {
    QList<QDResultUnit> res = src->getResultsList();
    QList<QDActor*> simActors;
    foreach (QDResultUnit ru, res) {
        QDActor* a = ru->owner->getActor();
        if (a->isSimmetric() && !simActors.contains(a)) {
            simActors.append(a);

            QList<QDSchemeUnit*> units = a->getSchemeUnits();
            for (int i=0, n=units.size()/2; i<n; i++) {
                QDSchemeUnit* begin = units[i];
                QDSchemeUnit* end = units[units.size()-i-1];
                QMutableListIterator<QDResultUnit> it(res);
                while(it.hasNext()) {
                    QDResultUnit& resUn = it.next();
                    if (resUn->owner==begin) {
                        resUn->owner = end;
                    } else if (resUn->owner==end) {
                        resUn->owner = begin;
                    }
                }
            }
        }
    }
    return res;
}

#define RESULT_PREFIX "Result"
void QDResultLinker::prepareAnnotations() {
    qint64 start(0), end(0);
    perfLog.details(QString("%1 groups").arg(candidates.size()));
    start = GTimer::currentTimeMicros();

    if (sched->getSettings().outputType == QDRunSettings::Single) {
        createMergedAnnotations(RESULT_PREFIX);
    } else {
        createAnnotations(RESULT_PREFIX);
    }

    end = GTimer::currentTimeMicros();
    perfLog.details(QString("push to table in %1 ms").arg(GTimer::millisBetween(start, end)));
}

void QDResultLinker::createAnnotations(const QString& groupPrefix) {
    int counter = 0;
    foreach(QDResultGroup* candidate, candidates) {
        if (sched->isCanceled()) {
            cleanupCandidates();
            return;
        }
        const QString& grpName = QString("%1 %2")
            .arg(groupPrefix)
            .arg(QString::number(++counter));

        QList<SharedAnnotationData> groupAnns;

        foreach(const QDResultUnit& res, candidate->getResultsList()) {
            SharedAnnotationData a = result2annotation.value(res, SharedAnnotationData());
            if (a == SharedAnnotationData()) {
                SharedAnnotationData ad(new AnnotationData);
                ad->name = prepareAnnotationName(res);
                ad->setStrand(res->strand);
                ad->location->regions.append(res->region);
                ad->qualifiers = res->quals;
                a = ad;
                result2annotation[res] = a;
            }
            groupAnns.append(a);
        }
        annotations[grpName] = groupAnns;
        candidates.replace(candidates.indexOf(candidate), NULL);
        delete candidate;
    }
    candidates.clear();
}

void QDResultLinker::createMergedAnnotations(const QString &groupPrefix) {
    const QDRunSettings &settings = sched->getSettings();
    int offset = settings.offset;
    U2Region seqRange(0, scheme->getSequence().length());
    QList<SharedAnnotationData> anns;
    foreach (QDResultGroup *candidate, candidates) {
        if (sched->isCanceled()) {
            cleanupCandidates();
            return;
        }

        qint64 startPos = candidate->getResultsList().first()->region.startPos;
        qint64 endPos = candidate->getResultsList().first()->region.endPos();
        foreach (const QDResultUnit &ru, candidate->getResultsList()) {
            startPos = qMin(startPos, ru->region.startPos);
            endPos = qMax(endPos, ru->region.endPos());
        }
        startPos = qMax(seqRange.startPos, startPos - offset);
        endPos = qMin(seqRange.endPos(), endPos + offset);
        U2Region r(startPos, endPos-startPos);

        SharedAnnotationData ad;
        ad->name = groupPrefix;
        ad->location->regions.append(r);
        anns.append(ad);
        candidates.replace(candidates.indexOf(candidate), NULL);
        delete candidate;
    }
    candidates.clear();
    annotations[""] = anns;
}

void QDResultLinker::pushToTable() {
    const QDRunSettings &settings = sched->getSettings();
    AnnotationTableObject *ao = settings.annotationsObj;
    SAFE_POINT(NULL != ao, "Invalid annotation table detected!",);

    AnnotationGroup *root = ao->getRootGroup();
    if (!settings.groupName.isEmpty()) {
        root = root->getSubgroup(settings.groupName, true);
    }

    QMapIterator<QString, QList<SharedAnnotationData> > iter(annotations);
    while (iter.hasNext()) {
        iter.next();
        AnnotationGroup *ag = root;
        if (!iter.key().isEmpty()) {
            ag = root->getSubgroup(iter.key(), true);
        }
        QList<SharedAnnotationData> data = iter.value();
        U1AnnotationUtils::addDescriptionQualifier(data, settings.annDescription);
        ag->addAnnotations(data);
    }
}

//QDStep
//////////////////////////////////////////////////////////////////////////
QDStep::QDStep(QDScheme* _scheme) : scheme(_scheme) {
    assert(!scheme->getActors().isEmpty());
    actor = scheme->getActors().first();
    initTotalMap();
}

void QDStep::initTotalMap() {
    QList<QDSchemeUnit*> units;
    foreach(QDActor const* a, scheme->getActors()) {
        units << a->getSchemeUnits();
    }
    for(int srcIdx=0; srcIdx<units.size()-1; srcIdx++) {
        for(int dstIdx=srcIdx+1; dstIdx<units.size(); dstIdx++) {
            QDSchemeUnit* srcSu = units.at(srcIdx);
            QDSchemeUnit* dstSu = units.at(dstIdx);
            QList<QDConstraint*> sharedConstraints = scheme->getConstraints(srcSu, dstSu);
            //build rough constraint if there is no direct constraints
            if (sharedConstraints.isEmpty()) {
                const QList<QDPath*>& paths = scheme->findPaths(srcSu, dstSu);
                //use only paths containing no linked units except source(destination)
                QList<QDPath*> allowedPaths = paths;
                //remove paths containing optional items
                QMutableListIterator<QDPath*> i(allowedPaths);
                while(i.hasNext()) {
                    QDPath* p = i.next();
                    foreach(QDSchemeUnit* su, p->getSchemeUnits()) {
                        if (su!=srcSu && su!=dstSu) {
                            QDActor* a = su->getActor();
                            QString group = scheme->getActorGroup(a);
                            if (!group.isEmpty()) {
                                i.remove();
                            }
                        }
                    }
                }
                //create overall constraint from list of paths between srcSu and dstSu
                if (!allowedPaths.isEmpty()) {
                    QDDistanceConstraint* overallConstraint = allowedPaths.first()->toConstraint();
                    int min = overallConstraint->getMin();
                    int max = overallConstraint->getMax();
                    for (int i=1, n=allowedPaths.size(); i<n; i++) {
                        QDPath* curPath = allowedPaths.at(i);
                        QDDistanceConstraint* curDc = curPath->toConstraint();

                        if (curDc->getSource()!=overallConstraint->getSource()) {
                            curDc->invert();
                        }
                        assert(curDc->getSource()==overallConstraint->getSource());
                        assert(curDc->getDestination()==overallConstraint->getDestination());
                        min = qMax(min, curDc->getMin());
                        max = qMin(max, curDc->getMax());
                    }
                    overallConstraint->setMin(min);
                    overallConstraint->setMax(max);
                    sharedConstraints.append(overallConstraint);
                }
            }
            constraintsMap.insertMulti(qMakePair(srcSu, dstSu), sharedConstraints);
            constraintsMap.insertMulti(qMakePair(dstSu, srcSu), sharedConstraints);
        }
    }
}

QList<QDConstraint*> QDStep::getConstraints(QDSchemeUnit* subj, QDSchemeUnit* linked) const {
    const QPair<QDSchemeUnit*, QDSchemeUnit*>& pair = qMakePair(subj, linked);
    assert(constraintsMap.contains(pair));
    return constraintsMap.value(pair);
}

void QDStep::next() {
    linkedActors.append(actor);
    const QList<QDActor*>& actors = scheme->getActors();
    int idx = actors.indexOf(actor)+1;
    assert(idx>=0 && idx<actors.size());
    actor = actors.at(idx);
}

bool QDStep::hasNext() const {
    const QList<QDActor*>& actors = scheme->getActors();
    int idx = actors.indexOf(actor);
    if (++idx>=actors.size()) {
        return false;
    }
    return true;
}

bool QDStep::hasPrev() const {
    const QList<QDActor*>& actors = scheme->getActors();
    int idx = actors.indexOf(actor);
    if (idx>0) {
        return true;
    }
    return false;
}

//QDTask
//////////////////////////////////////////////////////////////////////////
QDTask::QDTask(QDStep* _step, QDResultLinker* _linker)
: Task(tr("Query task: %1").arg(_step->getActor()->getParameters()->getLabel()), TaskFlag_NoRun), step(_step), linker(_linker), runTask(NULL) {
    tpm = Progress_Manual;
    stateInfo.progress = 0;

    findLocationTask = new QDFindLocationTask(step, linker);
    addSubTask(findLocationTask);
}

QList<Task*> QDTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subs;
    if (subTask->hasError()) {
        propagateSubtaskError();
        return subs;
    }
    if (isCanceled()) {
        return subs;
    }
    if (subTask==findLocationTask) {
        stateInfo.progress = 10;
        const QVector<U2Region>& searchLocation = findLocationTask->getSearchLocation();
        curActorLocation = findLocationTask->getResultLocation();
        if (searchLocation.isEmpty()) {
            return subs;
        }
        runTask = step->getActor()->getAlgorithmTask(searchLocation);
        assert(runTask);
        connect(runTask, SIGNAL(si_progressChanged()), SLOT(sl_updateProgress()));
        subs.append(runTask);
    }
    if (subTask==runTask) {
        stateInfo.progress = 50;
        assert(!curActorLocation.isEmpty());
        step->getActor()->filterResults(curActorLocation);
        QDLinkResultsTask* linkTask = new QDLinkResultsTask(step, linker);
        connect(linkTask, SIGNAL(si_progressChanged()), SLOT(sl_updateProgress()));
        subs.append(linkTask);
    }
    return subs;
}

static const float FIND_LOC_PROGRESS_WEIGHT = 0.05f;
static const float RUN_TASK_PROGRESS_WEIGHT = 0.45f;
static const float LINK_TASK_PROGRESS_WEIGHT = 0.5f;
static const int RUN_START = FIND_LOC_PROGRESS_WEIGHT*100;
static const int LINK_START = (FIND_LOC_PROGRESS_WEIGHT + RUN_TASK_PROGRESS_WEIGHT) * 100;

void QDTask::sl_updateProgress() {
    Task* sub = qobject_cast<Task*>(sender());
    if (sub==findLocationTask) {
    } else if (sub==runTask) {
        stateInfo.progress = RUN_START + sub->getProgress() * RUN_TASK_PROGRESS_WEIGHT;
    } else { //linkTask
        stateInfo.progress = LINK_START + sub->getProgress() * LINK_TASK_PROGRESS_WEIGHT;
    }
}

//QDFindLocationtask
//////////////////////////////////////////////////////////////////////////
const int QDFindLocationTask::REGION_DELTA(10);

void QDFindLocationTask::run() {
    const U2Region& seqReg = linker->getScheduler()->getSettings().region;
    if (step->hasPrev()) {
        resultLocation = linker->findLocation(step);
        foreach(const U2Region& r, resultLocation) {
            qint64 startPos = qMax(seqReg.startPos, r.startPos - REGION_DELTA);
            qint64 endPos = qMin(seqReg.endPos(), r.endPos() + REGION_DELTA);
            U2Region extended(startPos, endPos - startPos);
            searchLocation.append(extended);
        }
        searchLocation = joinRegions(searchLocation);
    }
    else {
        resultLocation << seqReg;
        searchLocation = resultLocation;
    }
}

}//namespace
