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

#ifndef _U2_GENOME_ALIGNER_TASK_H_
#define _U2_GENOME_ALIGNER_TASK_H_

#include <QSharedPointer>
#include <QTemporaryFile>
#include <QMutex>

#include <U2Algorithm/DnaAssemblyTask.h>
#include <U2Formats/StreamSequenceReader.h>
#include <U2Formats/StreamSequenceWriter.h>
#include "GenomeAlignerFindTask.h"
#include "GenomeAlignerIO.h"
#include "GenomeAlignerIndexPart.h"
#include "GenomeAlignerWriteTask.h"

namespace U2 {

class U2SequenceObject;
class DNATranslation;
class LoadDocumentTask;
class GenomeAlignerIndexTask;
class GenomeAlignerIndex;
class ReadShortReadsSubTask;
class WriteAlignedReadsSubTask;
class DbiConnection;

class GenomeAlignerTask : public DnaAssemblyToReferenceTask {
    Q_OBJECT
    friend class ReadShortReadsSubTask;
public:
    GenomeAlignerTask(const DnaAssemblyToRefTaskSettings& settings, bool justBuildIndex = false);
    ~GenomeAlignerTask();
    virtual void prepare();
    virtual ReportResult report();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    QString getIndexPath();
    static const QString OPTION_ALIGN_REVERSED;
    static const QString OPTION_OPENCL;
    static const QString OPTION_IF_ABS_MISMATCHES;
    static const QString OPTION_MISMATCHES;
    static const QString OPTION_PERCENTAGE_MISMATCHES;
    static const QString OPTION_INDEX_DIR;
    static const QString OPTION_QUAL_THRESHOLD;
    static const QString OPTION_BEST;
    static const QString OPTION_DBI_IO;
    static const QString OPTION_READS_MEMORY_SIZE;
    static const QString OPTION_SEQ_PART_SIZE;
    static const int MIN_SHORT_READ_LENGTH = 30;
    static int calculateWindowSize(bool absMismatches, int nMismatches, int ptMismatches, int minReadLength, int maxReadLength);

    DNA_ASSEMBLEY_TO_REF_TASK_FACTORY(GenomeAlignerTask)
private:
    LoadDocumentTask *loadDbiTask;
    GenomeAlignerIndexTask *createIndexTask;
    ReadShortReadsSubTask *readTask;
    GenomeAlignerFindTask *findTask;
    WriteAlignedReadsSubTask *writeTask;
    GenomeAlignerWriteTask *pWriteTask;
    Task* unzipTask;

    GenomeAlignerReader *seqReader;
    GenomeAlignerWriter *seqWriter;
    AlignContext alignContext;

    QTemporaryFile temp;

    bool justBuildIndex;
    uint bunchSize;

    bool alignReversed;
    bool dbiIO;
    QString indexFileName;
    bool prebuiltIndex;
    GenomeAlignerIndex *index;
    int qualityThreshold;
    quint64 readMemSize;
    int seqPartSize;
    SearchQuery *lastQuery;
    bool noDataToAlign;

    //statistics
    quint64 readsCount;
    quint64 readsAligned;
    qint64 shortreadLoadTime;
    qint64 resultWriteTime;
    qint64 searchTime;
    qint64 indexLoadTime;
    qint64 shortreadIOTime;
    float currentProgress;

    QMutex writeLock;

    void setupCreateIndexTask();
    void createGenomeAlignerWriteTask();
};

} //namespace

#endif // _U2_GENOME_ALIGNER_TASK_H_
