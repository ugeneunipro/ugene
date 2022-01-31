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
#include "ov_msa/DrawHelper.h"
#include "ov_msa/ScrollController.h"
#include "ov_msa/MultilineScrollController.h"

namespace U2 {

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
MaEditorMultilineWgt::MaEditorMultilineWgt(MaEditor *_editor)
    : editor(_editor),
      overviewArea(nullptr),
      statusBar(nullptr),
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new MultilineScrollController(editor, this)) {
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

    QVBoxLayout *layoutChildren = new QVBoxLayout;
    uiChildrenArea->setLayout(layoutChildren);
    uiChildrenArea->layout()->setContentsMargins(0, 0, 0, 0);
    uiChildrenArea->layout()->setSpacing(0);
    uiChildrenArea->layout()->setSizeConstraint(QLayout::SetMaximumSize);
    if (multilineMode) {
        uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    } else {
        uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    }

    QGridLayout *layoutMultilineArea = new QGridLayout;
    layoutMultilineArea->setContentsMargins(0, 0, 0, 0);
    layoutMultilineArea->setSpacing(0);
    layoutMultilineArea->setSizeConstraint(QLayout::SetMinAndMaxSize);
    QWidget *multilineArea = new QWidget;
    multilineArea->setLayout(layoutMultilineArea);
    layoutMultilineArea->addWidget(scrollArea, 0, 0);
    layoutMultilineArea->addWidget(cvBar, 0, 1);
    cvBar->setStyleSheet("border: none");

    scrollArea->setWidget(uiChildrenArea);

    treeSplitter = new QSplitter(Qt::Horizontal, this);
    treeSplitter->setContentsMargins(0, 0, 0, 0);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->setContentsMargins(0, 0, 0, 0);
    mainSplitter->setHandleWidth(0);
    mainSplitter->addWidget(multilineArea);

    QSplitter *shBarSplitter = new QSplitter(Qt::Horizontal, this);
    shBarSplitter->setFixedHeight(20);
    shBarSplitter->setContentsMargins(0, 0, 0, 0);
    shBarSplitter->setHandleWidth(0);
    shBarSplitter->addWidget(new QWidget());
    shBarSplitter->addWidget(shBar);
    shBarSplitter->addWidget(new QWidget());
    shBarSplitter->widget(0)->setMinimumSize(1, 1);
    shBarSplitter->widget(2)->setMinimumSize(1, 1);
    shBarSplitter->setStretchFactor(0, 0);
    shBarSplitter->setStretchFactor(1, 100);
    shBarSplitter->setStretchFactor(2, 0);
    shBarSplitter->setSizes({100, 100, cvBar->width()});
    shBarSplitter->setStyleSheet("background: transparent; background-color: transparent; border: 0;");
    shBarSplitter->widget(0)->setStyleSheet("background: white; background-color: white; border: none");
    shBarSplitter->widget(1)->setStyleSheet("border: none");
    shBarSplitter->handle(1)->setStyleSheet("border: none");
    shBarSplitter->handle(2)->setStyleSheet("border: none");
    shBarSplitter->handle(1)->setEnabled(false);
    shBarSplitter->handle(2)->setEnabled(false);
    shBarSplitter->widget(0)->setMinimumSize(1, 1);
    shBarSplitter->widget(2)->setMinimumSize(1, 1);

    mainSplitter->addWidget(shBarSplitter);
    mainSplitter->handle(1)->setStyleSheet("border: none");
    mainSplitter->handle(1)->setEnabled(false);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    treeSplitter->addWidget(mainSplitter);
    treeSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    mainLayout->addWidget(treeSplitter);
    mainLayout->addWidget(statusBar);
    mainLayout->addWidget(overviewArea);

    // the following must be after initing children area
    scrollController->init(shBar, cvBar);

    setLayout(mainLayout);

    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));
}

bool MaEditorMultilineWgt::setMultilineMode(bool newmode)
{
    bool oldmode = multilineMode;
    multilineMode = newmode;
    if (oldmode != newmode) {
        int currentScroll = getUI(0)->getScrollController()->getHorizontalScrollBar()->value();
        if (multilineMode) {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        } else {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        }
        updateChildren();
        scrollController->setMultilineVScrollbarValue(currentScroll);
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

int MaEditorMultilineWgt::getFirstVisibleBase(uint index) const {
    if (index >= getChildrenCount()) {
        return 0;
    }
    return getUI(index)->getSequenceArea()->getFirstVisibleBase();
}

int MaEditorMultilineWgt::getLastVisibleBase(uint index) const {
    if (index >= getChildrenCount()) {
        return 0;
    }
    return getUI(index)->getSequenceArea()->getLastVisibleBase(false);
}

int MaEditorMultilineWgt::getSequenceAreaBaseWidth(uint index) const {
    if (index >= getChildrenCount()) {
        return 0;
    }
    return getLastVisibleBase(index) - getFirstVisibleBase(index) + 1;
}

void MaEditorMultilineWgt::sl_toggleSequenceRowOrder(bool isOrderBySequence)
{
    for (uint i = 0; i < uiChildCount; i++) {
        getUI(i)->getSequenceArea()->sl_toggleSequenceRowOrder(isOrderBySequence);
    }
}

}  // namespace U2
