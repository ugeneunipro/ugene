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

#include "MaEditorStatusBar.h"

#include <QHBoxLayout>

#include <U2Core/MultipleSequenceAlignmentObject.h>

#include "MSAEditorSequenceArea.h"
#include "MaEditorSelection.h"

namespace U2 {

const QString MaEditorStatusBar::NONE_MARK = "-";
const QString MaEditorStatusBar::GAP_MARK = "gap";

MaEditorStatusBar::TwoArgPatternLabel::TwoArgPatternLabel(const QString& textPattern, const QString& tooltipPattern, const QString& objectName, QWidget* parent)
    : QLabel(textPattern, parent),
      textPattern(textPattern),
      tooltipPattern(tooltipPattern),
      fm(QFontMetrics(font(), this)) {
    setObjectName(objectName);
    setAlignment(Qt::AlignCenter);
}

void MaEditorStatusBar::TwoArgPatternLabel::setPatterns(const QString& newTextPattern, const QString& newTooltipPattern) {
    this->textPattern = newTextPattern;
    this->tooltipPattern = newTooltipPattern;
}

void MaEditorStatusBar::TwoArgPatternLabel::update(const QString& firstArg, int minWidth) {
    setText(textPattern.arg(firstArg));
    setToolTip(tooltipPattern.arg(firstArg));
    setMinimumWidth(minWidth);
}

void MaEditorStatusBar::TwoArgPatternLabel::update(const QString& firstArg, const QString& secondArg) {
    setText(textPattern.arg(firstArg).arg(secondArg));
    setToolTip(tooltipPattern.arg(firstArg).arg(secondArg));
    setMinimumWidth(10 + fm.width(textPattern.arg(secondArg).arg(secondArg)));
}

void MaEditorStatusBar::TwoArgPatternLabel::updateMinWidth(QString maxLenArg) {
    setMinimumWidth(10 + fm.width(textPattern.arg(maxLenArg).arg(maxLenArg)));
}

MaEditorStatusBar::MaEditorStatusBar(MaEditor* _editor)
    : editor(_editor),
      lockedIcon(":core/images/lock.png"),
      unlockedIcon(":core/images/lock_open.png") {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    selectionPattern = tr("Sel %1");
    lineLabel = new TwoArgPatternLabel(tr("Ln %1 / %2"), tr("Line %1 of %2"), "Line", this);
    columnLabel = new TwoArgPatternLabel(tr("Col %1 / %2"), tr("Column %1 of %2"), "Column", this);
    positionLabel = new TwoArgPatternLabel(tr("Pos %1 / %2"), tr("Position %1 of %2"), "Position", this);
    selectionLabel = new TwoArgPatternLabel(selectionPattern, tr("Selection width and height are %1"), "Selection", this);

    lockLabel = new QLabel();

    layout = new QHBoxLayout();
    layout->setMargin(2);
    layout->addStretch(1);
    setLayout(layout);

    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection &, const MaEditorSelection &)),
            SLOT(sl_updateStatusBar()));
    MultipleAlignmentObject *maObject = editor->getMaObject();
    connect(maObject, SIGNAL(si_alignmentChanged(const MultipleAlignment &, const MaModificationInfo &)), SLOT(sl_updateStatusBar()));
    connect(maObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_lockStateChanged()));
    // Workaround
    // updateLabels() must be called after all queued signals,
    // so we use Qt::QueuedConnection and signal si_updateStatusBar2()
    // which is emitted in intermediate signal si_updateStatusBar()
    connect(this, SIGNAL(si_updateStatusBar2()), SLOT(sl_updateStatusBar2()), Qt::QueuedConnection);

    updateLock();
}

void MaEditorStatusBar::sl_updateStatusBar() {
    // See comments in constructor
    emit si_updateStatusBar2();
}

void MaEditorStatusBar::sl_updateStatusBar2() {
    updateLabels();
}

void MaEditorStatusBar::sl_lockStateChanged() {
    updateLock();
}

QPair<QString, QString> MaEditorStatusBar::getGappedPositionInfo() const {
    const MaEditorSelection& selection = editor->getSelection();
    if (!selection.isSingleBaseSelection()) {
        return {NONE_MARK, NONE_MARK};
    }
    QPoint pos = selection.getRectList()[0].topLeft();
    int maRowIndex = editor->getCollapseModel()->getMaRowIndexByViewRowIndex(pos.y());
    MultipleAlignmentRow row = editor->getMaObject()->getRow(maRowIndex);
    QString ungappedLength = QString::number(row->getUngappedLength());
    if (row->charAt(pos.x()) == U2Msa::GAP_CHAR) {
        return {GAP_MARK, ungappedLength};
    }
    return {QString::number(row->getUngappedPosition(pos.x()) + 1), ungappedLength};
}

void MaEditorStatusBar::updateLock() {
    bool isLocked = editor->getMaObject()->isStateLocked();
    lockLabel->setPixmap(isLocked ? lockedIcon : unlockedIcon);
    lockLabel->setToolTip(isLocked ? tr("Alignment object is locked") : tr("Alignment object is not locked"));
}

void MaEditorStatusBar::updateLineLabel() {
    QString currentLineText = NONE_MARK;
    const MaEditorSelection& selection = editor->getSelection();
    if (selection.isSingleRowSelection()) {
        qint64 selectedViewRowIndex = selection.getRectList()[0].y();
        currentLineText = QString::number(selectedViewRowIndex + 1);
    }
    qint64 viewRowCount = editor->getCollapseModel()->getViewRowCount();
    lineLabel->update(currentLineText, QString::number(viewRowCount));
}

void MaEditorStatusBar::updatePositionLabel() {
    QPair<QString, QString> pp = getGappedPositionInfo();
    positionLabel->update(pp.first, pp.second);
    positionLabel->updateMinWidth(QString::number(editor->getAlignmentLen()));
}

void MaEditorStatusBar::updateColumnLabel() {
    const MaEditorSelection& selection = editor->getSelection();
    columnLabel->update(selection.isSingleColumnSelection()
                            ? QString::number(selection.getColumnRegion().startPos + 1)
                            : NONE_MARK,
                        QString::number(editor->getAlignmentLen()));
}

void MaEditorStatusBar::updateSelectionLabel() {
    const MaEditorSelection& selection = editor->getSelection();
    const QList<QRect>& selectionRects = selection.getRectList();
    QString selSize = selectionRects.isEmpty()
                          ? tr("none")
                          : QString("%1 x %2")
                                .arg(selection.getColumnRegion().length)
                                .arg(selection.isSingleRegionSelection()
                                         ? QString::number(selectionRects[0].height())
                                         : tr("%1 regions").arg(selectionRects.size()));

    QFontMetrics fm(lineLabel->font(), this);
    int maxSelLength = fm.width(selectionPattern.arg(editor->getAlignmentLen()) + "x" + QString::number(editor->getCollapseModel()->getViewRowCount()));
    int nonSelLength = fm.width(selectionPattern.arg(tr("none")));

    selectionLabel->update(selSize, 10 + qMax(maxSelLength, nonSelLength));
}

void MaEditorStatusBar::setStatusBarStyle() {
    setStyleSheet(QString("#%1 { background:rgb(219,219,219); border: 1px solid rgb(185,185,185); }").arg(objectName()));
}

}  // namespace U2
