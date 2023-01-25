/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MaEditorNameList.h"

#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QMouseEvent>
#include <QPainter>

#include <U2Core/ClipboardController.h>
#include <U2Core/Counter.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Theme.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/Notification.h>

#include "DrawHelper.h"
#include "MSAEditor.h"
#include "MaEditorSelection.h"
#include "MaEditorSequenceArea.h"
#include "MaEditorWgt.h"
#include "RowHeightController.h"
#include "ScrollController.h"

namespace U2 {

MaEditorNameList::MaEditorNameList(MaEditorWgt* _ui, QScrollBar* _nhBar)
    : labels(nullptr),
      ui(_ui),
      nhBar(_nhBar),
      changeTracker(nullptr),
      editor(_ui->getEditor()) {
    setObjectName("msa_editor_name_list");
    setFocusPolicy(Qt::WheelFocus);
    cachedView = new QPixmap();
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    editSequenceNameAction = new QAction(tr("Edit sequence name"), this);
    editSequenceNameAction->setObjectName("edit_sequence_name");
    editSequenceNameAction->setShortcut(QKeySequence(Qt::Key_F2));
    editSequenceNameAction->setShortcutContext(Qt::WidgetShortcut);
    connect(editSequenceNameAction, SIGNAL(triggered()), SLOT(sl_editSequenceName()));
    addAction(editSequenceNameAction);

    copyWholeRowAction = new QAction(tr("Copy whole selected row(s)"), this);
    copyWholeRowAction->setObjectName("copy_whole_row");
    connect(copyWholeRowAction, SIGNAL(triggered()), SLOT(sl_copyWholeRow()));

    removeSequenceAction = new QAction(tr("Remove sequence(s)"), this);
    removeSequenceAction->setObjectName("Remove sequence");
    removeSequenceAction->setShortcutContext(Qt::WidgetShortcut);
    connect(removeSequenceAction, SIGNAL(triggered()), SLOT(sl_removeSelectedRows()));
    addAction(removeSequenceAction);

    if (editor->getMaObject()) {
        connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), SLOT(sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)));
        connect(editor->getMaObject(), SIGNAL(si_lockedStateChanged()), SLOT(sl_lockedStateChanged()));
        changeTracker = new MsaEditorUserModStepController(editor->getMaObject()->getEntityRef());
    }

    connect(this, SIGNAL(si_startMaChanging()), ui, SIGNAL(si_startMaChanging()));
    connect(this, SIGNAL(si_stopMaChanging(bool)), ui, SIGNAL(si_stopMaChanging(bool)));

    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)));
    connect(editor, SIGNAL(si_fontChanged(const QFont&)), SLOT(sl_completeUpdate()));
    connect(editor->getCollapseModel(), SIGNAL(si_toggled()), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), SLOT(sl_completeRedraw()));
    connect(editor, SIGNAL(si_cursorPositionChanged(const QPoint&)), SLOT(sl_completeRedraw()));
    connect(editor, SIGNAL(si_completeUpdate()), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_updateActions()), SLOT(sl_updateActions()));
    connect(ui, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));
    connect(ui->getScrollController(), SIGNAL(si_visibleAreaChanged()), SLOT(sl_completeRedraw()));
    connect(ui->getScrollController()->getVerticalScrollBar(), SIGNAL(actionTriggered(int)), SLOT(sl_vScrollBarActionPerformed()));

    nhBar->setParent(this);
    nhBar->setVisible(false);
    sl_updateActions();

    QObject* labelsParent = new QObject(this);
    labelsParent->setObjectName("labels_parent");
    labels = new QObject(labelsParent);
}

MaEditorNameList::~MaEditorNameList() {
    delete cachedView;
    delete changeTracker;
}

QSize MaEditorNameList::getCanvasSize(const QList<int>& seqIdx) const {
    return QSize(width(), ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(seqIdx));
}

void MaEditorNameList::drawNames(QPainter& painter, const QList<int>& maRows, bool drawSelection) {
    painter.fillRect(painter.viewport(), Qt::white);

    const MaEditorSelection& selection = editor->getSelection();
    const MaCollapseModel* collapseModel = editor->getCollapseModel();
    int referenceViewRowIndex = collapseModel->getViewRowIndexByMaRowId(editor->getReferenceRowId());
    for (int maRowIndex : qAsConst(maRows)) {
        QString rowText = getTextForRow(maRowIndex);
        U2Region yRange = ui->getRowHeightController()->getGlobalYRegionByMaRowIndex(maRowIndex, maRows);
        int viewRowIndex = collapseModel->getViewRowIndexByMaRowIndex(maRowIndex);
        bool isSelected = drawSelection && selection.containsRow(viewRowIndex);
        bool isReference = viewRowIndex == referenceViewRowIndex;
        drawSequenceItem(painter, rowText, yRange, isSelected, isReference);
    }
}

QAction* MaEditorNameList::getEditSequenceNameAction() const {
    return editSequenceNameAction;
}

QAction* MaEditorNameList::getRemoveSequenceAction() const {
    return removeSequenceAction;
}

void MaEditorNameList::setSelection(const MaEditorSelection& selection) {
    editor->getSelectionController()->setSelection(selection);
}

