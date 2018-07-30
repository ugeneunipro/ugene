/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/Log.h>

#include <U2Lang/SupportClass.h>

#include "ConfigurationEditor.h"
#include "QDConstraint.h"
#include "QDScheme.h"

namespace U2 {

void QDParameters::setParameter(const QString& name, const QVariant& val) {
    Configuration::setParameter(name, val);
    emit si_modified();
}

//QDActor
//////////////////////////////////////////////////////////////////////////
const int QDActor::DEFAULT_MAX_RESULT_LENGTH(10000);

QDActor::QDActor(QDActorPrototype const* _proto) : scheme(NULL), proto(_proto), strand(QDStrand_Both), simmetric(false) {
    cfg = new QDActorParameters;
    foreach(Attribute* a, proto->getParameters()) {
        cfg->addParameter(a->getId(), a->clone());
    }
    ConfigurationEditor* ed = proto->getEditor();

    if (ed) {
        cfg->setEditor(ed);
    }

    const QMap<QString, Attribute*>& attrs = cfg->getParameters();
    QMapIterator<QString, Attribute*> it(attrs);
    while (it.hasNext()) {
        it.next();
        defaultCfg[it.key()] = it.value()->getAttributePureValue();
    }
}

QDActor::~QDActor() {
    qDeleteAll(paramConstraints);
    delete cfg;
}

void QDActor::reset() {
    const QMap<QString, Attribute*>& attrs = cfg->getParameters();
    foreach(const QString& key, attrs.keys()) {
        Attribute* a = attrs[key];
        a->setAttributeValue(defaultCfg.value(key));
    }
}

bool contains(const QDResultUnit& res, const QVector<U2Region>& location) {
    foreach(const U2Region& r, location) {
        if (r.contains(res->region)) {
            return true;
        }
    }
    return false;
}

void QDActor::filterResults(const QVector<U2Region>& ) {
    /*QList<QDResultGroup*> res = results;
    foreach(QDResultGroup* grp, res) {
        foreach(const QDResultUnit& ru, grp->getResultsList()) {
            if (!contains(ru, location)) {
                results.removeOne(grp);
                break;
            }
        }
    }*/
}

QList<QDResultGroup*> QDActor::popResults() {
    QList<QDResultGroup*> res = results;
    results.clear();
    return res;
}

static const QString KEY_ATTR = "key";

QList< QPair<QString,QString> > QDActor::saveConfiguration() const {
    QList< QPair<QString,QString> > res;
    QMapIterator<QString, Attribute*> it(cfg->getParameters());
    QString annKey = cfg->getAnnotationKey();
    if (annKey.contains(' ')) {
        annKey = "\"" + annKey + "\"";
    }
    res.append(qMakePair(KEY_ATTR, annKey));
    while (it.hasNext()) {
        it.next();
        Attribute* a = it.value();
        if (a->getAttributePureValue()==defaultCfg.value(it.key())) {
            continue;
        }
        //QString displayName = QDAttributeNameConverter::convertAttrName(a->getDisplayName());
        QString displayName = a->getId();
        QPair<QString,QString> newAttr = qMakePair(displayName, a->getAttributePureValue().toString());
        res.append(newAttr);
    }
    return res;
}

void QDActor::loadConfiguration(const QList< QPair<QString, QString> >& strMap) {
    foreach(const StringAttribute& attr, strMap) {
        if ( attr.first==KEY_ATTR ) {
            cfg->setAnnotationKey(attr.second);
        }
        QMapIterator<QString, Attribute*> paramsIterator(cfg->getParameters());
        while(paramsIterator.hasNext()) {
            paramsIterator.next();
            Attribute* a = paramsIterator.value();
            if (QDAttributeNameConverter::convertAttrName(a->getId())==attr.first) {
                QVariant val = QDAttributeValueMapper::stringToAttributeValue(attr.second);
                cfg->setParameter(a->getId(), val);
                break;
            }
        }
    }
}

QDStrandOption QDActor::getStrandToRun() const {
    QDStrandOption schemaStrand = scheme->getStrand();
    QDStrandOption strand2run = QDStrand_Both;
    if (schemaStrand!=QDStrand_Both) {
        if (schemaStrand==QDStrand_DirectOnly) {
            strand2run = strand;
        }
        if (schemaStrand==QDStrand_ComplementOnly) {
            if (strand==QDStrand_DirectOnly) {
                strand2run = QDStrand_ComplementOnly;
            }
            if (strand==QDStrand_ComplementOnly) {
                strand2run = QDStrand_DirectOnly;
            }
        }
    }
    return strand2run;
}

QDStrandOption QDActor::getStrand() const {
    if (hasStrand()) {
        return strand;
    }
    //return QDStrand_DirectOnly;
    return QDStrand_Both;
}

void QDActor::setStrand( QDStrandOption stOp ) {
    strand = stOp;
    emit si_strandChanged(strand);
}

QList<QDConstraint*> QDActor::getConstraints() const {
    QList<QDConstraint*> res;
    foreach(QDSchemeUnit* su, units) {
        res << su->getConstraints();
    }
    return res;
}

//QDSchemeUnit
//////////////////////////////////////////////////////////////////////////
QString QDSchemeUnit::getPersonalName() const {
    const QList<QDSchemeUnit*>& units = actor->getSchemeUnits();
    if(units.size()==1) {
        return actor->getParameters()->getLabel();
    }
    QDSchemeUnit* s = const_cast<QDSchemeUnit*>(this);
    assert(units.contains(s));
    int idx = units.indexOf(s) + 1;
    QString result = QString("%1_%2")
        .arg(actor->getParameters()->getLabel())
        .arg(QString::number(idx));
    return result;
}

QList<QDDistanceConstraint*> QDSchemeUnit::getDistanceConstraints() const {
    QList<QDDistanceConstraint*> res;
    foreach(QDConstraint* c, schemeConstraints) {
        QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(c);
        if (dc) {
            res.append(dc);
        }
    }
    return res;
}

//QDScheme
//////////////////////////////////////////////////////////////////////////
QDScheme::~QDScheme() {
    foreach(QDActor* a, actors) {
        removeActor(a);
    }
}

void QDScheme::addActor(QDActor* a) {
    assert(!actors.contains(a));
    assert(a->scheme==NULL);
    foreach(QDSchemeUnit* su, a->getSchemeUnits()) {
        assert(su->getConstraints().isEmpty());
        Q_UNUSED(su);
    }
    a->scheme = this;
    actors.append(a);
    emit si_schemeChanged();
}

bool QDScheme::removeActor(QDActor* a) {
    if(actors.contains(a)) {
        foreach(QDSchemeUnit* su, a->getSchemeUnits()) {
            foreach(QDConstraint* c, su->getConstraints()) {
                removeConstraint(c);
            }
        }
        actors.removeOne(a);
        const QString& ag = getActorGroup(a);
        if (!ag.isEmpty()) {
            actorGroups[ag].removeOne(a);
        }
        delete a;
        emit si_schemeChanged();
        return true;
    }
    return false;
}

void QDScheme::addConstraint(QDConstraint* constraint) {
    foreach(QDSchemeUnit* su, constraint->getSchemeUnits()) {
        assert(actors.contains(su->getActor()));
        su->schemeConstraints.append(constraint);
    }
    emit si_schemeChanged();
}

void QDScheme::removeConstraint(QDConstraint* constraint) {
    const QList<QDSchemeUnit*>& constraintUnits = constraint->getSchemeUnits();
    foreach(QDSchemeUnit* su, constraintUnits) {
        QDActor* actor = su->getActor();
        Q_UNUSED(actor);
        assert(actors.contains(actor));
        assert(su->getConstraints().contains(constraint));
        su->schemeConstraints.removeOne(constraint);
    }
    delete constraint;
    emit si_schemeChanged();
}

QList<QDConstraint*> QDScheme::getConstraints() const {
    QList<QDConstraint*> res;
    foreach(QDActor* actor, actors) {
        foreach(QDSchemeUnit* su, actor->getSchemeUnits()) {
            foreach(QDConstraint* c, su->getConstraints()) {
                if (!res.contains(c)) {
                    res.append(c);
                }
            }
        }
    }
    return res;
}

QList<QDConstraint*>
QDScheme::getConstraints(QDSchemeUnit const* su1, QDSchemeUnit const* su2) const {
    QList<QDConstraint*> sharedConstraints;
    const QList<QDConstraint*>& su1Cons = su1->getConstraints();
    const QList<QDConstraint*>& su2Cons = su2->getConstraints();
    foreach(QDConstraint* con, su1Cons) {
        if (su2Cons.contains(con)) {
            sharedConstraints.append(con);
        }
    }
    return sharedConstraints;
}

void QDScheme::clear() {
    //delete dna;
    dna = DNASequence();
    foreach(QDActor* a, actors) {
        removeActor(a);
    }
    actorGroups.clear();
    emit si_schemeChanged();
}

QList<QDSchemeUnit*> currentRoute;
QList< QList<QDSchemeUnit*> > routes;
QDSchemeUnit* routeDst = NULL;

QList<QDPath*> QDScheme::findPaths(QDSchemeUnit* src, QDSchemeUnit* dst) {
    assert(currentRoute.isEmpty());
    assert(routes.isEmpty());
    assert(!routeDst);
    routeDst = dst;
    currentRoute.append(src);
    findRoute(src);
    QList<QDPath*> res;
    foreach(const QList<QDSchemeUnit*>& route, routes) {
        QList<QDPath*> paths;
        for (int i=0, m=route.size()-1; i<m; i++) {
            QDSchemeUnit* src = route.at(i);
            QDSchemeUnit* dst = route.at(i+1);
            QList<QDConstraint*> joint = getConstraints(src, dst);

            //include "parameter" constraints
            foreach(QDConstraint* con, src->getActor()->getParamConstraints()) {
                if (con->getSchemeUnits().contains(src)
                    && con->getSchemeUnits().contains(dst)) {
                    joint.append(con);
                }
            }
            QList<QDDistanceConstraint*> jointCons;
            foreach(QDConstraint* con, joint) {
                QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(con);
                if (dc) {
                    jointCons.append(dc);
                }
            }
            assert(!jointCons.isEmpty());

            if (paths.isEmpty()) {
                foreach(QDDistanceConstraint* dc, jointCons) {
                    QDPath* newPath = new QDPath;
                    bool ok = newPath->addConstraint(dc);
                    assert(ok);
                    Q_UNUSED(ok);
                    paths << newPath;
                }
            } else {
                QList<QDPath*> newPaths;
                for (int i=1, n=jointCons.size(); i<n; i++) {
                    foreach(QDPath* path, paths) {
                        QDPath* newPath = path->clone();
                        bool ok = newPath->addConstraint(jointCons.at(i));
                        assert(ok);
                        Q_UNUSED(ok);
                        newPaths.append(newPath);
                    }
                }
                foreach(QDPath* path, paths) {
                    bool ok = path->addConstraint(jointCons.at(0));
                    assert(ok);
                    Q_UNUSED(ok);
                }
                paths << newPaths;
            }
        }
        res.append(paths);
    }
    currentRoute.clear();
    routes.clear();
    routeDst = NULL;
    return res;
}

void QDScheme::findRoute(QDSchemeUnit* curSu) {
    if (curSu==routeDst) {
        routes.append(currentRoute);
    }
    else {
        //build list of adjacent vertexes
        QList<QDSchemeUnit*> adjacentList;
        QList<QDDistanceConstraint*> dcList = curSu->getDistanceConstraints();
        //include "parameter" constraints
        foreach(QDConstraint* con, curSu->getActor()->getParamConstraints()) {
            if (con->constraintType()==QDConstraintTypes::DISTANCE) {
                QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(con);
                if (dc->getSchemeUnits().contains(curSu)) {
                    dcList.append(dc);
                }
            }
        }

        foreach(QDDistanceConstraint* dc, dcList) {
            QDSchemeUnit* adj = NULL;
            QDSchemeUnit* dcSrc = dc->getSource();
            QDSchemeUnit* dcDst = dc->getDestination();
            if (curSu==dcSrc) {
                adj = dcDst;
            }
            else {
                assert(curSu==dcDst);
                adj = dcSrc;
            }
            if (!adjacentList.contains(adj)) {
                adjacentList.append(adj);
            }
        }
        foreach(QDSchemeUnit* adj, adjacentList) {
            if (!currentRoute.contains(adj)) {
                currentRoute.append(adj);
                findRoute(adj);
                currentRoute.removeOne(adj);
            }
        }
    }
}

//QDPath
//////////////////////////////////////////////////////////////////////////
QDPath::~QDPath() {
    delete overallConstraint;
}

QDPath* QDPath::clone() const {
    QDPath* cln = new QDPath;
    cln->constraints = constraints;
    cln->pathSrc = pathSrc;
    cln->pathDst = pathDst;
    return cln;
}

bool QDPath::addConstraint(QDDistanceConstraint* dc) {
    assert(!constraints.contains(dc));
    QDSchemeUnit* dcSrc = dc->getSource();
    QDSchemeUnit* dcDst = dc->getDestination();
    if (!pathSrc) {
        assert(!pathDst);
        pathSrc = dcSrc;
        pathDst = dcDst;
        constraints.append(dc);
        schemeUnits << pathSrc << pathDst;
    }
    else if (pathDst==dcSrc) {
        pathDst = dcDst;
        constraints.append(dc);
        schemeUnits << pathDst;
    }
    else if (pathDst==dcDst) {
        pathDst = dcSrc;
        constraints.append(dc);
        schemeUnits << pathDst;
    }
    else if (pathSrc==dcSrc) {
        pathSrc = dcDst;
        constraints.prepend(dc);
        schemeUnits << pathSrc;
    }
    else if (pathSrc==dcDst) {
        pathSrc = dcSrc;
        constraints.prepend(dc);
        schemeUnits << pathSrc;
    }
    else {
        return false;
    }
    return true;
}

QDDistanceConstraint* QDPath::toConstraint() {
    if (constraints.isEmpty()) {
        return NULL;
    }
    delete overallConstraint;
    int minDist=0, maxDist=0;
    QDSchemeUnit* curSu = pathSrc;
    for (int i=0, n=constraints.size(); i<n; i++) {
        QDDistanceConstraint* curDc = constraints.at(i);
        QDDistanceConstraint* nextDc = NULL;
        if (i+1 < n) {
            nextDc = constraints.at(i+1);
        }
        QDSchemeUnit* curDcSrc = curDc->getSource();
        QDSchemeUnit* curDcDst = curDc->getDestination();

        if (curSu==curDcSrc) {
            curSu = curDcDst;
            minDist += curDc->getMin();
            maxDist += curDc->getMax();
            if (!nextDc) {
                continue;
            }
            QDSchemeUnit* nextDcSrc = nextDc->getSource();
            QDSchemeUnit* nextDcDst = nextDc->getDestination();

            if (nextDcSrc==curSu) {
                if (curDc->distanceType()==S2S || curDc->distanceType()==E2S) {
                    if (nextDc->distanceType()==E2S || nextDc->distanceType()==E2E) {
                        minDist+=curSu->getActor()->getMinResultLen();
                        maxDist+=curSu->getActor()->getMaxResultLen();
                    }
                }
                else { // S2E || E2E
                    if (nextDc->distanceType()==S2S || nextDc->distanceType()==S2E) {
                        minDist-=curSu->getActor()->getMaxResultLen();
                        maxDist-=curSu->getActor()->getMinResultLen();
                    }
                }
            }
            else {
                assert(nextDcDst==curSu);
                Q_UNUSED(nextDcDst);
                if (curDc->distanceType()==E2S || curDc->distanceType()==S2S) {
                    if (nextDc->distanceType()==S2E || nextDc->distanceType()==E2E) {
                        minDist+=curSu->getActor()->getMinResultLen();
                        maxDist+=curSu->getActor()->getMaxResultLen();
                    }
                }
                else { // S2E || E2E
                    if (nextDc->distanceType()==S2S || nextDc->distanceType()==E2S) {
                        minDist-=curSu->getActor()->getMaxResultLen();
                        maxDist-=curSu->getActor()->getMinResultLen();
                    }
                }
            }
        }
        else {
            assert(curSu==curDcDst);
            curSu = curDcSrc;
            minDist -= curDc->getMax();
            maxDist -= curDc->getMin();
            if (!nextDc) {
                continue;
            }
            QDSchemeUnit* nextDcSrc = nextDc->getSource();
            QDSchemeUnit* nextDcDst = nextDc->getDestination();

            if (nextDcSrc==curSu) {
                if (curDc->distanceType()==S2S || curDc->distanceType()==S2E) {
                    if (nextDc->distanceType()==E2S || nextDc->distanceType()==E2E) {
                        minDist+=curSu->getActor()->getMinResultLen();
                        maxDist+=curSu->getActor()->getMaxResultLen();
                    }
                }
                else { // E2S || E2E
                    if (nextDc->distanceType()==S2S || nextDc->distanceType()==S2E) {
                        minDist-=curSu->getActor()->getMaxResultLen();
                        maxDist-=curSu->getActor()->getMinResultLen();
                    }
                }
            }
            else {
                assert(nextDcDst==curSu);
                Q_UNUSED(nextDcDst);
                if (curDc->distanceType()==E2S || curDc->distanceType()==E2E) {
                    if (nextDc->distanceType()==S2S || nextDc->distanceType()==E2S) {
                        minDist-=curSu->getActor()->getMaxResultLen();
                        maxDist-=curSu->getActor()->getMinResultLen();
                    }
                }
                else { // S2E || S2S
                    if (nextDc->distanceType()==S2E || nextDc->distanceType()==E2E) {
                        minDist+=curSu->getActor()->getMinResultLen();
                        maxDist+=curSu->getActor()->getMaxResultLen();
                    }
                }
            }
        }
    }

    if (maxDist<minDist) {
        return NULL;
    }

    QDDistanceConstraint* firstDc = constraints.first();
    QDDistanceConstraint* lastDc = constraints.last();
    QList<QDSchemeUnit*> units;
    units << pathSrc << pathDst;

    QDDistanceType distType = E2S;
    if (pathSrc==firstDc->getSource()) {
        if (pathDst==lastDc->getSource()) {
            if (firstDc->distanceType()==S2S || firstDc->distanceType()==S2E) {
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==S2E) {
                    distType = S2S;
                }
                else { // E2S || E2E
                    distType = S2E;
                }
            }
            else { // E2S || E2E
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==S2E) {
                    distType = E2S;
                }
                else { // E2S || E2E
                    distType = E2E;
                }
            }
        }
        else {
            assert(pathDst==lastDc->getDestination());
            if (firstDc->distanceType()==S2S || firstDc->distanceType()==S2E) {
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==E2S) {
                    distType = S2S;
                }
                else { // S2E || E2E
                    distType = S2E;
                }
            }
            else { // E2S || E2E
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==E2S) {
                    distType = E2S;
                }
                else { // S2E || E2E
                    distType = E2E;
                }
            }
        }
    }
    else {
        assert(pathSrc==firstDc->getDestination());
        if (pathDst==lastDc->getSource()) {
            if (firstDc->distanceType()==S2S || firstDc->distanceType()==E2S) {
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==S2E) {
                    distType = S2S;
                }
                else { // E2S || E2E
                    distType = S2E;
                }
            }
            else { // S2E || E2E
                if (lastDc->distanceType()==E2S || lastDc->distanceType()==E2E) {
                    distType = E2E;
                }
                else { // S2S || S2E
                    distType = E2S;
                }
            }
        }
        else {
            assert(pathDst==lastDc->getDestination());
            if (firstDc->distanceType()==S2S || firstDc->distanceType()==E2S) {
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==E2S) {
                    distType = S2S;
                }
                else { // S2E || E2E
                    distType = S2E;
                }
            }
            else { // S2E || E2E
                if (lastDc->distanceType()==S2S || lastDc->distanceType()==E2S) {
                    distType = E2S;
                }
                else { // S2E || E2E
                    distType = E2E;
                }
            }
        }
    }
    overallConstraint = new QDDistanceConstraint(units, distType, minDist, maxDist);
    return overallConstraint;
}

