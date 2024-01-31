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

#include <U2Algorithm/MsaDistanceAlgorithmRegistry.h>

#include <U2View/BaseWidthController.h>
#include <U2View/DrawHelper.h>

#include "MaEditorNameList.h"
#include "MaEditorSelection.h"
#include "MaEditorSequenceArea.h"
#include "MsaEditor.h"
#include "MsaEditorOverviewArea.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "MsaEditorWgt.h"
#include "MsaMultilineScrollArea.h"
#include "MultilineScrollController.h"
#include "ScrollController.h"
#include "phy_tree/MsaEditorMultiTreeViewer.h"
#include "phy_tree/MsaEditorTreeTabArea.h"
#include "phy_tree/MsaEditorTreeViewer.h"

namespace U2 {

void MsaSizeUtil::updateMinHeightIfPossible(MaEditorSequenceArea* heightFrom, QWidget* setTo) {
    int recommendedMinimumHeight = heightFrom->minimumSizeHint().height();
    if (recommendedMinimumHeight >= 0) {
        setTo->setMinimumHeight(recommendedMinimumHeight);
    }
}

MsaEditorMultilineWgt::MsaEditorMultilineWgt(MsaEditor* _editor, QWidget* parent, bool multiline)
    : MaEditorMultilineWgt(parent), editor(_editor), scrollController(new MultilineScrollController(_editor, this)) {
    setFocusPolicy(Qt::ClickFocus);
    initWidgets();

    this->setObjectName("msa_editor_vertical_childs_layout_" + editor->getMaObject()->getGObjectName());

    createChildren();
    this->setMultilineMode(multiline);

    connect(editor->getMaObject(), &MsaObject::si_alignmentChanged, this, [this]() {
        this->updateSize();
    });
    connect(editor->getCollapseModel(), &MaCollapseModel::si_toggled, this, [this]() {
        this->updateSize();
    });

    connect(editor, &MaEditor::si_cursorPositionChanged, this, &MsaEditorMultilineWgt::sl_cursorPositionChanged);
}

MaEditorWgt* MsaEditorMultilineWgt::createChild(MaEditor* _editor,
                                                MaEditorOverviewArea* _overviewArea,
                                                MaEditorStatusBar* _statusBar) {
    auto msaEditor = qobject_cast<MsaEditor*>(_editor);
    SAFE_POINT(msaEditor != nullptr, "Not MSAEditor!", nullptr);
    return new MsaEditorWgt(msaEditor, this, _overviewArea, _statusBar);
}

void MsaEditorMultilineWgt::addChild(MaEditorWgt* child) {
    if (uiChildLength == 0) {
        uiChildLength = 8;
        uiChild.resize(uiChildLength);
        uiChildCount = 0;
    }

    int index = uiChildCount;

    if (index >= uiChildLength) {
        uiChildLength = index * 2;
        uiChild.resize(uiChildLength);
    }

    uiChild[index] = child;
    uiChildCount++;

    auto vbox = (QVBoxLayout*)uiChildrenArea->layout();
    vbox->addWidget(child);

    child->setObjectName(QString("msa_editor_" + editor->getMaObject()->getGObjectName() + "_%1").arg(index));
    child->getScrollController()->setHScrollBarVisible(!isWrapMode());

    connect(child->getScrollController(), &ScrollController::si_visibleAreaChanged, scrollController, &MultilineScrollController::si_visibleAreaChanged);

    scrollController->sl_updateScrollBars();

    setActiveChild(child);
}

void MsaEditorMultilineWgt::createChildren() {
    int childrenCount = isWrapMode() ? 3 : 1;

    for (int i = 0; i < childrenCount; i++) {
        MaEditorWgt* child = createChild(editor, overviewArea, statusBar);
        SAFE_POINT(child != nullptr, "Can't create sequence widget", );
        addChild(child);

        // recalculate count
        if (i == 0 && isWrapMode()) {
            QSize s = child->minimumSizeHint();
            childrenCount = height() / s.height() + 3;
            int l = editor->getAlignmentLen();
            int aw = getSequenceAreaAllBaseWidth();
            int al = getSequenceAreaAllBaseLen();

            // TODO:ichebyki: 0.66 is a heuristic value, need to define more smart
            int b = width() * 0.66 / (aw / al);
            if (b * (childrenCount - 1) > l) {
                childrenCount = l / b + (l % b > 0 ? 1 : 0);
            }
        }
    }

    // TODO:ichebyki
    // Need to move to special method
    // wich ass/updates connections
    for (int i = 0; i < this->getLineWidgetCount(); i++) {
        connect(getLineWidget(i)->getNameAndSequenceAreasSplitter(),
                &QSplitter::splitterMoved,
                this,
                &MsaEditorMultilineWgt::sl_setAllNameAndSequenceAreasSplittersSizes);
    }
}

void MsaEditorMultilineWgt::updateChildren() {
    if (treeView) {
        // TODO:ichebyki
        // Need complex save/update for phyl-tree
        // Then, we will able to reuse tree view
        auto treeViewer = qobject_cast<MsaEditorWgt*>(uiChild[0])->getMultiTreeViewer();
        if (treeViewer != nullptr) {
            MsaEditorTreeTab* treeTabWidget = treeViewer->getCurrentTabWidget();
            if (treeTabWidget != nullptr) {
                for (int i = treeTabWidget->count(); i > 0; i--) {
                    treeTabWidget->deleteTree(i - 1);
                }
            }
        }
        treeView = false;
    }

    bool showStatistics = false;
    for (; uiChildCount > 0; uiChildCount--) {
        auto child = qobject_cast<MsaEditorWgt*>(uiChild[uiChildCount - 1]);
        SAFE_POINT(child != nullptr, "Can't delete sequence widget in multiline mode", );

        const MsaEditorAlignmentDependentWidget* statWidget = child->getSimilarityWidget();
        showStatistics = statWidget != nullptr && statWidget->isVisible();

        delete child;
        uiChild[uiChildCount - 1] = nullptr;
    }

    createChildren();
    if (showStatistics) {
        showSimilarity();
    }
    activateWindow();
    getLineWidget(0)->getSequenceArea()->setFocus();
}

MaEditorStatusBar* MsaEditorMultilineWgt::getStatusBar() {
    return statusBar;
}

void MsaEditorMultilineWgt::initScrollArea() {
    SAFE_POINT(scrollArea == nullptr, "Scroll area is already initialized", );
    scrollArea = new MsaMultilineScrollArea(editor, this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
}

void MsaEditorMultilineWgt::initOverviewArea() {
    SAFE_POINT(statusBar == nullptr, "Duplicate initialization of overviewArea", );
    overviewArea = new MsaEditorOverviewArea(this);
}

void MsaEditorMultilineWgt::initStatusBar() {
    SAFE_POINT(statusBar == nullptr, "Duplicate initialization of statusBar", );
    statusBar = new MsaEditorStatusBar(editor);
}

void MsaEditorMultilineWgt::initChildrenArea() {
    SAFE_POINT(uiChildrenArea == nullptr, "Duplicate initialization of uiChildrenArea", );
    uiChildrenArea = new QGroupBox();
    uiChildrenArea->setFlat(true);
    uiChildrenArea->setStyleSheet("border:0;");
    uiChildrenArea->setObjectName("msa_editor_multiline_children_area");
}

MaEditorWgt* MsaEditorMultilineWgt::getLineWidget(int index) const {
    return index >= uiChildCount
               ? nullptr
               : qobject_cast<MsaEditorWgt*>(uiChild[index]);
}

void MsaEditorMultilineWgt::updateSize() {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        MaEditorWgt* w = getLineWidget(i);
        SAFE_POINT(w != nullptr, "UI widget is null! Index: " + QString::number(i), );
        MaEditorSequenceArea* area = w->getSequenceArea();
        MsaSizeUtil::updateMinHeightIfPossible(area, w->getEditorNameList());
        MsaSizeUtil::updateMinHeightIfPossible(area, area);
        w->setMinimumSize(w->minimumSizeHint());
    }
    updateGeometry();
}

MaEditor* MsaEditorMultilineWgt::getEditor() const {
    return editor;
}

void MsaEditorMultilineWgt::addPhylTreeWidget(MsaEditorMultiTreeViewer* newMultiTreeViewer) {
    multiTreeViewer = newMultiTreeViewer;
    treeSplitter->insertWidget(0, newMultiTreeViewer);
    treeSplitter->setSizes(QList<int>({isOsMac() ? 600 : 550, 550}));
    treeSplitter->setStretchFactor(0, 1);
    treeSplitter->setStretchFactor(1, 3);

    treeView = true;
}

void MsaEditorMultilineWgt::delPhylTreeWidget() {
    delete multiTreeViewer;
    multiTreeViewer = nullptr;
}

MsaEditorTreeViewer* MsaEditorMultilineWgt::getCurrentTree() const {
    CHECK(multiTreeViewer != nullptr, nullptr);
    auto page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    CHECK(page != nullptr, nullptr);
    return qobject_cast<MsaEditorTreeViewer*>(page->getObjectView());
}

void MsaEditorMultilineWgt::sl_changeColorSchemeOutside(const QString& id) {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        MaEditorSequenceArea* sequence = getLineWidget(i)->getSequenceArea();
        sequence->sl_changeColorSchemeOutside(id);
    }
}

