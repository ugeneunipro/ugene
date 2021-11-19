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

#include "MaSangerOverview.h"

#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GraphUtils.h>

#include "ov_mca/McaEditor.h"
#include "ov_mca/McaReferenceCharController.h"
#include "ov_msa/BaseWidthController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditorMultilineWgt.h"
#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/RowHeightController.h"
#include "ov_msa/ScrollController.h"

namespace U2 {

const int MaSangerOverview::READ_HEIGHT = 9;
const int MaSangerOverview::MINIMUM_HEIGHT = 100;
const qreal MaSangerOverview::ARROW_LINE_WIDTH = 2;
const qreal MaSangerOverview::ARROW_HEAD_WIDTH = 6;
const qreal MaSangerOverview::ARROW_HEAD_LENGTH = 7;
const QColor MaSangerOverview::ARROW_DIRECT_COLOR = "blue";
const QColor MaSangerOverview::ARROW_REVERSE_COLOR = "green";

MaSangerOverview::MaSangerOverview(MaEditor *editor, MaEditorWgt *ui)
    : MaOverview(editor, ui),
      vScrollBar(new QScrollBar(Qt::Vertical, this)),
      renderArea(new QWidget(this)),
      completeRedraw(true) {
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    mainLayout->addWidget(renderArea);
    mainLayout->addWidget(vScrollBar);
    setLayout(mainLayout);

    renderArea->installEventFilter(this);

    setMinimumHeight(MINIMUM_HEIGHT);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_updateScrollBar()));
    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_resetCaches()));
    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_completeRedraw()));
    connect(ui, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));
    connect(ui->getScrollController()->getVerticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(sl_screenMoved()));
    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), SLOT(sl_resetCaches()));
    connect(editor, SIGNAL(si_fontChanged(QFont)), SLOT(sl_resetCaches()));
    connect(vScrollBar, SIGNAL(valueChanged(int)), SLOT(sl_completeRedraw()));

    sl_updateScrollBar();
}

bool MaSangerOverview::isValid() const {
    return true;
}

QPixmap MaSangerOverview::getView() {
    if (cachedView.isNull()) {
        cachedView = QPixmap(renderArea->width(), getContentWidgetHeight() + getReferenceHeight());
    }

    if (cachedReferenceView.isNull()) {
        cachedReferenceView = QPixmap(renderArea->width(), getReferenceHeight());
    }

    if (cachedReadsView.isNull()) {
        cachedReadsView = QPixmap(renderArea->width(), getContentWidgetHeight());
    }

    if (completeRedraw) {
        QPainter painter(&cachedView);
        drawOverview(painter);
        completeRedraw = false;
    }
    return cachedView;
}

void MaSangerOverview::sl_updateScrollBar() {
    vScrollBar->setMinimum(0);
    vScrollBar->setSingleStep(3);
    const int maximum = getReadsHeight() - renderArea->height() + getReferenceHeight();
    vScrollBar->setMaximum(maximum);

    const bool prevVisibilityState = vScrollBar->isVisible();
    vScrollBar->setVisible(maximum > 0);
    const bool newVisibilityState = vScrollBar->isVisible();
    if (prevVisibilityState != newVisibilityState) {
        sl_completeRedraw();
    }
}

void MaSangerOverview::sl_completeRedraw() {
    completeRedraw = true;
    renderArea->update();
}

void MaSangerOverview::sl_resetCaches() {
    cachedReferenceHeight = -1;
    cachedView = QPixmap();
    cachedReferenceView = QPixmap();
    cachedReadsView = QPixmap();
    sl_completeRedraw();
}

void MaSangerOverview::sl_screenMoved() {
    const int screenYPosition = editor->getMaEditorWgt()->getScrollController()->getScreenPosition().y();
    const int screenHeight = editor->getMaEditorWgt()->getSequenceArea()->height();
    const int mappedTopPosition = screenYPosition / stepY;
    const int mappedBottomPosition = (screenYPosition + screenHeight) / stepY;

    if (mappedTopPosition < getScrollBarValue()) {
        vScrollBar->setValue(mappedTopPosition);
    }
    if (mappedBottomPosition > getScrollBarValue() + renderArea->height() - getReferenceHeight()) {
        vScrollBar->setValue(mappedBottomPosition - (renderArea->height() - getReferenceHeight()));
    }
}