void QDScheme::setOrder(QDActor* a, int serialNum) {
    assert(actors.contains(a));
    int aIdx = actors.indexOf(a);
    if (serialNum<0) {
        actors.move(aIdx, 0);
        return;
    }
    if (serialNum>=actors.size()) {
        actors.move(aIdx, actors.size()-1);
        return;
    }
    actors.move(aIdx, serialNum);
}

bool QDScheme::isValid() const {
    bool res = true;
    foreach(QDActor* actor, getActors()) {
        QDActorParameters* cfg = actor->getParameters();
        ProblemList problemList;
        if (!cfg->validate(problemList)) {
            res = false;
            foreach(const Problem& problem, problemList) {
                coreLog.error(QObject::tr("%1. %2").arg(cfg->getLabel()).arg(problem.message));
            }
        }
    }
    foreach(QDConstraint* con, getConstraints()) {
        if (con->constraintType()==QDConstraintTypes::DISTANCE) {
            QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(con);
            if (dc->getMin()>dc->getMax()) {
                coreLog.error(QObject::tr("Invalid distance values"));
                res = false;
            }
            QDActor* src = dc->getSource()->getActor();
            QDActor* dst = dc->getDestination()->getActor();
            QString group = getActorGroup(src);
            if (!group.isEmpty() && getActors(group).contains(dst)) {
                coreLog.error(QObject::tr("Constraints can not be placed between elements of the same group"));
                res = false;
            }
        }
    }
    return res;
}

