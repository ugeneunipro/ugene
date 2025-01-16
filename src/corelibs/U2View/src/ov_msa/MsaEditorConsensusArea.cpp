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

#include "MsaEditorConsensusArea.h"

#include <U2Algorithm/MsaConsensusUtils.h>

#include <U2Core/DNAAlphabet.h>

#include <U2Gui/GUIUtils.h>

#include "MaConsensusAreaRenderer.h"
#include "MsaEditor.h"

namespace U2 {

/************************************************************************/
/* MSAEditorConsensusArea */
/************************************************************************/
MsaEditorConsensusArea::MsaEditorConsensusArea(MsaEditorWgt* ui)
    : MaEditorConsensusArea(ui) {
    initCache();
    initRenderer();
    setupFontAndHeight();

    connect(editor, &GObjectViewController::si_buildMenu, this, &MsaEditorConsensusArea::sl_buildMenu);
}

QString MsaEditorConsensusArea::getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const {
    return MsaConsensusUtils::getConsensusPercentTip(editor->getMaObject()->getAlignment(), pos, minReportPercent, maxReportChars);
}

void MsaEditorConsensusArea::sl_buildMenu(GObjectViewController* /*view*/, QMenu* menu, const QString& menuType) {
    if (menuType == MsaEditorMenuType::CONTEXT || menuType == MsaEditorMenuType::STATIC) {
        buildMenu(menu);
    }
}

void MsaEditorConsensusArea::initRenderer() {
    renderer = new MaConsensusAreaRenderer(this);
}

QString MsaEditorConsensusArea::getLastUsedAlgoSettingsKey() const {
    const DNAAlphabet* al = editor->getMaObject()->getAlphabet();
    SAFE_POINT_NN(al, "");
    const char* suffix = al->isAmino() ? "_protein" : al->isNucleic() ? "_nucleic"
                                                                      : "_raw";
    return editor->getSettingsRoot() + "_consensus_algorithm_" + suffix;
}

void MsaEditorConsensusArea::buildMenu(QMenu* menu) {
    auto msaEditor = qobject_cast<MsaEditor*>(editor);
    SAFE_POINT_NN(msaEditor, );
    if (msaEditor->getMainWidget()->getActiveChild() != ui) {
        return;
    }
    menu->addAction(configureConsensusAction);
}

}  // namespace U2