McaEditor* MaSangerOverview::getEditor() const {
    return qobject_cast<McaEditor*>(editor);
}

int MaSangerOverview::getContentWidgetWidth() const {
    return renderArea->width();
}

int MaSangerOverview::getContentWidgetHeight() const {
    return qMax(getReadsHeight(), (vScrollBar->isVisible() ? 0 : renderArea->height()) - getReferenceHeight());
}

int MaSangerOverview::getReadsHeight() const {
    int rowsCount = editor->getCollapseModel()->getViewRowCount();
    return rowsCount * READ_HEIGHT;
}

int MaSangerOverview::getReferenceHeight() const {
    if (-1 == cachedReferenceHeight) {
        QFontMetrics fontMetrics(editor->getFont());
        return fontMetrics.height() + 2 * 2 + 4;  // Some magic. These values were taken from GraphUtils::drawRuler()
    }
    return cachedReferenceHeight;
}

int MaSangerOverview::getScrollBarValue() const {
    return vScrollBar->isVisible() ? vScrollBar->value() : 0;
}

void MaSangerOverview::resizeEvent(QResizeEvent* event) {
    sl_resetCaches();
    QWidget::resizeEvent(event);
    sl_updateScrollBar();
    sl_completeRedraw();
}

bool MaSangerOverview::eventFilter(QObject* object, QEvent* event) {
    QPaintEvent* paintEvent = dynamic_cast<QPaintEvent*>(event);
    CHECK(nullptr != paintEvent, MaOverview::eventFilter(object, event));
    if (object == renderArea) {
        QPainter painter(renderArea);
        painter.fillRect(renderArea->rect(), Qt::white);
        painter.drawPixmap(QPoint(0, 0), getView());

        drawVisibleRange(painter);
    }
    return true;
}

void MaSangerOverview::drawOverview(QPainter& painter) {
    CHECK(!editor->isAlignmentEmpty(), );
    recalculateScale();

    drawReference();
    drawReads();

    painter.drawPixmap(cachedReferenceView.rect(), cachedReferenceView);
    painter.drawPixmap(QRect(0, cachedReferenceView.height(), getContentWidgetWidth(), height() - cachedReferenceView.height()),
                       cachedReadsView,
                       QRect(0, getScrollBarValue(), cachedReadsView.width(), height() - cachedReferenceView.height()));
}

void MaSangerOverview::drawVisibleRange(QPainter& painter) {
    if (editor->isAlignmentEmpty()) {
        setVisibleRangeForEmptyAlignment();
    } else {
        recalculateScale();

        const QPoint screenPosition = editor->getMaEditorWgt()->getScrollController()->getScreenPosition();
        const QSize screenSize = editor->getMaEditorWgt()->getSequenceArea()->size();

        cachedVisibleRange.setX(qRound(screenPosition.x() / stepX));
        cachedVisibleRange.setWidth(qRound(screenSize.width() / stepX));
        cachedVisibleRange.setY(qRound(screenPosition.y() / stepY) + getReferenceHeight() - getScrollBarValue());
        cachedVisibleRange.setHeight(qMin(qRound(screenSize.height() / stepY), renderArea->height() - getReferenceHeight()));
    }

    painter.setClipRect(0, getReferenceHeight(), getContentWidgetWidth(), renderArea->height() - getReferenceHeight());
    painter.fillRect(cachedVisibleRange, VISIBLE_RANGE_COLOR);
    painter.drawRect(cachedVisibleRange.adjusted(0, 0, -1, -1));
    painter.setClipping(false);
}

