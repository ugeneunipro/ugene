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

#include <U2Core/Counter.h>

#include <U2Algorithm/MolecularSurface.h>
#include <BALL/STRUCTURE/triangulatedSAS.h>
#include "SAS.h"

namespace U2
{

// SolventAccessibleSurface

SolventAccessibleSurface::SolventAccessibleSurface()
{
    GCOUNTER( cvar, tvar, "SolventAccessibleSurface" );
}

void SolventAccessibleSurface::calculate(const QList<SharedAtom>& atoms, int& /*progress*/)
{
    BALL::Surface surface;
    {
        std::vector<BALL::TSphere3<double> > spheres;
        foreach(const SharedAtom a, atoms)
        {
            Vector3D coord=a->coord3d;
            double radius=AtomConstants::getAtomCovalentRadius(a->atomicNumber)+TOLERANCE;
            spheres.push_back(BALL::TSphere3<double>(BALL::TVector3<double>(coord.x,coord.y,coord.z),radius));
        }
        double probeRadius=1.4;
        BALL::ReducedSurface reducedSurface(spheres,probeRadius);
        reducedSurface.compute();
        BALL::SolventAccessibleSurface solventAccessibleSurface(&reducedSurface);
        solventAccessibleSurface.compute();
        double density = 1000/atoms.size();
        BALL::TriangulatedSAS triangulatedSAS(&solventAccessibleSurface, density);
        triangulatedSAS.compute();
        triangulatedSAS.exportSurface(surface);
    }
    for(unsigned int faceIndex=0;faceIndex < surface.getNumberOfTriangles();faceIndex++)
    {
        const BALL::Surface::Triangle &triangle=surface.getTriangle(faceIndex);
        Face face;
        for(int coordIndex=0;coordIndex < 3;coordIndex++)
        {
            face.v[0][coordIndex]=surface.getVertex(triangle.v1)[coordIndex];
            face.v[1][coordIndex]=surface.getVertex(triangle.v2)[coordIndex];
            face.v[2][coordIndex]=surface.getVertex(triangle.v3)[coordIndex];
            face.n[0][coordIndex]=surface.getNormal(triangle.v1)[coordIndex];
            face.n[1][coordIndex]=surface.getNormal(triangle.v2)[coordIndex];
            face.n[2][coordIndex]=surface.getNormal(triangle.v3)[coordIndex];
        }
        faces.append(face);
    }
}

qint64 SolventAccessibleSurface::estimateMemoryUsage( int )
{
    return 0;
}



// SolventAccessibleSurfaceFactory

MolecularSurface *SolventAccessibleSurfaceFactory::createInstance() const
{
    return new SolventAccessibleSurface();
}

} // namespace
