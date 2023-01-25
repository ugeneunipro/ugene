/**
 * UGENE - Integrated Bioinformatic
 * s Tools.
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

#include <core/MainThreadRunnable.h>

#include <QCoreApplication>
#include <QThread>

#include "GTGlobals.h"
#include "core/MainThreadTimer.h"
#include "utils/GTThread.h"
#include "utils/GTUtilsDialog.h"

namespace HI {

#define GT_CLASS_NAME "GTThread"

void GTThread::waitForMainThread() {
    // Wait enough time for all active dialog waiters to be activated.
    int waitTime = GUIDialogWaiter::ACTIVATION_TIME;
    MainThreadTimer mainThreadTimer(waitTime);
    while (mainThreadTimer.getCounter() == 0) {  // Wait until incremented at least once.
        GTGlobals::sleep(waitTime);
    }
}

#define GT_METHOD_NAME "runInMainThread"
void GTThread::runInMainThread(GUITestOpStatus& os, CustomScenario* scenario) {
    qDebug("Running in the main thread");
    MainThreadRunnable::runInMainThread(os, scenario);
    qDebug("Run in the main thread is finished");
}
#undef GT_METHOD_NAME

bool GTThread::isMainThread() {
    QThread* currentThread = QThread::currentThread();
    QThread* mainThread = QCoreApplication::instance()->thread();
    return currentThread == mainThread;
}

#undef GT_CLASS_NAME

}  // namespace HI
