/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MaEditorUtils.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MaEditorWgt.h"
#include "ov_msa/MaEditorSelection.h"

namespace U2 {

/************************************************************************/
/* MaUtilsWidget */
/************************************************************************/
MaUtilsWidget::MaUtilsWidget(MaEditorWgt* ui, QWidget* heightWidget)
    : ui(ui),
      heightWidget(heightWidget),
      heightMargin(0) {
    connect(ui->getEditor(), SIGNAL(si_zoomOperationPerformed(bool)), SLOT(sl_fontChanged()));
    setMinimumHeight(heightWidget->height() + heightMargin);
}

void MaUtilsWidget::sl_fontChanged() {
    update();
    setMinimumHeight(heightWidget->height() + heightMargin);
}

const QFont& MaUtilsWidget::getMsaEditorFont() {
    return ui->getEditor()->getFont();
}

void MaUtilsWidget::setHeightMargin(int _heightMargin) {
    heightMargin = _heightMargin;
    setMinimumHeight(heightWidget->height() + heightMargin);
}

void MaUtilsWidget::mousePressEvent(QMouseEvent*) {
    ui->getEditor()->getSelectionController()->clearSelection();
}
void MaUtilsWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    setMinimumHeight(heightWidget->height() + heightMargin);
}

/************************************************************************/
/* MaLabelWidget */
/************************************************************************/
MaLabelWidget::MaLabelWidget(MaEditorWgt* ui, QWidget* heightWidget, const QString& text, Qt::Alignment alignment, bool proxyMouseEventsToNameList)
    : MaUtilsWidget(ui, heightWidget), proxyMouseEventsToNameList(proxyMouseEventsToNameList) {
    label = new QLabel(text, this);
    label->setAlignment(alignment);
    label->setTextFormat(Qt::RichText);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Disable text interaction: all mouse events from QLabel will be delivered to this widget.
    label->setTextInteractionFlags(Qt::NoTextInteraction);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    setLayout(layout);
}

void MaLabelWidget::paintEvent(QPaintEvent* e) {
    MaUtilsWidget::paintEvent(e);
    label->setFont(getMsaEditorFont());
}

void MaLabelWidget::mousePressEvent(QMouseEvent* e) {
    if (proxyMouseEventsToNameList) {
        QMouseEvent proxyEvent(e->type(), QPoint(e->x(), 0), e->globalPos(), e->button(), e->buttons(), e->modifiers());
        sendEventToNameList(&proxyEvent);
    }
}

void MaLabelWidget::mouseReleaseEvent(QMouseEvent* e) {
    if (proxyMouseEventsToNameList) {
        QMouseEvent proxyEvent(e->type(), QPoint(e->x(), qMax(e->y() - height(), 0)), e->globalPos(), e->button(), e->buttons(), e->modifiers());
        sendEventToNameList(&proxyEvent);
    }
}

void MaLabelWidget::mouseMoveEvent(QMouseEvent* e) {
    if (proxyMouseEventsToNameList) {
        QMouseEvent proxyEvent(e->type(), QPoint(e->x(), e->y() - height()), e->globalPos(), e->button(), e->buttons(), e->modifiers());
        sendEventToNameList(&proxyEvent);
    }
}

void MaLabelWidget::sendEventToNameList(QMouseEvent* e) const {
    QApplication::instance()->notify((QObject*)ui->getEditorNameList(), e);
}

}  // namespace U2
