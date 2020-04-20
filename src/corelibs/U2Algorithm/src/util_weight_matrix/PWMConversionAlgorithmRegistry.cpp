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

#include "PWMConversionAlgorithmRegistry.h"

#include "PWMConversionAlgorithmBVH.h"
#include "PWMConversionAlgorithmLOD.h"
#include "PWMConversionAlgorithmMCH.h"
#include "PWMConversionAlgorithmNLG.h"

namespace U2 {

PWMConversionAlgorithmRegistry::PWMConversionAlgorithmRegistry(QObject* p) : QObject(p) {
    addAlgorithm(new PWMConversionAlgorithmFactoryBVH());
    addAlgorithm(new PWMConversionAlgorithmFactoryLOD());
    addAlgorithm(new PWMConversionAlgorithmFactoryMCH());
    addAlgorithm(new PWMConversionAlgorithmFactoryNLG());
}

PWMConversionAlgorithmRegistry::~PWMConversionAlgorithmRegistry() {
    QList<PWMConversionAlgorithmFactory*> list = algorithms.values();
    foreach(PWMConversionAlgorithmFactory* algo, list) {
        delete algo;
    }
}

PWMConversionAlgorithmFactory* PWMConversionAlgorithmRegistry::getAlgorithmFactory(const QString& algoId) {
    return algorithms.value(algoId);
}


void PWMConversionAlgorithmRegistry::addAlgorithm(PWMConversionAlgorithmFactory* algo) {
    const QString& id = algo->getId();
    PWMConversionAlgorithmFactory* oldVersion = algorithms.value(id);
    if (oldVersion!=NULL) {
        delete oldVersion;
        oldVersion = NULL;
    }
    algorithms[id] = algo;
}

QStringList PWMConversionAlgorithmRegistry::getAlgorithmIds() const  {
    QList<PWMConversionAlgorithmFactory*> list = algorithms.values();
    QStringList result;
    foreach(PWMConversionAlgorithmFactory* algo, list) {
        result.append(algo->getId());
    }
    return result;
}

}//namespace
