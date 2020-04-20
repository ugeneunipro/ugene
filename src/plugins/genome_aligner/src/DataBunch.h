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

#ifndef _U2_DATA_BUNCH_H_
#define _U2_DATA_BUNCH_H_

#include "GenomeAlignerSearchQuery.h"

namespace U2 {

struct DataBunch {
    ~DataBunch() {
        qDeleteAll(queries);
    }

    QVector<SearchQuery*> queries;
    QVector<BMType> bitValuesV;
    QVector<int> windowSizes;
    QVector<int> readNumbersV;
    QVector<int> positionsAtReadV;

    QVector<BMType> sortedBitValuesV;
    QVector<int> sortedIndexes;

    qint64 memoryHint() const;
    void squeeze();

    bool empty() const {
        return queries.empty() && bitValuesV.empty() && readNumbersV.empty() && positionsAtReadV.empty() && windowSizes.empty();
    }

    void prepareSorted();
};

} //namespace

#endif // _U2_WRITE_ALIGNER_READS_SUB_TASK_H_
