/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/PluginModel.h>

#include <U2Gui/ObjectViewModel.h>

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
    HMMMSAEditorContext* ctxMSA;
    HMMADVContext* ctxADV;
};

class HMMMSAEditorContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    HMMMSAEditorContext(QObject* p);

protected slots:
    void sl_build();

protected:
    void initViewContext(GObjectViewController* view) override;
    void buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) override;
};

class HMMADVContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    HMMADVContext(QObject* p);

protected slots:
    void sl_search();

protected:
    void initViewContext(GObjectViewController* view) override;
};

}  // namespace U2
