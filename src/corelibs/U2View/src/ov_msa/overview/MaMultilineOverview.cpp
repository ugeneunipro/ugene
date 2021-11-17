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

#include "MaMultilineOverview.h"

#include <QMouseEvent>
#include <QPainter>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"
#include "ov_msa/helpers/MultilineScrollController.h"
#include "ov_msa/view_rendering/MaEditorSelection.h"

namespace U2 {

MaMultilineOverview::MaMultilineOverview(MaEditorMultilineWgt *ui)
    : QWidget(ui),
      editor(ui->getEditor()),
      ui(ui),
      stepX(0),
      stepY(0) {
    // TODO:ichebyki
    // sequenceArea ?
    //connect(ui->getUI(0)->getSequenceArea(), SIGNAL(si_visibleRangeChanged()), this, SLOT(sl_visibleRangeChanged()));
    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection &, const MaEditorSelection &)),
            SLOT(sl_selectionChanged()));
    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), SLOT(sl_redraw()));
    // TODO:ichebyki
    // scroll ?
    connect(ui->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_redraw()));
    connect(editor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_redraw()));
}

MaEditor *MaMultilineOverview::getEditor() const {
    return editor;
}

void MaMultilineOverview::sl_visibleRangeChanged() {
    if (!isValid()) {
        return;
    }
    update();
}

void MaMultilineOverview::sl_redraw() {
    update();
}

void MaMultilineOverview::mousePressEvent(QMouseEvent *me) {
    if (!isValid()) {
        return;
    }

    if (me->buttons() == Qt::LeftButton) {
        visibleRangeIsMoving = true;
        setCursor(Qt::ClosedHandCursor);
        moveVisibleRange(me->pos());
    }
    QWidget::mousePressEvent(me);
}

void MaMultilineOverview::mouseMoveEvent(QMouseEvent *me) {
    if (!isValid()) {
        return;
    }

    if ((me->buttons() & Qt::LeftButton) && visibleRangeIsMoving) {
        moveVisibleRange(me->pos());
    }
    QWidget::mouseMoveEvent(me);
}

void MaMultilineOverview::mouseReleaseEvent(QMouseEvent *me) {
    if (!isValid()) {
        return;
    }

    if ((me->buttons() & Qt::LeftButton) && visibleRangeIsMoving) {
        visibleRangeIsMoving = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(me);
}

void MaMultilineOverview::setVisibleRangeForEmptyAlignment() {
    cachedVisibleRange = rect();
}

void MaMultilineOverview::recalculateScale() {
    // TODO:ichebyki
    // Is it right ?
    stepX = static_cast<double>(editor->getMaEditorWgt()->getBaseWidthController()->getTotalAlignmentWidth()) / getContentWidgetWidth();
    stepY = static_cast<double>(editor->getMaEditorWgt()->getRowHeightController()->getTotalAlignmentHeight()) / getContentWidgetHeight();
}

int MaMultilineOverview::getContentWidgetWidth() const {
    return width();
}

int MaMultilineOverview::getContentWidgetHeight() const {
    return height();
}

}  // namespace U2
