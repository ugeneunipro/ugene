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

#include "McaEditorConsensusArea.h"

#include <QToolBar>

#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusUtils.h>

#include <U2Core/AppContext.h>

#include <U2Gui/GUIUtils.h>

#include "McaEditor.h"
#include "ov_msa/MaConsensusMismatchController.h"
#include "view_rendering/McaConsensusAreaRenderer.h"

namespace U2 {

/************************************************************************/
/* McaEditorConsensusArea */
/************************************************************************/
McaEditorConsensusArea::McaEditorConsensusArea(McaEditorWgt *ui)
    : MaEditorConsensusArea(ui) {
    initCache();

    mismatchController = new MaConsensusMismatchController(this, consensusCache, editor);
    addAction(mismatchController->getPrevMismatchAction());
    addAction(mismatchController->getNextMismatchAction());

    initRenderer();
    setupFontAndHeight();
}

void McaEditorConsensusArea::buildStaticToolbar(QToolBar *t) {
    t->addAction(mismatchController->getPrevMismatchAction());
    t->addAction(mismatchController->getNextMismatchAction());
}

QString McaEditorConsensusArea::getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const {
    return MSAConsensusUtils::getConsensusPercentTip(editor->getMaObject()->getMultipleAlignment(), pos, 0, 4, true);
}

void McaEditorConsensusArea::initRenderer() {
    renderer = new McaConsensusAreaRenderer(this);
}

bool McaEditorConsensusArea::highlightConsensusChar(int pos) {
    return consensusSettings.highlightMismatches && mismatchController->isMismatch(pos);
}

QString McaEditorConsensusArea::getLastUsedAlgoSettingsKey() const {
    return editor->getSettingsRoot() + MCAE_SETTINGS_CONSENSUS_TYPE;
}

}    // namespace U2
