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

#ifndef _U2_GENOME_ALIGNER_FIND_TASK_H_
#define _U2_GENOME_ALIGNER_FIND_TASK_H_

#include <QMutex>
#include <QMutex>
#include <QSemaphore>
#include <QVector>

#include <U2Core/DNASequence.h>
#include <U2Core/Task.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include "GenomeAlignerSearchQuery.h"
#include "GenomeAlignerWriteTask.h"
#include "DataBunch.h"

#define BinarySearchResult qint64

namespace U2 {

class GenomeAlignerIndex;

#define CHECK_LOG(a, b) CHECK_EXT(a, algoLog.trace("Check failed " #a), b)

struct ShortReadData {
    ShortReadData(DataBunch* dataBunch, int i): valid(false) {
        CHECK_LOG(dataBunch,);
        int length = dataBunch->bitValuesV.size();
        CHECK_LOG(i>=0 && i<length,);

        currentW = dataBunch->windowSizes.at(i);
        CHECK_LOG(0 != currentW,);
        currentBitFilter = ((quint64)0 - 1) << (62 - currentW * 2);

        bv = dataBunch->bitValuesV.at(i);
        rn = dataBunch->readNumbersV.at(i);
        pos = dataBunch->positionsAtReadV.at(i);
        nextRn = i < length - 1 ? dataBunch->readNumbersV.at(i + 1) : 0;

        shortRead = dataBunch->queries.at(rn);
        CHECK_LOG(shortRead,);
        revCompl = shortRead->getRevCompl();

        valid = true;
    }

    bool haveExactResult() const {
        return ((0 == shortRead->firstMCount()) || (NULL != revCompl && 0 == revCompl->firstMCount()));
    }

    bool valid;

    int currentW;
    BMType currentBitFilter;
    BMType bv;
    int rn;
    int pos;
    int nextRn;
    SearchQuery *shortRead;
    SearchQuery *revCompl;
};


class AlignContext {
public:
    AlignContext(): w(-1), ptMismatches(0), nMismatches(0), absMismatches(0), bestMode(false),
        openCL(false), minReadLength(-1), maxReadLength(-1), isReadingFinished(false), isReadingStarted(false),
        needIndex(true), indexLoaded(-1) {}
    ~AlignContext() {
        cleanVectors();
    }
    int w;
    int ptMismatches;
    int nMismatches;
    bool absMismatches;
    bool bestMode;
    bool openCL;
    int minReadLength;
    int maxReadLength;

    QList<DataBunch*> data;

    bool isReadingFinished;
    bool isReadingStarted;
    bool needIndex;
    int indexLoaded;
    QMutex readingStatusMutex;
    QReadWriteLock listM, indexLock;
    QWaitCondition readShortReadsWait, loadIndexTaskWait, requireIndexWait;

    void cleanVectors() {
        qDeleteAll(data);
        data.clear();
    }
};

#define MAX_PERCENTAGE 100
class GenomeAlignerFindTask : public Task {
    Q_OBJECT
    friend class ShortReadAlignerCPU;
    friend class ShortReadAlignerOpenCL;
public:
    GenomeAlignerFindTask(GenomeAlignerIndex *i, AlignContext *s, GenomeAlignerWriteTask *writeTask);
    virtual void run();
    virtual void prepare();

    qint64 getIndexLoadTime() const {return indexLoadTime;}

protected:
    void requirePartForAligning(int part);
    DataBunch *waitForDataBunch();

private:
    GenomeAlignerIndex *index;
    GenomeAlignerWriteTask *writeTask;
    AlignContext *alignContext;

    int alignerTaskCount;
    int waiterCount;
    int nextElementToGive;
    qint64 indexLoadTime;

    QMutex loadPartMutex;
    QMutex waitDataForAligningMutex;
    QMutex waitMutex;
    QWaitCondition waiter;
};

class LoadIndexTask : public Task {
    Q_OBJECT
public:
    LoadIndexTask(GenomeAlignerIndex *_index, AlignContext *_alignContext)
        : Task("LoadIndexTask", TaskFlag_None), index(_index), alignContext(_alignContext), part(0) {}
    virtual void run();
private:
    GenomeAlignerIndex *index;
    AlignContext *alignContext;
    int part;
};


typedef QVector<SearchQuery*>::iterator QueryIter;

class ShortReadAlignerCPU : public Task {
    Q_OBJECT
public:
    ShortReadAlignerCPU(int taskNo, GenomeAlignerIndex *index, AlignContext *alignContext, GenomeAlignerWriteTask *writeTask);
    virtual void run();
private:
    int taskNo;
    GenomeAlignerIndex *index;
    AlignContext *alignContext;
    GenomeAlignerWriteTask *writeTask;
};

class ShortReadAlignerOpenCL : public Task {
    Q_OBJECT
public:
    ShortReadAlignerOpenCL(int taskNo, GenomeAlignerIndex *index, AlignContext *alignContext, GenomeAlignerWriteTask *writeTask);
    virtual void run();
private:
    int taskNo;
    GenomeAlignerIndex *index;
    AlignContext *alignContext;
    GenomeAlignerWriteTask *writeTask;
};

} //U2

#endif // _U2_GENOME_ALIGNER_FIND_TASK_H_
