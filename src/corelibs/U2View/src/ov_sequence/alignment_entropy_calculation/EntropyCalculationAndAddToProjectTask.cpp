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
EntropyCalculationAndAddToProjectTask::EntropyCalculationAndAddToProjectTask(AnnotatedDNAView * _annotatedDNAView,
                                                                              const QString& _alignmentFilePath,
                                                                              const QString& _saveToPath,
                                                                              const QString& _alignmentAlgorithm,
                                                                              bool _addToProject)
    : Task(tr("Alignment entropy calculation and adding to project"), TaskFlags_FOSE_COSC),
      annotatedDNAView(_annotatedDNAView),
      alignmentFilePath(_alignmentFilePath),
      saveToPath(_saveToPath),
      alignmentAlgorithm(_alignmentAlgorithm),
      addToProject(_addToProject) {
}

void EntropyCalculationAndAddToProjectTask::prepare() {
    entropyCalculationTask = new EntropyCalculationTask(annotatedDNAView, alignmentFilePath, saveToPath, alignmentAlgorithm);
    addSubTask(entropyCalculationTask);
    CHECK_OP(stateInfo, );
}

void EntropyCalculationAndAddToProjectTask::run() {
}

QList<Task*> EntropyCalculationAndAddToProjectTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask == entropyCalculationTask) {
        if (addToProject) {
            Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(saveToPath);
            CHECK_OP(stateInfo, res);
            res << openTask;
        }
    }
    return res;
}

}  // namespace U2
