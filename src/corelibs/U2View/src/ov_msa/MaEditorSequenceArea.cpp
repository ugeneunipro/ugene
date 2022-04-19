/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "MaEditorSequenceArea.h"

#include <QApplication>
#include <QCursor>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/Settings.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GScrollBar.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OptionsPanel.h>

#include "MaEditorWgt.h"
#include "SequenceAreaRenderer.h"
#include "UndoRedoFramework.h"
#include "ov_msa/BaseWidthController.h"
#include "ov_msa/DrawHelper.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorNameList.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/RowHeightController.h"
#include "ov_msa/ScrollController.h"
#include "ov_msa/highlighting/MSAHighlightingTabFactory.h"
#include "ov_msa/highlighting/MsaSchemesMenuBuilder.h"

namespace U2 {

const QChar MaEditorSequenceArea::emDash = QChar(0x2015);

MaEditorSequenceArea::MaEditorSequenceArea(MaEditorWgt* ui, GScrollBar* hb, GScrollBar* vb)
    : editor(ui->getEditor()),
      ui(ui),
      shBar(hb),
      svBar(vb),
      editModeAnimationTimer(this),
      prevPressedButton(Qt::NoButton),
      changeTracker(editor->getMaObject()->getEntityRef()) {
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    // show rubber band for selection in MSA editor only
    showRubberBandOnSelection = qobject_cast<MSAEditor*>(editor) != nullptr;
    maMode = ViewMode;

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumSize(100, 100);
    selecting = false;
    shifting = false;
    editingEnabled = false;
    movableBorder = SelectionModificationHelper::NoMovableBorder;
    lengthOnMousePress = editor->getMaObject()->getLength();

    cachedView = new QPixmap();
    completeRedraw = true;

    useDotsAction = new QAction(QString(tr("Use dots")), this);
    useDotsAction->setCheckable(true);
    useDotsAction->setChecked(false);
    connect(useDotsAction, SIGNAL(triggered()), SLOT(sl_useDots()));

    replaceCharacterAction = new QAction(tr("Replace selected character"), this);
    replaceCharacterAction->setObjectName("replace_selected_character");
    replaceCharacterAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_R));
    replaceCharacterAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(replaceCharacterAction);
    connect(replaceCharacterAction, SIGNAL(triggered()), SLOT(sl_replaceSelectedCharacter()));

    insertGapsAction = new QAction(tr("Insert gaps"), this);
    insertGapsAction->setObjectName("insert_gaps");
    insertGapsAction->setShortcut(QKeySequence(Qt::Key_Space));
    connect(insertGapsAction, &QAction::triggered, this, &MaEditorSequenceArea::sl_insertGaps2SelectedArea);
    addAction(insertGapsAction);

    replaceWithGapsAction = new QAction(tr("Replace with gaps"), this);
    replaceWithGapsAction->setObjectName("replace_with_gaps");
    replaceWithGapsAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Space));
    connect(replaceWithGapsAction, &QAction::triggered, this, &MaEditorSequenceArea::sl_replaceSelectionWithGaps);
    addAction(replaceWithGapsAction);

    connect(editor, SIGNAL(si_completeUpdate()), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_updateActions()), SLOT(sl_updateActions()));
    connect(ui, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));
    connect(hb, SIGNAL(actionTriggered(int)), SLOT(sl_hScrollBarActionPerformed()));

    // SANGER_TODO: why is it commented?
    //    connect(editor, SIGNAL(si_fontChanged(QFont)), SLOT(sl_fontChanged(QFont)));

    connect(&editModeAnimationTimer, SIGNAL(timeout()), SLOT(sl_changeSelectionColor()));

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), SLOT(sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)));

    connect(this, SIGNAL(si_startMaChanging()), editor->getUndoRedoFramework(), SLOT(sl_updateUndoRedoState()));
    connect(this, SIGNAL(si_stopMaChanging(bool)), editor->getUndoRedoFramework(), SLOT(sl_updateUndoRedoState()));
    connect(editor->getSelectionController(),
            SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)),
            SLOT(sl_onSelectionChanged(const MaEditorSelection&, const MaEditorSelection&)));
}

MaEditorSequenceArea::~MaEditorSequenceArea() {
    editModeAnimationTimer.stop();
    delete cachedView;
    qDeleteAll(customColorSchemeMenuActions);
    delete highlightingScheme;
}

MaEditor* MaEditorSequenceArea::getEditor() const {
    return editor;
}

QSize MaEditorSequenceArea::getCanvasSize(const QList<int>& seqIdx, const U2Region& region) const {
    return QSize(ui->getBaseWidthController()->getBasesWidth(region),
                 ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(seqIdx));
}

int MaEditorSequenceArea::getFirstVisibleBase() const {
    return ui->getScrollController()->getFirstVisibleBase();
}

void MaEditorSequenceArea::setFirstVisibleBase(int firstVisibleBase) {
    ui->getScrollController()->setFirstVisibleBase(firstVisibleBase);
}

int MaEditorSequenceArea::getLastVisibleBase(bool countClipped) const {
    return ui->getScrollController()->getLastVisibleBase(width(), countClipped);
}

int MaEditorSequenceArea::getNumVisibleBases() const {
    return ui->getDrawHelper()->getVisibleBasesCount(width());
}

int MaEditorSequenceArea::getViewRowCount() const {
    return editor->getCollapseModel()->getViewRowCount();
}

int MaEditorSequenceArea::getRowIndex(const int num) const {
    CHECK(!isAlignmentEmpty(), -1);
    MaCollapseModel* model = editor->getCollapseModel();
    return model->getMaRowIndexByViewRowIndex(num);
}

bool MaEditorSequenceArea::isAlignmentEmpty() const {
    return editor->isAlignmentEmpty();
}

bool MaEditorSequenceArea::isPosInRange(int position) const {
    return position >= 0 && position < editor->getAlignmentLen();
}

bool MaEditorSequenceArea::isSeqInRange(int rowNumber) const {
    return rowNumber >= 0 && rowNumber < getViewRowCount();
}

bool MaEditorSequenceArea::isInRange(const QPoint& point) const {
    return isPosInRange(point.x()) && isSeqInRange(point.y());
}

bool MaEditorSequenceArea::isInRange(const QRect& rect) const {
    return isSeqInRange(rect.y()) && isSeqInRange(rect.bottom()) && isPosInRange(rect.x()) && isPosInRange(rect.right());
}

QPoint MaEditorSequenceArea::boundWithVisibleRange(const QPoint& point) const {
    return QPoint(
        qBound(0, point.x(), editor->getAlignmentLen() - 1),
        qBound(0, point.y(), editor->getCollapseModel()->getViewRowCount() - 1));
}

QRect MaEditorSequenceArea::boundWithVisibleRange(const QRect& rect) const {
    QRect visibleRect(0, 0, editor->getAlignmentLen(), editor->getCollapseModel()->getViewRowCount());
    return rect.intersected(visibleRect);
}

bool MaEditorSequenceArea::isVisible(const QPoint& p, bool countClipped) const {
    return isPositionVisible(p.x(), countClipped) && isRowVisible(p.y(), countClipped);
}

bool MaEditorSequenceArea::isPositionVisible(int position, bool countClipped) const {
    return ui->getDrawHelper()->getVisibleBases(width(), countClipped, countClipped).contains(position);
}

bool MaEditorSequenceArea::isRowVisible(int rowNumber, bool countClipped) const {
    int rowIndex = editor->getCollapseModel()->getMaRowIndexByViewRowIndex(rowNumber);
    return ui->getDrawHelper()->getVisibleMaRowIndexes(height(), countClipped, countClipped).contains(rowIndex);
}

QFont MaEditorSequenceArea::getFont() const {
    return editor->getFont();
}

