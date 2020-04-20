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

#include "U2AssemblyUtils.h"

#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

const int U2AssemblyUtils::MAX_COVERAGE_VECTOR_SIZE = 1000 * 1000;

U2CigarOp U2AssemblyUtils::char2Cigar(char c, QString& err) {
    char cu = TextUtils::UPPER_CASE_MAP[c];
    switch (cu) {
    case 'D':
        return U2CigarOp_D; // deleted
    case 'I':
        return U2CigarOp_I; // inserted
    case 'H':
        return U2CigarOp_H; // hard-clipped
    case 'M':
        return U2CigarOp_M; // matched
    case 'N':
        return U2CigarOp_N; // skipped
    case 'P':
        return U2CigarOp_P; // padded
    case 'S':
        return U2CigarOp_S; // soft-clipped
    case '=':
        return U2CigarOp_EQ; // sequence match
    case 'X':
        return U2CigarOp_X; // sequence mismatch
    }
    err = tr("Invalid CIGAR op: '%1'!").arg(c);
    return U2CigarOp_Invalid;
}

char U2AssemblyUtils::cigar2Char(U2CigarOp op) {
    char c;
    switch (op) {
    case U2CigarOp_D: c = 'D'; break;
    case U2CigarOp_I: c = 'I'; break;
    case U2CigarOp_H: c = 'H'; break;
    case U2CigarOp_M: c = 'M'; break;
    case U2CigarOp_N: c = 'N'; break;
    case U2CigarOp_P: c = 'P'; break;
    case U2CigarOp_S: c = 'S'; break;
    case U2CigarOp_EQ:c = '='; break;
    case U2CigarOp_X: c = 'X'; break;
    default: assert(0); c = '?';
    }
    return c;
}

QList<U2CigarToken> U2AssemblyUtils::parseCigar(const QByteArray& cigarString, QString& err) {
    QList<U2CigarToken> result;
    int pos = 0, len = cigarString.length();
    const char* cigar = cigarString.constData();
    int count = 0;
    // count numbers
    while (pos < len) {
        char c = cigar[pos++];
        if (c >= '0' && c <= '9') {
            int n = c - '0';
            count = count * 10 + n;
            continue;
        }
        U2CigarOp op = char2Cigar(c, err);
        if (!err.isEmpty()) {
            break;
        }
        result.append(U2CigarToken(op, count));
        count = 0;
    }
    return result;
}


QByteArray U2AssemblyUtils::cigar2String(const QList<U2CigarToken>& cigar) {
    QByteArray res;
    foreach(const U2CigarToken& t, cigar) {
        if (t.op != U2CigarOp_Invalid) {
            res = res + QByteArray::number(t.count) + cigar2Char(t.op);
        }
    }
    return res;
}

qint64 U2AssemblyUtils::getEffectiveReadLength(const U2AssemblyRead& read) {
    return read->readSequence.length() + getCigarExtraLength(read->cigar);
}

qint64 U2AssemblyUtils::getCigarExtraLength(const QList<U2CigarToken>& cigar) {
    qint64 res = 0;
    foreach(const U2CigarToken& t, cigar) {
        switch (t.op) {
        case U2CigarOp_I:
        case U2CigarOp_S:
            res -= t.count;
            break;
        case U2CigarOp_D:
        case U2CigarOp_N:
            res += t.count;
            break;
        default:;
        }
    }
    return res;
}

QByteArray U2AssemblyUtils::serializeCoverageStat(const U2AssemblyCoverageStat& coverageStat) {
    QByteArray data;
    for (int index = 0;index < coverageStat.size();index++) {
        for (int i = 0;i < 4;i++) {
            data.append((char)(coverageStat[index] >> (i * 8)));
        }
    }
    return data;
}

void U2AssemblyUtils::deserializeCoverageStat(QByteArray data, U2AssemblyCoverageStat& res, U2OpStatus &os) {
    res.clear();
    if (!data.isEmpty() && 0 == (data.size() % 4)) {
        for (int index = 0;index < data.size() / 4;index++) {
            int value = 0;
            for (int i = 0;i < 4;i++) {
                value |= ((int)data[index * 4 + i] & 0xff) << (i * 8);
            }
            res.append(value);
        }
    } else {
        os.setError("Invalid attribute size");
    }
}

} //namespace
