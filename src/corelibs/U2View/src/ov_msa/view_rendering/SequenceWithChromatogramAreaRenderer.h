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

#ifndef _U2_SEQUENCE_WITH_CHROMATOGRAM_AREA_RENDERER_H_
#define _U2_SEQUENCE_WITH_CHROMATOGRAM_AREA_RENDERER_H_

#include "SequenceAreaRenderer.h"

#include <U2Core/MultipleChromatogramAlignmentRow.h>

namespace U2 {

class ChromatogramViewSettings;
class McaEditorSequenceArea;

class SequenceWithChromatogramAreaRenderer : public SequenceAreaRenderer {
    Q_OBJECT
public:
    SequenceWithChromatogramAreaRenderer(MaEditorWgt *ui, McaEditorSequenceArea* seqAreaWgt);

    void drawReferenceSelection(QPainter &painter) const;
    void drawNameListSelection(QPainter &painter) const;

    void setAreaHeight(int h);
    int getAreaHeight() const;

    int getScaleBarValue() const;

    static const int INDENT_BETWEEN_ROWS;
    static const int CHROMATOGRAM_MAX_HEIGHT;

private:
    int drawRow(QPainter &painter, const MultipleAlignment &mca, int rowIndex, const U2Region &region, int xStart, int yStart) const;

    void drawChromatogram(QPainter &painter, const MultipleChromatogramAlignmentRow &row, const U2Region &visibleRange, int xStart) const;

    QColor getBaseColor(char base) const;

    void drawChromatogramTrace(const DNAChromatogram& chroma, qreal x, qreal y, qreal h, QPainter& p,
                               const U2Region& visible) const;
    void drawOriginalBaseCalls(qreal h, QPainter& p, const U2Region& visible, const QByteArray& ba) const;
    void drawQualityValues(const DNAChromatogram& chroma, qreal w, qreal h,
                           QPainter& p, const U2Region& visible, const QByteArray& ba) const;
    void drawChromatogramBaseCallsLines(const DNAChromatogram& chroma, qreal h,
                                        QPainter& p, const U2Region& visible, const QByteArray& ba) const;
private:
    McaEditorSequenceArea* getSeqArea() const;
    const ChromatogramViewSettings& getSettings() const;
    static int getChromatogramHeight();
    void completePolygonsWithLastBaseCallTrace(QPolygonF& polylineA, QPolygonF& polylineC, QPolygonF& polylineG, QPolygonF& polylineT, const DNAChromatogram& chroma, qreal columnWidth, const U2Region& visible, qreal h) const;

private:
    qreal   charWidth;
    qreal   charHeight;

    mutable int     chromaMax;
    QPen            linePen;
    int             heightPD;
    int             heightBC;
    int             heightQuality;
    int             maxTraceHeight;

    static const qreal TRACE_OR_BC_LINES_DIVIDER;
};

} // namespace

#endif // _U2_SEQUENCE_WITH_CHROMATOGRAM_AREA_RENDERER_H_
