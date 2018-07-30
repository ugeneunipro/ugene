/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "StringAdapter.h"

namespace U2 {

StringAdapterFactory::StringAdapterFactory(QObject* o) : IOAdapterFactory(o) {
    name = tr("String buffer");
}

IOAdapter* StringAdapterFactory::createIOAdapter() {
    return new StringAdapter(this);
}

StringAdapter::StringAdapter(StringAdapterFactory *f, QObject *o)
: IOAdapter(f, o), opened(false), pos(0)
{

}

StringAdapter::StringAdapter(const QByteArray &data, StringAdapterFactory* f)
: IOAdapter(f), buffer(data)
{
    opened = true;
    pos = 0;
}

bool StringAdapter::open(const GUrl &_url, IOAdapterMode m) {
    url = _url;
    switch (m) {
        case IOAdapterMode_Write:
            buffer.clear();
            pos = 0;
            break;
        case IOAdapterMode_Read:
            pos = 0;
            break;
        case IOAdapterMode_Append:
            pos = buffer.length();
            break;
    }
    opened = true;

    return true;
}

void StringAdapter::close() {
    buffer.clear();
    opened = false;
}

qint64 StringAdapter::readBlock(char * data, qint64 maxSize) {
    qint64 size = qMin<qint64>((buffer.length() - pos), maxSize);
    memcpy(data, buffer.constData() + pos, size);
    if (formatMode == TextMode) {
        cutByteOrderMarks(data, size);
    }
    pos += size;
    return size;
}

qint64 StringAdapter::writeBlock(const char* data, qint64 size) {
    QByteArray ar(data, size);
    buffer.insert(pos, ar);
    pos += size;
    return size;
}

bool StringAdapter::skip(qint64 nBytes) {
    if (nBytes >= 0) {
        qint64 size = qMin<qint64>((buffer.length() - pos), nBytes);
        pos += size;
    } else {
        qint64 size = qMin<qint64>(pos, -nBytes);
        pos -= size;
    }
    return true;
}

qint64 StringAdapter::left() const {
    // todo
    return -1;
}

int StringAdapter::getProgress() const {
    if (buffer.isEmpty()) {
        return -1;
    }
    return (100 * pos) / buffer.length();
}

qint64 StringAdapter::bytesRead() const {
    return pos;
}

QString StringAdapter::errorString() const {
    return "";
}

GUrl StringAdapter::getURL() const {
    return url;
}

StringAdapterFactoryWithStringData::StringAdapterFactoryWithStringData(const QString &data, QObject *parent) :
StringAdapterFactory(parent), data(data){}

IOAdapter* StringAdapterFactoryWithStringData::createIOAdapter() {
    return new StringAdapter(data.toLatin1(), this);
}

} // U2
