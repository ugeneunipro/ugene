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

#ifndef _NOTIFICATION_WIDGET_H
#define _NOTIFICATION_WIDGET_H

#include <assert.h>

#include <QObject>
#include <QPoint>

#include <QMouseEvent>
#include <QStyle>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QScrollArea>

namespace U2 {

#define TT_HEIGHT 50
#define TT_WIDTH 500

#define TS_HEIGHT 350
#define TS_WIDTH TT_WIDTH + 27

class Header: public QFrame {
Q_OBJECT

public:
    Header(QWidget *w = NULL); 
    bool isFixed();

protected: 
    void mousePressEvent(QMouseEvent *me) ; 
    void mouseMoveEvent(QMouseEvent *me) ;
    bool eventFilter(QObject *, QEvent *);

private:
    QLabel *close;
    QLabel *pin;

    bool fix;

    QPoint startPos;
    QPoint clickPos;
    QPoint offset;
};

class NotificationWidget: public QFrame {
Q_OBJECT

public:
    NotificationWidget(QWidget *w = NULL);
    Header *titleBar() const;
    void addNotification(QWidget *w);
    bool removeNotification(QWidget *w);
    void setFixed(bool val);

protected:
    bool event(QEvent *event); 

private:
    QScrollArea *scrlArea;
    QVBoxLayout *layout;
    QFrame *frame;
    Header *header;

    QPoint m_old_pos; 
    bool m_mouse_down; 
    bool left, right, bottom; 

    bool isFixed;
};

} //namespace
#endif