void MaEditorSequenceArea::setSelectionRect(const QRect& newSelectionRect) {
    QRect safeRect = boundWithVisibleRange(newSelectionRect);
    if (!safeRect.isValid()) {  // 'newSelectionRect' is out of bounds - reset selection to empty.
        editor->getSelectionController()->clearSelection();
        return;
    }
    editor->getSelectionController()->setSelection(MaEditorSelection({safeRect}));
}

void MaEditorSequenceArea::sl_onSelectionChanged(const MaEditorSelection&, const MaEditorSelection&) {
    exitFromEditCharacterMode();
    QList<int> selectedMaRowsIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    MultipleAlignmentObject* maObject = editor->getMaObject();
    QStringList selectedRowNames;
    for (int maRow : qAsConst(selectedMaRowsIndexes)) {
        selectedRowNames.append(maObject->getRow(maRow)->getName());
    }
    emit si_selectionChanged(selectedRowNames);
    update();

    // TODO: the code below can be moved to the sl_updateActions().
    bool isReadOnly = maObject->isStateLocked();
    bool hasSelection = !selectedMaRowsIndexes.isEmpty();
    ui->copySelectionAction->setEnabled(hasSelection);
    ui->copyFormattedSelectionAction->setEnabled(hasSelection);
    emit si_copyFormattedChanging(hasSelection);
    ui->cutSelectionAction->setEnabled(hasSelection && !isReadOnly);

    sl_updateActions();
    sl_completeRedraw();
}

void MaEditorSequenceArea::moveSelection(int dx, int dy, bool allowSelectionResize) {
    QRect newSelectionRect = editor->getSelection().toRect().translated(dx, dy);

    if (!isInRange(newSelectionRect)) {
        if (!allowSelectionResize) {
            return;
        }
        setSelectionRect(newSelectionRect);
        return;
    }

    editor->setCursorPosition(editor->getCursorPosition() + QPoint(dx, dy));
    setSelectionRect(newSelectionRect);
    ui->getScrollController()->scrollToMovedSelection(dx, dy);
}

int MaEditorSequenceArea::getTopSelectedMaRow() const {
    const MaEditorSelection& selection = editor->getSelection();
    if (selection.isEmpty()) {
        return -1;
    }
    int firstSelectedViewRow = selection.toRect().y();
    return editor->getCollapseModel()->getMaRowIndexByViewRowIndex(firstSelectedViewRow);
}

QString MaEditorSequenceArea::getCopyFormattedAlgorithmId() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + SETTINGS_COPY_FORMATTED, BaseDocumentFormats::CLUSTAL_ALN).toString();
}

void MaEditorSequenceArea::deleteCurrentSelection() {
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );

    MultipleAlignmentObject* maObj = getEditor()->getMaObject();
    CHECK(!maObj->isStateLocked(), );

    SAFE_POINT(isInRange(selection.toRect()), "Selection is not in range!", );

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    int numColumns = editor->getAlignmentLen();
    QRect selectionRect = selection.toRect();
    bool isWholeRowRemoved = selectionRect.width() == numColumns;

    if (isWholeRowRemoved) {  // Reuse code of the name list.
        ui->getEditorNameList()->sl_removeSelectedRows();
        return;
    }

    Q_ASSERT(isInRange(QPoint(selectionRect.x() + selectionRect.width() - 1, selectionRect.y() + selectionRect.height() - 1)));

    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    int numRows = (int)maObj->getRowCount();
    if (selectedMaRowIndexes.size() == numRows) {
        bool isResultAlignmentEmpty = true;
        U2Region xRegion = U2Region::fromXRange(selectionRect);
        for (int i = 0; i < selectedMaRowIndexes.size() && isResultAlignmentEmpty; i++) {
            int maRow = selectedMaRowIndexes[i];
            isResultAlignmentEmpty = maObj->isRegionEmpty(0, maRow, xRegion.startPos, 1) &&
                                     maObj->isRegionEmpty(xRegion.endPos(), maRow, numColumns - xRegion.endPos(), 1);
        }
        if (isResultAlignmentEmpty) {
            return;
        }
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );
    maObj->removeRegion(selectedMaRowIndexes, selectionRect.x(), selectionRect.width(), true);
    GCounter::increment("Delete current selection", editor->getFactoryId());
}

bool MaEditorSequenceArea::shiftSelectedRegion(int shift) {
    CHECK(shift != 0, true);

    // shifting of selection
    MultipleAlignmentObject* maObj = editor->getMaObject();
    if (maObj->isStateLocked()) {
        return false;
    }
    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    const MaEditorSelection& selection = editor->getSelection();
    QRect selectionRectBefore = selection.toRect();
    if (maObj->isRegionEmpty(selectedMaRowIndexes, selectionRectBefore.x(), selectionRectBefore.width())) {
        return true;
    }
    int resultShift = shiftRegion(shift);
    if (resultShift == 0) {
        return false;
    }
    U2OpStatus2Log os;
    adjustReferenceLength(os);

    const QPoint& cursorPos = editor->getCursorPosition();
    int newCursorPosX = (cursorPos.x() + resultShift >= 0) ? cursorPos.x() + resultShift : 0;
    editor->setCursorPosition(QPoint(newCursorPosX, cursorPos.y()));

    setSelectionRect(QRect(selectionRectBefore.x() + resultShift, selectionRectBefore.y(), selectionRectBefore.width(), selectionRectBefore.height()));
    QRect selectionRectAfter = selection.toRect();
    qint64 scrollPos = resultShift > 0 ? selectionRectAfter.right() : selectionRectAfter.left();
    ui->getScrollController()->scrollToBase((int)scrollPos, width());
    return true;
}

int MaEditorSequenceArea::shiftRegion(int shift) {
    MultipleAlignmentObject* maObj = editor->getMaObject();
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), 0);

    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    const int selectionWidth = selection.getWidth();
    const int height = selectedMaRowIndexes.size();
    const int y = selection.getRectList()[0].y();
    int x = selection.toRect().x();
    int resultShift = 0;
    bool isCtrlPressed = QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
    if (isCtrlPressed) {
        if (shift > 0) {
            QVector<U2MsaGap> gapModelToRemove = findRemovableGapColumns(shift);
            if (!gapModelToRemove.isEmpty()) {
                foreach (U2MsaGap gap, gapModelToRemove) {
                    QRect currentSelectionRect = selection.toRect();
                    x = currentSelectionRect.x();
                    U2OpStatus2Log os;
                    const int length = maObj->getLength();
                    if (length != gap.startPos) {
                        maObj->deleteGapByRowIndexList(os, selectedMaRowIndexes, gap.startPos, gap.length);
                    }
                    CHECK_OP(os, resultShift);
                    resultShift += maObj->shiftRegion(x, y, selectionWidth, height, gap.length);
                    QRect newSelectionRect(gap.length + x, currentSelectionRect.y(), selectionWidth, height);
                    setSelectionRect(newSelectionRect);
                }
            }
        } else if (shift < 0 && !ctrlModeGapModel.isEmpty()) {
            QVector<U2MsaGap> gapModelToRestore = findRestorableGapColumns(shift);
            if (!gapModelToRestore.isEmpty()) {
                resultShift = maObj->shiftRegion(x, y, selectionWidth, height, shift);
                foreach (U2MsaGap gap, gapModelToRestore) {
                    if (gap.endPos() < lengthOnMousePress) {
                        maObj->insertGapByRowIndexList(selectedMaRowIndexes, gap.startPos, gap.length);
                    } else if (gap.startPos >= lengthOnMousePress) {
                        U2OpStatus2Log os;
                        U2Region allRows(0, maObj->getRowCount());
                        maObj->deleteGap(os, allRows, maObj->getLength() - gap.length, gap.length);
                        CHECK_OP(os, resultShift);
                    }
                }
            }
        }
    } else {
        resultShift = maObj->shiftRegion(x, y, selectionWidth, height, shift);
    }

    return resultShift;
}

