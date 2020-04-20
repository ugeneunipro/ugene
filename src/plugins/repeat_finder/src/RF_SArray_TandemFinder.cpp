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

#include <QMutexLocker>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>

#include "RFConstants.h"
#include "RF_SArray_TandemFinder.h"

namespace U2 {

const int FindTandemsTaskSettings::DEFAULT_MIN_REPEAT_COUNT = 0;
const int FindTandemsTaskSettings::DEFAULT_MIN_TANDEM_SIZE = 9;

FindTandemsToAnnotationsTask::FindTandemsToAnnotationsTask(const FindTandemsTaskSettings& s, const DNASequence& seq, const QString& _an, const QString& _gn, const QString &annDescription, const GObjectReference& _aor):
Task(tr("Find repeats to annotations"), TaskFlags_NR_FOSCOE), saveAnns(true), mainSeq(seq), annName(_an), annGroup(_gn), annDescription(annDescription), annObjRef(_aor), s(s)
{
    GCOUNTER(cvar, tvar, "FindTandemsToAnnotationsTask");
    setVerboseLogMode(true);
    if (annObjRef.isValid()) {
        LoadUnloadedDocumentTask::addLoadingSubtask(this,
            LoadDocumentTaskConfig(true, annObjRef, new LDTObjectFactory(this)));
    }
    addSubTask(new TandemFinder(s, mainSeq));
}

FindTandemsToAnnotationsTask::FindTandemsToAnnotationsTask(const FindTandemsTaskSettings& s, const DNASequence& seq)
: Task(tr("Find repeats to annotations"), TaskFlags_NR_FOSCOE), saveAnns(false), mainSeq(seq), s(s)
{
    GCOUNTER(cvar, tvar, "FindTandemsToAnnotationsTask");
    setVerboseLogMode(true);
    addSubTask(new TandemFinder(s, mainSeq));
}

QList<Task*> FindTandemsToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (qobject_cast<TandemFinder*>(subTask)!=NULL){
        TandemFinder* tandemFinderTask = qobject_cast<TandemFinder*>(subTask);
        QList<SharedAnnotationData> annotations = importTandemAnnotations(tandemFinderTask->getResults(),
            tandemFinderTask->getSettings().seqRegion.startPos, tandemFinderTask->getSettings().showOverlappedTandems);
        if (saveAnns) {
            if (!annotations.isEmpty()) {
                algoLog.info(tr("Found %1 repeat regions").arg(annotations.size()));
                Task* createTask = new CreateAnnotationsTask(annObjRef, annotations, annGroup);
                createTask->setSubtaskProgressWeight(0);
                res.append(createTask);
            }
        } else {
            result << annotations;
        }
    }
    return res;
}

QList<SharedAnnotationData> FindTandemsToAnnotationsTask::importTandemAnnotations(const QList<Tandem>& tandems, qint64 seqStart, const bool showOverlapped) {

    seqStart += s.reportSeqShift;

    QList<SharedAnnotationData> res;
    foreach(const Tandem& tan, tandems) {
        unsigned offset = 0;
        const unsigned maxOffset = tan.size % tan.repeatLen;
        do{
            SharedAnnotationData ad(new AnnotationData());
            ad->type = U2FeatureTypes::RepeatRegion;
            ad->name = annName;
            const quint32 tandemEnd = tan.offset+tan.size+seqStart;
            quint32 pos = tan.offset+seqStart+offset;
            for(; pos<=tandemEnd-tan.repeatLen; pos+=tan.repeatLen){
                ad->location->regions << U2Region(pos, tan.repeatLen);
            }
            if (ad->location->isEmpty()){
                continue;
            }
            ad->qualifiers.append(U2Qualifier("num_of_repeats", QString::number(tan.size / tan.repeatLen)));
            ad->qualifiers.append(U2Qualifier("repeat_length", QString::number(tan.repeatLen)));
            ad->qualifiers.append(U2Qualifier("whole_length", QString::number(tan.size)));
            U1AnnotationUtils::addDescriptionQualifier(ad, annDescription);
            res.append(ad);
            offset++;
        }while(showOverlapped && offset<=maxOffset);
    }
    return res;
}

