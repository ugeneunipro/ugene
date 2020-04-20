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

#include "SearchBox.h"

#include <QLabel>
#include <QMovie>
#include <QStyle>
#include <QToolButton>

static const QString LABEL_STYLE_SHEET = "border: 0px; padding: 0px;";
static const QString CLEAR_BUTTON_STYLE_SHEET = "border: 0px; padding: 1px 0px 0px 0px;";

namespace U2 {

SearchBox::SearchBox(QWidget *p)
    : QLineEdit(p), firstShow(true), progressLabel(new QLabel(this)), progressMovie(new QMovie(":/core/images/progress.gif", QByteArray(), progressLabel)),
    searchIconLabel(new QLabel(this)), clearButton(new QToolButton(this))
{
    setObjectName("nameFilterEdit");

    progressLabel->setStyleSheet(LABEL_STYLE_SHEET);
    progressLabel->setMovie(progressMovie);
    progressMovie->start();

    searchIconLabel->setStyleSheet(LABEL_STYLE_SHEET);
    searchIconLabel->setPixmap(QPixmap(":/core/images/zoom_whole.png"));

    clearButton->setStyleSheet(CLEAR_BUTTON_STYLE_SHEET);
    clearButton->setIcon(QIcon(":/core/images/close_small.png"));
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setVisible(false);
    connect(clearButton, SIGNAL(clicked()), SLOT(sl_filterCleared()));
    connect(this, SIGNAL(textChanged(const QString &)), SLOT(sl_textChanged(const QString &)));
    clearButton->setObjectName("project filter clear button");

    initStyle();
    setPlaceholderText(tr("Search..."));
}

void SearchBox::sl_filteringStarted() {
    progressLabel->setVisible(true);
    progressMovie->start();
    updateInternalControlsPosition();
}

void SearchBox::sl_filteringFinished() {
    progressMovie->stop();
    progressLabel->setVisible(false);
    updateInternalControlsPosition();
}

void SearchBox::sl_filterCleared() {
    clearButton->setVisible(false);
    setText(QString());
}

void SearchBox::sl_textChanged(const QString &text) {
    clearButton->setVisible(!text.isEmpty());
}

void SearchBox::paintEvent(QPaintEvent *event) {
    if (firstShow) {
        firstShow = false;
        sl_filteringFinished();
    }
    QLineEdit::paintEvent(event);
}

void SearchBox::initStyle() {
    const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    const QSize progressLabelSize = progressLabel->sizeHint();
    const QSize iconLabelSize = searchIconLabel->sizeHint();
    const QSize clearButtonSize = clearButton->sizeHint();
    const QSize minimumWidgetSize = minimumSizeHint();

    const int rightPadding = progressLabelSize.width() + clearButtonSize.width() + frameWidth + 1;
    const int leftPadding = iconLabelSize.width() + frameWidth + 1;
    setStyleSheet(QString("QLineEdit {padding-right: %1px; padding-left: %2px}").arg(rightPadding).arg(leftPadding));

    const int minimumContentWidth = iconLabelSize.width() + progressLabelSize.width() + clearButtonSize.width() + frameWidth * 2 + 2;
    const int minimumContentHeight = progressLabelSize.height() + frameWidth * 2 + 2;
    setMinimumSize(qMax(minimumWidgetSize.width(), minimumContentWidth), qMax(minimumWidgetSize.height(), minimumContentHeight));
}

void SearchBox::updateInternalControlsPosition() {
    const QSize progressLabelSize = progressLabel->sizeHint();
    const QSize iconLabelSize = searchIconLabel->sizeHint();
    const QSize clearButtonSize = clearButton->sizeHint();
    const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    const QRect widgetRect = rect();

    progressLabel->move(widgetRect.right() - 2 * frameWidth - progressLabelSize.width(),
        (widgetRect.bottom() - progressLabelSize.height() + 1) / 2);
    clearButton->move(widgetRect.right() - (progressLabel->isVisible() ? progressLabelSize.width() + 2 * frameWidth : 0) - clearButtonSize.width(),
        (widgetRect.bottom() - clearButtonSize.height() + 1) / 2);
    searchIconLabel->move(widgetRect.left() + 2 * frameWidth, (widgetRect.bottom() - iconLabelSize.height() + 1) / 2);
}

void SearchBox::resizeEvent(QResizeEvent *event) {
    updateInternalControlsPosition();
    QLineEdit::resizeEvent(event);
}

} // namespace U2
