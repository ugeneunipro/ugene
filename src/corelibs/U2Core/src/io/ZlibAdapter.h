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

#ifndef _U2_ZLIB_ADAPTER_H_
#define _U2_ZLIB_ADAPTER_H_

#include <U2Core/IOAdapter.h>
#include "RingBuffer.h"

namespace U2 {

class GzipUtil;
struct GZipIndex;
struct GZipIndexAccessPoint;

class U2CORE_EXPORT ZlibAdapter: public IOAdapter {
    Q_OBJECT
public:
    ZlibAdapter(IOAdapter* io);
    ~ZlibAdapter();

    virtual bool open(const GUrl& url, IOAdapterMode m_ );

    virtual bool isOpen() const;

    virtual void close();

    virtual qint64 readBlock(char* data, qint64 maxSize);

    virtual qint64 writeBlock(const char* data, qint64 size);

    virtual bool skip(qint64 nBytes);

    virtual qint64 left() const;

    virtual int getProgress() const;

    virtual qint64 bytesRead() const;

    virtual GUrl getURL() const;

    /**
     * should be invoked after open() ( needs z not null )
     */
    bool skip( const GZipIndexAccessPoint& point, qint64 offset );

    /**
     * returns -1 if a file is failed to open
     */
    static qint64 getUncompressedFileSizeInBytes(const GUrl &url);

    virtual QString errorString() const;

private:
    static const int BUFLEN = 32768;
    IOAdapter* io;
    GzipUtil* z;
    RingBuffer* buf; // seek buffer
    int rewinded; // how much should read from seek buffer
};

struct GZipIndexAccessPoint {
    qint64     out;    // corresponding offset in uncompressed data
    qint64     in;     // offset in input file of first full byte
    int        bits;   // number of bits (1-7) from byte at in - 1, or 0
    QByteArray window; //preceding WINSIZE of uncompressed data
};

struct U2CORE_EXPORT GZipIndex {
    static const int    WINSIZE = 32768;
    static const qint64 SPAN    = 1048576L;
    static const int    CHUNK   = 16384;

    QList< GZipIndexAccessPoint > points;
}; // GZipIndex

};//namespace

#endif
