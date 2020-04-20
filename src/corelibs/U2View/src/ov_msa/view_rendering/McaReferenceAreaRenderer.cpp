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

#include <U2Algorithm/MsaColorScheme.h>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "McaReferenceAreaRenderer.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MSAEditorConsensusArea.h"
#include "ov_msa/MSAEditorConsensusCache.h"
#include "ov_msa/MSAEditorSequenceArea.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/ScrollController.h"
#include "ov_msa/view_rendering/MaEditorWgt.h"
#include "ov_msa/view_rendering/MaEditorSequenceArea.h"
#include "ov_sequence/SequenceObjectContext.h"
#include "ov_sequence/PanView.h"

namespace U2 {

#define SELECTION_LINE_WIDTH 1

McaReferenceAreaRenderer::McaReferenceAreaRenderer(PanView *panView, SequenceObjectContext *ctx, MaEditor *maEditor)
    : PanViewRenderer(panView, ctx),
      maEditor(maEditor)
{
    SAFE_POINT(NULL != maEditor, "MA Editor is NULL", );
    setFont(maEditor->getFont());
}

qint64 McaReferenceAreaRenderer::getMinimumHeight() const {
    return commonMetrics.lineHeight;
}

float McaReferenceAreaRenderer::posToXCoordF(const qint64 position, const QSize & /*canvasSize*/, const U2Region & /*visibleRange*/) const {
    const int baseCenterX = maEditor->getUI()->getBaseWidthController()->getBaseScreenRange(position).center();
    const int columnWidth = maEditor->getUI()->getBaseWidthController()->getBaseWidth();
    return baseCenterX - columnWidth / 2;
}

void McaReferenceAreaRenderer::setFont(const QFont &font) {
    commonMetrics.sequenceFont = font;
    QFontMetrics fm(commonMetrics.sequenceFont);
    commonMetrics.lineHeight = fm.height() + 2 * commonMetrics.yCharOffset + 2 * SELECTION_LINE_WIDTH;
}

void McaReferenceAreaRenderer::drawSequence(QPainter &p, const QSize &/*canvasSize*/, const U2Region &region) {
    U2OpStatusImpl os;
    const QByteArray sequenceRegion = ctx->getSequenceData(region, os);
    SAFE_POINT_OP(os, );

    p.setPen(Qt::black);
    p.setFont(commonMetrics.sequenceFont);

    SAFE_POINT(maEditor->getUI() != NULL, "MaEditorWgt is NULL", );
    MaEditorSequenceArea* seqArea = maEditor->getUI()->getSequenceArea();
    SAFE_POINT(seqArea != NULL, "MaEditorSequenceArea is NULL", );
    MsaColorScheme* scheme = seqArea->getCurrentColorScheme();
    SAFE_POINT(scheme != NULL, "MsaColorScheme is NULL", );

    for (int position = region.startPos; position < region.endPos(); position++) {
        const U2Region baseXRange = maEditor->getUI()->getBaseWidthController()->getBaseScreenRange(position);

        const char c = sequenceRegion[(int)(position - region.startPos)];
        QRect charRect(baseXRange.startPos, 0, baseXRange.length + 1, commonMetrics.lineHeight);
        const QColor color = scheme->getBackgroundColor(0, 0, c);
        if (color.isValid()) {
            p.fillRect(charRect, color);
        }
        if (maEditor->getResizeMode() == MSAEditor::ResizeMode_FontAndContent) {
            p.drawText(charRect, Qt::AlignCenter, QString(c));
        }
    }
}

}   // namespace U2
