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

#include <QToolButton>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>

#include "McaEditorSequenceArea.h"
#include "helpers/MaAmbiguousCharactersController.h"
#include "helpers/ScrollController.h"
#include "helpers/RowHeightController.h"
#include "ov_msa/McaEditorConsensusArea.h"
#include "ov_sequence/SequenceObjectContext.h"
#include "view_rendering/SequenceWithChromatogramAreaRenderer.h"

namespace U2 {

McaEditorSequenceArea::McaEditorSequenceArea(McaEditorWgt *ui, GScrollBar *hb, GScrollBar *vb)
    : MaEditorSequenceArea(ui, hb, vb) {
    initRenderer();

    setObjectName("mca_editor_sequence_area");

    // TEST - remove the variable after fix
    editingEnabled = true;

    showQVAction = new QAction(tr("Show quality bars"), this);
    showQVAction->setIcon(QIcon(":chroma_view/images/bars.png"));
    showQVAction->setCheckable(true);
    // SANGER_TODO: check quality
//    showQVAction->setChecked(chroma.hasQV);
//    showQVAction->setEnabled(chroma.hasQV);
    connect(showQVAction, SIGNAL(toggled(bool)), SLOT(sl_completeUpdate()));

    showAllTraces = new QAction(tr("Show all"), this);
    connect(showAllTraces, SIGNAL(triggered()), SLOT(sl_showAllTraces()));
    connect(editor, SIGNAL(si_buildStaticToolbar(GObjectView *, QToolBar *)), SLOT(sl_buildStaticToolbar(GObjectView *, QToolBar *)));

    traceActionsMenu = new QMenu(tr("Show/hide trace"), this);
    traceActionsMenu->setObjectName("traceActionsMenu");
    traceActionsMenu->addAction( createToggleTraceAction("A") );
    traceActionsMenu->addAction( createToggleTraceAction("C") );
    traceActionsMenu->addAction( createToggleTraceAction("G") );
    traceActionsMenu->addAction( createToggleTraceAction("T") ) ;
    traceActionsMenu->addSeparator();
    traceActionsMenu->addAction(showAllTraces);

    insertAction = new QAction(tr("Insert character/gap"), this);
    insertAction->setShortcut(Qt::SHIFT + Qt::Key_I);
    connect(insertAction, SIGNAL(triggered()), SLOT(sl_addInsertion()));
    addAction(insertAction);

    replaceCharacterAction->setText(tr("Replace character/gap"));

    removeGapBeforeSelectionAction = new QAction(tr("Remove gap at the left"), this);
    removeGapBeforeSelectionAction->setShortcut(Qt::Key_Backspace);
    connect(removeGapBeforeSelectionAction, SIGNAL(triggered()), SLOT(sl_removeGapBeforeSelection()));
    addAction(removeGapBeforeSelectionAction);

    removeColumnsOfGapsAction = new QAction(tr("Remove all columns of gaps"), this);
    removeColumnsOfGapsAction->setObjectName("remove_columns_of_gaps");
    removeColumnsOfGapsAction->setShortcut(Qt::SHIFT + Qt::Key_Delete);
    connect(removeColumnsOfGapsAction, SIGNAL(triggered()), SLOT(sl_removeColumnsOfGaps()));
    addAction(removeColumnsOfGapsAction);

    trimLeftEndAction = new QAction(tr("Trim left end"), this);
    trimLeftEndAction->setObjectName("trim_left_end");
    trimLeftEndAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Backspace));
    connect(trimLeftEndAction, SIGNAL(triggered()), SLOT(sl_trimLeftEnd()));
    addAction(trimLeftEndAction);

    trimRightEndAction = new QAction(tr("Trim right end"), this);
    trimRightEndAction->setObjectName("trim_right_end");
    trimRightEndAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Delete));
    connect(trimRightEndAction, SIGNAL(triggered()), SLOT(sl_trimRightEnd()));
    addAction(trimRightEndAction);

    fillWithGapsinsSymAction->setText(tr("Insert gap"));
    fillWithGapsinsSymAction->setShortcut(Qt::Key_Space);
    fillWithGapsinsSymAction->setShortcutContext(Qt::WidgetShortcut);

    scaleBar = new ScaleBar(Qt::Horizontal);
    scaleBar->setRange(100, 1000);
    scaleBar->setTickInterval(100);
    scaleBar->setObjectName("peak_height_slider");

    scaleBar->getPlusAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up));
    addAction(scaleBar->getPlusAction());
    GUIUtils::updateButtonToolTip(scaleBar->getPlusButton(), scaleBar->getPlusAction()->shortcut());

    scaleBar->getMinusAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down));
    addAction(scaleBar->getMinusAction());
    GUIUtils::updateButtonToolTip(scaleBar->getMinusButton(), scaleBar->getMinusAction()->shortcut());

    scaleAction = NULL;

    ambiguousCharactersController = new MaAmbiguousCharactersController(ui);
    addAction(ambiguousCharactersController->getPreviousAction());
    addAction(ambiguousCharactersController->getNextAction());

    SequenceWithChromatogramAreaRenderer* r = qobject_cast<SequenceWithChromatogramAreaRenderer*>(renderer);
    scaleBar->setValue(r->getScaleBarValue());
    connect(scaleBar, SIGNAL(valueChanged(int)), SLOT(sl_setRenderAreaHeight(int)));

    updateColorAndHighlightSchemes();
    sl_updateActions();
}

