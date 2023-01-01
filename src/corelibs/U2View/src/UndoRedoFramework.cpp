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

#include "UndoRedoFramework.h"

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

namespace U2 {

MaUndoRedoFramework::MaUndoRedoFramework(QObject* parent, MultipleAlignmentObject* _maObject)
    : QObject(parent),
      maObject(_maObject) {
    SAFE_POINT(maObject != nullptr, "NULL MSA Object!", );

    undoAction = new QAction(QIcon(":core/images/undo.png"), tr("Undo"), this);
    undoAction->setObjectName("msa_action_undo");
    undoAction->setShortcut(QKeySequence::Undo);
    GUIUtils::updateActionToolTip(undoAction);

    redoAction = new QAction(QIcon(":core/images/redo.png"), tr("Redo"), this);
    redoAction->setObjectName("msa_action_redo");
    redoAction->setShortcut(QKeySequence::Redo);
    GUIUtils::updateActionToolTip(redoAction);

    checkUndoRedoEnabled();

    connect(maObject, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), SLOT(sl_updateUndoRedoState()));
    connect(maObject, SIGNAL(si_completeStateChanged(bool)), SLOT(sl_completeStateChanged(bool)));
    connect(maObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_updateUndoRedoState()));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(sl_undo()));
    connect(redoAction, SIGNAL(triggered()), this, SLOT(sl_redo()));
}

void MaUndoRedoFramework::sl_completeStateChanged(bool _stateComplete) {
    stateComplete = _stateComplete;
}

void MaUndoRedoFramework::sl_updateUndoRedoState() {
    checkUndoRedoEnabled();
}

void MaUndoRedoFramework::checkUndoRedoEnabled() {
    SAFE_POINT(maObject != nullptr, "NULL MSA Object!", );

    if (maObject->isStateLocked() || !stateComplete) {
        undoAction->setEnabled(false);
        redoAction->setEnabled(false);
        return;
    }

    U2OpStatus2Log os;
    DbiConnection con(maObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(objDbi != nullptr, "NULL Object Dbi!", );

    bool enableUndo = objDbi->canUndo(maObject->getEntityRef().entityId, os);
    SAFE_POINT_OP(os, );
    bool enableRedo = objDbi->canRedo(maObject->getEntityRef().entityId, os);
    SAFE_POINT_OP(os, );

    undoAction->setEnabled(enableUndo);
    redoAction->setEnabled(enableRedo);
    if (!enableUndo) {
        maObject->setModified(false);
    }
}

void MaUndoRedoFramework::sl_undo() {
    SAFE_POINT(maObject != nullptr, "NULL MSA Object!", );

    U2OpStatus2Log os;
    U2EntityRef msaRef = maObject->getEntityRef();

    assert(stateComplete);
    assert(!maObject->isStateLocked());

    DbiConnection con(msaRef.dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(objDbi != nullptr, "NULL Object Dbi!", );

    objDbi->undo(msaRef.entityId, os);
    SAFE_POINT_OP(os, );

    MaModificationInfo modInfo;
    modInfo.type = MaModificationType_Undo;
    maObject->updateCachedMultipleAlignment(modInfo);
}

void MaUndoRedoFramework::sl_redo() {
    SAFE_POINT(maObject != nullptr, "NULL MSA Object!", );

    U2OpStatus2Log os;
    U2EntityRef msaRef = maObject->getEntityRef();

    assert(stateComplete);
    assert(!maObject->isStateLocked());

    DbiConnection con(msaRef.dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(objDbi != nullptr, "NULL Object Dbi!", );

    objDbi->redo(msaRef.entityId, os);
    SAFE_POINT_OP(os, );

    MaModificationInfo modInfo;
    modInfo.type = MaModificationType_Redo;
    maObject->updateCachedMultipleAlignment(modInfo);
}

QAction* MaUndoRedoFramework::getUndoAction() const {
    return undoAction;
}

QAction* MaUndoRedoFramework::getRedoAction() const {
    return redoAction;
}

}  // namespace U2
