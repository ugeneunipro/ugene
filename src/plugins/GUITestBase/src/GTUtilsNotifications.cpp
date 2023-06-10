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

#include "GTUtilsNotifications.h"
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QMainWindow>
#include <QTextBrowser>
#include <QTimer>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/Notification.h>

#include "GTUtilsMdi.h"

namespace U2 {

#define GT_CLASS_NAME "NotificationChecker"

NotificationChecker::NotificationChecker() {
    t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(sl_checkNotification()));
    t->start(100);
}

#define GT_METHOD_NAME "sl_checkNotification"
void NotificationChecker::sl_checkNotification() {
    CHECK(QApplication::activeModalWidget() == nullptr, );  // Active modal widget will prevent click on the notification.
    // Activate floating notification.
    QWidgetList widgetList = QApplication::topLevelWidgets();
    for (QWidget* widget : qAsConst(widgetList)) {
        auto notification = qobject_cast<Notification*>(widget);
        if (notification != nullptr && notification->isVisible()) {
            uiLog.trace("notification is found");
            t->stop();
            GTWidget::click(notification);
            return;
        }
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "NotificationDialogFiller"

NotificationDialogFiller::NotificationDialogFiller(const QString& message)
    : Filler("NotificationDialog"),
      message(message) {
    settings.timeout = 350000;
}

#define GT_METHOD_NAME "commonScenario"
void NotificationDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    if (!message.isEmpty()) {
        QTextBrowser* tb = dialog->findChild<QTextBrowser*>();
        GT_CHECK(tb != nullptr, "text browser not found");
        QString actualMessage = tb->toPlainText();
        GT_CHECK(actualMessage.contains(message), "unexpected message: " + actualMessage);
    }
    QWidget* ok = GTWidget::findButtonByText("Ok", dialog);
    GTWidget::click(ok);
    if (isOsWindows() || isOsMac()) {
        dialog = QApplication::activeModalWidget();
        if (dialog != nullptr) {
            ok = GTWidget::findButtonByText("Ok", dialog);
            GTWidget::click(ok);
        }
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "NotificationChecker"

#define GT_METHOD_NAME "waitForNotification"
void GTUtilsNotifications::waitForNotification(bool dialogExpected, const QString& message) {
    if (dialogExpected) {
        GTUtilsDialog::waitForDialog(new NotificationDialogFiller(message));
    }
    new NotificationChecker();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNotificationReportText"
void GTUtilsNotifications::checkNotificationReportText(const QString& textToken) {
    checkNotificationReportText(QStringList(textToken));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNotificationReportTextList"
void GTUtilsNotifications::checkNotificationReportText(const QStringList& textTokens) {
    clickOnNotificationWidget();

    QWidget* reportWindow = GTUtilsMdi::checkWindowIsActive("Task report ");
    auto reportEdit = GTWidget::findTextEdit("reportTextEdit", reportWindow);
    QString html = reportEdit->toHtml();
    for (const QString& textToken : qAsConst(textTokens)) {
        CHECK_SET_ERR(html.contains(textToken), "Report contains expected text: " + textToken);
    }
    GTUtilsMdi::closeActiveWindow();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNotificationDialogText"
void GTUtilsNotifications::checkNotificationDialogText(const QString& textToken) {
    GTUtilsDialog::waitForDialog(new NotificationDialogFiller(textToken));
    clickOnNotificationWidget();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOnNotificationWidget"
void GTUtilsNotifications::clickOnNotificationWidget() {
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QWidgetList widgetList = QApplication::allWidgets();
        for (QWidget* widget : qAsConst(widgetList)) {
            auto notification = qobject_cast<Notification*>(widget);
            if (notification != nullptr && notification->isVisible()) {
                GTWidget::click(notification);
                return;
            }
        }
    }
    GT_FAIL("Notification widget is not found!", );
}
#undef GT_METHOD_NAME

/** Returns any visible notification popover widget or nullptr if no widget is found.*/
static QWidget* findAnyVisibleNotificationWidget() {
    QList<QWidget*> list = QApplication::allWidgets();
    foreach (QWidget* wid, list) {
        auto notification = qobject_cast<Notification*>(wid);
        if (notification != nullptr && notification->isVisible()) {
            return notification;
        }
    }
    return nullptr;
}

#define GT_METHOD_NAME "waitAllNotificationsClosed"
void GTUtilsNotifications::waitAllNotificationsClosed() {
    QWidget* notification = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        notification = findAnyVisibleNotificationWidget();
        if (notification == nullptr) {
            break;
        }
    }
    GT_CHECK(notification == nullptr, "Notification is still active after timeout!");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoVisibleNotifications"
void GTUtilsNotifications::checkNoVisibleNotifications() {
    QWidget* notification = findAnyVisibleNotificationWidget();
    GT_CHECK(notification == nullptr, "Found active notification!");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNotificationCounterValue"
QString GTUtilsNotifications::getNotificationCounterValue() {
    auto statusBar = GTWidget::findWidget("taskStatusBar");
    return GTWidget::findLabel("notificationLabel", statusBar)->property("notifications-count").toString();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openNotificationContainerWidget"
QWidget* GTUtilsNotifications::openNotificationContainerWidget() {
    auto statusBar = GTWidget::findWidget("taskStatusBar");
    auto label = GTWidget::findLabel("notificationLabel", statusBar);
    GTWidget::click(label);
    return GTWidget::findWidget("NotificationWidget");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
