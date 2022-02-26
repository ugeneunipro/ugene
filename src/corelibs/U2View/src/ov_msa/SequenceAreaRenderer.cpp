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

#include "SequenceAreaRenderer.h"

#include <QPainter>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/U2OpStatusUtils.h>

#include "ov_msa/BaseWidthController.h"
#include "ov_msa/DrawHelper.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/RowHeightController.h"
#include "ov_msa/ScrollController.h"

namespace U2 {

/*
 * Saturation of all colors in a selected region in the alignment
 * is increased by this value, if possible.
 */
const int SequenceAreaRenderer::SELECTION_SATURATION_INCREASE = 40;

SequenceAreaRenderer::SequenceAreaRenderer(MaEditorWgt* ui, MaEditorSequenceArea* seqAreaWgt)
    : QObject(seqAreaWgt),
      ui(ui),
      seqAreaWgt(seqAreaWgt),
      drawLeadingAndTrailingGaps(true) {
}

bool SequenceAreaRenderer::drawContent(QPainter& painter, const U2Region& columns, const QList<int>& maRows, int xStart, int yStart) const {
    CHECK(!columns.isEmpty(), false);
    CHECK(!maRows.isEmpty(), false);

    MsaHighlightingScheme* highlightingScheme = seqAreaWgt->getCurrentHighlightingScheme();
    MaEditor* editor = seqAreaWgt->getEditor();

    painter.setPen(Qt::black);
    painter.setFont(editor->getFont());

    MultipleAlignmentObject* maObj = editor->getMaObject();
    SAFE_POINT(maObj != nullptr, tr("Alignment object is NULL"), false);
    const MultipleAlignment& ma = maObj->getMultipleAlignment();

    // Use dots to draw regions, which are similar to reference sequence
    highlightingScheme->setUseDots(seqAreaWgt->getUseDotsCheckedState());

    foreach (int maRow, maRows) {
        drawRow(painter, ma, maRow, columns, xStart, yStart);
        int height = ui->getRowHeightController()->getRowHeightByMaIndex(maRow);
        yStart += height;
    }

    return true;
}

#define SELECTION_STROKE_WIDTH 2

void SequenceAreaRenderer::drawSelectionFrame(QPainter& painter) const {
    const MaEditorSelection& selection = seqAreaWgt->getEditor()->getSelection();
    CHECK(!selection.isEmpty(), );

    painter.save();
    QPen pen(seqAreaWgt->selectionColor);
    pen.setWidth(SELECTION_STROKE_WIDTH);
    if (seqAreaWgt->maMode == MaEditorSequenceArea::ViewMode) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);

    const QList<QRect> selectionRects = selection.getRectList();
    for (const QRect& selectionRect : qAsConst(selectionRects)) {
        QRect screenRect = ui->getDrawHelper()->getScreenRect(selectionRect);
        int viewWidth = ui->getSequenceArea()->width();
        if (screenRect.right() < 0 || screenRect.left() > viewWidth) {
            continue;  // Selection is out of the screen.
        }

        // Check that frame has enough space to be drawn on both sides.
        if (screenRect.left() >= 0 && screenRect.left() < SELECTION_STROKE_WIDTH) {
            screenRect.setLeft(SELECTION_STROKE_WIDTH);
        }
        if (screenRect.right() <= viewWidth && screenRect.right() + SELECTION_STROKE_WIDTH > viewWidth) {
            screenRect.setRight(viewWidth - SELECTION_STROKE_WIDTH);
        }
        switch (seqAreaWgt->maMode) {
            case MaEditorSequenceArea::ViewMode:
            case MaEditorSequenceArea::ReplaceCharMode:
                painter.drawRect(screenRect);
                break;
            case MaEditorSequenceArea::InsertCharMode:
                painter.drawLine(screenRect.left(), screenRect.top(), screenRect.left(), screenRect.bottom());
                break;
        }
    }
    painter.restore();
}

void SequenceAreaRenderer::drawFocus(QPainter& painter) const {
    if (seqAreaWgt->hasFocus()) {
        painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
        painter.drawRect(0, 0, seqAreaWgt->width() - 1, seqAreaWgt->height() - 1);
    }
}

