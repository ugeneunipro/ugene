/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "AlignSequencesToAlignmentSupport.h"

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/AppContext.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/Task.h>
#include <U2Core/TaskWatchdog.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "AlignSequencesToAlignmentTask.h"

namespace U2 {

AlignSequencesToAlignmentSupport::AlignSequencesToAlignmentSupport(QObject *parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID) {
}

void AlignSequencesToAlignmentSupport::initViewContext(GObjectView *view) {
    auto msaEditor = qobject_cast<MSAEditor *>(view);
    SAFE_POINT(msaEditor != nullptr, "View is not MSAEditor!", );
    CHECK(msaEditor->getMaObject() != nullptr, );

    AlignmentAlgorithmsRegistry *alignmentAlgorithmsRegistry = AppContext::getAlignmentAlgorithmsRegistry();
    QStringList addToAlignmentAlgorithmIds = alignmentAlgorithmsRegistry->getAvailableAlgorithmIds(AddToAlignment);
    for (const QString &algorithmId : qAsConst(addToAlignmentAlgorithmIds)) {
        AlignmentAlgorithm *algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
        auto alignAction = new AlignSequencesToAlignmentAction(this, msaEditor, algorithmId, algorithm->getActionName(), 100);
        alignAction->setIcon(QIcon(":/core/images/add_to_alignment.png"));    //TODO: add a dedicated icon per algorithm.
        alignAction->setObjectName(algorithmId);
        alignAction->setMenuTypes({MsaEditorMenuType::ALIGN_SEQUENCES_TO_ALIGNMENT});
        alignAction->sl_updateState();

        addViewAction(alignAction);
    }
}

void AlignSequencesToAlignmentAction::sl_activate() {
    auto msaObject = msaEditor->getMaObject();
    SAFE_POINT(!msaObject->isStateLocked(), "The action must never be called for a readonly object!", );

    ProjectView *projectView = AppContext::getProjectView();
    SAFE_POINT(projectView != nullptr, "Project view is null", );

    const GObjectSelection *selection = projectView->getGObjectSelection();
    SAFE_POINT(selection != nullptr, "GObjectSelection is null", );

    QList<GObject *> objects = selection->getSelectedObjects();
    bool selectFromProject = !objects.isEmpty();

    for (const GObject *object : qAsConst(objects)) {
        const GObjectType &objectType = object->getGObjectType();
        if (object == msaObject || (objectType != GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT && objectType != GObjectTypes::SEQUENCE)) {
            selectFromProject = false;
            break;
        }
    }

    // TODO: ask user if to select from the project or from files.
    if (selectFromProject) {
        SequenceObjectsExtractor extractor;
        extractor.setAlphabet(msaObject->getAlphabet());
        extractor.extractSequencesFromObjects(objects);

        if (!extractor.getSequenceRefs().isEmpty()) {
            auto task = new AlignSequencesToAlignmentTask(msaObject, algorithmId, extractor);
            TaskWatchdog::trackResourceExistence(msaObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
            AppContext::getTaskScheduler()->registerTopLevelTask(task);
        }
    } else {
        QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
        LastUsedDirHelper lod;
        QStringList urls = U2FileDialog::getOpenFileNames(nullptr, tr("Open file with sequences"), lod.dir, filter);

        if (!urls.isEmpty()) {
            lod.url = urls.first();
            auto task = new LoadSequencesAndAlignToAlignmentTask(msaObject, algorithmId, urls);
            TaskWatchdog::trackResourceExistence(msaObject, task, tr("A problem occurred during adding sequences. The multiple alignment is no more available."));
            AppContext::getTaskScheduler()->registerTopLevelTask(task);
        }
    }
}

AlignSequencesToAlignmentAction::AlignSequencesToAlignmentAction(QObject *parent, MSAEditor *view, const QString &_algorithmId, const QString &text, int order)
    : GObjectViewAction(parent, view, text, order), msaEditor(view), algorithmId(_algorithmId) {
    MultipleSequenceAlignmentObject *msaObject = msaEditor->getMaObject();
    connect(msaObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_updateState()));
    connect(msaObject, SIGNAL(si_alignmentChanged(const MultipleAlignment &, const MaModificationInfo &)), SLOT(sl_updateState()));
    connect(this, SIGNAL(triggered()), this, SLOT(sl_activate()));
}

MSAEditor *AlignSequencesToAlignmentAction::getEditor() const {
    return msaEditor;
}

void AlignSequencesToAlignmentAction::sl_updateState() {
    auto msaObject = msaEditor->getMaObject();
    if (msaObject == nullptr || msaObject->isStateLocked()) {
        setEnabled(false);
        return;
    }
    bool canBeUsedWithEmptyObject = algorithmId == BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_UGENE;
    if (msaObject->getMultipleAlignment()->isEmpty() && !canBeUsedWithEmptyObject) {
        setEnabled(false);
        return;
    }
    AlignmentAlgorithmsRegistry *alignmentAlgorithmsRegistry = AppContext::getAlignmentAlgorithmsRegistry();
    AlignmentAlgorithm *algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
    if (!algorithm->checkAlphabet(msaObject->getAlphabet())) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
}

}    // namespace U2
