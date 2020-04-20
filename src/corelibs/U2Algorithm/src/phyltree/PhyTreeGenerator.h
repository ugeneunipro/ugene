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

#ifndef _U2_PHY_TREE_GENERATOR_
#define _U2_PHY_TREE_GENERATOR_

#include <U2Core/Task.h>
#include <U2Core/PhyTree.h>
#include <U2Core/MultipleSequenceAlignment.h>

#include "CreatePhyTreeSettings.h"

namespace U2 {

class CreatePhyTreeWidget;
class CreatePhyTreeDialogController;

class U2ALGORITHM_EXPORT PhyTreeGenerator {
public:
    virtual ~PhyTreeGenerator() {}

    virtual Task * createCalculatePhyTreeTask(const MultipleSequenceAlignment &ma, const CreatePhyTreeSettings &s) = 0;
    virtual CreatePhyTreeWidget * createPhyTreeSettingsWidget(const MultipleSequenceAlignment &ma, QWidget *parent = NULL) = 0;
};

}   // namespace U2

#endif // _U2_PHY_TREE_GENERATOR_