void MaEditorNameList::updateScrollBar() {
    nhBar->disconnect(this);

    QFont f = editor->getFont();
    f.setItalic(true);
    QFontMetrics fm(f, this);
    int maxNameWidth = 0;

    MultipleAlignmentObject* maObj = editor->getMaObject();
    foreach (const MultipleAlignmentRow& row, maObj->getMultipleAlignment()->getRows()) {
        maxNameWidth = qMax(fm.width(row->getName()), maxNameWidth);
    }

    // Adjustment for branch primitive in collapsing mode
    bool hasChildLabels = editor->getCollapseModel()->hasGroupsWithMultipleRows();
    maxNameWidth += getGroupExpanderWidth() + (hasChildLabels ? CHILDREN_OFFSET : 0);

    int availableWidth = getAvailableWidth();
    int nSteps = 1;
    int stepSize = fm.width('W');
    if (availableWidth < maxNameWidth) {
        int dw = maxNameWidth - availableWidth;
        nSteps += dw / stepSize + (dw % stepSize != 0 ? 1 : 0);
    }
    nhBar->setMinimum(0);
    nhBar->setMaximum(nSteps - 1);
    nhBar->setValue(0);

    nhBar->setVisible(nSteps > 1);
    connect(nhBar, SIGNAL(valueChanged(int)), SLOT(sl_completeRedraw()));
}

int MaEditorNameList::getGroupExpanderWidth() const {
    return ui->isCollapsingOfSingleRowGroupsEnabled() || editor->getCollapseModel()->hasGroupsWithMultipleRows() ? 2 * CROSS_SIZE : 0;
}

void MaEditorNameList::sl_copyWholeRow() {
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );
    const QList<QRect>& selectedRects = selection.getRectList();
    const MaCollapseModel* collapseModel = editor->getCollapseModel();
    const MultipleAlignmentObject* maObject = editor->getMaObject();
    qint64 maLength = maObject->getLength();
    qint64 estimatedResultLength = 0;
    for (const QRect& selectedRect : qAsConst(selectedRects)) {
        estimatedResultLength += selectedRect.height() * maLength;
    }
    if (estimatedResultLength > U2Clipboard::MAX_SAFE_COPY_TO_CLIPBOARD_SIZE) {
        uiLog.error(tr("Block size is too big and can't be copied into the clipboard"));
        return;
    }
    QString resultText;
    for (const QRect& selectedRect : qAsConst(selectedRects)) {
        for (int viewRowIndex = selectedRect.top(); viewRowIndex <= selectedRect.bottom(); viewRowIndex++) {
            int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
            SAFE_POINT(maRowIndex >= 0, "Can't get MA index by View index", );
            MultipleAlignmentRow row = maObject->getRow(maRowIndex);
            if (!resultText.isEmpty()) {
                resultText += "\n";
            }
            U2OpStatus2Log os;
            QByteArray sequence = row->toByteArray(os, maObject->getLength());
            CHECK_OP_EXT(os, uiLog.error(os.getError()), );
            resultText.append(QString::fromLatin1(sequence));
        }
    }
    QApplication::clipboard()->setText(resultText);
}

void MaEditorNameList::sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo& mi) {
    if (mi.rowListChanged) {
        completeRedraw = true;
        sl_updateActions();
        updateScrollBar();
        update();
    }
}

void MaEditorNameList::sl_removeSelectedRows() {
    GCounter::increment("Remove row", editor->getFactoryId());
    QList<QRect> selectedRects = editor->getSelection().getRectList();
    CHECK(!selectedRects.isEmpty(), );

    MultipleAlignmentObject* maObj = editor->getMaObject();
    CHECK(!maObj->isStateLocked(), );

    // View selection converted to MSA row indexes
    QList<int> selectedMaRowIndexes;
    for (auto selectedRect : qAsConst(selectedRects)) {
        U2Region yRegion = U2Region::fromYRange(selectedRect);
        selectedMaRowIndexes << editor->getCollapseModel()->getMaRowIndexesByViewRowIndexes(yRegion, true);
    }
    if (maObj->getRowCount() == selectedMaRowIndexes.size()) {
        NotificationStack::addNotification(tr("Impossible to delete whole alignment!"), NotificationType::Warning_Not);
        return;
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );

    setSelection({});  // Clear selection.
    maObj->removeRows(selectedMaRowIndexes);

    int numRows = editor->getCollapseModel()->getViewRowCount();
    const QRect& firstSelectedRect = selectedRects.first();
    if (firstSelectedRect.top() < numRows) {
        QRect newSelectedRect(firstSelectedRect.left(), firstSelectedRect.top(), firstSelectedRect.width(), 1);
        setSelection({{newSelectedRect}});
    } else if (numRows > 0) {
        // Select the last sequence. This sequence was right before the removed selection.
        QRect newSelectedRect(firstSelectedRect.left(), numRows - 1, firstSelectedRect.width(), 1);
        setSelection({{newSelectedRect}});
    }
}

void MaEditorNameList::sl_lockedStateChanged() {
    sl_updateActions();
}

void MaEditorNameList::resizeEvent(QResizeEvent* e) {
    completeRedraw = true;
    updateScrollBar();
    QWidget::resizeEvent(e);
}

void MaEditorNameList::paintEvent(QPaintEvent*) {
    drawAll();
}

void MaEditorNameList::mouseDoubleClickEvent(QMouseEvent* e) {
    if (editor->gotoSelectedReadAction->isEnabled()) {
        editor->gotoSelectedReadAction->trigger();
        e->ignore();
        return;
    }
    QWidget::mouseDoubleClickEvent(e);
}

