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

#ifndef _U2_WRITE_ALIGNER_READS_SUB_TASK_H_
#define _U2_WRITE_ALIGNER_READS_SUB_TASK_H_

#include "GenomeAlignerIndex.h"
#include "WriteAlignedReadsSubTask.h"

#include <U2Core/Task.h>
#include <U2Algorithm/DnaAssemblyTask.h>

namespace U2 {

class WriteAlignedReadsSubTask : public Task {
    Q_OBJECT
public:
    WriteAlignedReadsSubTask(QReadWriteLock &listM, QMutex& _writeLock, GenomeAlignerWriter *seqWriter, QList<DataBunch*> &data, quint64 &readsAligned);
    virtual void run();
private:
    GenomeAlignerWriter *seqWriter;
    QList<DataBunch*> &data;
    quint64 &readsAligned;

    inline void setReadWritten(SearchQuery *read, SearchQuery *revCompl);
    QReadWriteLock &listM;

    // lock to use when data is written to output.
    QMutex& writeLock;
};

} //namespace

#endif // _U2_WRITE_ALIGNER_READS_SUB_TASK_H_