QDActor* QDScheme::getActorByLabel(const QString& label) const {
    foreach(QDActor* a, actors) {
        if (a->getParameters()->getLabel()==label) {
            return a;
        }
    }
    return NULL;
}

void QDScheme::addActorToGroup( QDActor* a, const QString& group ) {
    assert(actors.contains(a));
    assert(getActorGroup(a).isEmpty());
    assert(actorGroups.keys().contains(group));
    actorGroups[group].append(a);
    emit si_schemeChanged();
}

bool QDScheme::removeActorFromGroup( QDActor* a ) {
    const QString& group = getActorGroup(a);
    if (!group.isEmpty()) {
        bool res = actorGroups[group].removeOne(a);
        if (res) {
            emit si_schemeChanged();
        }
    }
    return false;
}

void QDScheme::createActorGroup( const QString& name ) {
    assert(validateGroupName(name));
    assert(!actorGroups.keys().contains(name));
    actorGroups.insert(name, QList<QDActor*>());
    actorGroupReqNum[name] = 1;
    emit si_schemeChanged();
}

bool QDScheme::removeActorGroup( const QString& name ) {
    bool res = actorGroups.remove(name);
    emit si_schemeChanged();
    return res;
}

QString QDScheme::getActorGroup( QDActor* a ) const {
    QMapIterator< QString, QList<QDActor*> > i(actorGroups);
    while (i.hasNext()) {
        i.next();
        if (i.value().contains(a)) {
            return i.key();
        }
    }
    return QString();
}

