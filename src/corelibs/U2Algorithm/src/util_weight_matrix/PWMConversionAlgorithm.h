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

#ifndef _U2_PWM_CONVERSION_ALGORITHM_H_
#define _U2_PWM_CONVERSION_ALGORITHM_H_

#include <U2Core/global.h>

#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>

namespace U2 {

class PWMConversionAlgorithm;

class U2ALGORITHM_EXPORT PWMConversionAlgorithmFactory : public QObject {
    Q_OBJECT
public:
    PWMConversionAlgorithmFactory(const QString& algoId, QObject* p = NULL);

    virtual PWMConversionAlgorithm* createAlgorithm(QObject* parent = NULL) = 0;

    QString getId() const {return algorithmId;}

    virtual QString getDescription() const = 0;

    virtual QString getName() const = 0;

private:
    QString algorithmId;

};

class U2ALGORITHM_EXPORT PWMConversionAlgorithm : public QObject {
    Q_OBJECT
public:
    PWMConversionAlgorithm(PWMConversionAlgorithmFactory* factory, QObject* p = NULL);

    virtual PWMatrix convert (const PFMatrix& matrix) = 0;

    QString getId() const {return factory->getId();}
private:
    PWMConversionAlgorithmFactory* factory;

};

}//namespace

#endif
