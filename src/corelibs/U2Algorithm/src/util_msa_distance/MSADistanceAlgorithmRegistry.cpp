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

#include "MSADistanceAlgorithmRegistry.h"

#include "MSADistanceAlgorithmHamming.h"
#include "MSADistanceAlgorithmSimilarity.h"
#include "MSADistanceAlgorithmHammingRevCompl.h"

namespace U2 {

MSADistanceAlgorithmRegistry::MSADistanceAlgorithmRegistry(QObject* p) : QObject(p) {
    addAlgorithm(new MSADistanceAlgorithmFactoryHamming());
    //addAlgorithm(new MSADistanceAlgorithmFactoryHammingRevCompl());
    addAlgorithm(new MSADistanceAlgorithmFactorySimilarity());
}

MSADistanceAlgorithmRegistry::~MSADistanceAlgorithmRegistry() {
    QList<MSADistanceAlgorithmFactory*> list = algorithms.values();
    foreach(MSADistanceAlgorithmFactory* algo, list) {
        delete algo;
    }
}

MSADistanceAlgorithmFactory* MSADistanceAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}


void MSADistanceAlgorithmRegistry::addAlgorithm(MSADistanceAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    MSADistanceAlgorithmFactory* oldVersion = algorithms.value(id);
    if (oldVersion!=NULL) {
        delete oldVersion;
        oldVersion = NULL;
    }
    algorithms[id] = algo;
}

QStringList MSADistanceAlgorithmRegistry::getAlgorithmIds() const  {
    QList<MSADistanceAlgorithmFactory*> list = algorithms.values();
    QStringList result;
    foreach(MSADistanceAlgorithmFactory* algo, list) {
        result.append(algo->getId());
    }
    return result;
}

QList<MSADistanceAlgorithmFactory*> MSADistanceAlgorithmRegistry::getAlgorithmFactories(DistanceAlgorithmFlags flags) const {
    QList<MSADistanceAlgorithmFactory*> allFactories = getAlgorithmFactories();
    QList<MSADistanceAlgorithmFactory*> result;
    foreach(MSADistanceAlgorithmFactory* f, allFactories) {
        if ((flags & f->getFlags()) == flags) {
            result.append(f);
        }
    }
    return result;
}

}//namespace
