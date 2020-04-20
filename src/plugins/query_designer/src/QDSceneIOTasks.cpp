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

#include "QDSceneIOTasks.h"
#include "QueryViewController.h"
#include "QueryViewItems.h"
#include "QueryViewAdapter.h"

#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>

#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Core/QVariantUtils.h>

#include <QFile>

namespace U2 {

//QDSaveSceneTask
//////////////////////////////////////////////////////////////////////////
QDSaveSceneTask::QDSaveSceneTask(QueryScene* scene, const QDSceneInfo& info)
: Task(tr("Save query scheme"), TaskFlag_None), path(info.path), scene(scene) {
//    int st = path.lastIndexOf("/")+1;
//    int end = path.indexOf(".", st);
    QString id = info.schemeName;//path.mid(st, end-st);
    QDDocument* doc = QDSceneSerializer::scene2doc(scene);
    doc->setName(id);
    doc->setDocDesc(info.description);
    rawData = doc->toByteArray();
}

void QDSaveSceneTask::run() {
    ioLog.details(tr("Saving scheme to file: %1").arg(path));
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qint64 s = f.write(rawData);
        f.close();
        if (s != rawData.size()) {
            stateInfo.setError(L10N::errorWritingFile(path));
        }
    } else {
        stateInfo.setError(L10N::errorOpeningFileWrite(path));
    }
}

//QDLoadSceneTask
//////////////////////////////////////////////////////////////////////////
QDLoadSceneTask::QDLoadSceneTask(QueryScene* scene, const QString& url)
: Task(tr("Load query scheme"), TaskFlag_NoRun), scene(scene), url(url) {
    scene->clearScene();
}

void QDLoadSceneTask::prepare() {
    QDLoadDocumentTask* t = new QDLoadDocumentTask(url);
    addSubTask(t);
}

QList<Task*> QDLoadSceneTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    QDLoadDocumentTask* t = qobject_cast<QDLoadDocumentTask*>(subTask);
    assert(t);
    QDDocument* loadedDoc = t->getDocument();
    assert(loadedDoc);
    docs << loadedDoc;
    foreach(const QString& importUrl, loadedDoc->getImportedUrls()) {
        QDLoadDocumentTask* sub = new QDLoadDocumentTask(importUrl);
        subTasks << sub;
    }
    return subTasks;
}

Task::ReportResult QDLoadSceneTask::report() {
    if (!stateInfo.hasError()) {
        QDSceneSerializer::doc2scene(scene, docs);
    }
    return Task::ReportResult_Finished;
}

//QDLoadSchemetask
//////////////////////////////////////////////////////////////////////////
QDLoadSchemeTask::QDLoadSchemeTask(const QString& uri)
: Task(tr("Load query task"), TaskFlag_NoRun) {
    scheme = new QDScheme;
    QDLoadDocumentTask* t = new QDLoadDocumentTask(uri);
    addSubTask(t);
}

QList<Task*> QDLoadSchemeTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    QDLoadDocumentTask* t = qobject_cast<QDLoadDocumentTask*>(subTask);
    assert(t);
    QDDocument* loadedDoc = t->getDocument();
    assert(loadedDoc);
    docs << loadedDoc;
    foreach(const QString& importUrl, loadedDoc->getImportedUrls()) {
        QDLoadDocumentTask* sub = new QDLoadDocumentTask(importUrl);
        subTasks << sub;
    }
    return subTasks;
}

Task::ReportResult QDLoadSchemeTask::report() {
    if (!stateInfo.hasError()) {
        QDSceneSerializer::doc2scheme(docs, scheme);
    }
    return Task::ReportResult_Finished;
}

//QDLoadDocumentTask
//////////////////////////////////////////////////////////////////////////
QDLoadDocumentTask::QDLoadDocumentTask(const QString& url)
: Task(tr("Loading document from %1").arg(url), TaskFlag_None), doc(NULL), url(url) {}

