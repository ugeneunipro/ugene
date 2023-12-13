/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "BuiltInConsensusAlgorithms.h"
#include "MSAConsensusAlgorithm.h"

namespace U2 {

// Default algorithm is based on JalView experience. It also makes letters with low threshold lowercase
class U2ALGORITHM_EXPORT MSAConsensusAlgorithmFactoryDefault : public MSAConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MSAConsensusAlgorithmFactoryDefault(QObject* p = nullptr);

    MSAConsensusAlgorithm* createAlgorithm(const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps, QObject* parent) override;

    QString getDescription() const override;

    QString getName() const override;

    int getMinThreshold() const override {
        return 1;
    }

    int getMaxThreshold() const override {
        return 100;
    }

    int getDefaultThreshold() const override {
        return 100;
    }

    QString getThresholdSuffix() const override {
        return "%";
    }

    bool isSequenceLikeResult() const override {
        return false;
    }
};

class U2ALGORITHM_EXPORT MSAConsensusAlgorithmDefault : public MSAConsensusAlgorithm {
    Q_OBJECT
public:
    MSAConsensusAlgorithmDefault(MSAConsensusAlgorithmFactoryDefault* f, bool ignoreTrailingLeadingGaps, QObject* p = nullptr)
        : MSAConsensusAlgorithm(f, ignoreTrailingLeadingGaps, p) {
    }

    char getConsensusChar(const MultipleAlignment& ma, int column, QVector<int> seqIdx) const override {
        int countStub = 0;
        return getConsensusCharAndScore(ma, column, countStub, seqIdx);
    }

    MSAConsensusAlgorithmDefault* clone() const override;

    char getConsensusCharAndScore(const MultipleAlignment& ma, int column, int& score, QVector<int> seqIdx) const override;
};

}  // namespace U2
