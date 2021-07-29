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

#include "MoveToObjectMaController.h"

#include <QMessageBox>

#include <U2Core/AddSequencesToAlignmentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaCollapseModel.h>
#include <U2View/MaEditor.h>
#include <U2View/MaEditorSelection.h>
#include <U2View/MaEditorWgt.h>

namespace U2 {

MoveToObjectMaController::MoveToObjectMaController(MaEditor *maEditor)
    : QObject(maEditor), MaEditorContext(maEditor) {
    moveSelectionToAnotherObjectAction = new QAction(tr("Move selected rows to another alignment"));
    moveSelectionToAnotherObjectAction->setObjectName("move_selection_to_another_object");
    connect(moveSelectionToAnotherObjectAction, &QAction::triggered, this, &MoveToObjectMaController::showMoveSelectedRowsToAnotherObjectMenu);

    connect(editor, &MaEditor::si_updateActions, this, &MoveToObjectMaController::updateActions);
    connect(editor, &MaEditor::si_buildMenu, this, &MoveToObjectMaController::buildMenu);
}

QMenu *MoveToObjectMaController::buildMoveSelectionToAnotherObjectMenu() const {
    QMenu *menu = new QMenu(moveSelectionToAnotherObjectAction->text());
    QList<GObject *> writableMsaObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true);
    writableMsaObjects.removeOne(maObject);
    std::stable_sort(writableMsaObjects.begin(), writableMsaObjects.end(), [&](const GObject *o1, const GObject *o2) {
        return o1->getGObjectName().compare(o2->getGObjectName(), Qt::CaseInsensitive);
    });
    for (const GObject *object : qAsConst(writableMsaObjects)) {
        GObjectReference reference(object);
        QString fileName = object->getDocument()->getURL().fileName();
        QString menuItemText = object->getGObjectName() + " [" + fileName + "] ";
        QAction *action = menu->addAction(menuItemText, [this, reference]() {
            GObject *object = GObjectUtils::selectObjectByReference(reference, UOF_LoadedOnly);
            CHECK_EXT(object != nullptr, QMessageBox::critical(ui, L10N::errorTitle(), L10N::errorObjectNotFound(reference.objName)), );
            CHECK_EXT(!object->isStateLocked(), QMessageBox::critical(ui, L10N::errorTitle(), L10N::errorObjectIsReadOnly(reference.objName)), );

            auto targetMsaObject = qobject_cast<MultipleSequenceAlignmentObject *>(object);
            CHECK_EXT(targetMsaObject != nullptr, QMessageBox::critical(ui, L10N::errorTitle(), L10N::nullPointerError(reference.objName)), );

            const QList<QRect> &selectedRects = getSelection().getRectList();
            QList<DNASequence> sequencesWithGapsToMove;
            QList<int> maRowIndexesToRemove;
            U2OpStatusImpl os;
            for (const QRect &rect : qAsConst(selectedRects)) {
                for (int viewRowIndex = rect.top(); viewRowIndex <= rect.bottom(); viewRowIndex++) {
                    int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
                    SAFE_POINT(maRowIndex >= 0, "MA row not found. View row: " + QString::number(viewRowIndex), );
                    maRowIndexesToRemove << maRowIndex;
                    MultipleAlignmentRow row = maObject->getRow(maRowIndex);
                    QByteArray sequenceWithGaps = row->getSequenceWithGaps(true, false);
                    CHECK_OP_EXT(os, QMessageBox::critical(ui, L10N::errorTitle(), os.getError()), );
                    sequencesWithGapsToMove << DNASequence(row->getName(), sequenceWithGaps, maObject->getAlphabet());
                }
            }
            CHECK_EXT(maRowIndexesToRemove.size() < maObject->getNumRows(), os.setError(tr("Can't remove all rows from the alignment")), );
            U2UseCommonUserModStep userModStep(maObject->getEntityRef(), os);
            CHECK_OP(os, );
            maObject->removeRows(maRowIndexesToRemove);
            // If not cleared explicitly another row is auto-selected and the result may be misinterpret like not all rows were moved.
            selectionController->clearSelection();

            auto addRowsTask = new AddSequenceObjectsToAlignmentTask(targetMsaObject, sequencesWithGapsToMove, -1, true);
            AppContext::getTaskScheduler()->registerTopLevelTask(addRowsTask);
        });
        action->setObjectName(fileName);    // For UI testing.
    }
    if (menu->isEmpty()) {
        QAction *noObjectsAction = menu->addAction(tr("No other alignment objects in the project"), []() {});
        noObjectsAction->setObjectName("no_other_objects_item");
        noObjectsAction->setEnabled(false);
    }
    menu->setEnabled(moveSelectionToAnotherObjectAction->isEnabled());
    return menu;
}

void MoveToObjectMaController::showMoveSelectedRowsToAnotherObjectMenu() {
    QScopedPointer<QMenu> menu(buildMoveSelectionToAnotherObjectMenu());
    menu->exec(QCursor::pos());
}

void MoveToObjectMaController::updateActions() {
    int countOfSelectedRows = getSelection().getCountOfSelectedRows();
    moveSelectionToAnotherObjectAction->setEnabled(!maObject->isStateLocked() && countOfSelectedRows > 0 && countOfSelectedRows < maObject->getNumRows());
}

void MoveToObjectMaController::buildMenu(GObjectView *, QMenu *menu, const QString &menuType) {
    CHECK(menuType == MsaEditorMenuType::CONTEXT, );
    QMenu *exportMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu is null", );
    QAction *menuAction = exportMenu->addMenu(buildMoveSelectionToAnotherObjectMenu());
    menuAction->setObjectName(moveSelectionToAnotherObjectAction->objectName());
}

}    // namespace U2
