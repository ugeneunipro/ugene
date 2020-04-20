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

#ifndef _U2_PHYLIP_TASK_H_
#define _U2_PHYLIP_TASK_H_

#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/U2Type.h>

namespace U2 {

class NeighborJoinCalculateTreeTask;

class PhylipTask : public CmdlineTask {
    Q_OBJECT
public:
    PhylipTask(const U2EntityRef &msaRef, const U2DbiRef &outDbiRef, const CreatePhyTreeSettings &settings);

    void prepare();
    ReportResult report();

private:
    U2DataId saveTree();

private:
    U2EntityRef msaRef;
    U2DbiRef outDbiRef;
    CreatePhyTreeSettings settings;

    NeighborJoinCalculateTreeTask *treeTask;
};

} // U2

#endif // _U2_PHYLIP_TASK_H_
