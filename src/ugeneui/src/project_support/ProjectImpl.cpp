/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ProjectImpl.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/Log.h>

#include <U2Gui/ObjectViewModel.h>
#include <U2Core/CollectionUtils.h>

namespace U2 {

ProjectImpl::ProjectImpl(const QString& _name, const QString& _url, const QList<Document*>& _docs, const QList<GObjectViewState*>& _states)
    : name(_name), url(_url) {
    idGen = 0;
    mainThreadModificationOnly = true;
    foreach (Document* doc, _docs) {
        addDocument(doc);
    }
    foreach (GObjectViewState* state, _states) {
        addGObjectViewState(state);
    }
    setModified(false);

    resourceTracker = AppContext::getAppSettings()->getAppResourcePool()->getResource(UGENE_RESOURCE_ID_MEMORY);

    MWMDIManager* mdi = AppContext::getMainWindow()->getMDIManager();
    if (mdi != nullptr) {
        connect(mdi, SIGNAL(si_windowAdded(MWMDIWindow*)), SLOT(sl_onMdiWindowAdded(MWMDIWindow*)));
        connect(mdi, SIGNAL(si_windowClosing(MWMDIWindow*)), SLOT(sl_onMdiWindowClosing(MWMDIWindow*)));
    }
}

ProjectImpl::~ProjectImpl() {
    // delete all views. Documents are to be removed by a Shutdown task instance
    qDeleteAll(objectViewStates);
}

void ProjectImpl::makeClean() {
    if (!isTreeItemModified()) {
        return;
    }
    setModified(false);
    foreach (Document* d, docs) {
        d->makeClean();
    }
    assert(!isTreeItemModified());
}

void ProjectImpl::sl_removeAllrelationsWithObject(GObject* obj) {
    QList<GObject*> allObjs;
    for (Document* d : qAsConst(getDocuments())) {
        allObjs << d->getObjects();
    }
    for (GObject* object : qAsConst(allObjs)) {
        if (object != obj) {
            const QList<GObjectRelation> relationList = object->getObjectRelations();
            for (const GObjectRelation& rel : qAsConst(relationList)) {
                if (rel.ref.entityRef == obj->getEntityRef()) {
                    object->removeObjectRelation(rel);
                }
            }
        }
    }
}

void ProjectImpl::setProjectName(const QString& newName) {
    if (name == newName) {
        return;
    }
    setModified(true);
    name = newName;
    emit si_projectRenamed(this);
}

void ProjectImpl::setProjectURL(const QString& newURL) {
    if (url == newURL) {
        return;
    }

    coreLog.details(tr("Project URL is set to %1\n").arg(newURL));

    setModified(true);
    QString oldURL = url;
    url = newURL;
    emit si_projectURLChanged(oldURL);
}

Document* ProjectImpl::findDocumentByURL(const QString& url) const {
    foreach (Document* d, docs) {
        SAFE_POINT(d != nullptr, "Project contains NULL document", nullptr);
        if (d->getURLString() == url) {
            return d;
        }
    }
    return nullptr;
}

void ProjectImpl::addDocument(Document* d) {
    SAFE_POINT(d != nullptr, "NULL document", );
    coreLog.details(tr("Adding document to the project: %1").arg(d->getURLString()));

    SAFE_POINT(findDocumentByURL(d->getURL()) == nullptr, QString("Project already has file opened %1").arg(d->getURLString()), );
    setParentStateLockItem_static(d, this);

    d->setGHints(new ModTrackHints(this, d->getGHintsMap(), true));

    docs.push_back(d);

    emit si_documentAdded(d);
    setModified(true);

    connect(d, SIGNAL(si_objectAdded(GObject*)), SLOT(sl_onObjectAdded(GObject*)));
    connect(d, SIGNAL(si_objectRemoved(GObject*)), SLOT(sl_onObjectRemoved(GObject*)));
    connect(d, &Document::si_beforeObjectRemoved, this, &ProjectImpl::sl_removeAllrelationsWithObject);
    foreach (GObject* obj, d->getObjects()) {
        sl_onObjectAdded(obj);
    }
}

bool ProjectImpl::lockResources(int sizeMB, const QString& url, QString& error) {
    Document* doc = findDocumentByURL(url);
    SAFE_POINT_EXT(doc != nullptr, error = "Find document failed during resource locking", false);

    if (resourceTracker->tryAcquire(sizeMB)) {
        resourceUsage[doc->getName()] = sizeMB;
        return true;
    } else {
        error = tr("Not enough resources for load document, resource: '%1' available: %2%3 requested: %4%3")
                    .arg(resourceTracker->id)
                    .arg(resourceTracker->available())
                    .arg(resourceTracker->units)
                    .arg(sizeMB);
        return false;
    }
}

void ProjectImpl::removeDocument(Document* d, bool autodelete) {
    SAFE_POINT(d != nullptr, "No document provided for removeDocument", );
    coreLog.details(tr("Removing document from the project: %1").arg(d->getURLString()));

    setParentStateLockItem_static(d, nullptr);
    if (docs.contains(d)) {
        docs.removeOne(d);
        d->disconnect(this);
        d->setGHints(new GHintsDefaultImpl(d->getGHints()->getMap()));
        emit si_documentRemoved(d);
    }

    if (autodelete) {
        if (resourceUsage.contains(d->getName())) {
            resourceTracker->release(resourceUsage[d->getName()]);
            resourceUsage.remove(d->getName());
        }
        d->deleteLater();
    }
    setModified(true);
}

void ProjectImpl::sl_onStateModified(GObjectViewState*) {
    setModified(true);
}

void ProjectImpl::sl_onMdiWindowAdded(MWMDIWindow* w) {
    auto vw = qobject_cast<GObjectViewWindow*>(w);
    if (vw != nullptr) {
        connect(vw->getObjectView(), SIGNAL(si_nameChanged(const QString&)), SLOT(sl_onViewRenamed(const QString&)));
    }
}

void ProjectImpl::sl_onMdiWindowClosing(MWMDIWindow* w) {
    auto vw = qobject_cast<GObjectViewWindow*>(w);
    if (vw != nullptr) {
        vw->getObjectView()->disconnect(this);
    }
}

void ProjectImpl::addState(GObjectViewState* s) {
    assert(!objectViewStates.contains(s));
    connect(s, SIGNAL(si_stateModified(GObjectViewState*)), SLOT(sl_onStateModified(GObjectViewState*)));
    objectViewStates.append(s);
    setModified(true);
}

void ProjectImpl::addGObjectViewState(GObjectViewState* s) {
    GObjectViewState* existingState = GObjectViewUtils::findStateInList(s->getViewName(), s->getStateName(), objectViewStates);
    SAFE_POINT(existingState == nullptr, "State with the same name already exists: " + s->getViewName() + ":" + s->getStateName(), );
    addState(s);
    emit si_objectViewStateAdded(s);
}

void ProjectImpl::updateGObjectViewState(const GObjectViewState& updatedState) {
    const QString& viewName = updatedState.getViewName();
    const QString& stateName = updatedState.getStateName();
    const QVariantMap& stateData = updatedState.getStateData();

    GObjectViewState* state = GObjectViewUtils::findStateInList(viewName, stateName, objectViewStates);
    SAFE_POINT(state != nullptr, "State not found: " + viewName + ":" + stateName, );
    SAFE_POINT(state->getViewFactoryId() == updatedState.getViewFactoryId(),
               "State factory does not match: " + state->getViewFactoryId() + " vs " + updatedState.getViewFactoryId(), );

    CHECK(stateData != state->getStateData(), );
    state->setStateData(stateData);

    emit si_objectViewStateChanged(state);
    setModified(true);
}

void ProjectImpl::removeGObjectViewState(GObjectViewState* s) {
    int i = objectViewStates.removeAll(s);
    Q_UNUSED(i);
    assert(i == 1);
    emit si_objectViewStateRemoved(s);
    setModified(true);
}

void ProjectImpl::sl_onObjectAdded(GObject* obj) {
    connect(obj, SIGNAL(si_nameChanged(const QString&)), SLOT(sl_onObjectRenamed(const QString&)));
    connect(obj, SIGNAL(si_relationChanged(const QList<GObjectRelation>&)), SLOT(sl_onObjectRelationChanged(const QList<GObjectRelation>&)));
    if (!obj->getGHints()->get(GObjectHint_InProjectId).isValid()) {
        obj->getGHints()->set(GObjectHint_InProjectId, genNextObjectId());
    }
}

void ProjectImpl::sl_onObjectRemoved(GObject* obj) {
    obj->disconnect(this);
    obj->getGHints()->remove(GObjectHint_InProjectId);
}

void ProjectImpl::sl_onObjectRelationChanged(const QList<GObjectRelation>& previousRelations) {
    auto senderObject = qobject_cast<GObject*>(sender());
    CHECK(senderObject != nullptr, )
    QSet<GObjectRelation> relationsSet = toSet(senderObject->getObjectRelations());
    relationsSet.unite(toSet(previousRelations));
    QList<GObject*> allObjs;
    foreach (Document* d, getDocuments()) {
        allObjs << d->getObjects();
    }
    for (GObject* obj : qAsConst(allObjs)) {
        foreach (const GObjectRelation& rel, relationsSet) {
            if (obj->getEntityRef() == rel.ref.entityRef) {
                obj->relatedObjectRelationChanged();
            }
        }
    }
}

QString ProjectImpl::genNextObjectId() {
    idGen++;
    return "Object:" + QString::number(idGen);
}

void ProjectImpl::sl_onViewRenamed(const QString& oldName) {
    auto view = qobject_cast<GObjectViewController*>(sender());
    updateGObjectViewStates(oldName, view->getName());
}

void ProjectImpl::updateGObjectViewStates(const QString& oldViewName, const QString& newViewName) {
    foreach (GObjectViewState* state, objectViewStates) {
        if (state->getViewName() == oldViewName) {
            state->setViewName(newViewName);
            setModified(true);
        }
    }
}

void ProjectImpl::sl_onObjectRenamed(const QString& oldName) {
    auto obj = qobject_cast<GObject*>(sender());
    GObjectReference from(obj->getDocument()->getURLString(), oldName, obj->getGObjectType());
    GObjectReference to(obj);
    updateObjectRelations(from, to);
    foreach (GObjectViewState* state, objectViewStates) {
        QVariantMap data = state->getStateData();
        if (updateReferenceFields(state->getStateName(), data, from, to) > 0) {
            state->setStateData(data);
        }
    }
}

int ProjectImpl::updateReferenceFields(const QString& stateName, QVariantMap& map, const GObjectReference& from, const GObjectReference& to) {
    static QString refType = "U2::GObjectReference";
    static QString refListType = "QList<U2::GObjectReference>";
    int n = 0;
    foreach (const QString& key, map.keys()) {
        const QVariant& v = map[key];
        QString typeName = v.typeName();
        if (typeName == refType) {
            GObjectReference ref = v.value<GObjectReference>();
            if (ref == from) {
                map[key] = QVariant::fromValue<GObjectReference>(to);
                coreLog.trace(QString("Renaming reference in state '%1': %2 -> %3").arg(stateName).arg(from.objName).arg(to.objName));
                n++;
            }
        } else if (typeName == refListType) {
            int nBefore = n;
            QList<GObjectReference> refList = v.value<QList<GObjectReference>>();
            QList<GObjectReference> newRefList;
            for (const GObjectReference& ref : qAsConst(refList)) {
                if (ref == from) {
                    coreLog.trace(QString("Renaming reference in [] state '%1': %2 -> %3").arg(stateName).arg(from.objName).arg(to.objName));
                    newRefList << to;
                    n++;
                } else {
                    newRefList << ref;
                }
            }
            if (nBefore < n) {
                map[key] = QVariant::fromValue<QList<GObjectReference>>(newRefList);
            }
        }
    }
    return n;
}

void ProjectImpl::updateObjectRelations(const GObjectReference& oldRef, const GObjectReference& newRef) {
    QList<GObject*> allObjs;
    foreach (Document* d, getDocuments()) {
        allObjs << d->getObjects();
    }

    GObjectRelationRole objRole;
    if (newRef.objType == GObjectTypes::SEQUENCE) {
        objRole = ObjectRole_Sequence;
    } else if (newRef.objType == GObjectTypes::ANNOTATION_TABLE) {
        objRole = ObjectRole_AnnotationTable;
    } else if (newRef.objType == GObjectTypes::PHYLOGENETIC_TREE) {
        objRole = ObjectRole_PhylogeneticTree;
    } else {
        return;  // other object types cannot be referenced
    }
    const QList<GObject*> dependentObjs = GObjectUtils::findObjectsRelatedToObjectByRole(oldRef, "", objRole, allObjs, UOF_LoadedAndUnloaded);

    foreach (GObject* obj, dependentObjs) {
        obj->updateRefInRelations(oldRef, newRef);
    }
}
void ProjectImpl::removeRelations(const QString& removedDocUrl) {
    foreach (Document* d, getDocuments()) {
        QList<GObject*> objects = d->getObjects();
        for (GObject* obj : qAsConst(objects)) {
            obj->removeRelations(removedDocUrl);
        }
    }
}

void ProjectImpl::updateDocInRelations(const QString& oldDocUrl, const QString& newDocUrl) {
    QList<Document*> documents = getDocuments();
    for (Document* d : qAsConst(documents)) {
        foreach (GObject* obj, d->getObjects()) {
            obj->updateDocInRelations(oldDocUrl, newDocUrl);
        }
    }
}

}  // namespace U2
