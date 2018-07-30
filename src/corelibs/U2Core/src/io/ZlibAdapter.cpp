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

#include <qendian.h>

#include <U2Core/TextUtils.h>

#include "LocalFileAdapter.h"

#include "ZlibAdapter.h"

#include <3rdparty/zlib/zlib.h>

#include <assert.h>

namespace {

using namespace U2;

// used in GZipIndex building
void setIfYouCan( bool what, bool* to ) {
    if( NULL != to ) {
        *to = what;
    }
}
// used in GZipIndex building
void addAccessPoint( GZipIndex& index, int bits, qint64 in, qint64 out, quint32 left, char* wnd ) {
    assert( NULL != wnd );

    QByteArray window;
    GZipIndexAccessPoint next;
    next.bits = bits;
    next.in = in;
    next.out = out;
    if (left) {
        window.append( QByteArray( wnd + GZipIndex::WINSIZE - left, left ) );
    }
    if (left < quint32(GZipIndex::WINSIZE)) {
        window.append( QByteArray( wnd, GZipIndex::WINSIZE - left ) );
    }
    next.window = qCompress( window ).toBase64();
    index.points.append( next );
}

} // anonymous namespace

namespace U2 {

class GzipUtil {
public:
    GzipUtil(IOAdapter* io, bool doCompression);
    ~GzipUtil();
    qint64 uncompress(char* outBuff, qint64 outSize);
    qint64 compress(const char* inBuff, qint64 inSize, bool finish = false);
    bool isCompressing() const {return doCompression;}
    qint64 getPos() const;
    bool skip( const GZipIndexAccessPoint& index, qint64 offset );
private:
    static const int CHUNK = 16384;
    z_stream strm;
    char buf[CHUNK];
    IOAdapter* io;
    bool doCompression;
    qint64 curPos; // position of uncompressed file
};

GzipUtil::GzipUtil(IOAdapter* io, bool doCompression) : io(io), doCompression(doCompression), curPos( 0 )
{
//#ifdef _DEBUG
    memset(buf, 0xDD, CHUNK);
//#endif

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    int ret = doCompression ?
        /* write a simple gzip header and trailer around the compressed data */
        deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + 15, 8, Z_DEFAULT_STRATEGY)
        /* enable zlib and gzip decoding with automatic header detection */
        : inflateInit2(&strm, 32 + 15);
    assert(ret == Z_OK); Q_UNUSED(ret);
}

GzipUtil::~GzipUtil()
{
    if (doCompression) {
        int ret = compress(NULL, 0, true);
        if( -1 != ret ) {
            assert(ret == 0); Q_UNUSED(ret);
        }
        deflateEnd(&strm);
    } else {
        inflateEnd(&strm);
    }
}

qint64 GzipUtil::getPos() const {
    return curPos;
}

qint64 GzipUtil::uncompress(char* outBuff, qint64 outSize)
{
    /* Based on gun.c (example from zlib, copyrighted (C) 2003, 2005 Mark Adler) */
    strm.avail_out = outSize;
    strm.next_out = (Bytef*)outBuff;
    do {
        /* run inflate() on input until output buffer is full */
        if (strm.avail_in == 0) {
            // need more input
            strm.avail_in = io->readBlock(buf, CHUNK);
            strm.next_in = (Bytef*)buf;
        }
        if (strm.avail_in == quint32(-1)) {
            // TODO log error
            return -1;
        }
        if (strm.avail_in == 0)
            break;

        int ret = inflate(&strm, Z_SYNC_FLUSH);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                return -1;
            case Z_STREAM_END:
            {
                qint64 readBytes = 0;
                readBytes = outSize - strm.avail_out;
                inflateReset(&strm);
                inflateInit2(&strm, 32 + 15);

                return readBytes;
            }
            case Z_BUF_ERROR:
            case Z_FINISH:
                curPos += outSize - strm.avail_out;
                return outSize - strm.avail_out;
        }
        if (strm.avail_out != 0 && strm.avail_in != 0) {
            assert(0);
            break;
        }
    } while (strm.avail_out != 0);
    curPos += outSize - strm.avail_out;

    return outSize - strm.avail_out;
}

