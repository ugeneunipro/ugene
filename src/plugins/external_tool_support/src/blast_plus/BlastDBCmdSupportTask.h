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

#ifndef _U2_BLAST_DB_CMD_SUPPORT_TASK_H
#define _U2_BLAST_DB_CMD_SUPPORT_TASK_H

#include <U2Core/Task.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/SaveDocumentTask.h>
#include "utils/ExportTasks.h"

#include <U2Core/MultipleSequenceAlignmentObject.h>

namespace U2 {

class BlastDBCmdSupportTaskSettings {
public:
    BlastDBCmdSupportTaskSettings() {reset();}
    void reset();

    QString         query;
    QString         outputPath;
    QString         databasePath;
    bool            isNuclDatabase;
    bool            addToProject;
};


class BlastDBCmdSupportTask : public Task {
    Q_OBJECT
public:
    BlastDBCmdSupportTask(const BlastDBCmdSupportTaskSettings& settings);
    void prepare();
    Task::ReportResult report();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
private:
    ExternalToolRunTask*        blastDBCmdTask;
    BlastDBCmdSupportTaskSettings settings;
    QString toolId;
};


}//namespace
#endif // _U2_BLAST_DB_CMD_SUPPORT_TASK_H
