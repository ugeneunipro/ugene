/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MaConsensusMismatchController.h"

#include <U2Algorithm/MSAConsensusAlgorithm.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceSelection.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Notification.h>

#include "McaEditor.h"
#include "McaEditorSequenceArea.h"
#include "ov_msa/MSAEditorConsensusCache.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MaEditorMultilineWgt.h"
#include "ov_sequence/SequenceObjectContext.h"

namespace U2 {

MaConsensusMismatchController::MaConsensusMismatchController(QObject* p,
                                                             const QSharedPointer<MSAEditorConsensusCache>& consCache,
                                                             MaEditor* editor)
    : QObject(p),
      consCache(consCache),
      editor(editor),
      nextMismatch(nullptr),
      prevMismatch(nullptr) {
    mismatchCache = QBitArray(editor->getAlignmentLen(), false);
    connect(consCache.data(), SIGNAL(si_cachedItemUpdated(int, char)), SLOT(sl_updateItem(int, char)));
    connect(consCache.data(), SIGNAL(si_cacheResized(int)), SLOT(sl_resize(int)));

    nextMismatch = new QAction(QIcon(":core/images/mismatch-forward.png"), tr("Jump to next variation"), this);
    nextMismatch->setObjectName("next_mismatch");
    nextMismatch->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_V);
    GUIUtils::updateActionToolTip(nextMismatch);
    connect(nextMismatch, SIGNAL(triggered(bool)), SLOT(sl_next()));

    prevMismatch = new QAction(QIcon(":core/images/mismatch-backward.png"), tr("Jump to previous variation"), this);
    prevMismatch->setShortcut(Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_V);
    prevMismatch->setObjectName("prev_mismatch");
    GUIUtils::updateActionToolTip(prevMismatch);
    connect(prevMismatch, SIGNAL(triggered(bool)), SLOT(sl_prev()));
}

bool MaConsensusMismatchController::isMismatch(int pos) const {
    SAFE_POINT(0 <= pos && pos < mismatchCache.size(), "Invalid pos", false);
    return mismatchCache[pos];
}

QAction* MaConsensusMismatchController::getPrevMismatchAction() const {
    return prevMismatch;
}

QAction* MaConsensusMismatchController::getNextMismatchAction() const {
    return nextMismatch;
}

void MaConsensusMismatchController::sl_updateItem(int pos, char c) {
    SAFE_POINT(0 <= pos && pos < mismatchCache.size(), "Invalid pos", );
    mismatchCache[pos] = c != MSAConsensusAlgorithm::INVALID_CONS_CHAR && editor->getReferenceCharAt(pos) != c;
}

void MaConsensusMismatchController::sl_resize(int newSize) {
    mismatchCache.resize(newSize);
    mismatchCache.fill(false);
}

void MaConsensusMismatchController::sl_next() {
    GCounter::increment("Jump to next variation", editor->getFactoryId());
    selectNextMismatch(Forward);
}

void MaConsensusMismatchController::sl_prev() {
    GCounter::increment("Jump to previous variation", editor->getFactoryId());
    selectNextMismatch(Backward);
}

void MaConsensusMismatchController::selectNextMismatch(NavigationDirection direction) {
    McaEditor* mcaEditor = qobject_cast<McaEditor*>(editor);
    CHECK(mcaEditor != nullptr, );

    SequenceObjectContext* ctx = mcaEditor->getReferenceContext();
    int initialPos = -1;

    if (ctx->getSequenceSelection()->isEmpty()) {
        // find next/prev from visible range
        MaEditorSequenceArea *seqArea = mcaEditor->getUI()->getSequenceArea();
        initialPos = seqArea->getFirstVisibleBase() != 0 ? seqArea->getFirstVisibleBase() - 1 : mismatchCache.size() - 1;
    } else {
        // find next/prev from referenece selection
        DNASequenceSelection* selection = ctx->getSequenceSelection();
        initialPos = selection->getSelectedRegions().first().startPos;
    }

    int pos = initialPos;
    do {
        switch (direction) {
            case Forward:
                pos++;
                if (pos == mismatchCache.size()) {
                    pos = 0;
                }
                break;
            default:
                pos--;
                if (pos == -1) {
                    pos = mismatchCache.size() - 1;
                }
                break;
        }
        consCache->updateCacheItem(pos);
        if (mismatchCache[pos] == true) {
            emit si_selectMismatch(pos);
            return;
        }
    } while (pos != initialPos);
    NotificationStack::addNotification(tr("There are no variations in the consensus sequence."), Info_Not);
}

}  // namespace U2
