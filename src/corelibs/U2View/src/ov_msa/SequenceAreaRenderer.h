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

#pragma once

#include <QPen>

#include <U2Core/Chromatogram.h>

#include <U2View/MsaEditorSequenceArea.h>

namespace U2 {

class SequenceAreaRenderer : public QObject {
    Q_OBJECT
public:
    SequenceAreaRenderer(MaEditorWgt* ui, MaEditorSequenceArea* seqAreaWgt);

    bool drawContent(QPainter& painter, const U2Region& columns, const QList<int>& maRows, int xStart, int yStart) const;

    virtual void drawSelectionFrame(QPainter& painter) const;
    void drawFocus(QPainter& painter) const;

    /**
     * Checks if the character at the given position should be rendered with a highlighted background.
     * By default MSA editor highlights background under selected bases.
     */
    virtual bool hasHighlightedBackground(int columnIndex, int viewRowIndex) const;

protected:
    // returns the height of the drawn row
    virtual int drawRow(QPainter& painter, const Msa& ma, int maRowIndex, const U2Region& columns, int xStart, int yStart) const;

    MaEditorWgt* ui;
    MaEditorSequenceArea* seqAreaWgt;

    bool drawLeadingAndTrailingGaps;

    static const int SELECTION_SATURATION_INCREASE;
};

}  // namespace U2
