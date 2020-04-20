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

#ifndef _U2_LOCAL_FILE_ADAPTER_H_
#define _U2_LOCAL_FILE_ADAPTER_H_

#include <U2Core/IOAdapter.h>

#include <QFileInfo>

namespace U2 {

class U2CORE_EXPORT LocalFileAdapterFactory: public IOAdapterFactory {
    Q_OBJECT
public:
    LocalFileAdapterFactory(QObject* p = NULL);

    virtual IOAdapter* createIOAdapter();

    virtual IOAdapterId getAdapterId() const {return BaseIOAdapters::LOCAL_FILE;}

    virtual const QString& getAdapterName() const {return name;}

    virtual bool isIOModeSupported(IOAdapterMode m)  const {Q_UNUSED(m); return true;} //files can be read and be written

    virtual TriState isResourceAvailable(const GUrl& url) const {return QFileInfo(url.getURLString()).exists() ? TriState_Yes : TriState_No;}

protected:
    QString name;
};

class U2CORE_EXPORT GzippedLocalFileAdapterFactory: public LocalFileAdapterFactory {
    Q_OBJECT
public:
    GzippedLocalFileAdapterFactory(QObject* p = NULL);

    virtual IOAdapter* createIOAdapter();

    virtual IOAdapterId getAdapterId() const {return BaseIOAdapters::GZIPPED_LOCAL_FILE;}
};



class U2CORE_EXPORT LocalFileAdapter: public IOAdapter {
    Q_OBJECT
public:
    LocalFileAdapter(LocalFileAdapterFactory* f, QObject* o = NULL, bool bufferOptimization = false);
    ~LocalFileAdapter();

    virtual bool open(const GUrl& url, IOAdapterMode m);

    virtual bool isOpen() const;

    virtual void close();

    virtual qint64 readBlock(char* data, qint64 maxSize);

    virtual qint64 writeBlock(const char* data, qint64 size);

    virtual bool skip(qint64 nBytes);

    virtual qint64 left() const;

    virtual int getProgress() const;

    virtual qint64 bytesRead() const;

    virtual GUrl getURL() const;

    virtual QString errorString() const;


private:
    QFile* f;
    quint64 fileSize;

    bool bufferOptimization;
    QByteArray buffer;
    char *bufData;
    qint64 bufLen;
    qint64 currentPos;
    static const quint64 BUF_SIZE;
};


}//namespace

#endif
