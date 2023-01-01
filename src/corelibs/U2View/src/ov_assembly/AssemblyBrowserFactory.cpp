/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "AssemblyBrowserFactory.h"

#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AssemblyBrowserState.h"
#include "AssemblyBrowserTasks.h"

namespace U2 {

//==============================================================================
// AssemblyBrowserFactory
//==============================================================================

const GObjectViewFactoryId AssemblyBrowserFactory::ID = "assembly-browser-factory";

AssemblyBrowserFactory::AssemblyBrowserFactory(QObject* parent /* = 0 */)
    : GObjectViewFactory(ID, tr("Assembly Browser"), parent) {
}

bool AssemblyBrowserFactory::canCreateView(const MultiGSelection& multiSelection) {
    bool hasAssembly = !SelectionUtils::findDocumentsWithObjects(GObjectTypes::ASSEMBLY, &multiSelection, UOF_LoadedAndUnloaded, true).isEmpty();
    return hasAssembly;
}

#define MAX_VIEWS 5

void addAsmObjs(QList<GObject*>& asmObjs, const QList<GObject*>& docObjs) {
    foreach (GObject* go, docObjs) {
        if (!asmObjs.contains(go)) {
            asmObjs.append(go);
        }
    }
}

Task* AssemblyBrowserFactory::createViewTask(const MultiGSelection& multiSelection, bool single /* = false */) {
    QList<GObject*> asmObjects = SelectionUtils::findObjects(GObjectTypes::ASSEMBLY, &multiSelection, UOF_LoadedAndUnloaded);
    QSet<Document*> docsWithAsm = SelectionUtils::findDocumentsWithObjects(GObjectTypes::ASSEMBLY,
                                                                           &multiSelection,
                                                                           UOF_LoadedAndUnloaded,
                                                                           false);
    QList<OpenAssemblyBrowserTask*> resTasks;

    foreach (Document* doc, docsWithAsm) {
        QList<GObject*> docObjs = doc->findGObjectByType(GObjectTypes::ASSEMBLY, UOF_LoadedAndUnloaded);
        if (!docObjs.isEmpty()) {
            addAsmObjs(asmObjects, docObjs);
        } else {
            resTasks.append(new OpenAssemblyBrowserTask(doc));
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
        }
    }

    if (!asmObjects.isEmpty()) {
        // Move up to MAX_VIEWS assembly objects with reads to the start of the list.
        // So they will be selected to be opened with a higher priority than empty assemblies.

        // Select assembly objects with reads first.
        QList<GObject*> assemblyObjectsWithReads;
        for (GObject* o : qAsConst(asmObjects)) {
            if (auto assemblyObject = qobject_cast<AssemblyObject*>(o)) {
                U2OpStatusImpl os;
                if (assemblyObject->getReadCount(os) > 0) {
                    assemblyObjectsWithReads.append(assemblyObject);
                    if (assemblyObjectsWithReads.size() == MAX_VIEWS) {
                        break;
                    }
                }
            }
        }
        // Move them to the beginning of the list.
        if (!assemblyObjectsWithReads.isEmpty()) {
            for (GObject* o : qAsConst(assemblyObjectsWithReads)) {
                asmObjects.removeOne(o);
                asmObjects.prepend(o);
            }
        }

        for (GObject* o : qAsConst(asmObjects)) {
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
            if (o->getGObjectType() == GObjectTypes::UNLOADED) {
                resTasks.append(new OpenAssemblyBrowserTask(qobject_cast<UnloadedObject*>(o)));
            } else {
                SAFE_POINT(o->getGObjectType() == GObjectTypes::ASSEMBLY, "Invalid assembly object!", nullptr);
                resTasks.append(new OpenAssemblyBrowserTask(qobject_cast<AssemblyObject*>(o)));
            }
        }
    }

    if (resTasks.isEmpty()) {
        return nullptr;
    }

    if (resTasks.size() == 1 || single) {
        return resTasks.first();
    }

    Task* result = new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach (Task* t, resTasks) {
        result->addSubTask(t);
    }
    return result;
}

bool AssemblyBrowserFactory::isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
    // TODO: this method of AssemblyBrowser, AnnotatedDNAView and MSAEditor is copypaste a little more than entirely
    AssemblyBrowserState state(stateData);
    if (!state.isValid()) {
        return false;
    }
    GObjectReference ref = state.getGObjectRef();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {  // todo: accept to use invalid state removal routines of ObjectViewTask ???
        return false;
    }
    // check that document is in selection
    QList<Document*> selectedDocs = SelectionUtils::getSelectedDocs(multiSelection);
    if (selectedDocs.contains(doc)) {
        return true;
    }
    // check that object is in selection
    QList<GObject*> selectedObjects = SelectionUtils::getSelectedObjects(multiSelection);
    GObject* obj = doc->findGObjectByName(ref.objName);
    bool res = obj != nullptr && selectedObjects.contains(obj);
    return res;
}

Task* AssemblyBrowserFactory::createViewTask(const QString& viewName, const QVariantMap& stateData) {
    return new OpenSavedAssemblyBrowserTask(viewName, stateData);
}

}  // namespace U2
