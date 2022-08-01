/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "PhyTreeGeneratorTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DocumentModel.h>

#include "PhyTreeGeneratorRegistry.h"

namespace U2 {

PhyTreeGeneratorTask::PhyTreeGeneratorTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings, const TaskFlags& taskFlags)
    : Task(PhyTreeGeneratorTask::tr("Calculating Phylogenetic Tree"), taskFlags), inputMA(ma), settings(_settings) {
    tpm = Task::Progress_Manual;
}

const PhyTree& PhyTreeGeneratorTask::getResult() const {
    return result;
}

const CreatePhyTreeSettings& PhyTreeGeneratorTask::getSettings() const {
    return settings;
}

PhyTreeGeneratorLauncherTask::PhyTreeGeneratorLauncherTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings)
    : Task(PhyTreeGeneratorLauncherTask::tr("Calculating Phylogenetic Tree"), TaskFlag_NoRun | TaskFlag_FailOnSubtaskError),
      inputMA(ma->getCopy()), settings(_settings), task(nullptr) {
    tpm = Task::Progress_SubTasksBased;
}

static const QString RENAMED_ROW_PREFIX = "r";

void PhyTreeGeneratorLauncherTask::prepare() {
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    PhyTreeGenerator* generator = registry->getGenerator(settings.algorithm);
    CHECK_EXT(generator != nullptr, stateInfo.setError(PhyTreeGeneratorLauncherTask::tr("Tree algorithm %1 is not found").arg(settings.algorithm)), );

    // Assign unique names to rows. The row name is a string representation of the index + 'row_' prefix.
    originalRowNameByIndex = inputMA->getRowNames();
    int rowsCount = inputMA->getRowCount();
    for (int i = 0; i < rowsCount; i++) {
        QString uniqueRowName = RENAMED_ROW_PREFIX + QString::number(i);
        inputMA->renameRow(i, uniqueRowName);
    }
    task = qobject_cast<PhyTreeGeneratorTask*>(generator->createCalculatePhyTreeTask(inputMA, settings));
    SAFE_POINT(task != nullptr, "Not a PhyTreeGeneratorTask!", );
    addSubTask(task);
}

Task::ReportResult PhyTreeGeneratorLauncherTask::report() {
    CHECK(!stateInfo.isCoR() && task != nullptr && !task->getStateInfo().isCoR(), ReportResult_Finished);
    PhyTree tree = task->getResult();
    SAFE_POINT(tree != nullptr, "Tree is not present!", ReportResult_Finished);
    QList<PhyNode*> nodes = tree->getNodesPreOrder();
    for (PhyNode* node : qAsConst(nodes)) {
        bool ok = false;
        QString rowName = node->getName();
        CHECK_CONTINUE(rowName.startsWith(RENAMED_ROW_PREFIX));  // Filter inner nodes. Name may be empty or equal to the distance (PhyML).
        int index = rowName.mid(RENAMED_ROW_PREFIX.length()).toInt(&ok);
        CHECK_EXT(ok && index >= 0 && index < originalRowNameByIndex.length(),
                  setError(tr("Failed to map row name: %1").arg(rowName)),
                  ReportResult_Finished);
        QString originalRowName = originalRowNameByIndex[index];
        node->setName(originalRowName);
    }
    result = tree;
    return ReportResult_Finished;
}

void PhyTreeGeneratorLauncherTask::sl_onCalculationCanceled() {
    cancel();
}

const PhyTree& PhyTreeGeneratorLauncherTask::getResult() const {
    return result;
}

}  // namespace U2