void MsaEditorMultilineWgt::sl_changeColorScheme(const QString& id) {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        MaEditorSequenceArea* sequence = getLineWidget(i)->getSequenceArea();
        sequence->applyColorScheme(id);
    }
}

void MsaEditorMultilineWgt::sl_triggerUseDots(int checkState) {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        MaEditorSequenceArea* sequence = getLineWidget(i)->getSequenceArea();
        sequence->sl_triggerUseDots(checkState);
    }
}

void MsaEditorMultilineWgt::sl_cursorPositionChanged(const QPoint& point) {
    if (multilineMode) {
        scrollController->scrollToPoint(point);
    }
}

void MsaEditorMultilineWgt::setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->setSimilaritySettings(settings);
        }
    }
}

void MsaEditorMultilineWgt::refreshSimilarityColumn() {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->refreshSimilarityColumn();
        }
    }
}

void MsaEditorMultilineWgt::showSimilarity() {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->showSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::hideSimilarity() {
    for (int i = 0; i < getLineWidgetCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->hideSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::sl_onPosChangeRequest(int position) {
    int baseIndex = position - 1;
    CHECK(baseIndex >= 0 && baseIndex < editor->getAlignmentLen(), );
    if (isWrapMode()) {
        getScrollController()->scrollToBase(QPoint(baseIndex, 0));
    } else {
        getLineWidget(0)->getScrollController()->scrollToBase(baseIndex, getSequenceAreaWidth(0));
    }
    // Keep the vertical part of the selection but limit the horizontal to the given position.
    // In case of 1-row selection it will procude a single cell selection as the result.
    // If there is no active selection - select a cell of the first visible row on the screen.
    int selectedBaseIndex = position - 1;
    QList<QRect> selectedRects = editor->getSelection().getRectList();
    if (selectedRects.isEmpty()) {
        int firstVisibleViewRowIndex = getScrollController()->getFirstVisibleViewRowIndex();
        selectedRects.append({selectedBaseIndex, firstVisibleViewRowIndex, 1, 1});
    } else {
        for (QRect& rect : selectedRects) {
            rect.setX(selectedBaseIndex);
            rect.setWidth(1);
        }
    }
    editor->getSelectionController()->setSelection(selectedRects);
}

void MsaEditorMultilineWgt::sl_setAllNameAndSequenceAreasSplittersSizes(int pos, int index) {
    // If you want to use the pos & index
    // you need to subclass QSplitter and call
    // protected method moveSplitter(int pos, int index)
    Q_UNUSED(pos);
    Q_UNUSED(index);
    auto signalSender = qobject_cast<QSplitter*>(sender());
    if (signalSender != nullptr) {
        auto sizes = signalSender->sizes();
        for (int i = 0; i < getLineWidgetCount(); i++) {
            MaEditorWgt* child = getLineWidget(i);
            child->getNameAndSequenceAreasSplitter()->setSizes(sizes);
        }
    }
}

void MsaEditorMultilineWgt::sl_goto() {
    // TODO: use QScopedPointer with dialogs.
    QDialog gotoDialog(this);
    gotoDialog.setModal(true);
    gotoDialog.setWindowTitle(tr("Go to Position"));
    auto ps = new PositionSelector(&gotoDialog, 1, editor->getMaObject()->getLength(), true);
    connect(ps, &PositionSelector::si_positionChanged, this, &MsaEditorMultilineWgt::sl_onPosChangeRequest);
    gotoDialog.exec();
}

bool MsaEditorMultilineWgt::moveSelection(int key, bool shift, bool ctrl) {
    // Ignore shift
    // See src/corelibs/U2View/src/ov_msa/MaEditorSequenceArea.cpp
    if (shift) {
        return false;
    }
    int length = getLastVisibleBase(0) + 1 - getFirstVisibleBase(0);
    QPoint cursorPosition = editor->getCursorPosition();
    const MaEditorSelection& selection = editor->getSelection();
    // Use cursor position for empty selection when arrow keys are used.
    QRect selectionRect = selection.isEmpty()
                              ? QRect(cursorPosition, cursorPosition)
                              : selection.toRect();
    bool isSingleSelection = selectionRect.isEmpty() ||
                             (selectionRect.width() == 1 && selectionRect.height() == 1);

    CHECK(isSingleSelection, false);

    if (key == Qt::Key_Up) {
        QPoint newPos;
        if (cursorPosition.y() == 0) {
            newPos = QPoint(cursorPosition.x() - length,
                            editor->getCollapseModel()->getViewRowCount() - 1);
            CHECK(newPos.x() >= 0, true);
        } else {
            newPos = QPoint(cursorPosition.x(), cursorPosition.y() - 1);
        }
        if (ctrl) {
            newPos.setY(0);
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    } else if (key == Qt::Key_Down) {
        QPoint newPos;
        if (cursorPosition.y() >= (editor->getCollapseModel()->getViewRowCount() - 1)) {
            newPos = QPoint(cursorPosition.x() + length, 0);
            if (newPos.x() >= editor->getAlignmentLen()) {
                newPos.setX(editor->getAlignmentLen() - 1);
            }
        } else {
            newPos = QPoint(cursorPosition.x(), cursorPosition.y() + 1);
        }
        if (ctrl) {
            newPos.setY(editor->getCollapseModel()->getViewRowCount() - 1);
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    } else if (key == Qt::Key_Right) {
        QPoint newPos(cursorPosition.x() + 1, cursorPosition.y());
        CHECK(newPos.x() < editor->getAlignmentLen(), true);
        if (ctrl) {
            newPos.setX(newPos.x() / length * length + length - 1);
            if (newPos.x() >= editor->getAlignmentLen()) {
                newPos.setX(editor->getAlignmentLen() - 1);
            }
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    } else if (key == Qt::Key_Left) {
        QPoint newPos(cursorPosition.x() - 1, cursorPosition.y());
        CHECK(newPos.x() >= 0, true);
        if (ctrl) {
            newPos.setX(newPos.x() / length * length);
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    } else if (key == Qt::Key_Home) {
        QPoint newPos(cursorPosition.x() / length * length, cursorPosition.y());
        if (ctrl) {
            newPos.setX(0);
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    } else if (key == Qt::Key_End) {
        QPoint newPos(cursorPosition.x() / length * length + length - 1, cursorPosition.y());
        if (ctrl) {
            newPos.setX(editor->getAlignmentLen() - 1);
        }
        if (newPos.x() >= editor->getAlignmentLen()) {
            newPos.setX(editor->getAlignmentLen() - 1);
        }
        editor->setCursorPosition(newPos);
        editor->getSelectionController()->setSelection(MaEditorSelection({QRect(newPos, newPos)}));
        getScrollController()->scrollToPoint(newPos);
        return true;
    }

    return false;
}

void MsaEditorMultilineWgt::initWidgets() {
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

    // The following must be called after initializing children area.
    scrollController->init(cvBar, scrollArea);

    setLayout(mainLayout);

    connect(editor, &MaEditor::si_zoomOperationPerformed, scrollController, &MultilineScrollController::sl_zoomScrollBars);
}

bool MsaEditorMultilineWgt::setMultilineMode(bool newmode) {
    bool oldmode = multilineMode;
    multilineMode = newmode;
    if (oldmode != newmode && getLineWidget(0) != nullptr) {
        if (multilineMode) {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        } else {
            uiChildrenArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        }
        updateChildren();
        int firstBase = getLineWidget(0)->getScrollController()->getFirstVisibleBase();
        if (multilineMode) {
            scrollController->setEnabled(true);
            scrollController->setFirstVisibleBase(firstBase);
        } else {
            scrollController->setEnabled(false);
        }

        emit si_maEditorUIChanged();

        return true;
    }
    return false;
}

MaEditorStatusBar* MsaEditorMultilineWgt::getStatusBar() const {
    return statusBar;
}

MaEditorWgt* MsaEditorMultilineWgt::getActiveChild() const {
    return activeChild.wgt;
}

void MsaEditorMultilineWgt::setActiveChild(MaEditorWgt* child) {
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

int MsaEditorMultilineWgt::getSequenceAreaWidth(int index) const {
    MaEditorWgt* ui = getLineWidget(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->width();
}

int MsaEditorMultilineWgt::getFirstVisibleBase(int index) const {
    MaEditorWgt* ui = getLineWidget(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->getFirstVisibleBase();
}

int MsaEditorMultilineWgt::getLastVisibleBase(int index) const {
    MaEditorWgt* ui = getLineWidget(index);
    return ui == nullptr ? 0 : ui->getSequenceArea()->getLastVisibleBase(false);
}

int MsaEditorMultilineWgt::getSequenceAreaBaseLen(int index) const {
    CHECK(index >= 0 && index < getLineWidgetCount(), 0);
    return getLastVisibleBase(index) - getFirstVisibleBase(index) + 1;
}

int MsaEditorMultilineWgt::getSequenceAreaBaseWidth(int index) const {
    CHECK(index >= 0 && index < getLineWidgetCount(), 0);
    MaEditorWgt* ui = getLineWidget(index);
    SAFE_POINT(ui != nullptr, "UI is nullptr for a valid index", 0);
    U2Region visibleBasesRegion = ui->getDrawHelper()->getVisibleBases(ui->getSequenceArea()->width());
    U2Region visibleScreenRegion = ui->getBaseWidthController()->getBasesScreenRange(visibleBasesRegion);
    return (int)visibleScreenRegion.length;
}

int MsaEditorMultilineWgt::getSequenceAreaAllBaseLen() const {
    int length = 0;
    for (int i = 0; i < getLineWidgetCount(); i++) {
        length += getSequenceAreaBaseLen(i);
    }
    return length;
}

int MsaEditorMultilineWgt::getSequenceAreaAllBaseWidth() const {
    int width = 0;
    for (int i = 0; i < getLineWidgetCount(); i++) {
        width += getSequenceAreaBaseWidth(i);
    }
    return width;
}

void MsaEditorMultilineWgt::sl_toggleSequenceRowOrder(bool isOrderBySequence) {
    for (int i = 0; i < uiChildCount; i++) {
        MaEditorWgt* ui = getLineWidget(i);
        SAFE_POINT(ui != nullptr, "UI widget is nullptr! Index: " + QString::number(i), );
        ui->getSequenceArea()->sl_toggleSequenceRowOrder(isOrderBySequence);
    }
}

MaEditorOverviewArea* MsaEditorMultilineWgt::getOverviewArea() const {
    return overviewArea;
}

MultilineScrollController* MsaEditorMultilineWgt::getScrollController() const {
    return scrollController;
}

QScrollArea* MsaEditorMultilineWgt::getChildrenScrollArea() const {
    return scrollArea;
}

int MsaEditorMultilineWgt::getUIIndex(MaEditorWgt* _ui) const {
    CHECK(_ui != nullptr, 0);
    for (int index = 0; index < uiChildCount && index < uiChildLength; index++) {
        if (_ui == uiChild[index]) {
            return index;
        }
    }
    return 0;
}

MsaEditorMultiTreeViewer* MsaEditorMultilineWgt::getPhylTreeWidget() const {
    return multiTreeViewer;
};

int MsaEditorMultilineWgt::getLineWidgetCount() const {
    return uiChildCount;
}

// Current multiline mode
bool MsaEditorMultilineWgt::isWrapMode() const {
    return multilineMode;
}

}  // namespace U2
