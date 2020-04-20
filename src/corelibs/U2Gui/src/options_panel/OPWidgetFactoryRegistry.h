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

#ifndef _U2_OP_WIDGET_FACTORY_REGISTRY_H_
#define _U2_OP_WIDGET_FACTORY_REGISTRY_H_

#include "OPWidgetFactory.h"

#include <U2Core/global.h>

#include <QMutex>


namespace U2 {


class U2GUI_EXPORT OPWidgetFactoryRegistry : public QObject
{
    Q_OBJECT

public:
    OPWidgetFactoryRegistry(QObject* parent = NULL);
    ~OPWidgetFactoryRegistry();

    bool registerFactory(OPWidgetFactory* factory);

    QList<OPWidgetFactory*> getRegisteredFactories(const QList<OPFactoryFilterVisitorInterface*>& filters);

private:
    QList<OPWidgetFactory*> opWidgetFactories;
    QMutex mutex;
};

class U2GUI_EXPORT OPCommonWidgetFactoryRegistry : public QObject
{
    Q_OBJECT

public:
    OPCommonWidgetFactoryRegistry(QObject *parent = NULL);
    ~OPCommonWidgetFactoryRegistry();

    bool registerFactory(OPCommonWidgetFactory *factory);

    QList<OPCommonWidgetFactory*> getRegisteredFactories(QString groupId);

private:
    QList<OPCommonWidgetFactory*> factories;
    QMutex mutex;
};

} // namespace

#endif

