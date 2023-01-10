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

#include "BgzfWriter.h"

#include "BAMDbiPlugin.h"
#include "IOException.h"

namespace U2 {
namespace BAM {

BgzfWriter::BgzfWriter(IOAdapter& ioAdapter)
    : ioAdapter(ioAdapter),
      headerOffset(ioAdapter.bytesRead()),
      blockEnd(false),
      finished(false) {
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = Z_NULL;
    stream.avail_in = 0;
    stream.next_out = Z_NULL;
    stream.avail_out = 0;
    if (Z_OK != deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + 15, 8, Z_DEFAULT_STRATEGY)) {
        throw Exception(BAMDbiPlugin::tr("Can't initialize zlib"));
    }
}

BgzfWriter::~BgzfWriter() {
    assert(finished);
    deflateEnd(&stream);
}

void BgzfWriter::write(const char* buff, qint64 size) {
    if (size == 0) {
        return;
    }
    assert(!finished);
    qint64 bytesWritten = 0;
    while (bytesWritten < size) {
        if (blockEnd) {
            deflateReset(&stream);
            blockEnd = false;
        }
        qint64 bytesToWrite1 = qMin(size - bytesWritten, BLOCK_SIZE - (qint64)stream.total_in);
        stream.next_in = (Bytef*)&buff[bytesWritten];
        stream.avail_in = (uInt)bytesToWrite1;
        while (stream.avail_in > 0) {
            stream.next_out = (Bytef*)buffer;
            stream.avail_out = sizeof(buffer);
            if (deflate(&stream, Z_NO_FLUSH) != Z_OK) {
                throw Exception(BAMDbiPlugin::tr("Can't compress data"));
            } else {
                qint64 bytesToWrite2 = sizeof(buffer) - stream.avail_out;
                if (ioAdapter.writeBlock(buffer, bytesToWrite2) != bytesToWrite2) {
                    throw IOException(BAMDbiPlugin::tr("Can't write output"));
                }
            }
        }
        if ((int)stream.total_in == BLOCK_SIZE) {
            finishBlock();
            headerOffset = ioAdapter.bytesRead();
        }
        bytesWritten += bytesToWrite1;
    }
}

void BgzfWriter::finish() {
    assert(!finished);
    finishBlock();
    finished = true;
}

VirtualOffset BgzfWriter::getOffset() const {
    return VirtualOffset(headerOffset, blockEnd ? 0 : stream.total_out);
}

void BgzfWriter::finishBlock() {
    stream.next_in = Z_NULL;
    stream.avail_in = 0;
    while (true) {
        stream.next_out = (Bytef*)buffer;
        stream.avail_out = sizeof(buffer);
        int returnedValue = deflate(&stream, Z_FINISH);
        if ((Z_OK == returnedValue) || (Z_STREAM_END == returnedValue)) {
            qint64 toWrite = sizeof(buffer) - stream.avail_out;
            if (ioAdapter.writeBlock(buffer, toWrite) != toWrite) {
                throw IOException(BAMDbiPlugin::tr("Can't write output"));
            }
            if (Z_STREAM_END == returnedValue) {
                break;
            }
        } else {
            throw Exception(BAMDbiPlugin::tr("Can't compress data"));
        }
    }
    blockEnd = true;
    headerOffset = ioAdapter.bytesRead();
}

}  // namespace BAM
}  // namespace U2