void QDLoadDocumentTask::run() {
    ioLog.details(tr("Loading scheme from file: %1").arg(url));
    doc = new QDDocument;
    //url = "E:/!sandbox/Schema.txt";
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) {
        stateInfo.setError(  L10N::errorOpeningFileRead(url) );
        return;
    }
    QByteArray data = f.readAll();
    const QString& content = QString::fromUtf8(data);
    f.close();
    bool res = doc->setContent(content);
    if (!res) {
        stateInfo.setError( tr("Invalid content: %1").arg(url) );
    }
}

//QDSceneSerializer
//////////////////////////////////////////////////////////////////////////
bool QDSceneSerializer::doc2scene(QueryScene* scene, const QList<QDDocument*>& docs) {
    QMap<QDElementStatement*, QDActor*> stmt2actor;
    QDScheme* scheme = scene->getScheme();

    bool res = doc2scheme(docs, stmt2actor, scheme);
    if (!res) {
        return false;
    }

    foreach(QDActor* actor, scheme->getActors()) {
        QObject::connect(actor->getParameters(), SIGNAL(si_modified()),
            scene, SLOT(sl_updateRulerText()));
    }

    const QPointF& topLeftCorner = scene->annotationsArea().topLeft();
    QueryViewAdapter qva(scheme, topLeftCorner);
    foreach(QDElement* uv, qva.getElements()) {
        QDActor* actor = uv->getActor();
        QDElementStatement* actorStmt = stmt2actor.key(actor, NULL);
        assert(actorStmt);
        QString unitId;
        if (actor->getSchemeUnits().size()==1) {
            unitId = "unit";
        }
        else {
            unitId = actor->getUnitId(uv->getSchemeUnit());
        }
        QDElementStatement* el = docs.at(0)->getElement(actorStmt->getId() + "." + unitId);
        if (el) {
            uv->loadState(el);
        }
        else {
            qva.placeElement(uv);
        }
    }
    qva.sortVertically();
    foreach(QDElement* uv, qva.getElements()) {
        scene->addItem(uv);
    }

    QList<QDConstraint*> haveEditor;
    foreach(Footnote* fn, qva.getFootnotes()) {
        scene->addItem(fn);
        fn->updatePos();
        QDConstraint* owner = fn->getConstraint();
        if (!haveEditor.contains(owner)) {
            QueryViewController::setupConstraintEditor(owner);
            haveEditor.append(owner);
        }
    }

    QString docName = docs.first()->getName();
    QString docDesc = docs.first()->getDocDesc();
    scene->setLabel(docName);
    scene->setDescription(docDesc);
    scene->sl_updateRulerText();

    return true;
}

bool QDSceneSerializer::doc2scheme(const QList<QDDocument*>& docs, QDScheme* scheme) {
    QMap<QDElementStatement*, QDActor*> stmt2actor;
    return doc2scheme(docs, stmt2actor, scheme);
}

