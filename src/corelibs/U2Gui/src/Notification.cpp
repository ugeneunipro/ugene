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

#include <QApplication>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTime>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/QObjectScopedPointer.h>

#include "MainWindow.h"
#include "Notification.h"

namespace U2 {

Notification::Notification(const QString &message, NotificationType _type, QAction *_action) :
    QLabel(qobject_cast<QWidget *>(AppContext::getMainWindow())),
    action(_action), type(_type) {
    setMinimumWidth(TT_WIDTH);
    setMaximumWidth(TT_WIDTH);
    setMaximumHeight(TT_HEIGHT);

    QString time = "[" + QTime::currentTime().toString() + "] ";
    text = time + message;

    setFrameStyle(QFrame::StyledPanel);
    close = new QLabel(this);
    QBoxLayout *h = new QHBoxLayout(this);
    setLayout(h);
    counter = 1;

    QFontMetrics metrics(font(),this);
    setText(metrics.elidedText(text, Qt::ElideRight, width()-50));
    setToolTip(text);
    
    generateCSS(false);
    generateCSSforCloseButton(false);

    setWindowFlags(Qt::ToolTip);
    close->installEventFilter(this);
    h->addStretch();
    h->addWidget(close);
    close->hide();
    setMouseTracking(true);

    close->setAttribute(Qt::WA_Hover);
    close->setFixedSize(16,16);
}

void Notification::generateCSS(bool isHovered) {
    QString bgColor;
    QString img;
    QString fontColor;
    QString border;
    switch(type) {
        case Info_Not: bgColor = "background-color: #BDE5F8;";
            fontColor = "color: #00529B;";
            img = "background-image: url(':core/images/info_notification.png');";
            break;
        case Error_Not: bgColor = "background-color: #FFBABA;";
            fontColor = "color: #D8000C;";
            img = "background-image: url(':core/images/error_notification.png');";
            break;
        case Report_Not: bgColor = "background-color: #BDE5F8;";
            fontColor = "color: #00529B;";
            img = "background-image: url(':core/images/info_notification.png');";
            break;
        case Warning_Not : bgColor = "background-color: #FCF8E3;";
            fontColor = "color: #C09853;";
            img = "background-image: url(':core/images/warning_notification.png');";
            break;
        default: assert(0);
    }

    if(isHovered) {
        border = "border: 2px solid;";
    } else {
        border = "border: 1px solid;";
    }
    QString css;

    css.append(border);
    css.append("padding: 2px 2px 2px 20px;");
    css.append("background-repeat: no-repeat;");
    css.append("background-position: left center;");
    css.append(fontColor);
    css.append(bgColor);
    css.append(img);

    setStyleSheet(css);
}

void Notification::generateCSSforCloseButton(bool isHovered) {
    QString css;
    QString background;
    if(isHovered) {
        css = "border: 1px solid;";
        background = "background-color: #C0C0C0;";
    } else {
        css = "border: none;";
        background = "background-color: transparent;";
    }
    
    css.append("border-radius: 3px;");
    css.append("background-position: center center;");
    css.append("padding: 2px 2px 2px 2px;");
    css.append(background);
    css.append("background-image: url(':core/images/close.png');");
    close->setStyleSheet(css);
}

bool Notification::event(QEvent *e) {
    if(e->type() == QEvent::ToolTip) {
        QHelpEvent *hEvent = static_cast<QHelpEvent *>(e);
        QToolTip::showText(hEvent->globalPos(), QString(text));
        return true;
    }
    if(e->type() == QEvent::HoverEnter) {
        generateCSS(true);
    }
    if(e->type() == QEvent::HoverLeave) {
        generateCSS(false);
    }
    return QWidget::event(e);
}

void Notification::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton) {
        if(timer.isActive()) {
            dissapear();
        }
        if(action) {
            action->trigger();
        } else if(!timer.isActive()){
            QObjectScopedPointer<QDialog> dlg = new QDialog(AppContext::getMainWindow()->getQMainWindow());
            dlg->setObjectName("NotificationDialog");
            QVBoxLayout vLayout;
            QHBoxLayout hLayout;
            QPushButton ok;
            QCheckBox isDelete;

            ok.setText(tr("OK"));
            isDelete.setText(tr("Remove notification after closing"));
            isDelete.setChecked(true);
            connect(&ok, SIGNAL(clicked()), dlg.data(), SLOT(accept()));
            hLayout.addWidget(&isDelete);
            hLayout.addWidget(&ok);

            dlg->setLayout(&vLayout);
            QTextBrowser txtEdit;
            txtEdit.setOpenExternalLinks(true);
            txtEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            dlg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            txtEdit.setReadOnly(true);
            txtEdit.setText(text);
            vLayout.addWidget(&txtEdit);
            vLayout.addLayout(&hLayout);

            dlg->setWindowTitle(tr("Detailed message"));

            NotificationStack *notificationStack = AppContext::getMainWindow()->getNotificationStack();
            if (NULL != notificationStack) {
                notificationStack->setFixed(true);
            }

            const int dialogResult = dlg->exec();
            CHECK(!dlg.isNull(), );

            if (QDialog::Accepted == dialogResult) {
                if(isDelete.isChecked()) {
                    emit si_delete();
                }
            }

            if (NULL != notificationStack) {
                notificationStack->setFixed(false);
            }
        }
    }
}



