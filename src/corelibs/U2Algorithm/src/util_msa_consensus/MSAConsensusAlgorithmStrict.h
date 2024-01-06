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

// Strict consensus: returns non-gap character only of all characters in the column are equal
class U2ALGORITHM_EXPORT MSAConsensusAlgorithmFactoryStrict : public MSAConsensusAlgorithmFactory {
    Q_OBJECT
public:
    MSAConsensusAlgorithmFactoryStrict();

    MSAConsensusAlgorithm* createAlgorithm(const Msa& ma, bool ignoreTrailingLeadingGaps) override;
};

class U2ALGORITHM_EXPORT MSAConsensusAlgorithmStrict : public MSAConsensusAlgorithm {
    Q_OBJECT
public:
    MSAConsensusAlgorithmStrict(MSAConsensusAlgorithmFactoryStrict* f, bool ignoreTrailingLeadingGaps)
        : MSAConsensusAlgorithm(f, ignoreTrailingLeadingGaps) {
    }

    char getConsensusChar(const Msa& ma, int column) const override;

    MSAConsensusAlgorithmStrict* clone() const override;
};

}  // namespace U2
