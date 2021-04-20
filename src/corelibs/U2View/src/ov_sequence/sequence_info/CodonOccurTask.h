/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

#include "CharOccurTask.h"

namespace U2 {

class DNATranslation;

/** Computes map of codon counts for the given sequence in the current set of regions. */
class U2VIEW_EXPORT CodonOccurTask : public BackgroundTask<QList<CharOccurResult>>, public SequenceDbiWalkerCallback {
public:
    CodonOccurTask(DNATranslation *complementTranslation, DNATranslation *aminoTranslation, const U2EntityRef &seqRef, const QVector<U2Region> &regions);

    /** Processes the given sequence region. A callback used by SequenceWalker subtask. */
    void onRegion(SequenceDbiWalkerSubtask *task, TaskStateInfo &ti) override;
    ReportResult report() override;

private:
    QHash<char, qint64> countPerCodon;
};

}    // namespace U2

#endif
