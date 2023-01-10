/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_DYN_TABLE_H_
#define _U2_DYN_TABLE_H_

#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

#include "RollingMatrix.h"

namespace U2 {

struct U2ALGORITHM_EXPORT MatchScores {
    int match = 0;
    int mismatch = 1;
    int ins = 1;
    int del = 1;
};

class U2ALGORITHM_EXPORT DynTable : public RollingMatrix {
public:
    DynTable(int sizeX, int sizeY, bool _allowInsDel, const MatchScores& _scores = {})
        : RollingMatrix(sizeX, sizeY), allowInsDel(_allowInsDel), scores(_scores) {
        // TODO: initial fill of the table does not respect the provided match scores and uses hardcoded 0,1,1,1 scores.
        for (int x = 0; x < sizeX; x++) {
            for (int y = 0; y < sizeY; y++) {
                setValue(x, y, y + 1, false);
            }
        }
    }

    int getLast() const {
        return getValue(sizeX - 1, sizeY - 1);
    }

    /**
     * Returns maximum match length for in the matrix.
     * Returns -1 if some unexpected error occurs.
     */
    int getLastLen() const {
        return getLen(sizeX - 1, sizeY - 1);
    }

    void match(int y, bool isMatch) {
        match(sizeX - 1, y, isMatch);
    }

    int get(int x, int y) const override {
        if (y < 0) {
            return 0;
        }
        if (x < 0) {
            return y + 1;
        }
        return RollingMatrix::get(x, y);
    }

    static qint64 estimateTableSizeInBytes(int n, int m) {
        return RollingMatrix::getMatrixSizeInBytes(n, m);
    }

protected:
    void match(int x, int y, bool isMatch) {
        int d = getValue(x - 1, y - 1);
        int res = d + (isMatch ? scores.match : scores.mismatch);
        if (allowInsDel) {
            int u = getValue(x, y - 1);
            int l = getValue(x - 1, y);
            int insDelRes = qMin(l + scores.ins, u + scores.del);
            res = qMin(insDelRes, res);
        }
        setValue(x, y, res, isMatch);
    }

    /**
     * Returns match length for the current point in the matrix.
     * Returns -1 if some unexpected error occurs.
     */
    int getLen(int x, int y) const {
        int lengthBefore = 0;
        for (; x >= 0 && y >= 0;) {
            if (!allowInsDel) {
                lengthBefore += 1;
                x--;
                y--;
                continue;
            }
            int v = getValue(x, y);
            bool match = isMatch(x, y);
            int d = getValue(x - 1, y - 1);
            int l = getValue(x - 1, y);
            int u = getValue(x, y - 1);

            if (match && v == d + scores.match) {
                lengthBefore += 1;
                x--;
                y--;
                continue;
            }
            if (v == u + scores.del) {  // Prefer deletion in X sequence to minimize result length.
                y--;
                continue;
            }
            if (!match && v == d + scores.mismatch) {  // Prefer a mismatch instead of insertion into X sequence.
                lengthBefore += 1;
                x--;
                y--;
                continue;
            }
            SAFE_POINT(v == l + scores.ins, "Invalid value", -1);
            lengthBefore += 1;
            x--;
        }
        return lengthBefore;
    }

private:
    void setValue(int x, int y, int val, bool isMatch) {
        SAFE_POINT(((quint32)val & MATCH_MASK) == 0, "Value don't match the mask.", );
        int valueWithMatchMask = int((quint32)val | (isMatch ? Flag_Match : Flag_Mismatch));
        set(x, y, valueWithMatchMask);
    }

    int getValue(int x, int y) const {
        return (int)((quint32)get(x, y) & VALUE_MASK);
    }

    bool isMatch(int x, int y) const {
        return (get(x, y) & MATCH_MASK) != 0;
    }

protected:
    const bool allowInsDel;

private:
    const static quint32 VALUE_MASK = 0x7FFFFFFF;
    const static quint32 MATCH_MASK = 0x80000000;

    enum MismatchFlag {
        Flag_Mismatch = 0x00000000,
        Flag_Match = 0x80000000
    };

    MatchScores scores;
};

}  // namespace U2

#endif
