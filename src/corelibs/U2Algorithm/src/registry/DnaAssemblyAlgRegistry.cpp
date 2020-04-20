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

#include "DnaAssemblyAlgRegistry.h"

#include <U2Algorithm/DnaAssemblyTask.h>
#include <U2View/DnaAssemblyGUIExtension.h>

namespace U2 {

DnaAssemblyAlgorithmEnv::DnaAssemblyAlgorithmEnv(
    const QString &id,
    DnaAssemblyToRefTaskFactory *taskFactory,
    DnaAssemblyGUIExtensionsFactory *guiExtFactory,
    bool supportsIndexFiles,
    bool supportsDbi,
    bool supportsPEReads,
    const QStringList &refrerenceFormats,
    const QStringList &readsFormats)
: id(id), taskFactory(taskFactory), guiExtFactory(guiExtFactory),
supportsIndexFiles(supportsIndexFiles), supportsDbi(supportsDbi),
supportsPEReads(supportsPEReads), refrerenceFormats(refrerenceFormats),
readsFormats(readsFormats)
{

}

DnaAssemblyAlgorithmEnv::~DnaAssemblyAlgorithmEnv() {
    delete taskFactory;
    delete guiExtFactory;
}

DnaAssemblyAlgRegistry::DnaAssemblyAlgRegistry( QObject* pOwn /* = 0*/ ) : QObject(pOwn) {
}

DnaAssemblyAlgRegistry::~DnaAssemblyAlgRegistry() {
    foreach( DnaAssemblyAlgorithmEnv* algo, algorithms.values()) {
        delete algo;
    }
}

bool DnaAssemblyAlgRegistry::registerAlgorithm(DnaAssemblyAlgorithmEnv* algo) {
    QMutexLocker locker(&mutex);

    if (algorithms.contains(algo->getId())){
        return false;
    }
    algorithms.insert(algo->getId(), algo);
    return true;

}

DnaAssemblyAlgorithmEnv* DnaAssemblyAlgRegistry::unregisterAlgorithm(const QString& id) {
    QMutexLocker locker(&mutex);

    if (!algorithms.contains(id)) {
        return NULL;
    }
    DnaAssemblyAlgorithmEnv* res = algorithms.value(id);
    algorithms.remove(id);
    return res;
}

DnaAssemblyAlgorithmEnv* DnaAssemblyAlgRegistry::getAlgorithm( const QString& id) const {
    QMutexLocker locker(&mutex);
    return algorithms.value(id);
}


QStringList DnaAssemblyAlgRegistry::getRegisteredAlgorithmIds() const {
    return algorithms.keys();
}

QStringList DnaAssemblyAlgRegistry::getRegisteredAlgorithmsWithIndexFileSupport() const {
    QStringList result;
    foreach( DnaAssemblyAlgorithmEnv* algo, algorithms.values()) {
        if (algo->isIndexFilesSupported()) {
            result << algo->getId();
        }
    }
    return result;
}

} //namespace

