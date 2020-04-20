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

#ifndef _U2_MSA_DISTANCE_ALGORITHM_HAMMING_COMPL_H_
#define _U2_MSA_DISTANCE_ALGORITHM_HAMMING_COMPL_H_

#include "MSADistanceAlgorithm.h"
#include "BuiltInDistanceAlgorithms.h"

namespace U2 {

// Hamming algorithm is based on Hamming distance between sequences
class U2ALGORITHM_EXPORT MSADistanceAlgorithmFactoryHammingRevCompl: public MSADistanceAlgorithmFactory {
    Q_OBJECT
public:
    MSADistanceAlgorithmFactoryHammingRevCompl(QObject* p = NULL);

    virtual MSADistanceAlgorithm* createAlgorithm(const MultipleSequenceAlignment& ma, QObject* parent);

    virtual QString getDescription() const;

    virtual QString getName() const;

};


class U2ALGORITHM_EXPORT MSADistanceAlgorithmHammingRevCompl : public MSADistanceAlgorithm {
    Q_OBJECT
public:
    MSADistanceAlgorithmHammingRevCompl(MSADistanceAlgorithmFactoryHammingRevCompl* f, const MultipleSequenceAlignment& ma)
        : MSADistanceAlgorithm(f, ma){}

    virtual void run();
};

}//namespace

#endif
