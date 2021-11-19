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

namespace U2 {

MsaEditorMultilineWgt::MsaEditorMultilineWgt(MSAEditor *editor)
    : MaEditorMultilineWgt(editor) {
    initActions();
    initWidgets();

    this->setObjectName("msa_editor_vertical_childs_layout_" +
                        editor->getMaObject()->getGObjectName());
    this->setMultilineMode(true);

    // TODO:ichebyki
    // calculate needed count
    uint childrenCount = this->getMultilineMode() ? 3 : 1;

    MaEditorOverviewArea *overviewArea = this->getOverviewArea();
    MaEditorStatusBar *statusBar = this->getStatusBar();

    for (uint i = 0; i < childrenCount; i++) {
        MsaEditorWgt *child = new MsaEditorWgt(editor, overviewArea, statusBar);
        SAFE_POINT(child != nullptr, "Can't create sequence widget in multiline mode", );
        QString objName = QString("msa_editor_" + editor->getMaObject()->getGObjectName() + "%1").arg(i);
        child->setObjectName(objName);

        this->addChild(child);
        if (i == 0) {
            this->setActiveChild(child);
        }
    }
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
        scrollArea = new QScrollArea(this);
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
        uiChildrenArea = new QGroupBox(tr("MSA multiline area"));
        uiChildrenArea->setObjectName("msa_editor_multiline_children_area");
    } else {
        uiChildrenArea = _uiChildrenArea;
    }
}

MaEditorWgt *MsaEditorMultilineWgt::getUI(uint index) const {
    return uiChild == nullptr || index >= uiChildCount
               ? nullptr
               : qobject_cast<MsaEditorWgt *>(uiChild[index]);
}

}  // namespace U2
