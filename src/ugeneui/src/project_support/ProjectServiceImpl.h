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

#ifndef _U2_PROJECT_SERVICE_IMPL_H_
#define _U2_PROJECT_SERVICE_IMPL_H_

#include <U2Core/ProjectService.h>
#include <U2Core/Task.h>

#include <QAction>

namespace U2 {

class ProjectServiceImpl : public ProjectService {
    Q_OBJECT

    friend class ProjectServiceEnableTask;
	friend class ProjectServiceDisableTask;
public:
	ProjectServiceImpl(Project* pr); 
	~ProjectServiceImpl();

	virtual Project* getProject() const {return pr;}

	virtual Task* saveProjectTask(SaveProjectTaskKind k);

	virtual Task* closeProjectTask();

    virtual void enableSaveAction(bool e);

protected:
	virtual Task* createServiceEnablingTask();
	virtual Task* createServiceDisablingTask();

private slots:
	void sl_save();
    void sl_saveAs();
	void sl_closeProject();
	void sl_exportProject();
	
private:
	QAction* saveAction;
    QAction* saveAsAction;
	QAction* closeProjectAction;
    QAction* projectActionsSeparator;
	QAction* exportProjectAction;

	Project* pr;
};


//////////////////////////////////////////////////////////////////////////
/// Tasks
class ProjectServiceEnableTask : public Task {
    Q_OBJECT
public:
	ProjectServiceEnableTask(ProjectServiceImpl* psi);
	virtual ReportResult report();
private:
	ProjectServiceImpl* psi;
};


class ProjectServiceDisableTask : public Task {
    Q_OBJECT
public:
	ProjectServiceDisableTask(ProjectServiceImpl* psi);
	virtual ReportResult report();
private:
	ProjectServiceImpl* psi;
};

}//namespace

#endif

