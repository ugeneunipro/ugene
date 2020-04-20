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

#include "BAMDbiPlugin.h"
#include "IOException.h"
#include "InvalidFormatException.h"
#include "BgzfReader.h"

namespace U2 {
namespace BAM {

BgzfReader::BgzfReader(IOAdapter &ioAdapter):
    ioAdapter(ioAdapter),
    headerOffset(ioAdapter.bytesRead()),
    endOfFile(false)
{
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = Z_NULL;
    stream.avail_in = 0;
    stream.next_out = Z_NULL;
    stream.avail_out = 0;
    if(Z_OK != inflateInit2(&stream, 16 + 15)) {
        throw Exception(BAMDbiPlugin::tr("Can't initialize zlib"));
    }
}

BgzfReader::~BgzfReader() {
    inflateEnd(&stream);
}

qint64 BgzfReader::read(char *buff, qint64 maxSize) {
    if(0 == maxSize) {
        return 0;
    }
    stream.next_out = (Bytef *)buff;
    stream.avail_out = maxSize;
    while(stream.avail_out > 0) {
        if(0 == stream.avail_in) {
            qint64 returnedValue = ioAdapter.readBlock(buffer, sizeof(buffer));
            if(-1 == returnedValue) {
                coreLog.error(QString("in BgzfReader::read, failed to read %1 bytes from ioAdapter, after %2 bytes already read. %3")
                              .arg(sizeof(buffer))
                              .arg(ioAdapter.bytesRead())
                              .arg(ioAdapter.errorString()));
                throw IOException(BAMDbiPlugin::tr("Can't read input"));
            } else if(0 == returnedValue) {
                endOfFile = true;
                break;
            } else {
                stream.avail_in = returnedValue;
                stream.next_in = (Bytef *)buffer;
            }
        }
        int returnedValue = inflate(&stream, Z_SYNC_FLUSH);
        if(Z_STREAM_END == returnedValue) {
            nextBlock();
        } else if(Z_OK != returnedValue) {
            coreLog.error(QString("in BgzfReader::read, failed to decompress %1 bytes, after %2 raw bytes already read")
                          .arg(sizeof(buffer))
                          .arg(ioAdapter.bytesRead()));
            throw InvalidFormatException(BAMDbiPlugin::tr("Can't decompress data"));
        }
    }
    if(0 == stream.avail_in) {
        nextBlock();
    }
    qint64 bytesRead = maxSize - stream.avail_out;
    return bytesRead;
}

qint64 BgzfReader::skip(qint64 size) {
    char skipBuffer[1024];
    qint64 bytesSkipped = 0;
    while(bytesSkipped < size) {
        qint64 toRead = qMin((qint64)sizeof(skipBuffer), size - bytesSkipped);
        qint64 returnedValue = read(skipBuffer, toRead);
        bytesSkipped += returnedValue;
        if(returnedValue < toRead) {
            break;
        }
    }
    return bytesSkipped;
}

bool BgzfReader::isEof()const {
    return endOfFile;
}

VirtualOffset BgzfReader::getOffset()const {
    return VirtualOffset(headerOffset, stream.total_out);
}

void BgzfReader::seek(VirtualOffset offset) {
    if((offset.getCoffset() == headerOffset) && (offset.getUoffset() >= (int)stream.total_out)) {
        qint64 toSkip = offset.getUoffset() - stream.total_out;
        if(skip(toSkip) < toSkip) {
            coreLog.error(QString("in BgzfReader::seek, cannot seek to offset {coffset=%1,uoffset=%2}, failed to skip %3")
                          .arg(offset.getCoffset())
                          .arg(offset.getUoffset())
                          .arg(toSkip));
            throw InvalidFormatException(BAMDbiPlugin::tr("Unexpected end of file"));
        }
    } else {
        qint64 toSkipIo = offset.getCoffset() - ioAdapter.bytesRead();
        if(!ioAdapter.skip(toSkipIo)) {
            coreLog.error(QString("in BgzfReader::seek, cannot seek to offset {coffset=%1,uoffset=%2}, ioAdapter failed to skip %3")
                          .arg(offset.getCoffset())
                          .arg(offset.getUoffset())
                          .arg(toSkipIo));
            throw IOException(BAMDbiPlugin::tr("Can't read input"));
        }
        stream.next_in = Z_NULL;
        stream.avail_in = 0;
        headerOffset = ioAdapter.bytesRead();
        inflateReset(&stream);
        qint64 toSkip = offset.getUoffset();
        if(skip(toSkip) < toSkip) {
            coreLog.error(QString("in BgzfReader::seek, cannot seek to offset {coffset=%1,uoffset=%2}, failed to skip %3 after ioAdapter skipped %4")
                          .arg(offset.getCoffset())
                          .arg(offset.getUoffset())
                          .arg(toSkip)
                          .arg(toSkipIo));
            throw InvalidFormatException(BAMDbiPlugin::tr("Unexpected end of file"));
        }
    }
    endOfFile = false;
}

void BgzfReader::nextBlock() {
    uInt oldAvailOut = stream.avail_out;
    stream.avail_out = 0;
    while(true) {
        if(0 == stream.avail_in) {
            qint64 returnedValue = ioAdapter.readBlock(buffer, sizeof(buffer));
            if(-1 == returnedValue) {
                coreLog.error(QString("in BgzfReader::nextBlock, failed to read %1 bytes from ioAdapter, after %2 bytes already read. %3")
                              .arg(sizeof(buffer))
                              .arg(ioAdapter.bytesRead())
                              .arg(ioAdapter.errorString()));
                throw IOException(BAMDbiPlugin::tr("Can't read input"));
            } else if(0 == returnedValue) {
                endOfFile = true;
                break;
            } else {
                stream.avail_in = returnedValue;
                stream.next_in = (Bytef *)buffer;
            }
        }
        int returnedValue = inflate(&stream, Z_SYNC_FLUSH);
        if(Z_STREAM_END == returnedValue) {
            headerOffset = ioAdapter.bytesRead() - stream.avail_in;
            inflateReset(&stream);
        } else if(Z_BUF_ERROR == returnedValue) {
            break;
        } else if(Z_OK != returnedValue) {
            coreLog.error(QString("in BgzfReader::nextBlock, failed to decompress %1 bytes, after %2 raw bytes already read")
                          .arg(sizeof(buffer))
                          .arg(ioAdapter.bytesRead()));
            throw InvalidFormatException(BAMDbiPlugin::tr("Can't decompress data"));
        }
    }
    stream.avail_out = oldAvailOut;
}

} // namespace BAM
} // namespace U2
