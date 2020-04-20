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

#ifndef _U2_PERF_VIEW_H_
#define _U2_PERF_VIEW_H_

#include <U2Gui/MainWindow.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace U2 {

class GCounter;
class PerfTreeItem;

class PerfMonitorView : public MWMDIWindow {
    Q_OBJECT
public:
    PerfMonitorView();

protected:
    void timerEvent(QTimerEvent *e);

private:
    void updateCounters();
    PerfTreeItem* findCounterItem(const GCounter* c) const;
    QTreeWidget* tree;
};

class PerfTreeItem : public QTreeWidgetItem {
public:
    PerfTreeItem(GCounter* counter);
    GCounter* counter;
    void updateVisual();
};

} //namespace

#endif
