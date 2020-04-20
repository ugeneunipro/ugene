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

#ifndef _U2_FIND_PATTERN_MSA_TASK_H_
#define _U2_FIND_PATTERN_MSA_TASK_H_

#include <U2Core/U2Region.h>

#include <U2View/FindPatternTask.h>

namespace U2 {

class MultipleSequenceAlignmentObject;
class FindPatternListTask;

struct U2VIEW_EXPORT FindPatternMsaSettings {
    FindPatternMsaSettings();

    MultipleSequenceAlignmentObject* msaObj;
    QList<NamePattern> patterns;
    bool removeOverlaps;
    int matchValue;
    FindAlgorithmSettings findSettings;
};

class U2VIEW_EXPORT FindPatternMsaTask : public Task {
    Q_OBJECT
public:
    FindPatternMsaTask(const FindPatternMsaSettings& settings);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    const QMap<int, QList<U2Region> >& getResults() const;

private:
    void getResultFromTask();
    void createSearchTaskForCurrentSequence();

    FindPatternMsaSettings settings;
    int currentSequenceIndex;
    FindPatternListTask* searchInSingleSequenceTask;
    int totalResultsCounter;

    QMap<int, QList<U2Region> > resultsBySeqIndex;
};

}

#endif