void MaEditorNameList::keyPressEvent(QKeyEvent* e) {
    int key = e->key();
    Qt::KeyboardModifiers modifiers = e->modifiers();
    bool isShiftPressed = modifiers.testFlag(Qt::ShiftModifier);
    int cursorRow = editor->getCursorPosition().y();

    switch (key) {
        case Qt::Key_Space:
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (modifiers == Qt::NoModifier && editor->gotoSelectedReadAction->isEnabled()) {
                editor->gotoSelectedReadAction->trigger();
                e->ignore();
                return;
            }
            break;
        case Qt::Key_Up: {
            const MaEditorSelection& selection = editor->getSelection();
            if (selection.isEmpty() || selection.isMultiRegionSelection()) {
                break;
            }
            if (isShiftPressed) {  // Extend or shrink the selection.
                QRect selectedRect = selection.getRectList().first();
                bool grow = selectedRect.height() == 1 || selectedRect.top() < cursorRow;
                if (grow) {
                    if (selectedRect.top() > 0) {
                        setSelection({{selectedRect.adjusted(0, -1, 0, 0)}});
                    }
                } else {  // Shrink.
                    setSelection({{selectedRect.adjusted(0, 0, 0, -1)}});
                }
                scrollSelectionToView(grow);
            } else {
                moveSelection(-1, true);
            }
            break;
        }
        case Qt::Key_Down: {
            const MaEditorSelection& selection = editor->getSelection();
            if (selection.isEmpty() || selection.isMultiRegionSelection()) {
                break;
            }
            if (isShiftPressed) {  // Extend or shrink the selection.
                QRect selectedRect = selection.getRectList().first();
                bool grow = selectedRect.height() == 1 || cursorRow < selectedRect.bottom();
                int numRows = editor->getCollapseModel()->getViewRowCount();
                if (grow) {
                    if (selectedRect.bottom() + 1 < numRows) {
                        setSelection({{selectedRect.adjusted(0, 0, 0, 1)}});
                    }
                } else {  // Shrink.
                    setSelection({{selectedRect.adjusted(0, 1, 0, 0)}});
                }
                scrollSelectionToView(!grow);
            } else {
                moveSelection(1, true);
            }
            break;
        }
        case Qt::Key_Left: {
            // Perform collapse action on the collapsed group by default and fallback to the horizontal scrolling
            if (!triggerExpandCollapseOnSelectedRow(true)) {
                nhBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
            }
            break;
        }
        case Qt::Key_Right: {
            // Perform expand action on the collapsed group by default and fallback to the horizontal scrolling
            if (!triggerExpandCollapseOnSelectedRow(false)) {
                nhBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
            }
            break;
        }
        case Qt::Key_Home:
            ui->getScrollController()->scrollToEnd(ScrollController::Up);
            break;
        case Qt::Key_End:
            ui->getScrollController()->scrollToEnd(ScrollController::Down);
            break;
        case Qt::Key_PageUp:
            ui->getScrollController()->scrollPage(ScrollController::Up);
            break;
        case Qt::Key_PageDown:
            ui->getScrollController()->scrollPage(ScrollController::Down);
            break;
        case Qt::Key_Escape:
            editor->getSelectionController()->clearSelection();
            break;
        default:
            break;
    }
    QWidget::keyPressEvent(e);
}

void MaEditorNameList::mousePressEvent(QMouseEvent* e) {
    setFocus();
    MaEditorSequenceArea* seqArea = ui->getSequenceArea();
    if (seqArea->isAlignmentEmpty() || e->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(e);
        return;
    }

    auto maObject = editor->getMaObject();
    maVersionBeforeMousePress = maObject->getModificationVersion();
    maObject->saveState();

    // FIXME: do not start tracking signal here. Do it when the real dragging starts.
    if (!maObject->isStateLocked()) {
        U2OpStatus2Log os;
        changeTracker->startTracking(os);
    }
    emit si_startMaChanging();

    mousePressPoint = e->pos();
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    RowHeightController* heightController = ui->getRowHeightController();
    int viewRow = qMin(heightController->getViewRowIndexByScreenYPosition(e->y()), collapseModel->getViewRowCount() - 1);

    // Do not update cursor position on clicks with Shift. Clicks with Shift update selection only.
    bool updateCursorPos = !e->modifiers().testFlag(Qt::ShiftModifier);
    if (updateCursorPos) {
        editor->setCursorPosition(QPoint(editor->getCursorPosition().x(), viewRow));
    }

    const MaCollapsibleGroup* group = getCollapsibleGroupByExpandCollapsePoint(mousePressPoint);
    if (group != nullptr) {
        collapseModel->toggle(viewRow);
        return;
    }

    // Control modifier is for spacial selection, Shift is for selection move.
    bool hasShiftModifier = e->modifiers().testFlag(Qt::ShiftModifier);
    bool hasCtrlModifier = e->modifiers().testFlag(Qt::ControlModifier);
    if (editor->getSelection().containsRow(viewRow) && !hasCtrlModifier && !hasShiftModifier) {
        // We support dragging only for 'flat' mode, when there are no groups with multiple sequences.
        // TODO: support dragging in the Free mode (v39). Today Free mode is enabled only when Sync is ON with tree: MSA order is enforced by Tree.
        isDragging = !editor->getCollapseModel()->hasGroupsWithMultipleRows() && editor->getRowOrderMode() != MaEditorRowOrderMode::Free;
    } else if (!hasShiftModifier) {
        rubberBand->setGeometry(QRect(mousePressPoint, QSize()));
        rubberBand->show();
    }

    QWidget::mousePressEvent(e);
}

