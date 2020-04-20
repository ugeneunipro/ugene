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

#ifndef _U2_SIMPLE_ADDING_TO_ALIGNMENT_H_
#define _U2_SIMPLE_ADDING_TO_ALIGNMENT_H_

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MSAUtils.h>
#include <U2Algorithm/AlignSequencesToAlignmentTaskSettings.h>
#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

namespace U2 {

class SimpleAddToAlignmentTask : public AbstractAlignmentTask{
    Q_OBJECT
public:
    SimpleAddToAlignmentTask(const AlignSequencesToAlignmentTaskSettings& settings);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    ReportResult report();
private:
    AlignSequencesToAlignmentTaskSettings settings;
    QMap<QString, int>            sequencePositions;

    MultipleSequenceAlignment                  inputMsa;
};

class BestPositionFindTask : public Task{
    Q_OBJECT
public:
    BestPositionFindTask(const MultipleSequenceAlignment& alignment, const U2EntityRef& sequenceRef, const QString& sequenceId, int referenceRowId);
    void run();

    int getPosition() const;
    const QString& getSequenceId() const;
private:
    const MultipleSequenceAlignment& inputMsa;
    U2EntityRef sequenceRef;
    QString sequenceId;
    int bestPosition;
    int referenceRowId;
};

class SimpleAddToAlignmentTaskFactory : public AbstractAlignmentTaskFactory
{
public:
    virtual AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings *_settings) const;
};

class SimpleAddToAlignmentAlgorithm : public AlignmentAlgorithm {
public:
    SimpleAddToAlignmentAlgorithm();
};


} // U2

#endif // _U2_PAIRWISE_ALIGNER_H_
