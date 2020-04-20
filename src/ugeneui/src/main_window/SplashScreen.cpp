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

#include "SplashScreen.h"

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>
#include <U2Core/Version.h>
#include <U2Core/U2SafePoints.h>

#include <QEvent>
#include <QMovie>
#include <QPainter>
#include <QLayout>
#include <QLabel>

namespace U2{

#define VERSION_HEIGHT_PX 15
#define TASK_HEIGHT_PX 13

SplashScreen::SplashScreen( QWidget *parent /* = NULL*/ ):QDialog(parent) {
    setEnabled(false);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#ifdef Q_OS_WIN
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
#endif
    setWindowModality(Qt::ApplicationModal);
    QHBoxLayout* mainLayout = new QHBoxLayout();
    setLayout(mainLayout);
    setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    QFrame *frame = new QFrame(this);
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

void SplashScreen::sl_close(){
    if(AppContext::getTaskScheduler() == qobject_cast<TaskScheduler*>(sender())){
        removeEventFilter(this);
        close();
    }
}

bool SplashScreen::eventFilter(QObject * /*obj*/, QEvent *ev){
    if(ev->type() == QEvent::Close){
        ev->ignore();
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////
SplashScreenWidget::SplashScreenWidget(){
    setObjectName("splash_screen_widget");

    Version v = Version::appVersion();
    version = v.text;

    QImage image(":ugene/images/ugene_splash.png");
    QSize widgetSize = image.size();
    setFixedSize(widgetSize);

    image = image.scaled(widgetSize * devicePixelRatio());
    image.setDevicePixelRatio(devicePixelRatio());
    image1 = image2 = image;

    dots_number = 0;
    task = "";
    dots_timer_id = startTimer(500);
    startTimer(50);

    getTask();
    getDots();
    drawInfo();
}

void SplashScreenWidget::getTask(){
    if(AppContext::getTaskScheduler() == NULL){
        return;
    }
    QList<Task*> tasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    if(tasks.size() > 0){
        Task* topLevelTask = tasks.at(0);
        task = topLevelTask->getTaskName();

        QList<QPointer<Task> > subtasks = topLevelTask->getSubtasks();
        if(subtasks.size() > 0){
            task = subtasks.at(0)->getTaskName();
        }
    }else{
        task = "";
    }
}

void SplashScreenWidget::getDots(){
    if(dots_number >= 3){
        dots_number = 0;
    }else{
        dots_number++;
    }
}

void SplashScreenWidget::timerEvent(QTimerEvent *e){
    getTask();
    if(e->timerId()==dots_timer_id){
        getDots();
    }
    drawInfo();

    update();
    QObject::timerEvent(e);
}

void SplashScreenWidget::paintEvent(QPaintEvent *e){
    QWidget::paintEvent(e);

    QPainter p(this);
    p.drawImage(0, 0, image2);
}

void SplashScreenWidget::drawInfo(){
    image2 = image1;

    QPainter p(&image2);

    QFont font = p.font();
    font.setFamily("Heiti");
    font.setBold(true);
    font.setPixelSize(VERSION_HEIGHT_PX);
    p.setFont( font );
    p.setPen(QColor(0, 46, 59));
    QString versionSign = version.right(version.indexOf(".")).toInt() == 0 ? version.left(version.indexOf(".")) : version;
    if (version.contains("-dev")) {
        versionSign += "-dev";
    }
    QString text = tr("Version ") + versionSign + tr(" is loading");
    for (int i = 0; i < dots_number; i++) {
        text.append(".");
    }
    p.drawText(17,285, text);

    if(!task.isEmpty()){
        font.setPixelSize(TASK_HEIGHT_PX);
        font.setBold(false);
        p.setFont( font );
        p.drawText(18,290 + VERSION_HEIGHT_PX, task);
    }
    p.end();
}
}