QVector<U2MsaGap> MaEditorSequenceArea::findRemovableGapColumns(int& shift) {
    CHECK(shift > 0, QVector<U2MsaGap>());

    int numOfRemovableColumns = 0;
    QVector<U2MsaGap> commonGapColumns = findCommonGapColumns(numOfRemovableColumns);
    if (numOfRemovableColumns < shift) {
        int count = shift - numOfRemovableColumns;
        commonGapColumns << addTrailingGapColumns(count);
    }

    QVector<U2MsaGap> gapColumnsToRemove;
    int count = shift;
    foreach (U2MsaGap gap, commonGapColumns) {
        if (count >= gap.length) {
            gapColumnsToRemove.append(gap);
            count -= gap.length;
            if (count == 0) {
                break;
            }
        } else {
            gapColumnsToRemove.append(U2MsaGap(gap.startPos, count));
            break;
        }
    }

    ctrlModeGapModel << gapColumnsToRemove;

    if (count < shift) {
        shift -= count;
    }
    return gapColumnsToRemove;
}

QVector<U2MsaGap> MaEditorSequenceArea::findCommonGapColumns(int& numOfColumns) {
    numOfColumns = 0;
    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    CHECK(!selectedMaRowIndexes.isEmpty(), {});

    U2Region columnRegion = editor->getSelection().getColumnRegion();
    QList<QVector<U2MsaGap>> listGapModel = editor->getMaObject()->getGapModel();

    QVector<U2MsaGap> gapModelToUpdate;
    const QVector<U2MsaGap>& firstRowGapList = listGapModel[selectedMaRowIndexes[0]];
    for (const U2MsaGap& gap : qAsConst(firstRowGapList)) {
        if (gap.startPos + gap.length <= columnRegion.endPos()) {
            continue;
        }
        if (gap.startPos < columnRegion.endPos() && gap.startPos + gap.length > columnRegion.endPos()) {
            qint64 startPos = columnRegion.endPos();
            U2MsaGap trailingGap(startPos, gap.startPos + gap.length - startPos);
            gapModelToUpdate << trailingGap;
        } else {
            gapModelToUpdate << gap;
        }
    }

    for (int i = 1; i < selectedMaRowIndexes.size(); i++) {
        int maRowIndex = selectedMaRowIndexes[i];
        QVector<U2MsaGap> currentGapModelToRemove;
        qint64 currentNumOfColumns = 0;
        const QVector<U2MsaGap>& rowGrapList = listGapModel[maRowIndex];
        for (const U2MsaGap& gap : qAsConst(rowGrapList)) {
            for (const U2MsaGap& gapToRemove : qAsConst(gapModelToUpdate)) {
                U2MsaGap intersectedGap = gap.intersect(gapToRemove);
                if (intersectedGap.length == 0) {
                    continue;
                }
                currentNumOfColumns += intersectedGap.length;
                currentGapModelToRemove << intersectedGap;
            }
        }
        gapModelToUpdate = currentGapModelToRemove;
        numOfColumns = currentNumOfColumns;
    }

    return gapModelToUpdate;
}

U2MsaGap MaEditorSequenceArea::addTrailingGapColumns(int count) {
    MultipleAlignmentObject* maObj = editor->getMaObject();
    qint64 length = maObj->getLength();
    return U2MsaGap(length, count);
}

QVector<U2MsaGap> MaEditorSequenceArea::findRestorableGapColumns(const int shift) {
    CHECK(shift < 0, QVector<U2MsaGap>());
    CHECK(!ctrlModeGapModel.isEmpty(), QVector<U2MsaGap>());

    QVector<U2MsaGap> gapColumnsToRestore;
    int absShift = qAbs(shift);
    const int size = ctrlModeGapModel.size();
    for (int i = size - 1; i >= 0; i--) {
        if (ctrlModeGapModel[i].length >= absShift) {
            const int offset = ctrlModeGapModel[i].length - absShift;
            U2MsaGap gapToRestore(ctrlModeGapModel[i].startPos + offset, absShift);
            gapColumnsToRestore.push_front(gapToRestore);
            ctrlModeGapModel[i].length -= absShift;
            if (ctrlModeGapModel[i].length == 0) {
                ctrlModeGapModel.removeOne(ctrlModeGapModel[i]);
            }
            break;
        } else {
            gapColumnsToRestore.push_front(ctrlModeGapModel[i]);
            absShift -= ctrlModeGapModel[i].length;
            ctrlModeGapModel.removeOne(ctrlModeGapModel[i]);
        }
    }

    return gapColumnsToRestore;
}

void MaEditorSequenceArea::centerPos(const QPoint& point) {
    SAFE_POINT(isInRange(point), QString("Point (%1, %2) is out of range").arg(point.x()).arg(point.y()), );
    ui->getScrollController()->centerPoint(point, size());
    update();
}

void MaEditorSequenceArea::centerPos(int position) {
    SAFE_POINT(isPosInRange(position), QString("Base %1 is out of range").arg(position), );
    ui->getScrollController()->centerBase(position, width());
    update();
}

bool MaEditorSequenceArea::isPositionCentered(int position) const {
    SAFE_POINT(isPosInRange(position), QString("Base %1 is out of range").arg(position), false);
    return ui->getScrollController()->isBaseCentered(position, width());
}

void MaEditorSequenceArea::onVisibleRangeChanged() {
    exitFromEditCharacterMode();
    CHECK(!isAlignmentEmpty(), );

    const QStringList rowsNames = editor->getMaObject()->getMultipleAlignment()->getRowNames();
    QStringList visibleRowsNames;

    const QList<int> visibleRows = ui->getDrawHelper()->getVisibleMaRowIndexes(height());
    foreach (const int rowIndex, visibleRows) {
        SAFE_POINT(rowIndex < rowsNames.size(), QString("Row index is out of rowsNames boundaries: index is %1, size is %2").arg(rowIndex).arg(rowsNames.size()), );
        visibleRowsNames << rowsNames[rowIndex];
    }

    const int rowsHeight = ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(visibleRows);

    emit si_visibleRangeChanged(visibleRowsNames, rowsHeight);
}

bool MaEditorSequenceArea::isAlignmentLocked() const {
    MultipleAlignmentObject* obj = editor->getMaObject();
    SAFE_POINT(obj != nullptr, tr("Alignment object is not available"), true);
    return obj->isStateLocked();
}

void MaEditorSequenceArea::drawVisibleContent(QPainter& painter) {
    U2Region columns = ui->getDrawHelper()->getVisibleBases(width());
    QList<int> maRows = ui->getDrawHelper()->getVisibleMaRowIndexes(height());
    CHECK(!columns.isEmpty() || !maRows.isEmpty(), );
    int xStart = ui->getBaseWidthController()->getBaseScreenRange(columns.startPos).startPos;
    int yStart = ui->getRowHeightController()->getScreenYPositionOfTheFirstVisibleRow(COUNT_FIRST_CLIPPED_ROW);
    drawContent(painter, columns, maRows, xStart, yStart);
}

bool MaEditorSequenceArea::drawContent(QPainter& painter, const U2Region& columns, const QList<int>& maRows, int xStart, int yStart) {
    // SANGER_TODO: optimize
    return renderer->drawContent(painter, columns, maRows, xStart, yStart);
}

MsaColorScheme* MaEditorSequenceArea::getCurrentColorScheme() const {
    return colorScheme;
}

MsaHighlightingScheme* MaEditorSequenceArea::getCurrentHighlightingScheme() const {
    return highlightingScheme;
}

bool MaEditorSequenceArea::getUseDotsCheckedState() const {
    return useDotsAction->isChecked();
}

QAction* MaEditorSequenceArea::getReplaceCharacterAction() const {
    return replaceCharacterAction;
}

void MaEditorSequenceArea::sl_changeColorSchemeOutside(const QString& id) {
    QAction* a = GUIUtils::findActionByData(QList<QAction*>() << colorSchemeMenuActions << customColorSchemeMenuActions << highlightingSchemeMenuActions, id);
    if (a != nullptr) {
        a->trigger();
    }
}

