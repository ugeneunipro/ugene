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

#ifndef _U2_ENTROPY_CALCULATION_AND_ADD_TO_PROJECT_TASK_H
#define _U2_ENTROPY_CALCULATION_AND_ADD_TO_PROJECT_TASK_H

#include "EntropyCalculationTask.h"

#include <U2Core/Task.h>

namespace U2 {
class U2VIEW_EXPORT EntropyCalculationAndAddToProjectTask : public Task {
    Q_OBJECT
public :
    EntropyCalculationAndAddToProjectTask(AnnotatedDNAView* annotatedDNAView,
        const QString& alignmentFilePath,
        const QString& saveToPath,
        bool addToProject);
    void prepare();
    void run();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    EntropyCalculationTask* entropyCalculationTask = nullptr;

    AnnotatedDNAView* annotatedDNAView;
    const QString alignmentFilePath;
    const QString saveToPath;
    const bool addToProject;
};

}  // namespace U2

#endif  // _U2_ENTROPY_CALCULATION_AND_ADD_TO_PROJECT_TASK_H