bool QDSceneSerializer::doc2scheme(const QList<QDDocument*>& docs, QMap<QDElementStatement*, QDActor*>& stmt2actor, QDScheme* scheme) {
    QList<QDElementStatement*> groups;
    foreach(QDDocument* doc, docs) {
        groups.append(doc->getElements(Group));
    }

    QDActor* actor = NULL;
    //map QDElementStatement to QDActor created from it

    foreach(QDElementStatement* grpStmt, groups) {
        //if grpStmt references to stmt defined in other query
        //find it and instantiate with that query attributes
        if (grpStmt->getId()==QDDocument::GROUPS_SECTION) {
            continue;
        }
        const QString& definedIn = grpStmt->definedIn();
        QString group;
        if (!definedIn.isEmpty()) {
            QDElementStatement* actualStmt = NULL;
            foreach(QDDocument* importedDoc, docs) {
                if (importedDoc->getName()==definedIn) {
                    actualStmt = importedDoc->getElement(grpStmt->getId());
                }
            }
            if (actualStmt==NULL) {
             //   algoLog.error(QObject::tr("{%1} is not found in imported files.").arg(grpStmt->getId()));
                return false;
            }
            actor = QDSchemeSerializer::loadActor(actualStmt, group);
        }
        else {
            actor = QDSchemeSerializer::loadActor(grpStmt, group);
        }
        if (!actor) {
            return false;
        }
        stmt2actor[grpStmt] = actor;
        scheme->addActor(actor);
        if (!group.isEmpty()) {
            if(!scheme->getActorGroups().contains(group)) {
                scheme->createActorGroup(group);
            }
            scheme->addActorToGroup(actor, group);
        }
    }

    //load constraints
    QList<QDLinkStatement*> lnks;
    foreach(QDDocument* doc, docs) {
        lnks << doc->getLinks();
    }
    foreach(QDLinkStatement* lnk, lnks) {
        QDConstraint* c = QDSchemeSerializer::loadConstraint(lnk, stmt2actor, docs);
        if (c) {
            scheme->addConstraint(c);
        }
        else {
            return false;
        }
    }

    //specify order
    QStringList ids = docs.first()->getOrder();
    if (!ids.isEmpty()) {
        for (int idx=0, n = ids.size(); idx < n; idx++) {
            QString id = ids.at(idx);
            foreach(QDActor* a, scheme->getActors()) {
                if (a->getParameters()->getLabel()==id) {
                    scheme->setOrder(a, idx);
                }
            }
        }
    }

    QDStrandOption strOpt = docs.first()->getSchemaStrand();
    scheme->setStrand(strOpt);

    //load groups
    bool ok = QDSchemeSerializer::loadGroups(scheme, docs.first());
    if (!ok) {
        return false;
    }

    return true;
}

QDDocument* QDSceneSerializer::scene2doc(QueryScene* scene) {
    QDDocument* doc = new QDDocument;
    QMap<QDSchemeUnit*, QDElementStatement*> unit2stmt;
    QDScheme* scheme = scene->getScheme();
    const QList<QDActor*>& sceneActors = scheme->getActors();
    foreach(QDActor* actor, sceneActors) {
        QDElementStatement* actorElement = QDSchemeSerializer::saveActor(actor, doc);
        foreach(QDSchemeUnit* su, actor->getSchemeUnits()) {
            QString name = actorElement->getId()+".";
            if (actor->getSchemeUnits().size()==1) {
                name+="unit";
            }
            else {
                name+=su->getId();
            }
            QDElementStatement* unitElement = new QDElementStatement(name, Element);
            unit2stmt[su] = unitElement;
            bool res = doc->addElement(unitElement);
            Q_UNUSED(res);
            assert(res);
            QDElement const* uv = scene->getUnitView(su);
            assert(uv);
            uv->saveState(unitElement);
        }
    }
    foreach(QDConstraint* c, scheme->getConstraints()) {
        QDSchemeSerializer::saveConstraint(c, doc, unit2stmt);
    }
    doc->saveOrder(scheme->getActors());
    doc->setSchemaStrand(scheme->getStrand());
    QDSchemeSerializer::saveGroups(scheme, doc);
    return doc;
}

//QDSchemeSerializer
//////////////////////////////////////////////////////////////////////////
static const QString GROUP_ATTR = "group";

static const QString STRAND_ATTR = "strand";
static const QString STRAND_DIRECT = "direct";
static const QString STRAND_COMPLEMENT = "complement";
static const QString STRAND_BOTH = "both";

static QMap<QDStrandOption, QString> initDirectionMap() {
    QMap<QDStrandOption, QString> res;
    res.insertMulti(QDStrand_DirectOnly, STRAND_DIRECT);
    res.insertMulti(QDStrand_ComplementOnly, STRAND_COMPLEMENT);
    res.insertMulti(QDStrand_Both, STRAND_BOTH);
    return res;
}

const QMap<QDStrandOption, QString> QDSchemeSerializer::STRAND_MAP(initDirectionMap());

