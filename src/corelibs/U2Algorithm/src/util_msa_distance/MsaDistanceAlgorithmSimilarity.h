/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include "BuiltInDistanceAlgorithms.h"
#include "MsaDistanceAlgorithm.h"

namespace U2 {

// Hamming algorithm is based on Hamming distance between sequences
// weight schemes is the following:
// w("A", "T") = 0
// w("A", "-") = w ("-", "A") = 0
// w("-", "-") = 1 or 0 (depends on "Exclude gaps" option)
// w("A", "A") = 1

class U2ALGORITHM_EXPORT MsaDistanceAlgorithmFactorySimilarity : public MsaDistanceAlgorithmFactory {
    Q_OBJECT
public:
    MsaDistanceAlgorithmFactorySimilarity(QObject* p = nullptr);

    MsaDistanceAlgorithm* createAlgorithm(const Msa& ma, QObject* parent) override;

    QString getDescription() const override;

    QString getName() const override;
};

class U2ALGORITHM_EXPORT MsaDistanceAlgorithmSimilarity : public MsaDistanceAlgorithm {
    Q_OBJECT
public:
    MsaDistanceAlgorithmSimilarity(MsaDistanceAlgorithmFactorySimilarity* f, const Msa& ma)
        : MsaDistanceAlgorithm(f, ma) {
        isSimilarity = true;
    }

    void run() override;
};

}  // namespace U2
