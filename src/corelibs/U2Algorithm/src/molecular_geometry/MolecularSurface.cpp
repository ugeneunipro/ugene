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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/Log.h>

#include "MolecularSurface.h"
#include "MolecularSurfaceFactoryRegistry.h"

namespace U2 {

double AtomConstants::atomRadiusTable[NUM_ELEMENTS] = {0.0};

void AtomConstants::init()
{
    static bool initialized = false;

    if (!initialized) {

        // Init atomRadiusTable
        atomRadiusTable[1] = 0.23;
        atomRadiusTable[5] = 0.83;
        atomRadiusTable[6] = 0.68;
        atomRadiusTable[7] = 0.68;
        atomRadiusTable[8] = 0.68;
        atomRadiusTable[9] = 0.64;
        atomRadiusTable[14] = 1.20;
        atomRadiusTable[15] = 1.05;
        atomRadiusTable[16] = 1.02;
        atomRadiusTable[17] = 0.99;
        atomRadiusTable[33] = 1.21;
        atomRadiusTable[34] = 1.22;
        atomRadiusTable[35] = 1.21;
        atomRadiusTable[52] = 1.47;
        atomRadiusTable[53] = 1.40;

        // now we've initialized utility maps
        initialized = true;
    }
}

// MolecularSurface

const float MolecularSurface::TOLERANCE = 1.0f;

const QVector<Face> &MolecularSurface::getFaces() const {
    return faces;
}

QList<SharedAtom> MolecularSurface::findAtomNeighbors( const SharedAtom& a, const QList<SharedAtom>& atoms ) {
    QList<SharedAtom> neighbors;
    // maximum covalent radius in angstroms
    static const float maxAtomRadius = 1.0;
    static const float doubleRadius = 2*maxAtomRadius;
    Vector3D v1 = a->coord3d;

    foreach (const SharedAtom& neighbor, atoms) {
        if (neighbor == a) {
            continue;
        }
        Vector3D v2 = neighbor->coord3d;
        if ( ( qAbs(v1.x - v2.x) <= doubleRadius) && ( qAbs(v1.y - v2.y) <= doubleRadius  ) && ( qAbs(v1.z - v2.z) <= doubleRadius ) ) {
            neighbors.append(neighbor);
        }
   }

    return neighbors;
}

U2::GeodesicSphere MolecularSurface::getAtomSurfaceDots( const SharedAtom& a, int detaillevel ) {
    QVector<Vector3D> surfaceDots;
    float radius = TOLERANCE + AtomConstants::getAtomCovalentRadius(a->atomicNumber);
    //Calculate sphere surface dots
    GeodesicSphere sphere(a->coord3d, radius, detaillevel);
    return sphere;
}

bool MolecularSurface::vertexNeighboursOneOf( const Vector3D& v, const QList<SharedAtom>& atoms ) {
    foreach (const SharedAtom& a, atoms) {
        float r = AtomConstants::getAtomCovalentRadius(a->atomicNumber) + TOLERANCE;
        //Vector3D v2 = a->coord3d;
        //qDebug("testing if vertex (%f,%f,%f) neighbors atom (%f,%f,%f) of radius %f", v.x, v.y, v.z, v2.x, v2.y, v2.z, radius);
        if ( sqr(v.x - a->coord3d.x) + sqr(v.y - a->coord3d.y) + sqr(v.z - a->coord3d.z ) <= r*r ) {
                return true;
        }
    }

    return false;
}

MolecularSurface::~MolecularSurface() {

}

qint64 MolecularSurface::estimateMemoryUsage( int numberOfAtoms )
{
    Q_UNUSED(numberOfAtoms);

    // this many elements we can keep in container
    int maxInt = std::numeric_limits<int>::max();

    return maxInt*sizeof(double)*6;
}


MolecularSurfaceCalcTask::MolecularSurfaceCalcTask( const QString& surfaceTypeName, const QList<SharedAtom>& _atoms )
    :Task(tr("Molecular surface calculation"), TaskFlag_None), typeName(surfaceTypeName), atoms(_atoms)
{
    MolecularSurfaceFactory *factory= AppContext::getMolecularSurfaceFactoryRegistry()->getSurfaceFactory(typeName);
    molSurface = factory->createInstance();

    qint64 memUseMB = (molSurface->estimateMemoryUsage(atoms.size())) / 1024 / 1024;
    algoLog.trace(QString("Estimated memory usage: %1 MB").arg(memUseMB));

    addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, memUseMB, true));

    tpm = Progress_Manual;

}


void MolecularSurfaceCalcTask::run() {
   stateInfo.progress = 0;
   molSurface->calculate(atoms, stateInfo.progress);
}

MolecularSurface * MolecularSurfaceCalcTask::getCalculatedSurface() {
    SAFE_POINT(molSurface != NULL, "Invalid molecular surface object detected!", NULL);
    MolecularSurface *returnValue = molSurface;
    molSurface = NULL;

    return returnValue;
}

Task::ReportResult MolecularSurfaceCalcTask::report() {
    int numFaces = molSurface->getFaces().size();
    algoLog.trace(QString("Number of atoms: %1, number of faces: %2").arg(atoms.size()).arg(numFaces));
    algoLog.trace(QString("Used memory: %1 MB").arg( ( (qint64) numFaces* sizeof(double)* 3*6) / 1024 / 1024 ));
    return ReportResult_Finished;
}

MolecularSurfaceFactory::~MolecularSurfaceFactory() {

}

} // namespace

