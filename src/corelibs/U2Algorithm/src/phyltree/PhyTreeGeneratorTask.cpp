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

#include <U2Core/DocumentModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include "PhyTreeGeneratorTask.h"
#include "PhyTreeGeneratorRegistry.h"


namespace U2 {

PhyTreeGeneratorTask::PhyTreeGeneratorTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings)
: Task(PhyTreeGeneratorTask::tr("Calculating Phylogenetic Tree"), TaskFlag_FailOnSubtaskError), inputMA(ma), settings(_settings)
{
    tpm = Task::Progress_Manual;
}

void PhyTreeGeneratorTask::run() {
}

Task::ReportResult PhyTreeGeneratorTask::report() {
    return ReportResult_Finished;
}

PhyTreeGeneratorLauncherTask::PhyTreeGeneratorLauncherTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings)
:Task(PhyTreeGeneratorLauncherTask::tr("Calculating Phylogenetic Tree"), TaskFlag_FailOnSubtaskError), inputMA(ma->getCopy()), settings(_settings), task(NULL){
    tpm = Task::Progress_SubTasksBased;
}
void PhyTreeGeneratorLauncherTask::prepare(){
    QString algId = settings.algorithm;
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    PhyTreeGenerator* generator = registry->getGenerator(algId);
    assert(generator!=NULL);
    if (generator == NULL) {
        stateInfo.setError(PhyTreeGeneratorLauncherTask::tr("Tree construction algorithm %1 not found").arg(algId));
    }else{
        const QStringList& rowsOrder = settings.rowsOrder;
        if(rowsOrder.size() >= inputMA->getRowNames().size()) {
            inputMA->sortRowsByList(rowsOrder);
        }

        namesConvertor.replaceNamesWithAlphabeticIds(inputMA);

        task = dynamic_cast<PhyTreeGeneratorTask*>(generator->createCalculatePhyTreeTask(inputMA,settings));
        addSubTask(task);
    }

}
Task::ReportResult PhyTreeGeneratorLauncherTask::report(){
    if(task){
        result = task->getResult();
        namesConvertor.restoreNames(result);
    }
    return ReportResult_Finished;
}

void PhyTreeGeneratorLauncherTask::sl_onCalculationCanceled() {
    cancel();
}

void SeqNamesConvertor::replaceNamesWithAlphabeticIds(MultipleSequenceAlignment& ma) {
    QStringList rows = ma->getRowNames();

    int rowsNum = ma->getNumRows();
    for(int i = 0; i < rowsNum; i++) {
        namesMap[generateNewAlphabeticId()] = rows.at(i);
        ma->renameRow(i, lastIdStr);
    }
}
void SeqNamesConvertor::restoreNames(const PhyTree& tree) {
    if(!tree) {
        return;
    }
    QList<const PhyNode*> nodes = tree->collectNodes();
    foreach(const PhyNode* node, nodes) {
        QString restoredName = namesMap[node->getName()];
        if(!restoredName.isEmpty()) {
            PhyNode* renamedNode = const_cast<PhyNode*>(node);
            renamedNode->setName(restoredName);
        }
    }
}

const QString& SeqNamesConvertor::generateNewAlphabeticId() {
    int idSize = lastIdStr.size();
    for(int i = idSize - 1; i >= 0; i--) {
        char curChar = lastIdStr.at(i).toLatin1();
        if(curChar < 'z') {
            lastIdStr[i] = curChar + 1;
            return lastIdStr;
        }
        else {
            lastIdStr[i] = 'a';
        }
    }

    lastIdStr.append('a');
    lastIdStr.fill('a');

    return lastIdStr;
}


} //namespace
