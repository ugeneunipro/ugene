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

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/TextUtils.h>
#include <U2Core/SequenceUtils.h>

#include "ExtractAnnotatedRegionTask.h"

namespace U2 {

ExtractAnnotatedRegionTask::ExtractAnnotatedRegionTask(const DNASequence & sequence_, const SharedAnnotationData &sd_,
    const ExtractAnnotatedRegionTaskSettings & cfg_)
    : Task(tr("Extract annotated regions"), TaskFlag_None), inputSeq(sequence_), inputAnn(sd_), cfg(cfg_), complT(NULL), aminoT(NULL) {

}

void ExtractAnnotatedRegionTask::prepare() {
    prepareTranslations();
}

void ExtractAnnotatedRegionTask::prepareTranslations() {
    //TODO move these logic somewhere above
    bool aminoSeq = inputSeq.alphabet->isAmino();
    if (aminoSeq) {
        return;
    }
    if (cfg.complement && inputAnn->getStrand().isCompementary()) {
        DNATranslation* compTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(inputSeq.alphabet);
        if (compTT != NULL) {
            complT = compTT;
        }
    }

    if (cfg.translate) {
        DNATranslationType dnaTranslType = (inputSeq.alphabet->getType() == DNAAlphabet_NUCL)
            ? DNATranslationType_NUCL_2_AMINO : DNATranslationType_RAW_2_AMINO;
        QList<DNATranslation*> aminoTTs = AppContext::getDNATranslationRegistry()->lookupTranslation(inputSeq.alphabet, dnaTranslType);
        if (!aminoTTs.isEmpty()) {
            aminoT = AppContext::getDNATranslationRegistry()->getStandardGeneticCodeTranslation(inputSeq.alphabet);
        }
    }
}

void ExtractAnnotatedRegionTask::run() {
    QVector<U2Region> safeLocation = inputAnn->getRegions();
    U2Region::bound(0, inputSeq.length(), safeLocation);
    QList<QByteArray> resParts = U1SequenceUtils::extractRegions(inputSeq.seq, safeLocation, complT, NULL, inputSeq.circular);
    if (aminoT == NULL) { // extension does not work for translated annotations
        if (cfg.extLeft > 0) {
            int annStart = safeLocation.first().startPos;
            int preStart = qMax(0, annStart - cfg.extLeft);
            int preLen = annStart - preStart;
            QByteArray preSeq = inputSeq.seq.mid(preStart, preLen);
            resParts.prepend(preSeq);

            for (int i = 0; i < safeLocation.size(); ++i) {
                safeLocation[i].startPos -= preLen;
                safeLocation[i].length += preLen;
            }
        }
        if (cfg.extRight) {
            U2Region annRegion = U2Region::containingRegion(safeLocation);
            int annEnd = annRegion.endPos();
            int postEnd = qMin(inputSeq.length(), annEnd + cfg.extRight);
            int postLen = postEnd - annEnd;
            QByteArray postSeq = inputSeq.seq.mid(annEnd, postLen);
            resParts.append(postSeq);

            for (int i = 0; i < safeLocation.size(); ++i) {
                safeLocation[i].length += postLen;
            }
        }
    } else {
        resParts = U1SequenceUtils::translateRegions(resParts, aminoT, inputAnn->isJoin());
    }
    foreach(const QByteArray &seq, resParts) {
        bool onlyOneIteration = false;
        DNASequence s;
        s.info[DNAInfo::ID] = inputSeq.getName();
        if (!cfg.splitJoined || resParts.size() == 1) {
            s.seq = resParts.size() == 1 ? resParts.first() : U1SequenceUtils::joinRegions(resParts, cfg.gapLength);
            onlyOneIteration = true;
        } else {
            s.seq = seq;
        }
        s.alphabet = aminoT ? aminoT->getDstAlphabet() : complT ? complT->getDstAlphabet() : inputSeq.alphabet;
        if (aminoT != NULL) {
            s.alphabet = aminoT->getDstAlphabet();
        } else if (complT != NULL) {
            s.alphabet = complT->getDstAlphabet();
        } else {
            s.alphabet = inputSeq.alphabet;
        }
        resultedSeqList.append(s);
        if (onlyOneIteration) {
            break;
        }
    }
}

const QList<DNASequence>& ExtractAnnotatedRegionTask::getResultedSequences() const {
    return resultedSeqList;
}

const SharedAnnotationData& ExtractAnnotatedRegionTask::getInputAnnotation() const {
    return inputAnn;
}


} // U2
