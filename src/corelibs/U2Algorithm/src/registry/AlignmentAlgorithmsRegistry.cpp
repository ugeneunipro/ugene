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

#include "AlignmentAlgorithmsRegistry.h"

#include <QMutexLocker>

#include <U2Core/U2SafePoints.h>

#include <U2View/AlignmentAlgorithmGUIExtension.h>

#include "msa_alignment/SimpleAddingToAlignment.h"

namespace U2 {

AlignmentAlgorithmsRegistry::AlignmentAlgorithmsRegistry(QObject* parent)
    : QObject(parent) {
    registerAlgorithm(new SimpleAddToAlignmentAlgorithm());
}

AlignmentAlgorithmsRegistry::~AlignmentAlgorithmsRegistry() {
    qDeleteAll(algorithms.values());
}

bool AlignmentAlgorithmsRegistry::registerAlgorithm(AlignmentAlgorithm* algorithm) {
    QMutexLocker locker(&mutex);

    if (algorithms.contains(algorithm->getId())) {
        return false;
    }
    algorithms.insert(algorithm->getId(), algorithm);
    return true;
}

AlignmentAlgorithm* AlignmentAlgorithmsRegistry::getAlgorithm(const QString& id) const {
    QMutexLocker locker(&mutex);
    return algorithms.value(id, nullptr);
}

QStringList AlignmentAlgorithmsRegistry::getAvailableAlgorithmIds(AlignmentAlgorithmType type) const {
    QStringList selectedAlgorithmIds;
    foreach (const AlignmentAlgorithm* algorithm, algorithms) {
        if (algorithm->getAlignmentType() == type && algorithm->isAlgorithmAvailable()) {
            selectedAlgorithmIds.append(algorithm->getId());
        }
    }
    return selectedAlgorithmIds;
}

AlignmentAlgorithm::AlignmentAlgorithm(AlignmentAlgorithmType alignmentType, const QString& _id, const QString& _actionName, AbstractAlignmentTaskFactory* tf, AlignmentAlgorithmGUIExtensionFactory* guif, const QString& _realizationId)
    : id(_id), actionName(_actionName), alignmentType(alignmentType) {
    realizations.insert(_realizationId, new AlgorithmRealization(_realizationId, tf, guif));
}

AlignmentAlgorithm::~AlignmentAlgorithm() {
    qDeleteAll(realizations);
}

AbstractAlignmentTaskFactory* AlignmentAlgorithm::getFactory(const QString& _realizationId) const {
    QMutexLocker locker(&mutex);
    SAFE_POINT(realizations.contains(_realizationId), "Realization is not registered", nullptr);
    return realizations.value(_realizationId)->getTaskFactory();
}

AlignmentAlgorithmGUIExtensionFactory* AlignmentAlgorithm::getGUIExtFactory(const QString& _realizationId) const {
    QMutexLocker locker(&mutex);
    SAFE_POINT(realizations.keys().contains(_realizationId), "Realization is not registered", nullptr);
    AlgorithmRealization* algReal = realizations.value(_realizationId, nullptr);
    return algReal->getGUIExtFactory();
}

const QString& AlignmentAlgorithm::getId() const {
    return id;
}

const QString& AlignmentAlgorithm::getActionName() const {
    return actionName;
}

QStringList AlignmentAlgorithm::getRealizationsList() const {
    QMutexLocker locker(&mutex);
    return realizations.keys();
}

bool AlignmentAlgorithm::addAlgorithmRealization(AbstractAlignmentTaskFactory* tf, AlignmentAlgorithmGUIExtensionFactory* guif, const QString& _realizationId) {
    QMutexLocker locker(&mutex);
    if (realizations.keys().contains(_realizationId)) {
        return false;
    }
    realizations.insert(_realizationId, new AlgorithmRealization(_realizationId, tf, guif));
    return true;
}

AlgorithmRealization* AlignmentAlgorithm::getAlgorithmRealization(const QString& _realizationId) const {
    QMutexLocker locker(&mutex);
    return realizations.value(_realizationId, nullptr);
}

bool AlignmentAlgorithm::isAlgorithmAvailable() const {
    return true;
}

AlignmentAlgorithmType AlignmentAlgorithm::getAlignmentType() const {
    return alignmentType;
}

AlgorithmRealization::AlgorithmRealization(const QString& _realizationId, AbstractAlignmentTaskFactory* tf, AlignmentAlgorithmGUIExtensionFactory* guif)
    : realizationId(_realizationId), alignmentAlgorithmTaskFactory(tf), alignmentAlgorithmGUIExtensionsFactory(guif) {
}

AlgorithmRealization::~AlgorithmRealization() {
    delete alignmentAlgorithmTaskFactory;
    delete alignmentAlgorithmGUIExtensionsFactory;
}

const QString& AlgorithmRealization::getRealizationId() const {
    return realizationId;
}

AbstractAlignmentTaskFactory* AlgorithmRealization::getTaskFactory() const {
    return alignmentAlgorithmTaskFactory;
}

AlignmentAlgorithmGUIExtensionFactory* AlgorithmRealization::getGUIExtFactory() const {
    return alignmentAlgorithmGUIExtensionsFactory;
}

}  // namespace U2
