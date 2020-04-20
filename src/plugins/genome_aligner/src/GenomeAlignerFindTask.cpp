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

#include <U2Algorithm/BitsTable.h>
#include <U2Algorithm/SyncSort.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GAutoDeleteList.h>

#include "GenomeAlignerIndex.h"
#include "GenomeAlignerTask.h"

#include <time.h>

#include "GenomeAlignerFindTask.h"

namespace U2 {

GenomeAlignerFindTask::GenomeAlignerFindTask(U2::GenomeAlignerIndex *i, AlignContext *s, GenomeAlignerWriteTask *w)
: Task("GenomeAlignerFindTask", TaskFlag_None),
index(i), writeTask(w), alignContext(s)
{
    nextElementToGive = 0;
    indexLoadTime = 0;
    waiterCount = 0;
    alignerTaskCount = 0;
}

void GenomeAlignerFindTask::prepare() {
    alignerTaskCount = alignContext->openCL ? 1 : AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
    if (alignContext->openCL) {
        Task *subTask = new ShortReadAlignerOpenCL(0, index, alignContext, writeTask);
        subTask->setSubtaskProgressWeight(1.0f);
        addSubTask(subTask);
    } else {
        setMaxParallelSubtasks(alignerTaskCount);
        for (int i = 0; i < alignerTaskCount; i++) {
            Task *subTask = new ShortReadAlignerCPU(i, index, alignContext, writeTask);
            subTask->setSubtaskProgressWeight(1.0f / alignerTaskCount);
            addSubTask(subTask);
        }
    }
}

#define GA_WAIT(condition, mutex) \
    condition.wait(&mutex);


void GenomeAlignerFindTask::run() {

    // TODO: this is a fastfix of reopened https://ugene.net/tracker/browse/UGENE-1190
    // Problem:
    // If reference sequence contains Ns only, ShortReadAligners will return without waiting for all short reads.
    // GenomeAlignerTask will create another ReadShortReadsSubTask on GenomeAlignerFindTask->WriteAlignedReadsSubTask subtask finish

    // Wait while ReadShortReadsSubTask finished reading.
    while (true) {
        if (isCanceled()) {
            break;
        }
        QMutexLocker(&(alignContext->readingStatusMutex));
        bool isReadingStarted = alignContext->isReadingStarted;
        bool isReadingFinished = alignContext->isReadingFinished;
        if (isReadingStarted && isReadingFinished) {
            break;
        }
        alignContext->readShortReadsWait.wait(&(alignContext->readingStatusMutex));
    }

    QReadLocker locker(&alignContext->indexLock);
    alignContext->needIndex = false;
    alignContext->loadIndexTaskWait.wakeOne();
}

void LoadIndexTask::run() {
    QWriteLocker locker(&alignContext->indexLock);
    while (true) {
        CHECK(!isCanceled(), );
        if (!alignContext->needIndex) {
            alignContext->loadIndexTaskWait.wait(&alignContext->indexLock);
        }
        CHECK(alignContext->needIndex, );

        taskLog.trace(QString("Going to load index part %1").arg(part + 1));
        if (!index->loadPart(part)) {
            setError("Incorrect index file. Please, try to create a new index file.");
            return;
        }
        taskLog.trace(QString("finished loading index part %1").arg(part + 1));

        alignContext->needIndex = false;
        alignContext->indexLoaded = part;
        part = part >= index->getPartCount()-1 ? 0 : part+1;
        alignContext->requireIndexWait.wakeAll();
    }
}

void GenomeAlignerFindTask::requirePartForAligning(int part) {
    {
        QMutexLocker locker(&waitMutex);
        waiterCount++;
        if (waiterCount != alignerTaskCount) {
            waiter.wait(&waitMutex);
        } else {
            waiterCount = 0;
        }
        waiter.wakeOne();
    }

    QMutexLocker lock(&loadPartMutex);
    QReadLocker locker(&alignContext->indexLock);
    if (alignContext->indexLoaded == part) {
        return;
    }

    alignContext->needIndex = true;
    alignContext->loadIndexTaskWait.wakeOne();
    alignContext->requireIndexWait.wait(&alignContext->indexLock);

    nextElementToGive = 0;
}

DataBunch* GenomeAlignerFindTask::waitForDataBunch() {
    QMutexLocker lock(&waitDataForAligningMutex);

    int lastDataBunchesIndex = -1;
    bool needToWait = false;
    do {
        QMutexLocker readingLock(&alignContext->readingStatusMutex);
        bool isReadingFinished = alignContext->isReadingFinished;
        if (isReadingFinished) {
            break;
        }
        alignContext->readShortReadsWait.wait(&alignContext->readingStatusMutex);

        // ReadShortReadsSubTask can add new data
        alignContext->listM.lockForRead();
        lastDataBunchesIndex = alignContext->data.size()-1;
        alignContext->listM.unlock();

        needToWait = nextElementToGive > lastDataBunchesIndex;
    }
    while (needToWait);

    alignContext->listM.lockForRead();
    lastDataBunchesIndex = alignContext->data.size()-1;
    alignContext->listM.unlock();

    if (nextElementToGive > lastDataBunchesIndex) {
        return NULL;
    } else {
        DataBunch* dataBunch = alignContext->data.at(nextElementToGive++);
        return dataBunch;
    }
}

#define GA_CHECK_BREAK(a) {if (!(a)) {algoLog.trace("Break because of ![" #a "]"); break;}}
#define GA_CHECK_CONTINUE(a) {if (!(a)) {algoLog.trace("Continue because of ![" #a "]"); continue;}}

ShortReadAlignerCPU::ShortReadAlignerCPU(int taskNo, GenomeAlignerIndex *i, AlignContext *s, GenomeAlignerWriteTask *w)
: Task("ShortReadAlignerCPU", TaskFlag_None), taskNo(taskNo), index(i), alignContext(s), writeTask(w)
{
}

void ShortReadAlignerCPU::run() {
    SAFE_POINT_EXT (NULL != alignContext, setError("Align context error"),);
    assert(!alignContext->openCL);

    GenomeAlignerFindTask *parent = static_cast<GenomeAlignerFindTask*>(getParentTask());
    SAFE_POINT_EXT(NULL != parent, setError("Aligner parent error"),);

    QVector<int> binarySearchResults;

    SAFE_POINT_EXT (NULL != index, setError("Aligner index error"),);
    for (int part = 0; part < index->getPartCount(); part++) {
        if (isCanceled()) {
            break;
        }
        stateInfo.setProgress(100*part/index->getPartCount());
        parent->requirePartForAligning(part);
        if (parent->hasError()) {
            break;
        }

        stateInfo.setProgress(stateInfo.getProgress() + 25/index->getPartCount());
        if (0 == index->getLoadedPart().getLoadedPartSize()) {
            algoLog.trace(tr("[%1] Index size for part %2/%3 is zero, skipping it.").arg(taskNo).arg(part + 1).arg(index->getPartCount()));
            continue;
        }

        do {
            if (isCanceled()) {
                break;
            }
            DataBunch *dataBunch = parent->waitForDataBunch();
            GA_CHECK_BREAK(dataBunch);
            CHECK_OP(stateInfo, );
            algoLog.trace(QString("[%1] Got for aligning").arg(taskNo));

            quint64 t0=0, fullStart = GTimer::currentTimeMicros();

            int length = dataBunch->bitValuesV.size();
            GA_CHECK_BREAK(length);

            dataBunch->prepareSorted();
            int binaryFound = 0;
            binarySearchResults.resize(length);
            t0 = GTimer::currentTimeMicros();
            for (int i = 0; i < length; i++) {
                int currentW = dataBunch->windowSizes.at(dataBunch->sortedIndexes[i]);
                CHECK_LOG(0 != currentW,);
                BMType currentBitFilter = ((quint64)0 - 1) << (62 - currentW * 2);
                BMType bv = dataBunch->sortedBitValuesV.at(i);

                binarySearchResults[dataBunch->sortedIndexes[i]] = index->bitMaskBinarySearch(bv, currentBitFilter);
                binaryFound += binarySearchResults[dataBunch->sortedIndexes[i]] == -1 ? 0 : 1;
            }
            algoLog.trace(QString("[%1] Binary search %2 results, found %3 in %4 ms.").arg(taskNo).arg(length).arg(binaryFound).arg((GTimer::currentTimeMicros() - t0) / double(1000), 0, 'f', 3));

            t0 = GTimer::currentTimeMicros();
            int skipped = 0;
            for (int i = 0; i < length; i++) {
                ShortReadData srData(dataBunch, i);
                GA_CHECK_CONTINUE(srData.valid);
                if (alignContext->bestMode && srData.haveExactResult()) {
                    skipped++;
                    continue;
                }

                BinarySearchResult bmr = binarySearchResults[i];
                index->alignShortRead(srData.shortRead, srData.bv, srData.pos, bmr, alignContext, srData.currentBitFilter, srData.currentW);

                if (!alignContext->bestMode) {
                    if ((i == length - 1) || (srData.nextRn != srData.rn)) {
                        if (srData.shortRead->haveResult()) {
                            writeTask->addResult(srData.shortRead);
                        }
                        srData.shortRead->onPartChanged();
                    }
                }
            }
            algoLog.trace(QString("[%1] Aligning took %2 ms").arg(taskNo).arg((GTimer::currentTimeMicros() - t0) / double(1000), 0, 'f', 3));
            algoLog.trace(QString("[%1] Skipped: %2, tried to align %3").arg(taskNo).arg(skipped).arg(length - skipped));
            float msec = (GTimer::currentTimeMicros() - fullStart);
            algoLog.trace(QString("[%1] Searching (%2) and aligning (%3) took %4 ms").arg(taskNo).arg(length).arg(binaryFound).arg(msec / double(1000), 0, 'f', 3));
        } while(true);
    }
    if (isCanceled() || hasError()) {
        QReadLocker locker(&alignContext->indexLock);
        alignContext->loadIndexTaskWait.wakeAll();
    }
}


ShortReadAlignerOpenCL::ShortReadAlignerOpenCL(int taskNo, GenomeAlignerIndex *i, AlignContext *s, GenomeAlignerWriteTask *w)
: Task("ShortReadAlignerOpenCL", TaskFlag_None), taskNo(taskNo), index(i), alignContext(s), writeTask(w)
{
}

void ShortReadAlignerOpenCL::run() {
#ifdef OPENCL_SUPPORT
    SAFE_POINT_EXT (NULL != alignContext, setError("Align context error"),);
    assert(alignContext->openCL);

    GenomeAlignerFindTask *parent = static_cast<GenomeAlignerFindTask*>(getParentTask());
    SAFE_POINT_EXT(NULL != parent, setError("Aligner parent error"),);

    SAFE_POINT_EXT (NULL != index, setError("Aligner index error"),);
    for (int part = 0; part < index->getPartCount(); part++) {
        if (isCanceled()) {
            break;
        }
        quint64 t0 = GTimer::currentTimeMicros();

        stateInfo.setProgress(100 * part / index->getPartCount());
        parent->requirePartForAligning(part);
        if (parent->hasError()) {
            break;
        }

        algoLog.trace(QString("Index part %1 loaded in %2 sec.").arg(part + 1).arg((GTimer::currentTimeMicros() - t0) / double(1000000), 0, 'f', 3));

        stateInfo.setProgress(stateInfo.getProgress() + 25 / index->getPartCount());
        if (0 == index->getLoadedPart().getLoadedPartSize()) {
            algoLog.trace(tr("Index size for part %1/%2 is zero, skipping it.").arg(part + 1).arg(index->getPartCount()));
            continue;
        }

        do {
            if (isCanceled()) {
                break;
            }
            DataBunch *dataBunch = parent->waitForDataBunch();
            GA_CHECK_BREAK(dataBunch);
            algoLog.trace(QString("[%1] Got for aligning").arg(taskNo));

            int length = dataBunch->bitValuesV.size();
            GA_CHECK_BREAK(length);

            BinarySearchResult* binarySearchResults = index->bitMaskBinarySearchOpenCL(dataBunch->bitValuesV.constData(), dataBunch->bitValuesV.size(),
                dataBunch->windowSizes.constData());
            SAFE_POINT_EXT (NULL != binarySearchResults, {alignContext->listM.unlock(); setError("OpenCL binary find error");},);

            stateInfo.setProgress(stateInfo.getProgress() + 50/index->getPartCount());
            t0 = GTimer::currentTimeMicros();

            int skipped = 0;
            for (int i = 0; i < length; i++) {
                ShortReadData srData(dataBunch, i);
                GA_CHECK_CONTINUE(srData.valid);
                if (alignContext->bestMode && srData.haveExactResult()) {
                    skipped++;
                    continue;
                }

                SAFE_POINT_EXT (NULL != binarySearchResults, setError("OpenCL binary find error"),);
                BinarySearchResult bmr = binarySearchResults[i];
                index->alignShortRead(srData.shortRead, srData.bv, srData.pos, bmr, alignContext, srData.currentBitFilter, srData.currentW);

                if (!alignContext->bestMode) {
                    if ((i == length - 1) || (srData.nextRn != srData.rn)) {
                        if (srData.shortRead->haveResult()) {
                            writeTask->addResult(srData.shortRead);
                        }
                        srData.shortRead->onPartChanged();
                    }
                }
            }
            algoLog.trace(QString("[%1] Skipped: %2, tried to align %3 in %4 ms").arg(taskNo).arg(skipped).arg(length - skipped).arg((GTimer::currentTimeMicros() - t0) / double(1000), 0, 'f', 3));
            delete[] binarySearchResults; binarySearchResults = NULL;
        } while(true);
    }
    if (isCanceled() || hasError()) {
        QReadLocker locker(&alignContext->indexLock);
        alignContext->loadIndexTaskWait.wakeAll();
    }

#endif
}

} // U2
