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

#ifndef _U2_REALIGN_SEQUENCES_IN_ALIGNMENT_TASK_
#define _U2_REALIGN_SEQUENCES_IN_ALIGNMENT_TASK_

#include <QSet>
#include <QStringList>

#include <U2Core/Task.h>

namespace U2 {

class MultipleSequenceAlignmentObject;
class StateLocker;

class U2VIEW_EXPORT RealignSequencesInAlignmentTask : public Task {
    Q_OBJECT
public:
    RealignSequencesInAlignmentTask(MultipleSequenceAlignmentObject* msaObject, const QSet<qint64>& sequencesToAlignIds, bool forceUseUgeneNativeAligner = false);
    ~RealignSequencesInAlignmentTask();

    Task::ReportResult report();

protected:
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    MultipleSequenceAlignmentObject* originalMsaObject;
    MultipleSequenceAlignmentObject* msaObject;
    const QSet<qint64> rowsToAlignIds;
    QStringList originalRowOrder;
    Task *extractSequences;
    QString extractedSequencesDirUrl;
    StateLocker *locker;
};

}

#endif
