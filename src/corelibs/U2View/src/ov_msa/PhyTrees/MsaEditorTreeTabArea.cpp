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

#include "MsaEditorTreeTabArea.h"
#include "../MSAEditor.h"
#include "MSAEditorTreeManager.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/PhyTreeObject.h>

#include <QPainter>
#include <QTabBar>
#include <QVBoxLayout>


namespace U2
{
MsaEditorTreeTab::MsaEditorTreeTab(MSAEditor* msa, QWidget* parent)
    : QTabWidget(parent), msa(msa), addTabButton(NULL){
    setObjectName("MsaEditorTreeTab");
    addTabButton = new QPushButton(QIcon(":/core/images/add_tree.png"), "", this);
    addTabButton->setToolTip(tr("Add existing tree"));
    setCornerWidget(addTabButton);
    connect(addTabButton, SIGNAL(clicked(bool)), this, SLOT(sl_addTabTriggered()));
    connect(this, SIGNAL(si_tabsCountChanged(int)), SLOT(sl_onCountChanged(int)));

    setTabsClosable(true);
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(sl_onTabCloseRequested(int)));

    tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(sl_onContextMenuRequested(const QPoint&)));

    closeOtherTabs = new QAction(tr("Close other tabs"), this);
    closeOtherTabs->setObjectName("Close other tabs");
    connect(closeOtherTabs, SIGNAL(triggered()), SLOT(sl_onCloseOtherTabs()));

    closeAllTabs = new QAction(tr("Close all tabs"), this);
    closeAllTabs->setObjectName("Close all tabs");
    connect(closeAllTabs, SIGNAL(triggered()), SLOT(sl_onCloseAllTabs()));

    closeTab = new QAction(tr("Close tab"), this);
    closeTab->setObjectName("Close tab");
    connect(closeTab, SIGNAL(triggered()), SLOT(sl_onCloseTab()));
}
void MsaEditorTreeTab::sl_onTabCloseRequested(int index) {
    deleteTree(index);
}

void MsaEditorTreeTab::sl_onContextMenuRequested(const QPoint &pos) {
    menuPos = pos;
    QMenu tabsMenu;
    tabsMenu.addAction(closeOtherTabs);
    tabsMenu.addAction(closeAllTabs);
    tabsMenu.addAction(closeTab);
    tabsMenu.exec(mapToGlobal(pos));
}

void MsaEditorTreeTab::sl_onCountChanged(int count) {
    if (count > 0) {
        closeOtherTabs->setEnabled(count > 1);
    }
}

void MsaEditorTreeTab::addTab(QWidget *page, const QString &label) {
    QTabWidget::addTab(page, label);
    emit si_tabsCountChanged(count());
}

void MsaEditorTreeTab::deleteTree(int index) {
    SAFE_POINT(-1 != index && index < count(), "Incorrect index is detected.",);
    GObjectViewWindow* win = qobject_cast<GObjectViewWindow*>(widget(index));
    const GObject* obj = win->getObjectView()->getObjects().at(0);
    Document* doc = obj->getDocument();
    GObjectReference treeRef(doc->getURLString(), "", GObjectTypes::PHYLOGENETIC_TREE);
    treeRef.objName = obj->getGObjectName();
    msa->getMaObject()->removeObjectRelation(GObjectRelation(treeRef, ObjectRole_PhylogeneticTree));

    removeTab(index);
    delete win;
    emit si_tabsCountChanged(count());
}

void MsaEditorTreeTab::sl_addTabTriggered() {
    msa->getTreeManager()->openTreeFromFile();
    emit si_tabsCountChanged(count());
}

void MsaEditorTreeTab::addExistingTree() {
    msa->getTreeManager()->openTreeFromFile();
    emit si_tabsCountChanged(count());
}

void MsaEditorTreeTab::sl_onCloseOtherTabs() {
    int selectedTabIndex = tabBar()->tabAt(menuPos);
    for (int i = count() - 1; i >= 0; i--) {
        if (i != selectedTabIndex) {
            deleteTree(i);
        }
    }
}

void MsaEditorTreeTab::sl_onCloseAllTabs() {
    for (int tabsCount = count(); tabsCount > 0; tabsCount--) {
        deleteTree(0);
    }
}

void MsaEditorTreeTab::sl_onCloseTab() {
    int index = tabBar()->tabAt(menuPos);
    deleteTree(index);
}

MsaEditorTreeTabArea::MsaEditorTreeTabArea(MSAEditor* msa, QWidget* parent)
    : QWidget(parent), addTabButton(NULL), msa(msa), treeTabWidget(NULL), currentLayout(NULL) {
    initialize();
}
void MsaEditorTreeTabArea::initialize() {
    treeTabWidget = createTabWidget();
    currentLayout = new QVBoxLayout();
    currentLayout->setMargin(0);
    currentLayout->setSpacing(0);
    currentLayout->addWidget(treeTabWidget);
    setLayout(currentLayout);
}
MsaEditorTreeTab* MsaEditorTreeTabArea::createTabWidget() {
    MsaEditorTreeTab* widget = new MsaEditorTreeTab(msa, this);
    connect(widget, SIGNAL(si_tabsCountChanged(int)), SIGNAL(si_tabsCountChanged(int)));
    return widget;
}

void MsaEditorTreeTabArea::addTab(QWidget *page, const QString &label) {
    treeTabWidget->addTab(page, label);
}
void MsaEditorTreeTabArea::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
}

} //namespace


