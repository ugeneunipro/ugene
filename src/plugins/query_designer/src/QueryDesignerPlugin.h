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

#ifndef _U2_QUERY_DESIGNER_PLUGIN_H_
#define _U2_QUERY_DESIGNER_PLUGIN_H_

#include <U2Gui/ObjectViewModel.h>
#include <U2Core/PluginModel.h>
#include <U2Core/ServiceModel.h>
#include <U2Core/ServiceTypes.h>


namespace U2 {

#define QUERY_SAMPLES_PATH "/query_samples"

class Task;
class QueryDesignerPlugin : public Plugin {
    Q_OBJECT
public:
    QueryDesignerPlugin();
private:
    void registerLibFactories();
private:
    GObjectViewWindowContext* viewCtx;
};

class QueryDesignerViewContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    QueryDesignerViewContext(QObject* p);
protected:
    virtual void initViewContext(GObjectView* view);
private slots:
    void sl_showDialog();
};

class QueryDesignerService : public Service {
    Q_OBJECT
public:
    QueryDesignerService() : Service(Service_QueryDesigner, tr("Query Designer"), "") {}
    bool closeViews();
protected:
    virtual Task* createServiceEnablingTask();
    virtual Task* createServiceDisablingTask();
    virtual void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged);
private slots:
    void sl_startQDPlugin();
    void sl_showDesignerWindow();
};

} //namespace

#endif
