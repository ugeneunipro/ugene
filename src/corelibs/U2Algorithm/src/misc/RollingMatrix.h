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

#ifndef _U2_ROLLING_MATRIX_H_
#define _U2_ROLLING_MATRIX_H_

#include <cstdio>

namespace U2 {

// TODO: rename n,m to rows, columns

class U2ALGORITHM_EXPORT RollingMatrix {
public:
    RollingMatrix(int _sizeX, int _sizeY)
        : sizeX(_sizeX), sizeY(_sizeY), column0(0) {
        // TODO: check memory overflow on the client side!
        U2_ASSERT(sizeX >= 0 && sizeY >= 0);
        U2_ASSERT(getMatrixSizeInBytes(sizeX, sizeY) < INT_MAX);
        data = new int[sizeX * sizeY];
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
        for (int y = 0; y < sizeY; y++) {
            for (int x = 0; x < sizeX; x++) {
                printf("%x ", get(x, y));
            }
            printf("\n");
        }
    }

    void shiftColumn() {
        if (++column0 == sizeX) {
            column0 = 0;
        }
    }

    static qint64 getMatrixSizeInBytes(int sizeX, int sizeY) {
        return (qint64)sizeX * sizeY * (int)sizeof(int);
    }

private:
    int getIdx(int x, int y) const {
        U2_ASSERT(x >= 0 && y >= 0 && x < sizeX && y < sizeY);
        return x * sizeY + y;
    }

    int transposeX(int x) const {
#ifndef U2_SUPPRESS_GCC_5_4_FALSE_WARNINGS
        U2_ASSERT(x >= 0 && x < sizeX);
#endif
        return (column0 + x) % sizeX;
    }

    int transposeY(int y) const {
#ifndef U2_SUPPRESS_GCC_5_4_FALSE_WARNINGS
        U2_ASSERT(y >= 0 && y < sizeY);
#endif
        return y;
    }

protected:
    const int sizeX;
    const int sizeY;
    int* data = nullptr;
    int column0;
};

}  // namespace U2

#endif
