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

#ifndef PHYTREEGENERATORREGISTRY_H
#define PHYTREEGENERATORREGISTRY_H

#include "PhyTreeGenerator.h"

namespace U2 {

    class U2ALGORITHM_EXPORT PhyTreeGeneratorRegistry : public QObject
    {
    public:
        PhyTreeGeneratorRegistry(QObject* pOwn = 0);
        ~PhyTreeGeneratorRegistry();
        bool registerPhyTreeGenerator(PhyTreeGenerator* generator, const QString& gen_id);
        bool hadRegistered(const QString& surfId);
        PhyTreeGenerator* getGenerator(const QString& surfId);
        QStringList getNameList();
    private:
        QMap<QString, PhyTreeGenerator*> genMap;
        // Copy prohibition
        PhyTreeGeneratorRegistry(const PhyTreeGeneratorRegistry& m);
        PhyTreeGeneratorRegistry& operator=(const PhyTreeGeneratorRegistry& m);
    };

} // namespace U2

#endif
