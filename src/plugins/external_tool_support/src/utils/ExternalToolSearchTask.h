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

#ifndef _U2_EXTERNAL_TOOL_SEARCH_TASK_H_
#define _U2_EXTERNAL_TOOL_SEARCH_TASK_H_

#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>

class ExternalTool;

namespace U2 {

/**
  * Search the possible paths to the external tool.
  * Search order:
  * 1. Tools folder (%UGENE_DIR%/tools)
  * 2. PATH variable
  * 3. Current tool's path (if it isn't empty)
  **/
class ExternalToolSearchTask : public Task {
    Q_OBJECT
public:
    ExternalToolSearchTask(const QString& toolId);

    virtual void run();
    QString getToolId() const { return toolId; }
    QStringList getPaths() const { return toolPaths; }

private:
    QString getExeName(ExternalTool *tool);

    QString toolId;
    QStringList toolPaths;

    static const QString TOOLS;
};

class ExternalToolsSearchTask : public SequentialMultiTask {
    Q_OBJECT
public:
    ExternalToolsSearchTask(const QList<Task*> &_tasks);

    virtual QList<Task*> onSubTaskFinished(Task* subTask);
};

}   // namespace U2

#endif // _U2_EXTERNAL_TOOL_SEARCH_TASK_H_
