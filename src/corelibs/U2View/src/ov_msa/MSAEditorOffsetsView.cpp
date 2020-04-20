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

#include <math.h>

#include <QPainter>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MaEditorNameList.h"
#include "McaEditor.h"
#include "MSAEditor.h"
#include "MSAEditorOffsetsView.h"
#include "MSAEditorSequenceArea.h"
#include "helpers/DrawHelper.h"
#include "helpers/RowHeightController.h"
#include "helpers/ScrollController.h"

namespace U2 {

#define SETTINGS_SHOW_OFFSETS "show_offsets"

MSAEditorOffsetsViewController::MSAEditorOffsetsViewController(MaEditorWgt* maEditorUi, MaEditor* ed, MaEditorSequenceArea* sa)
    : QObject(maEditorUi)
{
    seqArea = sa;
    editor = ed;

    lw = new MSAEditorOffsetsViewWidget(maEditorUi, ed, seqArea, true);
    lw->setObjectName("msa_editor_offsets_view_widget_left");
    rw = new MSAEditorOffsetsViewWidget(maEditorUi, ed, seqArea, false);
    rw->setObjectName("msa_editor_offsets_view_widget_right");

    connect(maEditorUi->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_updateOffsets()));
    connect(editor, SIGNAL(si_fontChanged(const QFont&)), SLOT(sl_updateOffsets()));

    MultipleAlignmentObject *mobj = editor->getMaObject();
    SAFE_POINT(NULL != mobj, L10N::nullPointerError("multiple alignment object"), );
    connect(mobj, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
        SLOT(sl_updateOffsets()));

    seqArea->installEventFilter(this);

    Settings* s = AppContext::getSettings();
    bool showOffsets = s->getValue(editor->getSettingsRoot() + SETTINGS_SHOW_OFFSETS, true).toBool();

    viewAction = new QAction(tr("Show offsets"), this);
    viewAction->setObjectName("show_offsets");
    viewAction->setCheckable(true);
    viewAction->setChecked(showOffsets);
    connect(viewAction, SIGNAL(triggered(bool)), SLOT(sl_showOffsets(bool)));
    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), SLOT(sl_updateOffsets()));
    connect(editor, SIGNAL(si_completeUpdate()), SLOT(sl_updateOffsets()));
    updateOffsets();
}

MSAEditorOffsetsViewWidget * MSAEditorOffsetsViewController::getLeftWidget() const {
    return lw;
}

MSAEditorOffsetsViewWidget * MSAEditorOffsetsViewController::getRightWidget() const {
    return rw;
}

QAction * MSAEditorOffsetsViewController::getToggleColumnsViewAction() const {
    return viewAction;
}

void MSAEditorOffsetsViewController::sl_updateOffsets() {
    updateOffsets();
}

bool MSAEditorOffsetsViewController::eventFilter(QObject* o, QEvent* e) {
    if (o == seqArea) {
        if (e->type() == QEvent::Resize || e->type() == QEvent::Show) {
            updateOffsets();
        }
    }
    return false;
}

void MSAEditorOffsetsViewController::sl_showOffsets(bool show) {
    updateOffsets();
    Settings* s = AppContext::getSettings();
    SAFE_POINT(s != NULL, "AppContext settings is NULL", );
    s->setValue(editor->getSettingsRoot() + SETTINGS_SHOW_OFFSETS, show);
}

void MSAEditorOffsetsViewController::updateOffsets() {
    if (lw->parentWidget() != NULL) {
        const bool vis = viewAction->isChecked();
        lw->setVisible(vis);
        rw->setVisible(vis);
    }

    lw->updateView();
    rw->updateView();
}

MSAEditorOffsetsViewWidget::MSAEditorOffsetsViewWidget(MaEditorWgt *maEditorUi, MaEditor *ed, MaEditorSequenceArea* sa, bool sp)
    : seqArea(sa),
      editor(ed),
      showStartPos(sp),
      completeRedraw(true)
{
    connect(maEditorUi, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));
}

void MSAEditorOffsetsViewWidget::sl_completeRedraw() {
    completeRedraw = true;
    update();
}

#define OFFS_WIDGET_BORDER 3
void MSAEditorOffsetsViewWidget::updateView() {
    const int aliLen = editor->getMaObject()->getLength();
    QFont f = getOffsetsFont();
    QFontMetrics fm(f, this);
    int aliLenStrLen = int(log10((double)aliLen)) + 1;
    int w = OFFS_WIDGET_BORDER + fm.width('X') * aliLenStrLen + OFFS_WIDGET_BORDER;
    w += (showStartPos ? fm.width('[') : fm.width(']'));
    setFixedWidth(w);
    completeRedraw = true;
    update();
}

