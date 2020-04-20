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

#include <U2Gui/GraphUtils.h>

#include "McaEditor.h"
#include "McaEditorNameList.h"
#include "McaEditorSequenceArea.h"
#include "helpers/RowHeightController.h"
#include "view_rendering/MaEditorSelection.h"

namespace U2 {

const int McaEditorNameList::MARGIN_ARROW_LEFT = 5;
const int McaEditorNameList::MARGIN_ARROW_RIGHT = 5;
const qreal McaEditorNameList::ARROW_LINE_WIDTH = 2;
const qreal McaEditorNameList::ARROW_LENGTH = 25;
const qreal McaEditorNameList::ARROW_HEAD_WIDTH = 6;
const qreal McaEditorNameList::ARROW_HEAD_LENGTH = 7;
const QColor McaEditorNameList::ARROW_DIRECT_COLOR = "blue"; // another possible color: "#4EADE1";
const QColor McaEditorNameList::ARROW_REVERSE_COLOR = "green"; // another possible color: "#03c03c";

McaEditorNameList::McaEditorNameList(McaEditorWgt *ui, QScrollBar *nhBar)
    : MaEditorNameList(ui, nhBar)
{
    setObjectName("mca_editor_name_list");

    editSequenceNameAction->setText(tr("Rename read"));
    editSequenceNameAction->setShortcut(Qt::Key_F2);

    removeSequenceAction->setText(tr("Remove read"));

    setMinimumWidth(getMinimumWidgetWidth());
}

void McaEditorNameList::sl_selectionChanged(const MaEditorSelection& current, const MaEditorSelection & /*oldSelection*/) {
    sl_updateActions();
    sl_completeRedraw();
    emit si_selectionChanged();
}

void McaEditorNameList::sl_updateActions() {
    MaEditorNameList::sl_updateActions();

    U2Region selection = getSelection();
    const bool hasSequenceSelection = !selection.isEmpty();
    const bool hasRowSelection = !selection.isEmpty();
    const bool isWholeReadSelected = hasRowSelection && !hasSequenceSelection;

    removeSequenceAction->setShortcut(isWholeReadSelected ? QKeySequence::Delete : QKeySequence());
}

void McaEditorNameList::drawCollapsibleSequenceItem(QPainter &painter, int rowIndex, const QString &name, const QRect &rect,
                                                    bool isSelected, bool isCollapsed, bool isReference) {
    const bool isReversed = isRowReversed(rowIndex);
    const QRectF arrowRect = calculateArrowRect(U2Region(rect.y(), rect.height()));
    MaEditorNameList::drawCollapsibleSequenceItem(painter, rowIndex, name, rect, isSelected, isCollapsed, isReference);
    drawArrow(painter, isReversed, arrowRect);
}

void McaEditorNameList::setSelection(int startSeq, int count) {
    ui->getSequenceArea()->setSelection(MaEditorSelection(0, startSeq, 0, count));
}

McaEditor* McaEditorNameList::getEditor() const {
    return qobject_cast<McaEditor*>(editor);
}

bool McaEditorNameList::isRowReversed(int rowIndex) const {
    return getEditor()->getMaObject()->getMcaRow(rowIndex)->isReversed();
}

void McaEditorNameList::drawText(QPainter &painter, const QString &text, const QRect &rect, bool selected) {
    const QFontMetrics fontMetrics(getFont(selected));
    const QString elidedText = fontMetrics.elidedText(text, Qt::ElideRight, rect.width());
    MaEditorNameList::drawText(painter, elidedText, rect, selected);
}

void McaEditorNameList::drawArrow(QPainter &painter, bool isReversed, const QRectF &arrowRect) {
    GraphUtils::ArrowConfig config;
    config.lineWidth = ARROW_LINE_WIDTH;
    config.lineLength = arrowRect.width();
    config.arrowHeadWidth = ARROW_HEAD_WIDTH;
    config.arrowHeadLength = ARROW_HEAD_LENGTH;
    config.color = isReversed ? ARROW_REVERSE_COLOR : ARROW_DIRECT_COLOR;
    config.direction = isReversed ? GraphUtils::RightToLeft : GraphUtils::LeftToRight;
    GraphUtils::drawArrow(painter, arrowRect, config);
}

QRectF McaEditorNameList::calculateArrowRect(const U2Region &yRange) const {
    const int widgetWidth = width();
    const qreal arrowWidth = ARROW_LENGTH;
    const qreal arrowHeight = ARROW_HEAD_LENGTH;
    const qreal arrowX = widgetWidth - arrowWidth - MARGIN_ARROW_RIGHT;
    const qreal arrowY = yRange.startPos + (qreal)(ui->getRowHeightController()->getSingleRowHeight() - arrowHeight) / 2;
    return QRectF(arrowX, arrowY, arrowWidth, arrowHeight);
}

int McaEditorNameList::getAvailableWidth() const {
    return MaEditorNameList::getAvailableWidth() - getIconColumnWidth();
}

int McaEditorNameList::getMinimumWidgetWidth() const {
    return 2 * CROSS_SIZE + getIconColumnWidth() + 20;
}

int McaEditorNameList::getIconColumnWidth() const {
    static int iconColumnWidth = MARGIN_ARROW_LEFT + ARROW_LENGTH + MARGIN_ARROW_RIGHT;
    return iconColumnWidth;
}

}   // namespace U2
