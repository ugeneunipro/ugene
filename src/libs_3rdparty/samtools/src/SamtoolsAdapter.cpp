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

#include <qendian.h>
#include <QScopedPointer>

#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatus.h>

#include "SamtoolsAdapter.h"

namespace U2 {

static quint8 cigarOp2samtools(U2CigarOp op, U2OpStatus &os) {
    switch(op) {
    case U2CigarOp_M: // alignment match
        return 0;
    case U2CigarOp_I: // inserted
        return 1;
    case U2CigarOp_D: // deleted
        return 2;
    case U2CigarOp_N: // skipped
        return 3;
    case U2CigarOp_S:  // soft-clipped
        return 4;
    case U2CigarOp_H: // hard-clipped
        return 5;
    case U2CigarOp_P: // padded
        return 6;
    case U2CigarOp_EQ:// sequence match
        return 7;
    case U2CigarOp_X: // sequence mismatch
        return 8;
    default:
        os.setError(SamtoolsAdapter::tr("Invalid cigar operation %1, cannot convert to samtools").arg(op));
        return 0;
    };
}

/**
    According to bam.h comments to bam1_cigar macro,
    In the CIGAR array, each element is a 32-bit integer. The
    lower 4 bits gives a CIGAR operation and the higher 28 bits keep the
    length of a CIGAR.
*/
QByteArray SamtoolsAdapter::cigar2samtools(QList<U2CigarToken> cigar, U2OpStatus &os) {
    QByteArray samtoolsCigar;
    foreach(const U2CigarToken &token, cigar) {
        qint8 op = cigarOp2samtools(token.op, os);
        qint32 value = (token.count << 4) | (op & 0xf);

        CHECK_OP(os, samtoolsCigar);
        samtoolsCigar.append((char*)(&value), sizeof(value));
    }
    return samtoolsCigar;
}

/**
    According to bam.h comments to bam1_seq macro,
    Each base is encoded in 4 bits: 1 for A, 2 for C, 4 for G,
    8 for T and 15 for N. Two bases are packed in one byte with the base
    at the higher 4 bits having smaller coordinate on the read.
*/
QByteArray SamtoolsAdapter::sequence2samtools(QByteArray sequence, U2OpStatus &os) {
    int packedLength = (sequence.length() + 1)/2;
    QByteArray samtoolsSequence(packedLength, 0);
    for(int i = 0; i < packedLength; ++i) {
        qint8 value = bam_nt16_table[sequence[2*i]] << 4;
        if(2*i + 1 < sequence.length()) {
            value |= bam_nt16_table[sequence[2*i+1]] & 0xf;
        }
        CHECK_OP(os, samtoolsSequence);
        samtoolsSequence[i] = value;
    }
    return samtoolsSequence;
}

QByteArray SamtoolsAdapter::aux2string(const QList<U2AuxData> &auxData) {
    QByteArray result;
    foreach (const U2AuxData &aux, auxData) {
        result.append(aux.tag, 2);
        result.append(aux.type);
        if ('B' == aux.type) {
            int typeSize = 1;
            if ('c' == aux.subType || 'C' == aux.subType) { typeSize = 1; }
            else if ('s' == aux.subType || 'S' == aux.subType) { typeSize = 2; }
            else if ('i' == aux.subType || 'I' == aux.subType) { typeSize = 4; }
            else if ('f' == aux.subType) { typeSize = 4; }
            int n = aux.value.length() / typeSize;
            n = qToLittleEndian<int>(n);
            result.append(aux.subType);
            result.append((char*)&n, 4);
        }
        result.append(aux.value);
        if ('Z' == aux.type || 'H' == aux.type) {
            result.append(char(0));
        }
    }
    return result;
}

template<class T>
inline static void addNum(T num, int tSize, QByteArray &result) {
    T leNum = qToLittleEndian<T>(num);
    result.append((char*)&leNum, tSize);
}

QList<U2AuxData> SamtoolsAdapter::samString2aux(const QByteArray &auxString) {
    CHECK(!auxString.isEmpty(), QList<U2AuxData>());
    // Adapted samtools code:
    QList<U2AuxData> result;
    QList<QByteArray> tokens = auxString.split('\t');
    foreach (const QByteArray &str, tokens) {
        U2AuxData aux;
        if (str.length() < 6 || str[2] != ':' || str[4] != ':') {
            coreLog.error("Samtools: missing colon in auxiliary data");
            continue;
        }
        aux.tag[0] = str[0]; aux.tag[1] = str[1];
        aux.type = str[3];
        if (aux.type == 'A' || aux.type == 'a' || aux.type == 'c' || aux.type == 'C') { // c and C for backward compatibility
            aux.type = 'A';
            aux.value.append(str[5]);
        } else if (aux.type == 'I' || aux.type == 'i') {
            QByteArray num = str.mid(5, 4);
            long long x = num.toLongLong();
            long long leX = qToLittleEndian<long long>(x);
            if (x < 0) {
                if (x >= -127) {
                    aux.type = 'c';
                    aux.value.append((char*)&leX, 1);
                } else if (x >= -32767) {
                    aux.type = 'x';
                    aux.value.append((char*)&leX, 2);
                } else {
                    aux.type = 'i';
                    aux.value.append((char*)&leX, 4);
                    if (x < -2147483648ll) {
                        coreLog.error(QString("Samtools: parse warning: integer %1 is out of range.").arg(x));
                    }
                }
            } else {
                if (x <= 255) {
                    aux.type = 'C';
                    aux.value.append((char*)&leX, 1);
                } else if (x <= 65535) {
                    aux.type = 'S';
                    aux.value.append((char*)&leX, 2);
                } else {
                    aux.type = 'I';
                    aux.value.append((char*)&leX, 4);
                    if (x > 4294967295ll) {
                        coreLog.error(QString("Samtools: parse warning: integer %1 is out of range.").arg(x));
                    }
                }
            }
        } else if (aux.type == 'f') {
            QByteArray num = str.mid(5, 4);
            float leX = qToLittleEndian<float>(num.toFloat());
            aux.value.append((char*)&leX, 4);
        } else if (aux.type == 'd') {
            QByteArray num = str.mid(9, 4);
            float leX = qToLittleEndian<float>(num.toFloat());
            aux.value.append((char*)&leX, 4);
        } else if (aux.type == 'Z' || aux.type == 'H') {
            int size = 1 + (str.length() - 5) + 1;
            if (aux.type == 'H') { // check whether the hex string is valid
                if ((str.length() - 5) % 2 == 1) {
                    coreLog.error("Samtools: length of the hex string not even.");
                    return result;
                }
                for (int i = 0; i < str.length() - 5; ++i) {
                    int c = toupper(str.data()[5 + i]);
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
                        coreLog.error("Samtools: invalid hex character.");
                        return result;
                    }
                }
            }
            aux.value = str.mid(5);
        } else if (aux.type == 'B') {
            int32_t k = 0;
            if (str.length() < 8) {
                coreLog.error("Samtools: too few values in aux type B.");
                continue;
            }
            aux.subType = str[5];
            QList<QByteArray> nums = str.mid(7).split(',');
            foreach (const QByteArray &num, nums) {
                if (aux.subType == 'c')      addNum<char>((char)num.toShort(), 1, aux.value);
                else if (aux.subType == 'C') addNum<char>((char)num.toUShort(), 1, aux.value);
                else if (aux.subType == 's') addNum<short>(num.toShort(), 2, aux.value);
                else if (aux.subType == 'S') addNum<ushort>(num.toUShort(), 2, aux.value);
                else if (aux.subType == 'i') addNum<int>(num.toInt(), 4, aux.value);
                else if (aux.subType == 'I') addNum<uint>(num.toUInt(), 4, aux.value);
                else if (aux.subType == 'f') addNum<float>(num.toFloat(), 4, aux.value);
                else {
                    coreLog.error("Samtools: unrecognized array type.");
                    continue;
                }
            }
        } else {
            coreLog.error("Samtools: unrecognized type.");
            continue;
        }
        result << aux;
    }
    return result;
}

