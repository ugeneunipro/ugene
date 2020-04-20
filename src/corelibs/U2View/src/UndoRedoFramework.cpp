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

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include "UndoRedoFramework.h"
#include "ov_msa/MaCollapseModel.h"

namespace U2 {

MsaUndoRedoFramework::MsaUndoRedoFramework(QObject *p, MultipleAlignmentObject *_maObj)
: QObject(p),
  maObj(_maObj),
  stateComplete(true),
  undoStepsAvailable(0),
  redoStepsAvailable(0)
{
    SAFE_POINT(maObj != NULL, "NULL MSA Object!", );

    undoAction = new QAction(this);
    undoAction->setText(tr("Undo"));
    undoAction->setIcon(QIcon(":core/images/undo.png"));
    undoAction->setShortcut(QKeySequence::Undo);
    GUIUtils::updateActionToolTip(undoAction);

    redoAction = new QAction(this);
    redoAction->setText(tr("Redo"));
    redoAction->setIcon(QIcon(":core/images/redo.png"));
    redoAction->setShortcut(QKeySequence::Redo);
    GUIUtils::updateActionToolTip(redoAction);

    checkUndoRedoEnabled();

    connect(maObj, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
                   SLOT(sl_updateUndoRedoState()));
    connect(maObj, SIGNAL(si_completeStateChanged(bool)), SLOT(sl_completeStateChanged(bool)));
    connect(maObj, SIGNAL(si_lockedStateChanged()), SLOT(sl_updateUndoRedoState()));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(sl_undo()));
    connect(redoAction, SIGNAL(triggered()), this, SLOT(sl_redo()));
}

void MsaUndoRedoFramework::sl_completeStateChanged(bool _stateComplete) {
    stateComplete = _stateComplete;
}

void MsaUndoRedoFramework::sl_updateUndoRedoState() {
    checkUndoRedoEnabled();
}

void MsaUndoRedoFramework::checkUndoRedoEnabled() {
    SAFE_POINT(maObj != NULL, "NULL MSA Object!", );

    if (maObj->isStateLocked() || !stateComplete) {
        undoAction->setEnabled(false);
        redoAction->setEnabled(false);
        return;
    }

    U2OpStatus2Log os;
    DbiConnection con(maObj->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(NULL != objDbi, "NULL Object Dbi!", );

    bool enableUndo = objDbi->canUndo(maObj->getEntityRef().entityId, os);
    SAFE_POINT_OP(os, );
    bool enableRedo = objDbi->canRedo(maObj->getEntityRef().entityId, os);
    SAFE_POINT_OP(os, );

    undoAction->setEnabled(enableUndo);
    redoAction->setEnabled(enableRedo);
}

void MsaUndoRedoFramework::sl_undo() {
    SAFE_POINT(maObj != NULL, "NULL MSA Object!", );

    U2OpStatus2Log os;
    U2EntityRef msaRef =  maObj->getEntityRef();

    assert(stateComplete);
    assert(!maObj->isStateLocked());

    DbiConnection con(msaRef.dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(NULL != objDbi, "NULL Object Dbi!", );

    objDbi->undo(msaRef.entityId, os);
    SAFE_POINT_OP(os, );

    MaModificationInfo modInfo;
    modInfo.type = MaModificationType_Undo;
    maObj->updateCachedMultipleAlignment(modInfo);
}

void MsaUndoRedoFramework::sl_redo() {
    SAFE_POINT(maObj != NULL, "NULL MSA Object!", );

    U2OpStatus2Log os;
    U2EntityRef msaRef =  maObj->getEntityRef();

    assert(stateComplete);
    assert(!maObj->isStateLocked());

    DbiConnection con(msaRef.dbiRef, os);
    SAFE_POINT_OP(os, );

    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();
    SAFE_POINT(NULL != objDbi, "NULL Object Dbi!", );

    objDbi->redo(msaRef.entityId, os);
    SAFE_POINT_OP(os, );

    MaModificationInfo modInfo;
    modInfo.type = MaModificationType_Redo;
    maObj->updateCachedMultipleAlignment(modInfo);
}


} // namespace