void MaEditorNameList::mouseMoveEvent(QMouseEvent* e) {
    bool isDragSelection = rubberBand->isVisible();
    bool isDragSequences = !isDragSelection && isDragging;
    bool isMoveSelection = !isDragSelection && !isDragSequences && e->modifiers().testFlag(Qt::ShiftModifier) && !editor->getSelection().isEmpty();

    if (!isDragSelection && !isDragSequences && !isMoveSelection) {
        QWidget::mouseMoveEvent(e);
        return;
    }
    // Scroll to view if needed.
    int mouseRow = ui->getRowHeightController()->getViewRowIndexByScreenYPosition(e->y());
    if (ui->getSequenceArea()->isSeqInRange(mouseRow)) {
        if (ui->getSequenceArea()->isRowVisible(mouseRow, false)) {
            ui->getScrollController()->stopSmoothScrolling();
        } else {
            ScrollController::Directions direction = ScrollController::None;
            if (mouseRow < ui->getScrollController()->getFirstVisibleViewRowIndex(false)) {
                direction |= ScrollController::Up;
            } else if (mouseRow > ui->getScrollController()->getLastVisibleViewRowIndex(height(), false)) {
                direction |= ScrollController::Down;
            }
            ui->getScrollController()->scrollSmoothly(direction);
        }
    }

    if (isDragSequences) {
        moveSelectedRegion(mouseRow - editor->getCursorPosition().y());
    } else if (isDragSelection) {
        rubberBand->setGeometry(QRect(mousePressPoint, e->pos()).normalized());
    } else if (isMoveSelection && mouseRow != -1) {  // getViewRowIndexByScreenYPosition returns -1 for out of range 'y' values.
        moveSelection(mouseRow - editor->getCursorPosition().y(), false);
    }
    QWidget::mouseMoveEvent(e);
}

void MaEditorNameList::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(e);
        return;
    }
    bool hasShiftModifier = e->modifiers().testFlag(Qt::ShiftModifier);
    bool hasCtrlModifier = e->modifiers().testFlag(Qt::ControlModifier);
    ScrollController* scrollController = ui->getScrollController();

    RowHeightController* rowsController = ui->getRowHeightController();
    int maxRows = ui->getSequenceArea()->getViewRowCount();
    int lastVisibleRow = scrollController->getLastVisibleViewRowIndex(height(), true);
    int lastVisibleRowY = (int)rowsController->getScreenYRegionByViewRowIndex(lastVisibleRow).endPos();

    QList<QRect> oldSelectedRects = editor->getSelection().getRectList();

    // mousePressRowExt has extended range: -1 (before first) to maxRows (after the last)
    int mousePressRowExt = mousePressPoint.y() >= lastVisibleRowY ? maxRows : rowsController->getViewRowIndexByScreenYPosition(mousePressPoint.y());
    int mousePressRow = qBound(0, mousePressRowExt, maxRows - 1);

    // mouseReleaseRowExt has extended range: -1 (before first) to maxRows (after the last)
    int mouseReleaseRowExt = e->y() >= lastVisibleRowY ? maxRows : rowsController->getViewRowIndexByScreenYPosition(e->y());
    int mouseReleaseRow = qBound(0, mouseReleaseRowExt, maxRows - 1);
    QPoint cursorPos = editor->getCursorPosition();

    bool isClick = e->pos() == mousePressPoint;
    U2Region nameListRegion(0, maxRows);
    if (isClick && getCollapsibleGroupByExpandCollapsePoint(mousePressPoint) != nullptr) {
        // Do nothing. Expand collapse is processed as a part of MousePress.
    } else if (!isClick && isDragging) {
        if (oldSelectedRects.size() == 1) {  // Drag is supported for a trivial selection only.
            QRect oldSelectionRect = oldSelectedRects.first();
            int shift;
            if (mouseReleaseRow == 0) {
                shift = -oldSelectionRect.y();
            } else if (mouseReleaseRow == maxRows - 1) {
                shift = maxRows - (oldSelectionRect.bottom() + 1);
            } else {
                shift = mouseReleaseRow - editor->getCursorPosition().y();
            }
            moveSelectedRegion(shift);
        }
    } else if (nameListRegion.contains(mousePressRowExt) || nameListRegion.contains(mouseReleaseRowExt)) {
        // Click or drag (but not move) within name list.
        int alignmentLen = editor->getAlignmentLen();
        if (hasShiftModifier) {
            // Drag (non-click) is processed as a part of mouse move event.
            if (isClick) {
                QRect oldSelectionRect = oldSelectedRects.isEmpty() ? QRect() : oldSelectedRects.first();
                // Default is a 1 full row where mouse release event happens. Drag or key-modifiers will change this region.
                QRect newSelectionRect(0, cursorPos.y(), alignmentLen, 1);
                // Keep X range when adding more rows to the existing selection.
                if (!oldSelectionRect.isEmpty()) {
                    newSelectionRect.setLeft(oldSelectionRect.left());
                    newSelectionRect.setRight(oldSelectionRect.right());
                }
                if (mouseReleaseRow < cursorPos.y()) {
                    newSelectionRect.setTop(mouseReleaseRow);
                    newSelectionRect.setHeight(cursorPos.y() - mousePressRow + 1);
                } else if (mouseReleaseRow > cursorPos.y()) {
                    newSelectionRect.setTop(cursorPos.y());
                    newSelectionRect.setHeight(mousePressRow - cursorPos.y() + 1);
                }
                setSelection({{newSelectionRect}});
            }
        } else if (hasCtrlModifier && !oldSelectedRects.isEmpty()) {  // If old selection is empty -> process like a normal click or drag.
            QList<QRect> newSelectedRects;
            // Flip selected state of the clicked row. Keep X range intact.
            int left = oldSelectedRects.first().left();  // All rects have equal left & right.
            int right = oldSelectedRects.first().right();
            int top = qMin(mousePressRow, mouseReleaseRow);
            int bottom = qMax(mousePressRow, mouseReleaseRow);
            QRect flipRect(QPoint(left, top), QPoint(right, bottom));
            QRect unprocessedFlipRect = flipRect;
            for (const QRect& oldRect : qAsConst(oldSelectedRects)) {
                QRect intersectedRect = oldRect.intersected(unprocessedFlipRect);
                if (!intersectedRect.isEmpty()) {  // Check if intersects.
                    if (oldRect.top() < intersectedRect.top()) {
                        newSelectedRects << QRect(oldRect.topLeft(), intersectedRect.topRight() - QPoint(0, 1));
                    }
                    if (oldRect.bottom() > intersectedRect.bottom()) {
                        newSelectedRects << QRect(intersectedRect.bottomLeft() + QPoint(0, 1), oldRect.bottomRight());
                    }
                    if (unprocessedFlipRect.top() < intersectedRect.top()) {  // Add non-intersected part & crop remaining flip rect.
                        newSelectedRects << QRect(unprocessedFlipRect.topLeft(), intersectedRect.topRight() - QPoint(0, 1));
                    }
                    unprocessedFlipRect.setTop(oldRect.bottom() + 1);
                } else {
                    newSelectedRects << oldRect;
                }
            }
            if (!unprocessedFlipRect.isEmpty()) {
                newSelectedRects << unprocessedFlipRect;
            }
            QList<QRect> newSafeSelectedRects = MaEditorSelection::buildSafeSelectionRects(newSelectedRects);
            setSelection(newSafeSelectedRects);
        } else {  // Process like 'No modifiers'.
            // Drag or click with no modifiers make a new 1-rect selection.
            int y = qMin(mousePressRow, mouseReleaseRow);
            int width = qMax(mousePressRow, mouseReleaseRow) - y + 1;
            QRect newSelectionRect(0, y, alignmentLen, width);
            setSelection({{newSelectionRect}});
        }
    } else {
        setSelection({});
    }

    rubberBand->hide();
    isDragging = false;
    changeTracker->finishTracking();
    editor->getMaObject()->releaseState();
    emit si_stopMaChanging(maVersionBeforeMousePress != editor->getMaObject()->getModificationVersion());
    maVersionBeforeMousePress = -1;
    scrollController->stopSmoothScrolling();

    QWidget::mouseReleaseEvent(e);
}

