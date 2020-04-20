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

#ifndef _U2_NW_ALIGNER_H_
#define _U2_NW_ALIGNER_H_

#include <U2Algorithm/PairwiseAligner.h>

#include <U2Core/SMatrix.h>


namespace U2 {

class FMatrix;

/**
 * http://en.wikipedia.org/wiki/Needleman%E2%80%93Wunsch_algorithm
 */
class U2ALGORITHM_EXPORT NWAligner : public PairwiseAligner {
public:
    NWAligner(const QByteArray &seq1, const QByteArray &seq2);
    virtual ~NWAligner();

    virtual void setSeq1(const QByteArray &value);
    virtual void setSeq2(const QByteArray &value);
    virtual void setSeqs(const QByteArray &value1, const QByteArray &value2);

    MultipleSequenceAlignment align();

private:
    SMatrix sMatrix;
    FMatrix *fMatrix;

private:
    void reassignSMatrixByAlphabet(const QByteArray &newSeq);
};

class FMatrix {
public:
    FMatrix(const SMatrix &sMatrix, float gapPenalty);
    ~FMatrix();

    void calculate(const QByteArray &seq1, const QByteArray &seq2);
    float getFValue(int i, int j);

private:
    SMatrix sMatrix;
    float gapPenalty;
    float *f;
    int h;
    int w;

private:
    void cleanup();
    void init(const QByteArray &seq1, const QByteArray &seq2);
    void setFValue(int i, int j, float v);
};

} // U2

#endif // _U2_NW_ALIGNER_H_
