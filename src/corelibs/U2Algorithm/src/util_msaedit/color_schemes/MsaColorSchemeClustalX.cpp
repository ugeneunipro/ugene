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

#include <U2Algorithm/MSAConsensusUtils.h>

#include <U2Core/MultipleAlignmentObject.h>

#include "MsaColorSchemeClustalX.h"

namespace U2 {

MsaColorSchemeClustalX::MsaColorSchemeClustalX(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj)
    : MsaColorScheme(parent, factory, maObj),
      objVersion(1),
      cacheVersion(0),
      aliLen(maObj->getLength())
{
    colorByIdx[ClustalColor_BLUE]    = "#80a0f0";
    colorByIdx[ClustalColor_RED]     = "#f01505";
    colorByIdx[ClustalColor_GREEN]   = "#15c015";
    colorByIdx[ClustalColor_PINK]    = "#f08080";
    colorByIdx[ClustalColor_MAGENTA] = "#c048c0";
    colorByIdx[ClustalColor_ORANGE]  = "#f09048";
    colorByIdx[ClustalColor_CYAN]    = "#15a4a4";
    colorByIdx[ClustalColor_YELLOW]  = "#c0c000";

    connect(maObj, SIGNAL(si_alignmentChanged(const MultipleAlignment &, const MaModificationInfo &)), SLOT(sl_alignmentChanged()));
}

QColor MsaColorSchemeClustalX::getBackgroundColor(int seq, int pos, char) const {
    if (cacheVersion != objVersion) {
        updateCache();
    }

    int idx = getColorIdx(seq, pos);
    assert(idx >=0 && idx < ClustalColor_NUM_COLORS);
    return colorByIdx[idx];
}

QColor MsaColorSchemeClustalX::getFontColor(int seq, int pos, char c) const {
    Q_UNUSED(seq);
    Q_UNUSED(pos);
    Q_UNUSED(c);

    return QColor();
}

void MsaColorSchemeClustalX::sl_alignmentChanged() {
    objVersion++;
}

namespace {

int basesContent(const int *freqs, const char *str, int len) {
    int res = 0;
    for (int i = 0; i < len; i++) {
        uchar c = str[i];
        res += freqs[c];
    }
    return res;
}

}

void MsaColorSchemeClustalX::updateCache() const {
    if (cacheVersion == objVersion) {
        return;
    }

    // compute colors for whole ali
    // use 4 bits per color
    const MultipleAlignment msa = maObj->getMultipleAlignment();
    int nSeq = msa->getNumRows();
    aliLen = maObj->getLength();
    cacheVersion = objVersion;

    bool stub = false;
    int cacheSize = getCacheIdx(nSeq, aliLen, stub) + 1;
    colorsCache.resize(cacheSize);

    /*  source: http://ekhidna.biocenter.helsinki.fi/pfam2/clustal_colours

        BLUE
            (W,L,V,I,M,F):  {50%, P}{60%, WLVIMAFCYHP}
            (A):            {50%, P}{60%, WLVIMAFCYHP}{85%, T,S,G}
            (C):            {50%, P}{60%, WLVIMAFCYHP}{85%, S}
        RED
            (K,R):          {60%, KR}{85%, Q}
        GREEN
            (T):            {50%, TS}{60%, WLVIMAFCYHP}
            (S):            {50%, TS}{80%, WLVIMAFCYHP}
            (N):            {50%, N}{85%, D}
            (Q):            {50%, QE}{60%, KR}
        PINK
            (C):            {85%, C}
        MAGENTA
            (D):            {50%, DE,N}
            (E):            {50%, DE,QE}
        ORANGE
            (G):            {ALWAYS}
        CYAN
            (H,Y):          {50%, P}{60%, WLVIMAFCYHP}
        YELLOW
            (P):            {ALWAYS}

        WARN: do not count gaps in percents!
    */

    QVector<int> freqsByChar(256);
    const int *freqs = freqsByChar.data();

    for (int pos = 0; pos < aliLen; pos++) {
        int nonGapChars = 0;
        MSAConsensusUtils::getColumnFreqs(msa, pos, freqsByChar, nonGapChars);
        int content50 = int(nonGapChars * 50.0 / 100);
        int content60 = int(nonGapChars * 60.0 / 100);
        int content80 = int(nonGapChars * 80.0 / 100);
        int content85 = int(nonGapChars * 85.0 / 100);

        for (int seq = 0; seq < nSeq; seq++) {
            char c = msa->charAt(seq, pos);
            int colorIdx = ClustalColor_NO_COLOR;
            switch(c) {
            case 'W': //(W,L,V,I,M,F): {50%, P}{60%, WLVIMAFCYHP} -> BLUE
            case 'L':
            case 'V':
            case 'I':
            case 'M':
            case 'F':
                if (freqs['P'] > content50 || basesContent(freqs, "WLVIMAFCYHP", 11) > content60) {
                    colorIdx = ClustalColor_BLUE;
                }
                break;
            case 'A': // {50%, P}{60%, WLVIMAFCYHP}{85%, T,S,G} -> BLUE
                if (freqs['P'] > content50 || basesContent(freqs, "WLVIMAFCYHP", 11) > content60) {
                    colorIdx = ClustalColor_BLUE;
                } else if (freqs['T'] > content85 || freqs['S'] > content85 || freqs['G'] > 85) {
                    colorIdx = ClustalColor_BLUE;
                }
                break;

            case 'K': //{60%, KR}{85%, Q} -> RED
            case 'R':
                if ((freqs['K'] + freqs['R'] > content60) || freqs['Q'] > content85) {
                    colorIdx = ClustalColor_RED;
                }
                break;

            case 'T': // {50%, TS}{60%, WLVIMAFCYHP} -> GREEN
                if ((freqs['T'] + freqs['S'] > content50) || basesContent(freqs, "WLVIMAFCYHP", 11) > content60) {
                    colorIdx = ClustalColor_GREEN;
                }
                break;

            case 'S': // {50%, TS}{80%, WLVIMAFCYHP} -> GREEN
                if ((freqs['T'] + freqs['S'] > content50) || basesContent(freqs, "WLVIMAFCYHP", 11) > content80) {
                    colorIdx = ClustalColor_GREEN;
                }
                break;

            case 'N': // {50%, N}{85%, D} -> GREEN
                if (freqs['N'] > content50 || freqs['D'] > content85) {
                    colorIdx = ClustalColor_GREEN;
                }
                break;

            case 'Q': // {50%, QE}{60%, KR} -> GREEN
                if ((freqs['Q'] + freqs['E']) > content50 || (freqs['K'] + freqs['R']) > content60) {
                    colorIdx = ClustalColor_GREEN;
                }
                break;

            case 'C': //{85%, C} -> PINK
                //{50%, P}{60%, WLVIMAFCYHP}{85%, S} -> BLUE
                if (freqs['C'] > content85) {
                    colorIdx = ClustalColor_PINK;
                } else if (freqs['P'] > content50 || basesContent(freqs, "WLVIMAFCYHP", 11) > content60 || freqs['S'] > content85) {
                    colorIdx = ClustalColor_BLUE;
                }
                break;

            case 'D': //{50%, DE,N} -> MAGENTA
                if ((freqs['D'] + freqs['E']) > content50 || freqs['N'] > content50) {
                    colorIdx = ClustalColor_MAGENTA;
                }
                break;
            case 'E': //{50%, DE,QE} -> MAGENTA
                if ((freqs['D'] + freqs['E']) > content50 || (freqs['Q'] + freqs['E']) > content50) {
                    colorIdx = ClustalColor_MAGENTA;
                }
                break;
            case 'G': //{ALWAYS} -> ORANGE
                colorIdx = ClustalColor_ORANGE;
                break;

            case 'H': // {50%, P}{60%, WLVIMAFCYHP} -> CYAN
            case 'Y':
                if (freqs['P'] > content50 || basesContent(freqs, "WLVIMAFCYHP", 11) > content60) {
                    colorIdx = ClustalColor_CYAN;
                }
                break;

            case 'P': //{ALWAYS} -> YELLOW
                colorIdx = ClustalColor_YELLOW;
                break;
            default:
                break;

            }
            setColorIdx(seq, pos, colorIdx);
        }
    }
}

int MsaColorSchemeClustalX::getCacheIdx(int seq, int pos, bool &low) const {
    assert(objVersion == cacheVersion);
    int res = seq * aliLen + pos;
    low = !(res & 0x1);
    return res / 2;
}

int MsaColorSchemeClustalX::getColorIdx(int seq, int pos) const {
    bool low = false;
    int cacheIdx = getCacheIdx(seq, pos, low);
    quint8 val = colorsCache[cacheIdx];
    int colorIdx = low ? val & 0x0F : (val & 0xF0) >> 4;
    assert(colorIdx >=0 && colorIdx < ClustalColor_NUM_COLORS);
    return colorIdx;
}

void MsaColorSchemeClustalX::setColorIdx(int seq, int pos, int colorIdx) const {
    assert(colorIdx >= 0 && colorIdx < ClustalColor_NUM_COLORS);
    bool low = false;
    int cacheIdx = getCacheIdx(seq, pos, low);
    quint8 val = colorsCache[cacheIdx];
    if (low) {
        val = (val & 0xF0) | colorIdx;
    } else {
        val = (val & 0x0F) | (colorIdx << 4);
    }
    colorsCache[cacheIdx] = val;
}

MsaColorSchemeClustalXFactory::MsaColorSchemeClustalXFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets) : MsaColorSchemeFactory(parent, id, name, supportedAlphabets)
{

}

MsaColorScheme * MsaColorSchemeClustalXFactory::create(QObject *parent, MultipleAlignmentObject *maObj) const {
    return new MsaColorSchemeClustalX(parent, this, maObj);
}

}   // namespace U2
