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

#ifndef _U2_REPEAT_PLUGIN_H_
#define _U2_REPEAT_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class RepeatFinderPlugin : public Plugin {
    Q_OBJECT
public:
    RepeatFinderPlugin();

private:
    GObjectViewWindowContext* viewCtx;
};

class RepeatViewContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    RepeatViewContext(QObject* p);

protected slots:
    void sl_showDialog();
    void sl_showTandemDialog();

protected:
    virtual void initViewContext(GObjectView* view);
};


} //namespace

#endif
