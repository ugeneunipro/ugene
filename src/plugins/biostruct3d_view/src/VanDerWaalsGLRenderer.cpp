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

#include <time.h>

#include <U2Core/BioStruct3D.h>
#include <U2Core/Log.h>
#include <U2Algorithm/MolecularSurface.h>

#include "GraphicUtils.h"
#include "BioStruct3DGLWidget.h"
#include "BioStruct3DColorScheme.h"
#include "VanDerWaalsGLRenderer.h"


namespace U2 {

const QString VanDerWaalsGLRenderer::ID(QObject::tr("Space Fill"));

//unsigned int DisplayLists::atomDL = -1;


void VanDerWaalsGLRenderer::drawBioStruct3D( )
{
    drawAtoms(colorScheme);
}


void VanDerWaalsGLRenderer::drawAtoms( const BioStruct3DColorScheme* colorScheme )
{
    static float tolerance = 0.45f;
    int numSlices = 10 * settings->detailLevel;
    GLUquadricObj *pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);


    // Draw atoms as spheres

    foreach (const SharedMolecule mol, bioStruct.moleculeMap) {
        foreach (int index, shownModels) {
            const Molecule3DModel& model = mol->models.value(index);
            foreach(const SharedAtom atom, model.atoms) {
                float radius = AtomConstants::getAtomCovalentRadius(atom->atomicNumber);
                Vector3D pos = atom->coord3d;
                Color4f atomColor = colorScheme->getAtomColor(atom);
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, atomColor.getConstData());
                glPushMatrix();
                glTranslatef(pos.x, pos.y, pos.z);
                gluSphere(pObj, radius + tolerance, numSlices, numSlices);
                glPopMatrix();
            }
        }
    }
    gluDeleteQuadric(pObj);

}

VanDerWaalsGLRenderer::VanDerWaalsGLRenderer( const BioStruct3D& struc, const BioStruct3DColorScheme* s, const QList<int> &shownModels, const BioStruct3DRendererSettings *settings)
    : BioStruct3DGLRenderer(struc,s,shownModels,settings)
{
    create();
}

} //namespace
