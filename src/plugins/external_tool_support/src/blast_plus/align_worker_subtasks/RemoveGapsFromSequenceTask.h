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

#ifndef _U2_REMOVE_GAPS_FROM_SEQUENCE_TASK_H_
#define _U2_REMOVE_GAPS_FROM_SEQUENCE_TASK_H_

#include <U2Core/SequenceDbiWalkerTask.h>

namespace U2 {

class U2MsaGap;
class U2SequenceObject;

class FindGapsInSequenceCallback : public SequenceDbiWalkerCallback {
public:
    FindGapsInSequenceCallback(U2SequenceObject * const sequenceObject);

    void onRegion(SequenceDbiWalkerSubtask *subtask, TaskStateInfo &stateInfo);

    const QList<U2Region> &getGappedRegions() const;

private:
    void addGaps(const QList<U2MsaGap> &gaps);

    U2SequenceObject * const sequenceObject;

    QMutex mutex;
    QList<U2Region> gappedRegions;
};

class RemoveGapsFromSequenceTask : public Task {
    Q_OBJECT
public:
    RemoveGapsFromSequenceTask(U2SequenceObject * const sequenceObject);

private:
    void prepare();
    void run();

    U2SequenceObject * const sequenceObject;

    FindGapsInSequenceCallback callback;
    SequenceDbiWalkerTask *findGapsTask;

    static const int CHUNK_SIZE = 128000;
};

}   // namespace U2

#endif // _U2_REMOVE_GAPS_FROM_SEQUENCE_TASK_H_