int SequenceAreaRenderer::drawRow(QPainter& painter, const MultipleAlignment& ma, int maRowIndex, const U2Region& columns, int xStart, int yStart) const {
    // SANGER_TODO: deal with frequent handling of editor or h/color schemes through the editor etc.
    // move to class parameter
    MsaHighlightingScheme* highlightingScheme = seqAreaWgt->getCurrentHighlightingScheme();
    highlightingScheme->setUseDots(seqAreaWgt->getUseDotsCheckedState());

    MaEditor* editor = seqAreaWgt->getEditor();
    QString schemeName = highlightingScheme->metaObject()->className();
    bool isGapsScheme = schemeName == "U2::MSAHighlightingSchemeGaps";
    bool isResizeMode = editor->getResizeMode() == MSAEditor::ResizeMode_FontAndContent;

    U2OpStatusImpl os;
    int referenceMaRowIndex = ma->getRowIndexByRowId(editor->getReferenceRowId(), os);
    bool isReferenceRow = referenceMaRowIndex == maRowIndex;
    bool hasReference = referenceMaRowIndex >= 0;

    qint64 regionEnd = qMin(columns.endPos(), (qint64)editor->getAlignmentLen());
    const MultipleAlignmentRow& maRow = ma->getRow(maRowIndex);
    int rowHeight = ui->getRowHeightController()->getSingleRowHeight();
    int baseWidth = ui->getBaseWidthController()->getBaseWidth();

    int viewRowIndex = editor->getCollapseModel()->getViewRowIndexByMaRowIndex(maRowIndex);

    painter.save();
    // regionEnd equals startPos + length, so we use '<' instead of '<='
    for (int column = columns.startPos; column < regionEnd; column++) {
        if (!drawLeadingAndTrailingGaps && (column < maRow->getCoreStart() || column > maRow->getCoreStart() + maRow->getCoreLength() - 1)) {
            xStart += baseWidth;
            continue;
        }

        QRect charRect(xStart, yStart, baseWidth, rowHeight);
        char c = ma->charAt(maRowIndex, column);

        QColor backgroundColor = seqAreaWgt->getCurrentColorScheme()->getBackgroundColor(maRowIndex, column, c);  //! SANGER_TODO: add NULL checks or do smt with the infrastructure
        if (backgroundColor.isValid() && hasHighlightedBackground(column, viewRowIndex)) {
            backgroundColor = backgroundColor.convertTo(QColor::Hsv);
            int modifiedSaturation = qMin(backgroundColor.saturation() + SELECTION_SATURATION_INCREASE, 255);
            backgroundColor.setHsv(backgroundColor.hue(), modifiedSaturation, backgroundColor.value());
        }

        bool highlight = false;
        if (isGapsScheme || highlightingScheme->getFactory()->isRefFree()) {  // schemes which applied without reference
            char refChar = '\n';
            highlightingScheme->process(refChar, c, backgroundColor, highlight, column, maRowIndex);
        } else if (isReferenceRow || !hasReference) {
            highlight = true;
        } else {
            char refChar = editor->getReferenceCharAt(column);
            highlightingScheme->process(refChar, c, backgroundColor, highlight, column, maRowIndex);
        }

        if (backgroundColor.isValid() && highlight) {
            painter.fillRect(charRect, backgroundColor);
        }
        if (isResizeMode) {
            QColor fontColor = seqAreaWgt->getCurrentColorScheme()->getFontColor(maRowIndex, column, c);  //! SANGER_TODO: add NULL checks or do smt with the infrastructure
            painter.setPen(fontColor);
            painter.drawText(charRect, Qt::AlignCenter, QString(c));
        }

        xStart += baseWidth;
    }
    painter.restore();
    return rowHeight;
}

bool SequenceAreaRenderer::hasHighlightedBackground(int columnIndex, int viewRowIndex) const {
    return ui->getEditor()->getSelection().contains(columnIndex, viewRowIndex);
}

}  // namespace U2
