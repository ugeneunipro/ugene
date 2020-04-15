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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/SignalBlocker.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/SequenceObjectContext.h>

#include "McaEditor.h"
#include "McaEditorConsensusArea.h"
#include "McaEditorReferenceArea.h"
#include "McaEditorSequenceArea.h"
#include "MSAEditorConsensusArea.h"
#include "helpers/DrawHelper.h"
#include "helpers/ScrollController.h"
#include "ov_msa/helpers/BaseWidthController.h"

#include <QApplication>

namespace U2 {

McaEditorReferenceArea::McaEditorReferenceArea(McaEditorWgt *ui, SequenceObjectContext *ctx)
    : PanView(ui, ctx, McaEditorReferenceRenderAreaFactory(ui, NULL != ui ? ui->getEditor() : NULL)),
    editor(NULL != ui ? ui->getEditor() : NULL),
    ui(ui),
    renderer(dynamic_cast<McaReferenceAreaRenderer *>(getRenderArea()->getRenderer())),
    firstPressedSelectionPosition(-1)
{
    SAFE_POINT(NULL != renderer, "Renderer is NULL", );

    setObjectName("mca_editor_reference_area");
    singleBaseSelection = true;
    setLocalToolbarVisible(false);
    settings->showMainRuler = false;

    scrollBar->hide();
    rowBar->hide();

    connect(ui->getEditor()->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)),
            SLOT(completeUpdate()));

    connect(ui->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_visibleRangeChanged()));
    connect(ctx->getSequenceSelection(),
        SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
        SLOT(sl_onSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));

    connect(this, SIGNAL(si_selectionChanged()),
            ui->getSequenceArea(), SLOT(sl_backgroundSelectionChanged()));
    connect(editor, SIGNAL(si_fontChanged(const QFont &)), SLOT(sl_fontChanged(const QFont &)));

    connect(ui->getConsensusArea(), SIGNAL(si_mismatchRedrawRequired()), SLOT(completeUpdate()));
    connect(scrollBar, SIGNAL(valueChanged(int)), ui->getScrollController()->getHorizontalScrollBar(), SLOT(setValue(int)));
    connect(ui->getScrollController()->getHorizontalScrollBar(), SIGNAL(valueChanged(int)), scrollBar, SLOT(setValue(int)));
    connect(ui, SIGNAL(si_clearSelection()), SLOT(sl_clearSelection()));
    connect(ui->getSequenceArea(), SIGNAL(si_clearReferenceSelection()),
            SLOT(sl_clearSelection()));
    connect(ui->getSequenceArea(), SIGNAL(si_selectionChanged(MaEditorSelection, MaEditorSelection)),
            SLOT(sl_selectionChanged(MaEditorSelection, MaEditorSelection)));

    setMouseTracking(false);

    sl_fontChanged(editor->getFont());
}

void McaEditorReferenceArea::sl_selectMismatch(int pos) {
    MaEditorSequenceArea* seqArea = ui->getSequenceArea();
    if (seqArea->getFirstVisibleBase() > pos || seqArea->getLastVisibleBase(false) < pos) {
        seqArea->centerPos(pos);
    }
    seqArea->sl_cancelSelection();
    setSelection(U2Region(pos, 1));
}

void McaEditorReferenceArea::sl_visibleRangeChanged() {
    const U2Region visibleRange = ui->getDrawHelper()->getVisibleBases(ui->getSequenceArea()->width());
    setVisibleRange(visibleRange);
    update();
}

void McaEditorReferenceArea::sl_selectionChanged(const MaEditorSelection &current, const MaEditorSelection &) {
    U2Region selection(current.x(), current.width());
    setSelection(selection);
}

void McaEditorReferenceArea::sl_clearSelection() {
    ctx->getSequenceSelection()->clear();
}

void McaEditorReferenceArea::sl_fontChanged(const QFont &newFont) {
    renderer->setFont(newFont);
    setFixedHeight(renderer->getMinimumHeight());
}

void McaEditorReferenceArea::mousePressEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) {
        Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
        const bool isShiftPressed = km.testFlag(Qt::ShiftModifier);
        if (!isShiftPressed) {
            firstPressedSelectionPosition = -1;
            emit editor->si_clearSelection();
        }
    } else {
        PanView::mousePressEvent(e);
    }
}

void McaEditorReferenceArea::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) {
        setReferenceSelection(e);
        e->accept();
    } else {
        PanView::mouseMoveEvent(e);
    }
}

void McaEditorReferenceArea::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        setReferenceSelection(e);
        e->accept();
    } else {
        PanView::mouseReleaseEvent(e);
    }
}

