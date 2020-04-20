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
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include "ShowHideSubgroupWidget.h"

namespace U2 {

ShowHideSubgroupWidget::ShowHideSubgroupWidget(QWidget *parent)
: QWidget(parent), arrowHeaderWidget(NULL), innerWidget(NULL)
{

}

ShowHideSubgroupWidget::ShowHideSubgroupWidget(const QString &_id, const QString &caption, QWidget* _innerWidget, bool isOpened)
    : subgroupId(_id), innerWidget(_innerWidget)
{
    init(subgroupId, caption, innerWidget, isOpened);
}

void ShowHideSubgroupWidget::init(const QString &subgroupId, const QString &caption, QWidget *innerWidget, bool isOpened) {
    this->subgroupId = subgroupId;
    this->innerWidget = innerWidget;

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 10);
    mainLayout->setSpacing(0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(mainLayout);

    innerWidget->setContentsMargins(17, 5, 5, 5);

    arrowHeaderWidget = new ArrowHeaderWidget(caption, isOpened);
    connect(arrowHeaderWidget, SIGNAL(si_arrowHeaderPressed(bool)), SLOT(updateSubgroupState(bool)));
    updateSubgroupState(isOpened);

    mainLayout->addWidget(arrowHeaderWidget);
    mainLayout->addWidget(innerWidget);

    setObjectName(subgroupId);

}

void ShowHideSubgroupWidget::updateSubgroupState(bool isSubgroupOpened) {
    innerWidget->setVisible(isSubgroupOpened);
    emit si_subgroupStateChanged(subgroupId);
}

bool ShowHideSubgroupWidget::isSubgroupOpened() const {
    SAFE_POINT(0 != arrowHeaderWidget, "The arrow header widget hasn't been created, but it is used.", false);
    return arrowHeaderWidget->isArrowOpened();
}

void ShowHideSubgroupWidget::setSubgroupOpened(bool open) {
    arrowHeaderWidget->setOpened(open);
}

void ShowHideSubgroupWidget::showProgress() {
    arrowHeaderWidget->showProgressWithTimeout();
}

void ShowHideSubgroupWidget::hideProgress() {
    arrowHeaderWidget->hideProgress();
}

void ShowHideSubgroupWidget::setPermanentlyOpen(bool isOpened) {
    arrowHeaderWidget->setOpened(isOpened);
    if (isOpened) {
        disconnect(arrowHeaderWidget, SIGNAL(si_arrowHeaderPressed(bool)), this, SLOT(updateSubgroupState(bool)));
        arrowHeaderWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    } else {
        connect(arrowHeaderWidget, SIGNAL(si_arrowHeaderPressed(bool)), SLOT(updateSubgroupState(bool)));
        arrowHeaderWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
}

ArrowHeaderWidget::ArrowHeaderWidget(const QString &caption, bool _isOpened)
    : isOpened(_isOpened)
{
    QHBoxLayout* arrowHeaderLayout = new QHBoxLayout();
    arrowHeaderLayout->setContentsMargins(0, 0, 0, 0);
    arrowHeaderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    arrow = new QLabel();
    arrow->setObjectName("ArrowHeader_" + caption);
    if (isOpened) {
        arrow->setPixmap(QPixmap(":core/images/arrow_down.png"));
    } else {
        arrow->setPixmap(QPixmap(":core/images/arrow_right.png"));
    }

    arrow->setMaximumSize(10, 10);

    QLabel* captionLabel = new QLabel(caption);
    captionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    progressMovieLabel = new QLabel();
    progressMovie = new QMovie(":/core/images/progress.gif", QByteArray(), progressMovieLabel);
    progressMovieLabel->setMovie(progressMovie);

    if (progressMovie->isValid()){
        progressMovie->start();
        progressMovie->setPaused(true);
    }

    arrowHeaderLayout->addWidget(arrow);
    arrowHeaderLayout->addWidget(captionLabel);
    arrowHeaderLayout->addWidget(progressMovieLabel);

    progressMovieLabel->hide();
    canStartProgress = false;

    setLayout(arrowHeaderLayout);
}

ArrowHeaderWidget::~ArrowHeaderWidget() {
    delete progressMovie;
}

void ArrowHeaderWidget::showProgressWithTimeout() {
    QTimer* timeoutToStartProgress = new QTimer(this);
    connect(timeoutToStartProgress, SIGNAL(timeout()), SLOT(sl_showProgress()));
    timeoutToStartProgress->start(TIMEOUT);
    canStartProgress = true;
}

void ArrowHeaderWidget::sl_showProgress() {
    if (canStartProgress) {
        progressMovie->setPaused(false);
        progressMovieLabel->show();
    }
}

void ArrowHeaderWidget::hideProgress() {
    canStartProgress = false;
    progressMovieLabel->hide();
    progressMovie->setPaused(true);
}

void ArrowHeaderWidget::setOpened(bool _isOpened) {
    if (_isOpened != isOpened) {
        if (isOpened) {
            arrow->setPixmap(QPixmap(":core/images/arrow_right.png"));
            isOpened = false;
        } else {
            arrow->setPixmap(QPixmap(":core/images/arrow_down.png"));
            isOpened = true;
        }
        emit si_arrowHeaderPressed(isOpened);
    }
}


void ArrowHeaderWidget::mousePressEvent(QMouseEvent * /* event */) {
    setOpened(!isOpened);
}

} // namespace U2
