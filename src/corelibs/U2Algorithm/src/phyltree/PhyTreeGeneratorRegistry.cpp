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

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

namespace U2 {

    PhyTreeGeneratorRegistry::PhyTreeGeneratorRegistry( QObject* pOwn /* = 0*/ ) : QObject(pOwn)
    {}

    PhyTreeGeneratorRegistry::~PhyTreeGeneratorRegistry()
    {
        foreach( PhyTreeGenerator* generator, genMap.values()) {
            delete generator;
        }
    }

    bool PhyTreeGeneratorRegistry::registerPhyTreeGenerator( PhyTreeGenerator* generator , const QString& gen_id)
    {
        if (genMap.contains(gen_id)){
            return false;
        }
        genMap.insert(gen_id, generator);
        return true;

    }

    bool PhyTreeGeneratorRegistry::hadRegistered( const QString& gen_id)
    {
        return genMap.contains(gen_id);
    }

    PhyTreeGenerator* PhyTreeGeneratorRegistry::getGenerator(const QString& gen_id)
    {
        if (genMap.contains(gen_id)) {
            return genMap.value(gen_id);
        } else {
            return NULL;
        }
    }

    QStringList PhyTreeGeneratorRegistry::getNameList()
    {
        return genMap.keys();
    }

} // namespace U2
