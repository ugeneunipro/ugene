/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_DNA_STAT_PLUGIN_H_
#define _U2_DNA_STAT_PLUGIN_H_

#include <U2Core/PluginModel.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class DNAStatPlugin : public Plugin {
    Q_OBJECT
public:
    DNAStatPlugin();

private:
    GObjectViewWindowContext *statViewCtx;
    GObjectViewWindowContext *distanceViewCtx;
};

class DNAStatMSAEditorContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    DNAStatMSAEditorContext(QObject *p);

protected slots:
    void sl_showMSAProfileDialog();
    void buildStaticOrContextMenu(GObjectView *view, QMenu *menu) override;

protected:
    void initViewContext(GObjectView *view) override;

private:
    /* Alignment length limint for opening grid report in UGENE */
    static constexpr int GRID_PROFILE_LENGTH_LIMIT = 20000;
};

class DistanceMatrixMSAEditorContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    DistanceMatrixMSAEditorContext(QObject *p);

protected slots:
    void sl_showDistanceMatrixDialog();
    void buildStaticOrContextMenu(GObjectView *view, QMenu *menu) override;

protected:
    void initViewContext(GObjectView *view) override;
};

}  // namespace U2

#endif
