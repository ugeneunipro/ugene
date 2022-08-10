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

#include <U2Gui/GScrollBar.h>

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

MsaEditorMultilineWgt::MsaEditorMultilineWgt(MSAEditor* editor, bool multiline)
    : MaEditorMultilineWgt(editor),
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

    connect(editor,
            SIGNAL(si_cursorPositionChanged(const QPoint&)),
            SLOT(sl_cursorPositionChanged(const QPoint&)));
}

MaEditorWgt* MsaEditorMultilineWgt::createChild(MaEditor* editor,
                                                MaEditorOverviewArea* overviewArea,
                                                MaEditorStatusBar* statusBar) {
    MsaEditorWgt* child = new MsaEditorWgt(qobject_cast<MSAEditor*>(editor),
                                           overviewArea,
                                           statusBar);
    SAFE_POINT(child != nullptr, "Can't create sequence widget", nullptr);
    return child;
}

void MsaEditorMultilineWgt::deleteChild(int index) {
    if (index < 0 || index >= (int)uiChildCount) {
        return;
    }

    MaEditorWgt* toDelete = getUI(index);
    QVBoxLayout* layout = (QVBoxLayout*)uiChildrenArea->layout();

    uiLog.details(tr("Deleting widget from grid, count %1, index %2").arg(layout->count()).arg(index));
    toDelete->hide();
    layout->removeWidget(toDelete);

    uiLog.details(
        tr("Deleting widget from uiChild, count %1, index %2").arg(uiChild.size()).arg(index));
    uiChild.remove(index);
    uiChild.resize(uiChildLength);
    uiChildCount--;
    uiLog.details(tr("Deleted widget, uiChildCount %1").arg(uiChildCount));

    delete toDelete;
}

void MsaEditorMultilineWgt::addChild(MaEditorWgt* child, int index) {
    if (uiChildLength == 0) {
        uiChildLength = 8;
        uiChild.resize(uiChildLength);
        uiChildCount = 0;
    }

    if (index < 0 || index > (int)uiChildCount) {
        index = uiChildCount;
    }

    if (index >= (int)uiChildLength) {
        uiChildLength = index * 2;
        uiChild.resize(uiChildLength);
    }

    uiChild[index] = child;
    uiChildCount++;

    QVBoxLayout* vbox = (QVBoxLayout*)uiChildrenArea->layout();
    vbox->addWidget(child);

    child->setObjectName(QString("msa_editor_" + editor->getMaObject()->getGObjectName() + "_%1").arg(index));
    child->getScrollController()->setHScrollBarVisible(!getMultilineMode());

    connect(child->getScrollController(), SIGNAL(si_visibleAreaChanged()), getScrollController(), SLOT(sl_updateScrollBars()));

    if (getMultilineMode()) {
        connect(child->getScrollController(), SIGNAL(si_visibleAreaChanged()), scrollController, SIGNAL(si_hScrollValueChanged()));
    }
    scrollController->sl_updateScrollBars();

    setActiveChild(child);
}

void MsaEditorMultilineWgt::createChildren() {
    uint childrenCount = getMultilineMode() ? 3 : 1;

    MaEditorOverviewArea* overviewArea = this->getOverviewArea();
    MaEditorStatusBar* statusBar = this->getStatusBar();
    MaEditorWgt* child = nullptr;
    for (uint i = 0; i < childrenCount; i++) {
        child = createChild(editor, overviewArea, statusBar);
        SAFE_POINT(child != nullptr, "Can't create sequence widget", );
        addChild(child);

        // recalculate count
        if (i == 0 && getMultilineMode()) {
            QSize s = child->minimumSizeHint();
            childrenCount = height() / s.height() + 3;
            uint l = editor->getAlignmentLen();
            uint aw = getSequenceAreaAllBaseWidth();
            uint al = getSequenceAreaAllBaseLen();

            // TODO:ichebyki: 0.66 is a heuristic value, need to define more smart
            uint b = width() * 0.66 / (aw / al);
            if (b * (childrenCount - 1) > l) {
                childrenCount = l / b + (l % b > 0 ? 1 : 0);
            }
        }
    }

    // TODO:ichebyki
    // Need to move to special method
    // wich ass/updates connections
    for (uint i = 0; i < this->getChildrenCount(); i++) {
        connect(getUI(i)->getNameAndSequenceAreasSplitter(),
                &QSplitter::splitterMoved,
                this,
                &MsaEditorMultilineWgt::sl_setAllNameAndSequenceAreasSplittersSizes);
    }
}

bool MsaEditorMultilineWgt::updateChildrenCount() {
    bool updated = false;

    if (getMultilineMode()) {
        MaEditorWgt* child = nullptr;
        int rowCount = editor->getNumSequences();
        int rowHeight = editor->getRowHeight();
        int headHeight = getUI(0)->getHeaderWidget()->height();
        int childrenAreaHeight = getChildrenScrollArea()->height();
        uint wantCount = childrenAreaHeight / (rowCount * rowHeight + headHeight) + 2;

        uint l = editor->getAlignmentLen();
        uint b = getSequenceAreaBaseLen(0);
        if (b * wantCount > l) {
            wantCount = l / b + (l % b > 0 ? 1 : 0);
        }

        if (getChildrenCount() < wantCount) {
            while (getChildrenCount() <= 8 && getChildrenCount() < wantCount) {
                child = createChild(editor, overviewArea, statusBar);
                SAFE_POINT(child != nullptr, "Can't create sequence widget", updated);
                addChild(child);
                updated = true;
            }
        } else if (getChildrenCount() > wantCount) {
            while (getChildrenCount() > 3 && getChildrenCount() > wantCount) {
                deleteChild(getChildrenCount() - 1);
                updated = true;
            }
        }
    }

    return updated;
}