void MaEditorSequenceArea::sl_changeCopyFormat(const QString& formatId) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_COPY_FORMATTED, formatId);
}

void MaEditorSequenceArea::sl_changeColorScheme() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action == nullptr) {
        action = GUIUtils::getCheckedAction(customColorSchemeMenuActions);
    }
    CHECK(action != nullptr, );

    applyColorScheme(action->data().toString());
}

void MaEditorSequenceArea::sl_delCurrentSelection() {
    emit si_startMaChanging();
    deleteCurrentSelection();
    emit si_stopMaChanging(true);
}

void MaEditorSequenceArea::sl_insertGaps2SelectedArea() {
    GCounter::increment("Insert gaps", editor->getFactoryId());
    CHECK(!isAlignmentLocked(), );

    emit si_startMaChanging();
    insertGapsBeforeSelection();
    emit si_stopMaChanging(true);
}

void U2::MaEditorSequenceArea::sl_replaceSelectionWithGaps() {
    GCounter::increment("Replace with gaps", editor->getFactoryId());
    CHECK(!isAlignmentLocked(), );

    emit si_startMaChanging();
    insertGapsBeforeSelection(-1, false);
    emit si_stopMaChanging(true);
}

void MaEditorSequenceArea::sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo& modInfo) {
    exitFromEditCharacterMode();
    updateCollapseModel(modInfo);
    ui->getScrollController()->sl_updateScrollBars();

    int columnCount = editor->getAlignmentLen();
    int rowCount = getViewRowCount();

    // Fix cursor position if it is out of range.
    QPoint cursorPosition = editor->getCursorPosition();
    QPoint fixedCursorPosition(qMin(cursorPosition.x(), columnCount - 1), qMin(cursorPosition.y(), rowCount - 1));
    if (cursorPosition != fixedCursorPosition) {
        editor->setCursorPosition(fixedCursorPosition);
    }

    editor->updateReference();
    sl_completeUpdate();
}

void MaEditorSequenceArea::sl_completeUpdate() {
    completeRedraw = true;
    sl_updateActions();
    update();
    onVisibleRangeChanged();
}

void MaEditorSequenceArea::sl_completeRedraw() {
    completeRedraw = true;
    update();
}

void MaEditorSequenceArea::sl_triggerUseDots() {
    useDotsAction->trigger();
}

void MaEditorSequenceArea::sl_useDots() {
    completeRedraw = true;
    update();
    emit si_highlightingChanged();
}

void MaEditorSequenceArea::sl_registerCustomColorSchemes() {
    qDeleteAll(customColorSchemeMenuActions);
    customColorSchemeMenuActions.clear();

    MsaSchemesMenuBuilder::createAndFillColorSchemeMenuActions(customColorSchemeMenuActions,
                                                               MsaSchemesMenuBuilder::Custom,
                                                               getEditor()->getMaObject()->getAlphabet()->getType(),
                                                               this);
}

void MaEditorSequenceArea::sl_colorSchemeFactoryUpdated() {
    applyColorScheme(colorScheme->getFactory()->getId());
}

void MaEditorSequenceArea::sl_setDefaultColorScheme() {
    MsaColorSchemeFactory* defaultFactory = getDefaultColorSchemeFactory();
    SAFE_POINT(defaultFactory != nullptr, L10N::nullPointerError("default color scheme factory"), );
    applyColorScheme(defaultFactory->getId());
}

void MaEditorSequenceArea::sl_changeHighlightScheme() {
    QAction* a = qobject_cast<QAction*>(sender());
    if (a == nullptr) {
        a = GUIUtils::getCheckedAction(customColorSchemeMenuActions);
    }
    CHECK(a != nullptr, );

    editor->saveHighlightingSettings(highlightingScheme->getFactory()->getId(), highlightingScheme->getSettings());

    QString id = a->data().toString();
    MsaHighlightingSchemeFactory* factory = AppContext::getMsaHighlightingSchemeRegistry()->getSchemeFactoryById(id);
    SAFE_POINT(factory != nullptr, L10N::nullPointerError("highlighting scheme"), );
    if (ui->getEditor()->getMaObject() == nullptr) {
        return;
    }

    delete highlightingScheme;
    highlightingScheme = factory->create(this, ui->getEditor()->getMaObject());
    highlightingScheme->applySettings(editor->getHighlightingSettings(id));

    const MultipleAlignment ma = ui->getEditor()->getMaObject()->getMultipleAlignment();

    U2OpStatusImpl os;
    const int refSeq = ma->getRowIndexByRowId(editor->getReferenceRowId(), os);

    MSAHighlightingFactory msaHighlightingFactory;
    QString msaHighlightingId = msaHighlightingFactory.getOPGroupParameters().getGroupId();

    CHECK(ui->getEditor(), );
    CHECK(ui->getEditor()->getOptionsPanel(), );

    if (!factory->isRefFree() && refSeq == -1 && ui->getEditor()->getOptionsPanel()->getActiveGroupId() != msaHighlightingId) {
        QMessageBox::warning(ui, tr("No reference sequence selected"), tr("Reference sequence for current highlighting scheme is not selected. Use context menu or Highlighting tab on Options panel to select it"));
    }

    foreach (QAction* action, highlightingSchemeMenuActions) {
        action->setChecked(action == a);
    }
    if (factory->isAlphabetTypeSupported(DNAAlphabet_RAW)) {
        AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_RAW, id);
    }
    if (factory->isAlphabetTypeSupported(DNAAlphabet_NUCL)) {
        AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_NUCL, id);
    }
    if (factory->isAlphabetTypeSupported(DNAAlphabet_AMINO)) {
        AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_AMINO, id);
    }
    if (factory->isAlphabetTypeSupported(DNAAlphabet_UNDEFINED)) {
        FAIL(tr("Unknown alphabet"), );
    }

    completeRedraw = true;
    update();
    emit si_highlightingChanged();
}

void MaEditorSequenceArea::sl_replaceSelectedCharacter() {
    maMode = ReplaceCharMode;
    editModeAnimationTimer.start(500);
    sl_updateActions();
}

void MaEditorSequenceArea::sl_changeSelectionColor() {
    QColor black(Qt::black);
    selectionColor = (black == selectionColor) ? Qt::darkGray : Qt::black;
    update();
}

/** Returns longest region of indexes from adjacent groups. */
U2Region findLongestRegion(const QList<int>& sortedViewIndexes) {
    U2Region longestRegion;
    U2Region currentRegion;
    foreach (int viewIndex, sortedViewIndexes) {
        if (currentRegion.endPos() == viewIndex) {
            currentRegion.length++;
        } else {
            currentRegion.startPos = viewIndex;
            currentRegion.length = 1;
        }
        if (currentRegion.length > longestRegion.length) {
            longestRegion = currentRegion;
        }
    }
    return longestRegion;
}

void MaEditorSequenceArea::sl_modelChanged() {
    ui->getScrollController()->updateVerticalScrollBar();
    sl_completeRedraw();
}

void MaEditorSequenceArea::sl_hScrollBarActionPerformed() {
    const QAbstractSlider::SliderAction action = shBar->getRepeatAction();
    CHECK(QAbstractSlider::SliderSingleStepAdd == action || QAbstractSlider::SliderSingleStepSub == action, );

    if (shifting && editingEnabled) {
        const QPoint localPoint = mapFromGlobal(QCursor::pos());
        const QPoint newCurPos = ui->getScrollController()->getViewPosByScreenPoint(localPoint);

        const QPoint& cursorPos = editor->getCursorPosition();
        shiftSelectedRegion(newCurPos.x() - cursorPos.x());
    }
}

void MaEditorSequenceArea::resizeEvent(QResizeEvent* e) {
    completeRedraw = true;
    ui->getScrollController()->sl_updateScrollBars();
    emit si_visibleRangeChanged();
    QWidget::resizeEvent(e);
}