bool Notification::eventFilter(QObject *, QEvent *event) {
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *e= static_cast<QMouseEvent*>(event);
        if(e->button() == Qt::LeftButton) {
            emit si_delete();
            return true;
        }
    } else if(event->type() == QEvent::HoverEnter) {
        generateCSSforCloseButton(true);
    } else if(event->type() == QEvent::HoverLeave) {
        generateCSSforCloseButton(false);
    }
    return false;
}

void Notification::showNotification(int x, int y) {
    timeCounter = 0;
    timer.setInterval(20);
    connect(&timer, SIGNAL(timeout()), SLOT(sl_timeout()));
    timer.start();
    show();
    move(x,y);
    resize(TT_WIDTH, 0);
}

void Notification::switchEmbeddedVisualState() {
    setMinimumHeight(TT_HEIGHT);
    close->show();
    setAttribute(Qt::WA_Hover);
}

void Notification::dissapear() {
    hide();
    timer.stop();
    emit si_dissapear();
}


void Notification::sl_timeout() {
    QRect rect = geometry();
    if(rect.height() >= TT_HEIGHT) {
        ++timeCounter;
        if(timeCounter > 250) {
            dissapear();
        }
    } else {        
        move(rect.topLeft().x(), rect.topLeft().y() - 10);
        resize(TT_WIDTH, rect.height() + 10 > TT_HEIGHT ? TT_HEIGHT : rect.height() + 10);
    }
}

QString Notification::getText() const {
    return text;
}

NotificationType Notification::getType() const {
    return type;
}

void Notification::increaseCounter(){
    counter++;
    QFontMetrics metrics(font(),this);
    QString addText = "(" + QString::number(counter) + ")";
    int cWidth = metrics.width(addText);
    setText(metrics.elidedText(text, Qt::ElideRight, width()-50 - cWidth ) + addText);
}

NotificationStack::NotificationStack(QObject *o /* = NULL */): QObject(o), notificationPosition(0), notificationNumber(0) {
    w = new NotificationWidget(AppContext::getMainWindow()->getQMainWindow());
}


NotificationStack::~NotificationStack() {
    foreach(Notification* t, notifications) {
        delete t;
    }
    delete w;
}

bool NotificationStack::hasError() const {
    foreach(Notification *n, notifications) {
        if(n->getType() == Error_Not) {
            return true;
        }
    }
    return false;
}

