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

#include <U2Core/AppContext.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVSequenceWidget.h>

namespace U2 {

class ChromatogramView;

class ChromaViewPlugin : public Plugin {
    Q_OBJECT
public:
    ChromaViewPlugin();
    ~ChromaViewPlugin();

private:
    GObjectViewWindowContext* viewCtx;
};

class ChromaViewContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    ChromaViewContext(QObject* p);

    bool canHandle(GObjectViewController* v, GObject* o) override;
protected slots:
    void sl_showChromatogram();
    void sl_sequenceWidgetAdded(ADVSequenceWidget*);

protected:
    void initViewContext(GObjectViewController* view) override;
};

class ChromaViewAction : public ADVSequenceWidgetAction {
    Q_OBJECT
public:
    ChromaViewAction();
    ChromatogramView* view;
};

}  // namespace U2
