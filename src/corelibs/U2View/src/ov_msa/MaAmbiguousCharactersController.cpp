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

#include "MaAmbiguousCharactersController.h"

#include <QBitArray>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Notification.h>

#include "ScrollController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

const QPoint MaAmbiguousCharactersController::INVALID_POINT = QPoint(-1, -1);

MaAmbiguousCharactersController::MaAmbiguousCharactersController(MaEditorWgt* maEditorWgt)
    : QObject(maEditorWgt),
      maEditor(nullptr != maEditorWgt ? maEditorWgt->getEditor() : nullptr),
      maEditorWgt(maEditorWgt),
      nextAction(nullptr),
      previousAction(nullptr) {
    SAFE_POINT(nullptr != maEditorWgt, "maEditorWgt is NULL", );
    SAFE_POINT(nullptr != maEditor, "maEditor is NULL", );

    nextAction = new QAction(QIcon(":core/images/amb_forward.png"), tr("Jump to next ambiguous character"), this);
    nextAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A));
    nextAction->setObjectName("next_ambiguous");
    GUIUtils::updateActionToolTip(nextAction);
    connect(nextAction, SIGNAL(triggered(bool)), SLOT(sl_next()));

    previousAction = new QAction(QIcon(":core/images/amb_backward.png"), tr("Jump to previous ambiguous character"), this);
    previousAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_A));
    previousAction->setObjectName("prev_ambiguous");
    GUIUtils::updateActionToolTip(previousAction);
    connect(previousAction, SIGNAL(triggered(bool)), SLOT(sl_previous()));

    connect(maEditor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_resetCachedIterator()));
    connect(maEditor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_resetCachedIterator()));
}

QAction* MaAmbiguousCharactersController::getPreviousAction() const {
    return previousAction;
}

QAction* MaAmbiguousCharactersController::getNextAction() const {
    return nextAction;
}

void MaAmbiguousCharactersController::sl_next() {
    GCounter::increment("Jump to next ambiguous character", maEditor->getFactoryId());
    scrollToNextAmbiguous(Forward);
}

void MaAmbiguousCharactersController::sl_previous() {
    GCounter::increment("Jump to previous ambiguous character", maEditor->getFactoryId());
    scrollToNextAmbiguous(Backward);
}

void MaAmbiguousCharactersController::sl_resetCachedIterator() {
    cachedIterator.reset();
}

void MaAmbiguousCharactersController::scrollToNextAmbiguous(NavigationDirection direction) const {
    QPoint nextAmbiguous = findNextAmbiguous(direction);
    if (nextAmbiguous != INVALID_POINT) {
        maEditorWgt->getScrollController()->centerPoint(nextAmbiguous, maEditorWgt->getSequenceArea()->size());
        maEditorWgt->getSequenceArea()->setSelectionRect(QRect(nextAmbiguous.x(), nextAmbiguous.y(), 1, 1));
    } else {
        // no mismatches - show notification
        NotificationStack::addNotification(tr("There are no ambiguous characters in the alignment."), Info_Not);
    }
}

QPoint MaAmbiguousCharactersController::getStartPosition() const {
    const MaEditorSelection& selection = maEditorWgt->getEditor()->getSelection();
    if (!selection.isEmpty()) {
        return selection.toRect().topLeft();
    }

    return QPoint(maEditorWgt->getScrollController()->getFirstVisibleBase(),
                  maEditorWgt->getScrollController()->getFirstVisibleMaRowIndex());
}

namespace {

QBitArray getAmbiguousCharacters() {
    QBitArray ambiguousCharacters(256);
    const QByteArray ambiguousCharactersString = "MRWSYKVHDBNX";
    for (int i = 0; i < ambiguousCharactersString.length(); i++) {
        ambiguousCharacters.setBit(static_cast<int>(ambiguousCharactersString[i]));
    }
    return ambiguousCharacters;
}

}  // namespace

QPoint MaAmbiguousCharactersController::findNextAmbiguous(NavigationDirection direction) const {
    static const QBitArray ambiguousCharacters = getAmbiguousCharacters();

    const QPoint startPosition = getStartPosition();
    prepareIterator(direction, startPosition);
    SAFE_POINT(nullptr != cachedIterator, "MaIterator is not valid", INVALID_POINT);

    while (cachedIterator->hasNext()) {
        if (ambiguousCharacters[cachedIterator->next()]) {
            return cachedIterator->getMaPoint();
        }
        CHECK(cachedIterator->getMaPoint() != startPosition, INVALID_POINT);
    }

    return INVALID_POINT;
}

void MaAmbiguousCharactersController::prepareIterator(NavigationDirection direction, const QPoint& startPosition) const {
    if (nullptr == cachedIterator) {
        cachedIterator.reset(new MaIterator(maEditor->getMaObject()->getMultipleAlignment(),
                                            direction,
                                            maEditor->getCollapseModel()->getMaRowsIndexesWithViewRowIndexes()));
        cachedIterator->setCircular(true);
        cachedIterator->setIterateInCoreRegionsOnly(true);
    }
    cachedIterator->setMaPoint(startPosition);
    cachedIterator->setDirection(direction);
}

}  // namespace U2
