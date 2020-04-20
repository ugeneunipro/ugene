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

#include <QPainter>

#include <U2Algorithm/MsaHighlightingScheme.h>
#include <U2Algorithm/MsaColorScheme.h>

#include <U2Core/U2OpStatusUtils.h>

#include "SequenceAreaRenderer.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

/*
 * Saturation of all colors in a selected region in the alignment
 * is increased by this value, if possible.
 */
const int SequenceAreaRenderer::SELECTION_SATURATION_INCREASE = 40;

SequenceAreaRenderer::SequenceAreaRenderer(MaEditorWgt *ui, MaEditorSequenceArea *seqAreaWgt)
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
    SAFE_POINT(maObj != NULL, tr("Alignment object is NULL"), false);
    const MultipleAlignment& ma = maObj->getMultipleAlignment();

    //Use dots to draw regions, which are similar to reference sequence
    highlightingScheme->setUseDots(seqAreaWgt->getUseDotsCheckedState());

    foreach (int maRow, maRows) {
        drawRow(painter, ma, maRow, columns, xStart, yStart);
        int height = ui->getRowHeightController()->getRowHeightByMaIndex(maRow);
        yStart += height;
    }

    return true;
}

#define SELECTION_STROKE_WIDTH  2

void SequenceAreaRenderer::drawSelection(QPainter &painter) const {
    QRect selectionRect = ui->getDrawHelper()->getSelectionScreenRect(seqAreaWgt->getSelection());
    int viewWidth = ui->getSequenceArea()->width();
    if (selectionRect.right() < 0 || selectionRect.left() > viewWidth) {
        return; // Selection is out of the screen.
    }

    // Check that frame has enough space to be drawn on both sides.
    if (selectionRect.left() >= 0 && selectionRect.left() < SELECTION_STROKE_WIDTH) {
        selectionRect.setLeft(SELECTION_STROKE_WIDTH);
    }
    if (selectionRect.right() <= viewWidth && selectionRect.right() + SELECTION_STROKE_WIDTH > viewWidth) {
        selectionRect.setRight(viewWidth - SELECTION_STROKE_WIDTH);
    }

    QPen pen(seqAreaWgt->selectionColor);
    if (seqAreaWgt->maMode == MaEditorSequenceArea::ViewMode) {
        pen.setStyle(Qt::DashLine);
    }
    pen.setWidth(SELECTION_STROKE_WIDTH);
    painter.setPen(pen);

    switch (seqAreaWgt->maMode) {
        case MaEditorSequenceArea::ViewMode:
        case MaEditorSequenceArea::ReplaceCharMode:
            painter.drawRect(selectionRect);
            break;
        case MaEditorSequenceArea::InsertCharMode:
            painter.drawLine(selectionRect.left(), selectionRect.top(), selectionRect.left(), selectionRect.bottom());
            break;
    }
}

void SequenceAreaRenderer::drawFocus(QPainter &painter) const {
    if (seqAreaWgt->hasFocus()) {
        painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
        painter.drawRect(0, 0, seqAreaWgt->width() - 1, seqAreaWgt->height() - 1);
    }
}

int SequenceAreaRenderer::drawRow(QPainter& painter, const MultipleAlignment& ma, int maRow, const U2Region& columns, int xStart, int yStart) const {
    // SANGER_TODO: deal with frequent handling of editor or h/color schemes through the editor etc.
    // move to class parameter
    MsaHighlightingScheme* highlightingScheme = seqAreaWgt->getCurrentHighlightingScheme();
    highlightingScheme->setUseDots(seqAreaWgt->getUseDotsCheckedState());

    MaEditor* editor = seqAreaWgt->getEditor();
    QString schemeName = highlightingScheme->metaObject()->className();
    bool isGapsScheme = schemeName == "U2::MSAHighlightingSchemeGaps";
    bool isResizeMode = editor->getResizeMode() == MSAEditor::ResizeMode_FontAndContent;

    U2OpStatusImpl os;
    const int refSeq = ma->getRowIndexByRowId(editor->getReferenceRowId(), os);
    QString refSeqName = editor->getReferenceRowName();

    qint64 regionEnd = columns.endPos() - (int)(columns.endPos() == editor->getAlignmentLen());
    const MultipleAlignmentRow& row = ma->getRow(maRow);
    const int rowHeight = ui->getRowHeightController()->getSingleRowHeight();
    const int baseWidth = ui->getBaseWidthController()->getBaseWidth();

    const MaEditorSelection& selection = seqAreaWgt->getSelection();
    U2Region selectionXRegion = selection.getXRegion();
    U2Region selectionYRegion = selection.getYRegion();
    int viewRow = ui->getCollapseModel()->getViewRowIndexByMaRowIndex(maRow);

    const QPen backupPen = painter.pen();
    for (int column = columns.startPos; column <= regionEnd; column++) {
        if (!drawLeadingAndTrailingGaps
                && (column < row->getCoreStart() || column > row->getCoreStart() + row->getCoreLength() - 1)) {
            xStart += baseWidth;
            continue;
        }

        const QRect charRect(xStart, yStart, baseWidth, rowHeight);
        char c = ma->charAt(maRow, column);

        bool highlight = false;

        QColor backgroundColor = seqAreaWgt->getCurrentColorScheme()->getBackgroundColor(maRow, column, c); //! SANGER_TODO: add NULL checks or do smt with the infrastructure
        bool isSelected =  selectionYRegion.contains(viewRow) && selectionXRegion.contains(column);
        if (backgroundColor.isValid() && isSelected) {
            backgroundColor = backgroundColor.convertTo(QColor::Hsv);
            int modifiedSaturation = qMin(backgroundColor.saturation() + SELECTION_SATURATION_INCREASE, 255);
            backgroundColor.setHsv(backgroundColor.hue(), modifiedSaturation, backgroundColor.value());
        }

        QColor fontColor = seqAreaWgt->getCurrentColorScheme()->getFontColor(maRow, column, c); //! SANGER_TODO: add NULL checks or do smt with the infrastructure
        if (isGapsScheme || highlightingScheme->getFactory()->isRefFree()) { //schemes which applied without reference
            const char refChar = '\n';
            highlightingScheme->process(refChar, c, backgroundColor, highlight, column, maRow);
        } else if (maRow == refSeq || refSeqName.isEmpty()) {
            highlight = true;
        } else {
            const char refChar = editor->getReferenceCharAt(column);
            highlightingScheme->process(refChar, c, backgroundColor, highlight, column, maRow);
        }

        if (backgroundColor.isValid() && highlight) {
            painter.fillRect(charRect, backgroundColor);
        }
        if (isResizeMode) {
            painter.setPen(fontColor);
            painter.drawText(charRect, Qt::AlignCenter, QString(c));
        }

        xStart += baseWidth;
    }
    painter.setPen(backupPen);
    return rowHeight;
}

} // namespace