void MaEditorSequenceArea::paintEvent(QPaintEvent* e) {
    drawAll();
    QWidget::paintEvent(e);
}

void MaEditorSequenceArea::wheelEvent(QWheelEvent* we) {
    bool toMin = we->delta() > 0;
    if (we->modifiers() == 0) {
        shBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    } else if (we->modifiers() & Qt::SHIFT) {
        svBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    }
    QWidget::wheelEvent(we);
}

void MaEditorSequenceArea::mousePressEvent(QMouseEvent* e) {
    prevPressedButton = e->button();

    if (!hasFocus()) {
        setFocus();
    }

    mousePressEventPoint = e->pos();
    mousePressViewPos = ui->getScrollController()->getViewPosByScreenPoint(mousePressEventPoint);

    if (e->button() == Qt::LeftButton) {
        if (e->modifiers() == Qt::ShiftModifier) {
            QWidget::mousePressEvent(e);
            return;
        }

        lengthOnMousePress = editor->getMaObject()->getLength();
        editor->setCursorPosition(boundWithVisibleRange(mousePressViewPos));

        Qt::CursorShape shape = cursor().shape();
        if (shape != Qt::ArrowCursor) {
            QPoint pos = e->pos();
            changeTracker.finishTracking();
            QPoint globalMousePosition = ui->getScrollController()->getGlobalMousePosition(pos);
            double baseWidth = ui->getBaseWidthController()->getBaseWidth();
            double baseHeight = ui->getRowHeightController()->getSingleRowHeight();
            const MaEditorSelection& selection = editor->getSelection();
            movableBorder = SelectionModificationHelper::NoMovableBorder;
            // Selection resizing is supported only in single selection mode.
            if (selection.isSingleRegionSelection()) {
                const QRect& selectionRect = selection.getRectList().first();
                movableBorder = SelectionModificationHelper::getMovableSide(shape, globalMousePosition, selectionRect, QSize(baseWidth, baseHeight));
                moveBorder(pos);
            }
        }
    }

    QWidget::mousePressEvent(e);
}

void MaEditorSequenceArea::mouseReleaseEvent(QMouseEvent* event) {
    rubberBand->hide();
    QPoint releasePos = ui->getScrollController()->getViewPosByScreenPoint(event->pos());
    bool isClick = !selecting && releasePos == mousePressViewPos;
    bool isSelectionResize = movableBorder != SelectionModificationHelper::NoMovableBorder;
    bool isShiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);
    bool isCtrlPressed = event->modifiers().testFlag(Qt::ControlModifier);
    if (shifting) {
        changeTracker.finishTracking();
        editor->getMaObject()->releaseState();
        emit si_stopMaChanging(maVersionBeforeShifting != editor->getMaObject()->getModificationVersion());
    } else if (isSelectionResize) {
        // Do nothing. selection was already updated on mouse move.
    } else if (selecting || isShiftPressed) {
        QPoint startPos = selecting ? mousePressViewPos : editor->getCursorPosition();
        int width = qAbs(releasePos.x() - startPos.x()) + 1;
        int height = qAbs(releasePos.y() - startPos.y()) + 1;
        int left = qMin(releasePos.x(), startPos.x());
        int top = qMin(releasePos.y(), startPos.y());
        ui->getScrollController()->scrollToPoint(releasePos, size());
        QRect dragRect = QRect(left, top, width, height);
        setSelectionRect(dragRect);
    } else if (isClick && event->button() == Qt::LeftButton) {
        QRect clickPosAsRect = QRect(releasePos.x(), releasePos.y(), 1, 1);
        setSelectionRect(clickPosAsRect);
    }
    shifting = false;
    selecting = false;
    maVersionBeforeShifting = -1;
    movableBorder = SelectionModificationHelper::NoMovableBorder;

    if (ctrlModeGapModel.isEmpty() && isCtrlPressed) {
        MultipleAlignmentObject* maObj = editor->getMaObject();
        maObj->si_completeStateChanged(true);
        MaModificationInfo mi;
        mi.alignmentLengthChanged = false;
        maObj->si_alignmentChanged(maObj->getMultipleAlignment(), mi);
    }
    ctrlModeGapModel.clear();

    ui->getScrollController()->stopSmoothScrolling();

    QWidget::mouseReleaseEvent(event);
}

void MaEditorSequenceArea::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() != Qt::LeftButton) {
        setBorderCursor(event->pos());
        QWidget::mouseMoveEvent(event);
        return;
    }
    bool isSelectionResize = movableBorder != SelectionModificationHelper::NoMovableBorder;
    QPoint mouseMoveEventPoint = event->pos();
    ScrollController* scrollController = ui->getScrollController();
    QPoint mouseMoveViewPos = ui->getScrollController()->getViewPosByScreenPoint(mouseMoveEventPoint);

    bool isDefaultCursorMode = cursor().shape() == Qt::ArrowCursor;
    const MaEditorSelection& selection = editor->getSelection();
    if (!shifting && selection.isSingleRegionSelection() && selection.contains(mousePressViewPos) && !isAlignmentLocked() && editingEnabled && isDefaultCursorMode) {
        shifting = true;
        maVersionBeforeShifting = editor->getMaObject()->getModificationVersion();
        U2OpStatus2Log os;
        changeTracker.startTracking(os);
        CHECK_OP(os, );
        editor->getMaObject()->saveState();
        emit si_startMaChanging();
    }

    if (isInRange(mouseMoveViewPos)) {
        selecting = !shifting && !isSelectionResize;
        if (selecting && showRubberBandOnSelection && !rubberBand->isVisible()) {
            rubberBand->setGeometry(QRect(mousePressEventPoint, QSize()));
            rubberBand->show();
        }
        if (isVisible(mouseMoveViewPos, false)) {
            scrollController->stopSmoothScrolling();
        } else {
            ScrollController::Directions direction = ScrollController::None;
            if (mouseMoveViewPos.x() < scrollController->getFirstVisibleBase(false)) {
                direction |= ScrollController::Left;
            } else if (mouseMoveViewPos.x() > scrollController->getLastVisibleBase(width(), false)) {
                direction |= ScrollController::Right;
            }

            if (mouseMoveViewPos.y() < scrollController->getFirstVisibleViewRowIndex(false)) {
                direction |= ScrollController::Up;
            } else if (mouseMoveViewPos.y() > scrollController->getLastVisibleViewRowIndex(height(), false)) {
                direction |= ScrollController::Down;
            }
            scrollController->scrollSmoothly(direction);
        }
    }

    if (isSelectionResize) {
        moveBorder(mouseMoveEventPoint);
    } else if (shifting && editingEnabled) {
        shiftSelectedRegion(mouseMoveViewPos.x() - editor->getCursorPosition().x());
    } else if (selecting && showRubberBandOnSelection) {
        rubberBand->setGeometry(QRect(mousePressEventPoint, mouseMoveEventPoint).normalized());
        rubberBand->show();
    }
    QWidget::mouseMoveEvent(event);
}

void MaEditorSequenceArea::setBorderCursor(const QPoint& p) {
    QPoint globalMousePos = ui->getScrollController()->getGlobalMousePosition(p);
    const MaEditorSelection& selection = editor->getSelection();
    int viewWidth = ui->getBaseWidthController()->getBaseWidth();
    int viewHeight = ui->getRowHeightController()->getSingleRowHeight();
    // Only single selection support resizing.
    CHECK(selection.isSingleRegionSelection(), );
    QRect selectionRect = selection.getRectList().first();
    Qt::CursorShape shape = SelectionModificationHelper::getCursorShape(globalMousePos, selectionRect, viewWidth, viewHeight);
    setCursor(shape);
}

