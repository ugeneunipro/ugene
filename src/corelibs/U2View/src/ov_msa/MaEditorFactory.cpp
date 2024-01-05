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

#include "MaEditorFactory.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MsaObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SelectionUtils.h>

#include "MSAEditor.h"
#include "MaEditorState.h"
#include "MaEditorTasks.h"
#include "align_to_alignment/AlignSequencesToAlignmentSupport.h"
#include "exclude_list/MsaExcludeList.h"
#include "ov_mca/McaEditor.h"

namespace U2 {

const GObjectViewFactoryId MsaEditorFactory::ID("MSAEditor");
const GObjectViewFactoryId McaEditorFactory::ID("MCAEditor");

/************************************************************************/
/* MaEditorFactory */
/************************************************************************/
MaEditorFactory::MaEditorFactory(GObjectType type, GObjectViewFactoryId id)
    : GObjectViewFactory(id, tr("Alignment Editor")),
      type(type) {
    if (id == MsaEditorFactory::ID) {
        name = tr("Multiple Alignment Editor");
    } else if (id == McaEditorFactory::ID) {
        name = tr("Sanger Reads Editor");
    } else {
        FAIL("Unknown factory id", );
    }
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
                                                                           &multiSelection,
                                                                           UOF_LoadedAndUnloaded,
                                                                           false);
    QList<OpenMaEditorTask*> resTasks;

    for (Document* doc : qAsConst(docsWithMSA)) {
        QList<GObject*> docObjs = doc->findGObjectByType(type, UOF_LoadedAndUnloaded);
        if (!docObjs.isEmpty()) {
            for (GObject* obj : qAsConst(docObjs)) {
                if (!msaObjects.contains(obj)) {
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
        foreach (GObject* o, msaObjects) {
            if (resTasks.size() == MAX_VIEWS) {
                break;
            }
            if (o->getGObjectType() == GObjectTypes::UNLOADED) {
                resTasks.append(getOpenMaEditorTask(qobject_cast<UnloadedObject*>(o)));
            } else {
                assert(o->getGObjectType() == type);
                resTasks.append(getOpenMaEditorTask(qobject_cast<MsaObject*>(o)));
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

bool MaEditorFactory::isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
    MaEditorState state(stateData);
    if (!state.isValid()) {
        return false;
    }
    GObjectReference ref = state.getMaObjectRef();
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
    : MaEditorFactory(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, ID) {
}

MaEditor* MsaEditorFactory::getEditor(const QString& viewName, GObject* obj, U2OpStatus& os) {
    auto msaObj = qobject_cast<MsaObject*>(obj);
    SAFE_POINT(msaObj != nullptr, "Invalid GObject", nullptr);
    if (msaObj->getLength() > MSAEditor::MAX_SUPPORTED_MSA_OBJECT_LENGTH) {
        os.setError(tr("MSA object is too large to be opened in MSA Editor!"));
        return nullptr;
    }
    return new MSAEditor(viewName, msaObj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(MsaObject* obj) {
    return new OpenMsaEditorTask(obj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(UnloadedObject* obj) {
    return new OpenMsaEditorTask(obj);
}

OpenMaEditorTask* MsaEditorFactory::getOpenMaEditorTask(Document* doc) {
    return new OpenMsaEditorTask(doc);
}

void MsaEditorFactory::registerMsaEditorViewFeatures() {
    (new AlignSequencesToAlignmentSupport(this))->init();
    (new MsaExcludeListContext(this))->init();
}

/************************************************************************/
/* McaEditorFactory */
/************************************************************************/
McaEditorFactory::McaEditorFactory()
    : MaEditorFactory(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT, ID) {
}

MaEditor* McaEditorFactory::getEditor(const QString& viewName, GObject* obj, U2OpStatus&) {
    auto mcaObj = qobject_cast<MsaObject*>(obj);
    SAFE_POINT(mcaObj != nullptr, "Invalid GObject", nullptr);
    return new McaEditor(viewName, mcaObj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(MsaObject* obj) {
    return new OpenMcaEditorTask(obj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(UnloadedObject* obj) {
    return new OpenMcaEditorTask(obj);
}

OpenMaEditorTask* McaEditorFactory::getOpenMaEditorTask(Document* doc) {
    return new OpenMcaEditorTask(doc);
}

}  // namespace U2
