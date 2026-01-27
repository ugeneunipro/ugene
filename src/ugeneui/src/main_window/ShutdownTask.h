/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once
#include <U2Core/Task.h>

namespace U2 {

class Document;
class MainWindowImpl;

class ShutdownTask : public Task {
    Q_OBJECT
public:
    ShutdownTask(MainWindowImpl* mw);

    void prepare() override;

    ReportResult report() override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    void cancelShutdown();

    MainWindowImpl* mw;
    bool docsToRemoveAreFetched;
    QList<Document*> docsToRemove;
};

class CloseWindowsTask : public Task {
    Q_OBJECT
public:
    CloseWindowsTask();

private:
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;
};

class CancelAllTask : public Task {
    Q_OBJECT
public:
    CancelAllTask();

private:
    void prepare() override;
    ReportResult report() override;
};

}  // namespace U2
