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

#ifndef _U2_MSA_DISTANCE_ALGORITHM_SIMILARITY_H_
#define _U2_MSA_DISTANCE_ALGORITHM_SIMILARITY_H_

#include "MSADistanceAlgorithm.h"
#include "BuiltInDistanceAlgorithms.h"

namespace U2 {

// Hamming algorithm is based on Hamming distance between sequences
// weight schemes is the following:
// w("A", "T") = 0
// w("A", "-") = w ("-", "A") = 0
// w("-", "-") = 1 or 0 (depends on "Exclude gaps" option)
// w("A", "A") = 1

class U2ALGORITHM_EXPORT MSADistanceAlgorithmFactorySimilarity: public MSADistanceAlgorithmFactory {
    Q_OBJECT
public:
    MSADistanceAlgorithmFactorySimilarity(QObject* p = NULL);

    virtual MSADistanceAlgorithm* createAlgorithm(const MultipleSequenceAlignment& ma, QObject* parent);

    virtual QString getDescription() const;

    virtual QString getName() const;

};


class U2ALGORITHM_EXPORT MSADistanceAlgorithmSimilarity : public MSADistanceAlgorithm {
    Q_OBJECT
public:
    MSADistanceAlgorithmSimilarity(MSADistanceAlgorithmFactorySimilarity* f, const MultipleSequenceAlignment& ma)
        : MSADistanceAlgorithm(f, ma){isSimilarity = true;}

    virtual void run();
};

}//namespace

#endif //_U2_MSA_DISTANCE_ALGORITHM_SIMILARITY_H_