QDElementStatement* QDSchemeSerializer::saveActor( QDActor* actor, QDDocument* doc ) {
    /*const QString& suffix = QString::number(doc->getElements(Group).size() + 1);
    QDElementStatement* actorElement = new QDElementStatement("A"+suffix, Group);*/
    QString elementName = actor->getParameters()->getLabel();
    QDElementStatement* actorElement = new QDElementStatement(elementName, Group);
    bool res = doc->addElement(actorElement);
    Q_UNUSED(res);
    assert(res);
    actorElement->setAttribute(QDElementStatement::ALGO_ATTR_NAME, actor->getActorType());
    QList< QPair<QString,QString> > attrsMap = actor->saveConfiguration();
    QString group = actor->getScheme()->getActorGroup(actor);
    if (!group.isEmpty()) {
        attrsMap.append(qMakePair(GROUP_ATTR, group));
    }
    foreach(const StringAttribute& attr, attrsMap) {
        actorElement->setAttribute(attr.first, attr.second);
    }
    if (actor->getStrand()!=QDStrand_Both) {
        actorElement->setAttribute(STRAND_ATTR, STRAND_MAP.value(actor->getStrand()));
    }
    return actorElement;
}

QDActor* QDSchemeSerializer::loadActor(QDElementStatement* actorElement, QString& group) {
    assert(actorElement->getType()==Group);
    const QString& algo = actorElement->getAttribute(QDElementStatement::ALGO_ATTR_NAME);
    if (algo.isEmpty()) {
        return NULL;
    }
    QDActorPrototypeRegistry* afr = AppContext::getQDActorProtoRegistry();
    if (!afr->getAllIds().contains(algo)) {
        ioLog.error(QObject::tr("Can not find %1.").arg(algo));
        return NULL;
    }
    QDActor* actor = afr->getProto(algo)->createInstance();
    if (actor==NULL) {
        return NULL;
    }
    QString actorName = actorElement->getId();
    actor->getParameters()->setLabel(actorName);
    actor->loadConfiguration(actorElement->getAttributes());

    QString dirAttrVal = actorElement->getAttribute(STRAND_ATTR);
    if (!dirAttrVal.isEmpty()) {
        if (!STRAND_MAP.values().contains(dirAttrVal)) {
            coreLog.error(QObject::tr("Error loading file."));
            return NULL;
        }
        actor->setStrand(STRAND_MAP.key(dirAttrVal));
    }

    group = actorElement->getAttribute(GROUP_ATTR);

    return actor;
}

static const QString DISTANCE_ATTR_NAME = "distance_type";
static const QString MIN_ATTR_NAME = "min";
static const QString MAX_ATTR_NAME = "max";
QDLinkStatement* QDSchemeSerializer::saveConstraint(QDConstraint* constraint, QDDocument* doc, const QMap<QDSchemeUnit*,QDElementStatement*>& unit2stmt) {
    if (QDConstraintTypes::DISTANCE==constraint->constraintType()) {
        QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(constraint);
        QDSchemeUnit* src = dc->getSource();
        QDSchemeUnit* dst = dc->getDestination();
        QList<QString> elIds;
        QString srcElId = unit2stmt.value(src)->getId();
        QString dstElId = unit2stmt.value(dst)->getId();
        elIds << srcElId << dstElId;
        QDLinkStatement* link = new QDLinkStatement(elIds);
        doc->addLink(link);
        link->setAttribute(QDLinkStatement::TYPE_ATTR_NAME, QDIdMapper::constraintType2string(QDConstraintTypes::DISTANCE));
        link->setAttribute(DISTANCE_ATTR_NAME, QDIdMapper::distance2string(dc->distanceType()));
        link->setAttribute(MIN_ATTR_NAME, QString::number(dc->getMin()));
        link->setAttribute(MAX_ATTR_NAME, QString::number(dc->getMax()));
        return link;
    }
    return NULL;
}

