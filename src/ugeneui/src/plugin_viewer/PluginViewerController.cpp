/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PluginViewerController.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// controller

/* TRANSLATOR U2::PluginViewerController */

PluginViewerController::PluginViewerController() {
    connectStaticActions();
}

PluginViewerController::~PluginViewerController() {
    AppContext::getPluginSupport()->disconnect(this);
    if (mdiWindow) {
        AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(mdiWindow);
        assert(mdiWindow == nullptr);  // must set to nullptr on close event
    }
}

void PluginViewerController::createWindow() {
    assert(mdiWindow == nullptr);

    mdiWindow = new MWMDIWindow(tr("Plugin Viewer"));
    ui.setupUi(mdiWindow);
    ui.treeWidget->setColumnWidth(1, 200);  // todo: save geom

    if (!showServices) {
        ui.treeWidget->hideColumn(0);
    }

    QList<int> sizes;
    sizes << 200 << 500;
    // ui.splitter->setSizes(sizes);
    ui.licenseLabel->hide();
    ui.licenseView->hide();
    ui.acceptLicenseButton->hide();
    ui.showLicenseButton->setDisabled(true);

    ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connectVisualActions();
    buildItems();

    ui.treeWidget->setSortingEnabled(true);
    ui.treeWidget->sortByColumn(1, Qt::AscendingOrder);

    mdiWindow->installEventFilter(this);
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(mdiWindow);

    if (ui.treeWidget->topLevelItemCount() > 0) {
        ui.treeWidget->setCurrentItem(ui.treeWidget->findItems("", Qt::MatchContains).first());
    }

    updateState();
}

void PluginViewerController::connectStaticActions() {
    // add actions to menu and toolbar
    MainWindow* mw = AppContext::getMainWindow();
    QMenu* pluginsMenu = mw->getTopLevelMenu(MWMENU_SETTINGS);

    auto viewPluginsAction = new QAction(QIcon(":ugene/images/plugins.png"), tr("Plugins..."), this);
    connect(viewPluginsAction, SIGNAL(triggered()), SLOT(sl_show()));
    viewPluginsAction->setObjectName(ACTION__PLUGINS_VIEW);
    pluginsMenu->addAction(viewPluginsAction);

    enableServiceAction = new QAction(tr("Enable service"), this);
    connect(enableServiceAction, SIGNAL(triggered()), SLOT(sl_enableService()));

    disableServiceAction = new QAction(tr("Disable service"), this);
    connect(disableServiceAction, SIGNAL(triggered()), SLOT(sl_disableService()));
}

void PluginViewerController::connectVisualActions() {
    // connect to plugin support signals
    if (showServices) {
        ServiceRegistry* sr = AppContext::getServiceRegistry();
        connect(sr, SIGNAL(si_serviceStateChanged(Service*, ServiceState)), SLOT(sl_onServiceStateChanged(Service*, ServiceState)));
        connect(sr, SIGNAL(si_serviceUnregistered(Service*)), SLOT(sl_onServiceUnregistered(Service*)));
    }

    connect(ui.treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(sl_treeCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    connect(ui.showLicenseButton, SIGNAL(clicked()), SLOT(sl_showHideLicense()));
    connect(ui.acceptLicenseButton, SIGNAL(clicked()), SLOT(sl_acceptLicense()));
}

void PluginViewerController::disconnectVisualActions() {
    AppContext::getPluginSupport()->disconnect(this);
    AppContext::getServiceRegistry()->disconnect(this);

    // ui.treeWidget and all button/menus are disconnected automatically -> widget is deleted
}

void PluginViewerController::updateActions() {
    auto item = static_cast<PlugViewTreeItem*>(ui.treeWidget->currentItem());

    bool isService = item != nullptr && item->isServiceItem();
    auto s = isService ? (static_cast<PlugViewServiceItem*>(item))->service : nullptr;
    bool isServiceEnabled = isService && s->isEnabled();

    enableServiceAction->setEnabled(isService && !isServiceEnabled);
    disableServiceAction->setEnabled(isService && isServiceEnabled);
}

void PluginViewerController::buildItems() const {
    const QList<Plugin*>& plugins = AppContext::getPluginSupport()->getPlugins();
    foreach (Plugin* p, plugins) {
        QTreeWidget* treeWidget = ui.treeWidget;
        auto pluginItem = new PlugViewPluginItem(nullptr, p, showServices);
        if (showServices) {
            QList<Service*> services = p->getServices();
            // this method is called for default state init also -> look for registered plugin services
            ServiceRegistry* sr = AppContext::getServiceRegistry();
            QList<Service*> registered = sr->getServices();
            for (Service* s : qAsConst(services)) {
                if (registered.contains(s)) {
                    PlugViewTreeItem* serviceItem = new PlugViewServiceItem(pluginItem, s);
                    pluginItem->addChild(serviceItem);
                }
            }
        }
        treeWidget->addTopLevelItem(pluginItem);
        pluginItem->setExpanded(true);
    }
}

PlugViewServiceItem* PluginViewerController::findServiceItem(Service* /*s*/) const {
    /*PlugViewPluginItem* pi = findPluginItem(s->getPlugin());
    if (!pi) {
        return NULL;
    }
    int nServices = pi->childCount();
    for (int i=0;i<nServices;i++) {
        PlugViewServiceItem* item = (PlugViewServiceItem*)pi->child(i);
        if (item->service == s) {
            return item;
        }
    }*/
    return nullptr;
}

bool PluginViewerController::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Close && obj == mdiWindow) {
        mdiWindow = nullptr;
        disconnectVisualActions();
    }
    return QObject::eventFilter(obj, event);
}

