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

#include "MaEditorFactory.h"

#include "MaEditorState.h"
#include "MaEditorTasks.h"
#include "McaEditor.h"
#include "MSAEditor.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>

namespace U2 {

const GObjectViewFactoryId MsaEditorFactory::ID("MSAEditor");
const GObjectViewFactoryId McaEditorFactory::ID("MCAEditor");


/************************************************************************/
/* MaEditorFactory */
/************************************************************************/
MaEditorFactory::MaEditorFactory(GObjectType type, GObjectViewFactoryId id)
    : GObjectViewFactory(id, tr("Alignment Editor")),
      type(type)
{
}

bool MaEditorFactory::canCreateView(const MultiGSelection& multiSelection) {
    bool hasMaDocuments = !SelectionUtils::findDocumentsWithObjects(type, &multiSelection, UOF_LoadedAndUnloaded, true).isEmpty();
    if (hasMaDocuments) {
        return true;
    }
    return false;
}

#define MAX_VIEWS 10

Task* MaEditorFactory::createViewTask(const MultiGSelection& multiSelection, bool single) {
    QList<GObject*> msaObjects = SelectionUtils::findObjects(type, &multiSelection, UOF_LoadedAndUnloaded);
    QSet<Document*> docsWithMSA = SelectionUtils::findDocumentsWithObjects(type,
        &multiSelection, UOF_LoadedAndUnloaded, false);
    QList<OpenMaEditorTask*> resTasks;

    foreach(Document* doc, docsWithMSA) {
        QList<GObject*> docObjs = doc->findGObjectByType(type, UOF_LoadedAndUnloaded);
        if (!docObjs.isEmpty()) {
            foreach(GObject* obj, docObjs){
                if(!msaObjects.contains(obj)){
                    msaObjects.append(obj);
                }
            }

        } else {
            resTasks.append(getOpenMaEditorTask(doc));
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
        }
    }

    if (!msaObjects.isEmpty()) {
        foreach(GObject* o, msaObjects) {
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
            if (o->getGObjectType() == GObjectTypes::UNLOADED) {
                resTasks.append(getOpenMaEditorTask(qobject_cast<UnloadedObject*>(o)));
            } else {
                assert(o->getGObjectType() == type);
                resTasks.append(getOpenMaEditorTask(qobject_cast<MultipleAlignmentObject*>(o)));
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

bool MaEditorFactory::isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
    MaEditorState state(stateData);
    if (!state.isValid()) {
        return false;
    }
    GObjectReference ref = state.getMaObjectRef();
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
    bool res = obj!=NULL && selectedObjects.contains(obj);
    return res;
}

Task* MaEditorFactory::createViewTask(const QString& viewName, const QVariantMap& stateData) {
    return new OpenSavedMaEditorTask(type, this, viewName, stateData);
}

bool MaEditorFactory::supportsSavedStates() const {
    return true;
}

/************************************************************************/
/* MsaEditorFactory */
/************************************************************************/
MsaEditorFactory::MsaEditorFactory()
    : MaEditorFactory(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, ID)
{
}

MaEditor* MsaEditorFactory::getEditor(const QString& viewName, GObject* obj) {
    MultipleSequenceAlignmentObject* msaObj = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    SAFE_POINT(msaObj != NULL, "Invalid GObject", NULL);
    return new MSAEditor(viewName, msaObj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(MultipleAlignmentObject* obj) {
    return new OpenMsaEditorTask(obj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(UnloadedObject* obj) {
    return new OpenMsaEditorTask(obj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(Document* doc) {
    return new OpenMsaEditorTask(doc);
}

/************************************************************************/
/* McaEditorFactory */
/************************************************************************/
McaEditorFactory::McaEditorFactory()
    : MaEditorFactory(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT, ID)
{
}

MaEditor* McaEditorFactory::getEditor(const QString& viewName, GObject* obj) {
    MultipleChromatogramAlignmentObject* mcaObj = qobject_cast<MultipleChromatogramAlignmentObject*>(obj);
    SAFE_POINT(mcaObj != NULL, "Invalid GObject", NULL);
    return new McaEditor(viewName, mcaObj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(MultipleAlignmentObject* obj) {
    return new OpenMcaEditorTask(obj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(UnloadedObject* obj) {
    return new OpenMcaEditorTask(obj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(Document* doc) {
    return new OpenMcaEditorTask(doc);
}

} // namespace