void MaEditorNameList::wheelEvent(QWheelEvent* we) {
    bool toMin = we->delta() > 0;
    ui->getScrollController()->scrollStep(toMin ? ScrollController::Up : ScrollController::Down);
    QWidget::wheelEvent(we);
}

const MaCollapsibleGroup* MaEditorNameList::getCollapsibleGroupByExpandCollapsePoint(const QPoint& point) const {
    const MaCollapseModel* collapseModel = editor->getCollapseModel();
    RowHeightController* heightController = ui->getRowHeightController();
    int viewRow = heightController->getViewRowIndexByScreenYPosition(point.y());
    if (viewRow < 0 || viewRow >= collapseModel->getViewRowCount()) {
        return nullptr;
    }
    const MaCollapsibleGroup* group = collapseModel->getCollapsibleGroupByViewRow(viewRow);
    int minRowsInGroupToExpandCollapse = ui->isCollapsingOfSingleRowGroupsEnabled() ? 1 : 2;
    if (group == nullptr || group->size() < minRowsInGroupToExpandCollapse) {
        return nullptr;
    }
    U2Region yRange = heightController->getScreenYRegionByViewRowIndex(viewRow);
    QRect textRect = calculateTextRect(yRange, editor->getSelection().containsRow(viewRow));
    QRect buttonRect = calculateExpandCollapseButtonRect(textRect);
    return buttonRect.contains(point) ? group : nullptr;
}

void MaEditorNameList::sl_selectionChanged(const MaEditorSelection&, const MaEditorSelection&) {
    completeRedraw = true;
    update();
    sl_updateActions();
}

void MaEditorNameList::sl_updateActions() {
    copyWholeRowAction->setEnabled(!editor->getSelection().isEmpty());
    MultipleAlignmentObject* maObj = editor->getMaObject();
    const MaEditorSelection& selection = editor->getSelection();
    removeSequenceAction->setEnabled(!maObj->isStateLocked() && !selection.isEmpty());
    editSequenceNameAction->setEnabled(!maObj->isStateLocked() && selection.isSingleRowSelection());
}

void MaEditorNameList::sl_vScrollBarActionPerformed() {
    CHECK(isDragging, );

    GScrollBar* vScrollBar = qobject_cast<GScrollBar*>(sender());
    SAFE_POINT(vScrollBar != nullptr, "vScrollBar is NULL", );

    const QAbstractSlider::SliderAction action = vScrollBar->getRepeatAction();
    CHECK(QAbstractSlider::SliderSingleStepAdd == action || QAbstractSlider::SliderSingleStepSub == action, );

    const QPoint localPoint = mapFromGlobal(QCursor::pos());
    const int newSeqNum = ui->getRowHeightController()->getViewRowIndexByScreenYPosition(localPoint.y());
    moveSelectedRegion(newSeqNum - editor->getCursorPosition().y());
}

