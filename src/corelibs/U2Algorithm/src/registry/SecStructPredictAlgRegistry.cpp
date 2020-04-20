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

#include <U2Algorithm/SecStructPredictTask.h>
#include "SecStructPredictAlgRegistry.h"
#include <QStringList>

namespace U2 {

SecStructPredictAlgRegistry::SecStructPredictAlgRegistry( QObject* pOwn /* = 0*/ ) : QObject(pOwn)
{


}

SecStructPredictAlgRegistry::~SecStructPredictAlgRegistry()
{
    foreach( SecStructPredictTaskFactory* factory, algMap.values()) {
        delete factory;
    }
}

bool SecStructPredictAlgRegistry::registerAlgorithm( SecStructPredictTaskFactory* alg, const QString& algId )
{
    QMutexLocker locker(&mutex);

    if (algMap.contains(algId)){
        return false;
    }
    algMap.insert(algId, alg);
    return true;

}

bool SecStructPredictAlgRegistry::hadRegistered( const QString& algId )
{
    return algMap.contains(algId);
}

SecStructPredictTaskFactory* SecStructPredictAlgRegistry::getAlgorithm( const QString& algId )
{
    if (algMap.contains(algId)) {
        return algMap.value(algId);
    } else {
        return NULL;
    }
}

QStringList SecStructPredictAlgRegistry::getAlgNameList()
{
    return algMap.keys();
}


}


