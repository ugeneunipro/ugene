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
#include "MSAEditorMultiTreeViewer.h"
#include "MsaEditorTreeTabArea.h"
#include "ov_msa/view_rendering/MaEditorUtils.h"

#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/MSAEditor.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Core/DocumentModel.h>
#include <QVBoxLayout>


namespace U2 {

MSAEditorMultiTreeViewer::MSAEditorMultiTreeViewer(QString _title, MSAEditor* _editor)
: editor(_editor) {
    treeTabs = new MsaEditorTreeTabArea(editor, this);
    titleWidget = _editor->getUI()->createHeaderLabelWidget(_title);
    MaUtilsWidget* title = dynamic_cast<MaUtilsWidget*>(titleWidget);
    title->setHeightMargin(-55);
    QVBoxLayout* treeAreaLayout = new QVBoxLayout(this);
    treeAreaLayout->setMargin(0);
    treeAreaLayout->setSpacing(0);
    treeAreaLayout->addWidget(titleWidget);
    treeAreaLayout->addWidget(treeTabs);

    this->setLayout(treeAreaLayout);

    connect(treeTabs, SIGNAL(si_tabsCountChanged(int)), SIGNAL(si_tabsCountChanged(int)));

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void MSAEditorMultiTreeViewer::addTreeView(GObjectViewWindow* treeView) {
    treeTabs->addTab(treeView, treeView->getViewName());
    treeViews.append(treeView);

    const QList<GObject*>& objects = treeView->getObjects();
    foreach(GObject* obj, objects) {
        if(GObjectTypes::PHYLOGENETIC_TREE == obj->getGObjectType()) {
            QString objName = obj->getDocument()->getName();
            tabsNames.append(objName);
        }
    }
}

QWidget* MSAEditorMultiTreeViewer::getCurrentWidget() const {
    return treeTabs->getCurrentWidget();
}

MsaEditorTreeTab* MSAEditorMultiTreeViewer::getCurrentTabWidget() const {
    return treeTabs->getCurrentTabWidget();
}

void MSAEditorMultiTreeViewer::sl_onTabCloseRequested(QWidget* page) {
    treeViews.removeOne(page);
    GObjectViewWindow* viewWindow = qobject_cast<GObjectViewWindow*>(page);
    if(NULL != viewWindow) {
        int i =  tabsNames.indexOf(viewWindow->getViewName());
        tabsNames.removeAt(i);
        delete viewWindow;
        emit si_tabsCountChanged(tabsNames.count());
    }
}

const QStringList& MSAEditorMultiTreeViewer::getTreeNames() const {
    return tabsNames;
}

}//namespace
