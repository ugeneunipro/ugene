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

#pragma once

#include <QAction>
#include <QMap>

#include <U2Core/AppContext.h>
#include <U2Core/PluginModel.h>

#include <U2Gui/ObjectViewModel.h>

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

class BioStruct3DViewContext : public GObjectViewWindowContext {
    Q_OBJECT
    QMap<GObjectViewController*, BioStruct3DSplitter*> splitterMap;

public:
    BioStruct3DViewContext(QObject* p);

    bool canHandle(GObjectViewController* v, GObject* o) override;

    void onObjectAdded(GObjectViewController* v, GObject* obj) override;
    void onObjectRemoved(GObjectViewController* v, GObject* obj) override;

protected:
    void initViewContext(GObjectViewController* view) override;

    void unregister3DView(GObjectViewController* view, BioStruct3DSplitter* view3d);

    QAction* getClose3DViewAction(GObjectViewController* view);

protected slots:
    void sl_close3DView();
    virtual void sl_windowClosing(MWMDIWindow*);
};

}  // namespace U2
