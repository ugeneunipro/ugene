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

#ifndef _U2_ROLLING_MATRIX_H_
#define _U2_ROLLING_MATRIX_H_

#include <cstdio>

namespace U2 {

//TODO: rename n,m to rows, columns

class U2ALGORITHM_EXPORT RollingMatrix {

public:
    RollingMatrix(int _n, int _m) : n(_n), m(_m), column0(0) {
        data = new int[n*m];
    }

    virtual ~RollingMatrix() {
        delete[] data;
    }

    virtual int get(int x, int y) const {
        int transX = transposeX(x);
        int transY = transposeY(y);
        int idx = getIdx(transX, transY);
        return data[idx];
    }

    void set(int x, int y, int v) {
        int transX = transposeX(x);
        int transY = transposeY(y);
        int idx = getIdx(transX, transY);
        data[idx] = v;
    }

    void dump() const {
        printf("----------------\n");
        for (int j=0; j<m; j++) {
            for (int i=0; i<n; i++) {
                printf("%x ", get(i, j));
            }
            printf("\n");
        }
    }

    void shiftColumn() {
        if (++column0  == n) {
            column0 = 0;
        }
    }

    static quint64 getMatrixSizeInBytes(const int _n, const int _m)
    {
        return _n * _m * sizeof(int);
    }

private:

    int getIdx(int x, int y) const {
        assert(x>=0 && y>=0 && x<n && y<m);
        return  x*m+y;
    }


    int transposeX(int x) const {
        assert(x >= 0 && x < n);
        return (column0 + x) % n ;
    }

    int transposeY(int y) const {
        assert(y >= 0 && y < m);
        return y;
    }

protected:
    int n;
    int m;
    int* data;
    int column0;
};



} //namespace

#endif