/*
void PluginViewerController::sl_onPluginAdded(Plugin* p) {
    assert(findPluginItem(p)==NULL);

    QTreeWidget* treeWidget = ui.treeWidget;
    PlugViewPluginItem* pluginItem = new PlugViewPluginItem(NULL, p, showServices);
    if (showServices) {
        const QList<Service*>& services = p->getServices();
        //this method is called for default state init also -> look for registered plugin services
        ServiceRegistry* sr = AppContext::getServiceRegistry();
        QList<Service*> registered = sr->getServices();
        foreach(Service* s, services) {
            if (registered.contains(s)) {
                PlugViewTreeItem* serviceItem = new PlugViewServiceItem(pluginItem, s);
                pluginItem->addChild(serviceItem);
            }
        }
    }
    treeWidget->addTopLevelItem(pluginItem);
    pluginItem->setExpanded(true);
}
*/

void PluginViewerController::sl_onServiceStateChanged(Service* s, ServiceState oldState) {
    Q_UNUSED(oldState);
    assert(showServices);
    PlugViewServiceItem* si = findServiceItem(s);
    assert(si != nullptr);
    si->updateVisual();
    updateState();
}

void PluginViewerController::sl_onServiceUnregistered(Service* s) {
    assert(showServices);
    PlugViewServiceItem* item = findServiceItem(s);
    assert(item != nullptr);
    delete item;
}

void PluginViewerController::sl_show() {
    if (mdiWindow == nullptr) {
        createWindow();
    } else {
        AppContext::getMainWindow()->getMDIManager()->activateWindow(mdiWindow);
    }
}

PlugViewServiceItem* PluginViewerController::getCurrentServiceItem() const {
    auto item = static_cast<PlugViewTreeItem*>(ui.treeWidget->currentItem());
    assert(item != nullptr && item->isServiceItem());
    auto si = static_cast<PlugViewServiceItem*>(item);
    return si;
}

