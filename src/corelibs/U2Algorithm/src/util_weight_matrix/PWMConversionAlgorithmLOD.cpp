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

#include "PWMConversionAlgorithmLOD.h"
#include <math.h>
#include <stdlib.h>

namespace U2 {

PWMConversionAlgorithmFactoryLOD::PWMConversionAlgorithmFactoryLOD(QObject* p)
: PWMConversionAlgorithmFactory(BuiltInPWMConversionAlgorithms::LOD_ALGO, p)
{
}

PWMConversionAlgorithm* PWMConversionAlgorithmFactoryLOD::createAlgorithm(QObject* p){
    return new PWMConversionAlgorithmLOD(this, p);
}

QString PWMConversionAlgorithmFactoryLOD::getName() const {
    return tr("log-odds");
}

QString PWMConversionAlgorithmFactoryLOD::getDescription() const {
    return tr("log-odds weight function");
}

PWMatrix PWMConversionAlgorithmLOD::convert(const PFMatrix& matrix) {
    int size = (matrix.getType() == PFM_MONONUCLEOTIDE) ? 4 : 16;
    int length = matrix.getLength();
    float bj = 1.0 / size;
    int sum[16];
    memset(sum, 0, size*sizeof(int));
    int all_hits = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < length; j++) {
            sum[i] += matrix.getValue(i, j);
            all_hits += matrix.getValue(i, j);
        }
    }
    if (all_hits <= 0) {
        return PWMatrix();
    }
    float p[16];
    for (int i = 0; i < size; i++) {
        p[i] = sum[i];
        p[i] /= all_hits;
        if (qAbs(p[i]) <= 1e-9) {
            return PWMatrix();
        }
    }

    QVarLengthArray<float> res(size*length);
    int N = all_hits / length;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < length; j++) {
            res[matrix.index(i, j)] = log(((matrix.getValue(i, j) + bj) / (N + 1)) / p[i]);
        }
    }
    PWMatrix w(res, (matrix.getType() == PFM_MONONUCLEOTIDE) ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    w.setInfo(UniprobeInfo(matrix.getProperties()));
    return w;
}

PWMConversionAlgorithmLOD::PWMConversionAlgorithmLOD(PWMConversionAlgorithmFactory* factory, QObject* p)
: PWMConversionAlgorithm(factory, p)
{
}

}
