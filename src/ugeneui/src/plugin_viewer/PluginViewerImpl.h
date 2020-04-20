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

#ifndef _U2_PLUGIN_VEIWER_IMPL_H_

#include <U2Gui/PluginViewer.h>
#include <U2Core/Task.h>

namespace U2 {

class PluginViewerController;
class EnablePluginViewerTask;
class DisablePluginViewerTask;

class PluginViewerImpl : public PluginViewer {
    Q_OBJECT

    friend class DisablePluginViewerTask;
	friend class EnablePluginViewerTask;
public:
    PluginViewerImpl() : PluginViewer(tr("PluginViewer"), tr("Basic plugins and services visualization and manipulation tool")) {viewer = NULL;}

protected:
	virtual Task* createServiceEnablingTask();

	virtual Task* createServiceDisablingTask();

private:
    PluginViewerController* viewer;

};

//////////////////////////////////////////////////////////////////////////
/// Tasks
class EnablePluginViewerTask : public Task {
    Q_OBJECT

public:
	EnablePluginViewerTask(PluginViewerImpl* pv);
	virtual ReportResult report();
private:
	PluginViewerImpl* pv;
};


class DisablePluginViewerTask : public Task {
    Q_OBJECT

public:
	DisablePluginViewerTask(PluginViewerImpl* pv);
	virtual ReportResult report();
private:
	PluginViewerImpl* pv;
};


}//namespace

#endif
