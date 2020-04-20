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

#include "MSAConsensusUtils.h"
#include "MSAConsensusAlgorithm.h"

#include <QVector>
#include <QPair>

namespace U2 {

void MSAConsensusUtils::updateConsensus(const MultipleAlignment& ma, QByteArray& cons, MSAConsensusAlgorithm* algo) {
    U2Region r(0, ma->getLength());
    updateConsensus(ma, r, cons, algo);
}

void MSAConsensusUtils::updateConsensus(const MultipleAlignment& ma, const U2Region& region, QByteArray& cons, MSAConsensusAlgorithm* algo) {
    QVector<U2Region> l;
    l.append(region);
    updateConsensus(ma, l, cons, algo);
}

void MSAConsensusUtils::updateConsensus(const MultipleAlignment& ma, const QVector<U2Region>& regions, QByteArray& cons, MSAConsensusAlgorithm* algo) {
    if (ma->isEmpty()) {
        return;
    }
    int aliLen = ma->getLength();
    if (cons.length()!=aliLen) {
        cons.resize(aliLen);
    }

    foreach(const U2Region& r, regions) {
        for (int i = r.startPos, n = r.endPos(); i < n ; i++) {
            cons[i] = algo->getConsensusChar(ma, i);
        }
    }
}


QString MSAConsensusUtils::getConsensusPercentTip(const MultipleAlignment& ma, int pos, int minReportPercent, int maxReportChars) {
    if (ma->getLength() == 0) {
        return QString();
    }
    QVector<QPair<int, char> > freqs(32); //TODO: try QVarLengthArray?
    assert(pos>=0 && pos < ma->getLength());
    int nSeq = ma->getNumRows();
    assert(nSeq > 0);
    if (nSeq == 0) {
        return QString();
    }
    int gaps = 0;
    for (int seq = 0; seq < nSeq; seq++) {
        uchar c = (uchar)ma->charAt(seq, pos);
        if (c >= 'A' && c <= 'Z') {
            int idx = c - 'A';
            freqs[idx].first++;
            freqs[idx].second = c;
        } else {
            // count gaps
            ++gaps;
        }
    }
    qSort(freqs);
    double percentK = 100.0 / nSeq;

    QString res = "<table cellspacing=7>";
    int i = 0;
    for(; i < 32; i++) {
        int p = freqs[freqs.size()-i-1].first;
        double percent = p * percentK;
        if (percent < minReportPercent || percent < 1.0 / nSeq) {
            break;
        }
        if (i == maxReportChars) {
            break;
        }
        int c = freqs[freqs.size()-i-1].second;
        res = res + "<tr><td><b>" + QChar(c) + "</b></td>";
        res = res + "<td align=right>" + QString::number(percent, 'f', 1) + "%</td>";
        res = res + "<td align=right>" + QString::number(p) + "</td>";
        res = res + "</tr>";
    }
    if (i == 0) {
        return "";
    }
    if (i == maxReportChars) {
        res+="<tr><td colspan=3>...</td></tr>";
    }

    // adding gaps statstics
    if (gaps != 0) {
        double gapsPercent = gaps*percentK;
        res += "<tr><td><b>" + QObject::tr("Gaps:") + "</b></td>";
        res = res + "<td align=right>" + QString::number(gapsPercent, 'f', 1) + "%</td>";
        res = res + "<td align=right>" + QString::number(gaps) + "</td>";
        res = res + "</tr>";
    }

    res+="</table>";
    return res;
}


void MSAConsensusUtils::unpackConsensusCharsFromInt(quint32 val, char* charVal, int* maskPos) {
    for (int i = 0; i < 4; i++) {
        quint32 byteVal = (val >> (i * 8)) & 0xFF;
        maskPos[i] = byteVal >> 5;
        charVal[i] = (maskPos[i] < 4) ? (byteVal & 0x1F) + 'A' : 0;
    }
}

uchar MSAConsensusUtils::getColumnFreqs(const MultipleAlignment& ma, int pos, QVector<int>& freqsByChar,
                                        int& nonGapChars, const QVector<int>& seqIdx) {
    assert(freqsByChar.size() == 256);
    freqsByChar.fill(0);
    nonGapChars = 0;
    uchar maxC = 0;
    int  maxCFreq = 0;
    int* freqs = freqsByChar.data();
    int nSeq = seqIdx.isEmpty() ? ma->getNumRows() : seqIdx.size();
    for (qint64 seq = 0; seq < nSeq; seq++) {
        uchar c = (uchar)ma->charAt( seqIdx.isEmpty() ? seq : seqIdx[ seq ],
                                    pos);
        freqs[c]++;
        if (c!=U2Msa::GAP_CHAR && freqs[c] > maxCFreq) {
            maxCFreq = freqs[c];
            maxC = c;
        }
        if (c!=U2Msa::GAP_CHAR) {
            nonGapChars++;
        }
    }
    return maxC;
}

quint32 MSAConsensusUtils::packConsensusCharsToInt(const MultipleAlignment& ma, int pos, const int* mask4, bool gapsAffectPercents) {
    QVector<QPair<int, char> > freqs(32);
    int numNoGaps = 0;
    int nSeq = ma->getNumRows();
    for (int seq = 0; seq < nSeq; seq++) {
        uchar c = (uchar)ma->charAt(seq, pos);
        if (c >= 'A' && c <= 'Z') {
            int idx = c - 'A';
            freqs[idx].first++;
            freqs[idx].second = c;
            numNoGaps++;
        }
    }
    qSort(freqs);
    if (!gapsAffectPercents && numNoGaps == 0) {
        return 0xE0E0E0E0;//'4' in masks, '0' in values
    }
    int res = 0;
    double percentK = 100.0 / (gapsAffectPercents ? nSeq : numNoGaps);
    for (int i=0;i < 4;i++) {
        int p = int(freqs[freqs.size()-i-1].first * percentK);
        quint32 rangeBits = (p >= mask4[0]) ?  0 :
            (p >= mask4[1]) ?  1 :
            (p >= mask4[2]) ?  2 :
            (p >= mask4[3]) ?  3 : 4;
        quint32 charVal = rangeBits == 4  ? 'A' : quint32(freqs[freqs.size()-i-1].second);
        quint32 maskedVal = (rangeBits << 5) | (charVal - 'A'); //3 bits for range, 5 for symbol
        assert(maskedVal <= 255);
        res = res | (maskedVal << (8 * i));
    }
    return res;
}


}//namespace
