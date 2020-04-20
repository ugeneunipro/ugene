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

#include "BlastDBCmdSupportTask.h"
#include "BlastDBCmdSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/Log.h>

#include <U2Gui/OpenViewTask.h>

namespace U2 {

void BlastDBCmdSupportTaskSettings::reset() {
    query = "";
    outputPath = "";
    databasePath = "";
    isNuclDatabase = true;
    addToProject = false;
}

BlastDBCmdSupportTask::BlastDBCmdSupportTask(const BlastDBCmdSupportTaskSettings& _settings) :
        Task("Run NCBI BlastDBCmd task", TaskFlags_NR_FOSCOE),settings(_settings)
{
    GCOUNTER( cvar, tvar, "BlastDBCmdSupportTask" );
    blastDBCmdTask = nullptr;
    toolId = BlastDbCmdSupport::ET_BLASTDBCMD_ID;
}

void BlastDBCmdSupportTask::prepare(){

    QStringList arguments;

    arguments << "-db" << settings.databasePath;
    arguments << "-dbtype" << (settings.isNuclDatabase ? "nucl" : "prot");
    arguments << "-entry" << settings.query;
    arguments << "-logfile" << settings.outputPath+".BlastDBCmd.log";
    arguments << "-out" << settings.outputPath;

    blastDBCmdTask = new ExternalToolRunTask(toolId, arguments, new ExternalToolLogParser());
    blastDBCmdTask->setSubtaskProgressWeight(95);
    addSubTask(blastDBCmdTask);
}
Task::ReportResult BlastDBCmdSupportTask::report(){
    return ReportResult_Finished;
}

QList<Task*> BlastDBCmdSupportTask::onSubTaskFinished( Task* subTask )
{
    QList<Task*> res;

    if (subTask == blastDBCmdTask) {
        if (settings.addToProject) {
            IOAdapterFactory * iow = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
            LoadDocumentTask* loadTask = new LoadDocumentTask(BaseDocumentFormats::FASTA, settings.outputPath, iow);
            res.append( new AddDocumentAndOpenViewTask(loadTask) );
        }
    }

    return res;
}

}//namespace
