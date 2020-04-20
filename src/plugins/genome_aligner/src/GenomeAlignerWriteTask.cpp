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

#include "GenomeAlignerWriteTask.h"

namespace U2 {

GenomeAlignerWriteTask::GenomeAlignerWriteTask(GenomeAlignerWriter *s)
: Task("WriteAlignedReadsSubTask", TaskFlag_None),
seqWriter(s), end(false), writing(false), readsWritten(0)
{
}

void GenomeAlignerWriteTask::setSeqWriter(GenomeAlignerWriter *seqWriter) {
    this->seqWriter = seqWriter;
}

void GenomeAlignerWriteTask::addResult(SearchQuery *qu) {
    listMutex.lock();
    WriteData data;

    foreach (SAType offset, qu->getResults()) {
        data.qu = qu;
        data.offset = offset;
        results.append(data);
    }

    if (!writing && results.size() > 1000) {
        writing = true;
        waiter.wakeAll();
    }
    listMutex.unlock();
}

void GenomeAlignerWriteTask::setFinished() {
    end = true;
    waiter.wakeAll();
}

void GenomeAlignerWriteTask::flush() {
    try {
        writeMutex.lock();
        foreach (WriteData data, results) {
            seqWriter->write(data.qu, data.offset);
            setReadWritten(data.qu, data.qu->getRevCompl());
        }
        results.clear();
        writeMutex.unlock();
    } catch (const QString &exeptionMessage) {
        setError(exeptionMessage);
    }
}

void GenomeAlignerWriteTask::run() {
    stateInfo.setProgress(0);
    try {
        do {
            waitMutex.lock();
            writing = false;
            waiter.wait(&waitMutex);
            if (end) {
                writing = false;
                break;
            }

            QVector<WriteData> newResults;
            listMutex.lock();
            newResults += (results);
            results.clear();
            listMutex.unlock();
            waitMutex.unlock();

            writeMutex.lock();
            foreach (WriteData data, newResults) {
                seqWriter->write(data.qu, data.offset);
                setReadWritten(data.qu, data.qu->getRevCompl());
            }
            writeMutex.unlock();
        } while (!end);
    } catch (const QString &exeptionMessage) {
        setError(exeptionMessage);
    }
}

void GenomeAlignerWriteTask::setReadWritten(SearchQuery *read, SearchQuery *revCompl) {
    if (!read->isWroteResult()) {
        readsWritten++;
        read->writeResult();
        if (NULL != revCompl) {
            revCompl->writeResult();
        }
    }
}

} // U2
