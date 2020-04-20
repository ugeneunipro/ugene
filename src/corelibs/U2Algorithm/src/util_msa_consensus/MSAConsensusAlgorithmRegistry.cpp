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

#include "MaConsensusAlgorithmSimpleExtended.h"
#include "MSAConsensusAlgorithmClustal.h"
#include "MSAConsensusAlgorithmDefault.h"
#include "MSAConsensusAlgorithmLevitsky.h"
#include "MSAConsensusAlgorithmRegistry.h"
#include "MSAConsensusAlgorithmStrict.h"

namespace U2 {

MSAConsensusAlgorithmRegistry::MSAConsensusAlgorithmRegistry(QObject* p) : QObject(p) {
    addAlgorithm(new MSAConsensusAlgorithmFactoryDefault());
    addAlgorithm(new MSAConsensusAlgorithmFactoryStrict());
    addAlgorithm(new MSAConsensusAlgorithmFactoryClustal());
    addAlgorithm(new MSAConsensusAlgorithmFactoryLevitsky());
    addAlgorithm(new MaConsensusAlgorithmFactorySimpleExtended());
}

MSAConsensusAlgorithmRegistry::~MSAConsensusAlgorithmRegistry() {
    QList<MSAConsensusAlgorithmFactory*> list = algorithms.values();
    foreach(MSAConsensusAlgorithmFactory* algo, list) {
        delete algo;
    }
}

MSAConsensusAlgorithmFactory* MSAConsensusAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}


void MSAConsensusAlgorithmRegistry::addAlgorithm(MSAConsensusAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    MSAConsensusAlgorithmFactory* oldVersion = algorithms.value(id);
    if (oldVersion!=NULL) {
        delete oldVersion;
        oldVersion = NULL;
    }
    algorithms[id] = algo;
}

QStringList MSAConsensusAlgorithmRegistry::getAlgorithmIds() const  {
    QList<MSAConsensusAlgorithmFactory*> list = algorithms.values();
    QStringList result;
    foreach(MSAConsensusAlgorithmFactory* algo, list) {
        result.append(algo->getId());
    }
    return result;
}

QList<MSAConsensusAlgorithmFactory*> MSAConsensusAlgorithmRegistry::getAlgorithmFactories(ConsensusAlgorithmFlags flags) const {
    QList<MSAConsensusAlgorithmFactory*> allFactories = getAlgorithmFactories();
    QList<MSAConsensusAlgorithmFactory*> result;
    foreach(MSAConsensusAlgorithmFactory* f, allFactories) {
        if ((flags & f->getFlags()) == flags) {
            result.append(f);
        }
    }
    return result;
}

}//namespace
