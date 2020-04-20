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

#include "SimpleTextObjectViewTasks.h"
#include "SimpleTextObjectView.h"

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//open view task

OpenSimpleTextObjectViewTask::OpenSimpleTextObjectViewTask(const QList<GObject *> &_objects)
    : ObjectViewTask(SimpleTextObjectViewFactory::ID), objects(_objects)
{
    foreach (GObject *obj, objects) {
        CHECK_EXT(NULL != obj, stateInfo.setError(tr("Invalid object detected!")), );
        CHECK_EXT(GObjectTypes::TEXT == obj->getGObjectType(),
            stateInfo.setError(tr("Invalid object type detected!")), );

        Document *doc = obj->getDocument();
        CHECK_EXT(NULL != doc, stateInfo.setError(tr("Invalid document detected!")), );
        if (!documentsToLoad.contains(doc) && !doc->isLoaded()) {
            documentsToLoad.append(doc);
        }
    }
}

OpenSavedTextObjectViewTask::OpenSavedTextObjectViewTask(const QString& vname, const QVariantMap& stateData)
: ObjectViewTask(SimpleTextObjectViewFactory::ID, vname, stateData), doc(NULL)
{
    QString documentUrl = SimpleTextObjectView::getDocumentUrl(stateData);
    doc = AppContext::getProject()->findDocumentByURL(documentUrl);
    if (doc.isNull()) {
        doc = createDocumentAndAddToProject(documentUrl, AppContext::getProject(), stateInfo);
        if (!doc) {
            stateIsIllegal = true;
            stateInfo.setError(L10N::errorDocumentNotFound(documentUrl));
            return;
        }
    }
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    }

}

void OpenSavedTextObjectViewTask::open() {
    if (stateInfo.hasError() || doc.isNull()) {
        return;
    }
    assert(doc->isLoaded());
    QString objName = SimpleTextObjectView::getObjectName(stateData);
    GObject* obj = doc->findGObjectByName(objName);
    TextObject* to = qobject_cast<TextObject*>(obj);
    if (!to) {
        stateInfo.setError(tr("Text object '%1' is not found").arg(objName));
        stateIsIllegal = true;
        return;
    }
    SimpleTextObjectView* v = new SimpleTextObjectView(viewName, to, stateData);
    GObjectViewWindow* w = new GObjectViewWindow(v, viewName, !stateData.isEmpty());
    MWMDIManager* mdiManager =  AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);
}

void OpenSimpleTextObjectViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    foreach (GObject *obj, objects) {
        Document *doc = obj->getDocument();
        CHECK_EXT(doc->isLoaded(), stateInfo.setError(tr("Document is not loaded!")), );

        TextObject *to = qobject_cast<TextObject*>(obj);
        CHECK_EXT(NULL != obj, stateInfo.setError(tr("Invalid object detected!")), );

        const QString viewName = GObjectViewUtils::genUniqueViewName(doc, to);
        SimpleTextObjectView* v = new SimpleTextObjectView(viewName, to, stateData);
        GObjectViewWindow* w = new GObjectViewWindow(v, viewName, !stateData.isEmpty());
        if (NULL == v->parent()) {
            stateInfo.setError("Could not open view");
            delete v;
            delete w;
            continue;
        }
        MWMDIManager* mdiManager =  AppContext::getMainWindow()->getMDIManager();
        mdiManager->addMDIWindow(w);
    }
}


//////////////////////////////////////////////////////////////////////////
// update view task

UpdateSimpleTextObjectViewTask::UpdateSimpleTextObjectViewTask(GObjectView* v, const QString& stateName, const QVariantMap& stateData)
: ObjectViewTask(v, stateName, stateData)
{
}

void UpdateSimpleTextObjectViewTask::update() {
    if (view.isNull()) {
        return;
    }
    SimpleTextObjectView* tv = qobject_cast<SimpleTextObjectView*>(view);
    if (tv == NULL) {
        return;
    }
    tv->updateView(stateData);

}


} // namespace
