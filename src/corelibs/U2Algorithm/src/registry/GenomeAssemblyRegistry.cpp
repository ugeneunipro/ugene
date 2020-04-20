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

#include "GenomeAssemblyRegistry.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

GenomeAssemblyTask::GenomeAssemblyTask( const GenomeAssemblyTaskSettings& s, TaskFlags _flags)
    : Task("GenomeAssemblyTask", _flags), settings(s), resultUrl(""){
}

QString GenomeAssemblyTask::getResultUrl() const{
    return resultUrl;
}

QVariant GenomeAssemblyTaskSettings::getCustomValue( const QString& optionName, const QVariant& defaultVal ) const {
    if (customSettings.contains(optionName)) {
        return customSettings.value(optionName);
    } else {
        return defaultVal;
    }
}

bool GenomeAssemblyTaskSettings::hasCustomValue(const QString & name) const {
    return customSettings.contains(name);
}

void GenomeAssemblyTaskSettings::setCustomValue( const QString& optionName, const QVariant& val ) {
    customSettings.insert(optionName,val);
}

void GenomeAssemblyTaskSettings::setCustomSettings( const QMap<QString, QVariant>& settings ) {
    customSettings = settings;
}

GenomeAssemblyAlgorithmEnv::GenomeAssemblyAlgorithmEnv(
    const QString &id,
    GenomeAssemblyTaskFactory *taskFactory,
    GenomeAssemblyGUIExtensionsFactory *guiExtFactory,
    const QStringList &readsFormats)
: id(id), taskFactory(taskFactory), guiExtFactory(guiExtFactory),
readsFormats(readsFormats)
{

}

GenomeAssemblyAlgorithmEnv::~GenomeAssemblyAlgorithmEnv() {
    delete taskFactory;
    delete guiExtFactory;
}

GenomeAssemblyAlgRegistry::GenomeAssemblyAlgRegistry( QObject* pOwn /* = 0*/ ) : QObject(pOwn) {
}

GenomeAssemblyAlgRegistry::~GenomeAssemblyAlgRegistry() {
    foreach( GenomeAssemblyAlgorithmEnv* algo, algorithms.values()) {
        delete algo;
    }
}

bool GenomeAssemblyAlgRegistry::registerAlgorithm(GenomeAssemblyAlgorithmEnv* algo) {
    QMutexLocker locker(&mutex);

    if (algorithms.contains(algo->getId())){
        return false;
    }
    algorithms.insert(algo->getId(), algo);
    return true;

}

GenomeAssemblyAlgorithmEnv* GenomeAssemblyAlgRegistry::unregisterAlgorithm(const QString& id) {
    QMutexLocker locker(&mutex);

    if (!algorithms.contains(id)) {
        return NULL;
    }
    GenomeAssemblyAlgorithmEnv* res = algorithms.value(id);
    algorithms.remove(id);
    return res;
}

GenomeAssemblyAlgorithmEnv* GenomeAssemblyAlgRegistry::getAlgorithm( const QString& id) const {
    QMutexLocker locker(&mutex);
    return algorithms.value(id);
}


QStringList GenomeAssemblyAlgRegistry::getRegisteredAlgorithmIds() const {
    return algorithms.keys();
}

QStringList GenomeAssemblyUtils::getOrientationTypes(){
    return QStringList() << ORIENTATION_FR << ORIENTATION_RF << ORIENTATION_FF;
}

bool GenomeAssemblyUtils::isLibraryPaired(const QString& libName){
    return (libName == LIB_PAIR_DEFAULT ||
            libName == LIB_PAIR_MATE ||
            libName == LIB_PAIR_MATE_HQ);
}

} //namespace
