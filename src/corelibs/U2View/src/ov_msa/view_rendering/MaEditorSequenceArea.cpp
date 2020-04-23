/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QCursor>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>

#include <U2Algorithm/MsaHighlightingScheme.h>
#include <U2Algorithm/MsaColorScheme.h>

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

#include "MaEditorSequenceArea.h"
#include "MaEditorWgt.h"
#include "SequenceAreaRenderer.h"
#include "UndoRedoFramework.h"
#include "ov_msa/Highlighting/MSAHighlightingTabFactory.h"
#include "ov_msa/Highlighting/MsaSchemesMenuBuilder.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorNameList.h"
#include "ov_msa/McaEditorWgt.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/RowHeightController.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

const QChar MaEditorSequenceArea::emDash = QChar(0x2015);

MaEditorSequenceArea::MaEditorSequenceArea(MaEditorWgt *ui, GScrollBar *hb, GScrollBar *vb)
    : editor(ui->getEditor()),
      ui(ui),
      colorScheme(NULL),
      highlightingScheme(NULL),
      shBar(hb),
      svBar(vb),
      editModeAnimationTimer(this),
      prevPressedButton(Qt::NoButton),
      maVersionBeforeShifting(-1),
      replaceCharacterAction(NULL),
      useDotsAction(NULL),
      changeTracker(editor->getMaObject()->getEntityRef())
{
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    // show rubber band for selection in MSA editor only
    showRubberBandOnSelection = qobject_cast<MSAEditor*>(editor) != NULL;
    maMode = ViewMode;

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumSize(100, 100);
    selecting = false;
    shifting = false;
    editingEnabled = false;
    movableBorder = SelectionModificationHelper::NoMovableBorder;
    isCtrlPressed = false;
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

    fillWithGapsinsSymAction = new QAction(tr("Fill selection with gaps"), this);
    fillWithGapsinsSymAction->setObjectName("fill_selection_with_gaps");
    connect(fillWithGapsinsSymAction, SIGNAL(triggered()), SLOT(sl_fillCurrentSelectionWithGaps()));
    addAction(fillWithGapsinsSymAction);

    QAction* undoAction = ui->getUndoAction();
    QAction* redoAction = ui->getRedoAction();
    addAction(undoAction);
    addAction(redoAction);

    connect(this, SIGNAL(si_selectionChanged(const MaEditorSelection&, const MaEditorSelection&)), SLOT(sl_completeRedraw()));
    connect(editor, SIGNAL(si_completeUpdate()), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), SLOT(sl_completeUpdate()));
    connect(editor, SIGNAL(si_updateActions()), SLOT(sl_updateActions()));
    connect(ui, SIGNAL(si_completeRedraw()), SLOT(sl_completeRedraw()));
    connect(hb, SIGNAL(actionTriggered(int)), SLOT(sl_hScrollBarActionPerformed()));


    // SANGER_TODO: why is it commented?
//    connect(editor, SIGNAL(si_fontChanged(QFont)), SLOT(sl_fontChanged(QFont)));

    connect(&editModeAnimationTimer, SIGNAL(timeout()), SLOT(sl_changeSelectionColor()));

    connect(editor->getMaObject(), SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
        SLOT(sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)));

    connect(this, SIGNAL(si_startMaChanging()), ui->getUndoRedoFramework(), SLOT(sl_updateUndoRedoState()));
    connect(this, SIGNAL(si_stopMaChanging(bool)), ui->getUndoRedoFramework(), SLOT(sl_updateUndoRedoState()));
}

MaEditorSequenceArea::~MaEditorSequenceArea() {
    exitFromEditCharacterMode();
    delete cachedView;
    deleteOldCustomSchemes();
    delete highlightingScheme;
}

MaEditor *MaEditorSequenceArea::getEditor() const {
    return editor;
}

QSize MaEditorSequenceArea::getCanvasSize(const QList<int> &seqIdx, const U2Region &region) const {
    return QSize(ui->getBaseWidthController()->getBasesWidth(region),
                 ui->getRowHeightController()->getSumOfRowHeightsByMaIndexes(seqIdx));
}

int MaEditorSequenceArea::getFirstVisibleBase() const {
    return ui->getScrollController()->getFirstVisibleBase();
}

int MaEditorSequenceArea::getLastVisibleBase(bool countClipped) const {
    return getEditor()->getUI()->getScrollController()->getLastVisibleBase(width(), countClipped);
}

int MaEditorSequenceArea::getNumVisibleBases() const {
    return ui->getDrawHelper()->getVisibleBasesCount(width());
}

int MaEditorSequenceArea::getViewRowCount() const {
    return ui->getCollapseModel()->getViewRowCount();
}

