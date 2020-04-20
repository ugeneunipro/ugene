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

#ifndef MOLECULARSURFACEFACTORYREGISTRY_H
#define MOLECULARSURFACEFACTORYREGISTRY_H

#include "MolecularSurface.h"

namespace U2 {

class U2ALGORITHM_EXPORT MolecularSurfaceFactoryRegistry : public QObject
{
public:
    MolecularSurfaceFactoryRegistry(QObject* pOwn = 0);
    ~MolecularSurfaceFactoryRegistry();
    bool registerSurfaceFactory(MolecularSurfaceFactory* surf, const QString& surfId);
    bool hadRegistered(const QString& surfId);
    MolecularSurfaceFactory* getSurfaceFactory(const QString& surfId);
    QStringList getSurfNameList();
private:
    QMap<QString, MolecularSurfaceFactory*> surfMap;
    // Copy prohibition
    MolecularSurfaceFactoryRegistry(const MolecularSurfaceFactoryRegistry& m);
    MolecularSurfaceFactoryRegistry& operator=(const MolecularSurfaceFactoryRegistry& m);
};

} // namespace U2

#endif // MOLECULARSURFACEFACTORYREGISTRY_H
