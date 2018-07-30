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

#ifndef _U2_GENOME_ASSEMBLY_MULTI_TASK_
#define _U2_GENOME_ASSEMBLY_MULTI_TASK_

#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Core/Task.h>
#include <U2Core/GUrl.h>

namespace U2 {

class Document;

class U2ALGORITHM_EXPORT GenomeAssemblyMultiTask : public Task {
    Q_OBJECT
public:
    GenomeAssemblyMultiTask(const GenomeAssemblyTaskSettings& settings);
    virtual void prepare();
    virtual ReportResult report();
    virtual QString generateReport() const;
    QString getResultUrl() const;
    QList<Task*> onSubTaskFinished(Task* subTask);
    const GenomeAssemblyTaskSettings& getSettings() const {return settings;}
    GenomeAssemblyTask* getAssemblyTask() const;
private:
    GenomeAssemblyTaskSettings settings;
    GenomeAssemblyTask* assemblyTask;
};

} // namespace
#endif
