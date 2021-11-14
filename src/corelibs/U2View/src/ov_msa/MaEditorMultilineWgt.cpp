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

#include "MaEditorMultilineWgt.h"

#include <QGridLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/MSAEditorMultilineOverviewArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorStatusBar.h>
#include <U2View/UndoRedoFramework.h>

#include "MaEditorUtils.h"
#include "SequenceAreaRenderer.h"
#include "ov_msa/helpers/MultilineBaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/MultilineScrollController.h"

namespace U2 {

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
MaEditorMultilineWgt::MaEditorMultilineWgt(MaEditor *_editor)
    : editor(_editor),
      overviewArea(nullptr),
      statusBar(nullptr),
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new MultilineScrollController(editor, this)),
      baseWidthController(new MultilineBaseWidthController(this)),
      rowHeightController(nullptr) {
    SAFE_POINT(editor != nullptr, "MaEditor is null!", );
    setFocusPolicy(Qt::ClickFocus);
}

MaEditorStatusBar *MaEditorMultilineWgt::getStatusBar() const {
    return statusBar;
}

void MaEditorMultilineWgt::initWidgets() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 200);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    GScrollBar *shBar = new GScrollBar(Qt::Horizontal);
    shBar->setObjectName("multiline_horizontal_sequence_scroll");
    shBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QScrollBar *nameListHorizontalScrollBar = new QScrollBar(Qt::Horizontal);
    nameListHorizontalScrollBar->setObjectName("multiline_horizontal_names_scroll");

    GScrollBar *cvBar = new GScrollBar(Qt::Vertical);
    cvBar->setObjectName("multiline_vertical_sequence_scroll");

    initScrollArea();
    initOverviewArea();
    initStatusBar();
    initChildrenArea();

    QGridLayout *layoutChildren = new QGridLayout;
    uiChildrenArea->setLayout(layoutChildren);
    uiChildrenArea->layout()->setContentsMargins(0, 0, 0, 0);
    uiChildrenArea->layout()->setSpacing(0);
    uiChildrenArea->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QGridLayout *layoutMultilineArea = new QGridLayout;
    layoutMultilineArea->setContentsMargins(0, 0, 0, 0);
    layoutMultilineArea->setSpacing(0);
    layoutMultilineArea->setSizeConstraint(QLayout::SetMinAndMaxSize);
    QWidget *multilineArea = new QWidget;
    multilineArea->setLayout(layoutMultilineArea);
    layoutMultilineArea->addWidget(scrollArea, 0, 0);
    layoutMultilineArea->addWidget(cvBar, 0, 1);
    layoutMultilineArea->addWidget(shBar, 1, 0);

    scrollArea->setWidget(uiChildrenArea);

    // the following must be after initing children area
    scrollController->init(shBar, cvBar);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(multilineArea);
    mainSplitter->addWidget(overviewArea);
    mainSplitter->addWidget(statusBar);
    mainSplitter->setStretchFactor(0, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mainLayout->addWidget(mainSplitter);

    setLayout(mainLayout);

    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));
}

void MaEditorMultilineWgt::addChild(MaEditorWgt *child, int index)
{
    if (uiChild == nullptr) {
        uiChildLength = 8;
        uiChild = new MaEditorWgt *[uiChildLength];
        uiChildCount = 0;
    }

    if (index == -1) {
        index = uiChildCount;
    }
    if (index > 0 && (uint)index > uiChildLength) {
        MaEditorWgt **tmp = uiChild;
        uiChild = new MaEditorWgt *[index + 8];
        for (uint i = 0; i < uiChildLength; i++) {
            uiChild[i] = tmp[i];
        }
    }

    uiChild[index] = child;
    uiChildCount++;

    QGridLayout *grid = (QGridLayout *)uiChildrenArea->layout();
    grid->addWidget(child, index, 0);
    scrollController->sl_updateScrollBars();
}

MaEditorWgt *MaEditorMultilineWgt::getActiveChild() {
    return activeChild;
}

void MaEditorMultilineWgt::setActiveChild(MaEditorWgt *child) {
    activeChild = child;
}

void MaEditorMultilineWgt::initActions() {
}

MaEditor *MaEditorMultilineWgt::getEditor() const {
    return editor;
}

int MaEditorMultilineWgt::getSequenceAreaWidth(uint index) const
{
    if (index >= getChildrenCount()) {
        return 0;
    }
    return getUI(index)->getSequenceArea()->width();
}

void MaEditorMultilineWgt::sl_toggleSequenceRowOrder(bool isOrderBySequence)
{
    for (uint i = 0; i < uiChildCount; i++) {
        getUI(i)->getSequenceArea()->sl_toggleSequenceRowOrder(isOrderBySequence);
    }
}

}  // namespace U2
