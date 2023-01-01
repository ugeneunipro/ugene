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

#include "SequenceViewRenderer.h"

#include <QFontMetrics>

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

CommonSequenceViewMetrics::CommonSequenceViewMetrics() {
    sequenceFont.setFamily("Courier New");
    sequenceFont.setPointSize(12);

    smallSequenceFont.setFamily("Courier New");
    smallSequenceFont.setPointSize(8);

    rulerFont.setFamily("Arial");
    rulerFont.setPointSize(8);

    QFontMetrics fm(sequenceFont);
    yCharOffset = 4;
    lineHeight = fm.boundingRect('W').height() + 2 * yCharOffset;
    xCharOffset = 1;
    charWidth = fm.boundingRect('W').width() + 2 * xCharOffset;

    QFontMetrics fms(smallSequenceFont);
    smallCharWidth = fms.boundingRect('W').width();
}

/************************************************************************/
/* SequenceViewRenderer */
/************************************************************************/
SequenceViewRenderer::SequenceViewRenderer(SequenceObjectContext* ctx)
    : ctx(ctx) {
}

int SequenceViewRenderer::posToXCoord(qint64 pos, const QSize&, const U2Region& visibleRange) const {
    CHECK(visibleRange.contains(pos) || pos == visibleRange.endPos(), -1);

    double res = (double)(pos - visibleRange.startPos) * getCurrentScale();
    return qRound(res);
}

int SequenceViewRenderer::getRowLineHeight() const {
    return commonMetrics.lineHeight;
}

}  // namespace U2