qint64 GzipUtil::compress(const char* inBuff, qint64 inSize, bool finish)
{
    int ret; Q_UNUSED(ret);
    /* Based on gun.c (example from zlib, copyrighted (C) 2003, 2005 Mark Adler) */
    strm.avail_in = inSize;
    strm.next_in = (Bytef*)inBuff;
    do {
        /* run deflate() on input until output buffer not full */
        strm.avail_out = CHUNK;
        strm.next_out = (Bytef*)buf;
        ret = deflate(&strm, finish ? Z_FINISH : Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        int have = CHUNK - strm.avail_out;
        qint64 l = io->writeBlock(buf, have);
        if (l != have) {
            // TODO log error
            return -1;
        }
    } while (strm.avail_out == 0);

    if (strm.avail_in != 0) {
        assert(0);     /* all input should be used */
        // TODO log error
        return -1;
    }

    assert(!finish || ret == Z_STREAM_END);        /* stream will be complete */

    return inSize;
}

// based on zran.c ( example from zlib Copyright (C) 2005 Mark Adler )
bool GzipUtil::skip( const GZipIndexAccessPoint& here, qint64 offset ) {
    if( here.out > offset || 0 > offset ) {
        return false;
    }
    int ret = 0;
    bool ok = false;
    char discard[GZipIndex::WINSIZE];

    LocalFileAdapter* localIO = qobject_cast< LocalFileAdapter* >( io );
    if( NULL == localIO ) {
        return false;
    }
    ok = localIO->skip( here.in - ( here.bits ? 1 : 0 ) );
    if ( !ok ) {
        return false;
    }
    inflateInit2( &strm, -15 );
    if ( here.bits ) {
        char chr = 0;
        ok = io->getChar( &chr );
        if( !ok ) {
            return false;
        }
        ret = chr;
        inflatePrime( &strm, here.bits, ret >> ( 8 - here.bits ) );
    }
    inflateSetDictionary( &strm, ( const Bytef* )here.window.data(), GZipIndex::WINSIZE );

    /* skip uncompressed bytes until offset reached, then satisfy request */
    offset -= here.out;
    do {
        /* define where to put uncompressed data, and how much */
        qint64 howMany = 0;
        if ( 0 == offset ) {          /* at offset now */
            break;                    /* all that we want */
        }
        if ( offset > GZipIndex::WINSIZE ) {             /* skip WINSIZE bytes */
            howMany = GZipIndex::WINSIZE;
        }
        else if ( 0 != offset ) {             /* last skip */
            howMany = offset;
        }
        offset -= howMany;
        qint64 uncompressed = uncompress( discard, howMany );
        if( uncompressed != howMany ) {
            return false; /* error or eof - cannot skip to desired position */
        }
    } while ( 1 );
    return true;
}

ZlibAdapter::ZlibAdapter(IOAdapter* io)
: IOAdapter(io->getFactory()), io(io), z(NULL), buf(NULL), rewinded(0) {}

ZlibAdapter::~ZlibAdapter() {
    close();
    delete io;
}

bool ZlibAdapter::isOpen() const {
    return io->isOpen();
}

void ZlibAdapter::close() {
    delete z;
    z = NULL;
    if (buf) {
        delete[] buf->rawData();
        delete buf;
        buf = NULL;
    }
    if (io->isOpen()) io->close();
}

bool ZlibAdapter::open(const GUrl& url, IOAdapterMode m ) {
    assert(!isOpen());
    close();
    bool res = io->open(url, m);
    if (res) {
        z = new GzipUtil(io, m == IOAdapterMode_Write);
        assert(z);
        if (m == IOAdapterMode_Read) {
            buf = new RingBuffer(new char[BUFLEN], BUFLEN);
            assert(buf);
        }
    }
    return res;
}

qint64 ZlibAdapter::readBlock(char* data, qint64 size)
{
    if (!isOpen() || z->isCompressing()) {
        assert(0 && "not ready to read");
        return false;
    }
    // first use data put back to buffer if any
    qint64 cached = 0;
    if (rewinded != 0) {
        assert(rewinded > 0 && rewinded <= buf->length());
        cached = buf->read(data, size, buf->length() - rewinded);
        if (formatMode == TextMode) {
            cutByteOrderMarks(data, cached);
        }
        if (cached == size) {
            rewinded -= size;
            return size;
        }
        assert(cached < size);
        rewinded = 0;
    }
    size = z->uncompress(data + cached, size - cached);
    if (formatMode == TextMode) {
        cutByteOrderMarks(data, size);
    }
    if (size == -1) {
        return -1;
    }
    buf->append(data + cached, size);

    return size + cached;
}

qint64 ZlibAdapter::writeBlock(const char* data, qint64 size) {
    if (!isOpen() || !z->isCompressing()) {
        assert(0 && "not ready to write");
        return false;
    }
    qint64 l = z->compress(data, size);
    return l;
}

bool ZlibAdapter::skip(qint64 nBytes) {
    if (!isOpen() || z->isCompressing()) {
        assert(0 && "not ready to seek");
        return false;
    }
    assert(buf);
    nBytes -= rewinded;
    if (nBytes <= 0) {
        if (-nBytes <= buf->length()) {
            rewinded = -nBytes;
            return true;
        }
        return false;
    }
    rewinded = 0;
    char* tmp = new char[nBytes];
    qint64 skipped = readBlock(tmp, nBytes);
    delete[] tmp;

    return skipped == nBytes;
}

bool ZlibAdapter::skip( const GZipIndexAccessPoint& point, qint64 offset ) {
    if( NULL == z ) {
        return false;
    }
    if( !point.window.size() || 0 > offset ) {
        return false;
    }
    return z->skip( point, offset );
}

qint64 ZlibAdapter::left() const {
    return -1;
}

int ZlibAdapter::getProgress() const {
    return io->getProgress();
}

qint64 ZlibAdapter::bytesRead() const {
    return z->getPos() - rewinded;
}

// based on zran.c ( example from zlib Copyright (C) 2005 Mark Adler )
GZipIndex ZlibAdapter::buildGzipIndex( IOAdapter* io, qint64 span, bool* ok ) {
    assert( NULL != io && io->isOpen() );

    int ret = 0;
    qint64 totin  = 0;        // our own total counters
    qint64 totout = 0;        // to avoid 4GB limit
    qint64 last = 0;          // totout value of last access point

    GZipIndex index;          // index being generated
    z_stream strm;

    char input[GZipIndex::CHUNK];
    char window[GZipIndex::WINSIZE];

    /* initialize inflate */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 47);      /* automatic zlib or gzip decoding */
    if (ret != Z_OK) {
        setIfYouCan( false, ok );
        return GZipIndex();
    }

    /* inflate the input, maintain a sliding window, and build an index -- this
    also validates the integrity of the compressed data using the check
    information at the end of the gzip or zlib stream */
    totin = totout = last = 0;
    strm.avail_out = 0;
    do {
        /* get some compressed data from input file */
        strm.avail_in = io->readBlock( input, GZipIndex::CHUNK );
        if ( -1 == int(strm.avail_in) || 0 == strm.avail_in ) {
            setIfYouCan( false, ok );
            return GZipIndex();
        }
        strm.next_in = ( Bytef* )&input[0];

        /* process all of that, or until end of stream */
        do {
            /* reset sliding window if necessary */
            if (strm.avail_out == 0) {
                strm.avail_out = GZipIndex::WINSIZE;
                strm.next_out = (Bytef*)window;
            }

            /* inflate until out of input, output, or at end of block --
            update the total input and output counters */
            totin  += strm.avail_in;
            totout += strm.avail_out;
            ret = inflate(&strm, Z_BLOCK);      /* return at end of block */
            totin  -= strm.avail_in;
            totout -= strm.avail_out;
            if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR || ret == Z_NEED_DICT ) {
                setIfYouCan( false, ok );
                return GZipIndex();
            }
            if (ret == Z_STREAM_END) {
                break;
            }
            /* if at end of block, consider adding an index entry (note that if
            data_type indicates an end-of-block, then all of the
            uncompressed data from that block has been delivered, and none
            of the compressed data after that block has been consumed,
            except for up to seven bits) -- the totout == 0 provides an
            entry point after the zlib or gzip header, and assures that the
            index always has at least one access point; we avoid creating an
            access point after the last block by checking bit 6 of data_type
            */
            if ((strm.data_type & 128) && !(strm.data_type & 64) && (totout == 0 || totout - last > span)) {
                addAccessPoint(index, strm.data_type & 7, totin, totout, strm.avail_out, window);
                last = totout;
            }
        } while (strm.avail_in != 0);
    } while (ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    setIfYouCan( true, ok );
    return index;
}

qint64 ZlibAdapter::getUncompressedFileSizeInBytes(const GUrl &url) {
    QFile file(url.getURLString());
    if (!file.open(QIODevice::ReadOnly)) {
        return -1;
    }

    int wordSizeInBytes = 4;
    file.seek(file.size() - wordSizeInBytes);
    QByteArray buffer = file.read(wordSizeInBytes);
    assert(buffer.size() == wordSizeInBytes);

    quint32 result = qFromLittleEndian<quint32>((uchar*)buffer.data());
    file.close();
    return result;
}

GUrl ZlibAdapter::getURL() const {
    return io->getURL();
}

QString ZlibAdapter::errorString() const{
    return io->errorString();
}

};//namespace
