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

#ifndef _U2_GT_UTILS_NOTIFICATIONS_H_
#define _U2_GT_UTILS_NOTIFICATIONS_H_

#include "utils/GTUtilsDialog.h"
#include "GTGlobals.h"

namespace U2 {
using namespace HI;

class NotificationChecker : public QObject {
    Q_OBJECT
public:
    NotificationChecker(HI::GUITestOpStatus &_os);
    ~NotificationChecker();
public slots:
    void sl_checkNotification();
private:
    QTimer* t;
    HI::GUITestOpStatus &os;
};

class NotificationDialogFiller : public Filler {
public:
    NotificationDialogFiller(HI::GUITestOpStatus &os, const QString &message = "");

    void commonScenario();

private:
    QString message;
};

class GTUtilsNotifications
{
public:
    static void waitForNotification(HI::GUITestOpStatus &os, bool dialogExpected = true, const QString &message = "");
};

}   // namespace U2

#endif // _U2_GT_UTILS_NOTIFICATIONS_H_
