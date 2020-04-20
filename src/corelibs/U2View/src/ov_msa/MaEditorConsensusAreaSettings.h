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

#ifndef _U2_MA_EDITOR_CONSENSUS_AREA_SETTINGS_H_
#define _U2_MA_EDITOR_CONSENSUS_AREA_SETTINGS_H_

#include <QFont>
#include <QMap>

namespace U2 {

enum MaEditorConsElement {
    MSAEditorConsElement_HISTOGRAM = 1 << 0,
    MSAEditorConsElement_CONSENSUS_TEXT = 1 << 1,
    MSAEditorConsElement_RULER = 1 << 2
};
Q_DECLARE_FLAGS(MaEditorConsElements, MaEditorConsElement)
Q_DECLARE_OPERATORS_FOR_FLAGS(MaEditorConsElements)

class MaEditorConsensusAreaSettings {
public:
    MaEditorConsensusAreaSettings();

    bool isVisible(const MaEditorConsElement element) const;

    const QFont &getRulerFont() const;
    void setRulerFont(const QFont &font);

    QFont font;
    QList<MaEditorConsElement>  order;
    MaEditorConsElements        visibleElements;
    // SANGER_TODO: valid only for mca yet - can be separated
    bool                        highlightMismatches;
    static const int RULER_NOTCH_SIZE;

private:
    QFont rulerFont;
};

}   // namespace U2

#endif // _U2_MA_EDITOR_CONSENSUS_AREA_SETTINGS_H_
