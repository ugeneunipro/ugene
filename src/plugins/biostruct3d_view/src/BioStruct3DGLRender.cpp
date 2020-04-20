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

#include "BioStruct3DColorScheme.h"
#include "BioStruct3DGLRender.h"
#include "BallAndStickGLRenderer.h"
#include "TubeGLRenderer.h"
#include "VanDerWaalsGLRenderer.h"
#include "WormsGLRenderer.h"

namespace U2 {

/* class BioStruct3DGLRendererRegistry */
const QString BioStruct3DGLRendererRegistry::defaultFactoryName() {
    return WormsGLRenderer::ID;
}

const QList<QString> BioStruct3DGLRendererRegistry::factoriesNames() {
    return getInstance()->factories.keys();
}

const BioStruct3DGLRendererFactory* BioStruct3DGLRendererRegistry::getFactory(const QString &name) {
    return getInstance()->factories.value(name, 0);
}

BioStruct3DGLRenderer* BioStruct3DGLRendererRegistry::createRenderer(const QString &name, const BioStruct3D &bs, const BioStruct3DColorScheme* s, const QList<int> &sm, const BioStruct3DRendererSettings *settings) {
    const BioStruct3DGLRendererFactory *fact = getFactory(name);

    if (fact) {
        return fact->createInstance(bs,s,sm,settings);
    }

    return 0;
}

BioStruct3DGLRendererRegistry::BioStruct3DGLRendererRegistry() {
    registerFactories();
}

BioStruct3DGLRendererRegistry* BioStruct3DGLRendererRegistry::getInstance() {
    static BioStruct3DGLRendererRegistry *reg = new BioStruct3DGLRendererRegistry();
    return reg;
}

bool BioStruct3DGLRendererRegistry::isAvailableFor(const QString &name, const BioStruct3D &biostruct) {
    const BioStruct3DGLRendererFactory *fact = getFactory(name);
    return fact->isAvailableFor(biostruct);
}

QList<QString> BioStruct3DGLRendererRegistry::getRenderersAvailableFor(const BioStruct3D &biostruct) {
    BioStruct3DGLRendererRegistry *reg = getInstance();

    QList<QString> ret;
    foreach (const BioStruct3DGLRendererFactory *fact, reg->factories) {
        if (fact->isAvailableFor(biostruct)) {
            ret.append(fact->getName());
        }
    }

    return ret;
}


#define REGISTER_FACTORY(c) factories.insert(c::ID, new c::Factory(c::ID))
void BioStruct3DGLRendererRegistry::registerFactories() {
    REGISTER_FACTORY(BallAndStickGLRenderer);
    REGISTER_FACTORY(VanDerWaalsGLRenderer);
    REGISTER_FACTORY(TubeGLRenderer);
    REGISTER_FACTORY(WormsGLRenderer);
}


/* class BioStruct3DGLRenderer */
BioStruct3DGLRenderer::BioStruct3DGLRenderer( const BioStruct3D& _bioStruct, const BioStruct3DColorScheme* _s, const QList<int> &_shownModels, const BioStruct3DRendererSettings *_settings)
        : bioStruct(_bioStruct), colorScheme(_s), shownModels(_shownModels), settings(_settings)
{}

void BioStruct3DGLRenderer::setColorScheme( const BioStruct3DColorScheme* s )
{
     colorScheme = s;
     updateColorScheme();
}

} //namespace
