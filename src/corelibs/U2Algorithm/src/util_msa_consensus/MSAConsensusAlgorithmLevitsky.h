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

#include <QVarLengthArray>
#include <QList>

#include "BuiltInConsensusAlgorithms.h"
#include "MSAConsensusAlgorithm.h"

namespace U2 {

// DNA/RNA only consensus, that tries to
//    1) switch to extended nucleic alphabet when there is no 100% match
//    2) uses threshold score and selects the most rare (in whole alignment) symbol that has greater score
class U2ALGORITHM_EXPORT MSAConsensusAlgorithmFactoryLevitsky : public MSAConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MSAConsensusAlgorithmFactoryLevitsky(QObject* p = nullptr);

    MSAConsensusAlgorithm* createAlgorithm(const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps) override;

    QString getDescription() const override;

    QString getName() const override;

    int getMinThreshold() const override {
        return 50;
    }

    int getMaxThreshold() const override {
        return 100;
    }

    int getDefaultThreshold() const override {
        return 90;
    }

    QString getThresholdSuffix() const override {
        return "%";
    }

    bool isSequenceLikeResult() const override {
        return true;
    }
};

class U2ALGORITHM_EXPORT MSAConsensusAlgorithmLevitsky : public MSAConsensusAlgorithm {
    Q_OBJECT
public:
    MSAConsensusAlgorithmLevitsky(MSAConsensusAlgorithmFactoryLevitsky* f, const MultipleAlignment& ma, bool ignoreTrailingLeadingGaps);

    char getConsensusChar(const MultipleAlignment& ma, int column) const override;

    MSAConsensusAlgorithmLevitsky* clone() const override;

    void reinitializeData(const MultipleAlignment& ma) override;

    void recalculateData(const MultipleAlignment& oldMa, const MultipleAlignment& newMa, const MaModificationInfo& mi) override;

private:
    QVarLengthArray<int> globalFreqs;
};

}  // namespace U2
