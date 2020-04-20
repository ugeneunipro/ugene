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

#ifndef _U2_UHMMER_PLUGIN_H_
#define _U2_UHMMER_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Core/AppContext.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMenu>

namespace U2 {

class XMLTestFactory;
class HMMMSAEditorContext;
class HMMADVContext;

class uHMMPlugin : public Plugin {
    Q_OBJECT
public:
    uHMMPlugin();
    ~uHMMPlugin();

private slots:
    void sl_build();
    void sl_calibrate();
    void sl_search();

private:
    HMMMSAEditorContext*    ctxMSA;
    HMMADVContext*          ctxADV;
};

class HMMMSAEditorContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    HMMMSAEditorContext(QObject* p);

protected slots:
    void sl_build();

protected:
    virtual void initViewContext(GObjectView* view);
    virtual void buildMenu(GObjectView* v, QMenu* m);
};

class HMMADVContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    HMMADVContext(QObject* p);

protected slots:
    void sl_search();

protected:
    virtual void initViewContext(GObjectView* view);
};

} //namespace

#endif
