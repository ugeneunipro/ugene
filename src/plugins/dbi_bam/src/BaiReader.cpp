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
#include "InvalidFormatException.h"
#include "IOException.h"
#include "BaiReader.h"

namespace U2 {
namespace BAM {

BaiReader::BaiReader(IOAdapter &ioAdapter):
    ioAdapter(ioAdapter)
{
}

// Undocumented extra bin with reference and reads info stored in chunks, mentioned here:
// https://github.com/keithj/cl-sam/blob/master/src/bam-index-reader.lisp#L27
static const unsigned int SAMTOOLS_KLUDGE_BIN = 37450;

Index BaiReader::readIndex() {
    {
        QByteArray magic = readBytes(4);
        if("BAI\001" != magic) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid magic number"));
        }
    }
    int referencesNumber = readInt32();
    if(referencesNumber < 0) {
        throw InvalidFormatException(BAMDbiPlugin::tr("Invalid number of references: %1").arg(referencesNumber));
    }
    QList<Index::ReferenceIndex> referenceIndices;
    for(int referenceId = 0;referenceId < referencesNumber;referenceId++) {
        int binsNumber = readInt32();
        if(binsNumber < 0) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid number of bins: %1").arg(binsNumber));
        }
        QList<Index::ReferenceIndex::Bin> bins;
        for(int binId = 0;binId < binsNumber;binId++) {
            unsigned int bin = readUint32();
            int chunksNumber = readInt32();
            if(chunksNumber < 0) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Invalid number of chunks: %1").arg(chunksNumber));
            }
            QList<Index::ReferenceIndex::Chunk> chunks;
            for(int chunkId = 0;chunkId < chunksNumber;chunkId++) {
                VirtualOffset chunkBegin(readUint64());
                VirtualOffset chunkEnd(readUint64());
                // Some index files fail this check.
//                if(chunkEnd < chunkBegin) {
//                    throw InvalidFormatException(BAMDbiPlugin::tr("Invalid chunk"));
//                }
                chunks.append(Index::ReferenceIndex::Chunk(chunkBegin, chunkEnd));
            }
            if(bin != SAMTOOLS_KLUDGE_BIN) {
                bins.append(Index::ReferenceIndex::Bin(bin, chunks));
            }
        }
        int intervalsNumber = readInt32();
        if(intervalsNumber < 0) {
            throw InvalidFormatException(BAMDbiPlugin::tr("Invalid number of intervals: %1").arg(intervalsNumber));
        }
        QList<VirtualOffset> intervals;
        for(int intervalId = 0;intervalId < intervalsNumber;intervalId++) {
            VirtualOffset intervalOffset(readUint64());
            intervals.append(intervalOffset);
        }
        referenceIndices.append(Index::ReferenceIndex(bins, intervals));
    }
    return Index(QList<Index::ReferenceIndex>(referenceIndices));
}

void BaiReader::readBytes(char *buff, qint64 size) {
    qint64 returnedValue = ioAdapter.readBlock(buff, size);
    if(-1 == returnedValue) {
        throw IOException(BAMDbiPlugin::tr("Can't read input. %1").arg(ioAdapter.errorString()));
    } else if(returnedValue < size) {
        throw InvalidFormatException(BAMDbiPlugin::tr("Unexpected end of file"));
    }
}

QByteArray BaiReader::readBytes(qint64 size) {
    QByteArray result(size, 0);
    readBytes(result.data(), result.size());
    return result;
}

quint64 BaiReader::readUint64() {
    char buffer[8];
    readBytes(buffer, sizeof(buffer));
    return (quint64)(buffer[0] & 0xff) |
            ((quint64)(buffer[1] & 0xff) << 8) |
            ((quint64)(buffer[2] & 0xff) << 16) |
            ((quint64)(buffer[3] & 0xff) << 24) |
            ((quint64)(buffer[4] & 0xff) << 32) |
            ((quint64)(buffer[5] & 0xff) << 40) |
            ((quint64)(buffer[6] & 0xff) << 48) |
            ((quint64)(buffer[7] & 0xff) << 56);
}

qint32 BaiReader::readInt32() {
    char buffer[4];
    readBytes(buffer, sizeof(buffer));
    return (buffer[0] & 0xff) |
            ((buffer[1] & 0xff) << 8) |
            ((buffer[2] & 0xff) << 16) |
            ((buffer[3] & 0xff) << 24);
}

quint64 BaiReader::readUint32() {
    char buffer[4];
    readBytes(buffer, sizeof(buffer));
    return (buffer[0] & 0xff) |
            ((buffer[1] & 0xff) << 8) |
            ((buffer[2] & 0xff) << 16) |
            ((buffer[3] & 0xff) << 24);
}

} // namespace BAM
} // namespace U2
