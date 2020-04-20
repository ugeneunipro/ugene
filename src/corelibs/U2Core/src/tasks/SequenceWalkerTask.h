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

#ifndef _U2_SEQUENCE_WALKER_TASK_H_
#define _U2_SEQUENCE_WALKER_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class DNATranslation;
class SequenceWalkerSubtask;

enum StrandOption {
    StrandOption_DirectOnly,
    StrandOption_ComplementOnly,
    StrandOption_Both
};
class U2CORE_EXPORT SequenceWalkerConfig {
public:

    //TODO: allow select custom strand only!

    SequenceWalkerConfig();

    const char*     seq;            //  sequence to split
    quint64         seqSize;        //  size of the sequence to split
    U2Region         range;          //  if not empty -> only this region is processed
    DNATranslation* complTrans;
    DNATranslation* aminoTrans;

    quint64         chunkSize;          // optimal chunk size, used by default for all regions except last one
    int             lastChunkExtraLen;  // extra length allowed to be added to the last chunk
    int             overlapSize;        // overlap for 2 neighbor regions
    int             nThreads;
    StrandOption    strandToWalk;

    bool            walkCircular;
    quint64         walkCircularDistance;
};

class U2CORE_EXPORT SequenceWalkerCallback {
public:
    virtual ~SequenceWalkerCallback(){}

    virtual void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) = 0;

    /* implement this to give SequenceWalkerSubtask required resources
     * here are resources for ONE(!) SequenceWalkerSubtask execution e.g. for one execution of onRegion function
     */
    virtual QList< TaskResourceUsage > getResources( SequenceWalkerSubtask * t ) {Q_UNUSED(t); return QList< TaskResourceUsage >(); }
};

class U2CORE_EXPORT SequenceWalkerTask : public Task {
    Q_OBJECT
public:
    SequenceWalkerTask(const SequenceWalkerConfig& config, SequenceWalkerCallback* callback,
        const QString& name, TaskFlags tf = TaskFlags_NR_FOSE_COSC);

    SequenceWalkerCallback*     getCallback() const {return callback;}
    const SequenceWalkerConfig& getConfig() const {return config;}

    // reverseMode - start splitting from the end of the range
    static QVector<U2Region> splitRange(const U2Region& range, int chunkSize, int overlapSize, int lastChunkExtraLen, bool reverseMode);

    void setError(const QString& err) {stateInfo.setError(err);}

private:
    QList<SequenceWalkerSubtask*> prepareSubtasks();
    QList<SequenceWalkerSubtask*> createSubs(const QVector<U2Region>& chunks, bool doCompl, bool doAmino);

    SequenceWalkerConfig    config;
    SequenceWalkerCallback* callback;

    QByteArray              tempBuffer;
};

class U2CORE_EXPORT SequenceWalkerSubtask : public Task {
    Q_OBJECT
public:
    SequenceWalkerSubtask(SequenceWalkerTask* t, const U2Region& globalReg, bool lo, bool ro,
                        const char* localSeq, int localLen, bool doCompl, bool doAmino);

    void run();

    const char* getRegionSequence();

    int  getRegionSequenceLen();

    bool isDNAComplemented() const {return doCompl;}

    bool isAminoTranslated() const {return doAmino;}

    U2Region getGlobalRegion() const {return globalRegion;}

    const SequenceWalkerConfig& getGlobalConfig() const {return t->getConfig();}

    bool intersectsWithOverlaps(const U2Region& globalReg) const;
    bool hasLeftOverlap() const {return leftOverlap;}
    bool hasRightOverlap() const {return rightOverlap;}

private:
    bool needLocalRegionProcessing() const {return (doAmino || doCompl) && processedSeqImage.isEmpty();}
    void prepareLocalRegion();

    SequenceWalkerTask*     t;
    U2Region                 globalRegion;
    const char*             localSeq;
    const char*             originalLocalSeq;
    int                     localLen;
    int                     originalLocalLen;
    bool                    doCompl;
    bool                    doAmino;
    bool                    leftOverlap;
    bool                    rightOverlap;

    QByteArray              processedSeqImage;

};


}//namespace

#endif