QDConstraint* QDSchemeSerializer::loadConstraint(QDLinkStatement* lnk, const QMap<QDElementStatement*, QDActor*>& stmt2unit, const QList<QDDocument*>& docs) {
    const QDConstraintType& typeStr = QDIdMapper::string2constraintType(lnk->getAttribute(QDLinkStatement::TYPE_ATTR_NAME));
    if (typeStr.isEmpty()) {
        return NULL;
    }
    if (QDConstraintTypes::DISTANCE==typeStr) {
        const QString& distanceStr = lnk->getAttribute(DISTANCE_ATTR_NAME);
        const QString& minS = lnk->getAttribute(MIN_ATTR_NAME);
        const QString& maxS = lnk->getAttribute(MAX_ATTR_NAME);
        const QList<QString>& elIds = lnk->getElementIds();
        if (distanceStr.isEmpty() || minS.isEmpty() || maxS.isEmpty() || elIds.size()!=2) {
            return NULL;
        }
        const QString& srcId = elIds.at(0);
        const QString& dstId = elIds.at(1);
        QDSchemeUnit* src = findSchemeUnit(srcId, lnk->getDocument(), stmt2unit, docs);
        if(!src) {
            return NULL;
        }
        QDSchemeUnit* dst = findSchemeUnit(dstId, lnk->getDocument(), stmt2unit, docs);
        if(!dst) {
            return NULL;
        }
        QList<QDSchemeUnit*> units;
        units << src << dst;
        int min = minS.toInt();
        int max = maxS.toInt();
        int distTypeInt = QDIdMapper::string2distance(distanceStr);
        if (distTypeInt<0) {
            return NULL;
        }
        QDDistanceType distType = QDDistanceType(distTypeInt);
        QDDistanceConstraint* dc = new QDDistanceConstraint(units, distType, min, max);
        return dc;
    }
    return NULL;
}

QDSchemeUnit* QDSchemeSerializer::findSchemeUnit(const QString& id, QDDocument* doc, const QMap<QDElementStatement*, QDActor*> stmt2actor, const QList<QDDocument*>& docs) {
    const QString& docName = QDDocument::definedIn(id);
    if (docName.isEmpty()) {
        int lastPointIdx = id.lastIndexOf('.');
        const QString& actorId = id.mid(0, lastPointIdx);
        const QString& unitId = id.mid(lastPointIdx+1);
        QDActor* actor = stmt2actor.value(doc->getElement(actorId));
        if (actor->getSchemeUnits().size()==1) {
            return actor->getSchemeUnits().first();
        }
        return actor->getSchemeUnit(unitId);
    }
    foreach(QDDocument* doc, docs) {
        if (doc->getName()==docName) {
            const QString& elStr = QDDocument::getLocalName(id);
            int lastPointIdx = elStr.lastIndexOf('.');
            const QString& actorId = elStr.mid(0, lastPointIdx);
            const QString& unitId = elStr.mid(lastPointIdx+1);
            QDActor* actor = stmt2actor.value(doc->getElement(actorId));
            if (actor->getSchemeUnits().size()==1) {
                return actor->getSchemeUnit(unitId);
            }
            return actor->getSchemeUnit(unitId);
        }
    }
    return NULL;
}

void QDSchemeSerializer::saveGroups(QDScheme* scheme, QDDocument* doc) {
    QDElementStatement* elStmt = new QDElementStatement(QDDocument::GROUPS_SECTION, Group);
    foreach(QString group, scheme->getActorGroups()) {
        elStmt->setAttribute(group, QString::number(scheme->getRequiredNumber(group)));
        doc->addElement(elStmt);
    }
}

bool QDSchemeSerializer::loadGroups(QDScheme* scheme, QDDocument* doc) {
    QDElementStatement* el = doc->getElement(QDDocument::GROUPS_SECTION);
    if (!el) {
        return true;
    }
    for(int idx=0, n=el->getAttributes().size(); idx<n; idx++) {
        QPair<QString, QString> attr = el->getAttributes().at(idx);
        QString group = attr.first;
        if (!scheme->getActorGroups().contains(group)) {
            coreLog.error(QObject::tr("Error loading schema. Schema does not contain group %1.").arg(group));
            return false;
        }
        bool ok;
        int reqNum = attr.second.toInt(&ok);
        if (!ok) {
            return false;
        }
        scheme->setRequiredNum(group, reqNum);
    }
    return true;
}

}//namespace
