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

#include "MoveToObjectMaController.h"
#include "../CreateSubalignmentDialogController.h"

#include <QMessageBox>

#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MsaObject.h>
#include <U2Core/MultiTask.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2Mod.h>

#include <U2Formats/ExportTasks.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/ExportDocumentDialogController.h>

#include <U2View/MaCollapseModel.h>
#include <U2View/MaEditorSelection.h>
#include <U2View/MsaEditor.h>

namespace U2 {

MoveToObjectMaController::MoveToObjectMaController(MaEditor* maEditor, QWidget* wgt)
    : QObject(maEditor), MaEditorContext(maEditor, wgt) {
    moveSelectionToAnotherObjectAction = new QAction(tr("Move selected rows to another alignment"));
    moveSelectionToAnotherObjectAction->setObjectName("move_selection_to_another_object");
    connect(moveSelectionToAnotherObjectAction, &QAction::triggered, this, &MoveToObjectMaController::showMoveSelectedRowsToAnotherObjectMenu);

    moveSelectionToNewFileAction = new QAction(tr("Create a new alignment"));
    moveSelectionToNewFileAction->setObjectName("move_selection_to_new_file");
    connect(moveSelectionToNewFileAction, &QAction::triggered, this, &MoveToObjectMaController::runMoveSelectedRowsToNewFileDialog);

    connect(editor, &MaEditor::si_updateActions, this, &MoveToObjectMaController::updateActions);
    connect(editor, &MaEditor::si_buildMenu, this, &MoveToObjectMaController::buildMenu);
}

QMenu* MoveToObjectMaController::buildMoveSelectionToAnotherObjectMenu() const {
    auto menu = new QMenu(moveSelectionToAnotherObjectAction->text());
    menu->setEnabled(moveSelectionToAnotherObjectAction->isEnabled());
    connect(moveSelectionToAnotherObjectAction, &QAction::changed, menu, [this, menu]() { menu->setEnabled(moveSelectionToAnotherObjectAction->isEnabled()); });
    connect(menu, &QMenu::aboutToShow, this, [this, menu]() {
        menu->clear();
        menu->addAction(moveSelectionToNewFileAction);
        menu->addSeparator();
        QList<GObject*> writableMsaObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true);
        writableMsaObjects.removeOne(maObject);
        std::stable_sort(writableMsaObjects.begin(), writableMsaObjects.end(), GObject::objectLessThan);  // Sort objects in the menu by name.

        if (writableMsaObjects.isEmpty()) {
            QAction* noObjectsAction = menu->addAction(tr("No other alignment objects in the project"), []() {});
            noObjectsAction->setObjectName("no_other_objects_item");
            noObjectsAction->setEnabled(false);
        }
        QIcon objectMenuIcon(":core/images/msa.png");
        for (const GObject* object : qAsConst(writableMsaObjects)) {
            GObjectReference reference(object);
            QString fileName = object->getDocument()->getURL().fileName();
            QString menuItemText = object->getGObjectName() + " [" + fileName + "]";
            QAction* action = menu->addAction(objectMenuIcon, menuItemText, [this, reference]() {
                GCounter::increment("MoveSelectedMsaRowsToNewObject");
                GObject* referenceObject = GObjectUtils::selectObjectByReference(reference, UOF_LoadedOnly);
                CHECK_EXT(referenceObject != nullptr, QMessageBox::critical(ui, L10N::errorTitle(), L10N::errorObjectNotFound(reference.objName)), );
                CHECK_EXT(!referenceObject->isStateLocked(), QMessageBox::critical(ui, L10N::errorTitle(), L10N::errorObjectIsReadOnly(reference.objName)), );

                auto targetMsaObject = qobject_cast<MsaObject*>(referenceObject);
                CHECK_EXT(targetMsaObject != nullptr, QMessageBox::critical(ui, L10N::errorTitle(), L10N::nullPointerError(reference.objName)), );

                QList<int> selectedViewRowIndexes = getSelection().getSelectedRowIndexes();
                QList<int> selectedMaRowIndexes = collapseModel->getMaRowIndexesByViewRowIndexes(selectedViewRowIndexes, true);
                QList<qint64> rowIdsToRemove = maObject->getRowIdsByRowIndexes(selectedMaRowIndexes);
                CHECK_EXT(!rowIdsToRemove.isEmpty(), QMessageBox::critical(ui, L10N::errorTitle(), L10N::internalError()), );
                QList<DNASequence> sequencesWithGapsToMove;
                for (int maRowIndex : qAsConst(selectedMaRowIndexes)) {
                    MsaRow row = maObject->getRow(maRowIndex);
                    QByteArray sequenceWithGaps = row->getSequenceWithGaps(true, false);
                    sequencesWithGapsToMove << DNASequence(row->getName(), sequenceWithGaps, maObject->getAlphabet());
                }
                auto addRowsTask = new AddSequenceObjectsToAlignmentTask(targetMsaObject, sequencesWithGapsToMove, -1, true);
                auto removeRowsTask = new RemoveRowsFromMaObjectTask(editor, rowIdsToRemove);
                AppContext::getTaskScheduler()->registerTopLevelTask(new MultiTask(tr("Move rows to another alignment"), {addRowsTask, removeRowsTask}));
            });
            action->setObjectName(fileName);  // For UI testing.
        }
    });
    return menu;
}

