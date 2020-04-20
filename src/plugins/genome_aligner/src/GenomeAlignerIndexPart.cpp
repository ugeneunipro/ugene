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

#include <U2Core/Timer.h>
#include <U2Algorithm/BitsTable.h>
#include <QtEndian>
#include <U2Algorithm/BitsTable.h>

#include "GenomeAlignerIndexPart.h"
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

IndexPart::IndexPart() {
    sArray = NULL;
    bitMask = NULL;
    seq = NULL;
    partCount = 0;
    currentPart = -1;
    seqStarts = NULL;
    seqLengths = NULL;
    saLengths = NULL;

    refFile = NULL;
    partFiles = NULL;
}

IndexPart::~IndexPart() {
    delete[] sArray;
    delete[] bitMask;
    delete[] seq;
    delete[] seqStarts;
    delete[] seqLengths;
    delete[] saLengths;

    if (NULL != refFile) {
        refFile->close();
    }
    delete refFile;

    for (int i=0; i<partCount; i++) {
        if (NULL != partFiles[i]) {
            partFiles[i]->close();
        }
        delete partFiles[i];
    }
    delete[] partFiles;
}

SAType IndexPart::getLoadedPartSize() const {
    assert(currentPart>=0);
    return saLengths[currentPart];
}

bool isLittleEndian() {
    char little[] = {1, 0, 0, 0};
    int *n = (int*)little;

    if (1 == *n) {
        return true;
    }
    return false;
}

bool IndexPart::load(int part) {

    qint64 t0 = GTimer::currentTimeMicros();

    assert(part < partCount);
    if (part == currentPart) {
        return true;
    }
    currentPart = part;
    qint64 size = 0;
    if (!partFiles[part]->isOpen()) {
        partFiles[part]->open(QIODevice::ReadOnly);
    }
    partFiles[part]->seek(0);

    char *buff = NULL;

    size_t needRead = 0;

    needRead = 1*sizeof(SAType);
    buff = (char*)&(saLengths[currentPart]);
    size = partFiles[part]->read(buff, needRead);
    SAFE_POINT(static_cast<quint64>(size) == needRead, "Index format error", false);

    needRead = saLengths[currentPart]*sizeof(SAType);
    buff = (char*)sArray;
    size = partFiles[part]->read(buff, needRead);
    SAFE_POINT(static_cast<quint64>(size) == needRead, "Index format error", false);

    needRead = saLengths[currentPart]*sizeof(BMType);
    buff = (char*)bitMask;
    size = partFiles[part]->read(buff, needRead);
    SAFE_POINT(static_cast<quint64>(size) == needRead, "Index format error", false);

    uchar *bitSeq = new uchar[1 + seqLengths[currentPart]/4];
    size = partFiles[part]->read((char*)bitSeq, 1 + seqLengths[currentPart]/4);
    assert(size == 1 + seqLengths[currentPart]/4);
    if (size != 1 + seqLengths[currentPart]/4) {
        delete[] bitSeq;
        return false;
    }

    refFile->seek(seqStarts[currentPart]);
    size = refFile->read(seq, seqLengths[currentPart]);
    assert(size == seqLengths[currentPart]);
    if (size != seqLengths[currentPart]) {
        delete[] bitSeq;
        return false;
    }

    for (quint32 i=0; i<saLengths[currentPart]; i++) {
        if (!isLittleEndian()) {
            sArray[i] = qFromLittleEndian<quint32>((uchar*)(sArray + i));
        }
    }

    qint64 t1 = GTimer::currentTimeMicros();
    algoLog.trace(QString("IndexPart::load time %1 ms").arg((t1 - t0) / double(1000), 0, 'f', 3));

    delete[] bitSeq;
    return true;
}

SAType IndexPart::getMaxLength() {
    SAType maxLength = 0;
    for (int i=0; i<partCount; i++) {
        if (maxLength < seqLengths[i]) {
            maxLength = seqLengths[i];
        }
    }

    return maxLength;
}

SAType IndexPart::getLoadedSeqStart() {
    assert(currentPart>=0);
    return seqStarts[currentPart];
}

void IndexPart::writePart(int part, quint32 arrLen) {

    qint64 t0 = GTimer::currentTimeMicros();

    partFiles[part]->open(QIODevice::ReadWrite);
    currentPart = part;

    if (!isLittleEndian()) {
        for (quint32 i=0; i<arrLen; i++) {
            qToLittleEndian(sArray[i], (uchar*)(sArray + i));
            qToLittleEndian(bitMask[i], (uchar*)(bitMask + i));
        }
    }

    partFiles[part]->write((char*)&arrLen, 1 * sizeof(SAType));

    partFiles[part]->write((char*)sArray, arrLen * sizeof(SAType));
    partFiles[part]->write((char*)bitMask, arrLen * sizeof(BMType));

    qint64 t1 = GTimer::currentTimeMicros();
    uchar *values = new uchar[1 + seqLengths[currentPart]/4];
    int i = 0;
    int bitNum = 0;
    BitsTable bt;
    const quint32 *bitTable = bt.getBitMaskCharBits(DNAAlphabet_NUCL);

    for (quint32 j=0; j<seqLengths[currentPart]; j++) {
        if (0 == bitNum) {
            values[i] = 0;
        }
        values[i] = (values[i]<<2);
        values[i] |= bitTable[uchar(*(seq+j))];
        bitNum += 2;
        if (bitNum >= 8) {
            bitNum = 0;
            i++;
        }
    }

    if (bitNum > 0) {
        values[i] <<= 8-bitNum;
    }
    algoLog.trace(QString("IndexPart::writePart some bits table time %1 ms").arg((GTimer::currentTimeMicros() - t1) / double(1000), 0, 'f', 3));

    partFiles[part]->write((char*)values, 1 + seqLengths[currentPart]/4);

    delete[] values;

    qint64 t2 = GTimer::currentTimeMicros();
    algoLog.trace(QString("IndexPart::writePart time %1 ms").arg((t2 - t0) / double(1000), 0, 'f', 3));
}

BMType IndexPart::getBitValue(uchar *seq, SAType idx) {
    int charNum = idx/4;
    BMType res = qFromBigEndian<BMType>(seq+charNum);
    if (0 == (idx - charNum*4)) {
        res >>= 2;
        return res;
    }
    res <<= 2*(idx - charNum*4);
    uchar qwe = *(seq+charNum+8);
    qwe >>= 2*((charNum+1)*4 - idx);
    res |= (BMType)qwe;

    res >>= 2;
    return res;
}

} //U2
