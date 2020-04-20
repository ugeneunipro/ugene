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

#ifndef _U2_MSA_DISTANCE_ALGORITHM_REGISTRY_H_
#define _U2_MSA_DISTANCE_ALGORITHM_REGISTRY_H_

#include "MSADistanceAlgorithm.h"

#include <QStringList>

namespace U2 {

class MSADistanceAlgorithm;

class U2ALGORITHM_EXPORT MSADistanceAlgorithmRegistry : public QObject {
    Q_OBJECT

public:
    MSADistanceAlgorithmRegistry(QObject* p = NULL);

    ~MSADistanceAlgorithmRegistry();

    MSADistanceAlgorithmFactory* getAlgorithmFactory(const QString& algoId);

    void addAlgorithm(MSADistanceAlgorithmFactory* algo);

    QStringList getAlgorithmIds() const ;

    QList<MSADistanceAlgorithmFactory*> getAlgorithmFactories() const {return algorithms.values();}

    QList<MSADistanceAlgorithmFactory*> getAlgorithmFactories(DistanceAlgorithmFlags flags) const;

private:
    QMap<QString , MSADistanceAlgorithmFactory*> algorithms;
};

}//namespace

#endif