TandemFinder::TandemFinder(const FindTandemsTaskSettings& _settings, const DNASequence& directSequence):
Task(tr("Find tandems"), TaskFlags_FOSCOE),
settings(_settings),regionCount(0){
    if (settings.seqRegion.length == 0) {
        settings.seqRegion= U2Region(0, directSequence.length());
    }
    startTime = GTimer::currentTimeMicros();
    sequence = (char*)directSequence.constData() + settings.seqRegion.startPos;
}

class TF_WalkerConfig: public SequenceWalkerConfig{
public:
    // TODO: check seqSize type compatibility!
    TF_WalkerConfig(const char* _sequence, quint32 _seqSize, quint32 _chunkSize){
        seq=_sequence;
        seqSize=_seqSize;
        chunkSize=_chunkSize;
        lastChunkExtraLen=_chunkSize/2;
        overlapSize=TandemFinder::maxCheckPeriod;
        walkCircular = false;
    }
};

void TandemFinder::prepare() {
    Q_ASSERT(settings.minRepeatCount >= 2);
    int chunkSize = 32*1024*1024; // optimized for 32Mb mem-chunk
    addSubTask(new SequenceWalkerTask(TF_WalkerConfig(sequence, settings.seqRegion.length, chunkSize), this, tr("Find tandems"), TaskFlags_NR_FOSCOE));
}

void TandemFinder::run(){
    algoLog.trace(tr("Find tandems finished %1").arg(GTimer::currentTimeMicros()-startTime));
}

QList<Task*> TandemFinder::onSubTaskFinished(Task* subTask){
    if (qobject_cast<SequenceWalkerTask*>(subTask)!=NULL){
        setMaxParallelSubtasks(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
        return regionTasks;
    }
    if (qobject_cast<TandemFinder_Region*>(subTask)!=NULL){
        TandemFinder_Region* regionTask = qobject_cast<TandemFinder_Region*>(subTask);
        const quint64 offs = regionTask->getRegionOffset();
        QMutexLocker foundTandemsLocker(&tandemsAccessMutex);
        QList<Tandem> regionTandems = regionTask->getResult();
        QMutableListIterator<Tandem> comTandIt(foundTandems);
        foreach(Tandem t, regionTandems){
            t.offset += offs;
            t.rightSide += offs;
            while(comTandIt.hasNext() && comTandIt.peekNext()<t){
                comTandIt.next();
            }
            if (!comTandIt.hasNext() || t<comTandIt.peekNext()){
                comTandIt.insert(t);
            }else{
                comTandIt.next().extend(t);
            }
        }
    }
    return QList<Task*>();
}

void TandemFinder::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti){
    if(ti.hasError()){return;}
    quint64 offs = t->getRegionSequence() - t->getGlobalConfig().seq;
    QMutexLocker lock(&subtasksQueue); //TODO: fix me
    regionTasks.append(new TandemFinder_Region(regionCount++, t->getRegionSequence(), t->getRegionSequenceLen(), offs, settings));
}

TandemFinder_Region::~TandemFinder_Region(){
    QMutexLocker tandemsAccessLocker(&tandemsAccessMutex);
}

QList<Task*> TandemFinder_Region::onSubTaskFinished(Task* subTask){
    if (qobject_cast<ConcreteTandemFinder*>(subTask)!=NULL){
        subTask->cleanup();
    }
    return QList<Task*>();
}

void TandemFinder_Region::prepare(){
    Q_ASSERT(settings.minPeriod<=settings.maxPeriod);
    int period=1;
    for (; period<=16; period = period*2+1){
        if (period*2<settings.minPeriod || period>settings.maxPeriod || period>=seqSize) continue;
        addSubTask(new ExactSizedTandemFinder(sequence, seqSize, settings, period));
    }
    {
        if (period>settings.maxPeriod) return;
        addSubTask(new LargeSizedTandemFinder(sequence, seqSize, settings, period));
    }
}

void TandemFinder_Region::addResult(const Tandem& tandem){
    QMutexLocker tandemsAccessLocker(&tandemsAccessMutex);
    foundTandems.append(tandem);
}
void  TandemFinder_Region::addResults(const QMap<Tandem,Tandem>& tandems){
    QMutexLocker tandemsAccessLocker(&tandemsAccessMutex);
    foundTandems.append(tandems.values());
}

