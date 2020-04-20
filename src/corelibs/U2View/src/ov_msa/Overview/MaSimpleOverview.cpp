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

#include <QMouseEvent>
#include <QPainter>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MaGraphCalculationTask.h"
#include "MaSimpleOverview.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

MaSimpleOverview::MaSimpleOverview(MaEditorWgt *_ui)
    : MaOverview(_ui),
      redrawMsaOverview(true),
      redrawSelection(true)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFixedHeight(FIXED_HEIGTH);

    setVisible(false);
}

bool MaSimpleOverview::isValid() const {
    if (width() < editor->getAlignmentLen() || height() < editor->getNumSequences()) {
        return false;
    }
    return true;
}


QPixmap MaSimpleOverview::getView() {
    resize(ui->width(), FIXED_HEIGTH);
    if (cachedMSAOverview.isNull()) {
        cachedMSAOverview = QPixmap(size());
        QPainter pOverview(&cachedMSAOverview);
        drawOverview(pOverview);
        redrawMsaOverview = false;
    }
    return cachedMSAOverview;
}

void MaSimpleOverview::sl_selectionChanged() {
    if (!isValid()) {
        return;
    }
    redrawSelection = true;

    update();
}

void MaSimpleOverview::sl_redraw() {
    redrawMsaOverview = true;
    redrawSelection = true;
    MaOverview::sl_redraw();
}

void MaSimpleOverview::sl_highlightingChanged() {
    if (!isValid()) {
        return;
    }
    redrawMsaOverview = true;
    update();
}

void MaSimpleOverview::paintEvent(QPaintEvent *e) {
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

    QPainter pVisibleRange(&cachedView);
    drawVisibleRange(pVisibleRange);
    pVisibleRange.end();

    if (redrawSelection) {
        recalculateSelection();
        redrawSelection = false;
    }

    QPainter pSelection(&cachedView);
    drawSelection(pSelection);
    pSelection.end();

    QPainter p(this);
    p.drawPixmap(0, 0, cachedView);
    QWidget::paintEvent(e);
}

void MaSimpleOverview::resizeEvent(QResizeEvent *e) {
    redrawMsaOverview = true;
    redrawSelection = true;
    QWidget::resizeEvent(e);
}

void MaSimpleOverview::drawOverview(QPainter &p) {
    p.fillRect(cachedMSAOverview.rect(), Qt::white);

    if (editor->isAlignmentEmpty()) {
        return;
    }

    recalculateScale();

    QString highlightingSchemeId = sequenceArea->getCurrentHighlightingScheme()->getFactory()->getId();

    MultipleAlignmentObject* mAlignmentObj = editor->getMaObject();
    SAFE_POINT(NULL != mAlignmentObj, tr("Incorrect multiple alignment object!"), );
    const MultipleAlignment ma = mAlignmentObj->getMultipleAlignment();

    U2OpStatusImpl os;
    for (int seq = 0; seq < editor->getNumSequences(); seq++) {
        for (int pos = 0; pos < editor->getAlignmentLen(); pos++) {
            U2Region yRange = ui->getRowHeightController()->getGlobalYRegionByMaRowIndex(seq);
            U2Region xRange = ui->getBaseWidthController()->getBaseGlobalRange(pos);

            QRect rect;
            rect.setX(qRound(xRange.startPos / stepX));
            rect.setY(qRound(yRange.startPos / stepY));
            rect.setWidth(qRound(xRange.length / stepX));
            rect.setHeight(qRound(yRange.length / stepY));

            QColor color = sequenceArea->getCurrentColorScheme()->getBackgroundColor(seq, pos, mAlignmentObj->charAt(seq, pos));
            if (MaHighlightingOverviewCalculationTask::isGapScheme(highlightingSchemeId)) {
                color = Qt::gray;
            }

            bool drawColor = true;
            int refPos = -1;;
            qint64 refId = editor->getReferenceRowId();
            if (refId != U2MsaRow::INVALID_ROW_ID) {
                refPos = ma->getRowIndexByRowId(refId, os);
                SAFE_POINT_OP(os, );
            }
            drawColor = MaHighlightingOverviewCalculationTask::isCellHighlighted(
                        ma,
                        sequenceArea->getCurrentHighlightingScheme(),
                        sequenceArea->getCurrentColorScheme(),
                        seq, pos,
                        refPos);

            if (color.isValid() && drawColor) {
                p.fillRect(rect, color);
            }
        }
    }
    p.setPen(Qt::gray);
    p.drawRect( rect().adjusted(0, 0, -1, -1) );
}

void MaSimpleOverview::drawVisibleRange(QPainter &p) {
    if (editor->isAlignmentEmpty()) {
        setVisibleRangeForEmptyAlignment();
    } else {
        const QPoint screenPosition = editor->getUI()->getScrollController()->getScreenPosition();
        const QSize screenSize = editor->getUI()->getSequenceArea()->size();

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

void MaSimpleOverview::drawSelection(QPainter &p) {
    p.fillRect(cachedSelection, SELECTION_COLOR);
}

void MaSimpleOverview::moveVisibleRange(QPoint _pos) {
    QRect newVisibleRange(cachedVisibleRange);
    const int newPosX = qBound((cachedVisibleRange.width()) / 2, _pos.x(), width() - (cachedVisibleRange.width() - 1 ) / 2);
    const int newPosY = qBound((cachedVisibleRange.height()) / 2, _pos.y(), height() - (cachedVisibleRange.height() - 1 ) / 2);
    const QPoint newPos(newPosX, newPosY);
    newVisibleRange.moveCenter(newPos);

    const int newHScrollBarValue = newVisibleRange.x() * stepX;
    ui->getScrollController()->setHScrollbarValue(newHScrollBarValue);
    const int newVScrollBarValue = newVisibleRange.y() * stepY;
    ui->getScrollController()->setVScrollbarValue(newVScrollBarValue);
}

void MaSimpleOverview::recalculateSelection() {
    recalculateScale();

    const MaEditorSelection& selection = sequenceArea->getSelection();

    const U2Region selectionBasesRegion = ui->getBaseWidthController()->getBasesGlobalRange(selection.x(), selection.width());
    const U2Region selectionRowsRegion = ui->getRowHeightController()->getGlobalYRegionByViewRowsRegion(selection.getYRegion());

    cachedSelection.setX(qRound(selectionBasesRegion.startPos / stepX));
    cachedSelection.setY(qRound(selectionRowsRegion.startPos / stepY));

    //(!) [(a - b)*c] != [a*c] - [b*c]
    cachedSelection.setWidth(qRound((selectionBasesRegion.startPos + selectionBasesRegion.length) / stepX) - qRound(selectionBasesRegion.startPos / stepX));
    cachedSelection.setHeight(qRound((selectionRowsRegion.startPos + selectionRowsRegion.length) / stepY) - qRound(selectionRowsRegion.startPos / stepY));
}

} // namespace
