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

#include "GeomUtils.h"

namespace U2 {

QScopedPointer< QVector<Vector3D> > GeodesicSphere::elementarySphere(NULL);
int GeodesicSphere::currentDetailLevel = 1;

GeodesicSphere::GeodesicSphere( const Vector3D& center, float radius, int detaillevel)
{
    if (elementarySphere.isNull() || currentDetailLevel != detaillevel) {
        elementarySphere.reset(createGeodesicSphere(detaillevel));
        currentDetailLevel = detaillevel;
    }

    QVector<Vector3D> normals;
    vertices.resize(elementarySphere->count());
    qCopy(elementarySphere->begin(), elementarySphere->end(), vertices.begin());

    int size = vertices.count();
    for (int i = 0; i < size; ++i) {
        Vector3D& vertex = vertices[i];
        vertex.normalize();
        normals.append(vertex);
        vertex *= radius;
        vertex += center;
    }

    for (int i = 0; i < size; i+= 3) {
        Face face;
        face.v[0] = vertices.at(i);
        face.v[1] = vertices.at(i+1);
        face.v[2] = vertices.at(i+2);

        face.n[0] = normals.at(i);
        face.n[1] = normals.at(i+1);
        face.n[2] = normals.at(i+2);

        faces.append(face);
    }

}

void GeodesicSphere::interpolate( const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, QVector<Vector3D>* v, int detailLevel )
{

    if (0 == detailLevel)
    {
        v->append(v1);
        v->append(v2);
        v->append(v3);
        return;
    }

    Vector3D nv1( (v1.x + v2.x) / 2,  (v1.y + v2.y) / 2, (v1.z + v2.z) / 2 );
    Vector3D nv2( (v2.x + v3.x) / 2,  (v2.y + v3.y) / 2, (v2.z + v3.z) / 2 );
    Vector3D nv3( (v3.x + v1.x) / 2,  (v3.y + v1.y) / 2, (v3.z + v1.z) / 2 );
    interpolate(nv1, nv2, nv3,v, detailLevel - 1);
    interpolate(v1, nv1, nv3,v,  detailLevel - 1);
    interpolate(nv1, v2, nv2,v,  detailLevel - 1);
    interpolate(nv3, nv2, v3,v,  detailLevel - 1);
    return;


}

QVector<Vector3D>* GeodesicSphere::createGeodesicSphere( int detailLevel )
{
    QVector<Vector3D> base;
    base.resize(24);

    // up
    base[0] = Vector3D(-1.0f, 0.0f, 0.0f);
    base[1] = Vector3D(0.0f, 1.0f, 0.0f);
    base[2] = Vector3D(0.0f, 0.0f, -1.0f);

    base[3] = Vector3D(0.0f, 0.0f, -1.0f);
    base[4] = Vector3D(0.0f, 1.0f, 0.0f);
    base[5] = Vector3D(1.0f, 0.0f, 0.0f);

    base[6] = Vector3D(1.0f, 0.0f, 0.0f);
    base[7] = Vector3D(0.0f, 1.0f, 0.0f);
    base[8] = Vector3D(0.0f, 0.0f, 1.0f);

    base[9] = Vector3D(0.0f, 0.0f, 1.0f);
    base[10] = Vector3D(0.0f, 1.0f, 0.0f);
    base[11] = Vector3D(-1.0f, 0.0f, 0.0f);

    //down
    base[12] = Vector3D(-1.0f, 0.0f, 0.0f);
    base[13] = Vector3D(0.0f, 0.0f, -1.0f);
    base[14] = Vector3D(0.0f, -1.0f, 0.0f);

    base[15] = Vector3D(0.0f, 0.0f, -1.0f);
    base[16] = Vector3D(1.0f, 0.0f, 0.0f);
    base[17] = Vector3D(0.0f, -1.0f, 0.0f);

    base[18] = Vector3D(1.0f, 0.0f, 0.0f);
    base[19] = Vector3D(0.0f, 0.0f, 1.0f);
    base[20] = Vector3D(0.0f, -1.0f, 0.0f);

    base[21] = Vector3D(0.0f, 0.0f, 1.0f);
    base[22] = Vector3D(-1.0f, 0.0f, 0.0f);
    base[23] = Vector3D(0.0f, -1.0f, 0.0f);

    QVector<Vector3D>* v = new QVector<Vector3D>();
    for (int i = 0; i < 24; i += 3) {
        interpolate(base.at(i), base.at(i+1), base.at(i+2),v, detailLevel);
    }

    return v;
}


} //namespace
