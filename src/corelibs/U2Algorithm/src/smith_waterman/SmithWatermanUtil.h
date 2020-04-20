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

#ifndef _U2_SMITH_WATERMAN_UTIL_H_
#define _U2_SMITH_WATERMAN_UTIL_H_

#include "SmithWatermanResult.h"


namespace U2 {

class SMatrix;

class U2ALGORITHM_EXPORT SmithWatermanUtil {
public:
    static float calcScoreItSelf(const SMatrix& m,
                                 const char* pattern,
                                 int patternLen);

    static int calcWindowLen(bool transl,
                             int sequenceLen,
                             int patternLen,
                             float scoreItSelf,
                             float thresholdScore,
                             float scoreGapOpen,
                             float scoreGapExtd);

    static float calcScoreThreshold(float percent,
                                    float scoreItSelf);

    static int calcOverlapSize(bool transl,
                               int windowLen,
                               float scoreItSelf,
                               float scoreGapOpen,
                               float scoreGapExtd);

    static bool removeIdenticalResults(QList<SmithWatermanResult>* lst);
};

} //namespace

#endif