void MaEditorSequenceArea::moveBorder(const QPoint& screenMousePos) {
    CHECK(movableBorder != SelectionModificationHelper::NoMovableBorder, );

    QPoint globalMousePos = ui->getScrollController()->getGlobalMousePosition(screenMousePos);
    globalMousePos = QPoint(qMax(0, globalMousePos.x()), qMax(0, globalMousePos.y()));
    const MaEditorSelection& selection = editor->getSelection();
    SAFE_POINT(selection.isSingleRegionSelection(), "Only single selection can be resized!", );
    const QRect& selectionRect = selection.getRectList().first();
    QSizeF viewSize(ui->getBaseWidthController()->getBaseWidth(), ui->getRowHeightController()->getSingleRowHeight());
    QRect newSelectionRect = SelectionModificationHelper::getNewSelection(movableBorder, globalMousePos, viewSize, selectionRect);
    newSelectionRect = boundWithVisibleRange(newSelectionRect);

    setCursor(SelectionModificationHelper::getCursorShape(movableBorder, cursor().shape()));

    CHECK(!newSelectionRect.isEmpty(), );
    setSelectionRect(newSelectionRect);
}

void MaEditorSequenceArea::keyPressEvent(QKeyEvent* e) {
    if (!hasFocus()) {
        setFocus();
    }

    int key = e->key();
    if (maMode != ViewMode) {
        processCharacterInEditMode(e);
        return;
    }

    bool isMsaEditor = qobject_cast<MSAEditor*>(getEditor()) != nullptr;
    bool isShiftPressed = e->modifiers().testFlag(Qt::ShiftModifier);
    bool isCtrlPressed = e->modifiers().testFlag(Qt::ControlModifier);
#ifdef Q_OS_DARWIN
    // In one case it is better to use a Command key as modifier,
    // in another - a Control key. genuineCtrl - Control key on Mac OS X.
    bool isGenuineCtrlPressed = e->modifiers().testFlag(Qt::MetaModifier);
#else
    bool isGenuineCtrlPressed = e->modifiers().testFlag(Qt::ControlModifier);
#endif

    // Arrow keys with control work as page-up/page-down.
    if (isCtrlPressed && (key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up || key == Qt::Key_Down)) {
        key = (key == Qt::Key_Up || key == Qt::Key_Left) ? Qt::Key_PageUp : Qt::Key_PageDown;
    }
    QPoint cursorPosition = editor->getCursorPosition();
    const MaEditorSelection& selection = editor->getSelection();
    // Use cursor position for empty selection when arrow keys are used.
    QRect selectionRect = selection.isEmpty() ? QRect(cursorPosition, cursorPosition) : selection.toRect();
    switch (key) {
        case Qt::Key_Escape:
            editor->getSelectionController()->clearSelection();
            break;
        case Qt::Key_Left:
            if (!isShiftPressed || !isMsaEditor) {
                moveSelection(-1, 0);
            } else {
                bool isMoveRightSide = cursorPosition.x() == selectionRect.x() && selectionRect.width() > 1;
                if (isMoveRightSide) {  // Move the right side (shrink).
                    setSelectionRect(QRect(selectionRect.topLeft(), selectionRect.bottomRight() + QPoint(-1, 0)));
                    editor->setCursorPosition(QPoint(selectionRect.left(), cursorPosition.y()));
                } else {  // Move the left side (grow).
                    setSelectionRect(QRect(selectionRect.topLeft() + QPoint(-1, 0), selectionRect.bottomRight()));
                    editor->setCursorPosition(QPoint(selectionRect.right(), cursorPosition.y()));
                }
            }
            break;
        case Qt::Key_Right:
            if (!isShiftPressed || !isMsaEditor) {
                moveSelection(1, 0);
            } else {
                bool isMoveLeftSide = cursorPosition.x() == selectionRect.right() && selectionRect.width() > 1;
                if (isMoveLeftSide) {  // Move the left side (shrink).
                    setSelectionRect(QRect(selectionRect.topLeft() + QPoint(1, 0), selectionRect.bottomRight()));
                    editor->setCursorPosition(QPoint(selectionRect.right(), cursorPosition.y()));
                } else {  // Move the right side (grow).
                    setSelectionRect(QRect(selectionRect.topLeft(), selectionRect.bottomRight() + QPoint(1, 0)));
                    editor->setCursorPosition(QPoint(selectionRect.left(), cursorPosition.y()));
                }
            }
            break;
        case Qt::Key_Up:
            if (!isShiftPressed || !isMsaEditor) {
                moveSelection(0, -1);
            } else {
                bool isMoveBottomSide = cursorPosition.y() == selectionRect.y() && selectionRect.height() > 1;
                if (isMoveBottomSide) {  // Move the bottom side (shrink).
                    setSelectionRect(QRect(selectionRect.topLeft(), selectionRect.bottomRight() + QPoint(0, -1)));
                    editor->setCursorPosition(QPoint(cursorPosition.x(), selectionRect.top()));
                } else {  // Move the top side (grow).
                    setSelectionRect(QRect(selectionRect.topLeft() + QPoint(0, -1), selectionRect.bottomRight()));
                    editor->setCursorPosition(QPoint(cursorPosition.x(), selectionRect.bottom()));
                }
            }
            break;
        case Qt::Key_Down:
            if (!isShiftPressed || !isMsaEditor) {
                moveSelection(0, 1);
            } else {
                bool isMoveTopSide = cursorPosition.y() == selectionRect.bottom() && selectionRect.height() > 1;
                if (isMoveTopSide) {  // Move the top side (shrink).
                    setSelectionRect(QRect(selectionRect.topLeft() + QPoint(0, 1), selectionRect.bottomRight()));
                    editor->setCursorPosition(QPoint(cursorPosition.x(), selectionRect.bottom()));
                } else {  // Move the bottom side (grow).
                    setSelectionRect(QRect(selectionRect.topLeft(), selectionRect.bottomRight() + QPoint(0, 1)));
                    editor->setCursorPosition(QPoint(cursorPosition.x(), selectionRect.top()));
                }
            }
            break;
        case Qt::Key_Delete:
            if (!isAlignmentLocked() && !isShiftPressed) {
                emit si_startMaChanging();
                deleteCurrentSelection();
            }
            break;
        case Qt::Key_Home:
            if (isShiftPressed) {
                ui->getScrollController()->scrollToEnd(ScrollController::Up);
                editor->setCursorPosition(QPoint(editor->getCursorPosition().x(), 0));
            } else {
                ui->getScrollController()->scrollToEnd(ScrollController::Left);
                editor->setCursorPosition(QPoint(0, editor->getCursorPosition().y()));
            }
            break;
        case Qt::Key_End:
            if (isShiftPressed) {
                ui->getScrollController()->scrollToEnd(ScrollController::Down);
                editor->setCursorPosition(QPoint(editor->getCursorPosition().x(), getViewRowCount() - 1));
            } else {
                ui->getScrollController()->scrollToEnd(ScrollController::Right);
                editor->setCursorPosition(QPoint(editor->getAlignmentLen() - 1, editor->getCursorPosition().y()));
            }
            break;
        case Qt::Key_PageUp:
            ui->getScrollController()->scrollPage(isShiftPressed ? ScrollController::Up : ScrollController::Left);
            break;
        case Qt::Key_PageDown:
            ui->getScrollController()->scrollPage(isShiftPressed ? ScrollController::Down : ScrollController::Right);
            break;
        case Qt::Key_Backspace:
            removeGapsPrecedingSelection(isGenuineCtrlPressed ? 1 : -1);
            break;
        case Qt::Key_Insert:
        case Qt::Key_Space:
            if (!isAlignmentLocked()) {
                emit si_startMaChanging();
                insertGapsBeforeSelection(isGenuineCtrlPressed ? 1 : -1);
            }
            break;
    }
    QWidget::keyPressEvent(e);
}

