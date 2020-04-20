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

#ifndef _U2_BIOSTRUCT3D_WORMS_RENDERER_H_
#define _U2_BIOSTRUCT3D_WORMS_RENDERER_H_

#include <QSharedDataPointer>
#include <QVector>
#include <QMap>
#include <QPair>

#include <U2Core/Vector3D.h>
#include "BioStruct3DGLRender.h"
#include "GraphicUtils.h"

class Object3D;
class AtomData;

namespace U2 {

class WormsGLRenderer : public BioStruct3DGLRenderer {


    // BioPolymer molecule

    struct Monomer
    {
        Monomer() : alphaCarbon(NULL), carbonylOxygen(NULL) {}
        QSharedDataPointer<AtomData> alphaCarbon;
        QSharedDataPointer<AtomData> carbonylOxygen;
    };

    struct BioPolymerModel
    {
        QMap<int,Monomer> monomerMap;
    };

    struct BioPolymer
    {
        // multiple models
        QMap<int, BioPolymerModel> bpModels;
    };

    QMap<int, BioPolymer > bioPolymerMap;


    // Worm Model

    typedef QVector<SharedAtom> AtomsVector;

    struct WormModel {
        //! These coords required to draw worms endings correctly
        Vector3D openingAtom, closingAtom;
        // Worm building atom coords
        AtomsVector atoms;
        // Objects representing secondary structure
        QVector<Object3D*>  objects;
    };

    struct Worm {
        QVector<WormModel> models;
    };

    QMap<int, Worm> wormMap;
    Color4f atomColor;


    void createObjects3D();
    Object3D* createStrand3D( int startId, int endId, const BioPolymerModel &bpModel );
    Object3D* createHelix3D( int startId, int endId, const BioPolymerModel &bpModel );
    void createWorms();
    void drawWorms();
    void drawSecondaryStructure();
    const float* getAtomColor(const SharedAtom& atom);

private:
    static void createBioPolymerMap(const QMap <int, SharedMolecule>& moleculeMap, QMap<int, BioPolymer>& bioPolymerMap);

protected:
    WormsGLRenderer(const BioStruct3D& struc, const BioStruct3DColorScheme* s, const QList<int> &shownModels, const BioStruct3DRendererSettings *settings);

public:
    virtual ~WormsGLRenderer();

    virtual void create();
    virtual void drawBioStruct3D();

    virtual void updateColorScheme();
    virtual void updateShownModels();
    virtual void updateSettings();

public:
    static bool isAvailableFor(const BioStruct3D &);

    RENDERER_FACTORY(WormsGLRenderer)
};

} //namespace

#endif // _U2_BIOSTRUCT3D_WORMS_RENDERER_H_
