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

#ifndef _U2_READ_SHORT_READS_SUB_TASK_H_
#define _U2_READ_SHORT_READS_SUB_TASK_H_

#include "GenomeAlignerSearchQuery.h"
#include "GenomeAlignerIndex.h"
#include "ReadShortReadsSubTask.h"

#include <U2Core/Task.h>
#include <U2Algorithm/DnaAssemblyTask.h>

namespace U2 {

class GenomeAlignerTask;
class GenomeAlignerReader;

class ReadShortReadsSubTask : public Task {
    Q_OBJECT
public:
    ReadShortReadsSubTask(SearchQuery **lastQuery,
                          GenomeAlignerReader *seqReader,
                          const DnaAssemblyToRefTaskSettings& settings,
                          AlignContext &alignContext,
                          quint64 freeMemorySize);
    virtual void run();

    uint bunchSize;
    int minReadLength;
    int maxReadLength;

private:
    SearchQuery **lastQuery;
    GenomeAlignerReader *seqReader;
    const DnaAssemblyToRefTaskSettings &settings;
    AlignContext &alignContext;
    quint64 freeMemorySize;
    qint64 prevMemoryHint;

    DataBunch* dataBunch;

    inline bool add(int &CMAX, int &W, int &q, int &readNum, SearchQuery *query, GenomeAlignerTask *parent);
    void dropToAlignContext();
    void readingFinishedWakeAll();
};

} //namespace

#endif // _U2_READ_SHORT_READS_SUB_TASK_H_
