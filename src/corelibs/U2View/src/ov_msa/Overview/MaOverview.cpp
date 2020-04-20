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

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MaOverview.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

MaOverview::MaOverview(MaEditorWgt *ui)
    : QWidget(ui),
      editor(ui->getEditor()),
      ui(ui),
      sequenceArea(ui->getSequenceArea()),
      stepX(0),
      stepY(0)
{
    connect(sequenceArea, SIGNAL(si_visibleRangeChanged()), this, SLOT(sl_visibleRangeChanged()));
    connect(sequenceArea, SIGNAL(si_selectionChanged(MaEditorSelection,MaEditorSelection)),
            SLOT(sl_selectionChanged()));
    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)),
            SLOT(sl_redraw()));
    connect(ui->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_redraw()));
    connect(ui->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_redraw()));
}

MaEditor *MaOverview::getEditor() const {
    return editor;
}

void MaOverview::sl_visibleRangeChanged() {
    if (!isValid()) {
        return;
    }
    update();
}

void MaOverview::sl_redraw() {
    update();
}

void MaOverview::mousePressEvent(QMouseEvent *me) {
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

void MaOverview::mouseMoveEvent(QMouseEvent *me) {
    if (!isValid()) {
        return;
    }

    if ((me->buttons() & Qt::LeftButton) && visibleRangeIsMoving) {
        moveVisibleRange(me->pos());
    }
    QWidget::mouseMoveEvent(me);
}

void MaOverview::mouseReleaseEvent(QMouseEvent *me) {
    if (!isValid()) {
        return;
    }

    if ((me->buttons() & Qt::LeftButton) && visibleRangeIsMoving) {
        visibleRangeIsMoving = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(me);
}

void MaOverview::setVisibleRangeForEmptyAlignment() {
    cachedVisibleRange = rect();
}

void MaOverview::recalculateScale() {
    stepX = static_cast<double>(ui->getBaseWidthController()->getTotalAlignmentWidth()) / getContentWidgetWidth();
    stepY = static_cast<double>(ui->getRowHeightController()->getTotalAlignmentHeight()) / getContentWidgetHeight();
}

int MaOverview::getContentWidgetWidth() const {
    return width();
}

int MaOverview::getContentWidgetHeight() const {
    return height();
}

} // namespace
