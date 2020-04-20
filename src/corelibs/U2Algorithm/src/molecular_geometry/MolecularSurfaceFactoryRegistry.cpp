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
#include "MolecularSurfaceFactoryRegistry.h"

namespace U2 {

MolecularSurfaceFactoryRegistry::MolecularSurfaceFactoryRegistry( QObject* pOwn /* = 0*/ ) : QObject(pOwn)
{
    registerSurfaceFactory(new VanDerWaalsSurfaceFactory(),QString("vdWS"));
}

MolecularSurfaceFactoryRegistry::~MolecularSurfaceFactoryRegistry()
{
    foreach( MolecularSurfaceFactory* factory, surfMap.values()) {
        delete factory;
    }
}

bool MolecularSurfaceFactoryRegistry::registerSurfaceFactory( MolecularSurfaceFactory* surf, const QString& surfId )
{
    if (surfMap.contains(surfId)){
        return false;
    }
    surfMap.insert(surfId, surf);
    return true;

}

bool MolecularSurfaceFactoryRegistry::hadRegistered( const QString& surfId )
{
    return surfMap.contains(surfId);
}

MolecularSurfaceFactory* MolecularSurfaceFactoryRegistry::getSurfaceFactory( const QString& surfId )
{
    if (surfMap.contains(surfId)) {
        return surfMap.value(surfId);
    } else {
        return NULL;
    }
}

QStringList MolecularSurfaceFactoryRegistry::getSurfNameList()
{
    return surfMap.keys();
}

} // namespace U2
