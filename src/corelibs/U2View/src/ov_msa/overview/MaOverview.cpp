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

#include "MaOverview.h"

#include <QMouseEvent>
#include <QPainter>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "ov_msa/BaseWidthController.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MultilineScrollController.h"
#include "ov_msa/RowHeightController.h"
#include "ov_msa/ScrollController.h"

namespace U2 {

MaOverview::MaOverview(MaEditor *_editor, QWidget *_ui)
    : QWidget(_ui),
      editor(_editor),
      ui(_ui),
      stepX(0),
      stepY(0) {
    // TODO:ichebyki
    // connect(sequenceArea, SIGNAL(si_visibleRangeChanged()), this, SLOT(sl_visibleRangeChanged()));
    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_selectionChanged()));
    connect(editor->getMaObject(),
            SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)),
            SLOT(sl_redraw()));
    MaEditorMultilineWgt *wgt = qobject_cast<MaEditorMultilineWgt *>(_ui);
    if (wgt != nullptr) {
        connect(wgt->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_redraw()));
    } else {
        MaEditorWgt *wgt = qobject_cast<MaEditorWgt *>(_ui);
        if (wgt != nullptr) {
            connect(wgt->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_redraw()));
        }
    }
    connect(editor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_redraw()));
}

MaEditor* MaOverview::getEditor() const {
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

void MaOverview::mousePressEvent(QMouseEvent* me) {
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

void MaOverview::mouseMoveEvent(QMouseEvent* me) {
    if (!isValid()) {
        return;
    }

    if ((me->buttons() & Qt::LeftButton) && visibleRangeIsMoving) {
        moveVisibleRange(me->pos());
    }
    QWidget::mouseMoveEvent(me);
}

void MaOverview::mouseReleaseEvent(QMouseEvent* me) {
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
    stepX = static_cast<double>(editor->getMaEditorWgt()->getBaseWidthController()->getTotalAlignmentWidth()) / getContentWidgetWidth();
    stepY = static_cast<double>(editor->getMaEditorWgt()->getRowHeightController()->getTotalAlignmentHeight()) / getContentWidgetHeight();
}

int MaOverview::getContentWidgetWidth() const {
    return width();
}

int MaOverview::getContentWidgetHeight() const {
    return height();
}

}  // namespace U2