void McaEditorSequenceArea::adjustReferenceLength(U2OpStatus& os) {
    McaEditor* mcaEditor = getEditor();
    qint64 newLength = mcaEditor->getMaObject()->getLength();
    qint64 currentLength = mcaEditor->getReferenceContext()->getSequenceLength();
    if (newLength > currentLength) {
        U2DataId id = mcaEditor->getMaObject()->getEntityRef().entityId;
        U2Region region(currentLength, 0);
        QByteArray insert(newLength - currentLength, U2Msa::GAP_CHAR);
        DNASequence seq(insert);
        mcaEditor->getReferenceContext()->getSequenceObject()->replaceRegion(id, region, seq, os);
    }
}

MaAmbiguousCharactersController *McaEditorSequenceArea::getAmbiguousCharactersController() const {
    return ambiguousCharactersController;
}

QMenu *McaEditorSequenceArea::getTraceActionsMenu() const {
    return traceActionsMenu;
}

QAction *McaEditorSequenceArea::getIncreasePeaksHeightAction() const {
    return scaleBar->getPlusAction();
}

QAction *McaEditorSequenceArea::getDecreasePeaksHeightAction() const {
    return scaleBar->getMinusAction();
}

QAction *McaEditorSequenceArea::getInsertAction() const {
    return insertAction;
}

QAction *McaEditorSequenceArea::getInsertGapAction() const {
    return fillWithGapsinsSymAction;
}

QAction *McaEditorSequenceArea::getRemoveGapBeforeSelectionAction() const {
    return removeGapBeforeSelectionAction;
}

QAction *McaEditorSequenceArea::getRemoveColumnsOfGapsAction() const {
    return removeColumnsOfGapsAction;
}

QAction *McaEditorSequenceArea::getTrimLeftEndAction() const {
    return trimLeftEndAction;
}

QAction *McaEditorSequenceArea::getTrimRightEndAction() const {
    return trimRightEndAction;
}

void McaEditorSequenceArea::setSelection(const MaEditorSelection &sel) {
    // Its only possible to select 1 character (width = 1) or multiple rows with no character (width = 0).
    CHECK((sel.width() == 1 && sel.height() == 1) || sel.width() == 0,);
    if (sel.width() == 1 && getEditor()->getMaObject()->getMca()->isTrailingOrLeadingGap(sel.y(), sel.x())) {
        // clear selection if gap is clicked
        emit si_clearReferenceSelection();
        MaEditorSequenceArea::setSelection(MaEditorSelection());
        return;
    }
    if (sel.isEmpty()) {
        emit si_clearReferenceSelection();
    }
    MaEditorSequenceArea::setSelection(sel);
}

