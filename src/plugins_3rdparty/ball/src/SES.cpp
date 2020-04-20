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
#include <BALL/STRUCTURE/triangulatedSES.h>

#include "SES.h"


namespace U2
{


// SolventExcludedSurface

SolventExcludedSurface::SolventExcludedSurface()
{
    GCOUNTER( cvar, tvar, "SolventExcludedSurface" );
}


static void calcSES( BALL::Surface& surface, const QList<SharedAtom>& atoms, double TOLERANCE )
{
    std::vector<BALL::TSphere3<double> > spheres;
    foreach(const SharedAtom a, atoms)
    {
        Vector3D coord=a->coord3d;
        double radius = AtomConstants::getAtomCovalentRadius(a->atomicNumber) + TOLERANCE;
        spheres.push_back(BALL::TSphere3<double>(BALL::TVector3<double>(coord.x,coord.y,coord.z),radius));
    }

    double probeRadius=1.4;
    double density = 1000. / atoms.size();

    BALL::ReducedSurface* reduced_surface = new BALL::ReducedSurface(spheres, probeRadius);
    reduced_surface->compute();

    {
        BALL::SolventExcludedSurface* ses = new BALL::SolventExcludedSurface(reduced_surface);
        ses->compute();
        double diff = 0.01;
        uint i = 0;
        bool ok = false;
        while (!ok && (i < 10))
        {
            i++;
            ok = ses->check();
            if (!ok)
            {
                delete ses;
                delete reduced_surface;
                probeRadius += diff;
                reduced_surface = new BALL::ReducedSurface(spheres, probeRadius);
                reduced_surface->compute();
                ses = new BALL::SolventExcludedSurface(reduced_surface);
                ses->compute();
            }
        }
        if (ok)
        {
            BALL::TriangulatedSES* tSurface = new BALL::TriangulatedSES(ses, density);
            tSurface->compute();
            tSurface->exportSurface(surface);
            delete tSurface;
        }
        delete ses;
    }

    delete reduced_surface;

}


void SolventExcludedSurface::calculate(const QList<SharedAtom>& atoms, int& /*progress*/)
{
//        std::vector<BALL::TSphere3<double> > spheres;
//         foreach(const SharedAtom a, atoms)
//         {
//             Vector3D coord=a->coord3d;
//             double radius=PDBFormat::getAtomCovalentRadius(a->atomicNumber)+TOLERANCE;
//             spheres.push_back(BALL::TSphere3<double>(BALL::TVector3<double>(coord.x,coord.y,coord.z),radius));
//         }
//         double probeRadius=1.4;
//         double density = 1000. / atoms.size();
//         for(int attempt=0;attempt < 10;attempt++)
//         {
//             progress = 0;
//             BALL::ReducedSurface* reducedSurface = new BALL::ReducedSurface(spheres,probeRadius);
//             reducedSurface->compute();
//             BALL::SolventExcludedSurface solventExcludedSurface(&reducedSurface);
//             solventExcludedSurface.compute();
//             if(solventExcludedSurface.check())
//             {
//                  BALL::TriangulatedSES triangulatedSES(&solventExcludedSurface,density);
//                  triangulatedSES.compute(progress);
//                  triangulatedSES.exportSurface(surface);
//                 break;
//             }
//             probeRadius+=0.01;
//         }
//
//
    BALL::Surface surface;
    calcSES(surface, atoms, TOLERANCE);

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

qint64 SolventExcludedSurface::estimateMemoryUsage( int )
{
    return 0;
}






// SolventExcludedSurfaceFactory

MolecularSurface *SolventExcludedSurfaceFactory::createInstance() const
{
    return new SolventExcludedSurface();
}

#define MAX_ATOMS_NUMBER 10000

bool SolventExcludedSurfaceFactory::hasConstraints( const BioStruct3D& biostruc ) const
{
    if (biostruc.getNumberOfAtoms() > MAX_ATOMS_NUMBER) {
        return true;
    } else {
        return false;
    }
}


} // namespace
