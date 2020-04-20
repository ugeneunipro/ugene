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

#ifndef RF_SARRAYTANDEMFINDER_H
#define RF_SARRAYTANDEMFINDER_H

#include <U2Core/AppResources.h>
#include <U2Core/Task.h>
#include <U2Core/GObjectReference.h>
#include <U2Algorithm/BitsTable.h>
#include <U2Algorithm/SArrayIndex.h>
#include <U2Core/SequenceWalkerTask.h>

#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>

#include <QMutex>
#include <QMap>

#include "RF_SuffixArray.h"
#include "RFBase.h"


namespace U2 {

    namespace TSConstants{
        enum TSAlgo{AlgoSuffix=0, AlgoSuffixBinary=1};
        enum TSPreset{PresetAll=0, PresetMicro=1, PresetMini=2, PresetBigPeriod=3, PresetCustom=4};
    }

class FindTandemsTaskSettings {
public:
    static const int DEFAULT_MIN_TANDEM_SIZE;
    static const int DEFAULT_MIN_REPEAT_COUNT;
    static const TSConstants::TSAlgo DEFAULT_ALGO = TSConstants::AlgoSuffixBinary;
public:
    FindTandemsTaskSettings() : minPeriod(1), maxPeriod(INT_MAX), minTandemSize(DEFAULT_MIN_TANDEM_SIZE), minRepeatCount(DEFAULT_MIN_REPEAT_COUNT),
        accuracy(0), maxResults(10*1000*100), reportSeqShift(0), showOverlappedTandems(false), algo(DEFAULT_ALGO), nThreads(MAX_PARALLEL_SUBTASKS_AUTO) {}

    int         minPeriod;
    int         maxPeriod;
    int         minTandemSize;
    int         minRepeatCount;
    int         accuracy;
    int         maxResults;
    qint64      reportSeqShift;
    U2Region    seqRegion;
    bool        showOverlappedTandems;

    TSConstants::TSAlgo    algo;
    int         nThreads;

};

class FindTandemsToAnnotationsTask : public Task {
    Q_OBJECT
public:
    FindTandemsToAnnotationsTask(const FindTandemsTaskSettings& s, const DNASequence& seq,
        const QString& annName, const QString& groupName, const QString &annDescription, const GObjectReference& annObjRef);
    FindTandemsToAnnotationsTask(const FindTandemsTaskSettings& s, const DNASequence& seq);

    QList<Task*> onSubTaskFinished(Task* subTask);
    QList<SharedAnnotationData> importTandemAnnotations(const QList<Tandem>& tandems, qint64 seqStart, const bool showOverlapped);

    QList<SharedAnnotationData> getResult() const {return result;}

private:
    bool                saveAnns;
    DNASequence         mainSeq;
    QString             annName;
    QString             annGroup;
    const QString       annDescription;
    GObjectReference    annObjRef;

    QList<SharedAnnotationData> result;
    const FindTandemsTaskSettings s;
};

class TandemFinder: public Task, public SequenceWalkerCallback {
    Q_OBJECT
public:
    TandemFinder(const FindTandemsTaskSettings& s, const DNASequence& seq);
    void prepare();
    void run();
    const QList<Tandem>& getResults(){return foundTandems;}
    const FindTandemsTaskSettings& getSettings() const{return settings;}
    const static quint32 maxCheckPeriod = 1024; //max period is 1k
protected:
    //main sequence
    char* sequence;
    FindTandemsTaskSettings settings;
    QMutex tandemsAccessMutex;
    QList<Tandem> foundTandems;
    QList<Task*> onSubTaskFinished(Task* subTask);
    void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti);

private:
    QMutex  subtasksQueue;
    quint32 regionCount;
    quint64 startTime;
    QList<Task*> regionTasks;
};

class TandemFinder_Region: public Task{
    Q_OBJECT
public:
    TandemFinder_Region(const int regionId, const char* _sequence, const quint32 _seqSize, const quint64 _regionOffset, const FindTandemsTaskSettings& _settings):
        Task(tr("Find tandems in %1 region").arg(regionId), TaskFlags_NR_FOSCOE),
        sequence(_sequence),seqSize(_seqSize),id(regionId),regionOffset(_regionOffset),settings(_settings)
    {}

    ~TandemFinder_Region();

    const QList<Tandem> getResult(){
        QMutexLocker tandemsAccessLocker(&tandemsAccessMutex);
        return foundTandems;
    };
    quint64 getRegionId()const{ return id;}
    quint64 getRegionOffset()const{ return regionOffset;}
    void prepare();
protected:
    //main sequence
    const char* sequence;
    const long seqSize;

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    const int id;
    const quint64 regionOffset;

    const FindTandemsTaskSettings& settings;
    QList<Tandem> foundTandems;
    QMutex tandemsAccessMutex;
    friend class ExactSizedTandemFinder;
    friend class LargeSizedTandemFinder;
    void addResult(const Tandem& tandem);
    void addResults(const QMap<Tandem,Tandem>& tandems);
};

class ConcreteTandemFinder: public Task{
    Q_OBJECT
public:
    ConcreteTandemFinder(QString taskName, const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _analysisSize);
    ~ConcreteTandemFinder(){};

    void prepare();
    void cleanup();

protected:
    //main sequence
    const char* sequence;
    const int seqSize;

    SArrayIndex* index;
    SuffixArray* suffixArray;
    const FindTandemsTaskSettings& settings;
    const int prefixLength;

    const quint32 suffArrSize;
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    const BitsTable bitsTable;
protected:
    QMap<Tandem,Tandem> rawTandems;

};

class ExactSizedTandemFinder: public ConcreteTandemFinder{
    Q_OBJECT
public:
    ExactSizedTandemFinder(const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _analysisSize);
    ~ExactSizedTandemFinder();

    void run();
private:
    quint32* checkAndSpreadTandem(const quint32* tandemStart, const quint32* tandemLast, quint32 repeatLen);
    quint32* checkAndSpreadTandem_bv(const quint32* tandemStart, const quint32* tandemLast, quint32 repeatLen);
    bool comparePrefixChars(const char*,const char*);
};

class LargeSizedTandemFinder: public ConcreteTandemFinder{
    Q_OBJECT
public:
    LargeSizedTandemFinder(const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _analysisSize);
    ~LargeSizedTandemFinder();

    void run();
private:
    quint32* checkAndSpreadTandem(const quint32* tandemStart, const quint32* tandemLast, const unsigned repeatLen);
    quint32* checkAndSpreadTandem_bv(const quint32* tandemStart, const quint32* tandemLast, const unsigned repeatLen);
    bool comparePrefixChars(const char*,const char*);
};

} //namespace

#endif // RF_SARRAYTANDEMFINDER_H
