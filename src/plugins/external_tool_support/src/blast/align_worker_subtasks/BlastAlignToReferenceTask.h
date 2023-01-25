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

#ifndef _U2_BLAST_ALIGN_TO_REFERENCE_TASK_H_
#define _U2_BLAST_ALIGN_TO_REFERENCE_TASK_H_

#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>

namespace U2 {

class AbstractAlignmentTaskFactory;
class AbstractAlignmentTask;
class BlastNTask;
class PairwiseAlignmentTaskSettings;

namespace Workflow {

class BlastAlignToReferenceTask;

/** Result of the 'BlastAlignToReferenceTask': a pairwise read & reference alignment. */
struct AlignToReferenceResult {
    /** Read's DBI handle. */
    SharedDbiDataHandler readHandle;

    /** Name of the read in the result alignment. May be different from the original read name. */
    QString readName;

    /** Gaps in the read. */
    QVector<U2MsaGap> readGaps;

    /** Gaps in the reference sequence. */
    QVector<U2MsaGap> referenceGaps;

    /** If the read is on complementary strand. */
    bool isOnComplementaryStrand = false;

    /** Location of the blast hit in reference sequence. */
    U2Region blastReferenceRegion;

    /** Location of the blast hit in read. */
    U2Region blastReadRegion;

    /** Blase 'identities' value. Count of bases. */
    int blastIdentity;

    /** Reference region processed by the pairwise alignment algorithm. */
    U2Region pairwiseAlignmentReferenceRegion;

    /** Extra leading gaps to add to the read for pairwise alignment. */
    int pairwiseAlignmentLeadingReadGap = 0;

    /** Percent identity of the match. */
    int identityPercent = 0;
};

/**
 * A multiplexer of BlastAlignToReferenceTasks.
 * Splits input reads into buckets and runs "BlastAlignToReferenceTask" per each bucket.
 * Each bucket is used for a single 'blastn' tool invocation.
 */
class BlastAlignToReferenceMuxTask : public Task {
    Q_OBJECT
public:
    BlastAlignToReferenceMuxTask(const QString& blastDbPath,
                                 const QList<SharedDbiDataHandler>& reads,
                                 const SharedDbiDataHandler& reference,
                                 const QMap<SharedDbiDataHandler, QString>& readRenameMap,
                                 DbiDataStorage* storage);

    void prepare() override;

    QList<Task*> onSubTaskFinished(Task* task) override;

    ReportResult report() override;

    /** Returns list of pairwise alignments of reads to reference. */
    const QList<AlignToReferenceResult>& getAlignmentResults() const;

private:
    /** Creates new subtask to process the specified range of reads. */
    BlastAlignToReferenceTask* createNewSubtask(const U2Region& readsRange) const;

    const QString blastDbPath;
    const QList<SharedDbiDataHandler> reads;
    const SharedDbiDataHandler reference;
    const QMap<SharedDbiDataHandler, QString> readRenameMap;

    DbiDataStorage* const storage;

    /** List of pending read regions to run blast tasks. */
    QList<U2Region> readsRangePerSubtask;

    QList<AlignToReferenceResult> alignmentResults;
};

/**
 * Run a single BLAST tool instance for a list of reads
 * and next re-aligns each read with Smith Waterman algorithm
 * using global position in the reference found by BLAST.
 *
 * Produces a result as a set of individual pair-wise alignments per each read & reference.
 */
class BlastAlignToReferenceTask : public Task {
    Q_OBJECT
public:
    BlastAlignToReferenceTask(const QString& blastDbPath,
                              const QList<SharedDbiDataHandler>& reads,
                              const SharedDbiDataHandler& reference,
                              const QMap<SharedDbiDataHandler, QString>& readRenameMap,
                              DbiDataStorage* storage,
                              const QString& taskNameSuffix = "");

    /** Returns list of pairwise alignments of reads to reference. */
    const QList<AlignToReferenceResult>& getAlignmentResults() const;

private:
    void prepare() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

    /**
     * Creates MSA object with 2 sequences: read & region of the reference sequence.
     * The MSA is created in the "storage" and must be cleaned up by the caller.
     */
    static MultipleSequenceAlignmentObject* createPairwiseAlignment(U2OpStatus& os,
                                                                    const U2DbiRef& dbiRef,
                                                                    const DNASequence& referenceSequence,
                                                                    const DNASequence& readSequence,
                                                                    const DNAAlphabet* alphabet,
                                                                    const AlignToReferenceResult& alignmentResult);

    static AbstractAlignmentTaskFactory* getAbstractAlignmentTaskFactory(const QString& algoId, const QString& implId, U2OpStatus& os);

    /** Fills related fields in 'alignResult' using 'blastResult' info. */
    static void convertBlastResultToAlignmentResult(const SharedAnnotationData& blastResult, AlignToReferenceResult& alignResult);

    /** Picks reference region for a slow pairwise alignment based on the blast result reference/read match regions. */
    static void assignReferencePairwiseAlignmentRegion(AlignToReferenceResult& alignResult, int readLength, int referenceLength);

    const QString dbPath;
    const QList<SharedDbiDataHandler> reads;
    const SharedDbiDataHandler reference;
    const QMap<SharedDbiDataHandler, QString> readRenameMap;

    DbiDataStorage* const storage;

    /** Final and complete alignment results with identityPercent >= minIdentityPercent. */
    QList<AlignToReferenceResult> alignmentResults;

    /** Map of alignment result by read sequence data id. */
    QMap<U2DataId, AlignToReferenceResult*> alignmentResultByRead;
    QMap<U2DataId, U2EntityRef> pairwiseMsaByRead;

    /** Keeps mapping of read index -> blast query sequence index. Used because not all reads are passed to BLAST, but only validated ones. */
    QVector<int> blastQuerySequenceIndexByReadIndex;
};

}  // namespace Workflow
}  // namespace U2

#endif  // _U2_BLAST_ALIGN_TO_REFERENCE_TASK_H_
