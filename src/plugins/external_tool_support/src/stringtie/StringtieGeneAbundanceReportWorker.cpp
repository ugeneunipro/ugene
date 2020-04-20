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

#include <U2Core/FailTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/TaskSignalMapper.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowMonitor.h>

#include "StringtieGeneAbundanceReportTask.h"
#include "StringtieGeneAbundanceReportWorker.h"
#include "StringtieGeneAbundanceReportWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

StringtieGeneAbundanceReportWorker::StringtieGeneAbundanceReportWorker(Actor *actor)
    : BaseWorker(actor, false)
{

}

void StringtieGeneAbundanceReportWorker::init() {
    input = ports.value(StringtieGeneAbundanceReportWorkerFactory::INPUT_PORT_ID);
    SAFE_POINT(NULL != input, QString("Port with id '%1' is NULL")
               .arg(StringtieGeneAbundanceReportWorkerFactory::INPUT_PORT_ID), );
}

Task *StringtieGeneAbundanceReportWorker::tick() {
    bool noMessage = true;
    bool portIsEnded = true;

    if (input->hasMessage()) {
        noMessage = false;
        while (input->hasMessage()) {
            Message message = getMessageAndSetupScriptValues(input);
            const QString stringtieReport = message.getData()
                    .toMap()[BaseSlots::URL_SLOT().getId()].toString();
            if (stringtieReport.isEmpty()) {
                setDone();
                return new FailTask(tr("An empty URL to StringTie report passed to the '%1'")
                                    .arg(getActor()->getLabel()));
            }
            stringtieReports << stringtieReport;
        }
    }
    if (!input->isEnded()) {
        portIsEnded = false;
    }

    if (noMessage && portIsEnded) {
        if (stringtieReports.size() > 0) {
            const QString geneAbudanceReportUrl = getValue<QString>(StringtieGeneAbundanceReportWorkerFactory::OUTPUT_FILE_ATTR_ID);
            FileAndDirectoryUtils::createWorkingDir(geneAbudanceReportUrl,
                                                    FileAndDirectoryUtils::FILE_DIRECTORY,
                                                    "",
                                                    "");
            StringtieGeneAbundanceReportTask *task = new StringtieGeneAbundanceReportTask(stringtieReports,
                                                                                          geneAbudanceReportUrl,
                                                                                          context->workingDir());
            stringtieReports.clear();
            connect(new TaskSignalMapper(task),
                    SIGNAL(si_taskSucceeded(Task *)),
                    SLOT(sl_taskSucceeded(Task *)));
            return task;
        }

        if (portIsEnded) {
            setDone();
            algoLog.info(QString("Filter worker is done as input was ended"));
        }
    }

    return NULL;
}

void StringtieGeneAbundanceReportWorker::cleanup() {
}

void StringtieGeneAbundanceReportWorker::sl_taskSucceeded(Task *task) {
    StringtieGeneAbundanceReportTask *geneAbudanceReportTask = qobject_cast<StringtieGeneAbundanceReportTask *>(task);
    SAFE_POINT(NULL != geneAbudanceReportTask, "StringTieGeneAbundanceReportTask is NULL", );

    const QString geneAbudanceReportUrl = geneAbudanceReportTask->getReportUrl();
    monitor()->addOutputFile(geneAbudanceReportUrl, getActor()->getId(), true);
}

}   // namespace LocalWorkflow
}   // namespace U2
