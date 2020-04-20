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

#include "VanDerWaalsSurface.h"

namespace U2 {

// VanDerWaalsSurface

// determined by geodesic sphere detail level
static int FACTOR = 64;


VanDerWaalsSurface::VanDerWaalsSurface()
{
}

void VanDerWaalsSurface::calculate(const QList<SharedAtom> &atoms, int& progress)
{
    // Van Der Vaals surface calculation
    // based on atom radius (look for neighbours, exclude unneeded atoms)
    int overall = atoms.size();

    int counter = 0;

    int detaillevel = 2;

    if (atoms.size() > 10000) {
        detaillevel = 1;
    }

    foreach (const SharedAtom& a, atoms) {
        QList<SharedAtom> neighbors = findAtomNeighbors(a, atoms);
        GeodesicSphere surface = getAtomSurfaceDots(a, detaillevel);
        QVector<Vector3D> surfaceDots = surface.getVertices();
        QVector<Vector3D> reducedVertices;
        foreach (const Vector3D& v, surfaceDots) {
            if ( vertexNeighboursOneOf(v, neighbors) ) {
                continue;
            } else {
                reducedVertices.append(v);
            }
        }
        QVector<Face> surfaceFaces = surface.getFaces();
        foreach (const Face& face, surfaceFaces) {
            if ( reducedVertices.contains(face.v[0]) || reducedVertices.contains(face.v[1]) || reducedVertices.contains(face.v[2]) ) {
                faces.append(face);
            }
        }
        counter++;
        progress = counter * 100 / overall;
    }

}

//void VanDerWaalsSurface::calculate(const BioStruct3D& bioStruct)
//{
//     Vector3D center = bioStruct.getCenter();
//     float radius = bioStruct.getMaxDistFromCenter();
//     GeodesicSphere sphere(center, radius);
//     vertices = sphere.getVertices();
//}

qint64 VanDerWaalsSurface::estimateMemoryUsage( int numberOfAtoms )
{
    return numberOfAtoms*FACTOR*sizeof(double)*3*6*1.5;
}


// VanDerWaalsSurfaceFactory

MolecularSurface *VanDerWaalsSurfaceFactory::createInstance()const
{
    return new VanDerWaalsSurface();
}

} // namespace U2
