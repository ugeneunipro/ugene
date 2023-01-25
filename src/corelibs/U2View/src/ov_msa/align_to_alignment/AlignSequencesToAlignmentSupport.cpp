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

#include "AlignSequencesToAlignmentSupport.h"

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/AppContext.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/TaskWatchdog.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaCollapseModel.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/MaEditorSelection.h>

#include "AlignSequencesToAlignmentTask.h"
#include "RealignSequencesInAlignmentTask.h"

namespace U2 {

AlignSequencesToAlignmentSupport::AlignSequencesToAlignmentSupport(QObject* parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID) {
}

void AlignSequencesToAlignmentSupport::initViewContext(GObjectView* view) {
    auto msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "View is not MSAEditor!", );
    CHECK(msaEditor->getMaObject() != nullptr, );
    msaEditor->registerActionProvider(this);

    AlignmentAlgorithmsRegistry* alignmentAlgorithmsRegistry = AppContext::getAlignmentAlgorithmsRegistry();

    QStringList alignNewSequencesAlgorithmIds = alignmentAlgorithmsRegistry->getAvailableAlgorithmIds(AlignNewSequencesToAlignment);
    for (auto& algorithmId : qAsConst(alignNewSequencesAlgorithmIds)) {
        AlignmentAlgorithm* algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
        auto alignAction = new AlignSequencesToAlignmentAction(this, msaEditor, algorithmId, algorithm->getActionName(), 3000);
        alignAction->setIcon(QIcon(":/core/images/add_to_alignment.png"));  // TODO: add a dedicated icon per algorithm.
        alignAction->setObjectName(algorithmId);
        alignAction->setMenuTypes({MsaEditorMenuType::ALIGN_NEW_SEQUENCES_TO_ALIGNMENT});
        addViewAction(alignAction);
    }

    QStringList alignNewAlignmentAlgorithmIds = alignmentAlgorithmsRegistry->getAvailableAlgorithmIds(AlignNewAlignmentToAlignment);
    for (auto& algorithmId : qAsConst(alignNewAlignmentAlgorithmIds)) {
        AlignmentAlgorithm* algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
        auto alignAction = new AlignSequencesToAlignmentAction(this, msaEditor, algorithmId, algorithm->getActionName(), 3000);
        alignAction->setIcon(QIcon(":/core/images/add_to_alignment.png"));  // TODO: add a dedicated icon per algorithm.
        alignAction->setObjectName(algorithmId);
        alignAction->setMenuTypes({MsaEditorMenuType::ALIGN_NEW_ALIGNMENT_TO_ALIGNMENT});
        addViewAction(alignAction);
    }

    QStringList alignSelectedSequencesAlgorithmIds = alignmentAlgorithmsRegistry->getAvailableAlgorithmIds(AlignSelectionToAlignment);
    for (const QString& algorithmId : qAsConst(alignSelectedSequencesAlgorithmIds)) {
        AlignmentAlgorithm* algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
        auto alignAction = new AlignSelectedSequencesAction(this, msaEditor, algorithmId, algorithm->getActionName(), 3000);
        alignAction->setIcon(QIcon(":/core/images/realign_some_sequences.png"));  // TODO: add a dedicated icon per algorithm.
        alignAction->setObjectName(algorithmId);
        alignAction->setMenuTypes({MsaEditorMenuType::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT});
        addViewAction(alignAction);
    }
}

/////////////////////////////////////
/// AlignSequencesToAlignmentAction
/////////////////////////////////////
BaseObjectViewAlignmentAction::BaseObjectViewAlignmentAction(QObject* parent, MSAEditor* view, const QString& _algorithmId, const QString& text, int order)
    : GObjectViewAction(parent, view, text, order), msaEditor(view), algorithmId(_algorithmId) {
}

MSAEditor* BaseObjectViewAlignmentAction::getEditor() const {
    return msaEditor;
}

/////////////////////////////////////
/// AlignSequencesToAlignmentAction
/////////////////////////////////////
AlignSequencesToAlignmentAction::AlignSequencesToAlignmentAction(QObject* parent, MSAEditor* view, const QString& algorithmId, const QString& text, int order)
    : BaseObjectViewAlignmentAction(parent, view, algorithmId, text, order) {
    connect(this, &QAction::triggered, this, &BaseObjectViewAlignmentAction::sl_activate);

    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    connect(msaObject, &MultipleSequenceAlignmentObject::si_lockedStateChanged, this, &AlignSequencesToAlignmentAction::sl_updateState);
    connect(msaObject, &MultipleSequenceAlignmentObject::si_alignmentChanged, this, &AlignSequencesToAlignmentAction::sl_updateState);

    sl_updateState();
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
    AlignmentAlgorithmsRegistry* alignmentAlgorithmsRegistry = AppContext::getAlignmentAlgorithmsRegistry();
    AlignmentAlgorithm* algorithm = alignmentAlgorithmsRegistry->getAlgorithm(algorithmId);
    if (!algorithm->checkAlphabet(msaObject->getAlphabet())) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
}

void AlignSequencesToAlignmentAction::sl_activate() {
    auto msaObject = msaEditor->getMaObject();
    SAFE_POINT(!msaObject->isStateLocked(), "The action must never be called for a readonly object!", );

    ProjectView* projectView = AppContext::getProjectView();
    SAFE_POINT(projectView != nullptr, "Project view is null", );

    const GObjectSelection* selection = projectView->getGObjectSelection();
    SAFE_POINT(selection != nullptr, "GObjectSelection is null", );

    QList<GObject*> objects = selection->getSelectedObjects();
    bool selectFromProject = !objects.isEmpty();

    for (const GObject* object : qAsConst(objects)) {
        const GObjectType& objectType = object->getGObjectType();
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
        QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, GObjectTypes::SEQUENCE});
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

/////////////////////////////////////
/// AlignSelectedSequencesAction
/////////////////////////////////////
AlignSelectedSequencesAction::AlignSelectedSequencesAction(QObject* parent, MSAEditor* view, const QString& algorithmId, const QString& text, int order)
    : BaseObjectViewAlignmentAction(parent, view, algorithmId, text, order) {
    connect(this, &QAction::triggered, this, &BaseObjectViewAlignmentAction::sl_activate);
    connect(msaEditor->alignSelectedSequencesToAlignmentAction,
            &QAction::changed,
            this,
            [this]() { setEnabled(msaEditor->alignSelectedSequencesToAlignmentAction->isEnabled()); });
    setEnabled(msaEditor->alignSelectedSequencesToAlignmentAction->isEnabled());
}

void AlignSelectedSequencesAction::sl_activate() {
    auto msaObject = msaEditor->getMaObject();
    SAFE_POINT(!msaObject->isStateLocked(), "The action must never be called for a readonly object!", );

    const MaEditorSelection& selection = msaEditor->getSelection();
    QList<int> selectedMaRowIndexes = msaEditor->getCollapseModel()->getMaRowIndexesFromSelectionRects(selection.getRectList());
    QList<qint64> selectedRowIds = msaObject->getRowIdsByRowIndexes(selectedMaRowIndexes);
    auto realignTask = new RealignSequencesInAlignmentTask(msaObject, selectedRowIds.toSet(), algorithmId);
    TaskWatchdog::trackResourceExistence(msaObject, realignTask, tr("A problem occurred during realigning sequences. The multiple alignment is no more available."));
    AppContext::getTaskScheduler()->registerTopLevelTask(realignTask);
}

}  // namespace U2
