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

#pragma once

#include "GTGlobals.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class NotificationChecker : public QObject {
    Q_OBJECT
public:
    NotificationChecker();

public slots:
    void sl_checkNotification();

private:
    QTimer* t;
};

class NotificationDialogFiller : public Filler {
public:
    NotificationDialogFiller(const QString& message = "");

    void commonScenario();

private:
    QString message;
};

class GTUtilsNotifications {
public:
    static void waitForNotification(bool dialogExpected = true, const QString& message = "");

    /** Waits for notification, clicks it and checks that the notification report contains the required text tokens. */
    static void checkNotificationReportText(const QString& textToken);
    static void checkNotificationReportText(const QStringList& textTokenList);

    /** Waits for notification, clicks it and checks that the modal dialog has the required text. */
    static void checkNotificationDialogText(const QString& textToken);

    /** Clicks on the active (first) notification widget. */
    static void clickOnNotificationWidget();

    /** Waits until all notification popups are closed. */
    static void waitAllNotificationsClosed();

    /** Checks that there are no notification balloons on the screen. */
    static void checkNoVisibleNotifications();

    /** Returns text shown on the notification container button in the status bar. */
    static QString getNotificationCounterValue();

    /** Opens notification container widget and returns it. */
    static QWidget* openNotificationContainerWidget();
};

}  // namespace U2
