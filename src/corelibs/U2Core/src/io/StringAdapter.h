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

#ifndef _U2_STRING_ADAPTER_H_
#define _U2_STRING_ADAPTER_H_

#include <U2Core/IOAdapter.h>

namespace U2 {

class U2CORE_EXPORT StringAdapterFactory: public IOAdapterFactory {
    Q_OBJECT
public:
    StringAdapterFactory(QObject* p = NULL);

    virtual IOAdapter* createIOAdapter();

    virtual IOAdapterId getAdapterId() const {return BaseIOAdapters::STRING;}

    virtual const QString& getAdapterName() const {return name;}

    virtual bool isIOModeSupported(IOAdapterMode m)  const {Q_UNUSED(m); return true;} //data can be read and be written

    virtual TriState isResourceAvailable(const GUrl& url) const {
        Q_UNUSED(url);
        return TriState_Yes;
    }

protected:
    QString name;
};

class U2CORE_EXPORT StringAdapterFactoryWithStringData: public StringAdapterFactory {
    Q_OBJECT
public:
    StringAdapterFactoryWithStringData(const QString &data, QObject *parent = NULL);

    virtual IOAdapter* createIOAdapter();
private:
    const QString data;
};

class U2CORE_EXPORT StringAdapter: public IOAdapter {
    Q_OBJECT
public:
    StringAdapter(StringAdapterFactory* f, QObject* o = NULL);
    ~StringAdapter() {if (isOpen()) close();}

    /** Creates opened adapter */
    StringAdapter(const QByteArray &data, StringAdapterFactory* f = NULL);

    virtual bool open(const GUrl& url, IOAdapterMode m);

    virtual bool isOpen() const {return opened;}

    virtual void close();

    virtual qint64 readBlock(char* data, qint64 maxSize);

    virtual qint64 writeBlock(const char* data, qint64 size);

    virtual bool skip(qint64 nBytes);

    virtual qint64 left() const;

    virtual int getProgress() const;

    virtual qint64 bytesRead() const;

    virtual GUrl getURL() const;

    const QByteArray & getBuffer() {return buffer;}

    virtual QString errorString() const;

private:
    bool opened;
    QByteArray buffer;
    int pos;
    GUrl url;
};


} // U2

#endif // _U2_STRING_ADAPTER_H_