int MaEditorSequenceArea::getRowIndex(const int num) const {
    CHECK(!isAlignmentEmpty(), -1);
    MaCollapseModel *model = ui->getCollapseModel();
    SAFE_POINT(model != NULL, tr("Invalid collapsible item model!"), -1);
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

bool MaEditorSequenceArea::isInRange(const QPoint &point) const {
    return isPosInRange(point.x()) && isSeqInRange(point.y());
}

QPoint MaEditorSequenceArea::boundWithVisibleRange(const QPoint &point) const {
    return QPoint(
            qBound(0, point.x(), editor->getAlignmentLen() - 1),
            qBound(0, point.y(), ui->getCollapseModel()->getViewRowCount() - 1)
    );
}

bool MaEditorSequenceArea::isVisible(const QPoint& p, bool countClipped) const {
    return isPositionVisible(p.x(), countClipped) && isRowVisible(p.y(), countClipped);
}

bool MaEditorSequenceArea::isPositionVisible(int position, bool countClipped) const {
    return ui->getDrawHelper()->getVisibleBases(width(), countClipped, countClipped).contains(position);
}

bool MaEditorSequenceArea::isRowVisible(int rowNumber, bool countClipped) const {
    const int rowIndex = ui->getCollapseModel()->getMaRowIndexByViewRowIndex(rowNumber);
    return ui->getDrawHelper()->getVisibleMaRowIndexes(height(), countClipped, countClipped).contains(rowIndex);
}

const MaEditorSelection & MaEditorSequenceArea::getSelection() const {
    return selection;
}

QFont MaEditorSequenceArea::getFont() const {
    return editor->getFont();
}

void MaEditorSequenceArea::setSelection(const MaEditorSelection& newSelection) {
    CHECK(!isAlignmentEmpty() || newSelection.isEmpty(), );
    if (newSelection == selection) {
        return;
    }
    exitFromEditCharacterMode();

    MaEditorSelection prevSelection = selection;
    if (newSelection.isEmpty()) {
        selection = newSelection;
    } else {
        selection = MaEditorSelection(MaEditorSequenceArea::boundWithVisibleRange(newSelection.topLeft()),
                                      MaEditorSequenceArea::boundWithVisibleRange(newSelection.bottomRight()));
    }

    QList<int> selectedMaRowsIndexes = getSelectedMaRowIndexes();
    selectedMaRowIds = editor->getMaObject()->convertMaRowIndexesToMaRowIds(selectedMaRowsIndexes);
    selectedColumns = selection.getXRegion();

    QStringList selectedRowNames;
    for (int i = 0; i < selectedMaRowsIndexes.length(); i++) {
        int maRow = selectedMaRowsIndexes[i];
        selectedRowNames.append(editor->getMaObject()->getRow(maRow)->getName());
    }
    emit si_selectionChanged(selectedRowNames);
    emit si_selectionChanged(selection, prevSelection);
    update();

    //TODO: the code below can be moved to the sl_updateActions().
    bool selectionExists = !selection.isEmpty();
    ui->getCopySelectionAction()->setEnabled(selectionExists);
    ui->getCopyFormattedSelectionAction()->setEnabled(selectionExists);
    emit si_copyFormattedChanging(selectionExists);

    sl_updateActions();
}

void MaEditorSequenceArea::moveSelection(int dx, int dy, bool allowSelectionResize) {
    int leftX = selection.x();
    int topY = selection.y();
    int bottomY = selection.y() + selection.height() - 1;
    int rightX = selection.x() + selection.width() - 1;
    QPoint baseTopLeft(leftX, topY);
    QPoint baseBottomRight(rightX,bottomY);

    QPoint newTopLeft = baseTopLeft + QPoint(dx,dy);
    QPoint newBottomRight = baseBottomRight + QPoint(dx,dy);

    if ((!isInRange(newTopLeft)) || (!isInRange(newBottomRight))) {
        if (!allowSelectionResize) {
            return;
        } else {
            MaEditorSelection newSelection(selection.topLeft(),
                                            qMin(selection.width(), editor->getAlignmentLen() - newTopLeft.x()),
                                            qMin(selection.height(), editor->getNumSequences() - newTopLeft.y()));
            setSelection(newSelection);
        }
    }

    MaEditorSelection newSelection(newTopLeft, selection.width(), selection.height());
    setSelection(newSelection);
    const QPoint& cursorPosition = editor->getCursorPosition();
    editor->setCursorPosition(QPoint(cursorPosition.x() + dx, cursorPosition.y() + dy));
    ui->getScrollController()->scrollToMovedSelection(dx, dy);
}

QList<int> MaEditorSequenceArea::getSelectedMaRowIndexes() const {
    return ui->getCollapseModel()->getMaRowIndexesByViewRowIndexes(selection.getYRegion(), true);
}

int MaEditorSequenceArea::getTopSelectedMaRow() const {
    if (selection.isEmpty()) {
        return -1;
    }
    int firstSelectedViewRow = (int) selection.getYRegion().startPos;
    return ui->getCollapseModel()->getMaRowIndexByViewRowIndex(firstSelectedViewRow);
}

QString MaEditorSequenceArea::getCopyFormattedAlgorithmId() const{
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + SETTINGS_COPY_FORMATTED, BaseDocumentFormats::CLUSTAL_ALN).toString();
}

void MaEditorSequenceArea::deleteCurrentSelection() {
    CHECK(getEditor() != NULL, );
    CHECK(!selection.isEmpty(), );

    MultipleAlignmentObject* maObj = getEditor()->getMaObject();
    CHECK (!maObj->isStateLocked(),);

    Q_ASSERT(isInRange(selection.topLeft()));

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    // Selection width may be equal to 0 (for example in MCA) -> this means that the whole row is selected.
    int effectiveWidth = selection.x() == 0 && selection.width() == 0 ? editor->getAlignmentLen() : selection.width();
    bool wholeRowRemoved = effectiveWidth == editor->getAlignmentLen();

    if (wholeRowRemoved) {
        // Reuse code of the name list.
        ui->getEditorNameList()->sl_removeSelectedRows();
        return;
    }

    Q_ASSERT(isInRange(QPoint(selection.x() + effectiveWidth - 1, selection.y() + selection.height() - 1)));

    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    int numRows = (int) maObj->getNumRows();
    if (selectedMaRows.size() == numRows) {
        bool isResultAlignmentEmpty = true;
        U2Region xRegion(selection.x(), effectiveWidth);
        for (int i = 0; i < selectedMaRows.size() && isResultAlignmentEmpty; i++) {
            int maRow = selectedMaRows[i];
            isResultAlignmentEmpty = maObj->isRegionEmpty(0, maRow, xRegion.startPos, 1) &&
                                     maObj->isRegionEmpty(xRegion.endPos(), maRow, numRows - xRegion.endPos(), 1);
        }
        if (isResultAlignmentEmpty) {
            return;
        }
    }

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );
    maObj->removeRegion(selectedMaRows, selection.x(), effectiveWidth, true);
    GRUNTIME_NAMED_COUNTER(cvar, tvar, "Delete current selection", editor->getFactoryId());
}

bool MaEditorSequenceArea::shiftSelectedRegion(int shift) {
    CHECK(shift != 0, true);

    // shifting of selection
    MultipleAlignmentObject *maObj = editor->getMaObject();
    if (maObj->isStateLocked()) {
        return false;
    }
    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    if (maObj->isRegionEmpty(selectedMaRows, selection.x(), selection.width())) {
        return true;
    }
    // backup current selection for the case when selection might disappear
    MaEditorSelection selectionBackup = selection;

    int resultShift = shiftRegion(shift);
    if (resultShift == 0) {
        return false;
    }
    U2OpStatus2Log os;
    adjustReferenceLength(os);

    const QPoint& cursorPos = editor->getCursorPosition();
    int newCursorPosX = (cursorPos.x() + resultShift >= 0) ? cursorPos.x() + resultShift : 0;
    editor->setCursorPosition(QPoint(newCursorPosX, cursorPos.y()));

    MaEditorSelection newSelection(selectionBackup.x() + resultShift, selectionBackup.y(),
                                   selectionBackup.width(), selectionBackup.height());
    setSelection(newSelection);
    if (resultShift > 0) {
        ui->getScrollController()->scrollToBase(static_cast<int>(newSelection.getXRegion().endPos() - 1), width());
    } else {
        ui->getScrollController()->scrollToBase(newSelection.x(), width());
    }

    return true;
}

