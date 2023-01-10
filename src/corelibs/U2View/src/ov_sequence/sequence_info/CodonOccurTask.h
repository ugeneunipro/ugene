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

#ifndef _U2_CODON_COUNT_TASK_H_
#define _U2_CODON_COUNT_TASK_H_

#include <QHash>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/SequenceDbiWalkerTask.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

#include "CharOccurTask.h"

namespace U2 {

class DNATranslation;
class Annotation;

/** Computes map of codon counts for the given sequence in the current set of regions. */
class U2VIEW_EXPORT CodonOccurTask : public BackgroundTask<QMap<QByteArray, qint64>>, public SequenceDbiWalkerCallback {
public:
    /** Create a task to count codons in all 6 frames (3 direct and 3 complement) in the whole sequence. */
    CodonOccurTask(DNATranslation* complementTranslation,
                   const U2EntityRef& seqRef);

    /**
     * Creates a task to count codons in 2 frames (1 direct and 1 complement) in the given regions.
     * Every regions in this case is a part of a 'sequence selection'.
     */
    CodonOccurTask(DNATranslation* complementTranslation,
                   const U2EntityRef& seqRef,
                   const QVector<U2Region>& regions);

    /** Creates a task to count codons in 1 frame guided by the annotation strand. */
    CodonOccurTask(DNATranslation* complementTranslation,
                   const U2EntityRef& seqRef,
                   const QList<Annotation*>& annotations);

    /** Processes the given sequence region. A callback used by SequenceWalker subtask. */
    void onRegion(SequenceDbiWalkerSubtask* task, TaskStateInfo& ti) override;
    ReportResult report() override;

private:
    QMap<QByteArray, qint64> countPerCodon;
};

}  // namespace U2

#endif
