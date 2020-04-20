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

#ifndef _U2_REPEAT_FINDER_ALG_IMPL_H_
#define _U2_REPEAT_FINDER_ALG_IMPL_H_

#include <U2Algorithm/RepeatFinderTaskFactory.h>
#include <U2Algorithm/RepeatFinderSettings.h>
#include <U2Core/Task.h>

#include <U2Core/DNASequence.h>

namespace U2 {

class RevComplSequenceTask;

class RFTaskFactory: public RepeatFinderTaskFactory {
public:
    RFTaskFactory() {};

    virtual Task* getTaskInstance(const RepeatFinderSettings& config) const;
    virtual void setRFResultsListener(Task*, RFResultsListener*);
};

/** Reverses X sequence and runs repeat finder task */
class ReverseAndCreateTask : public Task {
    Q_OBJECT
public:
    ReverseAndCreateTask(const RepeatFinderSettings& c);
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    RepeatFinderSettings c;
    RevComplSequenceTask *revTask;

};

} // namespace

#endif
