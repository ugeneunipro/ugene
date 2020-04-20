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

#ifndef _SMITHWATERMANALGORITHM_H
#define _SMITHWATERMANALGORITHM_H

#include "PairAlignSequences.h"

#include <U2Core/SMatrix.h>
#include <U2Algorithm/SmithWatermanSettings.h>

#include <QVector>
#include <QByteArray>

namespace U2 {

class SmithWatermanAlgorithm {

public:
    SmithWatermanAlgorithm();
    virtual ~SmithWatermanAlgorithm() {}

    virtual void launch(const SMatrix& m, const QByteArray  & _patternSeq, const QByteArray & _searchSeq,
        int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView _resultView);

    QList<PairAlignSequences> getResults();
    static void sortByScore(QList<PairAlignSequences> & pairAlignmentStrings);
    static quint64 estimateNeededRamAmount(const qint32 gapOpen, const qint32 gapExtension,
                                           const quint32 minScore, const quint32 maxScore,
                                           const QByteArray & patternSeq, const QByteArray & searchSeq,
                                           const SmithWatermanSettings::SWResultView resultView);
    static const char STOP;
    static const char UP;
    static const char LEFT;
    static const char DIAG;

protected:
    bool calculateMatrixLength();
    bool isValidParams();
    void setValues(const SMatrix& _substitutionMatrix,
        const QByteArray & _patternSeq, const QByteArray & _searchSeq,
        int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView _resultView);

    QList<PairAlignSequences> pairAlignmentStrings;

    SMatrix substitutionMatrix;

    QByteArray patternSeq;
    QByteArray searchSeq;

    int gapOpen;
    int gapExtension;
    int minScore;
    int matrixLength;
    SmithWatermanSettings::SWResultView resultView;

    QVector<QVector<char> > directionMatrix;

    struct KeyOfPairAlignSeq
    {
        KeyOfPairAlignSeq(int _score, U2Region const & _intervalSeq1) {
            setValues(_score, _intervalSeq1);
        }

        KeyOfPairAlignSeq() {
            score = 0;

            intervalSeq1.startPos = 0;
            intervalSeq1.length = 0;
        };

        static void exchange(PairAlignSequences & a, PairAlignSequences & b) {
            PairAlignSequences bufKey;

            bufKey = a;
            a = b;
            b = bufKey;
        }

        void setValues(int _score, U2Region const & _intervalSeq1) {
            score = _score;
            intervalSeq1 = _intervalSeq1;
        }

        int score;
        U2Region intervalSeq1;

    };


private:

    QVector<QVector<int> > matrix;
    QVector<int> EMatrix;
    QVector<int> FMatrix;

    void calculateMatrixForMultipleAlignmentResult();
    void calculateMatrixForAnnotationsResult();
};

}//namespace

#endif
