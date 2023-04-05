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
#include "ov_msa/BaseWidthController.h"
#include "ov_msa/DrawHelper.h"
#include "ov_msa/MultilineScrollController.h"
#include "ov_msa/ScrollController.h"
namespace U2 {

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
MaEditorMultilineWgt::MaEditorMultilineWgt(MaEditor* _editor, QWidget* parent)
    : QWidget(parent), editor(_editor),
      scrollController(new MultilineScrollController(editor, this)) {
    SAFE_POINT(editor != nullptr, "MaEditor is null!", );
    setFocusPolicy(Qt::ClickFocus);
}

MaEditorStatusBar* MaEditorMultilineWgt::getStatusBar() const {
    return statusBar;
}

void MaEditorMultilineWgt::initWidgets() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 200);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    auto cvBar = new GScrollBar(Qt::Vertical);
    cvBar->setObjectName("multiline_vertical_sequence_scroll");

    initScrollArea();
    initOverviewArea();
    initStatusBar();
    initChildrenArea();

    auto layoutChildren = new QVBoxLayout;
    uiChildrenArea->setLayout(layoutChildren);
    uiChildrenArea->layout()->setContentsMargins(0, 0, 0, 0);
    uiChildrenArea->layout()->setSpacing(0);
    uiChildrenArea->layout()->setSizeConstraint(QLayout::SetMaximumSize);
    if (multilineMode) {
        uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    } else {
        uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    }

    auto layoutMultilineArea = new QGridLayout;
    layoutMultilineArea->setContentsMargins(0, 0, 0, 0);
    layoutMultilineArea->setSpacing(0);
    layoutMultilineArea->setSizeConstraint(QLayout::SetMinAndMaxSize);
    auto multilineArea = new QWidget;
    multilineArea->setLayout(layoutMultilineArea);
    layoutMultilineArea->addWidget(scrollArea, 0, 0);
    layoutMultilineArea->addWidget(cvBar, 0, 1);
    cvBar->setStyleSheet("border: none");

    scrollArea->setWidget(uiChildrenArea);

    treeSplitter = new QSplitter(Qt::Horizontal, this);
    treeSplitter->setObjectName("maeditor_multilinewgt_phyltree_splitter");
    treeSplitter->setContentsMargins(0, 0, 0, 0);

    auto mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->setObjectName("maeditor_multilinewgt_main_splitter");
    mainSplitter->setContentsMargins(0, 0, 0, 0);
    mainSplitter->setHandleWidth(0);
    mainSplitter->addWidget(multilineArea);

    auto mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    treeSplitter->addWidget(mainSplitter);
    treeSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    mainLayout->addWidget(treeSplitter);
    mainLayout->addWidget(statusBar);
    mainLayout->addWidget(overviewArea);

    // the following must be after initing children area
    scrollController->init(cvBar, scrollArea);

    setLayout(mainLayout);

    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));
}

bool MaEditorMultilineWgt::setMultilineMode(bool newmode) {
    bool oldmode = multilineMode;
    multilineMode = newmode;
    if (oldmode != newmode && getUI(0) != nullptr) {
        if (multilineMode) {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        } else {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        }
        // TODO: 'updateChildren' is written in unsafe way on Mac: after all widgets are deleted the active window is switched to next non-empty window.
        //  Currently we use workaround: manually returning a focus back to the current window.
        auto activeWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
        updateChildren();
        if (isOsMac()) {
            AppContext::getMainWindow()->getMDIManager()->activateWindow(activeWindow);
        }
        int firstBase = getUI(0)->getScrollController()->getFirstVisibleBase();
        if (multilineMode) {
            scrollController->setEnable(true);
            scrollController->setFirstVisibleBase(firstBase);
        } else {
            scrollController->setEnable(false);
        }

        emit si_maEditorUIChanged();

        return true;
    }
    return false;
}

MaEditorWgt* MaEditorMultilineWgt::getActiveChild() {
    return activeChild.wgt;
}