bool QDScheme::validateGroupName( const QString& name ) const {
    if (name.isEmpty()) {
        return false;
    }
    return true;
}

void QDScheme::setRequiredNum(const QString& group, int num) {
    assert(actorGroups.keys().contains(group));
    const QList<QDActor*>& grpMembrs = actorGroups.value(group);
    Q_UNUSED(grpMembrs);
    assert(num<=grpMembrs.size());
    actorGroupReqNum[group] = num;
    emit si_schemeChanged();
}

void QDScheme::adaptActorsOrder() {
    QList<QDActor*> actorsQueue;
    foreach(QDActor* a, actors) {
        QString group = getActorGroup(a);
        if (group.isEmpty()) {
            assert(!actorsQueue.contains(a));
            actorsQueue.append(a);
        } else if (!actorsQueue.contains(a)) {
            const QList<QDActor*>& groupActors = getActors(group);
            actorsQueue.append(groupActors);
        }
    }
    actors = actorsQueue;
}

//QDResultGroup
//////////////////////////////////////////////////////////////////////////
void QDResultGroup::add(const QDResultUnit& res) {
    if(results.isEmpty()) {
        startPos = res->region.startPos;
        endPos = res->region.endPos();
    } else {
        if (res->region.startPos < startPos) {
            startPos = res->region.startPos;
        }
        if (res->region.endPos() > endPos) {
            endPos = res->region.endPos();
        }
    }
    results.append(res);
}

