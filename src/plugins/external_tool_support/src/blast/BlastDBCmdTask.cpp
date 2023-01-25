/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "BlastDBCmdTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/OpenViewTask.h>

#include "BlastSupport.h"

namespace U2 {

void BlastDBCmdSupportTaskSettings::reset() {
    query = "";
    outputPath = "";
    databasePath = "";
    isNuclDatabase = true;
    addToProject = false;
}

BlastDBCmdTask::BlastDBCmdTask(const BlastDBCmdSupportTaskSettings& _settings)
    : Task("Run NCBI BlastDBCmd task", TaskFlags_NR_FOSCOE), settings(_settings) {
    GCOUNTER(cvar, "BlastDBCmdSupportTask");
    blastDBCmdTask = nullptr;
    toolId = BlastSupport::ET_BLASTDBCMD_ID;
}

void BlastDBCmdTask::prepare() {
    QStringList arguments;

    arguments << "-db" << settings.databasePath;
    arguments << "-dbtype" << (settings.isNuclDatabase ? "nucl" : "prot");
    arguments << "-entry" << settings.query;
    arguments << "-logfile" << settings.outputPath + ".BlastDBCmd.log";
    arguments << "-out" << settings.outputPath;

    blastDBCmdTask = new ExternalToolRunTask(toolId, arguments, new ExternalToolLogParser());
    blastDBCmdTask->setSubtaskProgressWeight(95);
    addSubTask(blastDBCmdTask);
}

QList<Task*> BlastDBCmdTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK(subTask == blastDBCmdTask, res);
    if (settings.addToProject) {
        IOAdapterFactory* ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        auto loadTask = new LoadDocumentTask(BaseDocumentFormats::FASTA, settings.outputPath, ioFactory);
        res.append(new AddDocumentAndOpenViewTask(loadTask));
    }
    return res;
}

}  // namespace U2
