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

#include "SmithWatermanUtil.h"

#include <U2Core/SMatrix.h>

namespace U2 {

float SmithWatermanUtil::calcScoreItSelf(const SMatrix& m, const char* pattern,  int patternLen) {
    float score = 0.0F;
    for (int i = 0; i < patternLen; i++) {
        score += m.getScore(pattern[i], pattern[i]);
    }
    return score;
}

int SmithWatermanUtil::calcWindowLen(bool transl,
                                     int sequenceLen,
                                     int patternLen,
                                     float scoreItSelf,
                                     float thresholdScore,
                                     float scoreGapOpen,
                                     float scoreGapExtd) {
    int tail = (thresholdScore - scoreItSelf + scoreGapOpen) / scoreGapExtd;
    tail = qMax(tail, 0);
    int windowLen = qMin(patternLen + tail, sequenceLen);
    if (transl) {
        windowLen *= 3;
    }
    return windowLen;
}

float SmithWatermanUtil::calcScoreThreshold(float percent,
                                            float scoreItSelf) {
    if (percent < 0 || 100 < percent) {
        percent = 100;
    }
    float factor = (percent / 100.0F);
    return factor * scoreItSelf;
}

int SmithWatermanUtil::calcOverlapSize(bool transl, int windowLen, float scoreItSelf, float scoreGapOpen, float scoreGapExtd) {
    int dependSize = (scoreGapOpen - scoreItSelf) / scoreGapExtd;
    if (transl) {
        dependSize *= 3;
    }
    return qMax(windowLen, dependSize);
}

//greater score goes first
static bool revScoreComparator(const SmithWatermanResult &s0, const SmithWatermanResult &s1) {
    bool res = false;
    if (s0.score < s1.score) {
        res = false;
    } else if (s0.score == s1.score) {
        const U2Region& r0 = s0.refSubseq;
        const U2Region& r1 = s1.refSubseq;
        res = r0 < r1;
    } else {
        res = true;
    }
    return res;
}

bool SmithWatermanUtil::removeIdenticalResults(QList<SmithWatermanResult>* lst) {
    QList<SmithWatermanResult>& results = *lst;

    qSort(results.begin(), results.end(), revScoreComparator);
    int i = 0;
    int size = results.size();
    while (i < size) {
        int j = i + 1;
        while (j < size) {
            const SmithWatermanResult& currItem = results[i];
            const SmithWatermanResult& someItem = results[j];
            if (currItem.refSubseq == someItem.refSubseq &&
                currItem.strand == someItem.strand) {
                results.removeAt(j);
                size--;
                continue;
            }
            j++;
        }
        i++;
    }
    return true;
}

} // namespace
