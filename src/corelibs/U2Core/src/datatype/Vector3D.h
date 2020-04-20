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

#ifndef _U2_VECTOR_MATH_H_
#define _U2_VECTOR_MATH_H_

#include <QtGlobal>
#include <U2Core/global.h>

namespace U2 {

class Matrix44;

class U2CORE_EXPORT Vector3D {
public:
    double x, y, z;

    explicit Vector3D(double xi = 0.0, double yi = 0.0, double zi = 0.0);
    Vector3D(const Vector3D& v);
    Vector3D& operator = (const Vector3D& v);
    bool operator == (const Vector3D& other) const;
    bool operator != (const Vector3D& other) const;
    double& operator [] (unsigned int i);
    double operator [] (unsigned int i) const;
    Vector3D& operator += (const Vector3D& v);
    Vector3D& operator -= (const Vector3D& v);
    Vector3D& operator *= (double f);
    Vector3D& operator /= (double f);
    Vector3D& dot(const Matrix44 &m);
    void set(double xs, double ys, double zs);
    double length(void) const;
    double lengthSquared(void) const;
    void normalize(void);
};

U2CORE_EXPORT Vector3D operator - (const Vector3D& a);
U2CORE_EXPORT Vector3D operator + (const Vector3D& a, const Vector3D& b);
U2CORE_EXPORT Vector3D operator - (const Vector3D& a, const Vector3D& b);
U2CORE_EXPORT double vector_dot(const Vector3D& a, const Vector3D& b);
U2CORE_EXPORT Vector3D vector_cross(const Vector3D& a, const Vector3D& b);
U2CORE_EXPORT Vector3D operator * (const Vector3D& v, double f);
U2CORE_EXPORT Vector3D operator * (double f, const Vector3D& v);
U2CORE_EXPORT Vector3D operator / (const Vector3D& v, double f);




}//namespace

#endif //_U2_VECTOR_MATH_H_
