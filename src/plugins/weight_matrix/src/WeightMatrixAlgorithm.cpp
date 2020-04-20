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

#include "WeightMatrixAlgorithm.h"

#include <U2Core/DIProperties.h>

namespace U2 {

float WeightMatrixAlgorithm::getScore(const char* seq, int len, const PWMatrix& m, DNATranslation* complMap) {
    int l = m.getLength();

    float lower = m.getMinSum(), upper = m.getMaxSum();
    QByteArray complMapper = (complMap != NULL) ? complMap->getOne2OneMapper() : QByteArray();
    assert ((upper - lower) > 1e-9);
    float curr = 0;
    if (m.getType() == PWM_MONONUCLEOTIDE) {
        for (int i = 0; i < len && i < l; i++) {
            char c = (complMap != NULL) ? complMapper[uchar(seq[len - i])] : seq[i];
            curr += m.getValue(DiProperty::index(c), i);
        }
    } else {
        for (int i = 0; i < len && i < l; i++) {
            char c1 = (complMap != NULL) ? complMapper[uchar(seq[len - i])] : seq[i];
            char c2 = (complMap != NULL) ? complMapper[uchar(seq[len - (i+1)])] : seq[i+1];
            curr += m.getValue(DiProperty::index(c1, c2), i);
        }
    }
    assert (curr >= lower);
    assert (curr <= upper);
    return (curr - lower) / (upper - lower);
}

} //namespace
