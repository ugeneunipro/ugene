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

#ifndef _U2_MATRIX_44_H_
#define _U2_MATRIX_44_H_

#include <QVector>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT Matrix44 {
public:
    Matrix44();
    Matrix44(const float *data);
    Matrix44(const Matrix44 &other);

    Matrix44& operator= (const Matrix44 &other);

    void loadZero();
    void loadIdentity();

    void transpose();

    float* data();
    const float* data() const;

    float& operator[] (unsigned int i);
    float operator[] (unsigned int i) const;

    void load(const QVariantList &values);
    QVariantList store();

private:
    QVector<float> m;
};  // class Matrix44

}   // namespace U2

#endif  // #ifndef _U2_MATRIX_44_H_
