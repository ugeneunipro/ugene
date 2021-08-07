/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include <QtCore/QEventLoop>
#include <QtTest/QSpontaneKeyEvent>
#include <QtTest>
#if (QT_VERSION < 0x050000)  // Qt 5
#    include <QtGui/QApplication>
#    include <QtGui/QDesktopWidget>
#    include <QtGui/QPixmap>
#else
#    include <QtGui/QScreen>
#    include <QtWidgets/QApplication>
#    include <QtWidgets/QDesktopWidget>
#endif

#include "GTGlobals.h"

#ifdef Q_OS_WIN
#    include <windows.h>
#else
#    include <unistd.h>
#endif

namespace HI {
namespace {
void sysSleep(int sec) {
#ifdef Q_OS_WIN
    Sleep(1000 * sec);
#else
    sleep(sec);
#endif
}
}  // namespace

void GTGlobals::sleep(int msec) {
    if (msec > 0) {
        QTest::qWait(msec);
    }
}

void GTGlobals::systemSleep(int sec) {
    sysSleep(sec);
}

void GTGlobals::sendEvent(QObject *obj, QEvent *e) {
    QSpontaneKeyEvent::setSpontaneous(e);
    qApp->notify(obj, e);
}

void GTGlobals::takeScreenShot(const QString &path) {
    QPixmap originalPixmap = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
    originalPixmap.save(path);
}

GTGlobals::FindOptions::FindOptions(bool _failIfNotFound, Qt::MatchFlags _matchPolicy, int _depth)
    : failIfNotFound(_failIfNotFound),
      matchPolicy(_matchPolicy),
      depth(_depth) {
}

void GTGlobals::GUITestFail() {
    qCritical("\nGT_DEBUG_MESSAGE !!!FIRST FAIL");
}

}  // namespace HI
