/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
    int getLen(int x, int y) const { // NOLINT(misc-no-recursion)
        if (y == -1 || x == -1) {
            return 0;  // End of the matrix.
        }
        SAFE_POINT(x >= 0 && y >= 0, "Invalid X/Y range", -1);

        if (!allowInsDel) {
            int lengthBefore = getLen(x - 1, y - 1);
            CHECK(lengthBefore >= 0, -1);
            return 1 + lengthBefore;
        }
        int v = getValue(x, y);
        bool match = isMatch(x, y);
        int d = getValue(x - 1, y - 1);
        int l = getValue(x - 1, y);
        int u = getValue(x, y - 1);

        if (match && v == d + scores.match) {
            int lengthBefore = getLen(x - 1, y - 1);
            CHECK(lengthBefore >= 0, -1);
            return 1 + lengthBefore;
        }
        if (v == u + scores.del) {  // Prefer deletion in X sequence to minimize result length.
            int lengthBefore = getLen(x, y - 1);
            CHECK(lengthBefore >= 0, -1);
            return lengthBefore;
        }
        if (!match && v == d + scores.mismatch) {  // Prefer a mismatch instead of insertion into X sequence.
            int lengthBefore = getLen(x - 1, y - 1);
            CHECK(lengthBefore >= 0, -1);
            return 1 + lengthBefore;
        }
        SAFE_POINT(v == l + scores.ins, "Invalid value", -1);
        int lengthBefore = getLen(x - 1, y);  // This is an insertion into X sequence.
        CHECK(lengthBefore >= 0, -1);
        return 1 + lengthBefore;
    }

private:
    void setValue(int x, int y, int val, bool isMatch) {
        U2_ASSERT(((quint32)val & MATCH_MASK) == 0);
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
