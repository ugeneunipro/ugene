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

#include "LocalFileAdapter.h"
#include "ZlibAdapter.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/TextUtils.h>

namespace U2 {

LocalFileAdapterFactory::LocalFileAdapterFactory(QObject* o) : IOAdapterFactory(o) {
    name = tr("Local file");
}

IOAdapter* LocalFileAdapterFactory::createIOAdapter() {
    return new LocalFileAdapter(this);
}

GzippedLocalFileAdapterFactory::GzippedLocalFileAdapterFactory(QObject* o)
    : LocalFileAdapterFactory(o) {
    name = tr("GZIP file");
}

IOAdapter* GzippedLocalFileAdapterFactory::createIOAdapter() {
    return new ZlibAdapter(new LocalFileAdapter(this));
}

const quint64 LocalFileAdapter::BUF_SIZE = DocumentFormat::READ_BUFF_SIZE;

LocalFileAdapter::LocalFileAdapter(LocalFileAdapterFactory* factory, QObject* o, bool b)
    : IOAdapter(factory, o), f(NULL), fileSize(0), bufferOptimization(b) {
    bufferOptimization = true;
    if (bufferOptimization) {
        buffer = QByteArray(BUF_SIZE, '\0');
        bufData = buffer.data();
    } else {
        bufData = NULL;
    }
    bufLen = 0;
    currentPos = 0;
}

LocalFileAdapter::~LocalFileAdapter() {
    if (isOpen()) {
        close();
    }
}

bool LocalFileAdapter::open(const GUrl& url, IOAdapterMode m) {
    SAFE_POINT(!isOpen(), "Adapter is already opened!", false);
    SAFE_POINT(f == NULL, "QFile is not null!", false);

    if (url.isEmpty()) {
        return false;
    }
    f = new QFile(url.getURLString());
    QIODevice::OpenMode iomode;
    switch (m) {
    case IOAdapterMode_Read: iomode = QIODevice::ReadOnly; break;
    case IOAdapterMode_Write: iomode = QIODevice::WriteOnly | QIODevice::Truncate; break;
    case IOAdapterMode_Append: iomode = QIODevice::WriteOnly | QIODevice::Append; break;
    }
    bool res = f->open(iomode);
    if (!res) {
        delete f;
        f = NULL;
        return false;
    }
    fileSize = f->size();
    return true;
}

bool LocalFileAdapter::isOpen() const {
    return f != NULL;
}

void LocalFileAdapter::close() {
    SAFE_POINT(isOpen(), "Adapter is not opened!", );
    f->close();
    delete f;
    f = NULL;
    fileSize = 0;
}

qint64 LocalFileAdapter::readBlock(char* data, qint64 size) {
    SAFE_POINT(isOpen(), "Adapter is not opened!", -1);
    qint64 l = 0;
    if (bufferOptimization) {
        qint64 copySize = 0;
        while (l < size) {
            if (currentPos == bufLen) {
                bufLen = f->read(bufData, BUF_SIZE);
                if (formatMode == TextMode) {
                    bufLen = TextUtils::cutByteOrderMarks(bufData, errorMessage, bufLen);
                }
                if (bufLen == -1 || hasError()) {
                    //error
                    return -1;
                }
                currentPos = 0;
            }
            copySize = qMin(bufLen - currentPos, size - l);
            if (0 == copySize || hasError()) {
                break;
            }
            memcpy(data + l, bufData + currentPos, copySize);
            l += copySize;
            currentPos += copySize;
        }
    } else {
        l = f->read(data, size);
        if (formatMode == TextMode) {
            l = TextUtils::cutByteOrderMarks(bufData, errorMessage, l);
        }
        CHECK(!hasError(), -1);
    }
    return l;
}

qint64 LocalFileAdapter::writeBlock(const char* data, qint64 size) {
    SAFE_POINT(isOpen(), "Adapter is not opened!", -1);
    qint64 l = f->write(data, size);
    fileSize += size;
    return l;
}

bool LocalFileAdapter::skip(qint64 nBytes) {
    SAFE_POINT(isOpen(), "Adapter is not opened!", false);
    if (bufferOptimization) {
        qint64 newPos = currentPos + nBytes;
        if (newPos < 0 || newPos >= bufLen) {
            qint64 p = f->pos();
            bool res = f->seek((p - bufLen + currentPos) + nBytes);

            bufLen = 0;
            currentPos = 0;
            return res;
        } else {
            currentPos = newPos;
            return true;
        }
    } else {
        qint64 p = f->pos();
        return f->seek(p + nBytes);
    }
}

qint64 LocalFileAdapter::left() const {
    SAFE_POINT(isOpen(), "Adapter is not opened!", -1);
    qint64 p = f->pos();
    if (bufferOptimization) {
        p -= bufLen - currentPos;
    }
    return fileSize - p;
}

int LocalFileAdapter::getProgress() const {
    SAFE_POINT(isOpen(), "Adapter is not opened!", false);
    return int(100 * float(bytesRead()) / fileSize);
}

qint64 LocalFileAdapter::bytesRead() const {
    qint64 p = f->pos();
    if (bufferOptimization) {
        p -= bufLen - currentPos;
    }
    return p;
}

GUrl LocalFileAdapter::getURL() const {
    return GUrl(f->fileName(), GUrl_File);
}

QString LocalFileAdapter::errorString() const {
    return (f->error() == QFile::NoError) ? errorMessage : f->errorString();
}

};//namespace
