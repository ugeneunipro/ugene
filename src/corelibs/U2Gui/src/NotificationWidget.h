/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <QLabel>
#include <QMouseEvent>
#include <QObject>
#include <QPoint>
#include <QScrollArea>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

namespace U2 {

#define TT_HEIGHT 50
#define TT_WIDTH 500

#define TS_HEIGHT 350
#define TS_WIDTH (TT_WIDTH + 27)

class Header : public QFrame {
    Q_OBJECT

public:
    Header(QWidget* w = nullptr);
    bool isFixed() const;

protected:
    void mousePressEvent(QMouseEvent* me);
    void mouseMoveEvent(QMouseEvent* me);
    bool eventFilter(QObject*, QEvent*);

private:
    QLabel* close;
    QLabel* pin;

    bool fix;

    QPoint startPos;
    QPoint clickPos;
    QPoint offset;
};

/** Notification widgets container. */
class NotificationWidget : public QFrame {
    Q_OBJECT

public:
    NotificationWidget(QWidget* w);
    void addNotification(QWidget* w);
    bool removeNotification(QWidget* w);
    void setFixed(bool val);

protected:
    bool event(QEvent* event);

private:
    QScrollArea* scrollArea = nullptr;
    QVBoxLayout* layout = nullptr;
    QFrame* frame = nullptr;
    Header* header = nullptr;

    bool left = false;
    bool right = false;
    bool bottom = false;
    bool isFixed = false;
};

}  // namespace U2
#endif
