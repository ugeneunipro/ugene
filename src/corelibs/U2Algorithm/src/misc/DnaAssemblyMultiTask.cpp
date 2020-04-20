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

#include "DnaAssemblyMultiTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>

#include <U2Algorithm/DnaAssemblyAlgRegistry.h>

#include <U2Gui/OpenViewTask.h>
#include <U2Gui/MainWindow.h>

#include <QMainWindow>
#include <QMessageBox>

namespace U2 {

DnaAssemblyMultiTask::DnaAssemblyMultiTask(const DnaAssemblyToRefTaskSettings& s, bool view, bool _justBuildIndex)
: ExternalToolSupportTask("DnaAssemblyMultiTask", TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled), settings(s),
assemblyToRefTask(NULL), shortReadSets(s.shortReadSets), openView(view), justBuildIndex(_justBuildIndex) {}

void DnaAssemblyMultiTask::prepare() {
    // perform assembly
    QString algName = settings.algName;
    DnaAssemblyAlgorithmEnv* env= AppContext::getDnaAssemblyAlgRegistry()->getAlgorithm(algName);
    assert(env);
    if (env == NULL) {
        setError(QString("Algorithm %1 is not found").arg(algName));
        return;
    }
    assemblyToRefTask = env->getTaskFactory()->createTaskInstance(settings, justBuildIndex);
    assemblyToRefTask->addListeners(getListeners());
    addSubTask(assemblyToRefTask);
}

Task::ReportResult DnaAssemblyMultiTask::report() {
    return ReportResult_Finished;
}

QList<Task*> DnaAssemblyMultiTask::onSubTaskFinished( Task* subTask ) {
    QList<Task*> subTasks;
    if (subTask->hasError() || isCanceled()) {
        return subTasks;
    }

    if (subTask == assemblyToRefTask) {
        qint64 time=(subTask->getTimeInfo().finishTime - subTask->getTimeInfo().startTime);
        taskLog.details(QString("Align to reference task time: %1").arg((double)time/(1000*1000)));
    }

    if ( subTask == assemblyToRefTask && settings.openView ) {
        if (assemblyToRefTask->hasResult()) {
            Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(settings.resultFileName);
            if (openTask != NULL) {
                subTasks << openTask;
            }
        } else {
            QString message = tr("The short reads can't be mapped to the reference sequence!");
            coreLog.info(message);
            if (NULL != AppContext::getMainWindow()) {
                QMessageBox::information(AppContext::getMainWindow()->getQMainWindow(), L10N::warningTitle(), message);
            }
        }
    }

    return subTasks;
}


QString DnaAssemblyMultiTask::generateReport() const {
    QString res;
    if (hasError()) {
        return QString("Alignment task finished with error: %1").arg(getError());
    }

    if (justBuildIndex) {
        res = settings.algName + QString(" index-file for %1 was built successfully")
        .arg(settings.refSeqUrl.fileName());
    } else if (assemblyToRefTask->hasResult()) {
        res = QString("Alignment to reference %1 was finished successfully")
        .arg(settings.refSeqUrl.fileName());
    } else {
        res = QString("Alignment to reference %1 was failed. No possible alignment was found")
        .arg(settings.refSeqUrl.fileName());
    }
    return res;
}



} // namespace
