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

#include "PWMConversionAlgorithmNLG.h"
#include <math.h>

namespace U2 {

PWMConversionAlgorithmFactoryNLG::PWMConversionAlgorithmFactoryNLG(QObject* p)
: PWMConversionAlgorithmFactory(BuiltInPWMConversionAlgorithms::NLG_ALGO, p)
{
}

PWMConversionAlgorithm* PWMConversionAlgorithmFactoryNLG::createAlgorithm(QObject* p){
    return new PWMConversionAlgorithmNLG(this, p);
}

QString PWMConversionAlgorithmFactoryNLG::getName() const {
    return tr("NLG");
}

QString PWMConversionAlgorithmFactoryNLG::getDescription() const {
    return tr("NLG weight function");
}

PWMatrix PWMConversionAlgorithmNLG::convert(const PFMatrix& matrix) {
    int size = (matrix.getType() == PFM_MONONUCLEOTIDE) ? 4 : 16;
    int sum[16];
    memset(sum, 0, size*sizeof(int));
    int all_hits = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0, n = matrix.getLength(); j < n; j++) {
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

    QVarLengthArray<float> res(size*matrix.getLength());
    for (int i = 0; i < size; i++) {
        for (int j = 0, n = matrix.getLength(); j < n; j++) {
            res[matrix.index(i, j)] = -matrix.getValue(i, j)*log(p[i]);
        }
    }

    PWMatrix w(res, (matrix.getType() == PFM_MONONUCLEOTIDE) ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    w.setInfo(UniprobeInfo(matrix.getProperties()));
    return w;
}

PWMConversionAlgorithmNLG::PWMConversionAlgorithmNLG(PWMConversionAlgorithmFactory* factory, QObject* p)
: PWMConversionAlgorithm(factory, p)
{
}

}