void MaEditorNameList::focusInEvent(QFocusEvent* fe) {
    QWidget::focusInEvent(fe);
    update();
}

void MaEditorNameList::focusOutEvent(QFocusEvent* fe) {
    QWidget::focusOutEvent(fe);
    update();
}

void MaEditorNameList::sl_completeUpdate() {
    completeRedraw = true;
    updateScrollBar();
    update();
}

void MaEditorNameList::sl_completeRedraw() {
    completeRedraw = true;
    update();
}

//////////////////////////////////////////////////////////////////////////
// draw methods
QFont MaEditorNameList::getFont(bool selected) const {
    QFont f = editor->getFont();
    f.setItalic(true);
    if (selected) {
        f.setBold(true);
    }
    return f;
}

QRect MaEditorNameList::calculateTextRect(const U2Region& yRange, bool selected) const {
    int textX = MARGIN_TEXT_LEFT;
    int textW = getAvailableWidth();
    int textY = yRange.startPos + MARGIN_TEXT_TOP;
    int textH = yRange.length - MARGIN_TEXT_TOP - MARGIN_TEXT_BOTTOM;
    QRect textRect(textX, textY, textW, textH);
    if (nhBar->isVisible()) {
        QFontMetrics fm(getFont(selected));
        int stepSize = fm.width('W');
        int dx = stepSize * nhBar->value();
        textRect = textRect.adjusted(-dx, 0, 0, 0);
    }
    return textRect;
}

QRect MaEditorNameList::calculateExpandCollapseButtonRect(const QRect& itemRect) const {
    return QRect(itemRect.left() + CROSS_SIZE / 2, itemRect.top() + MARGIN_TEXT_TOP, CROSS_SIZE, CROSS_SIZE);
}

int MaEditorNameList::getAvailableWidth() const {
    return width() - MARGIN_TEXT_LEFT;
}

void MaEditorNameList::drawAll() {
    QSize s = size() * devicePixelRatio();
    if (cachedView->size() != s) {
        delete cachedView;
        cachedView = new QPixmap(s);
        cachedView->setDevicePixelRatio(devicePixelRatio());
        completeRedraw = true;
    }
    if (completeRedraw) {
        QPainter pCached(cachedView);
        drawContent(pCached);
        completeRedraw = false;
    }
    QPainter p(this);
    p.drawPixmap(0, 0, *cachedView);
    drawSelection(p);
}

void MaEditorNameList::drawContent(QPainter& painter) {
    painter.fillRect(cachedView->rect(), Qt::white);
    CHECK(!editor->isAlignmentEmpty(), );

    if (labels) {
        labels->setObjectName("");
    }

    MultipleAlignmentObject* maObj = editor->getMaObject();
    SAFE_POINT(maObj != nullptr, "NULL Ma Object in MAEditorNameList::drawContent", );

    const MultipleAlignment ma = maObj->getMultipleAlignment();

    U2OpStatusImpl os;
    const int referenceIndex = editor->getReferenceRowId() == U2MsaRow::INVALID_ROW_ID ? U2MsaRow::INVALID_ROW_ID : ma->getRowIndexByRowId(editor->getReferenceRowId(), os);
    SAFE_POINT_OP(os, );

    const MaCollapseModel* collapsibleModel = editor->getCollapseModel();
    int crossSpacing = getGroupExpanderWidth();
    const ScrollController* scrollController = ui->getScrollController();
    int firstVisibleViewRow = scrollController->getFirstVisibleViewRowIndex(true);
    int lastVisibleViewRow = scrollController->getLastVisibleViewRowIndex(height(), true);
    const MaEditorSelection& selection = editor->getSelection();
    int minRowsInGroupToExpandCollapse = ui->isCollapsingOfSingleRowGroupsEnabled() ? 1 : 2;
    for (int viewRow = firstVisibleViewRow; viewRow <= lastVisibleViewRow; viewRow++) {
        int maRow = collapsibleModel->getMaRowIndexByViewRowIndex(viewRow);
        const MaCollapsibleGroup* group = collapsibleModel->getCollapsibleGroupByViewRow(viewRow);

        U2Region yRange = ui->getRowHeightController()->getScreenYRegionByViewRowIndex(viewRow);

        bool isSelected = selection.containsRow(viewRow);
        bool isReference = maRow == referenceIndex;
        QString text = getTextForRow(maRow);
        if (group != nullptr && group->size() >= minRowsInGroupToExpandCollapse) {
            QRect rect = calculateTextRect(yRange, isSelected);
            // SANGER_TODO: check reference
            if (group->maRows[0] == maRow) {
                drawCollapsibleSequenceItem(painter, maRow, text, rect, isSelected, group->isCollapsed, isReference);
            } else if (!group->isCollapsed) {
                drawChildSequenceItem(painter, text, rect, isSelected, isReference);
            }
        } else {
            painter.translate(crossSpacing, 0);
            drawSequenceItem(painter, text, yRange, isSelected, isReference);
            painter.translate(-crossSpacing, 0);
        }
    }
}

void MaEditorNameList::drawSequenceItem(QPainter& painter, const QString& text, const U2Region& yRange, bool isSelected, bool isReference) {
    QRect rect = calculateTextRect(yRange, isSelected);
    drawBackground(painter, text, rect, isReference);
    drawText(painter, text, rect, isSelected);
}

void MaEditorNameList::drawCollapsibleSequenceItem(QPainter& painter, int /*rowIndex*/, const QString& name, const QRect& rect, bool isSelected, bool isCollapsed, bool isReference) {
    drawBackground(painter, name, rect, isReference);
    drawCollapsePrimitive(painter, isCollapsed, rect);
    drawText(painter, name, rect.adjusted(CROSS_SIZE * 2, 0, 0, 0), isSelected);
}

