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

#ifndef _U2_BLAST_READS_SUBTASK_H_
#define _U2_BLAST_READS_SUBTASK_H_

#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>


namespace U2 {

class AbstractAlignmentTaskFactory;
class BlastNPlusSupportTask;
class PairwiseAlignmentTaskSettings;

namespace Workflow {

/************************************************************************/
/* BlastReadsSubTask */
/************************************************************************/
class BlastAndSwReadTask;
class BlastReadsSubTask : public Task {
    Q_OBJECT
public:
    BlastReadsSubTask(const QString& dbPath,
                      const QList<SharedDbiDataHandler> &reads,
                      const SharedDbiDataHandler &reference,
                      const int minIdentityPercent,
                      const QMap<SharedDbiDataHandler, QString> &readsNames,
                      DbiDataStorage *storage);

    void prepare();

    const QList<BlastAndSwReadTask*>& getBlastSubtasks() const;

private:
    const QString dbPath;
    const QList<SharedDbiDataHandler> reads;
    const QMap<SharedDbiDataHandler, QString> readsNames;
    const SharedDbiDataHandler reference;
    const int minIdentityPercent;

    DbiDataStorage *storage;

    QList<BlastAndSwReadTask*> blastSubTasks;
};

/************************************************************************/
/* BlastAndSwReadTask */
/************************************************************************/
class BlastAndSwReadTask : public Task {
    Q_OBJECT
public:
    BlastAndSwReadTask(const QString& dbPath,
                       const SharedDbiDataHandler& read,
                       const SharedDbiDataHandler &reference,
                       const int minIdentityPercent,
                       const QString &readName,
                       DbiDataStorage *storage);

    bool isComplement() const;
    const SharedDbiDataHandler& getRead() const;
    const U2MsaRowGapModel&      getReferenceGaps() const;
    const U2MsaRowGapModel&      getReadGaps() const;

    bool        isReadAligned() const;
    QString     getReadName() const;
    MultipleSequenceAlignment getMAlignment();
    qint64      getOffset() const;
    int         getReadIdentity() const;

private:
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    ReportResult report();

    BlastNPlusSupportTask *getBlastTask();
    void checkRead(const QByteArray &sequenceData);

    U2Region getReferenceRegion(const QList<SharedAnnotationData>& blastAnnotations);
    void createAlignment(const U2Region& refRegion);
    void shiftGaps(U2MsaRowGapModel &gaps) const;

    static AbstractAlignmentTaskFactory* getAbstractAlignmentTaskFactory(const QString &algoId, const QString &implId, U2OpStatus &os);
    static PairwiseAlignmentTaskSettings* createSettings(DbiDataStorage *storage, const SharedDbiDataHandler &msa, U2OpStatus &os);

    const QString dbPath;
    const SharedDbiDataHandler read;
    const SharedDbiDataHandler reference;
    const int minIdentityPercent;
    qint64 referenceLength;
    int readIdentity;

    SharedDbiDataHandler msa;
    qint64 offset;
    qint64 readShift;

    DbiDataStorage *storage;

    BlastNPlusSupportTask *blastTask;
    QString blastResultDir;

    U2MsaRowGapModel referenceGaps;
    U2MsaRowGapModel readGaps;
    QString readName;
    bool complement;
    bool skipped;
};

} // namespace Workflow
} // namespace U2

#endif // _U2_BLAST_READS_SUBTASK_H_