QList<U2AuxData> SamtoolsAdapter::string2aux(const QByteArray &auxString) {
    CHECK(!auxString.isEmpty(), QList<U2AuxData>());
    // Adapted samtools code:
    QList<U2AuxData> result;
    const char *s = auxString.constData();
    const char *end = s + auxString.length();
    while (s < end) {
        U2AuxData aux;
        aux.tag[0] = s[0]; aux.tag[1] = s[1];
        s += 2; aux.type = *s; ++s;
        if (aux.type == 'A') { aux.value = QByteArray(s, 1); ++s; }
        else if (aux.type == 'C') { aux.value.append(s, 1); ++s; }
        else if (aux.type == 'c') { aux.value.append(s, 1); ++s; }
        else if (aux.type == 'S') { aux.value.append(s, 2); s += 2; }
        else if (aux.type == 's') { aux.value.append(s, 2); s += 2; }
        else if (aux.type == 'I') { aux.value.append(s, 4); s += 4; }
        else if (aux.type == 'i') { aux.value.append(s, 4); s += 4; }
        else if (aux.type == 'f') { aux.value.append(s, 4); s += 4; }
        else if (aux.type == 'd') { aux.value.append(s, 8); s += 8; }
        else if (aux.type == 'Z' || aux.type == 'H') { while (*s) aux.value.append(*s++); ++s; }
        else if (aux.type == 'B') {
            aux.subType = *(s++);
            qint32 n;
            memcpy(&n, s, 4);
            s += 4; // no point to the start of the array
            for (qint32 i = 0; i < n; ++i) {
                if ('c' == aux.subType || 'c' == aux.subType) { aux.value.append(s, 1); ++s; }
                else if ('C' == aux.subType) { aux.value.append(s, 1); ++s; }
                else if ('s' == aux.subType) { aux.value.append(s, 2); s += 2; }
                else if ('S' == aux.subType) { aux.value.append(s, 2); s += 2; }
                else if ('i' == aux.subType) { aux.value.append(s, 4); s += 4; }
                else if ('I' == aux.subType) { aux.value.append(s, 4); s += 4; }
                else if ('f' == aux.subType) { aux.value.append(s, 4); s += 4; }
            }
        }
        result << aux;
    }
    return result;
}

