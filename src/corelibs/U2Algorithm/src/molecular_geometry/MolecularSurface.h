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

#ifndef _U2_BIOSTRUCT3D_MOLECULAR_SURFACE_H_
#define _U2_BIOSTRUCT3D_MOLECULAR_SURFACE_H_

#include <QObject>
#include <QVector>
#include <QString>
#include <limits>

#include <U2Core/Task.h>
#include <U2Core/BioStruct3D.h>

#include "GeomUtils.h"

namespace U2 {

template<class T> inline T sqr(const T& x) { return x*x; }

class U2ALGORITHM_EXPORT AtomConstants {
    static const int NUM_ELEMENTS = 120;
    static double atomRadiusTable[NUM_ELEMENTS];
public:
    inline static double getAtomCovalentRadius(int atomicNumber);
    static void init();
};

double AtomConstants::getAtomCovalentRadius( int atomicNumber ) {
    Q_ASSERT(atomicNumber < NUM_ELEMENTS);
    return atomRadiusTable[atomicNumber];
}


class U2ALGORITHM_EXPORT MolecularSurface {
public:
    virtual ~MolecularSurface();

    virtual void calculate(const QList<SharedAtom>& atoms, int& progress) = 0;
    virtual qint64 estimateMemoryUsage(int numberOfAtoms);

    const QVector<Face> &getFaces() const;

    static QList<SharedAtom> findAtomNeighbors(const SharedAtom& a, const QList<SharedAtom>& atoms);
    static GeodesicSphere getAtomSurfaceDots(const SharedAtom& a, int detaillevel);
    static bool vertexNeighboursOneOf(const Vector3D& v, const QList<SharedAtom>& atoms);

protected:
    QVector<Face> faces;
    static const float TOLERANCE;
};


class U2ALGORITHM_EXPORT MolecularSurfaceCalcTask : public Task
{
    Q_OBJECT
    MolecularSurface* molSurface;
    QString typeName;
    const QList<SharedAtom> atoms;
public:
    MolecularSurfaceCalcTask(const QString& surfaceTypeName, const QList<SharedAtom>& atoms);
    MolecularSurface * getCalculatedSurface();
    virtual void run();
    virtual ReportResult report();
};



class U2ALGORITHM_EXPORT MolecularSurfaceFactory {
public:
    virtual ~MolecularSurfaceFactory();
    virtual MolecularSurface *createInstance()const=0;
    virtual bool hasConstraints(const BioStruct3D&) const {  return false; }
};

} //namespace

#endif //_U2_BIOSTRUCT3D_MOLECULAR_SURFACE_H_

