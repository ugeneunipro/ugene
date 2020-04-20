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

#include "MaEditorConsensusAreaSettings.h"

namespace U2 {

const int MaEditorConsensusAreaSettings::RULER_NOTCH_SIZE = 3;

MaEditorConsensusAreaSettings::MaEditorConsensusAreaSettings()
    : visibleElements(MSAEditorConsElement_HISTOGRAM | MSAEditorConsElement_CONSENSUS_TEXT | MSAEditorConsElement_RULER),
      highlightMismatches(false)
{
    // SANGER_TODO: currently the ruler cannot be drawn above the text - draw methods should be refactored
    order << MSAEditorConsElement_HISTOGRAM
          << MSAEditorConsElement_CONSENSUS_TEXT
          << MSAEditorConsElement_RULER;
}

bool MaEditorConsensusAreaSettings::isVisible(const MaEditorConsElement element) const {
    return visibleElements.testFlag(element);
}

const QFont &MaEditorConsensusAreaSettings::getRulerFont() const {
    return rulerFont;
}

void MaEditorConsensusAreaSettings::setRulerFont(const QFont &font) {
    rulerFont.setFamily("Arial");
    rulerFont.setPointSize(qMax(8, qRound(font.pointSize() * 0.7)));
}

}   // namespace U2