static bool check_seq2samtools(const bam1_t &b, QByteArray seq, U2OpStatus &os) {
    if(b.core.l_qseq != seq.length()) {
        os.setError(QString("Internal SamtoolsAdapter seq check failed: expected length %1, got %2").arg(seq.length()).arg(b.core.l_qseq));
        return false;
    }
    for(int i = 0; i < b.core.l_qseq; ++i) {
        char expected = seq[i];
        char actual = bam_nt16_rev_table[bam1_seqi(bam1_seq(&b), i)];
        if(expected != actual) {
            os.setError(QString("Internal SamtoolsAdapter seq check failed: expected %1, got %2 at pos %3").arg(expected).arg(actual).arg(i));
            return false;
        }
    }
    return true;
}

static bool check_qual(const bam1_t &b, QByteArray qual, U2OpStatus &os) {
    if(b.core.l_qseq != qual.length()) {
        os.setError(QString("Internal SamtoolsAdapter qual check failed: expected length %1, got %2").arg(qual.length()).arg(b.core.l_qseq));
        return false;
    }
    uint8_t * bQual = bam1_qual(&b);
    int lastBQualOffset = &bQual[b.core.l_qseq-1] - b.data;
    if(lastBQualOffset > b.data_len) {
        os.setError(QString("Internal SamtoolsAdapter qual check failed: quality out of bounds: data_len is %1, but last qual offset is %2").arg(b.data_len).arg(lastBQualOffset));
        return false;
    }
    for(int i = 0; i < b.core.l_qseq; ++i) {
        uint8_t expected = qual[i];
        uint8_t actual = bQual[i];
        if(expected != actual) {
            os.setError(QString("Internal SamtoolsAdapter qual check failed: expected %1, got %2 at pos %3").arg(expected).arg(actual).arg(i));
            return false;
        }
    }
    return true;
}

typedef quint8 *data_ptr;
inline static void copyArray(data_ptr &dest, const QByteArray array) {
    memcpy(dest, array.constData(), array.length());
    dest += array.length();
}

static const int SAMTOOLS_QUALITY_OFFSET = 33;
static const char SAMTOOLS_QUALITY_OFF_CHAR = 0xff;
bool SamtoolsAdapter::hasQuality(const QByteArray &qualString) {
    const char *data = qualString.constData();
    int size = qualString.length();

    bool result = false;
    for (int i=0; i<size; i++) {
        if(SAMTOOLS_QUALITY_OFF_CHAR != data[i]) {
            result = true;
            break;
        }
    }
    return result;
}

QByteArray SamtoolsAdapter::quality2samtools(const QByteArray &quality) {
    QByteArray samtoolsQuality = quality;
    if (hasQuality(quality)) {
        for (int i=0; i<quality.size(); i++) {
            samtoolsQuality.data()[i] -= SAMTOOLS_QUALITY_OFFSET;
        }
    }
    return samtoolsQuality;
}

