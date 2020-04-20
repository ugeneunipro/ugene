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

#include "TreeViewerFactory.h"
#include "TreeViewer.h"
#include "TreeViewerTasks.h"
#include "TreeViewerState.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/UnloadedObject.h>

#include <U2Core/SelectionUtils.h>

namespace U2 {

/* TRANSLATOR U2::TreeViewer */
/* TRANSLATOR U2::ObjectViewTask */

const GObjectViewFactoryId TreeViewerFactory::ID("PhylTreeViewer");

TreeViewerFactory::TreeViewerFactory(): GObjectViewFactory(ID, tr("Phylogenetic tree viewer")) {}

bool TreeViewerFactory::canCreateView(const MultiGSelection& multiSelection) {
    bool hasPhyDocuments = !SelectionUtils::findDocumentsWithObjects(
                                GObjectTypes::PHYLOGENETIC_TREE, &multiSelection, UOF_LoadedAndUnloaded, true).isEmpty();
    if (hasPhyDocuments) {
        return true;
    }
    return false;
}

#define MAX_VIEWS 10

Task* TreeViewerFactory::createViewTask(const MultiGSelection& multiSelection, bool single) {
    QList<GObject*> phyObjects = SelectionUtils::findObjects(GObjectTypes::PHYLOGENETIC_TREE, &multiSelection, UOF_LoadedAndUnloaded);
    QSet<Document*> docsWithPhy = SelectionUtils::findDocumentsWithObjects(GObjectTypes::PHYLOGENETIC_TREE,
                                                            &multiSelection, UOF_LoadedAndUnloaded, false);
    QList<OpenTreeViewerTask*> resTasks;

    foreach(Document* doc, docsWithPhy) {
        QList<GObject*> docObjs = doc->findGObjectByType(GObjectTypes::PHYLOGENETIC_TREE, UOF_LoadedAndUnloaded);
        if (!docObjs.isEmpty()) {
            foreach(GObject* obj, docObjs){
                if(!phyObjects.contains(obj)){
                    phyObjects.append(obj);
                }
            }
        } else {
            resTasks.append(new OpenTreeViewerTask(doc));
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
        }
    }

    if (!phyObjects.isEmpty()) {
        foreach(GObject* o, phyObjects) {
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
            if (o->getGObjectType() == GObjectTypes::UNLOADED) {
                resTasks.append(new OpenTreeViewerTask(qobject_cast<UnloadedObject*>(o)));
            } else {
                assert(o->getGObjectType() == GObjectTypes::PHYLOGENETIC_TREE);
                resTasks.append(new OpenTreeViewerTask(qobject_cast<PhyTreeObject*>(o)));
            }
        }
    }

    if (resTasks.isEmpty()) {
        return NULL;
    }

    if (resTasks.size() == 1 || single) {
        return resTasks.first();
    }

    Task* result = new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach(Task* t, resTasks) {
        result->addSubTask(t);
    }
    return result;
}

bool TreeViewerFactory::isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
    TreeViewerState state(stateData);
    if (!state.isValid()) {
        return false;
    }
    GObjectReference ref = state.getPhyObject();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == NULL) { //todo: accept to use invalid state removal routines of ObjectViewTask ???
        return false;
    }
    //check that document is in selection
    QList<Document*> selectedDocs = SelectionUtils::getSelectedDocs(multiSelection);
    if (selectedDocs.contains(doc)) {
        return true;
    }
    //check that object is in selection
    QList<GObject*> selectedObjects = SelectionUtils::getSelectedObjects(multiSelection);
    GObject* obj = doc->findGObjectByName(ref.objName);
    bool res = obj != NULL && selectedObjects.contains(obj);
    return res;
}

Task* TreeViewerFactory::createViewTask(const QString& viewName, const QVariantMap& stateData) {
    return new OpenSavedTreeViewerTask(viewName, stateData);
}


}//namespace
