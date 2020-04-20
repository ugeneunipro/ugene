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

#ifndef _U2_GENOME_ALIGNER_WRITE_TASK_H_
#define _U2_GENOME_ALIGNER_WRITE_TASK_H_

#include "GenomeAlignerIO.h"
#include "GenomeAlignerSearchQuery.h"

#include <QWaitCondition>
#include <QMutex>
#include <QList>

namespace U2 {

class WriteData {
public:
    SearchQuery *qu;
    SAType offset;
};

class GenomeAlignerWriteTask : public Task {
    Q_OBJECT
public:
    GenomeAlignerWriteTask(GenomeAlignerWriter *seqWriter);
    virtual void run();

    void addResult(SearchQuery *qu);
    void flush();
    void setFinished();
    quint64 getWrittenReadsCount() const {return readsWritten;}
    void setSeqWriter(GenomeAlignerWriter *seqWriter);
private:
    GenomeAlignerWriter *seqWriter;
    QVector<WriteData> results;
    bool end;
    bool writing;
    quint64 readsWritten;

    QMutex listMutex;
    QMutex writeMutex;
    QMutex waitMutex;
    QWaitCondition waiter;

    static const int MAX_LIST_SIZE = 1000;

    inline void setReadWritten(SearchQuery *read, SearchQuery *revCompl);
};
} //namespace

#endif // _U2_GENOME_ALIGNER_WRITE_TASK_H_
