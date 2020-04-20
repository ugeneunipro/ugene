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

#include "MSAConsensusAlgorithmClustal.h"

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/DNAAlphabet.h>

namespace U2 {

QString MSAConsensusAlgorithmFactoryClustal::getDescription() const {
    return tr("Emulates ClustalW program and file format behavior.");
}

QString MSAConsensusAlgorithmFactoryClustal::getName() const {
    return tr("ClustalW");
}


MSAConsensusAlgorithm* MSAConsensusAlgorithmFactoryClustal::createAlgorithm(const MultipleAlignment&, bool ignoreTrailingLeadingGaps, QObject* p) {
    return new MSAConsensusAlgorithmClustal(this, ignoreTrailingLeadingGaps, p);
}

//////////////////////////////////////////////////////////////////////////
//Algorithm

char MSAConsensusAlgorithmClustal::getConsensusChar(const MultipleAlignment& ma, int pos, QVector<int> seqIdx) const {
    CHECK(filterIdx(seqIdx, ma, pos), INVALID_CONS_CHAR);

    if (!ma->getAlphabet()->isAmino()) {
        // for nucleic alphabet work as strict algorithm but use ' ' as default
        char  defChar = ' ';
        char pc = ( seqIdx.isEmpty() ? ma->getRows().first() : ma->getRows()[ seqIdx[0] ] )->charAt(pos);
        if (pc == U2Msa::GAP_CHAR) {
            pc = defChar;
        }
        int nSeq =( seqIdx.isEmpty() ? ma->getNumRows() : seqIdx.size());
        for (int s = 1; s < nSeq; s++) {
            char c = ma->getRow(seqIdx.isEmpty() ? s : seqIdx[s])->charAt(pos);
            if (c != pc) {
                pc = defChar;
                break;
            }
        }
        char res = (pc == defChar) ? defChar : '*';
        return res;
    } else {
        /* From ClustalW doc:
        '*' indicates positions which have a single, fully conserved residue
        ':' indicates that one of the following 'strong' groups is fully conserved:
        STA, NEQK, NHQK, NDEQ, QHRK, MILV, MILF, HY, FYW,
        '.' indicates that one of the following 'weaker' groups is fully conserved:
        CSA, ATV, SAG, STNK, STPA, SGND, SNDEQK, NDEQHK, NEQHRK, FVLIM, HFY
        */
        static QByteArray strongGroups[] = {"STA", "NEQK", "NHQK", "NDEQ", "QHRK", "MILV", "MILF", "HY", "FYW"};
        static QByteArray weakGroups[]   = {"CSA", "ATV", "SAG", "STNK", "STPA", "SGND", "SNDEQK", "NDEQHK", "NEQHRK", "FVLIM", "HFY"};
        static int maxStrongGroupLen = 4;
        static int maxWeakGroupLen = 6;

        QByteArray currentGroup; //TODO: optimize 'currentGroup' related code!
        int nSeq =( seqIdx.isEmpty() ? ma->getNumRows() : seqIdx.size());
        for (int s = 0; s < nSeq; s++) {
            char c = ma->getRow(seqIdx.isEmpty() ? s : seqIdx[s])->charAt(pos);
            if (!currentGroup.contains(c)) {
                currentGroup.append(c);
            }
        }
        char consChar = U2Msa::GAP_CHAR;
        if (currentGroup.size() == 1) {
            consChar = (currentGroup[0] == U2Msa::GAP_CHAR) ? ' ' : '*';
        } else  {
            bool ok = false;
            int currentLen = currentGroup.length();
            const char* currentGroupData = currentGroup.data();
            //check strong groups
            if (currentLen <= maxStrongGroupLen) {
                for (int sgi=0, sgn = sizeof(strongGroups) / sizeof(QByteArray); sgi < sgn && !ok; sgi++) {
                    bool matches = true;
                    const QByteArray& sgroup = strongGroups[sgi];
                    for (int j=0; j < currentLen && matches; j++) {
                        char c = currentGroupData[j];
                        matches = sgroup.contains(c);
                    }
                    ok = matches;
                }
                if (ok) {
                    consChar = ':';
                }
            }

            //check weak groups
            if (!ok && currentLen <= maxWeakGroupLen) {
                for (int wgi=0, wgn = sizeof(weakGroups) / sizeof(QByteArray); wgi < wgn && !ok; wgi++) {
                    bool matches = true;
                    const QByteArray& wgroup = weakGroups[wgi];
                    for (int j=0; j < currentLen && matches; j++) {
                        char c = currentGroupData[j];
                        matches = wgroup.contains(c);
                    }
                    ok = matches;
                }
                if (ok) {
                    consChar = '.';
                }
            }
            //use default
            if (!ok) {
                consChar = ' ';
            }
        } //amino
        return consChar;
    }
}

U2::MSAConsensusAlgorithmClustal* MSAConsensusAlgorithmClustal::clone() const {
    return new MSAConsensusAlgorithmClustal(*this);
}

} //namespace
