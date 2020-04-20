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

#include "NWAligner.h"

#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>


namespace U2 {

NWAligner::NWAligner(const QByteArray &seq1, const QByteArray &seq2)
: PairwiseAligner(seq1, seq2), fMatrix(NULL)
{
    GTIMER(cvar, tvar, "NWAligner::NWAligner");
    const DNAAlphabet *alphabet = U2AlphabetUtils::findBestAlphabet(seq1 + seq2);
    if (alphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()) {
        alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    }
    QList<SMatrix> matrixList = AppContext::getSubstMatrixRegistry()->selectMatricesByAlphabet(alphabet);
    if (matrixList.size() > 0) {
        sMatrix = matrixList.first();
    } else {
        sMatrix = AppContext::getSubstMatrixRegistry()->getMatrices().first();
    }
}

NWAligner::~NWAligner() {
    GTIMER(cvar, tvar, "NWAligner::~NWAligner");
    delete fMatrix;
}

void NWAligner::reassignSMatrixByAlphabet(const QByteArray &newSeq) {
    GTIMER(cvar, tvar, "NWAligner::reassignSMatrixByAlphabet");
    const DNAAlphabet *alphabet = U2AlphabetUtils::findBestAlphabet(newSeq);
    const DNAAlphabet *newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(alphabet, sMatrix.getAlphabet());
    if (newAlphabet != sMatrix.getAlphabet()) {
        sMatrix = AppContext::getSubstMatrixRegistry()->selectMatricesByAlphabet(newAlphabet).first();
    }
}

void NWAligner::setSeq1(const QByteArray &value) {
    PairwiseAligner::setSeq1(value);
    reassignSMatrixByAlphabet(value);
}

void NWAligner::setSeq2(const QByteArray &value) {
    PairwiseAligner::setSeq2(value);
    reassignSMatrixByAlphabet(value);
}

void NWAligner::setSeqs(const QByteArray &value1, const QByteArray &value2) {
    PairwiseAligner::setSeqs(value1, value2);
    reassignSMatrixByAlphabet(value1 + value2);
}

MultipleSequenceAlignment NWAligner::align() {
    GTIMER(cvar, tvar, "NWAligner::align");
    float gapPenalty = -5;

    delete fMatrix;
    fMatrix = new FMatrix(sMatrix, gapPenalty);
    fMatrix->calculate(seq1, seq2);

    QByteArray aligned1;
    QByteArray aligned2;
    int i = seq1.size();
    int j = seq2.size();

    while (i>0 && j>0) {
        float score = fMatrix->getFValue(i, j);
        float scoreDiag = fMatrix->getFValue(i-1, j-1);
        float scoreUp = fMatrix->getFValue(i, j-1);
        float scoreLeft = fMatrix->getFValue(i-1, j);
        Q_UNUSED(scoreUp);

        if (score == scoreDiag + sMatrix.getScore(seq1[i-1], seq2[j-1])) {
            aligned1.prepend(seq1[i-1]);
            aligned2.prepend(seq2[j-1]);
            i--; j--;
        } else if (score == scoreLeft + gapPenalty) {
            aligned1.prepend(seq1[i-1]);
            aligned2.prepend(U2Msa::GAP_CHAR);
            i--;
        } else {
            assert(score == scoreUp + gapPenalty);
            aligned1.prepend(U2Msa::GAP_CHAR);
            aligned2.prepend(seq2[j-1]);
            j--;
        }
    }

    while (i>0) {
        aligned1.prepend(seq1[i-1]);
        aligned2.prepend(U2Msa::GAP_CHAR);
        i--;
    }

    while (j>0) {
        aligned1.prepend(U2Msa::GAP_CHAR);
        aligned2.prepend(seq2[j-1]);
        j--;
    }

    MultipleSequenceAlignment result(MA_OBJECT_NAME, sMatrix.getAlphabet());
    result->addRow("seq1", aligned1);
    result->addRow("seq2", aligned2);

    return result;
}

/************************************************************************/
/* FMatrix */
/************************************************************************/
FMatrix::FMatrix(const SMatrix &_sMatrix, float _gapPenalty)
: sMatrix(_sMatrix), gapPenalty(_gapPenalty), f(NULL), h(0), w(0)
{
    GTIMER(cvar, tvar, "FMatrix::FMatrix");
}

FMatrix::~FMatrix() {
    GTIMER(cvar, tvar, "FMatrix::~FMatrix");
    cleanup();
}

void FMatrix::calculate(const QByteArray &seq1, const QByteArray &seq2) {
    GTIMER(cvar, tvar, "FMatrix::calculate");
    init(seq1, seq2);

    for (int i=0; i<h; i++) {
        setFValue(i, 0, gapPenalty * i);
    }
    for (int j=0; j<w; j++) {
        setFValue(0, j, gapPenalty * j);
    }

    for (int i = 1; i < h; i++) {
        for (int j = 1; j < w; j++) {
            float match = getFValue(i-1, j-1) + sMatrix.getScore(seq1[i-1], seq2[j-1]);
            float del = getFValue(i-1, j) + gapPenalty;
            float ins = getFValue(i, j-1) + gapPenalty;

            float max = qMax(match, qMax(del, ins));
            setFValue(i, j, max);
        }
    }
}

void FMatrix::cleanup() {
    if (NULL == f) {
        return;
    }
    delete f;
    f = NULL;
}

void FMatrix::init(const QByteArray &seq1, const QByteArray &seq2) {
    cleanup();
    h = seq1.size() + 1;
    w = seq2.size() + 1;

    f = new float[h*w];
}

inline void FMatrix::setFValue(int i, int j, float v) {
    f[i*w + j] = v;
}

inline float FMatrix::getFValue(int i, int j) {
    return f[i*w + j];
}

} // U2