QByteArray SamtoolsAdapter::samtools2quality(const QByteArray &samtoolsQuality) {
    QByteArray quality = samtoolsQuality;
    if (hasQuality(quality)) {
        for (int i=0; i<quality.size(); i++) {
            quality.data()[i] += SAMTOOLS_QUALITY_OFFSET;
        }
    }
    return quality;
}

inline static void copyQuality(data_ptr &dest, const QByteArray &quality) {
    QByteArray samtoolsQuality = SamtoolsAdapter::quality2samtools(quality);
    copyArray(dest, samtoolsQuality);
}

inline static void copyChar(data_ptr &dest, quint8 c) {
    *dest = c;
    ++dest;
}

void SamtoolsAdapter::reads2samtools(U2DbiIterator<U2AssemblyRead> *reads, U2OpStatus &os, ReadsContainer & result) {
    while(reads->hasNext()) {
        U2AssemblyRead r = reads->next();
        bam1_t resRead;
        read2samtools(r, os, resRead);
        CHECK_OP(os, );
        result.append(resRead);
    }
}

void SamtoolsAdapter::read2samtools(const U2AssemblyRead &read, const ReadsContext &ctx, U2OpStatus &os, bam1_t &result) {
    read2samtools(read, os, result);
    CHECK_OP(os, );
    result.core.tid = ctx.getReadAssemblyNum();
    if ("*" != read->rnext) {
        result.core.mtid = ctx.getAssemblyNum(read->rnext);
    }
    result.core.mpos = read->pnext;
}

void SamtoolsAdapter::read2samtools(const U2AssemblyRead &r, U2OpStatus &os, bam1_t &resRead) {
    bam1_core_t &core = resRead.core;

    memset(&resRead, 0, sizeof(resRead));

    core.tid = 0;
    core.pos = r->leftmostPos;

    core.bin = bam_reg2bin(r->leftmostPos, r->leftmostPos+r->effectiveLen);
    core.qual = r->mappingQuality;
    core.l_qname = r->name.length() + 1;

    core.flag = r->flags;
    core.n_cigar = r->cigar.length();

    core.l_qseq = r->readSequence.size();
    core.mtid = -1;
    core.mpos = -1;
    core.isize = 0;

    QByteArray quality = r->quality;
    if(quality.isEmpty()) {
        quality = QByteArray(core.l_qseq, 0xff);
    }

    QByteArray cigar = cigar2samtools(r->cigar, os);
    QByteArray seq = sequence2samtools(r->readSequence, os);
    QByteArray aux = aux2string(r->aux);
    int dataLen = r->name.length() + 1 + cigar.length() + seq.length() + quality.length() + aux.length();
    quint8 * data = new quint8[dataLen];
    quint8 * dest = data;
    copyArray(dest, r->name);
    copyChar(dest, '\0');
    copyArray(dest, cigar);
    copyArray(dest, seq);
    copyQuality(dest, quality);
    copyArray(dest, aux);

    resRead.l_aux = aux.length();
    resRead.data_len = resRead.m_data = dataLen;
    resRead.data = data;

    CHECK_OP(os,);

    // TODO: remove checks, make tests
    //CHECK_EXT(cigar2samtools(r->cigar, os).size() == 4*r->cigar.size(), os.setError("Internal SamtoolsAdapter cigar length check failed"),);
    //check_seq2samtools(resRead, r->readSequence, os);
    //CHECK_OP(os,);
    //check_qual(resRead, quality, os);
    //CHECK_OP(os,);
}

static bool startPosLessThan(const bam1_t &a, const bam1_t &b) {
    return a.core.pos < b.core.pos;
}

void ReadsContainer::sortByStartPos() {
    qSort(vector.begin(), vector.end(), startPosLessThan);
}

ReadsContainer::~ReadsContainer() {
    foreach(const bam1_t &b, vector) {
        delete b.data;
    }
}

/************************************************************************/
/* ReadsContext */
/************************************************************************/
ReadsContext::ReadsContext(const QString &_assemblyName,
    const QMap<QString, int> &_assemblyNumMap)
: assemblyName(_assemblyName), assemblyNumMap(_assemblyNumMap)
{

}

int ReadsContext::getReadAssemblyNum() const {
    return assemblyNumMap.value(assemblyName, -1);
}

int ReadsContext::getAssemblyNum(const QString &assemblyName) const {
    if ("=" == assemblyName) {
        return getReadAssemblyNum();
    } else if ("*" == assemblyName) {
        return -1;
    }
    return assemblyNumMap.value(assemblyName, -1);
}

} // namespace
