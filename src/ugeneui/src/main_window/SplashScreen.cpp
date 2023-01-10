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

#include "SplashScreen.h"

#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QMovie>
#include <QPainter>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

namespace U2 {

#define VERSION_HEIGHT_PX 15
#define TASK_HEIGHT_PX 13

SplashScreen::SplashScreen(QWidget* parent /* = NULL*/)
    : QDialog(parent) {
    setEnabled(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    if (isOsWindows()) {
        setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    }
    setWindowModality(Qt::ApplicationModal);
    QHBoxLayout* mainLayout = new QHBoxLayout();
    setLayout(mainLayout);
    setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    QFrame* frame = new QFrame(this);
    mainLayout->addWidget(frame);

    QHBoxLayout* frameLayout = new QHBoxLayout();
    frameLayout->setMargin(0);
    frame->setContentsMargins(0, 0, 0, 0);
    frame->setLayout(frameLayout);

    SplashScreenWidget* sWidget = new SplashScreenWidget();

    QVBoxLayout* aWLayout = (QVBoxLayout*)frame->layout();
    aWLayout->insertWidget(0, sWidget);
    aWLayout->setStretchFactor(sWidget, 100);
    installEventFilter(this);
}

void SplashScreen::sl_close() {
    removeEventFilter(this);
    close();
}

bool SplashScreen::eventFilter(QObject* /*obj*/, QEvent* ev) {
    if (ev->type() == QEvent::Close) {
        ev->ignore();
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////
SplashScreenWidget::SplashScreenWidget() {
    setObjectName("splash_screen_widget");

    Version v = Version::appVersion();
    version = QString::number(v.major) +
              (v.minor == 0 ? "" : "." + QString::number(v.minor)) +
              (v.suffix.isEmpty() ? "" : "-" + v.suffix);

    QImage image(":ugene/images/ugene_splash.png");
    QSize widgetSize = image.size();
    setFixedSize(widgetSize);

    image = image.scaled(widgetSize * devicePixelRatio());
    image.setDevicePixelRatio(devicePixelRatio());
    image1 = image2 = image;

    dots_number = 0;
    activeTaskName = "";
    dots_timer_id = startTimer(500);
    startTimer(50);

    updateActiveTaskName();
    getDots();
    drawInfo();
}

static Task* selectTaskToShowAsActive(const QList<Task*>& taskList, bool preferFirst) {
    foreach (Task* task, taskList) {
        if (task->isRunning()) {
            return task;
        }
    }
    return taskList.isEmpty() ? nullptr : (preferFirst ? taskList.first() : taskList.last());
}

void SplashScreenWidget::updateActiveTaskName() {
    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    CHECK(scheduler != nullptr, )
    QList<Task*> topLevelTasks = scheduler->getTopLevelTasks();
    Task* activeTopLevelTask = selectTaskToShowAsActive(topLevelTasks, true);
    if (activeTopLevelTask == nullptr) {
        activeTaskName = "";
        return;
    }
    QList<Task*> subTasks = activeTopLevelTask->getPureSubtasks();
    Task* activeTask = subTasks.isEmpty() ? activeTopLevelTask : selectTaskToShowAsActive(subTasks, false);
    activeTaskName = activeTask->getTaskName();
}

void SplashScreenWidget::getDots() {
    if (dots_number >= 3) {
        dots_number = 0;
    } else {
        dots_number++;
    }
}

void SplashScreenWidget::timerEvent(QTimerEvent* e) {
    updateActiveTaskName();
    if (e->timerId() == dots_timer_id) {
        getDots();
    }
    drawInfo();

    update();
    QObject::timerEvent(e);
}

void SplashScreenWidget::paintEvent(QPaintEvent* e) {
    QWidget::paintEvent(e);

    QPainter p(this);
    p.drawImage(0, 0, image2);
}

void SplashScreenWidget::drawInfo() {
    image2 = image1;

    QPainter p(&image2);

    QFont font = p.font();

    // Error: qt.qpa.fonts: Populating font family aliases took 2647 ms. Replace uses of missing font family "Heiti" with one that exists to avoid this cost
    // font.setFamily("Heiti");

    font.setBold(true);
    font.setPixelSize(VERSION_HEIGHT_PX);
    p.setFont(font);
    p.setPen(QColor(0, 46, 59));

    QString text = tr("Version ") + version + tr(" is loading") + QString(".").repeated(dots_number);
    p.drawText(17, 285, text);

    if (!activeTaskName.isEmpty()) {
        font.setPixelSize(TASK_HEIGHT_PX);
        font.setBold(false);
        p.setFont(font);
        p.drawText(18, 290 + VERSION_HEIGHT_PX, activeTaskName);
    }
}
}  // namespace U2