ConcreteTandemFinder::ConcreteTandemFinder(QString taskName, const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _prefixLength):
    Task(taskName, TaskFlags_FOSCOE), sequence(_sequence), seqSize(_seqSize), index(NULL), suffixArray(NULL),
    settings(_settings),prefixLength(_prefixLength),suffArrSize(_seqSize-_prefixLength+1){
        Q_ASSERT(settings.minRepeatCount>1);
        int suffArrMemory;
        if (settings.algo == TSConstants::AlgoSuffixBinary){
            suffArrMemory = seqSize/4 + seqSize*sizeof(quint32) + ((size_t)1<<qMin(prefixLength*2,24))*sizeof(quint64)*7/6;
        }else{
            suffArrMemory = seqSize*sizeof(quint32)*2;
        }
        suffArrMemory = qMax(suffArrMemory/(1024*1024),1); //in Mb
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, suffArrMemory, true));
}

void ConcreteTandemFinder::prepare(){
    const quint32* nuclTable = bitsTable.getBitMaskCharBits(DNAAlphabet_NUCL);
    const quint32 bitMaskCharBitsNum = bitsTable.getBitMaskCharBitsNum(DNAAlphabet_NUCL);
    //single thread approximation (re-estimated in some algorithms)
    int arrayPercent = int((seqSize / double(seqSize + prefixLength)) * 100 / 5); //array creation time ~5 times faster than search

    if (settings.algo == TSConstants::AlgoSuffix){
        //index with double sized match
        CreateSArrayIndexTask* indexTask = new CreateSArrayIndexTask(sequence, seqSize, prefixLength,  'N', nuclTable, bitMaskCharBitsNum);
        indexTask->setSubtaskProgressWeight(arrayPercent/100.0F);
        // TODO fix algorithm selection
        if(qobject_cast<ExactSizedTandemFinder*>(this)!=NULL){
            addSubTask(indexTask);
        }
    }
}

QList<Task*> ConcreteTandemFinder::onSubTaskFinished(Task* subTask){
    if (qobject_cast<CreateSArrayIndexTask*>(subTask)!=NULL){
        index = qobject_cast<const CreateSArrayIndexTask*>(subTask)->index;
    }
    return QList<Task*>();
}

void ConcreteTandemFinder::cleanup(){
    if (getSubtasks().size()==0) {
        return;
    }
    getSubtasks().first()->cleanup();
}

ExactSizedTandemFinder::ExactSizedTandemFinder(const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _analysisSize)
: ConcreteTandemFinder(tr("Find %1-period tandems").arg(_analysisSize),_sequence,_seqSize,_settings,_analysisSize*2)
{
}

ExactSizedTandemFinder::~ExactSizedTandemFinder() {

};

inline char* seqCast(const quint32* suffArr, int ind){
    return reinterpret_cast<char*>(
        suffArr[ind]
   );
}

void ExactSizedTandemFinder::run(){
    if (seqSize<settings.minPeriod*settings.minRepeatCount){
        return;
    }
    if (seqSize<prefixLength){
        return;
    }
    int minPeriod = qMax(settings.minPeriod, prefixLength/2);
    int maxPeriod = qMin(settings.maxPeriod, prefixLength);
    if (index==NULL){
        try {
            suffixArray = new SuffixArray(sequence, seqSize, prefixLength);
        } catch (...) {
            setError("Not enough memory");
            return;
        }
        const BitMask& bitMask = suffixArray->getBitMask();
        const quint32* sArray = suffixArray->getArray();
        quint32* currentDiffPos = (quint32*)sArray;
        const quint32* sArrayLast  = sArray +suffArrSize-1;
        while(currentDiffPos<sArrayLast){
            int diff  = currentDiffPos[1]-currentDiffPos[0];
            if (diff>=minPeriod && diff<=maxPeriod){    //only our exact size
                quint32 suffixOffset = qMax(1, (settings.minTandemSize-prefixLength)/diff);
                if (currentDiffPos+suffixOffset<=sArrayLast && currentDiffPos[suffixOffset]-currentDiffPos[0] == suffixOffset*diff){
                    if (bitMask[currentDiffPos[0]]==bitMask[currentDiffPos[suffixOffset]]){
                        currentDiffPos = checkAndSpreadTandem_bv(currentDiffPos,currentDiffPos+suffixOffset, diff);
                        continue;
                    }
                }
            }
            currentDiffPos++;
        }
        delete suffixArray;//TODO: remove all deletes
    }else{
        const quint32* sArray = index->getSArray();
        const quint32* sArrayLast  = const_cast<quint32*>(sArray + index->getSArraySize() - 1);
        quint32* currentDiffPos = (quint32*)sArray;

        while(currentDiffPos<sArrayLast){
            const qint32 diff  = currentDiffPos[1]-currentDiffPos[0];
            if (diff>=minPeriod && diff<=maxPeriod){    //only our exact size
                unsigned suffixOffset = qMax(1, signed(settings.minTandemSize-prefixLength)/diff);
                if (currentDiffPos+suffixOffset>sArrayLast || currentDiffPos[suffixOffset]-currentDiffPos[0]!=suffixOffset*diff){
                }else{
                    const char* seq0 = index->sarr2seq(currentDiffPos);//seqCast(currentDiffPos,0);
                    const char* seq1 = index->sarr2seq(currentDiffPos+suffixOffset);//seqCast(currentDiffPos,suffixOffset);
                    if(seq0 == NULL || seq1 == NULL){
                        continue;
                    }
                    // in the other words  prefix currentDiffPos[0]==currentDiffPos[suffixOffset]
                    if (comparePrefixChars(seq0,seq1)){
                        currentDiffPos = checkAndSpreadTandem(currentDiffPos,currentDiffPos+suffixOffset, diff);
                        continue;
                    }
                }
            }
            currentDiffPos++;
        }
    }

    qobject_cast<TandemFinder_Region*>(getParentTask())->addResults(rawTandems);
}