void MaEditorNameList::drawChildSequenceItem(QPainter& painter, const QString& name, const QRect& rect, bool isSelected, bool isReference) {
    drawBackground(painter, name, rect, isReference);
    painter.translate(CROSS_SIZE * 2 + CHILDREN_OFFSET, 0);
    drawText(painter, name, rect, isSelected);
    painter.translate(-CROSS_SIZE * 2 - CHILDREN_OFFSET, 0);
}

void MaEditorNameList::drawBackground(QPainter& p, const QString&, const QRect& rect, bool isReference) {
    if (isReference) {
        p.fillRect(rect, QColor("#9999CC"));  // SANGER_TODO: create the const, reference  color
        return;
    }

    p.fillRect(rect, Qt::white);
}

void MaEditorNameList::drawText(QPainter& p, const QString& name, const QRect& rect, bool selected) {
    p.setFont(getFont(selected));
    p.drawText(rect, Qt::AlignTop | Qt::AlignLeft, name);  // SANGER_TODO: check the alignment
}

void MaEditorNameList::drawCollapsePrimitive(QPainter& p, bool collapsed, const QRect& rect) {
    QStyleOptionViewItem branchOption;
    branchOption.rect = calculateExpandCollapseButtonRect(rect);
    if (collapsed) {
        branchOption.state = QStyle::State_Children | QStyle::State_Sibling;  // test
    } else {
        branchOption.state = QStyle::State_Open | QStyle::State_Children;
    }
    style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p, this);
}

QString MaEditorNameList::getTextForRow(int maRowIndex) {
    QString rowName = editor->getMaObject()->getRow(maRowIndex)->getName();
    QString rowNamePrefix = "";
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    const MaCollapsibleGroup* group = collapseModel->getCollapsibleGroupByMaRow(maRowIndex);
    if (group != nullptr && group->maRows.size() > 1 && group->maRows.first() == maRowIndex) {
        rowNamePrefix = "[" + QString::number(group->maRows.size()) + "] ";
    }
    return rowNamePrefix + rowName;
}

void MaEditorNameList::drawSelection(QPainter& painter) {
    const QList<QRect>& selectionRectList = editor->getSelection().getRectList();

    CHECK(!selectionRectList.isEmpty(), );

    painter.save();
    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    for (const QRect& selectionRect : selectionRectList) {
        U2Region rowRange = U2Region::fromYRange(selectionRect);
        U2Region screenYRange = ui->getRowHeightController()->getScreenYRegionByViewRowsRegion(rowRange);
        QRect screenRect(0, (int)screenYRange.startPos, width() - 1, (int)screenYRange.length - 1);
        if (screenRect.isValid()) {
            painter.drawRect(screenRect);
        }
    }
    painter.restore();
}

void MaEditorNameList::sl_editSequenceName() {
    GCounter::increment("Rename row", editor->getFactoryId());
    MultipleAlignmentObject* maObj = editor->getMaObject();
    CHECK(!maObj->isStateLocked(), );

    const MaEditorSelection& selection = editor->getSelection();
    CHECK(selection.isSingleRowSelection(), );

    int viewRowIndex = selection.getRectList()[0].top();
    int maRowIndex = editor->getCollapseModel()->getMaRowIndexByViewRowIndex(viewRowIndex);

    QString curName = maObj->getRow(maRowIndex)->getName();

    bool isMca = maObj->getGObjectType() == GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT;
    QString title = isMca ? tr("Rename Read") : tr("Rename Sequence");
    bool ok = false;
    QString newName = QInputDialog::getText(editor->getUI(), title, tr("New name:"), QLineEdit::Normal, curName, &ok);

    if (ok && !newName.isEmpty() && curName != newName) {
        emit si_sequenceNameChanged(curName, newName);
        maObj->renameRow(maRowIndex, newName);
    }
}

void MaEditorNameList::groupSelectedSequencesIntoASingleRegion(int stableRowIndex, U2OpStatus& os) {
    const MaEditorSelection& selection = editor->getSelection();
    const QList<QRect>& rects = selection.getRectList();
    CHECK(rects.size() > 1, );

    QVector<U2Region> regions(rects.size());
    std::transform(rects.begin(), rects.end(), regions.begin(), [](auto& rect) { return U2Region::fromYRange(rect); });
    auto stableRegionPosition = std::find_if(regions.begin(),
                                             regions.end(),
                                             [&stableRowIndex](auto& region) { return region.contains(stableRowIndex); });
    SAFE_POINT_EXT(stableRegionPosition != regions.end(),
                   os.setError(L10N::internalError("stableRowIndex is not within the selection")), );
    int stableRegionIndex = std::distance(regions.begin(), stableRegionPosition);

    U2Region stableRegion = regions[stableRegionIndex];
    QList<qint64> rowOrder = editor->getMaRowIds();

    for (int regionIndex = stableRegionIndex; --regionIndex >= 0;) {
        const U2Region& region = regions[regionIndex];
        for (int rowIndex = region.endPos(); --rowIndex >= region.startPos; stableRegion.startPos--, stableRegion.length++) {
            rowOrder.move(rowIndex, stableRegion.startPos - 1);
        }
    }

    for (int regionIndex = stableRegionIndex + 1; regionIndex < regions.size(); regionIndex++) {
        const U2Region& region = regions[regionIndex];
        for (int rowIndex = region.startPos; rowIndex < region.endPos(); rowIndex++, stableRegion.length++) {
            rowOrder.move(rowIndex, stableRegion.endPos());
        }
    }

    editor->getMaObject()->updateRowsOrder(os, rowOrder);
    CHECK_OP(os, );

    U2Region columnRegion = selection.getColumnRegion();
    QRect newSelectedRect(columnRegion.startPos, stableRegion.startPos, columnRegion.length, stableRegion.length);
    setSelection({{newSelectedRect}});
}

