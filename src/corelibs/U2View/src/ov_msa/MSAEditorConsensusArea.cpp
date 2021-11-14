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

#include "MSAEditorConsensusArea.h"

#include <U2Algorithm/MSAConsensusUtils.h>

#include <U2Core/DNAAlphabet.h>

#include <U2Gui/GUIUtils.h>

#include "MSAEditor.h"
#include "MaConsensusAreaRenderer.h"

namespace U2 {

/************************************************************************/
/* MSAEditorConsensusArea */
/************************************************************************/
MSAEditorConsensusArea::MSAEditorConsensusArea(MsaEditorWgt* ui)
    : MaEditorConsensusArea(ui) {
    initCache();
    initRenderer();
    setupFontAndHeight();

    connect(editor, SIGNAL(si_buildMenu(GObjectView*, QMenu*, const QString&)), SLOT(sl_buildMenu(GObjectView*, QMenu*, const QString&)));
}

QString MSAEditorConsensusArea::getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const {
    return MSAConsensusUtils::getConsensusPercentTip(editor->getMaObject()->getMultipleAlignment(), pos, minReportPercent, maxReportChars);
}

void MSAEditorConsensusArea::sl_buildMenu(GObjectView* /*view*/, QMenu* menu, const QString& menuType) {
    if (menuType == MsaEditorMenuType::CONTEXT || menuType == MsaEditorMenuType::STATIC) {
        buildMenu(menu);
    }
}

void MSAEditorConsensusArea::initRenderer() {
    renderer = new MaConsensusAreaRenderer(this);
}

QString MSAEditorConsensusArea::getLastUsedAlgoSettingsKey() const {
    const DNAAlphabet* al = editor->getMaObject()->getAlphabet();
    SAFE_POINT(al != nullptr, "Alphabet is NULL", "");
    const char* suffix = al->isAmino() ? "_protein" : al->isNucleic() ? "_nucleic"
                                                                      : "_raw";
    return editor->getSettingsRoot() + "_consensus_algorithm_" + suffix;
}

void MSAEditorConsensusArea::buildMenu(QMenu *menu) {
    if (editor->getUI()->getActiveChild() != ui) {
        return;
    }
    menu->addAction(configureConsensusAction);
}

}  // namespace U2
