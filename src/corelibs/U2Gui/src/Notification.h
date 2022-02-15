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

class NotificationStack;

class U2GUI_EXPORT Notification : public QLabel {
    Q_OBJECT

public:
    Notification(NotificationStack* stack, const QString& message, const NotificationType& type, QAction* action = nullptr);

    const QString& getText() const;

    const NotificationType& getType() const;

    QAction* getAction() const;

    /** Increments counter of the notification and re-sets on-screen timeout. */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Start on-screen timer. The notification must be positioned & shown by the external controller. */
    void showFloatingOnScreen();

    /** Hides on-screen notification. Emits si_notificationHideEvent. */
    void hideFloatingOnScreen();

    /** Increments counter 'duplicate notification' counter. */
    void incrementCounter();

    /**
     * Switches notification to the embedded visual state:
     * In the embedded state notification is shown inside NotificationWidget.
     */
    void switchEmbeddedVisualState();

    /**
     * Returns timestamp in UNIX epoch millis when the notification was shown on screen.
     * Returns 0 for notifications not on screen.
     */
    qint64 getOnScreenStartTimeMillis() const;

    /** Returns true if notification is shown on the screen. */
    bool isOnScreen() const;

    /** Time to show floating on-screen notification popover in milliseconds. */
    static constexpr int ON_SCREEN_TIMEOUT_MILLIS = 10 * 1000;

private slots:
    /**
     * Adjusts notification geometry or even hides it if it was on-screen longer than ON_SCREEN_TIMEOUT_MILLIS.
     * Geometry of the notification may be changed if internal content (like 'counter') is updated.
     */
    void sl_updateNotificationState();

private:
    /** Updates notification style. The style is based on the notification type & hovered state. */
    void updateStyle(bool isHovered);

    /** Updates 'closeButton' when hovered / un-hovered. */
    void updateCloseButtonStyle(bool isHovered);

signals:
    /** The signal is emitted after the notification is hidden. */
    void si_notificationHideEvent();

    /**
     * The signal is emitted when notification wants to be removed/closed.
     * The code must be processed by the original creator of the notification. Usually this is NotificationStack.
     */
    void si_deleteRequested();

protected:
    /** Shows tooltip & update hovered styles. */
    bool event(QEvent* e) override;

    /**
     * Triggers notification action.
     * TODO: replace all MousePress with MouseRelease events around notifications code.
     */
    void mousePressEvent(QMouseEvent* ev) override;

private:
    /** Updates display text. Display text is a timestamp + original text.  */
    void updateDisplayText();

    /**
     * Action to execute when notification is activated (clicked).
     * If action is null a popup dialog with notification text is shown.
     */
    QAction* action;

    /** A button to remove the notification. Visible only for notifications inside of the notification stack widget. */
    QLabel* closeButton;

    /** On screen timer. Updates notification geometry & size. */
    QTimer onScreenTimer;

    /** The stack this notification belongs too. Not null.*/
    NotificationStack* stack;

    /**
     * Last recorded height in on-screen stack.
     * The widget dynamically adapts to a new height and uses this field to track changes.
     */
    int lastOnScreenHeight = TT_HEIGHT;

    const QString text;

    /** Time the notification is created/updated. Unix epoch millis. */
    qint64 timestamp;

    const NotificationType type;

    /** Timestamp in millis (Unix epoch) when the notification was shown on the screen. */
    qint64 onScreenStartTimeMillis = 0;

    /** Counter for duplicate notifications. */
    int counter = 0;
};

class U2GUI_EXPORT NotificationStack : public QObject {
    Q_OBJECT

public:
    /** Instantiates notification stack that will work inside of the parentWidget. */
    NotificationStack(QWidget* parentWidget);
    ~NotificationStack();

    /** Returns number of notifications in the stack. */
    int count() const;

    /** Shows stack widget with all notifications inside. */
    void showStack();

    /** Returns true if there is at least 1 notification with error state. */
    bool hasError() const;

    /** Toggles 'fixed' flag of notification stack widget. */
    void setFixed(bool val);

    /** Adds a new instance of Notification to the notification stack. Handles duplicates (repeating notifications). */
    void add(const QString& text, const NotificationType& type = NotificationType::Info_Not, QAction* action = nullptr);

    /** Adds notification to the default UGENE's notification stack. */
    static void addNotification(const QString& text, const NotificationType& type = NotificationType::Info_Not, QAction* action = nullptr);

    /** Updates on-screen notifications positions. Compacts the onscreen stack. */
    void updateOnScreenNotificationPositions();

    /** Maximum notifications to keep in the stack. The stack includes all kind of notifications: both on screen and not. */
    static constexpr int MAX_STACK_SIZE = 100;

    /** Updates on-screen notifications & stack widget position on main window move/resize events. */
    bool eventFilter(QObject* target, QEvent* event) override;

private slots:
    /** Called when notification is hidden. The caller is an instance of 'Notification'. */
    void sl_onNotificationHidden();

    /** Called when notification wants to be deleted. The caller is an instance of 'Notification'. */
    void sl_onNotificationDeleteRequest();

signals:
    /** Emitted when a new notification is added or removed from the stack. */
    void si_changed();

private:
    /** Returns bottom position of the stack widget & on-screen floating notifications. Uses screen global coordinates. */
    QPoint getStackBottomRightPoint() const;

    // Adds notification as a child to notification widget
    void moveToNotificationWidget(Notification* t);

    /** The widget this stack is created for. */
    QWidget* parentWidget;

    /** Container with all notifications. Shown when user clicks on notification icon in status bar. */
    NotificationWidget* notificationWidget;

    QList<Notification*> notifications;
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

    void setError(const QString& error) override {
        U2OpStatus2Log::setError(error);
        NotificationStack::addNotification(error, notificationType);
    }

private:
    NotificationType notificationType;
};

}  // namespace U2

#endif
