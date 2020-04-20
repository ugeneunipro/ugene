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

#include <math.h>

#include "Vector3D.h"

#include <U2Core/Matrix44.h>

namespace U2 {

Vector3D::Vector3D(double xi, double yi, double zi)
{
    x=xi; y=yi; z=zi;
}

Vector3D::Vector3D(const Vector3D& v)
{
    x=v.x; y=v.y; z=v.z;
}

Vector3D& Vector3D::operator = (const Vector3D& v)
{
    x=v.x; y=v.y; z=v.z;
    return *this;
}

void Vector3D::set(double xs, double ys, double zs)
{
    x=xs; y=ys; z=zs;
}

bool Vector3D::operator == (const Vector3D& other) const
{
    return (x == other.x && y == other.y && z == other.z);
}

bool Vector3D::operator != (const Vector3D& other) const
{
    return !(*this == other);
}

double& Vector3D::operator [] (unsigned int i)
{
    static double err = 0.0;
    if (i == 0) return x;
    else if (i == 1) return y;
    else if (i == 2) return z;
    else qCritical("Vector operator [] access out of range : %d", i);
    return err;
}

double Vector3D::operator [] (unsigned int i) const
{
    if (i == 0) return x;
    else if (i == 1) return y;
    else if (i == 2) return z;
    else qCritical("Vector operator [] access out of range : %d", i);
    return 0.0;
}

Vector3D operator - (const Vector3D& a)
{
    return Vector3D(-a.x, -a.y, -a.z);
}

Vector3D operator + (const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.x+b.x, a.y+b.y, a.z+b.z);
}

Vector3D& Vector3D::operator += (const Vector3D& v)
{
    x+=v.x; y+=v.y; z+=v.z;
    return *this;
}

Vector3D operator - (const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.x-b.x, a.y-b.y, a.z-b.z);
}

Vector3D& Vector3D::operator -= (const Vector3D& v)
{
    x-=v.x; y-=v.y; z-=v.z;
    return *this;
}

Vector3D operator * (const Vector3D& v, double f)
{
    return Vector3D(v.x*f, v.y*f, v.z*f);
}

Vector3D operator * (double f, const Vector3D& v)
{
    return Vector3D(v.x*f, v.y*f, v.z*f);
}

Vector3D& Vector3D::operator *= (double f)
{
    x*=f; y*=f; z*=f;
    return *this;
}

Vector3D operator / (const Vector3D& v, double f)
{
    return Vector3D(v.x/f, v.y/f, v.z/f);
}

Vector3D& Vector3D::operator /= (double f)
{
    x/=f; y/=f; z/=f;
    return *this;
}

Vector3D& Vector3D::dot(const Matrix44 &m) {
    const Vector3D tmp = *this;
    const float *mdata = m.data();

    x = tmp.x*mdata[0] + tmp.y*mdata[1] + tmp.z*mdata[2] + mdata[3];
    y = tmp.x*mdata[4] + tmp.y*mdata[5] + tmp.z*mdata[6] + mdata[7];
    z = tmp.x*mdata[8] + tmp.y*mdata[9] + tmp.z*mdata[10] + mdata[11];

    return *this;
}

double Vector3D::length(void) const
{
    return sqrt(x*x + y*y + z*z);
}

double Vector3D::lengthSquared(void) const
{
    return (x*x + y*y + z*z);
}

void Vector3D::normalize(void)
{
    *this /= length();
}

double vector_dot(const Vector3D& a, const Vector3D& b)
{
    return (a.x*b.x + a.y*b.y + a.z*b.z);
}

Vector3D vector_cross(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
        );
}

} //namespace
