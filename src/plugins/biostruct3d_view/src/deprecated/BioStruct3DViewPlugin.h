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

#ifndef _U2_BIOSTRUCT3D_VIEW_PLUGIN_H_
#define _U2_BIOSTRUCT3D_VIEW_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Core/AppContext.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMap>
#include <QAction>

namespace U2 {

class BioStruct3DSplitter;
class BioStruct3DObject;

class BioStruct3DViewPlugin : public Plugin {
    Q_OBJECT
public:
    BioStruct3DViewPlugin();
    ~BioStruct3DViewPlugin();
private:
    GObjectViewWindowContext* viewContext;
};

class BioStruct3DViewContext: public GObjectViewWindowContext {
    Q_OBJECT
    QMap<GObjectView*, BioStruct3DSplitter*> splitterMap;
public:
    BioStruct3DViewContext(QObject* p);

    virtual bool canHandle(GObjectView* v, GObject* o);

    virtual void onObjectAdded(GObjectView* v, GObject* obj);
    virtual void onObjectRemoved(GObjectView* v, GObject* obj);

protected:
    virtual void initViewContext(GObjectView* view);

    void unregister3DView(GObjectView* view, BioStruct3DSplitter* view3d);

    QAction* getClose3DViewAction(GObjectView* view);


protected slots:
    void sl_close3DView();
    virtual void sl_windowClosing(MWMDIWindow*);

};

} //namespace

#endif
