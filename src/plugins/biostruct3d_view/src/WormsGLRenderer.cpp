/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "WormsGLRenderer.h"

#include <U2Core/BioStruct3D.h>
#include <U2Core/U2SafePoints.h>

#include "BioStruct3DColorScheme.h"
#include "BioStruct3DGLWidget.h"
#include "GraphicUtils.h"

namespace U2 {

const QString WormsGLRenderer::ID(QObject::tr("Worms"));

WormsGLRenderer::WormsGLRenderer(const BioStruct3D& struc, const BioStruct3DColorScheme* s, const QList<int>& shownModels, const BioStruct3DRendererSettings* settings)
    : BioStruct3DGLRenderer(struc, s, shownModels, settings) {
    create();
}

bool WormsGLRenderer::isAvailableFor(const BioStruct3D& bioStruct) {
    // Try to construct objects to draw
    QMap<int, BioPolymer> bioPolymerMap;
    createBioPolymerMap(bioStruct.moleculeMap, bioPolymerMap);

    // Find objects to draw
    for (const SharedSecondaryStructure& ss : qAsConst(bioStruct.secondaryStructures)) {
        int startId = ss->startSequenceNumber;
        int endId = ss->endSequenceNumber;
        int chainId = ss->chainIndex;
        Q_ASSERT(chainId != 0);
        if (bioPolymerMap.contains(chainId)) {
            const BioPolymer& bpolymer = bioPolymerMap.value(chainId);
            foreach (const BioPolymerModel& bpModel, bpolymer.bpModels.values()) {
                if (bpModel.monomerMap.contains(startId) && bpModel.monomerMap.contains(endId)) {
                    if (ss->type == SecondaryStructure::Type_AlphaHelix) {
                        return true;
                    } else if (ss->type == SecondaryStructure::Type_BetaStrand) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void WormsGLRenderer::create() {
    assert(isAvailableFor(bioStruct) && "Availability must be checked first!");

    createBioPolymerMap(bioStruct.moleculeMap, bioPolymerMap);
    createWorms();
}

void WormsGLRenderer::drawWorms() {
    GLUquadricObj* pObj;  // Quadric Object

    // Draw atoms as spheres
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    static float ribbonThickness = 0.3f;
    static float tension = 0.01f;

    foreach (int chainId, wormMap.keys()) {
        const Worm worm = wormMap.value(chainId);
        for (int modelId : qAsConst(shownModelsIds)) {
            if (!worm.wormModelByBioStruct3DModelId.contains(modelId)) {
                continue;
            }
            const WormModel& model = worm.wormModelByBioStruct3DModelId.value(modelId);
            // Draw worm bodies (let the bodies set the scene!!!)
            const AtomsVector wormCoords = model.atoms;
            int size = wormCoords.size();
            for (int i = 0; i + 3 < size; ++i) {
                const SharedAtom a0 = wormCoords.at(i);
                const SharedAtom a1 = wormCoords.at(i + 1);
                const SharedAtom a2 = wormCoords.at(i + 2);
                const SharedAtom a3 = wormCoords.at(i + 3);
                // draw worms only between sequential residues
                if (a2->residueIndex.toInt() - a1->residueIndex.toInt() != 1) {
                    continue;
                }

                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, getAtomColor(a1));
                glDrawHalfWorm(a0->coord3d, a1->coord3d, a2->coord3d, a3->coord3d, ribbonThickness, false, false, tension, settings->detailLevel);
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, getAtomColor(a2));
                glDrawHalfWorm(a3->coord3d, a2->coord3d, a1->coord3d, a0->coord3d, ribbonThickness, false, false, tension, settings->detailLevel);
            }

            if (wormCoords.size() >= 3) {
                // Draw worm opening
                const SharedAtom a0 = wormCoords.at(0);
                const SharedAtom a1 = wormCoords.at(1);
                const SharedAtom a2 = wormCoords.at(2);
                Vector3D atomCoordFirst = model.openingAtom;
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, getAtomColor(a0));
                glDrawHalfWorm(atomCoordFirst, a0->coord3d, a1->coord3d, a2->coord3d, ribbonThickness, true, false, tension, settings->detailLevel);
                glDrawHalfWorm(a2->coord3d, a1->coord3d, a0->coord3d, atomCoordFirst, ribbonThickness, false, false, tension, settings->detailLevel);
                // Draw worm ending
                const SharedAtom aN1 = wormCoords.at(size - 1);
                const SharedAtom aN2 = wormCoords.at(size - 2);
                const SharedAtom aN3 = wormCoords.at(size - 3);
                Vector3D atomCoordLast = model.closingAtom;
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, getAtomColor(aN1));
                glDrawHalfWorm(atomCoordLast, aN1->coord3d, aN2->coord3d, aN3->coord3d, ribbonThickness, true, false, tension, settings->detailLevel);
                glDrawHalfWorm(aN3->coord3d, aN2->coord3d, aN1->coord3d, atomCoordLast, ribbonThickness, false, false, tension, settings->detailLevel);
            }

            // Draw 3d objects
            if (shownModelsIds.count() == 1) {
                for (Object3D* obj : qAsConst(model.objects)) {
                    obj->draw(settings->detailLevel);
                }
            }
        }
    }

    gluDeleteQuadric(pObj);
}

void WormsGLRenderer::drawBioStruct3D() {
    drawWorms();
}

void WormsGLRenderer::updateColorScheme() {
    QList<int> wormIds = wormMap.keys();
    for (int id : qAsConst(wormIds)) {
        Worm& worm = wormMap[id];
        QList<int> modelIds = worm.wormModelByBioStruct3DModelId.keys();
        for (int modelId : qAsConst(modelIds)) {
            WormModel& model = worm.wormModelByBioStruct3DModelId[modelId];
            qDeleteAll(model.objects);
            model.objects.clear();
        }
    }

    // TODO: optimization -> don't have to create everything again
    createObjects3D();
}

void WormsGLRenderer::updateShownModels() {
    updateColorScheme();
}

void WormsGLRenderer::updateSettings() {
    updateColorScheme();
}

void WormsGLRenderer::createObjects3D() {
    for (const SharedSecondaryStructure& ss : qAsConst(bioStruct.secondaryStructures)) {
        int startId = ss->startSequenceNumber;
        int endId = ss->endSequenceNumber;
        int chainId = ss->chainIndex;
        Q_ASSERT(chainId != 0);
        if (bioPolymerMap.contains(chainId)) {
            const BioPolymer& bpolymer = bioPolymerMap.value(chainId);
            foreach (const BioPolymerModel& bpModel, bpolymer.bpModels.values()) {
                if (bpModel.monomerMap.contains(startId) && bpModel.monomerMap.contains(endId)) {
                    Object3D* obj = nullptr;
                    if (ss->type == SecondaryStructure::Type_AlphaHelix) {
                        obj = createHelix3D(startId, endId, bpModel);
                    } else if (ss->type == SecondaryStructure::Type_BetaStrand) {
                        obj = createStrand3D(startId, endId, bpModel);
                    }
                    if (obj != nullptr) {
                        int modelId = bpModel.biostruct3DModelId;
                        Worm& worm = wormMap[chainId];
                        SAFE_POINT(worm.wormModelByBioStruct3DModelId.contains(modelId), "Worm has wrong modelId: " + QString::number(modelId), );
                        worm.wormModelByBioStruct3DModelId[modelId].objects.append(obj);
                    }
                }
            }
        }
    }
}

void WormsGLRenderer::createWorms() {
    // Create worms coordinates
    QMapIterator<int, BioPolymer> i(bioPolymerMap);
    while (i.hasNext()) {
        i.next();
        Worm worm;
        const BioPolymer& bioPolymer = i.value();
        foreach (const BioPolymerModel& bpModel, bioPolymer.bpModels.values()) {
            const QMap<int, Monomer> monomers = bpModel.monomerMap;
            SAFE_POINT(monomers.size() != 0, "Cannot create worms - no monomers!", );

            const bool atLeast2MonomersExist = (1 < monomers.size());
            QMap<int, Monomer>::const_iterator iter(monomers.constBegin());
            // Calculate opening atom coords
            Vector3D r1(iter.value().alphaCarbon.constData()->coord3d);
            Vector3D r2((atLeast2MonomersExist ? ++iter : iter).value().alphaCarbon.constData()->coord3d);

            Vector3D a(r1);
            Vector3D b((r2 - r1) / 100.f);

            WormModel wormModel;
            wormModel.openingAtom = a + b * (-10.f);
            // Calculate closing atom coords
            iter = bpModel.monomerMap.constEnd();
            r1 = (--iter).value().alphaCarbon.constData()->coord3d;
            r2 = (atLeast2MonomersExist ? --iter : iter).value().alphaCarbon.constData()->coord3d;
            a = r1;
            b = (r2 - r1) / 100.f;
            wormModel.closingAtom = a + b * (-10.f);
            // Add worm-building atom coords
            for (const Monomer& monomer : qAsConst(monomers)) {
                const SharedAtom& atom = monomer.alphaCarbon;
                wormModel.atoms.append(atom);
            }
            worm.wormModelByBioStruct3DModelId.insert(bpModel.biostruct3DModelId, wormModel);
        }
        const int chainID = i.key();
        wormMap.insert(chainID, worm);
    }
    createObjects3D();
}

void WormsGLRenderer::createBioPolymerMap(const QMap<int, SharedMolecule>& moleculeMap, QMap<int, BioPolymer>& bioPolymerMap) {
    const char* alphaCarbonTag = "CA";
    const char* carbonylOxygenTag = "O";

    QMapIterator<int, SharedMolecule> i(moleculeMap);
    while (i.hasNext()) {
        i.next();
        const SharedMolecule mol = i.value();
        BioPolymer bioPolymer;
        QList<int> modelIds = mol->models.keys();
        for (int modelId : qAsConst(modelIds)) {
            const Molecule3DModel& model = mol->models.value(modelId);
            BioPolymerModel& bpModel = bioPolymer.bpModels[modelId];
            bpModel.biostruct3DModelId = modelId;
            QHash<int, Monomer> monomerByResideMap;
            for (const SharedAtom& atom : qAsConst(model.atoms)) {
                if (atom->name == alphaCarbonTag || atom->name == carbonylOxygenTag) {
                    int residueIdx = atom->residueIndex.toInt();
                    if (!monomerByResideMap.contains(residueIdx)) {
                        monomerByResideMap[residueIdx] = Monomer();
                    }
                    if (atom->name == alphaCarbonTag) {
                        monomerByResideMap[residueIdx].alphaCarbon = atom;
                    } else {
                        monomerByResideMap[residueIdx].carbonylOxygen = atom;
                    };
                }
            }
            // Register complete monomers in the model.
            QList<int> linkResideIndexes = monomerByResideMap.keys();
            for (int residueIdx : qAsConst(linkResideIndexes)) {
                const Monomer& monomer = monomerByResideMap[residueIdx];
                if (monomer.alphaCarbon.data() != nullptr && monomer.carbonylOxygen.data() != nullptr) {
                    bpModel.monomerMap[residueIdx] = monomer;
                }
            }
            if (bpModel.monomerMap.isEmpty()) {
                bioPolymer.bpModels.remove(modelId);
            }
        }
        if (!bioPolymer.bpModels.isEmpty()) {
            bioPolymerMap.insert(i.key(), bioPolymer);
        }
    }
}

WormsGLRenderer::~WormsGLRenderer() {
    for (const Worm& worm : qAsConst(wormMap)) {
        QList<WormModel> wormModels = worm.wormModelByBioStruct3DModelId.values();
        for (const WormModel& model : qAsConst(wormModels)) {
            qDeleteAll(model.objects);
        }
    }
}

const float* WormsGLRenderer::getAtomColor(const SharedAtom& atom) {
    atomColor = colorScheme->getAtomColor(atom);
    return atomColor.getConstData();
}

Object3D* WormsGLRenderer::createHelix3D(int startId, int endId, const BioPolymerModel& bpModel) {
    static float radius = 1.5f;
    QVector<Vector3D> helixPoints;
    Color4f color(0, 0, 0, 0);

    for (int i = startId; i <= endId; ++i) {
        if (!bpModel.monomerMap.contains(i))
            continue;
        helixPoints.append(bpModel.monomerMap.value(i).alphaCarbon->coord3d);

        // getting "average" color
        const Color4f c = colorScheme->getAtomColor(bpModel.monomerMap.value(i).alphaCarbon);
        for (int colorIndex = 0; colorIndex < 4; ++colorIndex) {
            color[colorIndex] += c[colorIndex];
        }
    }
    QPair<Vector3D, Vector3D> axis = calcBestAxisThroughPoints(helixPoints);

    // getting "average" color
    for (int i = 0; i < 4; ++i) {
        color[i] /= (endId - startId + 1);
    }

    return new Helix3D(color, axis.first, axis.second, radius);
}

Object3D* WormsGLRenderer::createStrand3D(int startId, int endId, const BioPolymerModel& bpModel) {
    QVector<Vector3D> arrowPoints;
    Color4f color(0, 0, 0, 0);

    for (int i = startId; i <= endId; ++i) {
        if (!bpModel.monomerMap.contains(i))
            continue;
        arrowPoints.append(bpModel.monomerMap.value(i).alphaCarbon->coord3d);

        // getting "average" color
        const Color4f c = colorScheme->getAtomColor(bpModel.monomerMap.value(i).alphaCarbon);
        for (int colorIndex = 0; colorIndex < 4; ++colorIndex) {
            color[colorIndex] += c[colorIndex];
        }
    }
    QPair<Vector3D, Vector3D> axis = calcBestAxisThroughPoints(arrowPoints);

    // getting "average" color
    for (int i = 0; i < 4; ++i) {
        color[i] /= (endId - startId + 1);
    }

    return new Strand3D(color, axis.first, axis.second);
}

}  // namespace U2