void McaEditorSequenceArea::moveSelection(int dx, int dy, bool) {
    CHECK(selection.width() == 1 && selection.height() == 1, );

    const MultipleChromatogramAlignment mca = getEditor()->getMaObject()->getMca();
    if (dy == 0 && mca->isTrailingOrLeadingGap(selection.y(), selection.x() + dx)) {
        return;
    }

    int nextRowToSelect = selection.y() + dy;
    if (dy != 0) {
        bool noRowAvailabe = true;
        for ( ; nextRowToSelect >= 0 && nextRowToSelect < ui->getCollapseModel()->getViewRowCount(); nextRowToSelect += dy) {
            if (!mca->isTrailingOrLeadingGap(ui->getCollapseModel()->getMaRowIndexByViewRowIndex(nextRowToSelect), selection.x() + dx)) {
                noRowAvailabe  = false;
                break;
            }
        }
        CHECK(!noRowAvailabe, );
    }

    QPoint newSelectedPoint(selection.x() + dx, nextRowToSelect);
    MaEditorSelection newSelection(newSelectedPoint, selection.width(), selection.height());
    setSelection(newSelection);
    const QPoint& cursorPosition = editor->getCursorPosition();
    editor->setCursorPosition(QPoint(cursorPosition.x() + dx, nextRowToSelect));
    ui->getScrollController()->scrollToMovedSelection(dx, dy);
}

void McaEditorSequenceArea::sl_backgroundSelectionChanged() {
    update();
}

void McaEditorSequenceArea::sl_showHideTrace() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, "Selection of a 'Show / hide trace' item", editor->getFactoryId());
    QAction* traceAction = qobject_cast<QAction*> (sender());

    if (!traceAction) {
        return;
    }

    if (traceAction->text() == "A") {
        settings.drawTraceA = traceAction->isChecked();
    } else if (traceAction->text() == "C") {
        settings.drawTraceC = traceAction->isChecked();
    } else if(traceAction->text() == "G") {
        settings.drawTraceG = traceAction->isChecked();
    } else if(traceAction->text() == "T") {
        settings.drawTraceT = traceAction->isChecked();
    } else {
        assert(0);
    }

    sl_completeUpdate();
}

void McaEditorSequenceArea::sl_showAllTraces() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, "Selection of a 'Show / hide trace' item", editor->getFactoryId());
    settings.drawTraceA = true;
    settings.drawTraceC = true;
    settings.drawTraceG = true;
    settings.drawTraceT = true;
    QList<QAction*> actions = traceActionsMenu->actions();
    foreach(QAction* action, actions) {
        action->setChecked(true);
    }
    sl_completeUpdate();
}

void McaEditorSequenceArea::sl_setRenderAreaHeight(int k) {
    //k = chromaMax
    SequenceWithChromatogramAreaRenderer* r = qobject_cast<SequenceWithChromatogramAreaRenderer*>(renderer);
    GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, r->getAreaHeight() < k, "Increase peaks height", editor->getFactoryId());
    GRUNTIME_NAMED_CONDITION_COUNTER(ccvar, ttvar, r->getAreaHeight() > k, "Decrease peaks height", editor->getFactoryId());
    r->setAreaHeight(k);
    sl_completeUpdate();
}

void McaEditorSequenceArea::sl_buildStaticToolbar(GObjectView * /*v*/, QToolBar *t) {
    if (scaleAction != NULL) {
        t->addAction(scaleAction);
    } else {
        scaleAction = t->addWidget(scaleBar);
    }

    t->addSeparator();
    t->addAction(ambiguousCharactersController->getPreviousAction());
    t->addAction(ambiguousCharactersController->getNextAction());
    McaEditorConsensusArea* consensusArea = getEditor()->getUI()->getConsensusArea();
    consensusArea->buildStaticToolbar(t);

    t->addSeparator();
    t->addAction(ui->getUndoAction());
    t->addAction(ui->getRedoAction());
}

void McaEditorSequenceArea::sl_addInsertion() {
    maMode = InsertCharMode;
    editModeAnimationTimer.start(500);
    sl_updateActions();
}

void McaEditorSequenceArea::sl_removeGapBeforeSelection() {
    GCOUNTER(cvar, tvar, "Remove gap at the left");
    emit si_startMaChanging();
    removeGapsPrecedingSelection(1);
    emit si_stopMaChanging(true);
}

