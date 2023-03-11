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
#include "MaEditorNameList.h"
#include "MaEditorSelection.h"
#include "MaEditorSequenceArea.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "MsaEditorWgt.h"
#include "MsaMultilineScrollArea.h"
#include "MultilineScrollController.h"
#include "ScrollController.h"
#include "phy_tree/MSAEditorMultiTreeViewer.h"
#include "phy_tree/MSAEditorTreeViewer.h"
#include "phy_tree/MsaEditorTreeTabArea.h"

namespace U2 {

void MsaSizeUtil::updateMinHeightIfPossible(MaEditorSequenceArea* heightFrom, QWidget* setTo) {
    int recommendedMinimumHeight = heightFrom->minimumSizeHint().height();
    if (recommendedMinimumHeight >= 0) {
        setTo->setMinimumHeight(recommendedMinimumHeight);
    }
}

MsaEditorMultilineWgt::MsaEditorMultilineWgt(MSAEditor* editor, QWidget* parent, bool multiline)
    : MaEditorMultilineWgt(editor, parent),
      multiTreeViewer(nullptr),
      treeViewer(nullptr) {
    initActions();
    initWidgets();

    this->setObjectName("msa_editor_vertical_childs_layout_" +
                        editor->getMaObject()->getGObjectName());

    createChildren();
    this->setMultilineMode(multiline);

    connect(editor->getMaObject(), &MultipleAlignmentObject::si_alignmentChanged, this, [this]() {
        this->updateSize();
    });
    connect(editor->getCollapseModel(), &MaCollapseModel::si_toggled, this, [this]() {
        this->updateSize();
    });

    connect(editor,
            SIGNAL(si_cursorPositionChanged(const QPoint&)),
            SLOT(sl_cursorPositionChanged(const QPoint&)));
}

MaEditorWgt* MsaEditorMultilineWgt::createChild(MaEditor* editor,
                                                MaEditorOverviewArea* overviewArea,
                                                MaEditorStatusBar* statusBar) {
    auto msaEditor = qobject_cast<MSAEditor*>(editor);
    SAFE_POINT(msaEditor != nullptr, "Not MSAEditor!", nullptr);
    return new MsaEditorWgt(msaEditor, this, overviewArea, statusBar);
}

void MsaEditorMultilineWgt::deleteChild(int index) {
    if (index < 0 || index >= uiChildCount) {
        return;
    }

    MaEditorWgt* toDelete = getUI(index);
    auto layout = (QVBoxLayout*)uiChildrenArea->layout();

    uiLog.details(tr("Deleting widget from grid, count %1, index %2").arg(layout->count()).arg(index));
    // TODO:ichebyki
    // Is it needed to hide before delete?
    // toDelete->hide();
    layout->removeWidget(toDelete);

    uiLog.details(
        tr("Deleting widget from uiChild, count %1, index %2").arg(uiChild.size()).arg(index));
    uiChild.remove(index);
    uiChild.resize(uiChildLength);
    uiChildCount--;
    uiLog.details(tr("Deleted widget, uiChildCount %1").arg(uiChildCount));

    delete toDelete;
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
    child->getScrollController()->setHScrollBarVisible(!getMultilineMode());

    connect(child->getScrollController(), &ScrollController::si_visibleAreaChanged, scrollController, &MultilineScrollController::si_visibleAreaChanged);

    scrollController->sl_updateScrollBars();

    setActiveChild(child);
}

void MsaEditorMultilineWgt::createChildren() {
    int childrenCount = getMultilineMode() ? 3 : 1;

    MaEditorOverviewArea* overviewArea = this->getOverviewArea();
    MaEditorStatusBar* statusBar = this->getStatusBar();
    for (int i = 0; i < childrenCount; i++) {
        MaEditorWgt* child = createChild(editor, overviewArea, statusBar);
        SAFE_POINT(child != nullptr, "Can't create sequence widget", );
        addChild(child);

        // recalculate count
        if (i == 0 && getMultilineMode()) {
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
    for (int i = 0; i < this->getChildrenCount(); i++) {
        connect(getUI(i)->getNameAndSequenceAreasSplitter(),
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
}

MSAEditor* MsaEditorMultilineWgt::getEditor() const {
    return qobject_cast<MSAEditor*>(editor);
}

MaEditorOverviewArea* MsaEditorMultilineWgt::getOverview() {
    return overviewArea;
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
    overviewArea = new MSAEditorOverviewArea(this);
}

void MsaEditorMultilineWgt::initStatusBar() {
    SAFE_POINT(statusBar == nullptr, "Duplicate initialization of statusBar", );
    statusBar = new MsaEditorStatusBar(getEditor());
}

void MsaEditorMultilineWgt::initChildrenArea() {
    SAFE_POINT(uiChildrenArea == nullptr, "Duplicate initialization of uiChildrenArea", );
    uiChildrenArea = new QGroupBox();
    uiChildrenArea->setFlat(true);
    uiChildrenArea->setStyleSheet("border:0;");
    uiChildrenArea->setObjectName("msa_editor_multiline_children_area");
}

MaEditorWgt* MsaEditorMultilineWgt::getUI(int index) const {
    return index >= uiChildCount
               ? nullptr
               : qobject_cast<MsaEditorWgt*>(uiChild[index]);
}

void MsaEditorMultilineWgt::updateSize() {
    for (int i = 0; i < getChildrenCount(); i++) {
        MaEditorWgt* w = getUI(i);
        SAFE_POINT(w != nullptr, "UI widget is null! Index: " + QString::number(i), );
        MaEditorSequenceArea* area = w->getSequenceArea();
        MsaSizeUtil::updateMinHeightIfPossible(area, w->getEditorNameList());
        MsaSizeUtil::updateMinHeightIfPossible(area, area);
        w->setMinimumSize(w->minimumSizeHint());
    }
    updateGeometry();
}

void MsaEditorMultilineWgt::addPhylTreeWidget(MSAEditorMultiTreeViewer* newMultiTreeViewer) {
    multiTreeViewer = newMultiTreeViewer;
    treeSplitter->insertWidget(0, newMultiTreeViewer);
    treeSplitter->setSizes(QList<int>({500, 600}));
    treeSplitter->setStretchFactor(0, 1);
    treeSplitter->setStretchFactor(1, 3);

    treeView = true;
}

void MsaEditorMultilineWgt::delPhylTreeWidget() {
    delete multiTreeViewer;
    multiTreeViewer = nullptr;
}

MSAEditorTreeViewer* MsaEditorMultilineWgt::getCurrentTree() const {
    CHECK(multiTreeViewer != nullptr, nullptr);
    auto page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    CHECK(page != nullptr, nullptr);
    return qobject_cast<MSAEditorTreeViewer*>(page->getObjectView());
}

void MsaEditorMultilineWgt::sl_changeColorSchemeOutside(const QString& id) {
    for (int i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->sl_changeColorSchemeOutside(id);
    }
}

void MsaEditorMultilineWgt::sl_changeColorScheme(const QString& id) {
    for (int i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->applyColorScheme(id);
    }
}

void MsaEditorMultilineWgt::sl_triggerUseDots(int checkState) {
    for (int i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->sl_triggerUseDots(checkState);
    }
}

void MsaEditorMultilineWgt::sl_cursorPositionChanged(const QPoint& point) {
    if (multilineMode) {
        scrollController->scrollToPoint(point);
    }
}

void MsaEditorMultilineWgt::setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
    for (int i = 0; i < getChildrenCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->setSimilaritySettings(settings);
        }
    }
}

void MsaEditorMultilineWgt::refreshSimilarityColumn() {
    for (int i = 0; i < getChildrenCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->refreshSimilarityColumn();
        }
    }
}

void MsaEditorMultilineWgt::showSimilarity() {
    for (int i = 0; i < getChildrenCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->showSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::hideSimilarity() {
    for (int i = 0; i < getChildrenCount(); i++) {
        if (auto ui = qobject_cast<MsaEditorWgt*>(uiChild[i])) {
            ui->hideSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::sl_onPosChangeRequest(int position) {
    if (getMultilineMode()) {
        getScrollController()->scrollToBase(QPoint(position, 0));
    } else {
        getUI(0)->getScrollController()->scrollToBase(position, getSequenceAreaWidth(0));
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
        for (int i = 0; i < getChildrenCount(); i++) {
            MaEditorWgt* child = getUI(i);
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
        QPoint newPos(cursorPosition);

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
        QPoint newPos(cursorPosition);

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

}  // namespace U2