void QDResultGroup::add(const QList<QDResultUnit>& res) {
    foreach(const QDResultUnit& r, res) {
        add(r);
    }
}

void QDResultGroup::buildGroupFromSingleResult(const QDResultUnit& ru, QList<QDResultGroup*>& results) {
    QDStrandOption groupStrand = ru->strand == U2Strand::Direct ? QDStrand_DirectOnly : QDStrand_ComplementOnly;
    QDResultGroup* g = new QDResultGroup(groupStrand);
    g->add(ru);
    results.append(g);
}

//AttributeValueMapper
//////////////////////////////////////////////////////////////////////////

const QMap<QString, bool> QDAttributeValueMapper::BOOLEAN_MAP = initBooleanMap();

QMap<QString, bool> QDAttributeValueMapper::initBooleanMap() {
    QMap<QString, bool> map;
    map.insertMulti("true", true);
    map.insertMulti("yes", true);
    map.insertMulti("1", true);
    map.insertMulti("false", false);
    map.insertMulti("no", false);
    map.insertMulti("0", false);
    return map;
}

QVariant QDAttributeValueMapper::stringToAttributeValue(const QString& str) {
    if (getType(str)==BOOLEAN_TYPE) {
        return qVariantFromValue(BOOLEAN_MAP.value(str));
    }
    return qVariantFromValue(str);
}

QDAttributeValueMapper::ValueType QDAttributeValueMapper::getType(const QString& val) {
    if (BOOLEAN_MAP.keys().contains(val)) {
        return BOOLEAN_TYPE;
    }
    else {
        return UNKNOWN_TYPE;
    }
}

QDActorPrototype::~QDActorPrototype() {
    qDeleteAll(attributes);
    delete editor;
}

}//namespace