void McaEditorSequenceArea::sl_removeColumnsOfGaps() {
    GCOUNTER(cvar, tvar, "Remove all columns of gaps");
    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(editor->getMaObject()->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );
    editor->getMaObject()->deleteColumnsWithGaps(os);
}

void McaEditorSequenceArea::sl_trimLeftEnd() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, "Trim left end", editor->getFactoryId());
    trimRowEnd(MultipleChromatogramAlignmentObject::Left);
}

void McaEditorSequenceArea::sl_trimRightEnd() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, "Trim right end", editor->getFactoryId());
    trimRowEnd(MultipleChromatogramAlignmentObject::Right);
}

void McaEditorSequenceArea::sl_updateActions() {
    MultipleAlignmentObject* maObj = editor->getMaObject();
    SAFE_POINT(NULL != maObj, "MaObj is NULL", );

    const bool readOnly = maObj->isStateLocked();
    const bool canEditAlignment = !readOnly && !isAlignmentEmpty();
    const bool canEditSelectedArea = canEditAlignment && !selection.isEmpty();
    const bool isEditing = (maMode != ViewMode);
    const bool isSingleSymbolSelected = (selection.width() == 1 && selection.height() == 1);
    const bool hasGapBeforeSelection = (!selection.isEmpty() && selection.x() > 0 && maObj->getMultipleAlignment()->isGap(selection.y(), selection.x() - 1));

    ui->getDelSelectionAction()->setEnabled(canEditSelectedArea);
    updateTrimActions(canEditSelectedArea);
    insertAction->setEnabled(canEditSelectedArea && isSingleSymbolSelected && !isEditing);
    replaceCharacterAction->setEnabled(canEditSelectedArea && isSingleSymbolSelected && !isEditing);
    fillWithGapsinsSymAction->setEnabled(canEditSelectedArea && isSingleSymbolSelected && !isEditing);
    removeGapBeforeSelectionAction->setEnabled(hasGapBeforeSelection && !isEditing && canEditAlignment);
    removeColumnsOfGapsAction->setEnabled(canEditAlignment);
}

void McaEditorSequenceArea::trimRowEnd(MultipleChromatogramAlignmentObject::TrimEdge edge) {
    MultipleChromatogramAlignmentObject* mcaObj = getEditor()->getMaObject();
    QList<int> maRows = getSelectedMaRowIndexes();
    SAFE_POINT(!maRows.isEmpty() && maRows.size() == 1, "Incorrect selection", )
    int maRowIndex = maRows[0];

    U2OpStatus2Log os;
    U2UseCommonUserModStep userModStep(mcaObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );

    SAFE_POINT(!getSelection().isEmpty(), "selection is empty", );
    int currentPos = getSelection().x();

    mcaObj->trimRow(maRowIndex, currentPos, os, edge);
    CHECK_OP(os, );
}

void McaEditorSequenceArea::updateTrimActions(bool isEnabled) {
    trimLeftEndAction->setEnabled(isEnabled);
    trimRightEndAction->setEnabled(isEnabled);

    CHECK(isEnabled, );
    int maRowIndex = getTopSelectedMaRow();
    CHECK(maRowIndex >= 0,);

    MultipleAlignmentRow row = editor->getMaObject()->getRow(maRowIndex);
    int start = row->getCoreStart();
    int end = row->getCoreEnd();
    int currentSelection = getSelection().x();
    if (start == currentSelection) {
        trimLeftEndAction->setEnabled(false);
    }
    if (end - 1 == currentSelection) {
        trimRightEndAction->setEnabled(false);
    }
}

void McaEditorSequenceArea::initRenderer() {
    renderer = new SequenceWithChromatogramAreaRenderer(ui, this);
}

void McaEditorSequenceArea::drawBackground(QPainter &painter) {
    SequenceWithChromatogramAreaRenderer* r = qobject_cast<SequenceWithChromatogramAreaRenderer*>(renderer);
    SAFE_POINT(r != NULL, "Wrong renderer: fail to cast renderer to SequenceWithChromatogramAreaRenderer", );
    r->drawReferenceSelection(painter);
    r->drawNameListSelection(painter);
}

