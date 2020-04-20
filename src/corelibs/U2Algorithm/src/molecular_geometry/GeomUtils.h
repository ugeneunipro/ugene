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

#ifndef _U2_GEOM_UTILS_H_
#define _U2_GEOM_UTILS_H_

#include <QScopedPointer>
#include <QVector>

#include <U2Core/Vector3D.h>

namespace U2 {

const float PI = 3.14159265f;
const float Rad2Deg = 57.2957795f;
const float Deg2Rad = 0.017453293f;

struct U2ALGORITHM_EXPORT Face {
    Face() {}
    Vector3D v[3];
    Vector3D n[3];
};

//! Builds sphere
class U2ALGORITHM_EXPORT GeodesicSphere {
    QVector<Vector3D> vertices;
    QVector<Face> faces;
    static QScopedPointer< QVector<Vector3D> > elementarySphere;
    static int currentDetailLevel;
    static void interpolate(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, QVector<Vector3D>* v, int detailLevel);
public:
    GeodesicSphere(const Vector3D& center, float radius, int detaillevel);
    static QVector<Vector3D>* createGeodesicSphere(int detailLevel);
    QVector<Vector3D> getVertices()  { return vertices; }
    QVector<Face> getFaces() { return faces; }
};



} //namespace

#endif // _U2_GEOM_UTILS_H_

