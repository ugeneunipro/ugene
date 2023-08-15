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

#include "GTGlobals.h"

#include <QtCore/QEventLoop>
#include <QtGui/QScreen>
#include <QtTest/QSpontaneKeyEvent>
#include <QtTest>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include "core/CustomScenario.h"
#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTGlobals"

void GTGlobals::sleep(int msec, const QString& reason) {
    if (msec > 0) {
        if (reason.isEmpty()) {
            GT_LOG(QString("GTGlobals::sleep %1ms").arg(msec));
        } else {
            GT_LOG(QString("GTGlobals::sleep %1ms, reason: %2").arg(msec).arg(reason));
        }
        QTest::qWait(msec);
    }
}

void GTGlobals::sendEvent(QObject* obj, QEvent* e) {
    QSpontaneKeyEvent::setSpontaneous(e);
    qApp->notify(obj, e);
}

QImage GTGlobals::takeScreenShot() {
    if (GTThread::isMainThread()) {
        return QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()).toImage();
    }
    class TakeScreenshotScenario : public CustomScenario {
    public:
        TakeScreenshotScenario(QImage& _image)
            : image(_image) {
        }
        void run() override {
            image = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()).toImage();
        }
        QImage& image;
    };
    QImage image;
    GTThread::runInMainThread(new TakeScreenshotScenario(image));
    return image;
}

void GTGlobals::takeScreenShot(const QString& path) {
    QImage originalImage = takeScreenShot();
    bool ok = originalImage.save(path);
    CHECK_SET_ERR(ok, "Failed to save pixmap to file: " + path);
}

GTGlobals::FindOptions::FindOptions(bool _failIfNotFound, Qt::MatchFlags _matchPolicy, int _depth)
    : failIfNotFound(_failIfNotFound),
      matchPolicy(_matchPolicy),
      depth(_depth) {
}

void GTGlobals::logFirstFail() {
    qCritical("\nGT_DEBUG_MESSAGE !!!FIRST FAIL");
}

static GUITestOpStatus* activeOp = new GUITestOpStatus();

/** Returns active GUITestOpStatus instance. */
GUITestOpStatus& GTGlobals::getOpStatus() {
    return *activeOp;
}

/** Resets active GUITestOpStatus instance to a new one with no error inside. */
void GTGlobals::resetOpStatus() {
    delete activeOp;
    activeOp = new GUITestOpStatus();
}

#undef GT_CLASS_NAME

}  // namespace HI
