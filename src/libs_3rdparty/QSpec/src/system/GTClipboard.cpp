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

#include "system/GTClipboard.h"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTClipboard"

#define GT_METHOD_NAME "text"
QString GTClipboard::text() {
    GTGlobals::sleep(300);

    // check that clipboard contains text
    QString clipboardText;
    class Scenario : public CustomScenario {
    public:
        Scenario(QString& _text)
            : text(_text) {
        }
        void run() {
            QClipboard* clipboard = QApplication::clipboard();

            GT_CHECK(clipboard != NULL, "Clipboard is NULL");
            const QMimeData* mimeData = clipboard->mimeData();
            GT_CHECK(mimeData != NULL, "Clipboard MimeData is NULL");

            GT_CHECK(mimeData->hasText(), "Clipboard doesn't contain text data");
            text = QString(mimeData->text());  // make a copy of clipboard text.
        }

    private:
        QString& text;
    };

    GTThread::runInMainThread(new Scenario(clipboardText));
    GTThread::waitForMainThread();
    return clipboardText;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkHasNonEmptyImage"
void GTClipboard::checkHasNonEmptyImage() {
    class Scenario : public CustomScenario {
    public:
        Scenario(QImage& _image)
            : image(_image) {
        }
        void run() {
            QClipboard* clipboard = QApplication::clipboard();
            const QMimeData* mimeData = clipboard->mimeData();
            GT_CHECK(mimeData->hasImage(), "Clipboard doesn't contain image data");
            QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
            image = pixmap.toImage();
        }

    private:
        QImage& image;
    };

    QImage image;
    GTThread::runInMainThread(new Scenario(image));
    GTThread::waitForMainThread();
    GT_CHECK(!image.isNull(), "Clipboard image is empty");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setText"
void GTClipboard::setText(QString text) {
    class Scenario : public CustomScenario {
    public:
        Scenario(QString _text)
            : text(_text) {
        }
        void run() {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->clear();
            clipboard->setText(text);
        }

    private:
        QString text;
    };

    GTThread::runInMainThread(new Scenario(text));
    GTThread::waitForMainThread();
}

#undef GT_METHOD_NAME

namespace {
QList<QUrl> toLocalQUrls(const QList<QString>& urls) {
    QList<QUrl> qurls;
    for (const QString& url : qAsConst(urls)) {
        QFileInfo fi(url);
        if (fi.isAbsolute()) {
            qurls.append(QUrl::fromLocalFile(url));
        } else if (fi.makeAbsolute()) {
            QString absolutePath = fi.absoluteFilePath();
            qurls.append(QUrl::fromLocalFile(absolutePath));
        } else {
            GT_FAIL("Cannot make an absolute path: " + url, qurls);
        }
    }
    return qurls;
}
}  // namespace

#define GT_METHOD_NAME "setUrls"
void GTClipboard::setUrls(const QList<QString>& urls) {
    class Scenario : public CustomScenario {
        QList<QUrl> urls;

    public:
        Scenario(const QList<QUrl>& urls)
            : urls(urls) {
        }

        void run() {
            QMimeData* urlMime = new QMimeData();
            urlMime->setUrls(urls);

            QClipboard* clipboard = QApplication::clipboard();
            clipboard->clear();
            clipboard->setMimeData(urlMime);
        }
    };

    QList<QUrl> qurls = toLocalQUrls(urls);
    GTThread::runInMainThread(new Scenario(qurls));
    GTThread::waitForMainThread();
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clear"
void GTClipboard::clear() {
    class Scenario : public CustomScenario {
    public:
        void run() override{
            QApplication::clipboard()->clear();
        }
    };

    GTThread::runInMainThread(new Scenario());
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
