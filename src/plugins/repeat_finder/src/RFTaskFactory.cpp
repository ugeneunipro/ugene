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

#include "RFTaskFactory.h"
#include "RFBase.h"
#include "FindRepeatsTask.h"

namespace U2 {

Task* RFTaskFactory::getTaskInstance( const RepeatFinderSettings& c) const {

    if (c.inverted) {
        return new ReverseAndCreateTask(c);
    } else {
        return RFAlgorithmBase::createTask(
            c.l,
            c.seqX,
            c.sizeX,
            c.seqY,
            c.sizeY,
            c.al,
            c.w,
            c.mismatches,
            c.alg,
            c.nThreads
        );
    }
}

void RFTaskFactory::setRFResultsListener(Task* b, RFResultsListener* l) {
    RFAlgorithmBase* rfTask = qobject_cast<RFAlgorithmBase*>(b);
    if (rfTask) {
        rfTask->setRFResultsListener(l);
    }
}

ReverseAndCreateTask::ReverseAndCreateTask(const RepeatFinderSettings& c)
: Task("Make reverse sequence and find repeats", TaskFlag_NoRun)
{
    this->c = c;

    revTask = new RevComplSequenceTask(DNASequence(QByteArray(c.seqX), c.al), U2Region(0, c.sizeX));
    addSubTask(revTask);
}

QList<Task*> ReverseAndCreateTask::onSubTaskFinished(Task* subTask) {

    QList<Task*> subTasks;

    if (revTask == subTask) {

        Task *rfBase = RFAlgorithmBase::createTask(
            c.l,
            revTask->complementSequence.constData(),
            c.sizeX,
            c.seqY,
            c.sizeY,
            c.al,
            c.w,
            c.mismatches,
            c.alg,
            c.nThreads
        );
        subTasks.append(rfBase);
    }

    return subTasks;
}

} // namespace