bool ExactSizedTandemFinder::comparePrefixChars(const char* first,const char* second){
    return strncmp(first,second,prefixLength)==0;
}
quint32* ExactSizedTandemFinder::checkAndSpreadTandem(const quint32* tandemStartIndex, const quint32* tandemLastIndex, quint32 repeatLen){
    const char* tandemStart = index->sarr2seq(tandemStartIndex);//seqCast(tandemStartIndex,0);
    quint32* arrRunner = (quint32*)tandemLastIndex-1;
    {
        const quint32* sArrayLast  = const_cast<quint32*>(index->getSArray() + index->getSArraySize() - 1);
        // run until distance become incorrect, it is incredible if we run far with changing prefix
        do{
            ++arrRunner;
        }while (arrRunner<sArrayLast && arrRunner[1]-arrRunner[0]==repeatLen);
        while(!comparePrefixChars(tandemStart, index->sarr2seq(arrRunner)/*seqCast(arrRunner,0)*/)){
            --arrRunner;
        }
    }

//    if (index->getMaskedSequence()!=NULL){
    if (false){
    }else{
        // in this case seqCast(tandemStartIndex,1)==seqCast(tandemStartIndex,0)+repeatLen
        const char* seqRunner = index->sarr2seq(arrRunner);//seqCast(arrRunner,0);
        //move forward by 0.5-1.0 repeatlen to find exact lower bound
        const char* seqEnd = sequence + seqSize;
        while(seqRunner<=seqEnd-repeatLen && strncmp(tandemStart, seqRunner, repeatLen)==0){
            seqRunner += repeatLen;
        }

        const quint32 size = seqRunner-tandemStart;
        const Tandem tandem(tandemStart - sequence, repeatLen, size);
        //check if there is already marked area
        QMap<Tandem,Tandem>::iterator it = rawTandems.find(tandem);
        if (it==rawTandems.end()){ // there are no close tandems
            int tandemMinSize = qMax(settings.minTandemSize, settings.minRepeatCount*tandem.repeatLen);
            if (tandem.size>=tandemMinSize){
                rawTandems.insert(tandem, tandem);
            }
        }else{
            Tandem t = *it;
            rawTandems.erase(it);
            t.extend(tandem);
            rawTandems.insert(t,t);
        }
    }
    return arrRunner;
}

