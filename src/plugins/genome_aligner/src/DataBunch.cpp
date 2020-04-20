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

#include "DataBunch.h"
#include <U2Core/Timer.h>
#include <U2Core/Log.h>
#include <U2Algorithm/SyncSort.h>

namespace U2 {

void DataBunch::prepareSorted() {

    if (sortedBitValuesV.size()>0) {
        return;
    }

    qint64 t0 = GTimer::currentTimeMicros();
    sortedBitValuesV.reserve(bitValuesV.size());
    foreach(BMType n, bitValuesV) {
        sortedBitValuesV.append(n);
    }
    sortedBitValuesV.squeeze();

    sortedIndexes.resize(sortedBitValuesV.size());
    for (int i=0; i<sortedIndexes.size(); i++) {
        sortedIndexes[i] = i;
    }
    sortedIndexes.squeeze();
    algoLog.trace(QString("DataBunch::prepareSorted copy %1 results in %2 ms.").arg(bitValuesV.size()).arg((GTimer::currentTimeMicros() - t0) / double(1000), 0, 'f', 3));

    t0 = GTimer::currentTimeMicros();
    SyncSort<BMType, int> s(sortedBitValuesV, sortedIndexes);
    s.sort();
    algoLog.trace(QString("DataBunch::prepareSorted: Sorted %1 results in %2 ms.").arg(bitValuesV.size()).arg((GTimer::currentTimeMicros() - t0) / double(1000), 0, 'f', 3));
}

qint64 DataBunch::memoryHint() const {
    qint64 m = sizeof(*this);

    m += queries.capacity() * (qint64)sizeof(SearchQuery*);
    m += bitValuesV.capacity() * (qint64)sizeof(BMType);
    m += readNumbersV.capacity() * (qint64)sizeof(int);
    m += positionsAtReadV.capacity() * (qint64)sizeof(int);
    m += windowSizes.capacity() * (qint64)sizeof(int);

    // sorted optimization on prepareSorted
    m += bitValuesV.size() * (qint64)sizeof(BMType);
    m += bitValuesV.size() * (qint64)sizeof(int);

    return m;
}

void DataBunch::squeeze() {
    queries.squeeze();
    bitValuesV.squeeze();
    readNumbersV.squeeze();
    positionsAtReadV.squeeze();
    windowSizes.squeeze();
}

} // U2
