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

#ifndef _U2_RANGE_H_
#define _U2_RANGE_H_

#include <U2Core/global.h>

namespace U2 {

/**
    Range or interval or values
    Usually used by functions that must return not exact value, but interval: [min, max]
*/
template <typename T> class U2Range {
public:
    U2Range(T _minValue = 0, T _maxValue = 0) : minValue(_minValue), maxValue(_maxValue){}

    T minValue;
    T maxValue;
};

template <typename T>
U2Range<T> operator +(const U2Range<T>& r1, const U2Range<T>& r2) {
    U2Range<T> result(qMin(r1.minValue, r2.minValue), qMax(r1.maxValue, r2.maxValue));
    return result;
}



}//namespace
#endif
