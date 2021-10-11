/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MaSimpleMultilineOverview.h"

#include <QMouseEvent>
#include <QPainter>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MaGraphCalculationTask.h"
#include "ov_msa/helpers/MultilineBaseWidthController.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/MultilineScrollController.h"
#include "ov_msa/view_rendering/MaEditorSelection.h"

namespace U2 {

MaSimpleMultilineOverview::MaSimpleMultilineOverview(MaEditorMultilineWgt *ui)
    : MaMultilineOverview(ui) {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFixedHeight(FIXED_HEIGTH);
}

bool MaSimpleMultilineOverview::isValid() const {
    // The overview can be shown only if there are more on-screen pixels available than bases in the alignment. */
    return width() >= editor->getAlignmentLen() && height() >= editor->getNumSequences();
}

QPixmap MaSimpleMultilineOverview::getView() {
    resize(ui->width(), FIXED_HEIGTH);
    if (cachedMSAOverview.isNull()) {
        cachedMSAOverview = QPixmap(size());
        QPainter pOverview(&cachedMSAOverview);
        drawOverview(pOverview);
        redrawMsaOverview = false;
    }
    return cachedMSAOverview;
}

void MaSimpleMultilineOverview::sl_selectionChanged() {
    CHECK(isValid(), );
    update();
}

void MaSimpleMultilineOverview::sl_redraw() {
    redrawMsaOverview = true;
    redrawSelection = true;
    MaMultilineOverview::sl_redraw();
}

void MaSimpleMultilineOverview::sl_highlightingChanged() {
    CHECK(isValid(), );
    redrawMsaOverview = true;
    update();
}

void MaSimpleMultilineOverview::paintEvent(QPaintEvent *e) {
    if (!isValid()) {
        QPainter messagePainter(this);
        GUIUtils::showMessage(this, messagePainter, tr("Multiple sequence alignment is too big for current window size.\nSimple overview is unavailable."));
        QWidget::paintEvent(e);
        return;
    }

    if (redrawMsaOverview) {
        cachedMSAOverview = QPixmap(size());
        QPainter pOverview(&cachedMSAOverview);
        drawOverview(pOverview);
        redrawMsaOverview = false;
    }
    cachedView = cachedMSAOverview;

    QPainter cachedViewPainter(&cachedView);
    drawVisibleRange(cachedViewPainter);

    drawSelection(cachedViewPainter);
    cachedViewPainter.end();

    QPainter painter(this);
    painter.drawPixmap(0, 0, cachedView);
    QWidget::paintEvent(e);
}

void MaSimpleMultilineOverview::resizeEvent(QResizeEvent *e) {
    redrawMsaOverview = true;
    redrawSelection = true;
    QWidget::resizeEvent(e);
}

void MaSimpleMultilineOverview::drawOverview(QPainter &p) {
    p.fillRect(cachedMSAOverview.rect(), Qt::white);
    CHECK(!editor->isAlignmentEmpty(), );

    recalculateScale();

    // TODO:ichebyki
    QString highlightingSchemeId = editor->getUI()->getSequenceArea()->getCurrentHighlightingScheme()->getFactory()->getId();

    MultipleAlignmentObject *mAlignmentObj = editor->getMaObject();
    SAFE_POINT(mAlignmentObj != nullptr, tr("Incorrect multiple alignment object!"), );

    const MultipleAlignment &ma = mAlignmentObj->getMultipleAlignment();
    U2OpStatusImpl os;
    for (int seq = 0; seq < editor->getNumSequences(); seq++) {
        for (int pos = 0; pos < editor->getAlignmentLen(); pos++) {
            U2Region yRange = ui->getRowHeightController()->getGlobalYRegionByMaRowIndex(seq);
            U2Region xRange = ui->getBaseWidthController()->getBaseGlobalRange(pos);

            QRect rect;
            rect.setLeft(qRound(xRange.startPos / stepX));
            rect.setTop(qRound(yRange.startPos / stepY));
            rect.setRight(qRound(xRange.endPos() / stepX));
            rect.setBottom(qRound(yRange.endPos() / stepY));

            // TODO:ichebyki
            QColor color = editor->getUI()->getSequenceArea()->getCurrentColorScheme()->getBackgroundColor(seq, pos, mAlignmentObj->charAt(seq, pos));
            if (MaHighlightingOverviewCalculationTask::isGapScheme(highlightingSchemeId)) {
                color = Qt::gray;
            }

            bool drawColor = true;
            int refPos = -1;

            qint64 refId = editor->getReferenceRowId();
            if (refId != U2MsaRow::INVALID_ROW_ID) {
                refPos = ma->getRowIndexByRowId(refId, os);
                SAFE_POINT_OP(os, );
            }
            drawColor = MaHighlightingOverviewCalculationTask::isCellHighlighted(
                ma,
                // TODO:ichebyki
                editor->getUI()->getSequenceArea()->getCurrentHighlightingScheme(),
                // TODO:ichebyki
                editor->getUI()->getSequenceArea()->getCurrentColorScheme(),
                seq,
                pos,
                refPos);

            if (color.isValid() && drawColor) {
                p.fillRect(rect, color);
            }
        }
    }
    p.setPen(Qt::gray);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

void MaSimpleMultilineOverview::drawVisibleRange(QPainter &p) {
    if (editor->isAlignmentEmpty()) {
        setVisibleRangeForEmptyAlignment();
    } else {
        QPoint screenPosition = ui->getScrollController()->getScreenPosition();
        QSize screenSize = editor->getUI()->getSequenceArea()->size();

        cachedVisibleRange.setX(qRound(screenPosition.x() / stepX));
        cachedVisibleRange.setWidth(qRound(screenSize.width() / stepX));
        cachedVisibleRange.setY(qRound(screenPosition.y() / stepY));
        cachedVisibleRange.setHeight(qRound(screenSize.height() / stepY));

        if (cachedVisibleRange.width() < VISIBLE_RANGE_CRITICAL_SIZE || cachedVisibleRange.height() < VISIBLE_RANGE_CRITICAL_SIZE) {
            p.setPen(Qt::red);
        }
    }

    p.fillRect(cachedVisibleRange, VISIBLE_RANGE_COLOR);
    p.drawRect(cachedVisibleRange.adjusted(0, 0, -1, -1));
}

void MaSimpleMultilineOverview::drawSelection(QPainter &p) {
    const MaEditorSelection &selection = editor->getSelection();

    QList<QRect> selectedRects = selection.getRectList();
    for (const QRect &selectedRect : qAsConst(selectedRects)) {
        U2Region columnRange = ui->getBaseWidthController()->getBasesGlobalRange(selectedRect.x(), selectedRect.width());
        U2Region rowRange = U2Region::fromYRange(selectedRect);
        U2Region sequenceViewYRegion = ui->getRowHeightController()->getGlobalYRegionByViewRowsRegion(rowRange);

        QRect drawRect;
        drawRect.setLeft(qRound(columnRange.startPos / stepX));
        drawRect.setRight(qRound(columnRange.endPos() / stepX));
        drawRect.setTop(qRound(sequenceViewYRegion.startPos / stepY));
        drawRect.setBottom(qRound(sequenceViewYRegion.endPos() / stepY));
        p.fillRect(drawRect, SELECTION_COLOR);
    }
}

void MaSimpleMultilineOverview::moveVisibleRange(QPoint pos) {
    QRect newVisibleRange(cachedVisibleRange);
    int newPosX = qBound(cachedVisibleRange.width() / 2, pos.x(), width() - (cachedVisibleRange.width() - 1) / 2);
    int newPosY = qBound(cachedVisibleRange.height() / 2, pos.y(), height() - (cachedVisibleRange.height() - 1) / 2);
    QPoint newPos(newPosX, newPosY);
    newVisibleRange.moveCenter(newPos);

    int newHScrollBarValue = newVisibleRange.x() * stepX;
    ui->getScrollController()->setHScrollbarValue(newHScrollBarValue);
    int newVScrollBarValue = newVisibleRange.y() * stepY;
    ui->getScrollController()->setVScrollbarValue(newVScrollBarValue);
}

}  // namespace U2
