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

#include "RepeatFinderTaskFactoryRegistry.h"

#include <QMutexLocker>
#include <QStringList>


namespace U2 {

RepeatFinderTaskFactoryRegistry::RepeatFinderTaskFactoryRegistry(QObject* pOwn):
    QObject(pOwn)
{
}

RepeatFinderTaskFactoryRegistry::~RepeatFinderTaskFactoryRegistry()
{
    foreach (const RepeatFinderTaskFactory* factory, factories) {
        delete factory;
    }
}

bool RepeatFinderTaskFactoryRegistry::registerFactory(
                                                RepeatFinderTaskFactory* factory,
                                                const QString& factoryId)
{
    QMutexLocker locker(&mutex);
    if (factories.contains(factoryId)) {
        return false;
    }
    factories[factoryId] = factory;
    return true;
}

RepeatFinderTaskFactory* RepeatFinderTaskFactoryRegistry::getFactory(const QString& factoryId) {
    return factories.value(factoryId, 0);
}

QStringList RepeatFinderTaskFactoryRegistry::getListFactoryNames() {
    return factories.keys();
}

bool RepeatFinderTaskFactoryRegistry::hadRegistered(const QString& factoryId) {
    return factories.contains(factoryId);
}

} // namespace