void MSAEditorOffsetsViewWidget::paintEvent(QPaintEvent*) {
    SAFE_POINT(isVisible(), "Attempting to paint an invisible widget", );
    const QSize s = size() * devicePixelRatio();
    if (s != cachedView.size()) {
        cachedView = QPixmap(s);
        cachedView.setDevicePixelRatio(devicePixelRatio());
        completeRedraw = true;
    }
    if (completeRedraw) {
        QPainter pCached(&cachedView);
        drawAll(pCached);
        completeRedraw = false;
    }
    QPainter p(this);
    p.drawPixmap(0, 0, cachedView);
}

QFont MSAEditorOffsetsViewWidget::getOffsetsFont() {
    QFont f = editor->getFont();
    f.setPointSize(qMax(f.pointSize()-1, 6));
    return f;
}

int MSAEditorOffsetsViewWidget::getBaseCounts(int seqNum, int aliPos, bool inclAliPos) const {
    const MultipleAlignmentRow &row = editor->getMaObject()->getRow(seqNum);
    const int endPos = inclAliPos ? aliPos + 1 : aliPos;

    return (endPos < row->getCoreStart()) ? 0 : row->getBaseCount(endPos);
}

void MSAEditorOffsetsViewWidget::drawAll(QPainter& painter) {
    QLinearGradient gradient(0, 0, width(), 0);
    QColor lg(0xDA, 0xDA, 0xDA);
    QColor dg(0x4A, 0x4A, 0x4A);
    gradient.setColorAt(0.00, lg);
    gradient.setColorAt(0.25, Qt::white);
    gradient.setColorAt(0.75, Qt::white);
    gradient.setColorAt(1.00, lg);
    painter.fillRect(rect(), QBrush(gradient));

    const int widgetWidth = width();

    QFont font = getOffsetsFont();
    QFontMetrics fm(font,this);
    painter.setFont(font);

    MaEditorWgt* ui = editor->getUI();
    int alignmentLength = editor->getMaObject()->getLength();
    int lbw = fm.width('[');
    int rbw = fm.width(']');
    int pos = showStartPos ? ui->getScrollController()->getFirstVisibleBase(true) : ui->getScrollController()->getLastVisibleBase(seqArea->width(), true);

    QList<int> visibleRows = ui->getDrawHelper()->getVisibleMaRowIndexes(height());

    const MultipleAlignment alignment = editor->getMaObject()->getMultipleAlignment();
    U2OpStatusImpl os;
    const int refSeq = alignment->getRowIndexByRowId(editor->getReferenceRowId(), os);

    foreach (const int rowNumber, visibleRows) {
        const U2Region yRange = ui->getRowHeightController()->getScreenYRegionByMaRowIndex(rowNumber);
        int offs = getBaseCounts(rowNumber, pos, !showStartPos);
        int seqSize = getBaseCounts(rowNumber, alignmentLength - 1, true);
        QString offset = offs + 1 > seqSize ? QString::number(seqSize) : QString::number(offs + 1);
        if (showStartPos && offs == 0) {
            painter.setPen(Qt::black);
            QRect lbr(OFFS_WIDGET_BORDER, yRange.startPos, lbw, yRange.length);
            if (rowNumber == refSeq){
                drawRefSequence(painter, lbr);
            }
            painter.drawText(lbr, Qt::AlignTop, "[");
        } else if (!showStartPos && offs == seqSize) {
            painter.setPen(Qt::black);
            QRect rbr(widgetWidth - OFFS_WIDGET_BORDER - rbw, yRange.startPos, rbw, yRange.length);
            if (rowNumber == refSeq){
                drawRefSequence(painter, rbr);
            }
            painter.drawText(rbr, Qt::AlignTop, "]");
            offset = QString::number(offs);
        } else {
            painter.setPen(dg);
        }
        QRect tr(OFFS_WIDGET_BORDER + (showStartPos ? lbw : 0), yRange.startPos, widgetWidth - 2 * OFFS_WIDGET_BORDER - (showStartPos ? lbw : rbw), yRange.length);
        if (rowNumber == refSeq){
            drawRefSequence(painter, tr);
        }
        painter.drawText(tr, Qt::AlignRight | Qt::AlignTop, offset);
    }
}

void MSAEditorOffsetsViewWidget::drawRefSequence(QPainter &p, const QRect &r){
    p.fillRect(r, QColor("#9999CC"));
}

}//namespace
