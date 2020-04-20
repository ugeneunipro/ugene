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

#include "SequenceViewRenderer.h"

#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <QFontMetrics>


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
SequenceViewRenderer::SequenceViewRenderer(SequenceObjectContext *ctx)
    : ctx(ctx) {

}

int SequenceViewRenderer::posToXCoord(const qint64 p, const QSize &canvasSize, const U2Region &visibleRange) const {
    return qRound(posToXCoordF(p, canvasSize, visibleRange));
}

float SequenceViewRenderer::posToXCoordF(qint64 p, const QSize &canvasSize, const U2Region &visibleRange) const {
    Q_UNUSED(canvasSize);
    CHECK(visibleRange.contains(p) || p == visibleRange.endPos(), -1);

    float res = ((p - visibleRange.startPos) * getCurrentScale());
    return res;
}

qint64 SequenceViewRenderer::getRowLineHeight() const {
    return commonMetrics.lineHeight;
}

} // namespace
