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

#ifndef _U2_MAKE_BLAST_DB_TASK_H
#define _U2_MAKE_BLAST_DB_TASK_H

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include "utils/ExportTasks.h"

namespace U2 {

class PrepareInputFastaFilesTask;

class MakeBlastDbSettings {
public:
    MakeBlastDbSettings() {
        reset();
    }
    void reset();

    QStringList inputFilesPath;
    QString outputPath;
    QString databaseTitle;
    QString tempDirPath;
    bool isInputAmino;
};

class MakeBlastDbTask : public Task {
    Q_OBJECT
public:
    MakeBlastDbTask(const MakeBlastDbSettings& settings);

private:
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    Task::ReportResult report() override;
    QString generateReport() const override;

    QString prepareTempDir();
    QString prepareLink(const QString& path) const;
    void initMakeBlastDbExternalToolTask();

    QString externalToolLog;
    PrepareInputFastaFilesTask* prepareTask = nullptr;
    ExternalToolRunTask* makeBlastDbExternalToolTask = nullptr;
    MakeBlastDbSettings settings;

    QStringList inputFastaFiles;
    QStringList fastaTmpFiles;
};

}  // namespace U2

#endif  // _U2_MAKE_BLAST_DB_TASK_H
