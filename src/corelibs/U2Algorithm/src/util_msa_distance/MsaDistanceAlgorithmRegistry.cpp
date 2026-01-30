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

#include "MsaDistanceAlgorithmRegistry.h"

#include "MsaDistanceAlgorithmHamming.h"
#include "MsaDistanceAlgorithmHammingRevCompl.h"
#include "MsaDistanceAlgorithmSimilarity.h"

namespace U2 {

MsaDistanceAlgorithmRegistry::MsaDistanceAlgorithmRegistry(QObject* p)
    : QObject(p) {
    addAlgorithm(new MsaDistanceAlgorithmFactoryHamming());
    // addAlgorithm(new MSADistanceAlgorithmFactoryHammingRevCompl());
    addAlgorithm(new MsaDistanceAlgorithmFactorySimilarity());
}

MsaDistanceAlgorithmRegistry::~MsaDistanceAlgorithmRegistry() {
    QList<MsaDistanceAlgorithmFactory*> list = algorithms.values();
    foreach (MsaDistanceAlgorithmFactory* algo, list) {
        delete algo;
    }
}

MsaDistanceAlgorithmFactory* MsaDistanceAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}

void MsaDistanceAlgorithmRegistry::addAlgorithm(MsaDistanceAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    MsaDistanceAlgorithmFactory* oldVersion = algorithms.value(id);
    if (oldVersion != nullptr) {
        delete oldVersion;
        oldVersion = nullptr;
    }
    algorithms[id] = algo;
}

QStringList MsaDistanceAlgorithmRegistry::getAlgorithmIds() const {
    QList<MsaDistanceAlgorithmFactory*> list = algorithms.values();
    QStringList result;
    foreach (MsaDistanceAlgorithmFactory* algo, list) {
        result.append(algo->getId());
    }
    return result;
}

QList<MsaDistanceAlgorithmFactory*> MsaDistanceAlgorithmRegistry::getAlgorithmFactories(DistanceAlgorithmFlags flags) const {
    QList<MsaDistanceAlgorithmFactory*> allFactories = getAlgorithmFactories();
    QList<MsaDistanceAlgorithmFactory*> result;
    foreach (MsaDistanceAlgorithmFactory* f, allFactories) {
        if ((flags & f->getFlags()) == flags) {
            result.append(f);
        }
    }
    return result;
}

}  // namespace U2
