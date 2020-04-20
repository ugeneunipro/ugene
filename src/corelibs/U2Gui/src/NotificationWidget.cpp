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

#include "NotificationWidget.h"


namespace U2 {

 Header:: Header(QWidget *w):QFrame(w) {
    setAutoFillBackground(true); 
    QPalette pal;
    pal.setBrush(backgroundRole(),QBrush(palette().highlight().color()));
    pal.setBrush(foregroundRole(), QBrush(Qt::white));
    setPalette(pal);
    //setBackgroundRole(QPalette::Highlight);

    close= new QLabel(this); 
    pin = new QLabel(this);

    QPixmap pix = QPixmap(":core/images/close_2.png");
    close->setPixmap(pix); 

    pix = QPixmap(":/core/images/pushpin_fixed.png");
    pin->setPixmap(pix);

    pin->setMinimumHeight(12); 
    close->setMinimumHeight(12); 

    close->setToolTip(tr("close"));
    pin->setToolTip(tr("Always on top"));

    pin->installEventFilter(this);
    close->installEventFilter(this);

    QLabel *label = new QLabel(this); 
    label->setText("Notifications"); 
    w->setWindowTitle("Notifications"); 

    QHBoxLayout *hbox = new QHBoxLayout(this); 

    hbox->addWidget(label); 
    hbox->addWidget(pin); 
    hbox->addWidget(close); 

    hbox->insertStretch(1, 500); 
    hbox->setSpacing(0); 
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 

    fix = false;
}

bool Header::isFixed() { 
    return fix; 
} 

bool Header::eventFilter(QObject *o, QEvent *e) {
    if(e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *event = static_cast<QMouseEvent *>(e);
        if(event->button() == Qt::LeftButton) {
            if(o == close) {
                parentWidget()->close();
            } else {
                fix = !fix;
                if(fix) {
                    pin->setPixmap(QPixmap(":/core/images/pushpin.png"));
                    parentWidget()->setWindowState(Qt::WindowActive);
                } else {
                    pin->setPixmap(QPixmap(":/core/images/pushpin_fixed.png"));
                    parentWidget()->setWindowState(Qt::WindowActive);
                }
            }
        }
    }
    return false;
}

void Header::mousePressEvent(QMouseEvent *me) { 
    startPos = me->globalPos(); 
    clickPos = mapToParent(me->pos()); 
    offset = startPos - mapToGlobal(QPoint(0,0));
} 
void Header::mouseMoveEvent(QMouseEvent *me) { 
    parentWidget()->move(me->globalPos() - offset);
    startPos = me->globalPos();
} 

NotificationWidget::NotificationWidget(QWidget *w): QFrame(w){
    m_mouse_down = false; 
    isFixed = false;
    setFrameShape(Panel); 

    setWindowFlags(Qt::ToolTip); 
    setMouseTracking(true); 

    frame = new QFrame();
    layout = new QVBoxLayout();
    layout->addStretch();
    layout->setMargin(3);
    frame->setLayout(layout);

    header = new Header(this); 

    scrlArea = new QScrollArea(this);

    scrlArea->setWidget(frame);
    scrlArea->setWidgetResizable(true);
    scrlArea->installEventFilter(this);

    QVBoxLayout *vbox = new QVBoxLayout(); 
    vbox->addWidget(header); 
    vbox->setMargin(0); 
    vbox->setSpacing(0); 
    setLayout(vbox);

    QVBoxLayout *lbox = new QVBoxLayout();
    lbox->addWidget(scrlArea); 
    lbox->setMargin(0); 
    lbox->setSpacing(0); 
    vbox->addLayout(lbox); 

    int newWidth = TT_WIDTH + layout->margin()*2 + 4;
    setFixedSize(newWidth, header->height() + 40);
}
  
Header *NotificationWidget::titleBar() const { 
    return header; 
} 

void NotificationWidget::setFixed(bool val) {
    isFixed = val;
}

bool NotificationWidget::event(QEvent *event) {
    if(event->type() == QEvent::WindowDeactivate) {
        if(!header->isFixed() && !isFixed) {
                close();
        }
        return false;
    }
    return QFrame::event(event);
}

void NotificationWidget::addNotification(QWidget *w) {
    int newWidth = width();
    int newHeight = height() + TT_HEIGHT;
    if(height() + TT_HEIGHT >= TS_HEIGHT) {
        newWidth = TS_WIDTH;
    }
    if(newHeight >= TS_HEIGHT) {
        newHeight = TS_HEIGHT;
    }
    setFixedSize(newWidth, newHeight);
    layout->insertWidget(0, w);
}

bool NotificationWidget::removeNotification(QWidget *w) {
    if(layout->indexOf(w) == -1) { //Can't delete notification that isn't in stack 
        return false;
    }
    layout->removeWidget(w);
    w->close();
    delete w;
    int notificationCount = layout->count();
    if(notificationCount * TT_HEIGHT + header->height() <= TS_HEIGHT) {
        int newWidth = TT_WIDTH + layout->margin()*2 + 4;
        setFixedSize(newWidth, notificationCount * (TT_HEIGHT + 6) + header->height());
    }

    if(notificationCount == 1) {
        close();
    }
    return true;
}

}
