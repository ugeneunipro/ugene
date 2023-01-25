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

void GTGlobals::sleep(int msec) {
    if (msec > 0) {
        QTest::qWait(msec);
    }
}

void GTGlobals::sendEvent(QObject* obj, QEvent* e) {
    QSpontaneKeyEvent::setSpontaneous(e);
    qApp->notify(obj, e);
}

#define GT_METHOD_NAME "takeScreenShot"
QImage GTGlobals::takeScreenShot(HI::GUITestOpStatus& os) {
    if (GTThread::isMainThread()) {
        return QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()).toImage();
    }
    class TakeScreenshotScenario : public CustomScenario {
    public:
        TakeScreenshotScenario(QImage& _image)
            : image(_image) {
        }
        void run(HI::GUITestOpStatus&) override {
            image = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId()).toImage();
        }
        QImage& image;
    };
    QImage image;
    GTThread::runInMainThread(os, new TakeScreenshotScenario(image));
    return image;
}
#undef GT_METHOD_NAME

void GTGlobals::takeScreenShot(HI::GUITestOpStatus& os, const QString& path) {
    QImage originalImage = takeScreenShot(os);
    bool ok = originalImage.save(path);
    CHECK_SET_ERR(ok, "Failed to save pixmap to file: " + path);
}

GTGlobals::FindOptions::FindOptions(bool _failIfNotFound, Qt::MatchFlags _matchPolicy, int _depth)
    : failIfNotFound(_failIfNotFound),
      matchPolicy(_matchPolicy),
      depth(_depth) {
}

void GTGlobals::GUITestFail() {
    qCritical("\nGT_DEBUG_MESSAGE !!!FIRST FAIL");
}

#undef GT_CLASS_NAME

}  // namespace HI