void PluginViewerController::sl_enableService() {
    assert(showServices);
    PlugViewServiceItem* si = getCurrentServiceItem();
    if (si == nullptr || si->service->isEnabled()) {
        return;
    }
    Task* task = AppContext::getServiceRegistry()->enableServiceTask(si->service);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void PluginViewerController::sl_disableService() {
    PlugViewServiceItem* si = getCurrentServiceItem();
    if (si == nullptr || si->service->isDisabled()) {
        return;
    }
    Task* task = AppContext::getServiceRegistry()->disableServiceTask(si->service);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void PluginViewerController::updateState() {
    updateActions();
    ui.infoView->clear();
    auto item = static_cast<PlugViewTreeItem*>(ui.treeWidget->currentItem());
    if (item == nullptr) {
        ui.infoView->setText(tr("Select a plugin to view more information about it."));
        return;
    }
    QString text;
    if (item->isPluginItem()) {
        auto p = static_cast<PlugViewPluginItem*>(item);
        text = "<b>" + p->plugin->getName() + "</b><p>";
        text += p->plugin->getDescription();
    } else {
        assert(item->isServiceItem());
        auto s = static_cast<PlugViewServiceItem*>(item);
        text = s->service->getDescription();
    }
    ui.infoView->setText(text);
}

void PluginViewerController::sl_treeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) {
    if (current == previous) {
        return;
    }
    ui.showLicenseButton->setEnabled(true);
    auto currentItem = dynamic_cast<PlugViewPluginItem*>(current);
    if (!currentItem->plugin->isFree()) {
        if (currentItem->plugin->isLicenseAccepted()) {
            hideLicense();
        } else {
            showLicense();
        }
    } else {
        hideLicense();
    }
    updateState();
}

void PluginViewerController::sl_showHideLicense() {
    if (ui.licenseView->isHidden()) {
        showLicense();
    } else {
        hideLicense();
    }
}
void PluginViewerController::sl_acceptLicense() {
    auto currentItem = dynamic_cast<PlugViewPluginItem*>(ui.treeWidget->currentItem());
    showLicense();
    AppContext::getPluginSupport()->setLicenseAccepted(currentItem->plugin);
    hideLicense();
}

void PluginViewerController::showLicense() const {
    ui.showLicenseButton->setText(tr("Hide License"));
    ui.licenseView->show();
    ui.licenseLabel->show();
    auto currentItem = dynamic_cast<PlugViewPluginItem*>(ui.treeWidget->currentItem());
    ui.acceptLicenseButton->setHidden(!currentItem || currentItem->plugin->isFree() || currentItem->plugin->isLicenseAccepted());

    // Opening the license file.
    QFile licenseFile(currentItem->plugin->getLicensePath().getURLString());
    if (!licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui.licenseView->setText(tr("License file not found."));
    } else {
        ui.licenseView->setText(QString(licenseFile.readAll()));
        licenseFile.close();
    }
}

void PluginViewerController::hideLicense() const {
    ui.showLicenseButton->setText(tr("Show License"));
    ui.licenseView->hide();
    ui.licenseLabel->hide();
    ui.acceptLicenseButton->hide();
}
//////////////////////////////////////////////////////////////////////////
// TreeItems

PlugViewPluginItem::PlugViewPluginItem(PlugViewTreeItem* parent, Plugin* p, bool _showServices)
    : PlugViewTreeItem(parent), plugin(p), showServices(_showServices) {
    updateVisual();
}

void PlugViewPluginItem::updateVisual() {
    setData(0, Qt::DisplayRole, PluginViewerController::tr("Plugin"));
    setData(1, Qt::DisplayRole, plugin->getName());

    QString state = PluginViewerController::tr("On");

    setData(2, Qt::DisplayRole, state);
    QString desc = QString(plugin->getDescription()).replace("\n", " ");
    setData(3, Qt::DisplayRole, desc);

    setIcon(showServices ? 0 : 1, QIcon(":ugene/images/plugins.png"));

    GUIUtils::setMutedLnF(this, false);
    if (!plugin->getDescription().contains("\n") && desc.length() > 80) {
        for (int i = 80; i < desc.length();) {
            i = desc.lastIndexOf(" ", i);
            desc.replace(i, 1, "\n");
            i += 80;
        }
        setToolTip(1, desc);
        setToolTip(2, desc);
        setToolTip(3, desc);
    } else {
        setToolTip(1, plugin->getDescription());
        setToolTip(2, plugin->getDescription());
        setToolTip(3, plugin->getDescription());
    }
}

PlugViewServiceItem::PlugViewServiceItem(PlugViewPluginItem* parent, Service* s)
    : PlugViewTreeItem(parent), service(s) {
    updateVisual();
}

void PlugViewServiceItem::updateVisual() {
    setData(0, Qt::DisplayRole, PluginViewerController::tr("Service"));
    setData(1, Qt::DisplayRole, service->getName());
    setData(2, Qt::DisplayRole, service->isEnabled() ? PluginViewerController::tr("On") : PluginViewerController::tr("Off"));
    setData(3, Qt::DisplayRole, service->getDescription());
    setIcon(0, QIcon(":ugene/images/service.png"));
}

}  // namespace U2
