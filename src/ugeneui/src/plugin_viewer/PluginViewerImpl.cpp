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

#include "PluginViewerImpl.h"
#include <U2Gui/MainWindow.h>

#include <AppContextImpl.h>
#include "PluginViewerController.h"

#include <QAction>

namespace U2 {

Task* PluginViewerImpl::createServiceEnablingTask() {
	return new EnablePluginViewerTask(this);
}

Task* PluginViewerImpl::createServiceDisablingTask() {
	return new DisablePluginViewerTask(this);	
}

//////////////////////////////////////////////////////////////////////////
// tasks

EnablePluginViewerTask::EnablePluginViewerTask(PluginViewerImpl* _pv)
: Task (tr("Enable PluginViewer"), TaskFlag_NoRun), pv(_pv)
{
}

Task::ReportResult EnablePluginViewerTask::report() {
	AppContextImpl::getApplicationContext()->setPluginViewer(pv);
	pv->viewer = new PluginViewerController();
	return ReportResult_Finished;
}


DisablePluginViewerTask::DisablePluginViewerTask(PluginViewerImpl* _pv)
: Task (tr("Disable PluginViewer"), TaskFlag_NoRun), pv(_pv)
{
}

Task::ReportResult DisablePluginViewerTask::report() {
    AppContextImpl::getApplicationContext()->setPluginViewer(NULL);
    delete pv->viewer;
    pv->viewer = NULL;
	return ReportResult_Finished;
}


}//namespace
