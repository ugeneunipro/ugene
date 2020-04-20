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

#include <QHBoxLayout>

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "MSAEditorSequenceArea.h"
#include "MaEditorStatusBar.h"

namespace U2 {

const QString MaEditorStatusBar::NONE_MARK = "-";
const QString MaEditorStatusBar::GAP_MARK = "gap";

MaEditorStatusBar::TwoArgPatternLabel::TwoArgPatternLabel(QString textPattern, QString tooltipPattern,
                                                          QString objectName, QWidget* parent)
    : QLabel(textPattern, parent),
      textPattern(textPattern),
      tooltipPattern(tooltipPattern),
      fm(QFontMetrics(font(),this)) {
    setObjectName(objectName);
    setAlignment(Qt::AlignCenter);
}

MaEditorStatusBar::TwoArgPatternLabel::TwoArgPatternLabel(QString objectName, QWidget* parent)
    : QLabel(parent),
      fm(QFontMetrics(font(),this)) {
    setObjectName(objectName);
    setAlignment(Qt::AlignCenter);
}

void MaEditorStatusBar::TwoArgPatternLabel::setPatterns(QString textPattern, QString tooltipPattern) {
    this->textPattern = textPattern;
    this->tooltipPattern = tooltipPattern;
}

void MaEditorStatusBar::TwoArgPatternLabel::update(QString firstArg, int minWidth) {
    setText(textPattern.arg(firstArg));
    setToolTip(tooltipPattern.arg(firstArg));
    setMinimumWidth(minWidth);
}

void MaEditorStatusBar::TwoArgPatternLabel::update(QString firstArg, QString secondArg) {
    setText(textPattern.arg(firstArg).arg(secondArg));
    setToolTip(tooltipPattern.arg(firstArg).arg(secondArg));
    setMinimumWidth(10 + fm.width(textPattern.arg(secondArg).arg(secondArg)));
}

void MaEditorStatusBar::TwoArgPatternLabel::updateMinWidth(QString maxLenArg) {
    setMinimumWidth(10 + fm.width(textPattern.arg(maxLenArg).arg(maxLenArg)));
}

MaEditorStatusBar::MaEditorStatusBar(MultipleAlignmentObject* mobj, MaEditorSequenceArea* sa)
    : aliObj(mobj),
      seqArea(sa),
      lockedIcon(":core/images/lock.png"),
      unlockedIcon(":core/images/lock_open.png")
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    selectionPattern = tr("Sel %1");
    lineLabel = new TwoArgPatternLabel(tr("Ln %1 / %2"), tr("Line %1 of %2"), "Line", this);
    colomnLabel = new TwoArgPatternLabel(tr("Col %1 / %2"), tr("Column %1 of %2"), "Column", this);
    positionLabel = new TwoArgPatternLabel(tr("Pos %1 / %2"), tr("Position %1 of %2"), "Position", this);
    selectionLabel = new TwoArgPatternLabel(selectionPattern, tr("Selection width and height are %1"), "Selection", this);

    lockLabel = new QLabel();

    layout = new QHBoxLayout();
    layout->setMargin(2);
    layout->addStretch(1);
    setLayout(layout);

    connect(seqArea, SIGNAL(si_selectionChanged(const MaEditorSelection& , const MaEditorSelection& )),
            SLOT(sl_update()));
    connect(mobj, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
            SLOT(sl_update()));
    connect(mobj, SIGNAL(si_lockedStateChanged()), SLOT(sl_lockStateChanged()));

    updateLock();
}

void MaEditorStatusBar::sl_update() {
    updateLabels();
}

void MaEditorStatusBar::sl_lockStateChanged() {
    updateLock();
}

QPair<QString, QString> MaEditorStatusBar::getGappedPositionInfo(const QPoint& pos) const{
    if (pos.isNull()) {
        return QPair<QString, QString>(NONE_MARK, NONE_MARK);
    }
    int maRowIndex = seqArea->getTopSelectedMaRow();
    if (maRowIndex == -1) {
        return QPair<QString, QString>(NONE_MARK, NONE_MARK);
    }
    MultipleAlignmentRow row = seqArea->getEditor()->getMaObject()->getRow(maRowIndex);
    QString ungappedLength = QString::number(row->getUngappedLength());
    if (row->charAt(pos.x()) == U2Msa::GAP_CHAR) {
        return QPair<QString, QString>(GAP_MARK, ungappedLength);
    }
    return QPair<QString, QString>(QString::number(row->getUngappedPosition(pos.x()) + 1), ungappedLength);
}

void MaEditorStatusBar::updateLock() {
    bool locked = aliObj->isStateLocked();
    lockLabel->setPixmap(locked ? lockedIcon : unlockedIcon);
    lockLabel->setToolTip(locked ? tr("Alignment object is locked") : tr("Alignment object is not locked"));
}

void MaEditorStatusBar::updateLineLabel() {
    MaEditorSelection selection = seqArea->getSelection();
    const int firstSelected = seqArea->getRowIndex(selection.y());
    const int totalVisible = aliObj->getNumRows();
    lineLabel->update(selection.isEmpty() || firstSelected == -1 ? NONE_MARK : QString::number(firstSelected + 1),
                      QString::number(totalVisible));
}

void MaEditorStatusBar::updatePositionLabel() {
    MaEditorSelection selection = seqArea->getSelection();
    QPair<QString, QString> pp = getGappedPositionInfo(selection.topLeft());
    positionLabel->update(pp.first, pp.second);
    positionLabel->updateMinWidth(QString::number(aliObj->getLength()));
}

void MaEditorStatusBar::updateColumnLabel() {
    MaEditorSelection selection = seqArea->getSelection();
    const QPoint& pos = selection.topLeft();

    qint64 alignmentLen = aliObj->getLength();
    colomnLabel->update(selection.isEmpty() ? NONE_MARK : QString::number(pos.x() + 1), QString::number(alignmentLen));
}

void MaEditorStatusBar::updateSelectionLabel() {
    MaEditorSelection selection = seqArea->getSelection();
    QString selSize;
    if (selection.isEmpty()) {
        selSize = QObject::tr("none");
    } else {
        selSize = QString::number(selection.width()) + "x" + QString::number(selection.height());
    }
    QFontMetrics fm(lineLabel->font(),this);
    int maxSelLength = fm.width(selectionPattern.arg(QString::number(aliObj->getLength()) + "x" + QString::number( aliObj->getNumRows())));
    int nonSelLength = fm.width(selectionPattern.arg(QObject::tr("none")));

    selectionLabel->update(selSize, 10 + qMax(maxSelLength, nonSelLength));
}

void MaEditorStatusBar::setStatusBarStyle() {
    setStyleSheet(QString("#%1 { background:rgb(219,219,219); border: 1px solid rgb(185,185,185); }").arg(objectName()));
}

}//namespace
