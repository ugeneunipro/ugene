/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_BLAST_AND_SMITH_WATERMAN_ALIGNMENT_TASK_H_
#define _U2_BLAST_AND_SMITH_WATERMAN_ALIGNMENT_TASK_H_

#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>

namespace U2 {

class AbstractAlignmentTaskFactory;
class AbstractAlignmentTask;
class BlastNTask;
class PairwiseAlignmentTaskSettings;

namespace Workflow {

struct BlastAndSmithWatermanAlignmentResult {
    SharedDbiDataHandler readHandle;
    QVector<U2MsaGap> readGaps;
    QVector<U2MsaGap> referenceGaps;
    bool isOnComplementaryStrand = false;
    bool isSkipped = false;

    int readIdentity;
    qint64 offset;
    qint64 readShift;
};

/**
 * Aligns reads with BLAST and Smith Waterman. First searches for a local position of the read sub-sequence
 * with BLAST and next runs a precise Smith-Waterman on that position to align the whole read's sequence.
 */
class BlastAndSmithWatermanAlignmentTask : public Task {
    Q_OBJECT
public:
    BlastAndSmithWatermanAlignmentTask(const QString &dbPath,
                                       const QList<SharedDbiDataHandler> &reads,
                                       const SharedDbiDataHandler &reference,
                                       const int minIdentityPercent,
                                       DbiDataStorage *storage);

    void prepare() override;

    QList<Task *> onSubTaskFinished(Task *task) override;

    ReportResult report() override;

    const QList<BlastAndSmithWatermanAlignmentResult *> &getAlignmentResults() const;

private:
    const QString dbPath;
    const QList<SharedDbiDataHandler> reads;
    const SharedDbiDataHandler reference;
    const int minIdentityPercent;

    DbiDataStorage *const storage;

    /** List of pending read regions to run blast tasks. */
    QList<U2Region> readsRangePerSubtask;

    QList<BlastAndSmithWatermanAlignmentResult *> alignmentResults;
};

/**
 * Run a single BLAST tool instance for a list of reads
 * and next re-aligns each read with Smith Waterman algorithm
 * using global position in the reference found by BLAST.
 */
class BlastAndSmithWatermanAlignmentSubtask : public Task {
    Q_OBJECT
public:
    BlastAndSmithWatermanAlignmentSubtask(const QString &dbPath,
                                          const QList<SharedDbiDataHandler> &reads,
                                          const SharedDbiDataHandler &reference,
                                          int minIdentityPercent,
                                          DbiDataStorage *storage);

    const QList<BlastAndSmithWatermanAlignmentResult *> &getAlignmentResults() const;

    bool isComplement() const;
    const SharedDbiDataHandler &getRead() const;
    const QVector<U2MsaGap> &getReferenceGaps() const;
    const QVector<U2MsaGap> &getReadGaps() const;

    bool isReadAligned() const;
    QString getReadName() const;
    MultipleSequenceAlignment getMAlignment();
    qint64 getOffset() const;
    int getReadIdentity() const;

private:
    void prepare() override;
    QList<Task *> onSubTaskFinished(Task *subTask) override;
    ReportResult report() override;

    BlastNTask *getBlastTask();
    void checkRead(const QByteArray &sequenceData);

    U2Region getReferenceRegion(const QList<SharedAnnotationData> &blastAnnotations);
    void createAlignment(const U2Region &refRegion);
    void shiftGaps(QVector<U2MsaGap> &gaps) const;

    static AbstractAlignmentTaskFactory *getAbstractAlignmentTaskFactory(const QString &algoId, const QString &implId, U2OpStatus &os);
    static PairwiseAlignmentTaskSettings *createSettings(DbiDataStorage *storage, const SharedDbiDataHandler &msa, U2OpStatus &os);

    const QString dbPath;
    const QList<SharedDbiDataHandler> reads;
    const SharedDbiDataHandler reference;
    const int minIdentityPercent;

    qint64 referenceLength = 0;

    SharedDbiDataHandler msa;

    DbiDataStorage *storage = nullptr;

    BlastNTask *blastTask = nullptr;
    AbstractAlignmentTask *alignTask = nullptr;
    QString blastResultDir;

    QList<BlastAndSmithWatermanAlignmentResult *> alignmentResults;
};

}  // namespace Workflow
}  // namespace U2

#endif  // _U2_BLAST_READS_SUBTASK_H_
