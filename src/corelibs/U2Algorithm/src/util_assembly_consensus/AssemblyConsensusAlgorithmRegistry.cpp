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

#include "AssemblyConsensusAlgorithmRegistry.h"

#include "AssemblyConsensusAlgorithmDefault.h"
#include "AssemblyConsensusAlgorithmSamtools.h"

namespace U2 {

AssemblyConsensusAlgorithmRegistry::AssemblyConsensusAlgorithmRegistry() {
    addAlgorithmFactory(new AssemblyConsensusAlgorithmFactoryDefault());
    addAlgorithmFactory(new AssemblyConsensusAlgorithmFactorySamtools());
}

AssemblyConsensusAlgorithmRegistry::~AssemblyConsensusAlgorithmRegistry() {
    qDeleteAll(getAlgorithmFactories());
}

AssemblyConsensusAlgorithmFactory* AssemblyConsensusAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}

void AssemblyConsensusAlgorithmRegistry::addAlgorithmFactory(AssemblyConsensusAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    AssemblyConsensusAlgorithmFactory* oldVersion = algorithms.value(id);
    if (oldVersion!=NULL) {
        delete oldVersion;
        oldVersion = NULL;
    }
    algorithms[id] = algo;
}

}//namespace