void MsaEditorMultilineWgt::updateChildren() {
    if (treeView) {
        // TODO:ichebyki
        // Need complex save/update for phyl-tree
        // Then, we will able to reuse tree view
        MSAEditorMultiTreeViewer* treeViewer = qobject_cast<MsaEditorWgt*>(uiChild[0])
                                                   ->getMultiTreeViewer();
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
        MsaEditorWgt* child = qobject_cast<MsaEditorWgt*>(uiChild[uiChildCount - 1]);
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

void MsaEditorMultilineWgt::initScrollArea(QScrollArea* _scrollArea) {
    if (_scrollArea == nullptr) {
        scrollArea = new MsaMultilineScrollArea(editor, this);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        scrollArea = _scrollArea;
    }
    scrollArea->setWidgetResizable(true);
}

void MsaEditorMultilineWgt::initOverviewArea(MaEditorOverviewArea* _overviewArea) {
    if (_overviewArea == nullptr) {
        overviewArea = new MSAEditorOverviewArea(this);
    } else {
        overviewArea = _overviewArea;
    }
}

void MsaEditorMultilineWgt::initStatusBar(MaEditorStatusBar* _statusBar) {
    if (_statusBar == nullptr) {
        statusBar = new MsaEditorStatusBar(getEditor());
    } else {
        statusBar = _statusBar;
    }
}

void MsaEditorMultilineWgt::initChildrenArea(QGroupBox* _uiChildrenArea) {
    if (_uiChildrenArea == nullptr) {
        uiChildrenArea = new QGroupBox();
        uiChildrenArea->setFlat(true);
        uiChildrenArea->setStyleSheet("border:0;");
        uiChildrenArea->setObjectName("msa_editor_multiline_children_area");
    } else {
        uiChildrenArea = _uiChildrenArea;
    }
}

MaEditorWgt* MsaEditorMultilineWgt::getUI(uint index) const {
    return !(index < uiChildCount)
               ? nullptr
               : qobject_cast<MsaEditorWgt*>(uiChild[index]);
}

void MsaEditorMultilineWgt::updateSize(bool recurse) {
    if (recurse) {
        for (uint i = 0; i < getChildrenCount(); i++) {
            MaEditorWgt* w = getUI(i);
            QSize s = w->getSequenceArea()->minimumSizeHint();

            w->getEditorNameList()->setMinimumSize(s);
            w->getSequenceArea()->setMinimumSize(s);
            w->setMinimumSize(w->minimumSizeHint());
        }
    }
    updateGeometry();
}

void MsaEditorMultilineWgt::addPhylTreeWidget(MSAEditorMultiTreeViewer* multiTreeViewer) {
    this->multiTreeViewer = multiTreeViewer;
    treeSplitter->insertWidget(0, multiTreeViewer);
    treeSplitter->setSizes(QList<int>({500, 600}));
    treeSplitter->setStretchFactor(0, 1);
    treeSplitter->setStretchFactor(1, 3);

    treeView = true;
}

void MsaEditorMultilineWgt::delPhylTreeWidget() {
    if (multiTreeViewer != nullptr) {
        delete multiTreeViewer;
        multiTreeViewer = nullptr;
    }
}

MSAEditorTreeViewer* MsaEditorMultilineWgt::getCurrentTree() const {
    if (nullptr == multiTreeViewer) {
        return nullptr;
    }
    GObjectViewWindow* page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    if (nullptr == page) {
        return nullptr;
    }
    return qobject_cast<MSAEditorTreeViewer*>(page->getObjectView());
}

void MsaEditorMultilineWgt::sl_changeColorSchemeOutside(const QString& id) {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->sl_changeColorSchemeOutside(id);
    }
}

void MsaEditorMultilineWgt::sl_changeColorScheme(const QString& id) {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->applyColorScheme(id);
    }
}

void MsaEditorMultilineWgt::sl_triggerUseDots(int checkState) {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MaEditorSequenceArea* sequence = getUI(i)->getSequenceArea();
        sequence->sl_triggerUseDots(checkState);
    }
}

void MsaEditorMultilineWgt::sl_cursorPositionChanged(const QPoint& point) {
    if (multilineMode) {
        scrollController->scrollToPoint(point, size());
    }
}

void MsaEditorMultilineWgt::setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MsaEditorWgt* ui = qobject_cast<MsaEditorWgt*>(uiChild[i]);
        if (ui != nullptr) {
            ui->setSimilaritySettings(settings);
        }
    }
}

void MsaEditorMultilineWgt::refreshSimilarityColumn() {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MsaEditorWgt* ui = qobject_cast<MsaEditorWgt*>(uiChild[i]);
        if (ui != nullptr) {
            ui->refreshSimilarityColumn();
        }
    }
}

void MsaEditorMultilineWgt::showSimilarity() {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MsaEditorWgt* ui = qobject_cast<MsaEditorWgt*>(uiChild[i]);
        if (ui != nullptr) {
            ui->showSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::hideSimilarity() {
    for (uint i = 0; i < getChildrenCount(); i++) {
        MsaEditorWgt* ui = qobject_cast<MsaEditorWgt*>(uiChild[i]);
        if (ui != nullptr) {
            ui->hideSimilarity();
        }
    }
}

void MsaEditorMultilineWgt::sl_onPosChangeRequest(int position) {
    getScrollController()->centerBase(position, getUI(0)->getSequenceArea()->width());
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
        for (uint i = 0; i < getChildrenCount(); i++) {
            MaEditorWgt* child = getUI(i);
            child->getNameAndSequenceAreasSplitter()->setSizes(sizes);
        }
    }
}

}  // namespace U2