quint32* ExactSizedTandemFinder::checkAndSpreadTandem_bv(const quint32* tandemStartIndex, const quint32* tandemLastIndex, quint32 repeatLen){
    const BitMask& bitMask = suffixArray->getBitMask();
    const quint32 tandemStart = tandemStartIndex[0];

    const quint64 matchRepeat = bitMask[tandemStart];
    quint32* arrRunner = (quint32*)tandemLastIndex-1;
    {
        const quint32* sArrayLast  = suffixArray->getArray()+suffArrSize-1;
        // run until distance become incorrect, it is incredible if we would run far with changing prefix
        do{
            ++arrRunner;
        }while (arrRunner<sArrayLast && arrRunner[1]-arrRunner[0]==repeatLen);
        while(matchRepeat!=bitMask[arrRunner[0]]){
            --arrRunner;
        }
    }

    //    if (index->getMaskedSequence()!=NULL){
    if (false){
    }else{
        // in this case seqCast(tandemStartIndex,1)==seqCast(tandemStartIndex,0)+repeatLen
        quint32 seqRunner = arrRunner[0];
        //move forward by 0.5-1.0 repeatlen to find exact lower bound
        const quint32& seqEnd = seqSize;
        const quint64 periodMask = ~(~0ull>>(2*repeatLen));
        while(seqRunner<=seqEnd-repeatLen && ((matchRepeat^bitMask[seqRunner])&periodMask)==0){
            seqRunner += repeatLen;
        }

        const quint32 size = seqRunner-tandemStart;
        const Tandem tandem(tandemStart, repeatLen, size);
        //check if there is already marked area
        QMap<Tandem,Tandem>::iterator it = rawTandems.find(tandem);
        if (it==rawTandems.end()){ // there are no close tandems
            int tandemMinSize = qMax(settings.minTandemSize, settings.minRepeatCount*tandem.repeatLen);
            if (tandem.size >= tandemMinSize){
                rawTandems.insert(tandem, tandem);
            }
        }else{
            Tandem t = *it;
            rawTandems.erase(it);
            t.extend(tandem);
            rawTandems.insert(t,t);
        }
    }
    return arrRunner;
}

LargeSizedTandemFinder::LargeSizedTandemFinder(const char* _sequence, const long _seqSize, const FindTandemsTaskSettings& _settings, const int _analysisSize):
ConcreteTandemFinder(tr("Find big-period tandems"),_sequence,_seqSize,_settings,_analysisSize){}
LargeSizedTandemFinder::~LargeSizedTandemFinder(){}