void MaEditorSequenceArea::keyReleaseEvent(QKeyEvent* ke) {
    if ((ke->key() == Qt::Key_Space || ke->key() == Qt::Key_Delete) && !isAlignmentLocked() && !ke->isAutoRepeat()) {
        emit si_stopMaChanging(true);
    }

    QWidget::keyReleaseEvent(ke);
}

void MaEditorSequenceArea::drawBackground(QPainter&) {
}

void MaEditorSequenceArea::insertGapsBeforeSelection(int countOfGaps, bool moveSelectedRect) {
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );
    QRect selectionRect = selection.toRect();
    SAFE_POINT(isInRange(selectionRect), "Selection is not in range", );

    if (countOfGaps == -1) {
        countOfGaps = selectionRect.width();
    }
    CHECK(countOfGaps > 0, );

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    MultipleAlignmentObject* maObj = editor->getMaObject();
    if (maObj == nullptr || maObj->isStateLocked()) {
        return;
    }
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );

    const MultipleAlignment& ma = maObj->getMultipleAlignment();
    if (selectionRect.width() == ma->getLength() && selectionRect.height() == ma->getRowCount()) {
        return;
    }

    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    maObj->insertGapByRowIndexList(selectedMaRowIndexes, selectionRect.x(), countOfGaps);
    adjustReferenceLength(os);
    CHECK_OP(os, );
    if (moveSelectedRect) {
        moveSelection(countOfGaps, 0, true);
    }
    if (!editor->getSelection().isEmpty()) {
        ui->getScrollController()->scrollToMovedSelection(ScrollController::Right);
    }
}

void MaEditorSequenceArea::removeGapsPrecedingSelection(int countOfGaps) {
    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );
    MultipleAlignmentObject* maObj = editor->getMaObject();
    CHECK(!maObj->isStateLocked(), );
    QRect selectionRect = selection.toRect();

    // Don't perform the deletion if the selection is at the alignment start.
    if (selectionRect.x() == 0 || countOfGaps < -1 || countOfGaps == 0) {
        return;
    }
    int removedRegionWidth = (countOfGaps == -1) ? selectionRect.width() : countOfGaps;
    QPoint topLeftCornerOfRemovedRegion(selectionRect.x() - removedRegionWidth, selectionRect.y());
    if (topLeftCornerOfRemovedRegion.x() < 0) {
        removedRegionWidth -= qAbs(topLeftCornerOfRemovedRegion.x());
        topLeftCornerOfRemovedRegion.setX(0);
    }

    // If this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);

    QList<int> selectedMaRowIndexes = editor->getSelectionController()->getSelectedMaRowIndexes();
    int countOfDeletedSymbols = maObj->deleteGapByRowIndexList(os, selectedMaRowIndexes, topLeftCornerOfRemovedRegion.x(), removedRegionWidth);

    // if some symbols were actually removed and the selection is not located
    // at the alignment end, then it's needed to move the selection
    // to the place of the removed symbols
    if (countOfDeletedSymbols > 0) {
        QRect newSelectionRect(selectionRect.x() - countOfDeletedSymbols,
                               topLeftCornerOfRemovedRegion.y(),
                               selectionRect.width(),
                               selectionRect.height());
        setSelectionRect(newSelectionRect);
    }
}

void MaEditorSequenceArea::cancelShiftTracking() {
    shifting = false;
    selecting = false;
    changeTracker.finishTracking();
    editor->getMaObject()->releaseState();
}

void MaEditorSequenceArea::drawAll() {
    QSize s = size() * devicePixelRatio();
    if (cachedView->size() != s) {
        delete cachedView;
        cachedView = new QPixmap(s);
        cachedView->setDevicePixelRatio(devicePixelRatio());
        completeRedraw = true;
    }
    if (completeRedraw) {
        cachedView->fill(Qt::transparent);
        QPainter pCached(cachedView);
        drawVisibleContent(pCached);
        completeRedraw = false;
    }

    QPainter painter(this);
    painter.fillRect(QRect(QPoint(0, 0), s), Qt::white);
    drawBackground(painter);

    painter.drawPixmap(0, 0, *cachedView);
    renderer->drawSelectionFrame(painter);
    renderer->drawFocus(painter);
}

void MaEditorSequenceArea::updateColorAndHighlightSchemes() {
    Settings* s = AppContext::getSettings();
    if (!s || !editor) {
        return;
    }
    MultipleAlignmentObject* maObj = editor->getMaObject();
    if (!maObj) {
        return;
    }

    const DNAAlphabet* al = maObj->getAlphabet();
    if (!al) {
        return;
    }

    MsaColorSchemeRegistry* csr = AppContext::getMsaColorSchemeRegistry();
    MsaHighlightingSchemeRegistry* hsr = AppContext::getMsaHighlightingSchemeRegistry();

    QString csid;
    QString hsid;
    getColorAndHighlightingIds(csid, hsid);
    MsaColorSchemeFactory* csf = csr->getSchemeFactoryById(csid);
    MsaHighlightingSchemeFactory* hsf = hsr->getSchemeFactoryById(hsid);
    initColorSchemes(csf);
    initHighlightSchemes(hsf);
}

void MaEditorSequenceArea::initColorSchemes(MsaColorSchemeFactory* defaultColorSchemeFactory) {
    MsaColorSchemeRegistry* msaColorSchemeRegistry = AppContext::getMsaColorSchemeRegistry();
    connect(msaColorSchemeRegistry, SIGNAL(si_customSettingsChanged()), SLOT(sl_registerCustomColorSchemes()));

    registerCommonColorSchemes();
    sl_registerCustomColorSchemes();

    applyColorScheme(defaultColorSchemeFactory->getId());
}

void MaEditorSequenceArea::registerCommonColorSchemes() {
    qDeleteAll(colorSchemeMenuActions);
    colorSchemeMenuActions.clear();

    MsaSchemesMenuBuilder::createAndFillColorSchemeMenuActions(colorSchemeMenuActions, MsaSchemesMenuBuilder::Common, getEditor()->getMaObject()->getAlphabet()->getType(), this);
}

void MaEditorSequenceArea::initHighlightSchemes(MsaHighlightingSchemeFactory* hsf) {
    qDeleteAll(highlightingSchemeMenuActions);
    highlightingSchemeMenuActions.clear();
    SAFE_POINT(hsf != nullptr, "Highlight scheme factory is NULL", );

    MultipleAlignmentObject* maObj = editor->getMaObject();
    QVariantMap settings = highlightingScheme != nullptr ? highlightingScheme->getSettings() : QVariantMap();
    delete highlightingScheme;

    highlightingScheme = hsf->create(this, maObj);
    highlightingScheme->applySettings(settings);

    MsaSchemesMenuBuilder::createAndFillHighlightingMenuActions(highlightingSchemeMenuActions, getEditor()->getMaObject()->getAlphabet()->getType(), this);
    QList<QAction*> tmpActions = QList<QAction*>() << highlightingSchemeMenuActions;
    foreach (QAction* action, tmpActions) {
        action->setChecked(action->data() == hsf->getId());
    }
}

MsaColorSchemeFactory* MaEditorSequenceArea::getDefaultColorSchemeFactory() const {
    MsaColorSchemeRegistry* msaColorSchemeRegistry = AppContext::getMsaColorSchemeRegistry();

    switch (editor->getMaObject()->getAlphabet()->getType()) {
        case DNAAlphabet_RAW:
            return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::EMPTY);
        case DNAAlphabet_NUCL:
            return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::UGENE_NUCL);
        case DNAAlphabet_AMINO:
            return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::UGENE_AMINO);
        default:
            FAIL(tr("Unknown alphabet"), nullptr);
    }
    return nullptr;
}

MsaHighlightingSchemeFactory* MaEditorSequenceArea::getDefaultHighlightingSchemeFactory() const {
    MsaHighlightingSchemeRegistry* hsr = AppContext::getMsaHighlightingSchemeRegistry();
    MsaHighlightingSchemeFactory* hsf = hsr->getSchemeFactoryById(MsaHighlightingScheme::EMPTY);
    return hsf;
}