void McaEditorSequenceArea::getColorAndHighlightingIds(QString &csid, QString &hsid) {
    csid = MsaColorScheme::UGENE_SANGER_NUCL;
    hsid = MsaHighlightingScheme::DISAGREEMENTS;
}

QAction* McaEditorSequenceArea::createToggleTraceAction(const QString& actionName) {
    QAction* showTraceAction = new QAction(actionName, this);
    showTraceAction->setCheckable(true);
    showTraceAction->setChecked(true);
    showTraceAction->setEnabled(true);
    connect(showTraceAction, SIGNAL(triggered(bool)), SLOT(sl_showHideTrace()));

    return showTraceAction;
}

void McaEditorSequenceArea::insertChar(char newCharacter) {
    CHECK(maMode == InsertCharMode, );
    CHECK(getEditor() != NULL, );
    CHECK(!selection.isEmpty(), );

    assert(isInRange(selection.topLeft()));
    assert(isInRange(QPoint(selection.x() + selection.width() - 1, selection.y() + selection.height() - 1)));

    MultipleChromatogramAlignmentObject* maObj = getEditor()->getMaObject();
    CHECK(maObj != NULL && !maObj->isStateLocked(), );

    // if this method was invoked during a region shifting
    // then shifting should be canceled
    cancelShiftTracking();

    U2OpStatusImpl os;
    U2UseCommonUserModStep userModStep(maObj->getEntityRef(), os);
    Q_UNUSED(userModStep);
    SAFE_POINT_OP(os, );

    int xSelection = selection.x();
    maObj->changeLength(os, maObj->getLength() + 1);
    maObj->insertCharacter(selection.y(), xSelection, newCharacter);

    GRUNTIME_NAMED_CONDITION_COUNTER(cvar, tvar, newCharacter == U2Msa::GAP_CHAR, "Insert gap into a new column", editor->getFactoryId());
    GRUNTIME_NAMED_CONDITION_COUNTER(ccvar, ttvar, newCharacter != U2Msa::GAP_CHAR, "Insert character into a new column", editor->getFactoryId());

    // insert char into the reference
    U2SequenceObject* ref = getEditor()->getMaObject()->getReferenceObj();
    U2Region region = U2Region(xSelection, 0);
    ref->replaceRegion(maObj->getEntityRef().entityId, region, DNASequence(QByteArray(1, U2Msa::GAP_CHAR)), os);
    SAFE_POINT_OP(os, );

    exitFromEditCharacterMode();
}

bool McaEditorSequenceArea::isCharacterAcceptable(const QString &text) const {
    static const QString alphabetCharacters = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED())->getAlphabetChars();
    static const QRegExp dnaExtendedCharacterOrGap(QString("([%1]| |-|%2)").arg(alphabetCharacters).arg(emDash));
    return dnaExtendedCharacterOrGap.exactMatch(text);
}

const QString &McaEditorSequenceArea::getInacceptableCharacterErrorMessage() const {
    static const QString message = tr("It is not possible to insert the character into the alignment. "
                                      "Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character ('Space', '-' or '%1').").arg(emDash);
    return message;
}

McaEditorWgt *McaEditorSequenceArea::getMcaEditorWgt() const {
    return qobject_cast<McaEditorWgt *>(ui);
}

void McaEditorSequenceArea::updateCollapseModel(const MaModificationInfo& modInfo) {
    if (!modInfo.rowListChanged) {
        return;
    }
    MultipleAlignmentObject* maObject = getEditor()->getMaObject();
    MaCollapseModel* collapseModel = ui->getCollapseModel();
    QSet<int> expandedGroupIndexes;
    for (int i = 0, n = collapseModel->getGroupCount(); i < n; i++) {
        const MaCollapsibleGroup* group = collapseModel->getCollapsibleGroup(i);
        if (!group->isCollapsed) {
            qint64 rowId = group->maRowIds[0];
            expandedGroupIndexes << maObject->getRowPosById(rowId);
        }
    }
    collapseModel->reset(getEditor()->getMaRowIds(), expandedGroupIndexes);
}

} // namespace