void LargeSizedTandemFinder::run(){
    if (seqSize<settings.minPeriod*settings.minRepeatCount){
        return;
    }
    if (seqSize<prefixLength){
        return;
    }
    int minPeriod = qMax(settings.minPeriod, prefixLength);
    int maxPeriod = settings.maxPeriod;
    if (index==NULL){
        try {
            suffixArray = new SuffixArray(sequence, seqSize, prefixLength);
        } catch (...) {
            setError("Not enough memory");
            return;
        }
        const BitMask& bitMask = suffixArray->getBitMask();
        const quint32* sArray = suffixArray->getArray();
        quint32* currentDiffPos = (quint32*)sArray;
        const quint32* sArrayLast  = sArray+suffArrSize-1;
        while(currentDiffPos<sArrayLast){
            int diff  = currentDiffPos[1]-currentDiffPos[0];
            if (diff>=minPeriod && diff<=maxPeriod){
                quint32 firstSuffixPos = currentDiffPos[0];
                quint32 secondSuffixPos = currentDiffPos[1];
                const quint32 endSuffixPos = secondSuffixPos;
                bool correctRepeat = false;
                do{
                    correctRepeat = bitMask[firstSuffixPos]==bitMask[secondSuffixPos];
                    firstSuffixPos += prefixLength;
                    secondSuffixPos += prefixLength;
                }while(correctRepeat && firstSuffixPos<endSuffixPos);
                if (correctRepeat){
                    currentDiffPos = checkAndSpreadTandem_bv(currentDiffPos,currentDiffPos+1, diff);
                    continue;
                }
            }
            currentDiffPos++;
        }
        delete suffixArray;
    }else{
        const quint32* sArray = index->getSArray();
        const quint32* sArrayLast  = const_cast<quint32*>(sArray + index->getSArraySize() - 1);
        quint32* currentDiffPos = (quint32*)sArray;

        // TODO: rewrite this code
        while(currentDiffPos<sArrayLast){
            const qint32 diff  = currentDiffPos[1]-currentDiffPos[0];
            if (diff>=minPeriod && diff<=maxPeriod){    //skip already found tandems
                unsigned suffixOffset = static_cast<unsigned>(qMax(1, signed(settings.minTandemSize - prefixLength) / diff));
                if (currentDiffPos+suffixOffset>sArrayLast || currentDiffPos[suffixOffset]-currentDiffPos[0]!=suffixOffset*diff){
                    //                    currentDiffPos += suffixOffset;continue;
                }else{
                    const char* seq0 = index->sarr2seq(currentDiffPos);//seqCast(currentDiffPos,0);
                    const char* seq1 = index->sarr2seq(currentDiffPos+suffixOffset);//seqCast(currentDiffPos,suffixOffset);
                    // in the other words  prefix currentDiffPos[0]==currentDiffPos[1]
                    if (comparePrefixChars(seq0,seq1)){
                        currentDiffPos = checkAndSpreadTandem(currentDiffPos,currentDiffPos+suffixOffset, diff);
                        continue;
                    }
                }
            }
            currentDiffPos++;
        }
    }

    qobject_cast<TandemFinder_Region*>(getParentTask())->addResults(rawTandems);
}
bool LargeSizedTandemFinder::comparePrefixChars(const char* first,const char* second){
    return strncmp(first,second,prefixLength)==0;
}
quint32* LargeSizedTandemFinder::checkAndSpreadTandem(const quint32* tandemStartIndex, const quint32* tandemLastIndex, const unsigned repeatLen){
    const char* tandemStart = index->sarr2seq(tandemStartIndex);//seqCast(tandemStartIndex,0);

    quint32* arrRunner = (quint32*)tandemLastIndex-1;
    {
        const quint32* sArrayLast  = const_cast<quint32*>(index->getSArray() + index->getSArraySize() - 1);
        // run until distance become incorrect, it is incredible if we run far with changing prefix
        do{
            ++arrRunner;
        }while (arrRunner<sArrayLast && arrRunner[1]-arrRunner[0]==repeatLen);
        while(!comparePrefixChars(tandemStart, index->sarr2seq(arrRunner)/*seqCast(arrRunner,0)*/)){
            --arrRunner;
        }
    }

    // in this case seqCast(tandemStartIndex,1)==seqCast(tandemStartIndex,0)+repeatLen
    const char* seqRunner = index->sarr2seq(arrRunner);//seqCast(arrRunner,0);
    //move forward by 0.5-1.0 of repeat len to find exact lower bound
    const char* seqEnd = sequence + seqSize;
    while(seqRunner<=seqEnd-repeatLen && strncmp(tandemStart, seqRunner, repeatLen)==0){
        seqRunner += repeatLen;
    }

    const quint32 size = seqRunner-tandemStart;
    const Tandem tandem(tandemStart - sequence, repeatLen, size);
    //check if there is already marked area
    QMap<Tandem,Tandem>::iterator it = rawTandems.find(tandem);
    if (it==rawTandems.end()){ // there are no close tandems
        rawTandems.insert(tandem, tandem);
    }else{
        Tandem t = *it;
        rawTandems.erase(it);
        t.extend(tandem);
        rawTandems.insert(t,t);
    }

    return arrRunner;
}

quint32* LargeSizedTandemFinder::checkAndSpreadTandem_bv(const quint32* tandemStartIndex, const quint32* tandemLastIndex, const unsigned repeatLen){
    const BitMask& bitMask = suffixArray->getBitMask();
    const quint32 tandemStart = tandemStartIndex[0];
    quint32 tandemLast = tandemLastIndex[0];
    const quint32& seqEnd = seqSize;
    // repeatLen > prefixLength
    while(tandemLast<seqEnd-prefixLength && bitMask[tandemLast]==bitMask[tandemLast-repeatLen]){
        tandemLast += prefixLength;
    }

    const quint32 size = tandemLast-tandemStart;
    const Tandem tandem(tandemStart, repeatLen, size);
    //check if there is already marked area
    QMap<Tandem,Tandem>::iterator it = rawTandems.find(tandem);
    if (it==rawTandems.end()){ // there are no close tandems
        int tandemMinSize = qMax(settings.minTandemSize, 2*tandem.repeatLen);
        if (tandem.size>=tandemMinSize){
            rawTandems.insert(tandem, tandem);
        }
    }else{
        Tandem t = *it;
        rawTandems.erase(it);
        t.extend(tandem);
        rawTandems.insert(t,t);
    }
    return const_cast<quint32*>(tandemStartIndex + size/repeatLen);
}

} //namespace
