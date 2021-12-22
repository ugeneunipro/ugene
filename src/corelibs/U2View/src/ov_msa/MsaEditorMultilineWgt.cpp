/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MsaEditorMultilineWgt.h"

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include "MSAEditor.h"
#include "MSAEditorOverviewArea.h"
#include "MsaEditorStatusBar.h"
#include "MsaEditorWgt.h"
#include "MsaMultilineScrollArea.h"
#include "helpers/MultilineScrollController.h"
#include "helpers/ScrollController.h"
#include "phy_tree/MSAEditorMultiTreeViewer.h"
#include "phy_tree/MsaEditorTreeTabArea.h"

namespace U2 {

MsaEditorMultilineWgt::MsaEditorMultilineWgt(MSAEditor *editor, bool multiline)
    : MaEditorMultilineWgt(editor) {
    initActions();
    initWidgets();

    this->setObjectName("msa_editor_vertical_childs_layout_" +
                        editor->getMaObject()->getGObjectName());
    this->setMultilineMode(multiline);

    createChildren();
}

void MsaEditorMultilineWgt::createChildren()
{
    // TODO:ichebyki
    // calculate needed count
    uint childrenCount = getMultilineMode() ? 3 : 1;

    MaEditorOverviewArea *overviewArea = this->getOverviewArea();
    MaEditorStatusBar *statusBar = this->getStatusBar();
    for (uint i = 0; i < childrenCount; i++) {
        MsaEditorWgt *child = new MsaEditorWgt(qobject_cast<MSAEditor *>(editor),
                                               overviewArea,
                                               statusBar);
        SAFE_POINT(child != nullptr, "Can't create sequence widget", );
        QString objName = QString("msa_editor_" + editor->getMaObject()->getGObjectName() + "%1")
                              .arg(i);
        child->setObjectName(objName);
        child->getScrollController()->setHScrollBarVisible(!getMultilineMode());

        this->addChild(child);
        if (i == 0) {
            this->setActiveChild(child);
        }
    }
    emit scrollController->si_hScrollValueChanged();
}

void MsaEditorMultilineWgt::updateChildren()
{
    if (treeView) {
        // TODO:ichebyki
        // Need complex save/update for phyl-tree
        // If so, we have to reuse tree view
        MSAEditorMultiTreeViewer *treeViewer = qobject_cast<MsaEditorWgt *>(uiChild[0])
                                                   ->getMultiTreeViewer();
        if (treeViewer != nullptr) {
            MsaEditorTreeTab *treeTabWidget = treeViewer->getCurrentTabWidget();
            if (treeTabWidget != nullptr) {
                for (int i = treeTabWidget->count(); i > 0; i--) {
                    treeTabWidget->deleteTree(i - 1);
                }
            }
        }
        treeView = false;
    }
    for (; uiChildCount > 0; uiChildCount--) {
        MsaEditorWgt *child = qobject_cast<MsaEditorWgt *>(uiChild[uiChildCount - 1]);
        SAFE_POINT(child != nullptr, "Can't delete sequence widget in multiline mode", );

        delete child;
        uiChild[uiChildCount - 1] = nullptr;
    }

    createChildren();
}

MSAEditor *MsaEditorMultilineWgt::getEditor() const {
    return qobject_cast<MSAEditor *>(editor);
}

MaEditorOverviewArea *MsaEditorMultilineWgt::getOverview() {
    return overviewArea;
}
MaEditorStatusBar *MsaEditorMultilineWgt::getStatusBar() {
    return statusBar;
}

void MsaEditorMultilineWgt::initScrollArea(QScrollArea *_scrollArea)
{
    if (_scrollArea == nullptr) {
        scrollArea = new MsaMultilineScrollArea(editor, this);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        scrollArea = _scrollArea;
    }
    scrollArea->setWidgetResizable(true);
}

void MsaEditorMultilineWgt::initOverviewArea(MaEditorOverviewArea *_overviewArea) {
    if (_overviewArea == nullptr) {
        overviewArea = new MSAEditorOverviewArea(this);
    } else {
        overviewArea = _overviewArea;
    }
}

void MsaEditorMultilineWgt::initStatusBar(MaEditorStatusBar *_statusBar) {
    if (_statusBar == nullptr) {
        statusBar = new MsaEditorStatusBar(getEditor());
    } else {
        statusBar = _statusBar;
    }
}

void MsaEditorMultilineWgt::initChildrenArea(QGroupBox *_uiChildrenArea) {
    if (_uiChildrenArea == nullptr) {
        uiChildrenArea = new QGroupBox();
        uiChildrenArea->setFlat(true);
        uiChildrenArea->setStyleSheet("border:0;");
        uiChildrenArea->setObjectName("msa_editor_multiline_children_area");
    } else {
        uiChildrenArea = _uiChildrenArea;
    }
}

MaEditorWgt *MsaEditorMultilineWgt::getUI(uint index) const {
    return !(index < uiChildCount)
               ? nullptr
               : qobject_cast<MsaEditorWgt *>(uiChild[index]);
}

void MsaEditorMultilineWgt::addPhylTreeWidget(QWidget *multiTreeViewer) {
    treeSplitter->insertWidget(0, multiTreeViewer);
    treeSplitter->setSizes(QList<int>({200, 600}));
    treeSplitter->setStretchFactor(0, 1);
    treeSplitter->setStretchFactor(1, 3);

    treeView = true;
}

}  // namespace U2
