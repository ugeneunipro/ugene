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

#include <QVarLengthArray>

#include "BuiltInConsensusAlgorithms.h"
#include "MsaConsensusAlgorithm.h"

namespace U2 {

// DNA/RNA only consensus, that tries to
//    1) switch to extended nucleic alphabet when there is no 100% match
//    2) uses threshold score and selects the most rare (in whole alignment) symbol that has greater score
class U2ALGORITHM_EXPORT MsaConsensusAlgorithmFactoryLevitsky : public MsaConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MsaConsensusAlgorithmFactoryLevitsky();

    MsaConsensusAlgorithm* createAlgorithm(const Msa& ma, bool ignoreTrailingLeadingGaps) override;
};

class U2ALGORITHM_EXPORT MsaConsensusAlgorithmLevitsky : public MsaConsensusAlgorithm {
    Q_OBJECT
public:
    MsaConsensusAlgorithmLevitsky(MsaConsensusAlgorithmFactoryLevitsky* f, const Msa& ma, bool ignoreTrailingLeadingGaps);

    char getConsensusChar(const Msa& ma, int column) const override;

    MsaConsensusAlgorithmLevitsky* clone() const override;

    void reinitializeData(const Msa& ma) override;

private:
    QVarLengthArray<int, 256> globalFreqs;
};

}  // namespace U2
