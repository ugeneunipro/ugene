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
#include <U2View/MSAEditorOverviewArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorStatusBar.h>
#include <U2View/UndoRedoFramework.h>

#include "MaEditorUtils.h"
#include "SequenceAreaRenderer.h"
#include "ov_msa/helpers/MultilineBaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/ScrollController.h"
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
    uiChildrenArea->layout()->setSizeConstraint(QLayout::SetMaximumSize);
    uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    QGridLayout *layoutMultilineArea = new QGridLayout;
    layoutMultilineArea->setContentsMargins(0, 0, 0, 0);
    layoutMultilineArea->setSpacing(0);
    layoutMultilineArea->setSizeConstraint(QLayout::SetMinAndMaxSize);
    QWidget *multilineArea = new QWidget;
    multilineArea->setLayout(layoutMultilineArea);
    layoutMultilineArea->addWidget(scrollArea, 0, 0);
    layoutMultilineArea->addWidget(cvBar, 0, 1);
    layoutChildren->addWidget(shBar, 0, 0);

    scrollArea->setWidget(uiChildrenArea);

    // the following must be after initing children area
    scrollController->init(shBar, cvBar);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(multilineArea);
    mainSplitter->addWidget(statusBar);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mainLayout->addWidget(mainSplitter);
    mainLayout->addWidget(statusBar);
    mainLayout->addWidget(overviewArea);

    setLayout(mainLayout);

    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));
}

void MaEditorMultilineWgt::addChild(MaEditorWgt *child, int index) {
    if (uiChildLength == 0) {
        uiChildLength = 8;
        uiChild.resize(uiChildLength);
        uiChildCount = 0;
    }

    if (index == -1) {
        index = uiChildCount;
    }
    if (index > 0 && (uint) index >= uiChildLength) {
        uiChildLength = index * 2;
        uiChild.resize(uiChildLength);
    }

    uiChild[index] = child;
    uiChildCount++;

    QGridLayout *grid = (QGridLayout *)uiChildrenArea->layout();
    grid->addWidget(child, index, 0);
    grid->addWidget(scrollController->getHorizontalScrollBar(), index + 1, 0);

    connect(child->getScrollController(), SIGNAL(si_visibleAreaChanged()), getScrollController(), SLOT(sl_updateScrollBars()));
    scrollController->sl_updateScrollBars();
}

bool MaEditorMultilineWgt::setMultilineMode(bool newmode)
{
    bool oldmode = multilineMode;
    multilineMode = newmode;
    if (oldmode != newmode) {
        updateChildren();
        return true;
    }
    return false;
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

int MaEditorMultilineWgt::getSequenceAreaBaseWidth(uint index) const {
    if (index >= getChildrenCount()) {
        return 0;
    }

    int l = getUI(index)->getSequenceArea()->getLastVisibleBase(false);
    int f = getUI(index)->getSequenceArea()->getFirstVisibleBase();
    return l - f + 1;
}

void MaEditorMultilineWgt::sl_toggleSequenceRowOrder(bool isOrderBySequence)
{
    for (uint i = 0; i < uiChildCount; i++) {
        getUI(i)->getSequenceArea()->sl_toggleSequenceRowOrder(isOrderBySequence);
    }
}

}  // namespace U2
