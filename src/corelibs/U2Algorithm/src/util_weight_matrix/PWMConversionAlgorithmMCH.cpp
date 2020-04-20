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

#include "PWMConversionAlgorithmMCH.h"
#include <math.h>

namespace U2 {

PWMConversionAlgorithmFactoryMCH::PWMConversionAlgorithmFactoryMCH(QObject* p)
: PWMConversionAlgorithmFactory(BuiltInPWMConversionAlgorithms::MCH_ALGO, p)
{
}

PWMConversionAlgorithm* PWMConversionAlgorithmFactoryMCH::createAlgorithm(QObject* p){
    return new PWMConversionAlgorithmMCH(this, p);
}

QString PWMConversionAlgorithmFactoryMCH::getName() const {
    return tr("Match");
}

QString PWMConversionAlgorithmFactoryMCH::getDescription() const {
    return tr("Match weight function");
}

PWMatrix PWMConversionAlgorithmMCH::convert(const PFMatrix& matrix) {
    int size = (matrix.getType() == PFM_MONONUCLEOTIDE) ? 4 : 16;
    int all_hits = 0;
    if (matrix.getLength() <= 0) {
        return PWMatrix();
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0, n = matrix.getLength(); j < n; j++) {
            all_hits += matrix.getValue(i, j);
        }
    }
    if (all_hits <= 0) {
        return PWMatrix();
    }
    QVarLengthArray<float> res(size * matrix.getLength());

    int N = all_hits / matrix.getLength();
    for (int j = 0, n = matrix.getLength(); j < n; j++) {
        float coef = 0;
        for (int i = 0; i < size; i++) {
            if (matrix.getValue(i, j) <= 0) {
                return PWMatrix();
            }
            coef += matrix.getValue(i, j) * log((float)size * matrix.getValue(i, j) / N);
        }
        for (int i = 0; i < size; i++) {
            res[matrix.index(i, j)] = matrix.getValue(i, j) * coef;
        }
    }
    PWMatrix w(res, (matrix.getType() == PFM_MONONUCLEOTIDE) ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    w.setInfo(UniprobeInfo(matrix.getProperties()));
    return w;
}

PWMConversionAlgorithmMCH::PWMConversionAlgorithmMCH(PWMConversionAlgorithmFactory* factory, QObject* p)
: PWMConversionAlgorithm(factory, p)
{
}

}
