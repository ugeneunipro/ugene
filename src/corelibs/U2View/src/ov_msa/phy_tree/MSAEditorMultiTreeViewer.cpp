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
#include "MSAEditorMultiTreeViewer.h"

#include <QVBoxLayout>

#include <U2Core/DocumentModel.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/MSAEditor.h>

#include "MsaEditorTreeTabArea.h"
#include "ov_msa/MaEditorUtils.h"

namespace U2 {

MSAEditorMultiTreeViewer::MSAEditorMultiTreeViewer(const QString& title, MSAEditor* msaEditor)
    : editor(msaEditor) {
    treeTabArea = new MsaEditorTreeTabArea(editor, this);
    titleWidget = msaEditor->getMaEditorWgt()->createHeaderLabelWidget(title);

    auto maUtilsWidget = dynamic_cast<MaUtilsWidget*>(titleWidget);
    maUtilsWidget->setHeightMargin(-55);

    auto treeAreaLayout = new QVBoxLayout(this);
    treeAreaLayout->setMargin(0);
    treeAreaLayout->setSpacing(0);
    treeAreaLayout->addWidget(titleWidget);
    treeAreaLayout->addWidget(treeTabArea);

    this->setLayout(treeAreaLayout);

    connect(treeTabArea, SIGNAL(si_tabsCountChanged(int)), SIGNAL(si_tabsCountChanged(int)));

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void MSAEditorMultiTreeViewer::addTreeView(GObjectViewWindow* treeView) {
    treeTabArea->addTab(treeView, treeView->getViewName(), true);
    treeViewList.append(treeView);

    const QList<GObject*>& objects = treeView->getObjects();
    for (GObject* obj : qAsConst(objects)) {
        if (obj->getGObjectType() == GObjectTypes::PHYLOGENETIC_TREE) {
            tabsNameList.append(obj->getDocument()->getName());
        }
    }
}

QWidget* MSAEditorMultiTreeViewer::getCurrentWidget() const {
    return treeTabArea->getCurrentWidget();
}

MsaEditorTreeTab* MSAEditorMultiTreeViewer::getCurrentTabWidget() const {
    return treeTabArea->getCurrentTabWidget();
}

void MSAEditorMultiTreeViewer::sl_onTabCloseRequested(QWidget* page) {
    treeViewList.removeOne(page);
    auto viewWindow = qobject_cast<GObjectViewWindow*>(page);
    CHECK(viewWindow != nullptr, );

    int tabIndex = tabsNameList.indexOf(viewWindow->getViewName());
    tabsNameList.removeAt(tabIndex);
    delete viewWindow;
    emit si_tabsCountChanged(tabsNameList.count());
}

const QStringList& MSAEditorMultiTreeViewer::getTreeNames() const {
    return tabsNameList;
}

}  // namespace U2
