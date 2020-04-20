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

#include "GraphicUtils.h"
#include "BioStruct3DColorScheme.h"
#include "BioStruct3DGLWidget.h"
#include "TubeGLRenderer.h"

namespace U2 {

const QString TubeGLRenderer::ID(QObject::tr("Tubes"));

void TubeGLRenderer::drawTubes( const BioStruct3DColorScheme* colorScheme )
{
    GLUquadricObj *pObj;    // Quadric Object

    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    bool firstPass = true;
    static float ribbonThickness = 0.3f;
    SharedAtom bufAtom;

    foreach (Tube tube, tubeMap) {
        foreach (int index, shownModels) {
            const AtomsVector& tubeAtoms = tube.modelsMap.value(index);
            foreach(const SharedAtom atom, tubeAtoms) {
                Color4f atomColor = colorScheme->getAtomColor(atom);
                Vector3D pos = atom.constData()->coord3d;
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, atomColor.getConstData());
                glDrawAtom(pObj, pos, ribbonThickness, settings->detailLevel);

                if (!firstPass) {
                    // Draw bonds only between atoms of the same molecular chain
                    if (atom.constData()->chainIndex == bufAtom.constData()->chainIndex) {
                        // ... and when they are sequential
                        if (atom.constData()->residueIndex.toInt() - bufAtom.constData()->residueIndex.toInt() == 1) {
                            Vector3D bufPos = bufAtom.constData()->coord3d;
                            Color4f bufAtomColor = colorScheme->getAtomColor(bufAtom);
                            glDrawHalfBond(pObj, bufPos, pos, ribbonThickness, settings->detailLevel);
                            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bufAtomColor.getConstData());
                            glDrawHalfBond(pObj, pos, bufPos, ribbonThickness, settings->detailLevel);
                        }
                    }
                } else {
                    firstPass = false;
                }

                bufAtom = atom;
            }
        }
    }

    gluDeleteQuadric(pObj);
}


void TubeGLRenderer::drawBioStruct3D()
{

    drawTubes(colorScheme);
}


TubeGLRenderer::TubeGLRenderer( const BioStruct3D& struc, const BioStruct3DColorScheme* s, const QList<int> &shownModels, const BioStruct3DRendererSettings *settings )
        : BioStruct3DGLRenderer(struc,s,shownModels,settings)
{
    create();
}

bool TubeGLRenderer::isAvailableFor(const BioStruct3D &bioStruct) {
    bool available = false;

    const char* alphaCarbonTag = "CA";
    const char* phosporTag = "P";

    foreach (const SharedMolecule mol, bioStruct.moleculeMap) {
        foreach (const Molecule3DModel& model, mol->models.values()) {
            foreach (const SharedAtom atom, model.atoms) {
                if ( (atom->name.trimmed() == alphaCarbonTag) || (atom->name.trimmed() == phosporTag)) {
                    available = true;
                }
            }
        }
    }

    return available;
}

void TubeGLRenderer::create() {
    assert(isAvailableFor(bioStruct) && "Availability must be checked first!");

    tubeMap.clear();

    const char* alphaCarbonTag = "CA";
    const char* phosporTag = "P";

    foreach (const SharedMolecule mol, bioStruct.moleculeMap) {
        foreach (int modelId, mol->models.keys()) {
            const Molecule3DModel& model = mol->models.value(modelId);
            foreach (const SharedAtom atom, model.atoms) {
                if ( (atom->name.trimmed() == alphaCarbonTag) || (atom->name.trimmed() == phosporTag)) {
                    tubeMap[atom->chainIndex].modelsMap[modelId].append(atom);
                }
            }
        }
    }
}


} //namespace
