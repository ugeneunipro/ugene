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

#ifndef _U2_SITECON_PLUGIN_H_
#define _U2_SITECON_PLUGIN_H_

#include "DIPropertiesSitecon.h"

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMenu>

namespace U2 {

class SiteconADVContext;
class XMLTestFactory;

class SiteconPlugin : public Plugin {
    Q_OBJECT
public:
    SiteconPlugin();
    ~SiteconPlugin();

    static QList<DiPropertySitecon*> getDinucleotiteProperties() {return dp.getProperties();}

protected slots:
    void sl_build();
    void sl_search();

private:
    SiteconADVContext*                  ctxADV;
    static DinucleotitePropertyRegistry dp;
};


class SiteconADVContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    SiteconADVContext(QObject* p);

protected slots:
    void sl_search();

protected:
    virtual void initViewContext(GObjectView* view);
};

class SiteconAlgorithmTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} //namespace

#endif
