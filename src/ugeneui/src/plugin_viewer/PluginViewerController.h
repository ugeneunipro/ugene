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

#include <U2Core/PluginModel.h>
#include <U2Gui/MainWindow.h>
#include "ui_PluginViewerWidget.h"

namespace U2 {

class PlugViewPluginItem;
class PlugViewServiceItem;

class PluginViewerController : public QObject {
    Q_OBJECT
public:
    PluginViewerController();
    ~PluginViewerController();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private slots:

    void sl_show();
    void sl_enableService();
    void sl_disableService();

    void sl_treeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

    void sl_onServiceStateChanged(Service* s, ServiceState oldState);
    void sl_onServiceUnregistered(Service* s);
    void sl_showHideLicense();
    void sl_acceptLicense();

private:
    void connectStaticActions();
    void connectVisualActions();
    void disconnectVisualActions();
    void buildItems() const;
    void createWindow();
    void updateActions();
    void updateState();
    void showLicense() const;
    void hideLicense() const;

    PlugViewServiceItem* findServiceItem(Service* s) const;
    PlugViewServiceItem* getCurrentServiceItem() const;

    Ui_PluginViewWidget ui;

    MWMDIWindow* mdiWindow = nullptr;
    QAction* enableServiceAction = nullptr;
    QAction* disableServiceAction = nullptr;

    bool showServices = false;
};

class PlugViewTreeItem : public QTreeWidgetItem {
public:
    PlugViewTreeItem(PlugViewTreeItem* parent)
        : QTreeWidgetItem(parent) {
    }

    bool isRootItem() {
        return parent() == nullptr;
    }
    virtual void updateVisual() = 0;
    virtual bool isPluginItem() const {
        return false;
    }
    virtual bool isServiceItem() const {
        return false;
    }
};

class PlugViewPluginItem : public PlugViewTreeItem {
public:
    PlugViewPluginItem(PlugViewTreeItem* parent, Plugin* p, bool showServices);
    virtual bool isPluginItem() const {
        return true;
    }
    void updateVisual();
    Plugin* plugin;
    bool showServices;
};

class PlugViewServiceItem : public PlugViewTreeItem {
public:
    PlugViewServiceItem(PlugViewPluginItem* parent, Service* s);
    virtual bool isServiceItem() const {
        return true;
    }
    void updateVisual();

    Service* service;
};
}  // namespace U2