void MoveToObjectMaController::showMoveSelectedRowsToAnotherObjectMenu() {
    QScopedPointer<QMenu> menu(buildMoveSelectionToAnotherObjectMenu());
    menu->exec(QCursor::pos());
}

void MoveToObjectMaController::updateActions() {
    int countOfSelectedRows = getSelection().getCountOfSelectedRows();
    bool isMoveOk = !maObject->isStateLocked() && countOfSelectedRows > 0 && countOfSelectedRows < maObject->getRowCount();
    moveSelectionToAnotherObjectAction->setEnabled(isMoveOk);
    moveSelectionToNewFileAction->setEnabled(isMoveOk);
}

void MoveToObjectMaController::buildMenu(GObjectViewController*, QMenu* menu, const QString&) {
    QMenu* exportMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu is null", );
    QAction* menuAction = exportMenu->addMenu(buildMoveSelectionToAnotherObjectMenu());
    menuAction->setObjectName(moveSelectionToAnotherObjectAction->objectName());
}

void MoveToObjectMaController::runMoveSelectedRowsToNewFileDialog() {
    GCounter::increment("MoveSelectedMsaRowsToNewFile");

    // Get the file name and format to move rows to first.
    DocumentFormatConstraints dfc;
    dfc.supportedObjectTypes.insert(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    dfc.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    dfc.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    dfc.addFlagToExclude(DocumentFormatFlag_Hidden);
    LastUsedDirHelper lod;
    QObjectScopedPointer<ExportDocumentDialogController> dialog(new ExportDocumentDialogController(
                                                                lod.getLastUsedDir() + "/selected_row",
                                                                dfc, ui));
    dialog->setWindowTitle(tr("File path and format to save"));
    CHECK(dialog->exec() == QDialog::Accepted, );
    const DocumentFormatId formatId = dialog->getDocumentFormatId();
    const QString url = dialog->getDocumentURL();

    // Create a sub-alignment from moved rows.
    QList<int> selectedViewRowIndexes = getSelection().getSelectedRowIndexes();
    QList<int> selectedMaRowIndexes = collapseModel->getMaRowIndexesByViewRowIndexes(selectedViewRowIndexes, true);
    QList<qint64> rowIdsToRemove = maObject->getRowIdsByRowIndexes(selectedMaRowIndexes);
    SAFE_POINT(!rowIdsToRemove.isEmpty(), "rowIdsToRemove is empty", );

    QFileInfo urlInfo(url);
    CHECK_EXT(!urlInfo.baseName().isEmpty() && !urlInfo.isDir(), QMessageBox::critical(ui, L10N::errorTitle(), tr("Please select a file with a non-empty name.")), );
    Msa msaToExport;
    msaToExport->setName(urlInfo.baseName());
    msaToExport->setAlphabet(maObject->getAlphabet());
    for (int maRowIndex : qAsConst(selectedMaRowIndexes)) {
        const MsaRow& row = maObject->getRow(maRowIndex);
        msaToExport->addRow(row->getName(), row->getSequenceWithGaps(true, true));
    }

    // Run 2 tasks: first create a new document, next remove moved rows from the original document.
    if (dialog->getAddToProjectFlag()) {
        auto createNewMsaTask = new AddDocumentAndOpenViewTask(new ExportAlignmentTask(msaToExport, url, formatId));
        auto removeRowsTask = new RemoveRowsFromMaObjectTask(editor, rowIdsToRemove);
        auto task = new MultiTask(tr("Export alignment rows to a new file"), {createNewMsaTask, removeRowsTask});
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    } else {
        MsaObject* maObject = editor->getMaObject();
        QList<qint64> rowsToKeep = maObject->getRowIds();
        for (const qint64 rowId : qAsConst(rowIdsToRemove)) {
            rowsToKeep.removeAll(rowId);
        }
        CreateSubalignmentSettings createSubalignmentSettings(rowsToKeep, U2Region(0, maObject->getLength()), url, true, false, formatId);
        auto createSubAlignmentTask = new CreateSubalignmentAndOpenViewTask(maObject, createSubalignmentSettings);
        AppContext::getTaskScheduler()->registerTopLevelTask(createSubAlignmentTask);
    }
}

/************************************************************************/
/* RemoveRowsFromMsaObjectTask */
/************************************************************************/
RemoveRowsFromMaObjectTask::RemoveRowsFromMaObjectTask(MaEditor* _maEditor, const QList<qint64>& _rowIds)
    : Task(tr("Remove rows from alignment"), TaskFlag_RunInMainThread), maEditor(_maEditor), rowIds(_rowIds) {
}

void RemoveRowsFromMaObjectTask::run() {
    CHECK(!maEditor.isNull(), );  // The editor may be closed while the task in the queue.

    MsaObject* maObject = maEditor->getMaObject();
    CHECK_EXT(rowIds.size() < maObject->getRowCount(), setError(tr("Can't remove all rows from the alignment")), );
    U2UseCommonUserModStep userModStep(maObject->getEntityRef(), stateInfo);
    CHECK_OP(stateInfo, );

    maObject->removeRowsById(rowIds);
    // If not cleared explicitly another row is auto-selected and the result may be misinterpreted like not all rows were moved.
    maEditor->getSelectionController()->clearSelection();
}

}  // namespace U2