void MaEditorMultilineWgt::setActiveChild(MaEditorWgt* child) {
    disconnect(activeChild.startChangingHandle);
    disconnect(activeChild.stopChangingHandle);
    activeChild.wgt = child;
    activeChild.startChangingHandle = connect(child,
                                              &MaEditorWgt::si_startMaChanging,
                                              this,
                                              &MaEditorMultilineWgt::si_startMaChanging,
                                              Qt::UniqueConnection);
    activeChild.stopChangingHandle = connect(child,
                                             &MaEditorWgt::si_stopMaChanging,
                                             this,
                                             &MaEditorMultilineWgt::si_stopMaChanging,
                                             Qt::UniqueConnection);
}

void MaEditorMultilineWgt::initActions() {
}

MaEditor* MaEditorMultilineWgt::getEditor() const {
    return editor;
}

int MaEditorMultilineWgt::getSequenceAreaWidth(int index) const {
    MaEditorWgt* ui = getUI(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->width();
}

int MaEditorMultilineWgt::getFirstVisibleBase(int index) const {
    MaEditorWgt* ui = getUI(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->getFirstVisibleBase();
}

int MaEditorMultilineWgt::getLastVisibleBase(int index) const {
    MaEditorWgt* ui = getUI(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->getLastVisibleBase(false);
}

int MaEditorMultilineWgt::getSequenceAreaBaseLen(int index) const {
    CHECK(index >= 0 && index < getChildrenCount(), 0);
    return getLastVisibleBase(index) - getFirstVisibleBase(index) + 1;
}

int MaEditorMultilineWgt::getSequenceAreaBaseWidth(int index) const {
    CHECK(index >= 0 && index < getChildrenCount(), 0);
    MaEditorWgt* ui = getUI(index);
    SAFE_POINT(ui != nullptr, "UI is nullptr for a valid index", 0);
    U2Region visibleBasesRegion = ui->getDrawHelper()->getVisibleBases(ui->getSequenceArea()->width());
    U2Region visibleScreenRegion = ui->getBaseWidthController()->getBasesScreenRange(visibleBasesRegion);
    return (int)visibleScreenRegion.length;
}

int MaEditorMultilineWgt::getSequenceAreaAllBaseLen() const {
    int length = 0;
    for (int i = 0; i < getChildrenCount(); i++) {
        length += getSequenceAreaBaseLen(i);
    }
    return length;
}

int MaEditorMultilineWgt::getSequenceAreaAllBaseWidth() const {
    int width = 0;
    for (int i = 0; i < getChildrenCount(); i++) {
        width += getSequenceAreaBaseWidth(i);
    }
    return width;
}

void MaEditorMultilineWgt::sl_toggleSequenceRowOrder(bool isOrderBySequence) {
    for (int i = 0; i < uiChildCount; i++) {
        MaEditorWgt* ui = getUI(i);
        SAFE_POINT(ui != nullptr, "UI widget is nullptr! Index: " + QString::number(i), );
        ui->getSequenceArea()->sl_toggleSequenceRowOrder(isOrderBySequence);
    }
}

MaEditorOverviewArea* MaEditorMultilineWgt::getOverviewArea() const {
    return overviewArea;
}

MultilineScrollController* MaEditorMultilineWgt::getScrollController() const {
    return scrollController;
}

QScrollArea* MaEditorMultilineWgt::getChildrenScrollArea() const {
    return scrollArea;
}

MaEditorWgt* MaEditorMultilineWgt::getUI(int index) const {
    return index < uiChildCount && index < uiChildLength
               ? uiChild[index]
               : nullptr;
}

void MaEditorMultilineWgt::updateSize() {
    // Do nothing by default.
}

int MaEditorMultilineWgt::getUIIndex(MaEditorWgt* _ui) const {
    CHECK(_ui != nullptr, 0);
    for (int index = 0; index < uiChildCount && index < uiChildLength; index++) {
        if (_ui == uiChild[index]) {
            return index;
        }
    }
    return 0;
}

}  // namespace U2