void MaEditorNameList::moveSelectedRegion(int shift) {
    CHECK(shift != 0, );
    MultipleAlignmentObject* maObj = editor->getMaObject();
    CHECK(!maObj->isStateLocked(), );

    const MaEditorSelection& selection = editor->getSelection();
    SAFE_POINT(!selection.isEmpty(), "moveSelectedRegion with no selection!", );
    U2OpStatus2Log os;
    int multiRegionStableRowIndex = editor->getCursorPosition().y();
    groupSelectedSequencesIntoASingleRegion(multiRegionStableRowIndex, os);
    CHECK_OP(os, );
    SAFE_POINT(selection.getRectList().size() == 1, "Expected to have a single continuous selection.", );

    QRect selectedRect = selection.getRectList().first();
    int numRowsInSelection = selectedRect.height();
    int firstRowInSelection = selectedRect.top();
    int lastRowInSelection = selectedRect.bottom();

    // "Out-of-range" checks.
    if ((shift > 0 && lastRowInSelection + shift >= editor->getNumSequences()) ||
        (shift < 0 && firstRowInSelection + shift < 0) ||
        (shift < 0 && firstRowInSelection + qAbs(shift) > editor->getNumSequences())) {
        return;
    }
    maObj->moveRowsBlock(firstRowInSelection, numRowsInSelection, shift);
    editor->setCursorPosition(editor->getCursorPosition() + QPoint(0, shift));
    QRect newSelectedRect(selectedRect.x(), firstRowInSelection + shift, selectedRect.width(), numRowsInSelection);
    setSelection({{newSelectedRect}});
}

qint64 MaEditorNameList::sequenceIdAtPos(const QPoint& p) {
    int rowIndex = ui->getRowHeightController()->getViewRowIndexByScreenYPosition(p.y());
    CHECK(ui->getSequenceArea()->isSeqInRange(rowIndex), U2MsaRow::INVALID_ROW_ID);
    CHECK(rowIndex >= 0, U2MsaRow::INVALID_ROW_ID);
    MultipleAlignmentObject* maObj = editor->getMaObject();
    return maObj->getMultipleAlignment()->getRow(editor->getCollapseModel()->getMaRowIndexByViewRowIndex(rowIndex))->getRowId();
}

void MaEditorNameList::moveSelection(int offset, bool resetXRange) {
    CHECK(offset != 0, );
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );

    QList<QRect> oldSelectedRects = selection.getRectList();
    int safeOffset = offset;
    if (safeOffset < 0) {
        if (oldSelectedRects.first().top() + safeOffset < 0) {
            safeOffset = -oldSelectedRects.first().top();
        }
    } else {
        int viewRowCount = editor->getCollapseModel()->getViewRowCount();
        if (oldSelectedRects.last().bottom() + safeOffset >= viewRowCount) {
            safeOffset = viewRowCount - oldSelectedRects.last().bottom() - 1;
        }
    }
    CHECK(safeOffset != 0, );
    editor->setCursorPosition(editor->getCursorPosition() + QPoint(0, safeOffset));
    int alignmentLength = editor->getAlignmentLen();
    QList<QRect> newSelectedRects;
    for (const QRect& oldRect : qAsConst(oldSelectedRects)) {
        QRect newRect = oldRect.translated(0, safeOffset);
        if (resetXRange) {
            newRect.setX(0);
            newRect.setWidth(alignmentLength);
        }
        newSelectedRects << newRect;
    }
    setSelection(newSelectedRects);
    scrollSelectionToView(safeOffset >= 0);
}

void MaEditorNameList::scrollSelectionToView(bool fromStart) {
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );
    QRect selectionRect = fromStart ? selection.getRectList().first() : selection.getRectList().last();
    int height = ui->getSequenceArea()->height();
    ui->getScrollController()->scrollToViewRow(fromStart ? selectionRect.top() : selectionRect.bottom(), height);
}

bool MaEditorNameList::triggerExpandCollapseOnSelectedRow(bool collapse) {
    const QList<QRect>& selectionRects = editor->getSelection().getRectList();
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    int minRowsInGroupToShowExpandCollapse = ui->isCollapsingOfSingleRowGroupsEnabled() ? 1 : 2;
    QList<int> groupsToToggle;
    for (const QRect& selectedRect : qAsConst(selectionRects)) {
        for (int viewRow = selectedRect.top(); viewRow <= selectedRect.bottom(); viewRow++) {
            int groupIndex = collapseModel->getCollapsibleGroupIndexByViewRowIndex(viewRow);
            const MaCollapsibleGroup* group = collapseModel->getCollapsibleGroup(groupIndex);
            if (group != nullptr && group->size() >= minRowsInGroupToShowExpandCollapse && group->isCollapsed != collapse) {
                groupsToToggle << groupIndex;
            }
        }
    }
    for (int groupIndex : qAsConst(groupsToToggle)) {
        collapseModel->toggleGroup(groupIndex, collapse);
    }
    return !groupsToToggle.isEmpty();
}

}  // namespace U2
