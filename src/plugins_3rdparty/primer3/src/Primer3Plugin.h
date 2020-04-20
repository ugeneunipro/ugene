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

#ifndef _PRIMER3_PLUGIN_H_
#define _PRIMER3_PLUGIN_H_ 1
#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMenu>

#include "Primer3Tests.h"

namespace U2 {

class Primer3ADVContext;
class XMLTestFactory;

class Primer3Plugin : public Plugin {
    Q_OBJECT
public:
    Primer3Plugin();
    ~Primer3Plugin();

private:
    Primer3ADVContext* viewCtx;
};

class Primer3ADVContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    Primer3ADVContext(QObject* p);
protected slots:
        void sl_showDialog();
protected:
    virtual void initViewContext(GObjectView* v);
    //virtual void makeBaseMenu(GObjectView* v, QMenu* m);
};

class Primer3Tests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} //namespace
#endif //_PRIMER3_PLUGIN_H_
