/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

#include <QAction>
#include <QCheckBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/global.h>

#include "NotificationWidget.h"
#include "NotificationsTypes.h"

namespace U2 {

#define MAX_NOTIFICATION 100

class U2GUI_EXPORT Notification : public QLabel {
    Q_OBJECT

public:
    Notification(const QString &message, NotificationType _type, QAction *_action = 0);

    void showNotification(int x, int y);

    QString getText() const;

    NotificationType getType() const;

    bool eventFilter(QObject *watched, QEvent *event) override;

    void incrementCounter();

    /**
     * Switches notification to the embedded visual state:
     * In the embedded state notification is shown inside NotificationWidget.
     */
    void switchEmbeddedVisualState();

private slots:
    void sl_timeout();

private:
    void hideNotification();
    void updateStyle(bool isHovered);
    void updateCloseButtonStyle(bool isHovered);

signals:
    /** The signal is emitted when notification is hidden. */
    void si_notificationHideEvent();
    void si_delete();

protected:
    bool event(QEvent *e) override;
    void mousePressEvent(QMouseEvent *ev) override;

private:
    QAction *action;
    QLabel *close;
    QTimer timer;

    QString text;
    NotificationType type;
    int timeCounter = 0;
    //counter for duplicate notifications
    int counter = 0;
};

class U2GUI_EXPORT NotificationStack : public QObject {
    Q_OBJECT

public:
    NotificationStack(QObject *o = NULL);
    ~NotificationStack();

    void addNotification(Notification *t);
    int count() const;
    Notification *getNotification(int row) const;
    QList<Notification *> getItems() const;
    void showStack();
    bool hasError() const;
    void setFixed(bool val);

    // just a shortcut to write less
    static void addNotification(const QString &message, NotificationType type, QAction *action = 0);

private slots:
    /** Called when notification is hidden. The called is the 'Notification' instance. */
    void sl_onNotificationHidden();
    void sl_delete();

signals:
    void si_changed();

private:
    static QPoint getBottomRightOfMainWindow();    // because of Mac's strange behavior

    // Adds notification as a child to notification widget
    void addToNotificationWidget(Notification *n);

    NotificationWidget *notificationWidget;

    QList<Notification *> notifications;
    QList<Notification *> notificationsOnScreen;
    int notificationPosition;
    int notificationNumber;
};

/**
    Used to handle important errors that needs to be reported to user.
    Dumps error to coreLog while showing notification to user.
    LogLevel and NotificationType can be passed as params.
    Defaults are LogLevel_ERROR and Error_Not, respectively
*/
class U2GUI_EXPORT U2OpStatus2Notification : public U2OpStatus2Log {
public:
    U2OpStatus2Notification(NotificationType type = Error_Not, LogLevel level = LogLevel_ERROR)
        : U2OpStatus2Log(level), notificationType(type) {
    }

    void setError(const QString &error) override {
        U2OpStatus2Log::setError(error);
        NotificationStack::addNotification(error, notificationType);
    }

private:
    NotificationType notificationType;
};

}    // namespace U2

#endif
