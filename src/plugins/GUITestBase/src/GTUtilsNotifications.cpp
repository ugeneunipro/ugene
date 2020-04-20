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
#include <QMainWindow>
#include <QTextBrowser>
#include <QTimer>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/Notification.h>

#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include <primitives/GTWidget.h>

namespace U2 {

#define GT_CLASS_NAME "NotificationChecker"

NotificationChecker::NotificationChecker(HI::GUITestOpStatus &_os):os(_os){
    t = new QTimer(this);
    t->connect(t, SIGNAL(timeout()), this, SLOT(sl_checkNotification()));
    t->start(100);
}

NotificationChecker::~NotificationChecker(){
    delete t;
}

#define GT_METHOD_NAME "sl_checkNotification"
void NotificationChecker::sl_checkNotification() {
    CHECK(NULL == QApplication::activeModalWidget(), );
    QList<QWidget*> list = QApplication::allWidgets();
    foreach(QWidget* wid, list){
        Notification* notif = qobject_cast<Notification*>(wid);
        if(notif!=NULL && notif->isVisible()){
            uiLog.trace("found");
            GTWidget::click(os, notif);
            t->stop();
            return;
        }
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "NotificationDialogFiller"

NotificationDialogFiller::NotificationDialogFiller(HI::GUITestOpStatus &os, const QString &message) :
    Filler(os, "NotificationDialog"),
    message(message)
{
    settings.timeout = 350000;
}

#define GT_METHOD_NAME "commonScenario"
void NotificationDialogFiller::commonScenario() {
    GTGlobals::sleep(1000);
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "active modal widget is invalid");

    if(!message.isEmpty()){
        QTextBrowser* tb = dialog->findChild<QTextBrowser*>();
        GT_CHECK(tb != NULL, "text browser not found");
        QString actualMessage = tb->toPlainText();
        GT_CHECK(actualMessage.contains(message), "unexpected message: " + actualMessage);
    }

    QWidget* ok = GTWidget::findButtonByText(os, "Ok", dialog);
    GTWidget::click(os, ok);
#if defined Q_OS_WIN || defined Q_OS_MAC
    dialog = QApplication::activeModalWidget();
    if (dialog != NULL) {
        ok = GTWidget::findButtonByText(os, "Ok", dialog);
        GTWidget::click(os, ok);
    }
#endif

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "NotificationChecker"
#define GT_METHOD_NAME "waitForNotification"
void GTUtilsNotifications::waitForNotification(HI::GUITestOpStatus &os, bool dialogExpected, const QString &message){
    if (dialogExpected) {
        GTUtilsDialog::waitForDialog(os, new NotificationDialogFiller(os, message));
    }
    new NotificationChecker(os);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}   // namespace U2
