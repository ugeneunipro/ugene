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

#include "OPWidgetFactoryRegistry.h"

#include <U2Core/U2SafePoints.h>


namespace U2 {


OPWidgetFactoryRegistry::OPWidgetFactoryRegistry(QObject* parent /* = NULL */)
    : QObject(parent)
{
}


OPWidgetFactoryRegistry::~OPWidgetFactoryRegistry()
{
    foreach (OPWidgetFactory* factory, opWidgetFactories) {
        delete factory;
    }
}


bool OPWidgetFactoryRegistry::registerFactory(OPWidgetFactory* factory)
{
    QMutexLocker lock(&mutex);

    SAFE_POINT(!opWidgetFactories.contains(factory),
        "The registry already contains submitted Options Panel factory!", false);

    opWidgetFactories.append(factory);
    return true;
}


QList<OPWidgetFactory*> OPWidgetFactoryRegistry::getRegisteredFactories(const QList<OPFactoryFilterVisitorInterface*>& filters)
{
    QMutexLocker lock(&mutex);

    QList<OPWidgetFactory*> factoriesForObjView;

    foreach (OPWidgetFactory* factory, opWidgetFactories) {
        bool pass = true;
        foreach(OPFactoryFilterVisitorInterface* filter, filters){
            pass &= factory->passFiltration(filter);
        }
        if (pass) {
            factoriesForObjView.append(factory);
        }
    }

    return factoriesForObjView;
}


OPCommonWidgetFactoryRegistry::OPCommonWidgetFactoryRegistry(QObject *parent)
    : QObject(parent)
{
}


OPCommonWidgetFactoryRegistry::~OPCommonWidgetFactoryRegistry()
{
    foreach (OPCommonWidgetFactory *factory, factories) {
        delete factory;
    }
}

bool OPCommonWidgetFactoryRegistry::registerFactory(OPCommonWidgetFactory *factory) {
    QMutexLocker lock(&mutex);

    SAFE_POINT(!factories.contains(factory), "OP common widget factory has been already registered!", false);
    factories.append(factory);

    return true;
}

QList<OPCommonWidgetFactory*> OPCommonWidgetFactoryRegistry::getRegisteredFactories(QString groupId) {
    QMutexLocker lock(&mutex);
    QList<OPCommonWidgetFactory*> result;

    foreach (OPCommonWidgetFactory *factory, factories) {
        SAFE_POINT(NULL != factory, "NULL factory!", result);
        if (factory->isInGroup(groupId)) {
            result.append(factory);
        }
    }

    return result;
}




} // namespace
