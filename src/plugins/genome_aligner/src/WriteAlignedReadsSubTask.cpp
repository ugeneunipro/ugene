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

#include "WriteAlignedReadsSubTask.h"

namespace U2 {

WriteAlignedReadsSubTask::WriteAlignedReadsSubTask(QReadWriteLock& _listM, QMutex& _writeLock, GenomeAlignerWriter* _seqWriter, QList<DataBunch*>& _data, qint64& r)
    : Task("WriteAlignedReadsSubTask", TaskFlag_None), seqWriter(_seqWriter), data(_data), readsAligned(r), listM(_listM), writeLock(_writeLock) {
}

void WriteAlignedReadsSubTask::setReadWritten(SearchQuery* read, SearchQuery* revCompl) {
    if (!read->isWroteResult()) {
        readsAligned++;
        read->writeResult();
        if (nullptr != revCompl) {
            revCompl->writeResult();
        }
    }
}

void WriteAlignedReadsSubTask::run() {
    // ReadShortReadsSubTask can add new data what can lead to realloc. Noone can touch these vectors without sync
    QReadLocker locker(&listM);
    QMutexLocker writeLockLocker(&writeLock);

    stateInfo.setProgress(0);

    foreach (DataBunch* d, data) {
        SAFE_POINT(nullptr != d, "NULL data bunch", );

        QVector<SearchQuery*>& queries = d->queries;

        try {
            SearchQuery* read = nullptr;
            SearchQuery* revCompl = nullptr;
            SearchQuery** q = queries.data();
            int size = queries.size();

            for (int i = 0; i < size; i++) {
                read = q[i];
                revCompl = read->getRevCompl();

                if (i < size - 1 && revCompl == q[i + 1]) {
                    continue;
                }

                if (nullptr == revCompl && read->haveResult()) {
                    seqWriter->write(read, read->firstResult());
                    readsAligned++;
                } else if (nullptr != revCompl) {
                    int c = read->firstMCount();
                    int cRev = revCompl->firstMCount();

                    if (c <= cRev && c < INT_MAX) {
                        seqWriter->write(read, read->firstResult());
                        readsAligned++;
                    } else if (cRev < INT_MAX) {
                        seqWriter->write(revCompl, revCompl->firstResult());
                        readsAligned++;
                    }
                }
            }
        } catch (QString exeptionMessage) {
            setError(exeptionMessage);
        }
    }
}

}  // namespace U2
