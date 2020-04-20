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

#include "U2Bits.h"
#include <U2Core/U2OpStatus.h>

namespace U2 {

//static int getLenBits(int len) {
//    return len == 0 ? 3 : len < 0XFF ? 0 : len < 0xFFFF ? 1 : 2;
//}
static int getLenBitsSize(int len) {
    return len == 0 ? 0 : len < 0XFF ? 8 : len < 0xFFFF ? 16 : 32;
}

static void writeLength(uchar* bits, int len, int lenBitsLen) {
    if (lenBitsLen == 8) { // 00
        U2Bits::writeInt8(bits, 2, (qint8)len);
    } else if (lenBitsLen == 16) { // 01
        U2Bits::setBit(bits, 0);
        U2Bits::writeInt16(bits, 2, (qint16)len);
    } else if (lenBitsLen == 32) { // 10
        U2Bits::setBit(bits, 1);
        U2Bits::writeInt16(bits, 2, (qint32)len);
    } else {
        U2Bits::setBit(bits, 0); // empty length => both bits set: 11
        U2Bits::setBit(bits, 1);
    }
}

static int readLength(const uchar* bits, int& nBits) {
    bool b0 = U2Bits::getBit(bits, 0);
    bool b1 = U2Bits::getBit(bits, 1);
    if (b0 == b1)  {
        if (!b0) {
            nBits = 8;
            return U2Bits::readInt8(bits, 2);
        }
        nBits = 0;
        return 0;
    } else if (b0) {
        nBits = 16;
        return U2Bits::readInt16(bits, 2);
    } else {
        nBits = 32;
        return U2Bits::readInt32(bits, 2);
    }
}

QByteArray U2BitCompression::compress(const char* text, int len, int alphabetSize, const int* alphabetCharNums, U2OpStatus& os) {
    // algorithm:
    // 1. compute chars freq -> derive number of bits per char
    // 2. assign bit masks per char. Do not assign any bit masks for non used alphabet chars
    // 3. compress chars
    // 4. create header with used char mask
    // Result bits [len type][len][used alpha bits][compressed text]
    //  where [len type] is a type of length field: 00 -> empty, 01 -> 8 byte, 10 -> 16 bytes, 11 -> 32 bytes
    //  [len] - length of the result sequence
    //  [used alpha bits] bit is set if alpha char is used in the text.
    //  [compressed text] the data in compressed form

    assert(alphabetSize <= 32); //avoid this check in runtime -> use this method correctly

    // find all used chars in text
    QVector<bool> visitVector(alphabetSize, false);
    bool* visited = visitVector.data();
    for (int i = 0; i < len; i++) {
        uchar c = text[i];
        int n = alphabetCharNums[c];
        if (n == -1) {
            os.setError(tr("Bit compression: illegal character in text '%1'").arg(char(c)));
            return QByteArray();
        }
        if (!visited[n]) {
            visited[n] = true;
        }
    }

    // assign sequential bit-mask for all used chars
    QVector<uchar> maskVector(alphabetSize, 0);
    uchar* mask = maskVector.data();
    uchar m = 0;
    for (int i = 0; i < alphabetSize; i++) {
        if (visited[i]) {
            mask[i] = m;
            m++;
        }
    }
    // store header and data to bit set
    int bitsPerChar = U2Bits::getNumberOfBitsPerChar(m);
    int compressedBitSize = len * bitsPerChar;
    int lenBits = getLenBitsSize(len);
    int headerSizeBits = 2 + lenBits + alphabetSize;
    int resultSizeBits = headerSizeBits + compressedBitSize;
    static QByteArray res;
    QByteArray bitSet = U2Bits::allocateBits(resultSizeBits);
    uchar* bits = (uchar*)bitSet.data();
    writeLength(bits, len, lenBits);
    int pos = 2 + lenBits;
    for (; pos < alphabetSize; pos++) {
        if (visited[pos]) {
            U2Bits::setBit(bits, pos);
        }
    }
    for (int i = 0; i < len; i++, pos+=bitsPerChar) {
        uchar c = text[i];
        int n = alphabetCharNums[c];
        uchar m = mask[n];
        U2Bits::setBits(bits, pos, &m, bitsPerChar);
    }
    return bitSet;
}

QByteArray U2BitCompression::uncompress(const char* data, const QByteArray& alphabetChars, U2OpStatus&) {
    // algorithm
    // 1. Derive all chars from header
    // 2. Assign bit masks per chars that have signed bit in header
    // 3. Unpack value

    int alphabetSize = alphabetChars.size();
    const char* aChars = alphabetChars.data();
    const uchar* bits = (const uchar*)data;

    int alphaMaskOffset = 0;
    int len = readLength(bits, alphaMaskOffset);


    // restore bit masks
    QVector<bool> visitVector(alphabetSize, false);
    bool* visited = visitVector.data();
    int nChars = 0;
    for (int i = 0; i < alphabetSize; i++) {
        if (U2Bits::getBit(bits, i + alphaMaskOffset)) {
            visited[i] = true;
            nChars++;
        }
    }
    int bitsPerChar = U2Bits::getNumberOfBitsPerChar(nChars);

    QVector<char> mask2Char(nChars, 0);
    uchar m = 0;
    for (int i = 0; i < alphabetSize; i++) {
        if (visited[i]) {
            mask2Char[m] = aChars[i];
            m++;
        }
    }
    int pos = alphaMaskOffset + alphabetSize;
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    QByteArray result(len, Qt::Uninitialized);
#else
    QByteArray result(len, (char)0);
#endif
    char* res = result.data();
    for (int i = 0; i < len; i++, pos += bitsPerChar) {
        int m = U2Bits::bitsRange2Int32(bits, pos, bitsPerChar);
        char c = mask2Char[m];
        assert(c != 0);
        res[i] = c;
    }
    return result;
}

QVector<int> U2BitCompression::prepareCharNumsMask(const QByteArray& alphabetChars) {
    QVector<int> res(256, -1);
    for (int i = 0, n = alphabetChars.size(); i < n; i++) {
        uchar a = (uchar)alphabetChars[i];
        res[a] = i;
    }
    return res;
}

#define K_FACTOR 1.5
/** Compression is eligible if compressed text < original text length with a compression factor of K */
//static bool isCompressionNeeded(int textLen, int alphabetSize) {
//    int nBits = alphabetSize + U2Bits::getNumberOfBitsPerChar(alphabetSize) * textLen;
//    int compressedBytes = U2Bits::getNumberOfBytes(nBits);
//    return compressedBytes * K_FACTOR < textLen;
//}

//////////////////////////////////////////////////////////////////////////
// bits helper


int U2Bits::getNumberOfBitsPerChar(int nChars)  {
    int bitsPerChar = nChars <= 2 ? 1 : (nChars <= 4) ? 2 : (nChars <= 8) ? 3 : (nChars <= 16) ? 4 : 5;
    return bitsPerChar;
}

QByteArray U2Bits::allocateBits(int nBits) {
    int nBytes = getNumberOfBytes(nBits);
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        return QByteArray(nBytes, Qt::Uninitialized);
#else
        return QByteArray(nBytes, char(0));
#endif
}


void U2Bits::setBits(uchar* dstBits, int pos, const uchar* srcBits, int nBits) {
    // TODO: optimize
    for (int i = 0; i < nBits; i++) {
        bool val = getBit(srcBits, i);
        setBit(dstBits, i + pos, val);
    }
}
int U2Bits::bitsRange2Int32(const uchar* bits, int pos, int len) {
    //TODO: optimize
    assert(len <= 32);
    int res = 0;
    for (int i = 0; i < len; i++) {
        bool b = getBit(bits, pos + i);
        if (b) {
            res = res | (1 << i);
        }
    }
    return res;
}

qint8 U2Bits::readInt8(const uchar* bits, int pos) {
    int res = 0;
    for (int i = 0; i < 8; i++) {
        res = res << 1;
        if (U2Bits::getBit(bits,  pos + i)) {
            res += 1;
        }
    }
    return qint8(res);
}

void U2Bits::writeInt8(uchar* bits, int pos, qint8 val) {
    const uchar* data = (const uchar*)&val;
    for (int i = 0; i < 8; i++) {
        if (U2Bits::getBit(data, i)) {
            U2Bits::setBit(bits, pos + i);
        } else {
            U2Bits::clearBit(bits, pos + i);
        }
    }
}

qint8 U2Bits::readInt16(const uchar* bits, int pos) {
    int res = (readInt8(bits, pos) << 8) + readInt8(bits, pos + 8);
    return qint16(res);
}

void U2Bits::writeInt16(uchar* bits, int pos, qint16 val) {
    writeInt8(bits, pos + 8, qint8(val));
    writeInt8(bits, pos , qint8(val >> 8));
}

qint8 U2Bits::readInt32(const uchar* bits, int pos) {
    int res = (readInt8(bits, pos) << 24) + (readInt8(bits, pos + 8) << 16) + (readInt8(bits, pos + 16) << 8) + readInt8(bits, pos + 24);
    return qint16(res);
}

void U2Bits::writeInt32(uchar* bits, int pos, qint32 val) {
    writeInt8(bits, pos + 24, qint8(val));
    writeInt8(bits, pos + 16, qint8(val >> 8));
    writeInt8(bits, pos + 8, qint8(val >> 16));
    writeInt8(bits, pos, qint8(val >> 24));
}

} //namespace
