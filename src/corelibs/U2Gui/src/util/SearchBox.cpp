/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

static const QString LABEL_STYLE_SHEET = "border: 0px; padding: 0px;";
static const QString CLEAR_BUTTON_STYLE_SHEET = "border: 0px; padding: 1px 0px 0px 0px;";

namespace U2 {

SearchBox::SearchBox(QWidget* p)
    : QLineEdit(p) {
    setObjectName("nameFilterEdit");

    progressLabel = new QLabel(this);
    progressMovie = new QMovie(GUIUtils::getResourceName("core", "progress.gif"), QByteArray(), progressLabel);
    progressLabel->setStyleSheet(LABEL_STYLE_SHEET);
    progressLabel->setMovie(progressMovie);

    searchIconLabel = new QLabel(this);
    searchIconLabel->setStyleSheet(LABEL_STYLE_SHEET);
    static constexpr int ZOOM_PIIXMAP_SIZE = 16;
    searchIconLabel->setPixmap(QPixmap(":/core/images/zoom_whole.png").scaled(ZOOM_PIIXMAP_SIZE, ZOOM_PIIXMAP_SIZE));

    clearButton = new QToolButton(this);
    clearButton->setStyleSheet(CLEAR_BUTTON_STYLE_SHEET);
    clearButton->setIcon(GUIUtils::getIconResource("core", "close_small.png"));
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setVisible(false);
    clearButton->setObjectName("project filter clear button");

    connect(clearButton, &QAbstractButton::clicked, this, &SearchBox::sl_clearButtonClicked);
    connect(this, &QLineEdit::textChanged, this, &SearchBox::sl_textChanged);
    connect(AppContext::getMainWindow(), &MainWindow::si_colorModeSwitched, this, &SearchBox::sl_colorModeSwitched);

    QWidget::setTabOrder(this, this);

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

void SearchBox::sl_clearButtonClicked() {
    setText("");
}

void SearchBox::sl_textChanged(const QString& text) {
        if (text.isEmpty()) {
            clearButton->hide();
            if (isOsMac()) {
                // Bug in Qt 5.15 & MacOS 13+.
                // When clearButton is hidden QT has QLineEdit (this) widget marked as focused (this is correct).
                // But the OS does not have focus on the QLineEdit and no keyboard events are passed to the QLineEdit.
                activateWindow();
            }
        } else {
            clearButton->show();
        }

}

void SearchBox::sl_colorModeSwitched() {
    clearButton->setIcon(GUIUtils::getIconResource("core", "close_small.png"));
    auto tmpProgressMovie = progressMovie;
    progressMovie = new QMovie(GUIUtils::getResourceName("core", "progress.gif"), QByteArray(), progressLabel);
    progressLabel->setMovie(progressMovie);
    delete tmpProgressMovie;
}

void SearchBox::paintEvent(QPaintEvent* event) {
    if (firstShow) {
        firstShow = false;
        sl_filteringFinished();
    }
    QLineEdit::paintEvent(event);
}

void SearchBox::initStyle() {
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QSize progressLabelSize = progressLabel->sizeHint();
    QSize iconLabelSize = searchIconLabel->sizeHint();
    QSize clearButtonSize = clearButton->sizeHint();
    QSize minimumWidgetSize = minimumSizeHint();

    int rightPadding = progressLabelSize.width() + clearButtonSize.width() + frameWidth + 1;
    int leftPadding = iconLabelSize.width() + frameWidth + 1;
    setStyleSheet(QString("QLineEdit {padding-right: %1px; padding-left: %2px}").arg(rightPadding).arg(leftPadding));

    int minimumContentWidth = iconLabelSize.width() + progressLabelSize.width() + clearButtonSize.width() + frameWidth * 2 + 2;
    int minimumContentHeight = progressLabelSize.height() + frameWidth * 2 + 2;
    setMinimumSize(qMax(minimumWidgetSize.width(), minimumContentWidth), qMax(minimumWidgetSize.height(), minimumContentHeight));
}

void SearchBox::updateInternalControlsPosition() {
    QSize progressLabelSize = progressLabel->sizeHint();
    QSize iconLabelSize = searchIconLabel->sizeHint();
    QSize clearButtonSize = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QRect widgetRect = rect();

    progressLabel->move(widgetRect.right() - 2 * frameWidth - progressLabelSize.width(),
                        (widgetRect.bottom() - progressLabelSize.height() + 1) / 2);
    clearButton->move(widgetRect.right() - (progressLabel->isVisible() ? progressLabelSize.width() + 2 * frameWidth : 0) - clearButtonSize.width(),
                      (widgetRect.bottom() - clearButtonSize.height() + 1) / 2);
    searchIconLabel->move(widgetRect.left() + 2 * frameWidth, (widgetRect.bottom() - iconLabelSize.height() + 1) / 2);
}

void SearchBox::resizeEvent(QResizeEvent* event) {
    updateInternalControlsPosition();
    QLineEdit::resizeEvent(event);
}

}  // namespace U2
