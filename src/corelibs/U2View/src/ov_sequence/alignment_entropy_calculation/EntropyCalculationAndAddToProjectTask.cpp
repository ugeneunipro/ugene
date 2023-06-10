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

#include "EntropyCalculationAndAddToProjectTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

namespace U2 {
EntropyCalculationAndAddToProjectTask::EntropyCalculationAndAddToProjectTask(const QString& _alignmentFilePath,
                                                                             const QString& _saveToPath,
                                                                             const QString& _originalFilePath,
                                                                             DNASequence& _sequence,
                                                                             int _chainId,
                                                                             bool _addToProject)
    : Task(tr("Alignment entropy calculation and adding to project"), TaskFlags_NR_FOSE_COSC),
      alignmentFilePath(_alignmentFilePath),
      saveToPath(_saveToPath),
      originalFilePath(_originalFilePath),
      sequence(_sequence),
      chainId(_chainId),
      addToProject(_addToProject) {
}

void EntropyCalculationAndAddToProjectTask::prepare() {
    entropyCalculationTask = new EntropyCalculationTask(alignmentFilePath, saveToPath, originalFilePath, sequence, chainId);
    addSubTask(entropyCalculationTask);
}

QList<Task*> EntropyCalculationAndAddToProjectTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);
    if (subTask == entropyCalculationTask) {
        if (addToProject) {
            Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(saveToPath);
            res << openTask;
        }
    }
    return res;
}

}  // namespace U2
