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

#include "PWMConversionAlgorithmBVH.h"

#include <math.h>

namespace U2 {

PWMConversionAlgorithmFactoryBVH::PWMConversionAlgorithmFactoryBVH(QObject* p)
: PWMConversionAlgorithmFactory(BuiltInPWMConversionAlgorithms::BVH_ALGO, p)
{
}

PWMConversionAlgorithm* PWMConversionAlgorithmFactoryBVH::createAlgorithm(QObject* p){
    return new PWMConversionAlgorithmBVH(this, p);
}

QString PWMConversionAlgorithmFactoryBVH::getName() const {
    return "Berg and von Hippel";
}

QString PWMConversionAlgorithmFactoryBVH::getDescription() const {
    return tr("Berg and von Hippel weight function");
}

PWMatrix PWMConversionAlgorithmBVH::convert(const PFMatrix& matrix) {
    int size = (matrix.getType() == PFM_MONONUCLEOTIDE) ? 4 : 16;
    int length = matrix.getLength();
    QVarLengthArray<int> best(length);
    memset(best.data(), 0, length*sizeof(int));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < length; j++) {
            if (best[j] < matrix.getValue(i, j)) {
                best[j] = matrix.getValue(i, j);
            }
        }
    }

    QVarLengthArray<float> res(size*length);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < length; j++) {
            res[matrix.index(i, j)] = log((matrix.getValue(i, j) + 0.5) / (best[j] + 0.5));
        }
    }
    PWMatrix w(res, (matrix.getType() == PFM_MONONUCLEOTIDE) ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    w.setInfo(UniprobeInfo(matrix.getProperties()));
    return w;
}

PWMConversionAlgorithmBVH::PWMConversionAlgorithmBVH(PWMConversionAlgorithmFactory* factory, QObject* p)
: PWMConversionAlgorithm(factory, p)
{
}

}
