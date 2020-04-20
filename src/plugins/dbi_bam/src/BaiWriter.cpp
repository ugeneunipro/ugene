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
#include "BaiWriter.h"

namespace U2 {
namespace BAM {

BaiWriter::BaiWriter(IOAdapter &ioAdapter):
    ioAdapter(ioAdapter)
{
}

void BaiWriter::writeIndex(const Index &index) {
    writeBytes(QByteArray("BAI\001"));
    writeInt32(index.getReferenceIndices().size());
    foreach(const Index::ReferenceIndex &referenceIndex, index.getReferenceIndices()) {
        writeInt32(referenceIndex.getBins().size());
        for(int i = 0;i< referenceIndex.getBins().size();i++) {
            const Index::ReferenceIndex::Bin &bin = referenceIndex.getBins()[i];
            if(!bin.getChunks().isEmpty()) {
                writeUint32(bin.getBin());
                writeInt32(bin.getChunks().size());
                foreach(const Index::ReferenceIndex::Chunk &chunk, bin.getChunks()) {
                    writeUint64(chunk.getStart().getPackedOffset());
                    writeUint64(chunk.getEnd().getPackedOffset());
                }
            }
        }
        writeInt32(referenceIndex.getIntervals().size());
        foreach(const VirtualOffset &offset, referenceIndex.getIntervals()) {
            writeUint64(offset.getPackedOffset());
        }
    }
}

void BaiWriter::writeBytes(const char *buff, qint64 size) {
    if(ioAdapter.writeBlock(buff, size) != size) {
        throw IOException(BAMDbiPlugin::tr("Can't write output"));
    }
}

void BaiWriter::writeBytes(const QByteArray &buffer) {
    writeBytes(buffer.constData(), buffer.size());
}

void BaiWriter::writeUint64(quint64 value) {
    char buffer[8];
    buffer[0] = (char)(value >> 0);
    buffer[1] = (char)(value >> 8);
    buffer[2] = (char)(value >> 16);
    buffer[3] = (char)(value >> 24);
    buffer[4] = (char)(value >> 32);
    buffer[5] = (char)(value >> 40);
    buffer[6] = (char)(value >> 48);
    buffer[7] = (char)(value >> 56);
    writeBytes(buffer, sizeof(buffer));
}

void BaiWriter::writeInt32(qint32 value) {
    char buffer[4];
    buffer[0] = (char)(value >> 0);
    buffer[1] = (char)(value >> 8);
    buffer[2] = (char)(value >> 16);
    buffer[3] = (char)(value >> 24);
    writeBytes(buffer, sizeof(buffer));
}

void BaiWriter::writeUint32(quint32 value) {
    char buffer[4];
    buffer[0] = (char)(value >> 0);
    buffer[1] = (char)(value >> 8);
    buffer[2] = (char)(value >> 16);
    buffer[3] = (char)(value >> 24);
    writeBytes(buffer, sizeof(buffer));
}

} // namespace BAM
} // namespace U2
