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

#include "CodonOccurTask.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

namespace U2 {

CodonOccurTask::CodonOccurTask(DNATranslation *complementTranslation, DNATranslation *aminoTranslation, const U2EntityRef &seqRef, const QVector<U2Region> &regions)
    : BackgroundTask<QList<CharOccurResult>>(tr("Count codons"), TaskFlag_NoRun) {
    SequenceDbiWalkerConfig config;
    config.seqRef = seqRef;
    config.complTrans = complementTranslation;
    config.strandToWalk = StrandOption_Both;
    config.aminoTrans = aminoTranslation;
    config.chunkSize = 10 * 1000 * 1000;    // Use maximum 6*10mb RAM.
    // Run only 1 subtask at a time: the code in onRegion() is not thread-safe: updates global QHash state.
    config.nThreads = 1;
    for (const U2Region region : regions) {
        config.range = region;
        addSubTask(new SequenceDbiWalkerTask(config, this, tr("Count codons per region")));
    }
}

void CodonOccurTask::onRegion(SequenceDbiWalkerSubtask *task, TaskStateInfo &) {
    const QByteArray &sequence = task->getRegionSequence();
    for (char codon : qAsConst(sequence)) {
        countPerCodon[codon] = countPerCodon.value(codon, 0) + 1;
    }
}

Task::ReportResult CodonOccurTask::report() {
    qint64 totalCodonCount = 0;
    for (auto count : qAsConst(countPerCodon)) {
        totalCodonCount += count;
    }
    CHECK(totalCodonCount > 0, ReportResult_Finished);

    for (auto it = countPerCodon.begin(); it != countPerCodon.end(); it++) {
        char codon = it.key();
        qint64 count = it.value();
        double percent = double(count) * 100 / double(totalCodonCount);
        result << CharOccurResult(codon, count, percent);
    }
    std::sort(result.begin(), result.end(), [](const CharOccurResult &c1, CharOccurResult &c2) {
        return c1.getNumberOfOccur() > c2.getNumberOfOccur();    // Reversed order: most frequent is first.
    });
    return ReportResult_Finished;
}

}    // namespace U2
