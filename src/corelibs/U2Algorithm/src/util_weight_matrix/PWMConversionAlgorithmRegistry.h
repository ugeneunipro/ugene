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

#ifndef _U2_PWM_CONVERSION_ALGORITHM_REGISTRY_H_
#define _U2_PWM_CONVERSION_ALGORITHM_REGISTRY_H_

#include "PWMConversionAlgorithm.h"

#include <QStringList>

namespace U2 {

class PWMConversionAlgorithm;

class U2ALGORITHM_EXPORT PWMConversionAlgorithmRegistry : public QObject {
    Q_OBJECT

public:
    PWMConversionAlgorithmRegistry(QObject* p = NULL);

    ~PWMConversionAlgorithmRegistry();

    PWMConversionAlgorithmFactory* getAlgorithmFactory(const QString& algoId);

    void addAlgorithm(PWMConversionAlgorithmFactory* algo);

    QStringList getAlgorithmIds() const ;

    QList<PWMConversionAlgorithmFactory*> getAlgorithmFactories() const {return algorithms.values();}

private:
    QMap<QString , PWMConversionAlgorithmFactory*> algorithms;
};

}//namespace

#endif