void MaSangerOverview::drawReference() {
    QPainter painter(&cachedReferenceView);
    painter.fillRect(cachedReferenceView.rect(), Qt::white);

    const int referenceUngappedLength = getEditor()->getUI()->getRefCharController()->getUngappedLength();
    GraphUtils::RulerConfig config;
    config.drawArrow = false;
    config.drawNumbers = true;
    config.drawNotches = false;
    config.drawAxis = false;
    config.drawBorderNotches = false;

    GraphUtils::drawRuler(painter, QPoint(0, 0), getContentWidgetWidth() - 1, 0, referenceUngappedLength, editor->getFont(), config);

    const int yOffset = config.notchSize + QFontMetrics(editor->getFont()).height() + config.textOffset;
    config.drawNotches = true;
    config.drawNumbers = false;
    config.drawAxis = true;
    config.drawBorderNotches = true;

    GraphUtils::drawRuler(painter, QPoint(0, yOffset), getContentWidgetWidth() - 1, 0, referenceUngappedLength, editor->getFont(), config);
}

void MaSangerOverview::drawReads() {
    QPainter painter(&cachedReadsView);
    painter.fillRect(cachedReadsView.rect(), Qt::white);

    MultipleChromatogramAlignmentObject const* const mcaObject = getEditor()->getMaObject();
    SAFE_POINT(nullptr != mcaObject, tr("Incorrect multiple chromatogram alignment object"), );
    const MultipleChromatogramAlignment mca = mcaObject->getMultipleAlignment();
    const int rowsCount = editor->getCollapseModel()->getViewRowCount();

    double yOffset = 0;
    const double yStep = qMax(static_cast<double>(READ_HEIGHT), static_cast<double>(cachedReadsView.height()) / rowsCount);
    yOffset += (yStep - READ_HEIGHT) / 2;

    for (int viewRowIndex = 0; viewRowIndex < rowsCount; viewRowIndex++) {
        int maRowIndex = editor->getCollapseModel()->getMaRowIndexByViewRowIndex(viewRowIndex);
        const MultipleChromatogramAlignmentRow row = mca->getMcaRow(maRowIndex);
        const U2Region coreRegion = row->getCoreRegion();
        const U2Region positionRegion = editor->getMaEditorWgt()->getBaseWidthController()->getBasesGlobalRange(coreRegion);

        QRect readRect;
        readRect.setX(qRound(positionRegion.startPos / stepX));
        readRect.setY(qRound(yOffset));
        readRect.setHeight(READ_HEIGHT);
        readRect.setWidth(positionRegion.length / stepX);

        GraphUtils::ArrowConfig config;
        config.lineWidth = ARROW_LINE_WIDTH;
        config.lineLength = readRect.width();
        config.arrowHeadWidth = ARROW_HEAD_WIDTH;
        config.arrowHeadLength = ARROW_HEAD_LENGTH;
        config.color = row->isReversed() ? ARROW_REVERSE_COLOR : ARROW_DIRECT_COLOR;
        config.direction = row->isReversed() ? GraphUtils::RightToLeft : GraphUtils::LeftToRight;
        GraphUtils::drawArrow(painter, readRect, config);

        yOffset += yStep;
    }
}

void MaSangerOverview::moveVisibleRange(QPoint pos) {
    QRect newVisibleRange(cachedVisibleRange);
    const int newPosX = qBound((cachedVisibleRange.width() - 1) / 2, pos.x(), width() - (cachedVisibleRange.width() - 1) / 2);
    const int newPosY = qBound(getReferenceHeight() + (cachedVisibleRange.height() - 1) / 2, pos.y(), height() - (cachedVisibleRange.height() - 1) / 2);
    const QPoint newPos(newPosX, newPosY);
    newVisibleRange.moveCenter(newPos);

    if (pos.y() < newPosY && 0 < getScrollBarValue()) {
        vScrollBar->triggerAction(QScrollBar::SliderSingleStepSub);
    }
    if (newPosY < pos.y() && getScrollBarValue() < vScrollBar->maximum()) {
        vScrollBar->triggerAction(QScrollBar::SliderSingleStepAdd);
    }

    const int newHScrollBarValue = newVisibleRange.x() * stepX;
    editor->getMaEditorWgt()->getScrollController()->setHScrollbarValue(newHScrollBarValue);
    const int newVScrollBarValue = (newVisibleRange.y() - getReferenceHeight() + getScrollBarValue()) * stepY;
    editor->getMaEditorWgt()->getScrollController()->setVScrollbarValue(newVScrollBarValue);
}

}  // namespace U2
