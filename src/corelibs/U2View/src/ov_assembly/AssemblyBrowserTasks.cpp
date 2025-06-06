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

#include "AssemblyBrowserTasks.h"

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/Notification.h>

#include "AssemblyBrowser.h"
#include "AssemblyBrowserFactory.h"
#include "AssemblyBrowserState.h"

namespace U2 {

//==============================================================================
// OpenAssemblyBrowserTask - open new view
//==============================================================================

OpenAssemblyBrowserTask::OpenAssemblyBrowserTask(AssemblyObject* obj)
    : ObjectViewTask(AssemblyBrowserFactory::ID) {
    selectedObjects.append(obj);
}

OpenAssemblyBrowserTask::OpenAssemblyBrowserTask(UnloadedObject* unloadedObj)
    : ObjectViewTask(AssemblyBrowserFactory::ID),
      unloadedObjRef(unloadedObj) {
    documentsToLoad.append(unloadedObj->getDocument());
}

OpenAssemblyBrowserTask::OpenAssemblyBrowserTask(Document* doc)
    : ObjectViewTask(AssemblyBrowserFactory::ID) {
    assert(!doc->isLoaded());
    documentsToLoad.append(doc);
}

void OpenAssemblyBrowserTask::open() {
    if (stateInfo.hasError() || (documentsToLoad.isEmpty() && selectedObjects.isEmpty())) {
        return;
    }

    if (selectedObjects.isEmpty()) {
        assert(documentsToLoad.size() == 1);
        Document* doc = documentsToLoad.first();
        QList<GObject*> objects;
        if (unloadedObjRef.isValid()) {
            // To do: replace the object finding to "GObject* obj = doc->findGObjectByName(unloadedObjRef.objName);" after fixing of UGENE-4904
            QList<GObject*> objs = doc->findGObjectByType(unloadedObjRef.objType);
            GObject* obj = nullptr;
            foreach (GObject* curObj, objs) {
                if (curObj->getGObjectName() == unloadedObjRef.objName) {
                    obj = curObj;
                    break;
                }
            }
            if (obj != nullptr && obj->getGObjectType() == GObjectTypes::ASSEMBLY) {
                selectedObjects.append(qobject_cast<AssemblyObject*>(obj));
            }
        } else {
            QList<GObject*> assemblyObjects = doc->findGObjectByType(GObjectTypes::ASSEMBLY, UOF_LoadedAndUnloaded);
            if (!assemblyObjects.isEmpty()) {
                selectedObjects.append(qobject_cast<AssemblyObject*>(assemblyObjects.first()));
            }
        }
        if (selectedObjects.isEmpty()) {
            stateInfo.setError(tr("Assembly object not found"));
            return;
        }
    }

    foreach (QPointer<GObject> po, selectedObjects) {
        auto o = qobject_cast<AssemblyObject*>(po);

        SAFE_POINT(o, "Invalid assembly object!", );

        viewName = GObjectViewUtils::genUniqueViewName(o->getDocument(), o);
        openBrowserForObject(o, viewName, false);
    }
}

void OpenAssemblyBrowserTask::updateTitle(AssemblyBrowser* ab) {
    const QString& oldViewName = ab->getName();
    GObjectViewWindow* w = GObjectViewUtils::findViewByName(oldViewName);
    if (w != nullptr) {
        AssemblyObject* aObj = ab->getAssemblyObject();
        QString newViewName = GObjectViewUtils::genUniqueViewName(aObj->getDocument(), aObj);
        ab->setName(newViewName);
        w->setWindowTitle(newViewName);
    }
}

AssemblyBrowser* OpenAssemblyBrowserTask::openBrowserForObject(AssemblyObject* obj, const QString& viewName, bool persistent) {
    auto v = new AssemblyBrowser(viewName, obj);
    U2OpStatus2Notification os;
    if (!v->checkValid(os)) {
        delete v;
        return nullptr;
    }
    auto w = new GObjectViewWindow(v, viewName, persistent);
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(w);
    return v;
}

//==============================================================================
// OpenSavedAssemblyBrowserTask - restore a new view from saved state
//==============================================================================

OpenSavedAssemblyBrowserTask::OpenSavedAssemblyBrowserTask(const QString& viewName, const QVariantMap& stateData)
    : ObjectViewTask(AssemblyBrowserFactory::ID, viewName, stateData) {
    AssemblyBrowserState state(stateData);
    GObjectReference ref = state.getGObjectRef();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        doc = createDocumentAndAddToProject(ref.docUrl, AppContext::getProject(), stateInfo);
        CHECK_OP_EXT(stateInfo, stateIsIllegal = true, );
    }
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    }
}

void OpenSavedAssemblyBrowserTask::open() {
    CHECK_OP(stateInfo, );

    AssemblyBrowserState state(stateData);
    GObjectReference ref = state.getGObjectRef();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        stateIsIllegal = true;
        stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
        return;
    }
    GObject* obj = nullptr;
    // To do: replace the object finding to "GObject* obj = doc->findGObjectByName(unloadedObjRef.objName);" after fixing of UGENE-4904
    QList<GObject*> objs = doc->findGObjectByType(ref.objType);
    foreach (GObject* curObj, objs) {
        if (curObj->getGObjectName() == ref.objName) {
            obj = curObj;
            break;
        }
    }

    if (obj == nullptr || obj->getGObjectType() != GObjectTypes::ASSEMBLY) {
        stateIsIllegal = true;
        stateInfo.setError(tr("Assembly object not found: %1").arg(ref.objName));
        return;
    }
    auto asmObj = qobject_cast<AssemblyObject*>(obj);
    SAFE_POINT(asmObj != nullptr, "Object has type ASSEMBLY, but cannot cast to AssemblyObject", );

    AssemblyBrowser* ab = OpenAssemblyBrowserTask::openBrowserForObject(asmObj, viewName, true);
    CHECK(ab != nullptr, );
    state.restoreState(ab);
}

//==============================================================================
// UpdateAssemblyBrowserTask - restore saved state on current view
//==============================================================================

void UpdateAssemblyBrowserTask::update() {
    if (view.isNull() || view->getFactoryId() != AssemblyBrowserFactory::ID) {
        return;  // view was closed;
    }

    auto ab = qobject_cast<AssemblyBrowser*>(view.data());
    SAFE_POINT(ab != nullptr, "UpdateAssemblyBrowserTask::update: view is not AssemblyBrowser", );

    AssemblyBrowserState(stateData).restoreState(ab);
}

}  // namespace U2
