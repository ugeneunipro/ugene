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

#ifndef _U2_PAIRWISE_ALIGNMENT_TASK_H_
#define _U2_PAIRWISE_ALIGNMENT_TASK_H_

#include "../msa_alignment/AbstractAlignmentTask.h"
#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

namespace U2 {

class PairwiseAlignmentSettings;
class PairwiseAlignmentTask;

class U2ALGORITHM_EXPORT PairwiseAlignmentTaskSettings : public AbstractAlignmentTaskSettings {
public:
    PairwiseAlignmentTaskSettings();
    PairwiseAlignmentTaskSettings(const QVariantMap& someSettings);
    PairwiseAlignmentTaskSettings(const PairwiseAlignmentTaskSettings &s);

    virtual bool isValid() const;

    U2EntityRef firstSequenceRef;           //couldn't be in customSettings. Set manually.
    U2EntityRef secondSequenceRef;          //couldn't be in customSettings. Set manually.

    static const QString PA_FIRST_SEQUENCE_REF;
    static const QString PA_SECOND_SEQUENCE_REF;
    static const QString DEFAULT_NAME;
};

class U2ALGORITHM_EXPORT PairwiseAlignmentTask : public AbstractAlignmentTask {
    Q_OBJECT

public:
    PairwiseAlignmentTask(TaskFlags flags = TaskFlags_FOSCOE);

protected:
    QByteArray first;
    QByteArray second;
};
}   //namespace

#endif // _U2_PAIRWISE_ALIGNMENT_TASK_H_
