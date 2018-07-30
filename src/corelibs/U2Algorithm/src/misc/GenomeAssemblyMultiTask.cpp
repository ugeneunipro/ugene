/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "GenomeAssemblyMultiTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Algorithm/DnaAssemblyAlgRegistry.h>

#include <U2Gui/OpenViewTask.h>
#include <U2Gui/MainWindow.h>

#include <QMainWindow>
#include <QMessageBox>

namespace U2 {

GenomeAssemblyMultiTask::GenomeAssemblyMultiTask( const GenomeAssemblyTaskSettings& s)
: Task("GenomeAssemblyMultiTask", TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled), settings(s),
assemblyTask(NULL)
{

}

void GenomeAssemblyMultiTask::prepare() {
    // perform assembly
    QString algName = settings.algName;
    GenomeAssemblyAlgorithmEnv* env= AppContext::getGenomeAssemblyAlgRegistry()->getAlgorithm(algName);
    assert(env);
    if (env == NULL) {
        setError(QString("Algorithm %1 is not found").arg(algName));
        return;
    }
    assemblyTask = env->getTaskFactory()->createTaskInstance(settings);
    addSubTask(assemblyTask);
}

Task::ReportResult GenomeAssemblyMultiTask::report() {
    return ReportResult_Finished;
}

QList<Task*> GenomeAssemblyMultiTask::onSubTaskFinished( Task* subTask ) {
    QList<Task*> subTasks;
    if (subTask->hasError() || isCanceled()) {
        return subTasks;
    }

    if (subTask == assemblyTask) {
        qint64 time=(subTask->getTimeInfo().finishTime - subTask->getTimeInfo().startTime);
        taskLog.details(QString("Assembly task time: %1").arg((double)time/(1000*1000)));
    }

    if ( subTask == assemblyTask && settings.openView ) {
        if (assemblyTask->hasResult()) {
            Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(assemblyTask->getResultUrl());
            if (openTask != NULL) {
                subTasks << openTask;
            }
        } else {
            QString message = tr("Assembly cannot be performed.");
            coreLog.info(message);
            if (NULL != AppContext::getMainWindow()) {
                QMessageBox::information(AppContext::getMainWindow()->getQMainWindow(), L10N::warningTitle(), message);
            }
        }
    }

    return subTasks;
}


U2::GenomeAssemblyTask* GenomeAssemblyMultiTask::getAssemblyTask() const {
    return assemblyTask;
}

QString GenomeAssemblyMultiTask::generateReport() const {
    QString res;
    if (hasError()) {
        return tr("Assembly task finished with error: %1").arg(getError());
    }
    CHECK(assemblyTask != NULL, tr("Assembly task wasn't set"));

    if (assemblyTask->hasResult()) {
        res = tr("Assembly was finished successfully");
    } else {
        res = tr("Assembly failed.");
    }
    return res;
}

QString GenomeAssemblyMultiTask::getResultUrl() const {
    if(assemblyTask && assemblyTask->isFinished() && !assemblyTask->hasError()){
        return assemblyTask->getResultUrl();
    }
    return "";
}

} // namespace
