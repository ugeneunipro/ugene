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

#include <AppContextImpl.h>

#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>

#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include "ProjectImpl.h"
#include "ProjectServiceImpl.h"

namespace U2 {

#define SETTINGS_DIR QString("project_loader/")

ProjectServiceImpl::ProjectServiceImpl(Project* _pr)
    : ProjectService(tr("Project"), tr("Project service is available when opened a project file. Other services that depends on Project service will be automatically started after this service is enabled.")) {
    pr = _pr;
    assert(pr != nullptr);
}

ProjectServiceImpl::~ProjectServiceImpl() {
    delete pr;
}

Task* ProjectServiceImpl::saveProjectTask(SaveProjectTaskKind k) {
    return nullptr;
}

Task* ProjectServiceImpl::closeProjectTask() {
    return nullptr;
}

void ProjectServiceImpl::enableSaveAction(bool) {
}

Task* ProjectServiceImpl::createServiceEnablingTask() {
    return new ProjectServiceEnableTask(this);
}

Task* ProjectServiceImpl::createServiceDisablingTask() {
    return new ProjectServiceDisableTask(this);
}

//////////////////////////////////////////////////////////////////////////
/// Service tasks

ProjectServiceEnableTask::ProjectServiceEnableTask(ProjectServiceImpl* _psi)
    : Task(tr("Enable Project"), TaskFlag_NoRun), psi(_psi) {
}

Task::ReportResult ProjectServiceEnableTask::report() {
    AppContextImpl::getApplicationContext()->setProjectService(psi);
    AppContextImpl::getApplicationContext()->setProject(psi->getProject());

    return ReportResult_Finished;
}

ProjectServiceDisableTask::ProjectServiceDisableTask(ProjectServiceImpl* _psi)
    : Task(tr("Disable Project"), TaskFlag_NoRun), psi(_psi) {
}

Task::ReportResult ProjectServiceDisableTask::report() {
    AppContextImpl::getApplicationContext()->setProject(nullptr);
    AppContextImpl::getApplicationContext()->setProjectService(nullptr);

    return ReportResult_Finished;
}

}  // namespace U2
