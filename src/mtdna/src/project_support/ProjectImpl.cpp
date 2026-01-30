/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <U2Gui/ObjectViewModel.h>

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
}

const QString& ProjectImpl::getProjectName() const {
    return name;
}

void ProjectImpl::makeClean() {
}

quint64 ProjectImpl::getObjectIdCounter() const {
    return idGen;
}

void ProjectImpl::setObjectIdCounter(quint64 c) {
    idGen = c;
}

void ProjectImpl::sl_removeAllrelationsWithObject(GObject* obj) {
}

void ProjectImpl::setProjectName(const QString& newName) {
    if (name == newName) {
        return;
    }
    setModified(true);
    name = newName;
    emit si_projectRenamed(this);
}

const QString& ProjectImpl::getProjectURL() const {
    return url;
}

void ProjectImpl::setProjectURL(const QString& newURL) {
    if (url == newURL) {
        return;
    }

    setModified(true);
    QString oldURL = url;
    url = newURL;
    emit si_projectURLChanged(oldURL);
}

const QList<Document*>& ProjectImpl::getDocuments() const {
    return docs;
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

Document* ProjectImpl::findDocumentByURL(const GUrl& url) const {
    return findDocumentByURL(url.getURLString());
}

const QList<GObjectViewState*>& ProjectImpl::getGObjectViewStates() const {
    return {};
}

void ProjectImpl::addDocument(Document* d) {
    SAFE_POINT(d != nullptr, "NULL document", );
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

bool ProjectImpl::lockResources(int, const QString&, QString&) {
    return false;
}

void ProjectImpl::removeDocument(Document*, bool) {
}

void ProjectImpl::sl_onStateModified(GObjectViewState*) {
}

void ProjectImpl::sl_onMdiWindowAdded(MWMDIWindow*) {
}

void ProjectImpl::sl_onMdiWindowClosing(MWMDIWindow* w) {
    auto vw = qobject_cast<GObjectViewWindow*>(w);
    if (vw != nullptr) {
        vw->getObjectView()->disconnect(this);
    }
}

void ProjectImpl::addGObjectViewState(GObjectViewState*) {
}

void ProjectImpl::updateGObjectViewState(const GObjectViewState& ) {
}

void ProjectImpl::removeGObjectViewState(GObjectViewState*) {
}

void ProjectImpl::sl_onObjectAdded(GObject* obj) {
    connect(obj, SIGNAL(si_nameChanged(const QString&)), SLOT(sl_onObjectRenamed(const QString&)));
    connect(obj, SIGNAL(si_relationChanged(const QList<GObjectRelation>&)), SLOT(sl_onObjectRelationChanged(const QList<GObjectRelation>&)));
    if (!obj->getGHints()->get(GObjectHint_InProjectId).isValid()) {
        obj->getGHints()->set(GObjectHint_InProjectId, genNextObjectId());
    }
}

void ProjectImpl::sl_onObjectRemoved(GObject*) {
}

void ProjectImpl::sl_onObjectRelationChanged(const QList<GObjectRelation>& ) {
}

QString ProjectImpl::genNextObjectId() {
    idGen++;
    return "Object:" + QString::number(idGen);
}

void ProjectImpl::sl_onViewRenamed(const QString& ) {
}

void ProjectImpl::sl_onObjectRenamed(const QString& ) {
}

void ProjectImpl::removeRelations(const QString&) {
}

void ProjectImpl::updateDocInRelations(const QString& , const QString& ) {
}

}  // namespace U2
