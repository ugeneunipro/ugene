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

#include "StructuralAlignmentAlgorithmRegistry.h"
#include "StructuralAlignmentAlgorithmFactory.h"

namespace U2 {

/* class U2ALGORITHM_EXPORT StructuralAlignmentAlgorithmRegistry : public QObject */

StructuralAlignmentAlgorithmRegistry::StructuralAlignmentAlgorithmRegistry(QObject *parent)
    : QObject(parent), factories()
{
}

StructuralAlignmentAlgorithmRegistry::~StructuralAlignmentAlgorithmRegistry() {
    foreach (StructuralAlignmentAlgorithmFactory *f, factories) {
        delete f;
    }
}

void StructuralAlignmentAlgorithmRegistry::registerAlgorithmFactory(StructuralAlignmentAlgorithmFactory *factory, const QString &id) {
    assert(!factories.contains(id));
    factories.insert(id, factory);
}

StructuralAlignmentAlgorithmFactory* StructuralAlignmentAlgorithmRegistry::getAlgorithmFactory(const QString &id) {
    return factories.value(id, 0);
}

QList<QString> StructuralAlignmentAlgorithmRegistry::getFactoriesIds() const {
    return factories.keys();
}

StructuralAlignmentAlgorithm* StructuralAlignmentAlgorithmRegistry::createStructuralAlignmentAlgorithm(const QString &algorithm) {
    return getAlgorithmFactory(algorithm)->create();
}

StructuralAlignmentTask* StructuralAlignmentAlgorithmRegistry::createStructuralAlignmentTask(const QString &algorithm, const StructuralAlignmentTaskSettings &settings) {
    StructuralAlignmentTask *t = new StructuralAlignmentTask(getAlgorithmFactory(algorithm)->create(), settings);
    return t;
}

}   // namespace U2
