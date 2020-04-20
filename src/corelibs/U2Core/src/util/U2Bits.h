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

#ifndef _U2_BITS_H_
#define _U2_BITS_H_

#include <QByteArray>
#include <QObject>
#include <QVector>

#include <U2Core/global.h>

namespace U2 {

class U2OpStatus;

/**
    Utility class that can compress any string with a small alphabet size ( < 20-30 symbols types)
*/
class U2CORE_EXPORT U2BitCompression : public QObject {
public:
    Q_OBJECT
    static QByteArray compress(const char* text, int uncompressedLen, int alphabetSize, const int* alphabetCharNums, U2OpStatus& os);

    static QByteArray uncompress(const char* data, const QByteArray& alphabetChars, U2OpStatus& os);

    static QVector<int> prepareCharNumsMask(const QByteArray& alphabetChars);

    static bool isCompressionNeeded(int textLen, int alphabetSize);

};

/** Bits manipulation helper */
class U2CORE_EXPORT U2Bits {
public:

    static int getBitInByte(int pos);

    static int getByteIndex(int bit);

    static int getNumberOfBytes(int nBits);

    static int getNumberOfBitsPerChar(int nChars);

    static QByteArray allocateBits(int nBits);

    static bool getBit(const uchar* bitSet, int idx);

    static void setBit(uchar* bits, int pos, bool val) { if (val) setBit(bits, pos); else clearBit(bits, pos);}

    static void setBit(uchar* bits, int pos);

    static void clearBit(uchar* bits, int pos);

    static void setBits(uchar* dstBits, int pos, const uchar* srcBits, int nBits);

    static int bitsRange2Int32(const uchar* bits, int pos, int len);

    static qint64 makeFirstNSignedBitsValue(int nBits);

    static qint8 readInt8(const uchar* bits, int pos);
    static void writeInt8(uchar* bits, int pos, qint8 val);

    static qint8 readInt16(const uchar* bits, int pos);
    static void writeInt16(uchar* bits, int pos, qint16 val);

    static qint8 readInt32(const uchar* bits, int pos);
    static void writeInt32(uchar* bits, int pos, qint32 val);
};

inline int U2Bits::getBitInByte(int pos) {
    return pos & 7; // use last 3 bits
}

inline int U2Bits::getByteIndex(int bit) {
    return (bit >> 3); // we store 8 bits (2^3) per word
}

inline int U2Bits::getNumberOfBytes(int nBits) {
    return nBits == 0 ? 0 : getByteIndex(nBits - 1) + 1;
}

inline bool U2Bits::getBit(const uchar* bits, int idx) {
    const uchar* b = bits + + getByteIndex(idx);
    int bitInByte = getBitInByte(idx);
    bool res = bool(*b & uchar(1 << bitInByte));
    return res;
}

inline void U2Bits::setBit(uchar* bits, int pos) {
    int idx = getByteIndex(pos);
    uchar* b = bits + idx;
    int bitInByte = getBitInByte(pos);
    *b = *b | uchar(1 << bitInByte);
}

inline void U2Bits::clearBit(uchar* bits, int pos) {
    int idx = getByteIndex(pos);
    uchar* b = bits + idx;
    int bitInByte = getBitInByte(pos);
    *b = *b & ~uchar(1 << bitInByte);
}

inline qint64 U2Bits::makeFirstNSignedBitsValue(int nBits) {
    static qint64 allSignedBitsValue = 0xFFFFFFFFFFFFFFFFLL;
    qint64 res = allSignedBitsValue;
    res = res << nBits;
    return ~res;
}

} //namespace

#endif
