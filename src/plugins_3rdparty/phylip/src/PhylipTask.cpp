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

#include <U2Core/CmdlineInOutTaskRunner.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>
#include "NeighborJoinAdapter.h"

#include "PhylipTask.h"

namespace U2 {

PhylipTask::PhylipTask(const U2EntityRef &msaRef, const U2DbiRef &outDbiRef, const CreatePhyTreeSettings &settings)
: CmdlineTask(tr("PHYLIP task"), TaskFlags_NR_FOSE_COSC), msaRef(msaRef), outDbiRef(outDbiRef), settings(settings), treeTask(NULL)
{
}

void PhylipTask::prepare() {
    MultipleSequenceAlignmentObject *msaObject = new MultipleSequenceAlignmentObject("msa", msaRef);
    msaObject->setParent(this);

    treeTask = new NeighborJoinCalculateTreeTask(msaObject->getMultipleAlignment(), settings);
    addSubTask(treeTask);
}

Task::ReportResult PhylipTask::report() {
    CmdlineTask::report();
    CHECK_OP(stateInfo, ReportResult_Finished);

    CmdlineInOutTaskRunner::logOutputObject(saveTree());
    return ReportResult_Finished;
}

U2DataId PhylipTask::saveTree() {
    PhyTreeObject *treeObject = PhyTreeObject::createInstance(treeTask->getResult(), "Tree", outDbiRef, stateInfo);
    CHECK_OP(stateInfo, U2DataId());

    U2DataId treeId = treeObject->getEntityRef().entityId;
    delete treeObject;
    return treeId;
}

} // U2