void NotificationStack::addNotification(Notification *t) {
    foreach(Notification *nt, notificationsOnScreen) {
        if(nt->getText().split("]")[1] == t->getText().split("]")[1]) { //there is always minimum one ']' symbol
            nt->increaseCounter();
            delete t;
            return;
        }
    }

    if(notifications.count() >= MAX_NOTIFICATION) {
        Notification *toRemove = notifications.takeAt(0);
        if(!notificationsOnScreen.removeOne(toRemove)) {
            if(notificationsOnScreen.contains(toRemove)) {
                notificationsOnScreen.removeOne(toRemove);
                toRemove->deleteLater();
            }
        }
    }

    bool isModalWidgetActive = nullptr != QApplication::activeModalWidget() && QApplication::activeModalWidget()->isActiveWindow();
    bool onScreen = AppContext::getMainWindow()->getQMainWindow()->isActiveWindow();
    notifications.append(t);
    if (onScreen || isModalWidgetActive) {
        notificationsOnScreen.append(t);
    }
    emit si_changed();
    
    connect(t, SIGNAL(si_delete()), this, SLOT(sl_delete()), Qt::DirectConnection);
    if (onScreen || isModalWidgetActive) {
        QPoint pos = getBottomRightOfMainWindow();
        t->showNotification(pos.x() - TT_WIDTH, pos.y() - 50 - notificationPosition);
        notificationNumber++;
        notificationPosition += TT_HEIGHT;
        connect(t, SIGNAL(si_dissapear()), SLOT(sl_notificationDissapear()));
    } else {
        addToNotificationWidget(t);
    }
}

void NotificationStack::sl_notificationDissapear() {
    notificationNumber = qMax(0, notificationNumber - 1);
    notificationPosition = qMax(0, notificationPosition  - TT_HEIGHT);

    Notification *t = qobject_cast<Notification*>(sender());
    addToNotificationWidget(t);
}

void NotificationStack::addToNotificationWidget(Notification* t) {
    t->switchEmbeddedVisualState();
    t->show();
    t->setParent(w);
    w->addNotification(t);
    notificationsOnScreen.removeOne(t);
}

void NotificationStack::sl_delete() {
    Notification *t = qobject_cast<Notification*>(sender());
    int i = notifications.indexOf(t);
    assert(i != -1);
    w->removeNotification(t);
    notifications.takeAt(i);
    emit si_changed();
    //t->deleteLater();
}

int NotificationStack::count() const {
    return notifications.count();
}

Notification *NotificationStack::getNotification(int row) const {
    return notifications[row];
}

QList <Notification *> NotificationStack::getItems() const {
    return notifications;
}

void NotificationStack::showStack() {
    QPoint pos = getBottomRightOfMainWindow();

    w->move(pos.x() - w->width(), pos.y() - w->height());
    w->show();
    w->setWindowState(Qt::WindowActive);
}

void NotificationStack::setFixed(bool val) {
    w->setFixed(val);
}

void NotificationStack::addNotification(const QString& message, NotificationType type, QAction *action) {
    Notification *n = new Notification(message, type, action);
    AppContext::getMainWindow()->getNotificationStack()->addNotification(n);
}


QPoint NotificationStack::getBottomRightOfMainWindow() {
    QPoint pos;
#ifndef Q_OS_MAC
    // This behavior is correct.
    pos = AppContext::getMainWindow()->getQMainWindow()->geometry().bottomRight();
#else
    // Widget's rect doesn't know its real position on the screen. Lets calculate it manually.
    QPoint topLeft = AppContext::getMainWindow()->getQMainWindow()->mapToGlobal(QPoint(0,0));
    QSize mainWindowSize = AppContext::getMainWindow()->getQMainWindow()->geometry().size();
    pos = QPoint(topLeft.x() + mainWindowSize.width(), topLeft.y() + mainWindowSize.height());  // bottom right
    pos -= QPoint(4, 27);  // Some space for the statusbar and window's edge.
#endif
    return pos;
}

}