void McaEditorReferenceArea::keyPressEvent(QKeyEvent *event) {
    const int key = event->key();
    bool accepted = false;
    DNASequenceSelection * const selection = ctx->getSequenceSelection();
    U2Region selectedRegion = (NULL != selection && !selection->isEmpty() ? selection->getSelectedRegions().first() : U2Region());
    const qint64 selectionEndPos = selectedRegion.endPos() - 1;
    Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    const bool isShiftPressed = km.testFlag(Qt::ShiftModifier);
    qint64 baseToScroll = firstPressedSelectionPosition;

    switch(key) {
    case Qt::Key_Left:
        if (!selectedRegion.isEmpty() && selectedRegion.startPos > 0) {
            if (isShiftPressed) {
                if (selectionEndPos == firstPressedSelectionPosition) {
                    selectedRegion.startPos--;
                    selectedRegion.length++;
                    baseToScroll = selectedRegion.startPos;
                } else if (selectedRegion.startPos == firstPressedSelectionPosition) {
                    selectedRegion.length--;
                    baseToScroll = selectionEndPos;
                } else {
                    assert(false);
                }
            } else {
                selectedRegion.startPos--;
                firstPressedSelectionPosition--;
                baseToScroll = selectedRegion.startPos;
            }
            ctx->getSequenceSelection()->setSelectedRegions(QVector<U2Region>() << selectedRegion);
            ui->getScrollController()->scrollToBase(baseToScroll, width());
        }
        accepted = true;
        break;
    case Qt::Key_Up:
        accepted = true;
        break;
    case Qt::Key_Right:
        if (!selectedRegion.isEmpty() && selectionEndPos + 1 < ctx->getSequenceLength()) {
            if (isShiftPressed) {
                if (selectedRegion.startPos == firstPressedSelectionPosition) {
                    selectedRegion.length++;
                    baseToScroll = selectionEndPos;
                } else if (selectionEndPos == firstPressedSelectionPosition) {
                    selectedRegion.startPos++;
                    selectedRegion.length--;
                    baseToScroll = selectedRegion.startPos;
                } else {
                    assert(false);
                }
            } else {
                selectedRegion.startPos++;
                firstPressedSelectionPosition++;
                baseToScroll = selectionEndPos + 1;
            }
            ctx->getSequenceSelection()->setSelectedRegions(QVector<U2Region>() << selectedRegion);
            ui->getScrollController()->scrollToBase(baseToScroll, width());
        }
        accepted = true;
        break;
    case Qt::Key_Down:
        accepted = true;
        break;
    case Qt::Key_Home:
        ui->getScrollController()->scrollToEnd(ScrollController::Left);
        accepted = true;
        break;
    case Qt::Key_End:
        ui->getScrollController()->scrollToEnd(ScrollController::Right);
        accepted = true;
        break;
    case Qt::Key_PageUp:
        ui->getScrollController()->scrollPage(ScrollController::Left);
        accepted = true;
        break;
    case Qt::Key_PageDown:
        ui->getScrollController()->scrollPage(ScrollController::Right);
        accepted = true;
        break;
    }


    if (accepted) {
        event->accept();
    } else {
        PanView::keyPressEvent(event);
    }
}

void McaEditorReferenceArea::setReferenceSelection(QMouseEvent* e) {
    QPoint p = e->pos();
    QPoint areaPoint = toRenderAreaPoint(p);
    qint64 pos = renderArea->coordToPos(areaPoint);
    qint64 start = 0;
    qint64 count = 0;
    if (firstPressedSelectionPosition != -1) {
        start = qMin(pos, firstPressedSelectionPosition);
        count = qAbs(pos - firstPressedSelectionPosition) + 1;
    } else {
        firstPressedSelectionPosition = pos;
        start = pos;
        count = 1;
    }
    U2Region reg(start, count);
    setSelection(reg);
}

void McaEditorReferenceArea::updateScrollBar() {
    SignalBlocker signalBlocker(scrollBar);
    Q_UNUSED(signalBlocker);

    const QScrollBar * const hScrollbar = ui->getScrollController()->getHorizontalScrollBar();

    scrollBar->setMinimum(hScrollbar->minimum());
    scrollBar->setMaximum(hScrollbar->maximum());
    scrollBar->setSliderPosition(hScrollbar->value());
    scrollBar->setSingleStep(hScrollbar->singleStep());
    scrollBar->setPageStep(hScrollbar->pageStep());
}

void McaEditorReferenceArea::sl_onSelectionChanged(LRegionsSelection * /*selection*/, const QVector<U2Region> &addedRegions, const QVector<U2Region> &removedRegions) {
    if (addedRegions.size() == 1) {
        const U2Region addedRegion = addedRegions.first();
        qint64 baseToScrollTo = -1;
        if (removedRegions.size() == 1) {
            if (removedRegions.first() == addedRegions.first()) {
                int hSchrollValue = ui->getScrollController()->getHorizontalScrollBar()->value();
                ui->getScrollController()->setHScrollbarValue(hSchrollValue);
            } else {
                const U2Region removedRegion = removedRegions.first();
                if (addedRegion.startPos == removedRegion.startPos || addedRegion.startPos == removedRegion.endPos() - 1) {
                    baseToScrollTo = addedRegion.endPos() - 1;
                } else {
                    baseToScrollTo = addedRegion.startPos;
                }
            }
        } else {
            baseToScrollTo = addedRegion.startPos;
        }
        if (baseToScrollTo != -1) {
            ui->getScrollController()->scrollToBase(static_cast<int>(baseToScrollTo), width());
        }
    }
    emit si_selectionChanged();
}

McaEditorReferenceRenderArea::McaEditorReferenceRenderArea(McaEditorWgt *_ui, PanView *d, PanViewRenderer *renderer)
    : PanViewRenderArea(d, renderer),
    ui(_ui) {
}

qint64 McaEditorReferenceRenderArea::coordToPos(int x) const {
    qint64 res = 0;
    if (ui != NULL) {
        res = qBound(0, ui->getBaseWidthController()->screenXPositionToColumn(x), ui->getEditor()->getAlignmentLen());
    }
    return res;
}

McaEditorReferenceRenderAreaFactory::McaEditorReferenceRenderAreaFactory(McaEditorWgt *_ui, McaEditor *_editor)
    : PanViewRenderAreaFactory(),
    ui(_ui),
    maEditor(_editor) {

}

PanViewRenderArea * McaEditorReferenceRenderAreaFactory::createRenderArea(PanView *panView) const {
    return new McaEditorReferenceRenderArea(ui, panView, new McaReferenceAreaRenderer(panView, panView->getSequenceContext(), maEditor));
}

}   // namespace U2
