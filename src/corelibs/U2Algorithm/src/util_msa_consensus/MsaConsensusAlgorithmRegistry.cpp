/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "MsaConsensusAlgorithmRegistry.h"

#include "MaConsensusAlgorithmSimpleExtended.h"
#include "MsaConsensusAlgorithmClustal.h"
#include "MsaConsensusAlgorithmDefault.h"
#include "MsaConsensusAlgorithmLevitsky.h"
#include "MsaConsensusAlgorithmStrict.h"

namespace U2 {

MsaConsensusAlgorithmRegistry::MsaConsensusAlgorithmRegistry(QObject* p)
    : QObject(p) {
    addAlgorithm(new MsaConsensusAlgorithmFactoryDefault());
    addAlgorithm(new MsaConsensusAlgorithmFactoryStrict());
    addAlgorithm(new MsaConsensusAlgorithmFactoryClustal());
    addAlgorithm(new MsaConsensusAlgorithmFactoryLevitsky());
    addAlgorithm(new MaConsensusAlgorithmFactorySimpleExtended());
}

MsaConsensusAlgorithmRegistry::~MsaConsensusAlgorithmRegistry() {
    QList<MsaConsensusAlgorithmFactory*> list = algorithms.values();
    foreach (MsaConsensusAlgorithmFactory* algo, list) {
        delete algo;
    }
}

MsaConsensusAlgorithmFactory* MsaConsensusAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}

void MsaConsensusAlgorithmRegistry::addAlgorithm(MsaConsensusAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    MsaConsensusAlgorithmFactory* oldVersion = algorithms.value(id);
    delete oldVersion;
    algorithms[id] = algo;
}

QStringList MsaConsensusAlgorithmRegistry::getAlgorithmIds() const {
    QList<MsaConsensusAlgorithmFactory*> list = algorithms.values();
    QStringList result;
    foreach (MsaConsensusAlgorithmFactory* algo, list) {
        result.append(algo->getId());
    }
    return result;
}

QList<MsaConsensusAlgorithmFactory*> MsaConsensusAlgorithmRegistry::getAlgorithmFactories(ConsensusAlgorithmFlags flags) const {
    QList<MsaConsensusAlgorithmFactory*> allFactories = getAlgorithmFactories();
    QList<MsaConsensusAlgorithmFactory*> result;
    foreach (MsaConsensusAlgorithmFactory* f, allFactories) {
        if ((flags & f->getFlags()) == flags) {
            result.append(f);
        }
    }
    return result;
}

}  // namespace U2
