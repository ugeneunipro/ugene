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

#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVSequenceWidget.h>

namespace U2 {

class DotPlotSplitter;
class DotPlotWidget;

class DotPlotPlugin : public Plugin {
    Q_OBJECT
public:
    DotPlotPlugin();

private:
    GObjectViewWindowContext* viewCtx;

private slots:
    void sl_initDotPlotView();
};

class DotPlotViewContext : public GObjectViewWindowContext {
    Q_OBJECT

public:
    DotPlotViewContext(QObject* p);

protected:
    void initViewContext(GObjectViewController* view) override;

    void createSplitter();
    void buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) override;
    DotPlotSplitter* getView(GObjectViewController* view, bool create);
    void removeDotPlotView(GObjectViewController* view);

private slots:
    void sl_buildDotPlot();
    void sl_removeDotPlot();

    void sl_showDotPlotDialog();
    void sl_loadTaskStateChanged(Task* task);
    void sl_windowActivated(MWMDIWindow* w);

private:
    void showBuildDotPlotDialog(GObjectViewController* v);

    bool createdByWizard;
    QString firstFile;
    QString secondFile;
};

#define DOTPLOT_ACTION_NAME "DOTPLOT_ACTION"

class DotPlotViewAction : public ADVSequenceWidgetAction {
    Q_OBJECT
public:
    DotPlotViewAction()
        : ADVSequenceWidgetAction(DOTPLOT_ACTION_NAME, tr("Show dot plot")), view(nullptr) {
    }
    ~DotPlotViewAction() {
    }
    DotPlotWidget* view;
};

}  // namespace U2
