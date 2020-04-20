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

#include "Matrix44.h"

namespace U2 {

/* class Matrix44 */

Matrix44::Matrix44() : m(16, 0.0)
{
    loadIdentity();
}

Matrix44::Matrix44(const float *data) : m(16) {
    for (int i = 0; i < 16; ++i) {
        m[i] = data[i];
    }
}

Matrix44::Matrix44(const Matrix44 &other) : m(other.m)
{}

Matrix44& Matrix44::operator= (const Matrix44 &other) {
    m = other.m;
    return *this;
}

void Matrix44::loadZero() {
    m.fill(0.0);
}

void Matrix44::loadIdentity() {
    m.fill(0.0);
    for (int i = 0; i < 4; ++i) {
        m[i + i*4] = 1.0;
    }
}

float* Matrix44::data() {
    return m.data();
}

const float* Matrix44::data() const {
    return m.constData();
}

float& Matrix44::operator[] (unsigned int i) {
    return m[i];
}

float Matrix44::operator[] (unsigned int i) const {
    return m[i];
}

void Matrix44::load(const QVariantList &values) {
    assert(values.size() == 16);
    for (int i = 0; i < 16; ++i) {
        m[i] = values.at(i).value<float>();
    }
}

QVariantList Matrix44::store() {
    QVariantList values;
    for (int i = 0; i < 16; ++i ) {
        values.append(QVariant::fromValue(m[i]));
    }
    return values;
}

void Matrix44::transpose() {
    float *data = m.data();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < i; ++j) {
            float tmp = data[i + j*4];
            data[i + j*4] = data[j + i*4];
            data[j + i*4] = tmp;
        }
    }
}

}   // namespace U2