int MaEditorSequenceArea::shiftRegion(int shift) {
    int resultShift = 0;

    MultipleAlignmentObject *maObj = editor->getMaObject();
    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    const int selectionWidth = selection.width();
    const int height = selectedMaRows.size();
    const int y = getTopSelectedMaRow();
    int x = selection.x();
    if (isCtrlPressed) {
        if (shift > 0) {
            QList<U2MsaGap> gapModelToRemove = findRemovableGapColumns(shift);
            if (!gapModelToRemove.isEmpty()) {
                foreach(U2MsaGap gap, gapModelToRemove) {
                    x = selection.x();
                    U2OpStatus2Log os;
                    const int length = maObj->getLength();
                    if (length != gap.offset) {
                        maObj->deleteGapByRowIndexList(os, selectedMaRows, gap.offset, gap.gap);
                    }
                    CHECK_OP(os, resultShift);
                    resultShift += maObj->shiftRegion(x, y, selectionWidth, height, gap.gap);
                    MaEditorSelection newSel(QPoint(gap.gap + x, selection.y()), selectionWidth, height);
                    setSelection(newSel);
                }
            }
        } else if (shift < 0 && !ctrlModeGapModel.isEmpty()) {
            QList<U2MsaGap> gapModelToRestore = findRestorableGapColumns(shift);
            if (!gapModelToRestore.isEmpty()) {
                resultShift = maObj->shiftRegion(x, y, selectionWidth, height, shift);
                foreach(U2MsaGap gap, gapModelToRestore) {
                    if (gap.endPos() < lengthOnMousePress) {
                        maObj->insertGapByRowIndexList(selectedMaRows, gap.offset, gap.gap);
                    } else if (gap.offset >= lengthOnMousePress) {
                        U2OpStatus2Log os;
                        U2Region allRows(0, maObj->getNumRows());
                        maObj->deleteGap(os, allRows, maObj->getLength() - gap.gap, gap.gap);
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

QList<U2MsaGap> MaEditorSequenceArea::findRemovableGapColumns(int& shift) {
    CHECK(shift > 0, QList<U2MsaGap>());

    int numOfRemovableColumns = 0;
    U2MsaRowGapModel commonGapColumns = findCommonGapColumns(numOfRemovableColumns);
    if (numOfRemovableColumns < shift){
        int count = shift - numOfRemovableColumns;
        commonGapColumns << addTrailingGapColumns(count);
    }

    QList<U2MsaGap> gapColumnsToRemove;
    int count = shift;
    foreach(U2MsaGap gap, commonGapColumns) {
        if (count >= gap.gap) {
            gapColumnsToRemove.append(gap);
            count -= gap.gap;
            if (count == 0) {
                break;
            }
        } else {
            gapColumnsToRemove.append(U2MsaGap(gap.offset, count));
            break;
        }
    }

    ctrlModeGapModel << gapColumnsToRemove;

    if (count < shift) {
        shift -= count;
    }
    return gapColumnsToRemove;
}

QList<U2MsaGap> MaEditorSequenceArea::findCommonGapColumns(int& numOfColumns) {
    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    if (selectedMaRows.isEmpty()) {
        return QList<U2MsaGap>();
    }
    int x = selection.x();
    int wight = selection.width();
    U2MsaListGapModel listGapModel = editor->getMaObject()->getGapModel();

    U2MsaRowGapModel gapModelToUpdate;
    foreach(U2MsaGap gap, listGapModel[selectedMaRows[0]]) {
        if (gap.offset + gap.gap <= x + wight) {
            continue;
        }
        if (gap.offset < x + wight && gap.offset + gap.gap > x + wight) {
            int startPos = x + wight;
            U2MsaGap g(startPos, gap.offset + gap.gap - startPos);
            gapModelToUpdate << g;
        } else {
            gapModelToUpdate << gap;
        }
    }

    numOfColumns = 0;
    for (int i = 1; i < selectedMaRowIds.size(); i++) {
        int maRow = selectedMaRows[i];
        U2MsaRowGapModel currentGapModelToRemove;
        int currentNumOfColumns = 0;
        foreach(U2MsaGap gap, listGapModel[maRow]) {
            foreach(U2MsaGap gapToRemove, gapModelToUpdate) {
                U2MsaGap intersectedGap = gap.intersect(gapToRemove);
                if (intersectedGap.gap == 0) {
                    continue;
                }
                currentNumOfColumns += intersectedGap.gap;
                currentGapModelToRemove << intersectedGap;
            }
        }
        gapModelToUpdate = currentGapModelToRemove;
        numOfColumns = currentNumOfColumns;
    }

    return gapModelToUpdate;
}

U2MsaGap MaEditorSequenceArea::addTrailingGapColumns(int count) {
    MultipleAlignmentObject *maObj = editor->getMaObject();
    qint64 length = maObj->getLength();
    return U2MsaGap(length, count);
}

QList<U2MsaGap> MaEditorSequenceArea::findRestorableGapColumns(const int shift) {
    CHECK(shift < 0, QList<U2MsaGap>());
    CHECK(!ctrlModeGapModel.isEmpty(), QList<U2MsaGap>());

    QList<U2MsaGap> gapColumnsToRestore;
    int absShift = qAbs(shift);
    const int size = ctrlModeGapModel.size();
    for (int i = size - 1; i >= 0; i--) {
        if (ctrlModeGapModel[i].gap >= absShift) {
            const int offset = ctrlModeGapModel[i].gap - absShift;
            U2MsaGap gapToRestore(ctrlModeGapModel[i].offset + offset, absShift);
            gapColumnsToRestore.push_front(gapToRestore);
            ctrlModeGapModel[i].gap -= absShift;
            if (ctrlModeGapModel[i].gap == 0) {
                ctrlModeGapModel.removeOne(ctrlModeGapModel[i]);
            }
            break;
        } else {
            gapColumnsToRestore.push_front(ctrlModeGapModel[i]);
            absShift -= ctrlModeGapModel[i].gap;
            ctrlModeGapModel.removeOne(ctrlModeGapModel[i]);
        }
    }

    return gapColumnsToRestore;
}

void MaEditorSequenceArea::centerPos(const QPoint &point) {
    SAFE_POINT(isInRange(point), QString("Point (%1, %2) is out of range").arg(point.x()).arg(point.y()), );
    ui->getScrollController()->centerPoint(point, size());
    update();
}

void MaEditorSequenceArea::centerPos(int position) {
    SAFE_POINT(isPosInRange(position), QString("Base %1 is out of range").arg(position), );
    ui->getScrollController()->centerBase(position, width());
    update();
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
    SAFE_POINT(obj != NULL, tr("Alignment object is not available"), true);
    return obj->isStateLocked();
}

void MaEditorSequenceArea::drawVisibleContent(QPainter& painter) {
    U2Region columns = ui->getDrawHelper()->getVisibleBases(width());
    QList<int> maRows = ui->getDrawHelper()->getVisibleMaRowIndexes(height());
    CHECK(!columns.isEmpty() || !maRows.isEmpty(), );
    int xStart = ui->getBaseWidthController()->getBaseScreenRange(columns.startPos).startPos;
    int yStart = ui->getRowHeightController()->getScreenYPositionOfTheFirstVisibleRow(true);
    drawContent(painter, columns, maRows, xStart, yStart);
}

bool MaEditorSequenceArea::drawContent(QPainter& painter, const U2Region& columns, const QList<int>& maRows, int xStart, int yStart) {
    // SANGER_TODO: optimize
    return renderer->drawContent(painter, columns, maRows, xStart, yStart);
}

MsaColorScheme * MaEditorSequenceArea::getCurrentColorScheme() const {
    return colorScheme;
}

MsaHighlightingScheme * MaEditorSequenceArea::getCurrentHighlightingScheme() const {
    return highlightingScheme;
}

bool MaEditorSequenceArea::getUseDotsCheckedState() const {
    return useDotsAction->isChecked();
}

QAction *MaEditorSequenceArea::getReplaceCharacterAction() const {
    return replaceCharacterAction;
}

void MaEditorSequenceArea::sl_changeColorSchemeOutside(const QString &id) {
    QAction* a = GUIUtils::findActionByData(QList<QAction*>() << colorSchemeMenuActions << customColorSchemeMenuActions << highlightingSchemeMenuActions, id);
    if (a != NULL) {
        a->trigger();
    }
}

void MaEditorSequenceArea::sl_changeCopyFormat(const QString& alg){
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SETTINGS_COPY_FORMATTED, alg);
}

void MaEditorSequenceArea::sl_changeColorScheme() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (NULL == action) {
        action = GUIUtils::getCheckedAction(customColorSchemeMenuActions);
    }
    CHECK(action != NULL, );

    applyColorScheme(action->data().toString());
}

void MaEditorSequenceArea::sl_delCurrentSelection() {
    emit si_startMaChanging();
    deleteCurrentSelection();
    emit si_stopMaChanging(true);
}

void MaEditorSequenceArea::sl_cancelSelection() {
    if (maMode != ViewMode) {
        exitFromEditCharacterMode();
        return;
    }
    GRUNTIME_NAMED_CONDITION_COUNTER(cvat, tvar, qobject_cast<McaEditorWgt*>(sender()) != NULL, "Clear selection", editor->getFactoryId());
    MaEditorSelection emptySelection;
    setSelection(emptySelection);
}

void MaEditorSequenceArea::sl_fillCurrentSelectionWithGaps() {
    GRUNTIME_NAMED_COUNTER(cvat, tvar, "Fill selection with gaps", editor->getFactoryId());
    if(!isAlignmentLocked()) {
        emit si_startMaChanging();
        insertGapsBeforeSelection();
        emit si_stopMaChanging(true);
    }
}

void MaEditorSequenceArea::sl_alignmentChanged(const MultipleAlignment &, const MaModificationInfo &modInfo) {
    exitFromEditCharacterMode();
    updateCollapseModel(modInfo);
    restoreViewSelectionFromMaSelection();

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

void MaEditorSequenceArea::sl_completeUpdate(){
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

void MaEditorSequenceArea::sl_useDots(){
    completeRedraw = true;
    update();
    emit si_highlightingChanged();
}

void MaEditorSequenceArea::sl_registerCustomColorSchemes() {
    deleteOldCustomSchemes();

    MsaSchemesMenuBuilder::createAndFillColorSchemeMenuActions(customColorSchemeMenuActions,
                                                               MsaSchemesMenuBuilder::Custom, getEditor()->getMaObject()->getAlphabet()->getType(),
                                                               this);
}

void MaEditorSequenceArea::sl_colorSchemeFactoryUpdated() {
    applyColorScheme(colorScheme->getFactory()->getId());
}

void MaEditorSequenceArea::sl_setDefaultColorScheme() {
    MsaColorSchemeFactory *defaultFactory = getDefaultColorSchemeFactory();
    SAFE_POINT(defaultFactory != NULL, L10N::nullPointerError("default color scheme factory"), );
    applyColorScheme(defaultFactory->getId());
}

void MaEditorSequenceArea::sl_changeHighlightScheme(){
    QAction* a = qobject_cast<QAction*>(sender());
    if (a == NULL) {
        a = GUIUtils::getCheckedAction(customColorSchemeMenuActions);
    }
    CHECK(a != NULL, );

    editor->saveHighlightingSettings(highlightingScheme->getFactory()->getId(), highlightingScheme->getSettings());

    QString id = a->data().toString();
    MsaHighlightingSchemeFactory* factory = AppContext::getMsaHighlightingSchemeRegistry()->getSchemeFactoryById(id);
    SAFE_POINT(factory != NULL, L10N::nullPointerError("highlighting scheme"), );
    if (ui->getEditor()->getMaObject() == NULL) {
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

    if(!factory->isRefFree() && refSeq == -1 && ui->getEditor()->getOptionsPanel()->getActiveGroupId() != msaHighlightingId) {
        QMessageBox::warning(ui, tr("No reference sequence selected"),
            tr("Reference sequence for current highlighting scheme is not selected. Use context menu or Highlighting tab on Options panel to select it"));
    }

    foreach(QAction* action, highlightingSchemeMenuActions) {
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

void MaEditorSequenceArea::restoreViewSelectionFromMaSelection() {
    if (selectedColumns.isEmpty() || selectedMaRowIds.isEmpty()) {
        return;
    }
    MaCollapseModel* m = ui->getCollapseModel();

    int columnCount = editor->getAlignmentLen();
    int viewRowCount = m->getViewRowCount();

    // Ensure the columns region is in range.
    U2Region columnsRegions = selectedColumns;
    columnsRegions.startPos = qMin(columnsRegions.startPos, (qint64) editor->getAlignmentLen() - 1);
    qint64 selectedColumnsEndPos = qMin(columnsRegions.endPos(), (qint64) columnCount);
    columnsRegions.length = selectedColumnsEndPos - columnsRegions.startPos;

    // Convert selected MA rows indexes to view row indexes.
    QList<int> selectedMaRowIndexes = editor->getMaObject()->convertMaRowIdsToMaRowIndexes(selectedMaRowIds);
    if (!selectedMaRowIndexes.isEmpty()) {
        int newStartRowIdx = m->getViewRowIndexByMaRowIndex(selectedMaRowIndexes[0]);
        int newEndRowIdx = newStartRowIdx;
        for (int i = 1; i < selectedMaRowIndexes.size(); i++) {
            int viewRowIndex = m->getViewRowIndexByMaRowIndex(selectedMaRowIndexes[i]);
            newStartRowIdx = qMin(newStartRowIdx, viewRowIndex);
            newEndRowIdx = qMax(newEndRowIdx, viewRowIndex);
        }
        int selectionHeight = newEndRowIdx - newStartRowIdx + 1;
        if (selectionHeight <= 0 || newStartRowIdx + selectionHeight > viewRowCount) {
            sl_cancelSelection();
        } else {
            MaEditorSelection newSelection(columnsRegions.startPos, newStartRowIdx, columnsRegions.length, selectionHeight);
            setSelection(newSelection);
        }
    } else {
        sl_cancelSelection();
    }

    ui->getScrollController()->updateVerticalScrollBar();
}

void MaEditorSequenceArea::sl_modelChanged() {
    restoreViewSelectionFromMaSelection();
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

void MaEditorSequenceArea::resizeEvent(QResizeEvent *e) {
    completeRedraw = true;
    ui->getScrollController()->sl_updateScrollBars();
    emit si_visibleRangeChanged();
    QWidget::resizeEvent(e);
}

void MaEditorSequenceArea::paintEvent(QPaintEvent *e) {
    drawAll();
    QWidget::paintEvent(e);
}

void MaEditorSequenceArea::wheelEvent(QWheelEvent *we) {
    bool toMin = we->delta() > 0;
    if (we->modifiers() == 0) {
        shBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    }  else if (we->modifiers() & Qt::SHIFT) {
        svBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    }
    QWidget::wheelEvent(we);
}

void MaEditorSequenceArea::mousePressEvent(QMouseEvent *e) {
    prevPressedButton = e->button();

    if (!hasFocus()) {
        setFocus();
    }

    mousePressEventPoint = e->pos();
    mousePressViewPos = ui->getScrollController()->getViewPosByScreenPoint(mousePressEventPoint);

    if ((e->button() == Qt::LeftButton)) {
        if (Qt::ShiftModifier == e->modifiers()) {
            QWidget::mousePressEvent(e);
            return;
        }

        Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
        isCtrlPressed = km.testFlag(Qt::ControlModifier);
        lengthOnMousePress = editor->getMaObject()->getLength();

        QPoint cursorPos = boundWithVisibleRange(mousePressViewPos);
        editor->setCursorPosition(cursorPos);

        Qt::CursorShape shape = cursor().shape();
        if (shape != Qt::ArrowCursor) {
            QPoint pos = e->pos();
            changeTracker.finishTracking();
            QPoint globalMousePosition = ui->getScrollController()->getGlobalMousePosition(pos);
            const double baseWidth = ui->getBaseWidthController()->getBaseWidth();
            const double baseHeight = ui->getRowHeightController()->getSingleRowHeight();
            movableBorder = SelectionModificationHelper::getMovableSide(shape, globalMousePosition, selection.toRect(), QSize(baseWidth, baseHeight));
            moveBorder(pos);
        }
    }

    QWidget::mousePressEvent(e);
}

void MaEditorSequenceArea::mouseReleaseEvent(QMouseEvent* e) {
    rubberBand->hide();
    QPoint releasePos = ui->getScrollController()->getViewPosByScreenPoint(e->pos());
    bool isClick = !selecting && releasePos == mousePressViewPos;
    bool isSelectionResize = movableBorder != SelectionModificationHelper::NoMovableBorder;
    bool isShiftPressed = e->modifiers() == Qt::ShiftModifier;
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
        QPoint topLeft = boundWithVisibleRange(QPoint(left, top));
        QPoint bottomRight = boundWithVisibleRange(QPoint(left + width - 1, top + height - 1));
        ui->getScrollController()->scrollToPoint(releasePos, size());
        setSelection(MaEditorSelection(topLeft, bottomRight));
    } else if (isClick && e->button() == Qt::LeftButton) {
        if (isInRange(releasePos)) {
            setSelection(MaEditorSelection(releasePos, releasePos));
        } else {
            setSelection(MaEditorSelection());
        }
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

    QWidget::mouseReleaseEvent(e);
}

void MaEditorSequenceArea::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        setBorderCursor(event->pos());
        QWidget::mouseMoveEvent(event);
        return;
    }
    bool isSelectionResize = movableBorder != SelectionModificationHelper::NoMovableBorder;
    QPoint mouseMoveEventPoint = event->pos();
    ScrollController* scrollController = ui->getScrollController();
    QPoint mouseMoveViewPos = ui->getScrollController()->getViewPosByScreenPoint(mouseMoveEventPoint);

    bool isDefaultCursorMode = cursor().shape() == Qt::ArrowCursor;
    if (!shifting && selection.toRect().contains(mousePressViewPos)
        && !isAlignmentLocked() && editingEnabled && isDefaultCursorMode) {
        shifting = true;
        maVersionBeforeShifting = editor->getMaObject()->getModificationVersion();
        U2OpStatus2Log os;
        changeTracker.startTracking(os);
        CHECK_OP(os,);
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
    const QPoint globalMousePos = ui->getScrollController()->getGlobalMousePosition(p);
    setCursor(SelectionModificationHelper::getCursorShape(globalMousePos, selection.toRect(), ui->getBaseWidthController()->getBaseWidth(),
                                                          ui->getRowHeightController()->getSingleRowHeight()));
}

void MaEditorSequenceArea::moveBorder(const QPoint& screenMousePos) {
    CHECK(movableBorder != SelectionModificationHelper::NoMovableBorder, );

    QPoint globalMousePos = ui->getScrollController()->getGlobalMousePosition(screenMousePos);
    globalMousePos = QPoint(qMax(0, globalMousePos.x()), qMax(0, globalMousePos.y()));
    const qreal baseWidth = ui->getBaseWidthController()->getBaseWidth();
    const qreal baseHeight = ui->getRowHeightController()->getSingleRowHeight();

    QRect newSelection = SelectionModificationHelper::getNewSelection(movableBorder, globalMousePos, QSizeF(baseWidth, baseHeight), selection.toRect());

    setCursor(SelectionModificationHelper::getCursorShape(movableBorder, cursor().shape()));

    CHECK(!newSelection.isEmpty(), );
    if (!isPosInRange(newSelection.right())) {
        newSelection.setRight(selection.toRect().right());
    }
    if (!isSeqInRange(newSelection.bottom())) {
        newSelection.setBottom(selection.bottom());
    }

    CHECK(isInRange(newSelection.bottomRight()), );
    CHECK(isInRange(newSelection.topLeft()), );
    setSelection(MaEditorSelection(newSelection.topLeft(), newSelection.bottomRight()));
}

void MaEditorSequenceArea::keyPressEvent(QKeyEvent *e) {
    if (!hasFocus()) {
        setFocus();
    }

    int key = e->key();
    if (maMode != ViewMode) {
        processCharacterInEditMode(e);
        return;
    }

    bool enlargeSelection = qobject_cast<MSAEditor*>(getEditor()) != NULL;

    bool shift = e->modifiers().testFlag(Qt::ShiftModifier);
    const bool ctrl = e->modifiers().testFlag(Qt::ControlModifier);
#ifdef Q_OS_MAC
    // In one case it is better to use a Command key as modifier,
    // in another - a Control key. genuineCtrl - Control key on Mac OS X.
    const bool genuineCtrl = e->modifiers().testFlag(Qt::MetaModifier);
#else
    const bool genuineCtrl = ctrl;
#endif
    static QPoint selectionStart(0, 0);
    static QPoint selectionEnd(0, 0);

    if (ctrl && (key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up || key == Qt::Key_Down)) {
        //remap to page_up/page_down
        shift = key == Qt::Key_Up || key == Qt::Key_Down;
        key =  (key == Qt::Key_Up || key == Qt::Key_Left) ? Qt::Key_PageUp : Qt::Key_PageDown;
    }
    //part of these keys are assigned to actions -> so them never passed to keyPressEvent (action handling has higher priority)
    int endX, endY;
    switch(key) {
        case Qt::Key_Escape:
             sl_cancelSelection();
             break;
        case Qt::Key_Left:
            if(!shift || !enlargeSelection) {
                moveSelection(-1,0);
                break;
            }
            if (selectionEnd.x() < 1) {
                break;
            }
            selectionEnd.setX(selectionEnd.x() - 1);
            endX = selectionEnd.x();
            if (isPosInRange(endX)) {
                if (endX != -1) {
                    int firstColumn = qMin(selectionStart.x(),endX);
                    int selectionWidth = qAbs(endX - selectionStart.x()) + 1;
                    int startSeq = selection.y();
                    int height = selection.height();
                    if (selection.isEmpty()) {
                        startSeq = editor->getCursorPosition().y();
                        height = 1;
                    }
                    MaEditorSelection _selection(firstColumn, startSeq, selectionWidth, height);
                    setSelection(_selection);
                    ui->getScrollController()->scrollToBase(endX, width());
                }
            }
            break;
        case Qt::Key_Right:
            if(!shift || !enlargeSelection) {
                moveSelection(1,0);
                break;
            }
            if (selectionEnd.x() >= (editor->getAlignmentLen() - 1)) {
                break;
            }

            selectionEnd.setX(selectionEnd.x() +  1);
            endX = selectionEnd.x();
            if (isPosInRange(endX)) {
                if (endX != -1) {
                    int firstColumn = qMin(selectionStart.x(),endX);
                    int selectionWidth = qAbs(endX - selectionStart.x()) + 1;
                    int startSeq = selection.y();
                    int height = selection.height();
                    if (selection.isEmpty()) {
                        startSeq = editor->getCursorPosition().y();
                        height = 1;
                    }
                    MaEditorSelection _selection(firstColumn, startSeq, selectionWidth, height);
                    setSelection(_selection);
                    ui->getScrollController()->scrollToBase(endX, width());
                }
            }
            break;
        case Qt::Key_Up:
            if(!shift || !enlargeSelection) {
                moveSelection(0,-1);
                break;
            }
            if(selectionEnd.y() < 1) {
                break;
            }
            selectionEnd.setY(selectionEnd.y() - 1);
            endY = selectionEnd.y();
            if (isSeqInRange(endY)) {
                if (endY != -1) {
                    int startSeq = qMin(selectionStart.y(),endY);
                    int height = qAbs(endY - selectionStart.y()) + 1;
                    int firstColumn = selection.x();
                    int width = selection.width();
                    if (selection.isEmpty()) {
                        firstColumn = editor->getCursorPosition().x();
                        width = 1;
                    }
                    MaEditorSelection _selection(firstColumn, startSeq, width, height);
                    setSelection(_selection);
                    ui->getScrollController()->scrollToViewRow(endY, this->height());
                }
            }
            break;
        case Qt::Key_Down:
            if(!shift || !enlargeSelection) {
                moveSelection(0, 1);
                break;
            }
            if (selectionEnd.y() >= (ui->getCollapseModel()->getViewRowCount() - 1)) {
                break;
            }
            selectionEnd.setY(selectionEnd.y() + 1);
            endY = selectionEnd.y();
            if (isSeqInRange(endY)) {
                if (endY != -1) {
                    int startSeq = qMin(selectionStart.y(),endY);
                    int height = qAbs(endY - selectionStart.y()) + 1;
                    int firstColumn = selection.x();
                    int width = selection.width();
                    if (selection.isEmpty()) {
                        firstColumn = editor->getCursorPosition().x();
                        width = 1;
                    }
                    MaEditorSelection _selection(firstColumn, startSeq, width, height);
                    setSelection(_selection);
                    ui->getScrollController()->scrollToViewRow(endY, this->height());
                }
            }
            break;
        case Qt::Key_Delete:
            if (!isAlignmentLocked() && !shift) {
                emit si_startMaChanging();
                deleteCurrentSelection();
            }
            break;
        case Qt::Key_Home:
            if (shift) {
                // vertical scrolling
                ui->getScrollController()->scrollToEnd(ScrollController::Up);
                editor->setCursorPosition(QPoint(editor->getCursorPosition().x(), 0));
            } else {
                // horizontal scrolling
                ui->getScrollController()->scrollToEnd(ScrollController::Left);
                editor->setCursorPosition(QPoint(0, editor->getCursorPosition().y()));
            }
            break;
        case Qt::Key_End:
            if (shift) {
                // vertical scrolling
                ui->getScrollController()->scrollToEnd(ScrollController::Down);
                editor->setCursorPosition(QPoint(editor->getCursorPosition().x(), getViewRowCount() - 1));
            } else {
                // horizontal scrolling
                ui->getScrollController()->scrollToEnd(ScrollController::Right);
                editor->setCursorPosition(QPoint(editor->getAlignmentLen() - 1, editor->getCursorPosition().y()));
            }
            break;
        case Qt::Key_PageUp:
            if (shift) {
                // vertical scrolling
                ui->getScrollController()->scrollPage(ScrollController::Up);
            } else {
                // horizontal scrolling
                ui->getScrollController()->scrollPage(ScrollController::Left);
            }
            break;
        case Qt::Key_PageDown:
            if (shift) {
                // vertical scrolling
                ui->getScrollController()->scrollPage(ScrollController::Down);
            } else {
                // horizontal scrolling
                ui->getScrollController()->scrollPage(ScrollController::Right);
            }
            break;
        case Qt::Key_Backspace:
            removeGapsPrecedingSelection(genuineCtrl ? 1 : -1);
            break;
        case Qt::Key_Insert:
        case Qt::Key_Space:
            // We can't use Command+Space on Mac OS X - it is reserved
            if(!isAlignmentLocked()) {
                emit si_startMaChanging();
                insertGapsBeforeSelection(genuineCtrl ? 1 : -1);
            }
            break;
        case Qt::Key_Shift:
            if (!selection.isEmpty()) {
                selectionStart = selection.topLeft();
                selectionEnd = selection.bottomRight();
            } else {
                selectionStart = editor->getCursorPosition();
                selectionEnd = editor->getCursorPosition();
            }
            break;
    }
    QWidget::keyPressEvent(e);
}

void MaEditorSequenceArea::keyReleaseEvent(QKeyEvent *ke) {
    if ((ke->key() == Qt::Key_Space || ke->key() == Qt::Key_Delete) && !isAlignmentLocked() && !ke->isAutoRepeat()) {
        emit si_stopMaChanging(true);
    }

    QWidget::keyReleaseEvent(ke);
}

void MaEditorSequenceArea::drawBackground(QPainter &) {

}

void MaEditorSequenceArea::insertGapsBeforeSelection(int countOfGaps) {
    CHECK(getEditor() != NULL,);
    CHECK(!selection.isEmpty(),);
    if (countOfGaps == -1) {
        countOfGaps = selection.width();
    }
    CHECK(countOfGaps > 0,);
    SAFE_POINT(isInRange(selection.topLeft()), tr("Top left corner of the selection has incorrect coords"), );
    SAFE_POINT(isInRange(QPoint(selection.x() + selection.width() - 1, selection.y() + selection.height() - 1)),
        tr("Bottom right corner of the selection has incorrect coords"), );

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    MultipleAlignmentObject *maObj = editor->getMaObject();
    if (maObj == NULL || maObj->isStateLocked()) {
        return;
    }
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os,);

    const MultipleAlignment& ma = maObj->getMultipleAlignment();
    if (selection.width() == ma->getLength() && selection.height() == ma->getNumRows()) {
        return;
    }

    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    maObj->insertGapByRowIndexList(selectedMaRows, selection.x(), countOfGaps);
    adjustReferenceLength(os);
    CHECK_OP(os,);
    moveSelection(countOfGaps, 0, true);
    if (!getSelection().isEmpty()) {
        ui->getScrollController()->scrollToMovedSelection(ScrollController::Right);
    }
}

void MaEditorSequenceArea::removeGapsPrecedingSelection(int countOfGaps) {
    const MaEditorSelection selectionBackup = selection;
    // check if selection exists
    if (selectionBackup.isEmpty()) {
        return;
    }

    const QPoint selectionTopLeftCorner(selectionBackup.topLeft());
    // don't perform the deletion if the selection is at the alignment start
    if (selectionTopLeftCorner.x() == 0 || countOfGaps < -1 || countOfGaps == 0) {
        return;
    }

    int removedRegionWidth = (countOfGaps == -1) ? selectionBackup.width() : countOfGaps;
    QPoint topLeftCornerOfRemovedRegion(selectionTopLeftCorner.x() - removedRegionWidth,
        selectionTopLeftCorner.y());
    if (0 > topLeftCornerOfRemovedRegion.x()) {
        removedRegionWidth -= qAbs(topLeftCornerOfRemovedRegion.x());
        topLeftCornerOfRemovedRegion.setX(0);
    }

    MultipleAlignmentObject *maObj = editor->getMaObject();
    if (NULL == maObj || maObj->isStateLocked()) {
        return;
    }

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);

    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    int countOfDeletedSymbols = maObj->deleteGapByRowIndexList(os, selectedMaRows, topLeftCornerOfRemovedRegion.x(), removedRegionWidth);

    // if some symbols were actually removed and the selection is not located
    // at the alignment end, then it's needed to move the selection
    // to the place of the removed symbols
    if (countOfDeletedSymbols > 0) {
        const MaEditorSelection newSelection(selectionBackup.x() - countOfDeletedSymbols,
            topLeftCornerOfRemovedRegion.y(), selectionBackup.width(),
            selectionBackup.height());
        setSelection(newSelection);
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
    renderer->drawSelection(painter);
    renderer->drawFocus(painter);
}

void MaEditorSequenceArea::updateColorAndHighlightSchemes() {
    Settings* s = AppContext::getSettings();
    if (!s || !editor){
        return;
    }
    MultipleAlignmentObject* maObj = editor->getMaObject();
    if (!maObj){
        return;
    }

    const DNAAlphabet* al = maObj->getAlphabet();
    if (!al){
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

void MaEditorSequenceArea::initColorSchemes(MsaColorSchemeFactory *defaultColorSchemeFactory) {
    MsaColorSchemeRegistry *msaColorSchemeRegistry = AppContext::getMsaColorSchemeRegistry();
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
    SAFE_POINT(hsf != NULL, "Highlight scheme factory is NULL", );

    MultipleAlignmentObject* maObj = editor->getMaObject();
    QVariantMap settings = highlightingScheme != NULL
            ? highlightingScheme->getSettings()
            : QVariantMap();
    delete highlightingScheme;

    highlightingScheme = hsf->create(this, maObj);
    highlightingScheme->applySettings(settings);

    MsaSchemesMenuBuilder::createAndFillHighlightingMenuActions(highlightingSchemeMenuActions, getEditor()->getMaObject()->getAlphabet()->getType(), this);
    QList<QAction *> tmpActions = QList<QAction *>() << highlightingSchemeMenuActions;
    foreach(QAction *action, tmpActions) {
        action->setChecked(action->data() == hsf->getId());
    }
}

MsaColorSchemeFactory * MaEditorSequenceArea::getDefaultColorSchemeFactory() const {
    MsaColorSchemeRegistry *msaColorSchemeRegistry = AppContext::getMsaColorSchemeRegistry();

    switch (editor->getMaObject()->getAlphabet()->getType()) {
    case DNAAlphabet_RAW:
        return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::EMPTY);
    case DNAAlphabet_NUCL:
        return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::UGENE_NUCL);
    case DNAAlphabet_AMINO:
        return msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::UGENE_AMINO);
    default:
        FAIL(tr("Unknown alphabet"), NULL);
    }
    return NULL;
}

MsaHighlightingSchemeFactory* MaEditorSequenceArea::getDefaultHighlightingSchemeFactory() const {
    MsaHighlightingSchemeRegistry *hsr = AppContext::getMsaHighlightingSchemeRegistry();
    MsaHighlightingSchemeFactory *hsf = hsr->getSchemeFactoryById(MsaHighlightingScheme::EMPTY);
    return hsf;
}

void MaEditorSequenceArea::getColorAndHighlightingIds(QString &csid, QString &hsid) {
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
    if (csf == NULL) {
        csid = getDefaultColorSchemeFactory()->getId();
    }
    MsaHighlightingSchemeFactory* hsf = hsr->getSchemeFactoryById(hsid);
    if (hsf == NULL) {
        hsid = getDefaultHighlightingSchemeFactory()->getId();
    }


    if (colorScheme != NULL && colorScheme->getFactory()->isAlphabetTypeSupported(atype)) {
        csid = colorScheme->getFactory()->getId();
    }
    if (highlightingScheme != NULL && highlightingScheme->getFactory()->isAlphabetTypeSupported(atype)) {
        hsid = highlightingScheme->getFactory()->getId();
    }
}

void MaEditorSequenceArea::applyColorScheme(const QString &id) {
    CHECK(ui->getEditor()->getMaObject() != NULL, );

    MsaColorSchemeFactory *factory = AppContext::getMsaColorSchemeRegistry()->getSchemeFactoryById(id);
    delete colorScheme;
    colorScheme = factory->create(this, ui->getEditor()->getMaObject());

    connect(factory, SIGNAL(si_factoryChanged()), SLOT(sl_colorSchemeFactoryUpdated()), Qt::UniqueConnection);
    connect(factory, SIGNAL(destroyed(QObject *)), SLOT(sl_setDefaultColorScheme()), Qt::UniqueConnection);

    QList<QAction *> tmpActions = QList<QAction *>() << colorSchemeMenuActions << customColorSchemeMenuActions;
    foreach (QAction *action, tmpActions) {
        action->setChecked(action->data() == id);
    }

    if (qobject_cast<MSAEditor*>(getEditor()) != NULL) { // to avoid setting of sanger scheme
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

void MaEditorSequenceArea::processCharacterInEditMode(QKeyEvent *e) {
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
            MainWindow *mainWindow = AppContext::getMainWindow();
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
    CHECK(getEditor() != NULL, );
    if (selection.isEmpty()) {
        return;
    }
    SAFE_POINT(isInRange(selection.topLeft()), "Incorrect selection is detected!", );
    MultipleAlignmentObject* maObj = editor->getMaObject();
    if (maObj == NULL || maObj->isStateLocked()) {
        return;
    }
    if (maObj->getNumRows() == 1 && maObj->getRow(selection.y())->getCoreLength() == 1 && newCharacter == U2Msa::GAP_CHAR) {
        exitFromEditCharacterMode();
        return;
    }

    const bool isGap = maObj->getRow(selection.y())->isGap(selection.x());
    GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, isGap, "Replace gap", editor->getFactoryId());
    GRUNTIME_NAMED_CONDITION_COUNTER(ccvar, ttvar, !isGap, "Replace character", editor->getFactoryId());

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );

    QList<int> selectedMaRows = getSelectedMaRowIndexes();
    int column = selection.x();
    for (int i = 0; i < selectedMaRows.size(); i++) {
        int row = selectedMaRows[i];
        maObj->replaceCharacter(column, row, newCharacter);
    }

    exitFromEditCharacterMode();
}

void MaEditorSequenceArea::exitFromEditCharacterMode() {
    if (maMode != ViewMode) {
        editModeAnimationTimer.stop();
        selectionColor = Qt::black;
        maMode = ViewMode;
        sl_updateActions();
        update();
    }
}

bool MaEditorSequenceArea::isCharacterAcceptable(const QString &text) const {
    static const QRegExp latinCharacterOrGap(QString("([A-Z]| |-|%1)").arg(emDash));
    return latinCharacterOrGap.exactMatch(text);
}

const QString &MaEditorSequenceArea::getInacceptableCharacterErrorMessage() const {
    static const QString message = tr("It is not possible to insert the character into the alignment. "
                                      "Please use a character from set A-Z (upper-case or lower-case) or the gap character ('Space', '-' or '%1').").arg(emDash);
    return message;
}

void MaEditorSequenceArea::deleteOldCustomSchemes() {
    qDeleteAll(customColorSchemeMenuActions);
    customColorSchemeMenuActions.clear();
}

void MaEditorSequenceArea::updateCollapseModel(const MaModificationInfo&) {

}

MaEditorSequenceArea::MaMode MaEditorSequenceArea::getModInfo() {
    return maMode;
}

} // namespace

