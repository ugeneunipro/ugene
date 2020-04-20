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

#include "ORFFinder.h"

#include <U2Algorithm/DynTable.h>
#include <U2Algorithm/RollingArray.h>

#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/Log.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <assert.h>

namespace U2 {

const QString ORFAlgorithmSettings::ANNOTATION_GROUP_NAME("orf");
const QString ORFAlgorithmSettings::STRAND_BOTH("both strands");
const QString ORFAlgorithmSettings::STRAND_DIRECT("direct");
const QString ORFAlgorithmSettings::STRAND_COMPL("complement");

static bool isDirect(ORFAlgorithmStrand s) {
    return s == ORFAlgorithmStrand_Both || s == ORFAlgorithmStrand_Direct;
}

static bool isComplement(ORFAlgorithmStrand s) {
    return s == ORFAlgorithmStrand_Both || s == ORFAlgorithmStrand_Complement;
}

void ORFFindAlgorithm::find(
                            ORFFindResultsListener* rl,
                            const ORFAlgorithmSettings& cfg,
                            U2EntityRef& entityRef,
                            int& stopFlag,
                            int& percentsCompleted)
{
    SAFE_POINT(cfg.maxResult2Search >= 0, "Invalid max results count!", );
    SAFE_POINT(cfg.proteinTT && cfg.proteinTT->isThree2One(), "Amino translation is not 3to1 translation!", );

    TaskStateInfo os;
    U2SequenceObject dnaSeq("sequence",entityRef);

    const int BLOCK_READ_FROM_DB = 128000;
    int seqPointer = 0;
    QByteArray sequence("");

    DNATranslation3to1Impl* aTT = dynamic_cast<DNATranslation3to1Impl*>(cfg.proteinTT);
    SAFE_POINT(aTT != NULL, "Cannot convert DNATranslation to DNATranslation3to1Impl!", );
    bool mustFit = cfg.mustFit;
    bool mustInit = cfg.mustInit;
    bool allowAltStart = cfg.allowAltStart;
    bool allowOverlap = cfg.allowOverlap;
    bool circularSearch = cfg.circularSearch && (cfg.searchRegion.endPos() == dnaSeq.getSequenceLength());
    int minLen = qMax(cfg.minLen, 3);
    CHECK(cfg.searchRegion.length >= minLen, );

    int onePercentLen = cfg.searchRegion.length/100;
    if (cfg.strand == ORFAlgorithmStrand_Both) onePercentLen /=2;
    int leftTillPercent = onePercentLen;
    percentsCompleted = 0;

    if (isDirect(cfg.strand)) {
        QList<int> start[3];
        if (!mustInit) {
            for (int i = 0; i < 3; i++) {
                int frame = (cfg.searchRegion.startPos + i) % 3;
                start[frame].append(cfg.searchRegion.startPos + i);
            }
        }
        qint64 end = cfg.searchRegion.endPos();
        for(qint64 i = cfg.searchRegion.startPos; i < end && !stopFlag && !os.isCoR(); i++, leftTillPercent--,++seqPointer) {
            if((seqPointer % BLOCK_READ_FROM_DB) == 0){ // query to db
                sequence.clear();
                qint64 regLen = qMin(end - i, (qint64)BLOCK_READ_FROM_DB + 3);
                sequence.append(dnaSeq.getSequenceData(U2Region(i, regLen), os));
                SAFE_POINT_OP(os, );
                seqPointer = 0;
            }
            int frame = i % 3;
            QList<int>* initiators = start + frame;
            if (!initiators->isEmpty() && aTT->isStopCodon(sequence.data() + seqPointer)) {
                foreach(int initiator, *initiators) {
                    qint64 len = i - initiator;
                    if (cfg.includeStopCodon) {
                        len += 3;
                    }
                    if (len >= minLen && !os.isCoR()) {
                        rl->onResult(ORFFindResult(U2Region(initiator, len), frame),os);
                    }
                }
                initiators->clear();
                if (!mustInit) {
                    initiators->append(i+3);
                }
            } else if (initiators->isEmpty() || allowOverlap) {
                if (aTT->isStartCodon(sequence.data() + seqPointer)
                    || (allowAltStart && aTT->isCodon(DNATranslationRole_Start_Alternative, sequence.data() + seqPointer))
                   ) {
                    if (initiators->isEmpty() || initiators->last() != i) {
                        initiators->append(i);
                    }
                }
            }
            if (leftTillPercent == 0) {
                percentsCompleted = qMin(percentsCompleted+1,100);
                leftTillPercent = onePercentLen;
            }
        }

        if(circularSearch && !stopFlag && !os.isCoR()){
            //circular
            addStartCodonsFromJunction(dnaSeq, cfg, ORFAlgorithmStrand_Direct, start);

            qint64 regLen = end - cfg.searchRegion.startPos;
            qint64 minInitiator = end;
            bool initiatorsRemain = false;
            for (int i=0; i<3;i++) {
                foreach(int initiator, start[i]) {
                    if(initiator < minInitiator) {
                        minInitiator = initiator;
                        initiatorsRemain = true;
                    }

                }
            }

            checkStopCodonOnJunction(dnaSeq, cfg, ORFAlgorithmStrand_Direct, rl, start, os);
            SAFE_POINT_OP(os, );

            seqPointer = 0;
            for(qint64 i = cfg.searchRegion.startPos; i < minInitiator && !stopFlag && initiatorsRemain && !os.isCoR(); i++,++seqPointer) {
                if( (seqPointer % BLOCK_READ_FROM_DB) == 0){ // query to db
                    sequence.clear();
                    qint64 regLen = qMin((qint64)minInitiator - i, (qint64)BLOCK_READ_FROM_DB + 3);
                    sequence.append(dnaSeq.getSequenceData(U2Region(i, regLen), os));
                    SAFE_POINT_OP(os, );
                    seqPointer = 0;
                }
                int frame = i % 3;
                // NOTE: frames of the start and the end of circular region are not equal!
                int startFrame = (dnaSeq.getSequenceLength() - (3- frame) % 3) % 3;
                QList<int>* initiators = start + startFrame;
                if (!initiators->isEmpty() && aTT->isStopCodon(sequence.data() + seqPointer)) {
                    foreach(int initiator, *initiators) {
                        int len = regLen + i - initiator;
                        if (len>=minLen && !os.isCoR()){
                            if (i == cfg.searchRegion.startPos && !cfg.includeStopCodon) {
                                // stop codon is on junction, not included
                                rl->onResult(ORFFindResult(U2Region(initiator, end - initiator), frame), os);
                            } else {
                                rl->onResult(ORFFindResult(U2Region(initiator, end - initiator),
                                                           U2Region(cfg.searchRegion.startPos,
                                                                    i + 3 * cfg.includeStopCodon), frame),os);
                            }
                        }
                    }
                    initiators->clear();
                }
            }
        }

        if (!mustFit && !stopFlag && !circularSearch) {
            //check if non-terminated ORFs remained
            for (int i = 0; i < 3; i++) {
                foreach (int initiator, start[i]) {
                    int len = end - initiator;
                    len -= len % 3;
                    if (len >= minLen && !os.isCoR()) {
                        rl->onResult(ORFFindResult(U2Region(initiator, len), i + 1),os);
                    }
                }
            }
         }
    }

    if (isComplement(cfg.strand)) {
        assert(cfg.complementTT && cfg.complementTT->isOne2One());

        QList<int> start[3];
        if (!mustInit) {
            for (int i = 0; i < 3; i++) {
                int frame = (cfg.searchRegion.endPos() - i) % 3;
                start[frame].append(cfg.searchRegion.endPos() - 1 - i);
            }
        }

        qint64 end = cfg.searchRegion.startPos;
        seqPointer = 0;
        for(int i = cfg.searchRegion.endPos()-1; i >= end && !stopFlag && !os.isCoR();i--, leftTillPercent--,seqPointer++) {
            if((seqPointer % BLOCK_READ_FROM_DB) == 0){// query to db
                sequence.clear();
                QByteArray tmp;
                qint64 regStart = qMax(end, (qint64)i - (BLOCK_READ_FROM_DB + 3));
                qint64 regLen = qMin(i - end + 1, (qint64)BLOCK_READ_FROM_DB + 3 + 1);
                tmp.append(dnaSeq.getSequenceData(U2Region(regStart, regLen), os));
                SAFE_POINT_OP(os, );
                sequence.append(tmp,tmp.size());
                cfg.complementTT->translate(tmp,tmp.size(),sequence.data(),sequence.size());
                TextUtils::reverse(sequence.data(),sequence.size());
                seqPointer = 0;
            }
            int frame = (i + 1) % 3;
            QList<int>* initiators = start + frame;
            if (!initiators->isEmpty() && aTT->isStopCodon(sequence.data()+seqPointer)) {
                foreach(int initiator, *initiators) {
                    int len = initiator - i;
                    int ind = i;
                    if (cfg.includeStopCodon) {
                        ind -= 3;
                        len += 3;
                    }
                    if (len>=minLen && !os.isCoR()) {
                        rl->onResult(ORFFindResult(U2Region(ind+1, len), frame - 3),os);
                    }
                }
                initiators->clear();
                if (!mustInit) {
                    initiators->append(i-3);
                }
            } else if (initiators->isEmpty() || allowOverlap) {
                if (aTT->isStartCodon(sequence.data()+seqPointer)
                    || (allowAltStart && aTT->isCodon(DNATranslationRole_Start_Alternative, sequence.data()+seqPointer))
                   ) {
                    if (initiators->isEmpty() || initiators->last() != i) {
                        initiators->append(i);
                    }
                }
            }
            if (leftTillPercent == 0) {
                percentsCompleted = qMin(percentsCompleted+1,100);
                leftTillPercent = onePercentLen;
            }
        }
        if(circularSearch && !stopFlag && !os.isCoR()){
            addStartCodonsFromJunction(dnaSeq, cfg, ORFAlgorithmStrand_Complement, start);

            int regLen = cfg.searchRegion.endPos() - cfg.searchRegion.startPos;
            int maxInitiator = -1;
            bool initiatorsRemain = false;
            for (int i=0; i<3;i++) {
                foreach(int initiator, start[i]) {
                    if(initiator > maxInitiator){
                        maxInitiator = initiator;
                        initiatorsRemain = true;
                    }

                }
            }

            checkStopCodonOnJunction(dnaSeq, cfg, ORFAlgorithmStrand_Complement, rl, start, os);

            seqPointer = 0;
            for(qint64 i = cfg.searchRegion.endPos()-1; i >= maxInitiator && !stopFlag && initiatorsRemain && !os.isCoR(); i--,seqPointer++) {
                if((seqPointer % BLOCK_READ_FROM_DB) == 0){// query to db
                    sequence.clear();
                    QByteArray tmp;
                    qint64 regStart = qMax((qint64)maxInitiator, i - (BLOCK_READ_FROM_DB + 3));
                    qint64 regLen = qMin(i - maxInitiator + 1, (qint64)BLOCK_READ_FROM_DB + 3 + 1);
                    tmp.append(dnaSeq.getSequenceData(U2Region(regStart, regLen), os));
                    SAFE_POINT_OP(os, );
                    sequence.append(tmp,tmp.size());
                    cfg.complementTT->translate(tmp,tmp.size(),sequence.data(),sequence.size());
                    TextUtils::reverse(sequence.data(), sequence.size());
                    seqPointer = 0;
                }
                int frame = (i + 1) % 3;
                // NOTE: frames of the start and the end of circular region are not equal!
                int startFrame =  (3 - ((dnaSeq.getSequenceLength() - frame) % 3)) % 3;
                QList<int>* initiators = start + startFrame;
                if (!initiators->isEmpty() && aTT->isStopCodon(sequence.data()+seqPointer)) {
                    foreach(int initiator, *initiators) {
                        int len = regLen + initiator - i ;
                        if (len >= minLen && !os.isCoR()){
                            if (cfg.searchRegion.endPos() == i + 1 && !cfg.includeStopCodon) {
                                rl->onResult(ORFFindResult(U2Region(end, initiator + 1), frame - 3), os);
                            } else {
                                rl->onResult(ORFFindResult(U2Region(i + 1 - 3 * cfg.includeStopCodon,
                                                                    cfg.searchRegion.endPos() - (i + 1 - 3 * cfg.includeStopCodon)),
                                                           U2Region(end, initiator + 1), frame - 3), os);
                            }
                        }
                    }
                    initiators->clear();
                }
            }
        }

        if (!mustFit && !stopFlag && !circularSearch) {
            //check if non-terminated ORFs remained
            for (int i = 0; i < 3; i++) {
                foreach (int initiator, start[i]) {
                    int ind = end + i % 3;
                    int len = initiator - ind + 1;
                    len -= len % 3;
                    if (len >= minLen && !os.isCoR()) {
                        rl->onResult(ORFFindResult(U2Region(ind, len), i - 3),os);
                    }
                }
            }
        }
    }

}

void ORFFindAlgorithm::addStartCodonsFromJunction(const U2SequenceObject &dnaSeq,
                                                  const ORFAlgorithmSettings &cfg,
                                                  ORFAlgorithmStrand strand, QList<int> *start) {
    SAFE_POINT(strand != ORFAlgorithmStrand_Both, "Invalid strand: direct or complement are the only possible variants!", );

    char *tmp;
    DNATranslation3to1Impl* aTT = (DNATranslation3to1Impl*)cfg.proteinTT;
    qint64 seqLen = dnaSeq.getSequenceLength();

    for (int i = 1; i <= 2; i++) {
        tmp = getCodonFromJunction(dnaSeq, strand, i);
        SAFE_POINT(tmp != NULL, "Incorrect codon!", );
        if (strand == ORFAlgorithmStrand_Complement) {
            cfg.complementTT->translate(tmp, 3);
        }

        if (aTT->isStartCodon(tmp) || (cfg.allowAltStart && aTT->isCodon(DNATranslationRole_Start_Alternative, tmp))) {
            if (strand == ORFAlgorithmStrand_Direct) {
                start[(seqLen - i) % 3].append(seqLen - i);
            } else {
                start[3 - i].append(2 - i);
            }
        }
    }

    delete[] tmp;
}

void ORFFindAlgorithm::checkStopCodonOnJunction(const U2SequenceObject &dnaSeq, const ORFAlgorithmSettings &cfg,
                                                ORFAlgorithmStrand strand, ORFFindResultsListener *rl,
                                                QList<int> *start,
                                                TaskStateInfo &os) {
    CHECK_EXT(strand != ORFAlgorithmStrand_Both, os.setError("Invalid strand: direct or complement are the only possible variants!"), );

    qint64 seqLen = dnaSeq.getSequenceLength();
    int regLen = cfg.searchRegion.length;
    DNATranslation3to1Impl* aTT = dynamic_cast<DNATranslation3to1Impl*>(cfg.proteinTT);
    CHECK_EXT(aTT != NULL, os.setError("Cannot convert DNATranslation to DNATranslation3to1Impl!"), );
    if (strand == ORFAlgorithmStrand_Direct){
        int end = cfg.searchRegion.endPos();
        for (int i = 1; i <= 2; i++) {
            char *tmp = getCodonFromJunction(dnaSeq, strand, i);
            CHECK_EXT(tmp != NULL, os.setError("Incorrect codon"), );

            if (aTT->isStopCodon(tmp)) {
                int startFrame = (seqLen - i) % 3;
                QList<int>* initiators = start + startFrame;
                if (!initiators->isEmpty()) {
                    foreach(int initiator, *initiators) {
                        int len = regLen - initiator + 3 - i;
                        if (len >= cfg.minLen && !os.isCoR()){
                            if (cfg.includeStopCodon) {
                                rl->onResult(ORFFindResult(U2Region(initiator, end - initiator),
                                                           U2Region(0, 3 - i), startFrame),os);
                            } else {
                                rl->onResult(ORFFindResult(U2Region(initiator, end - initiator - i), startFrame), os);
                            }
                        }
                    }
                    initiators->clear();
                }
            }
        }
    }


    if (strand == ORFAlgorithmStrand_Complement)
    {
        for (int i = 1; i <=2; i++) {
            char *tmp = getCodonFromJunction(dnaSeq, strand, i);
            SAFE_POINT(tmp != NULL, "Incorrect codon", );
            cfg.complementTT->translate(tmp, 3);

            if (aTT->isStopCodon(tmp)) {
                int startFrame = 3 - i;
                QList<int>* initiators = start + startFrame;
                if (!initiators->isEmpty()) {
                    foreach(int initiator, *initiators) {
                        int len = initiator + 1;
                        if (len >= cfg.minLen && !os.isCoR()){
                            if (cfg.includeStopCodon) {
                                rl->onResult(ORFFindResult(U2Region(seqLen - i, i),
                                                           U2Region(0, len), startFrame - 3), os);
                            } else {
                                rl->onResult(ORFFindResult(U2Region(3 - i, len - (3 - i)), startFrame - 3), os);
                            }
                        }
                    }
                    initiators->clear();
                }
            }

        }
    }
}

char* ORFFindAlgorithm::getCodonFromJunction(const U2SequenceObject &dnaSeq, ORFAlgorithmStrand strand,
                                             int symbolsFromEnd) {
    SAFE_POINT(strand != ORFAlgorithmStrand_Both, "Invalid strand: direct or complement are the only possible variants!", NULL);
    SAFE_POINT(symbolsFromEnd == 1 || symbolsFromEnd == 2, "Codon can be split only in two places: after the first or the second symbol!", NULL);

    char *tmp = new char[3];
    int seqLen = dnaSeq.getSequenceLength();
    if (strand == ORFAlgorithmStrand_Direct) {
        if (symbolsFromEnd == 1) {
            //[Y, Z, ... , X] --> codon XYZ
            tmp[0] = dnaSeq.getSequenceData(U2Region(seqLen - 1, 1)).at(0);
            tmp[1] = dnaSeq.getSequenceData(U2Region(0, 1)).at(0);
            tmp[2] = dnaSeq.getSequenceData(U2Region(1, 1)).at(0);
        }

        if (symbolsFromEnd == 2) {
            //[Z, ... , X, Y] --> codon XYZ
            tmp[0] = dnaSeq.getSequenceData(U2Region(seqLen - 2, 1)).at(0);
            tmp[1] = dnaSeq.getSequenceData(U2Region(seqLen - 1, 1)).at(0);
            tmp[2] = dnaSeq.getSequenceData(U2Region(0, 1)).at(0);
        }


    } else {
        if (symbolsFromEnd == 1) {
            //complement [Y, X, ... , Z] --> codon XYZ
            tmp[0] = dnaSeq.getSequenceData(U2Region(1, 1)).at(0);
            tmp[1] = dnaSeq.getSequenceData(U2Region(0, 1)).at(0);
            tmp[2] = dnaSeq.getSequenceData(U2Region(seqLen - 1, 1)).at(0);
        }

        if (symbolsFromEnd == 2) {
            //complement [X, ... , Z, Y] --> codon XYZ
            tmp[0] = dnaSeq.getSequenceData(U2Region(0, 1)).at(0);
            tmp[1] = dnaSeq.getSequenceData(U2Region(seqLen - 1, 1)).at(0);
            tmp[2] = dnaSeq.getSequenceData(U2Region(seqLen - 2, 1)).at(0);
        }
    }

    return tmp;
}


QString ORFAlgorithmSettings::getStrandStringId(ORFAlgorithmStrand strand)
{
    if (strand == ORFAlgorithmStrand_Direct) {
        return STRAND_DIRECT;
    } else if (strand == ORFAlgorithmStrand_Complement) {
        return STRAND_COMPL;
    } else {
        assert (strand == ORFAlgorithmStrand_Both );
        return STRAND_BOTH;
    }
}

ORFAlgorithmStrand ORFAlgorithmSettings::getStrandByStringId( const QString& id )
{
    if (id == STRAND_DIRECT) {
        return ORFAlgorithmStrand_Direct;
    } else if (id == STRAND_COMPL) {
        return ORFAlgorithmStrand_Complement;
    } else {
        return ORFAlgorithmStrand_Both;
    }
}

}//namespace