void MaEditorSequenceArea::getColorAndHighlightingIds(QString& csid, QString& hsid) {
    DNAAlphabetType atype = getEditor()->getMaObject()->getAlphabet()->getType();
    Settings* s = AppContext::getSettings();
    switch (atype) {
        case DNAAlphabet_RAW:
            csid = s->getValue(SETTINGS_ROOT + SETTINGS_COLOR_RAW, MsaColorScheme::EMPTY).toString();
            hsid = s->getValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_RAW, MsaHighlightingScheme::EMPTY).toString();
            break;
        case DNAAlphabet_NUCL:
            csid = s->getValue(SETTINGS_ROOT + SETTINGS_COLOR_NUCL, MsaColorScheme::UGENE_NUCL).toString();
            hsid = s->getValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_NUCL, MsaHighlightingScheme::EMPTY).toString();
            break;
        case DNAAlphabet_AMINO:
            csid = s->getValue(SETTINGS_ROOT + SETTINGS_COLOR_AMINO, MsaColorScheme::UGENE_AMINO).toString();
            hsid = s->getValue(SETTINGS_ROOT + SETTINGS_HIGHLIGHT_AMINO, MsaHighlightingScheme::EMPTY).toString();
            break;
        default:
            csid = "";
            hsid = "";
            break;
    }

    MsaColorSchemeRegistry* csr = AppContext::getMsaColorSchemeRegistry();
    MsaHighlightingSchemeRegistry* hsr = AppContext::getMsaHighlightingSchemeRegistry();

    MsaColorSchemeFactory* csf = csr->getSchemeFactoryById(csid);
    if (csf == nullptr) {
        csid = getDefaultColorSchemeFactory()->getId();
    }
    MsaHighlightingSchemeFactory* hsf = hsr->getSchemeFactoryById(hsid);
    if (hsf == nullptr) {
        hsid = getDefaultHighlightingSchemeFactory()->getId();
    }

    if (colorScheme != nullptr && colorScheme->getFactory()->isAlphabetTypeSupported(atype)) {
        csid = colorScheme->getFactory()->getId();
    }
    if (highlightingScheme != nullptr && highlightingScheme->getFactory()->isAlphabetTypeSupported(atype)) {
        hsid = highlightingScheme->getFactory()->getId();
    }
}

void MaEditorSequenceArea::applyColorScheme(const QString& id) {
    CHECK(ui->getEditor()->getMaObject() != nullptr, );

    MsaColorSchemeFactory* factory = AppContext::getMsaColorSchemeRegistry()->getSchemeFactoryById(id);
    delete colorScheme;
    colorScheme = factory->create(this, ui->getEditor()->getMaObject());

    connect(factory, SIGNAL(si_factoryChanged()), SLOT(sl_colorSchemeFactoryUpdated()), Qt::UniqueConnection);
    connect(factory, SIGNAL(destroyed(QObject*)), SLOT(sl_setDefaultColorScheme()), Qt::UniqueConnection);

    QList<QAction*> tmpActions = QList<QAction*>() << colorSchemeMenuActions << customColorSchemeMenuActions;
    foreach (QAction* action, tmpActions) {
        action->setChecked(action->data() == id);
    }

    if (qobject_cast<MSAEditor*>(getEditor()) != nullptr) {  // to avoid setting of sanger scheme
        switch (ui->getEditor()->getMaObject()->getAlphabet()->getType()) {
            case DNAAlphabet_RAW:
                AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_COLOR_RAW, id);
                break;
            case DNAAlphabet_NUCL:
                AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_COLOR_NUCL, id);
                break;
            case DNAAlphabet_AMINO:
                AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_COLOR_AMINO, id);
                break;
            default:
                FAIL(tr("Unknown alphabet"), );
                break;
        }
    }

    completeRedraw = true;
    update();
    emit si_highlightingChanged();
}

void MaEditorSequenceArea::processCharacterInEditMode(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) {
        exitFromEditCharacterMode();
        return;
    }

    QString text = e->text().toUpper();
    if (1 == text.length()) {
        if (isCharacterAcceptable(text)) {
            QChar newChar = text.at(0);
            newChar = (newChar == '-' || newChar == emDash || newChar == ' ') ? U2Msa::GAP_CHAR : newChar;
            processCharacterInEditMode(newChar.toLatin1());
        } else {
            MainWindow* mainWindow = AppContext::getMainWindow();
            mainWindow->addNotification(getInacceptableCharacterErrorMessage(), Error_Not);
            exitFromEditCharacterMode();
        }
    }
}

void MaEditorSequenceArea::processCharacterInEditMode(char newCharacter) {
    switch (maMode) {
        case ReplaceCharMode:
            replaceChar(newCharacter);
            break;
        case InsertCharMode:
            insertChar(newCharacter);
        case ViewMode:
        default:
            // do nothing
            ;
    }
}

void MaEditorSequenceArea::replaceChar(char newCharacter) {
    CHECK(maMode == ReplaceCharMode, );

    MultipleAlignmentObject* maObj = editor->getMaObject();
    CHECK(!maObj->isStateLocked(), );

    const MaEditorSelection& selection = editor->getSelection();
    CHECK(!selection.isEmpty(), );

    MaCollapseModel* collapseModel = editor->getCollapseModel();
    QList<QRect> selectedRects = selection.getRectList();  // Get a copy of rect to avoid parallel modification.

    if (newCharacter == U2Msa::GAP_CHAR) {  // Do not allow to replace all chars in any row with a gap.
        bool hasEmptyRowsAsResult = false;
        U2Region columnRange = selection.getColumnRegion();
        for (int i = 0; i < selectedRects.size() && !hasEmptyRowsAsResult; i++) {
            const QRect& selectedRect = selectedRects[i];
            for (int viewRowIndex = selectedRect.top(); viewRowIndex <= selectedRect.bottom() && !hasEmptyRowsAsResult; viewRowIndex++) {
                int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
                MultipleAlignmentRow row = maObj->getRow(maRowIndex);
                hasEmptyRowsAsResult = columnRange.contains(U2Region::fromStartAndEnd(row->getCoreStart(), row->getCoreEnd()));
            }
        }
        if (hasEmptyRowsAsResult) {
            uiLog.info(tr("Can't replace selected characters. The result row will have only gaps."));
            exitFromEditCharacterMode();
            return;
        }
    }
    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    SAFE_POINT_OP(os, );
    for (const QRect& selectedRect : qAsConst(selectedRects)) {
        for (int viewRowIndex = selectedRect.top(); viewRowIndex <= selectedRect.bottom(); viewRowIndex++) {
            int maRowIndex = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
            maObj->replaceCharacters(U2Region::fromXRange(selectedRect), maRowIndex, newCharacter);
        }
    }

    exitFromEditCharacterMode();
}

void MaEditorSequenceArea::exitFromEditCharacterMode() {
    CHECK(maMode != ViewMode, );
    editModeAnimationTimer.stop();
    selectionColor = Qt::black;
    maMode = ViewMode;
    sl_updateActions();
    update();
}

bool MaEditorSequenceArea::isCharacterAcceptable(const QString& text) const {
    static const QRegExp latinCharacterOrGap(QString("([A-Z]| |-|%1)").arg(emDash));
    return latinCharacterOrGap.exactMatch(text);
}

const QString& MaEditorSequenceArea::getInacceptableCharacterErrorMessage() const {
    static const QString message = tr("It is not possible to insert the character into the alignment. "
                                      "Please use a character from set A-Z (upper-case or lower-case) or the gap character ('Space', '-' or '%1').")
                                       .arg(emDash);
    return message;
}

void MaEditorSequenceArea::updateCollapseModel(const MaModificationInfo&) {
}

MaEditorSequenceArea::MaMode MaEditorSequenceArea::getMode() const {
    return maMode;
}

}  // namespace U2
