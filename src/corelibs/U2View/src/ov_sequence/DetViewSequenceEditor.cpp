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

#include "DetViewSequenceEditor.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>
#include <U2Core/ModifySequenceObjectTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/SequenceObjectContext.h>

#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "AnnotatedDNAView.h"
#include "DetView.h"

namespace U2 {

DetViewSequenceEditor::DetViewSequenceEditor(DetView *view)
    : cursorColor(Qt::black),
      animationTimer(this),
      view(view) {
    editAction = new QAction(tr("Switch on the editing mode"), this);
    editAction->setIcon(QIcon(":core/images/edit.png"));
    editAction->setObjectName("edit_sequence_action");
    editAction->setCheckable(true);
    editAction->setDisabled(view->getSequenceObject()->isStateLocked());
    connect(editAction, SIGNAL(triggered(bool)), SLOT(sl_editMode(bool)));
    connect(view->getSequenceObject(), SIGNAL(si_lockedStateChanged()), SLOT(sl_objectLockStateChanged()));

    reset();
    connect(&animationTimer, SIGNAL(timeout()), SLOT(sl_changeCursorColor()));
    setParent(view);
}

DetViewSequenceEditor::~DetViewSequenceEditor() {
    view->removeEventFilter(this);
    animationTimer.stop();
}

void DetViewSequenceEditor::reset() {
    cursor = view->getVisibleRange().startPos;
}

bool DetViewSequenceEditor::isEditMode() const {
    SAFE_POINT(editAction != NULL, "editAction is NULL", false);
    return editAction->isChecked();
}

bool DetViewSequenceEditor::eventFilter(QObject *, QEvent *event) {
    CHECK(!view->getSequenceObject()->isStateLocked(), false)

    SequenceObjectContext *ctx = view->getSequenceContext();
    const QList<ADVSequenceWidget *> list = ctx->getSequenceWidgets();
    CHECK(!list.isEmpty(), false);
    ADVSequenceWidget *wgt = list.first();
    AnnotatedDNAView *dnaView = wgt->getAnnotatedDNAView();
    QAction *a = dnaView->removeAnnsAndQsAction;

    switch (event->type()) {
    case QEvent::FocusOut:
        // return delete
        a->setShortcut(QKeySequence(Qt::Key_Delete));
        return true;
    case QEvent::FocusIn:
        // block delete again
        a->setShortcut(QKeySequence());
        return true;

        // TODO_SVEDIT: shift button!
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove: {
        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        SAFE_POINT(mouseEvent != NULL, "Failed to cast QEvent to QMouseEvent", true);

        if (mouseEvent->buttons() & Qt::LeftButton) {
            qint64 pos = view->getRenderArea()->coordToPos(view->toRenderAreaPoint(mouseEvent->pos()));
            setCursor(pos);    // use navigate and take shift into account
        }
        return false;
    }

        // TODO_SVEDIT: separate methods
    case QEvent::KeyPress: {
        // set cursor position
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
        SAFE_POINT(keyEvent != NULL, "Failed to cast QEvent to QKeyEvent", true);

        int key = keyEvent->key();
        bool shiftPressed = keyEvent->modifiers().testFlag(Qt::ShiftModifier);
        switch (key) {
        case Qt::Key_Left:
            navigate(cursor - 1, shiftPressed);
            break;
        case Qt::Key_Right:
            navigate(cursor + 1, shiftPressed);
            break;
        case Qt::Key_Up:
            if (view->isWrapMode()) {
                navigate(cursor - view->getSymbolsPerLine(), shiftPressed);
            }
            break;
        case Qt::Key_Down:
            if (view->isWrapMode()) {
                navigate(cursor + view->getSymbolsPerLine(), shiftPressed);
            }
            break;
        case Qt::Key_Home:
            navigate(0, shiftPressed);
            break;
        case Qt::Key_End:
            navigate(view->getSequenceLength(), shiftPressed);
            break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            deleteChar(key);
            break;
        case Qt::Key_Space:
            insertChar(U2Msa::GAP_CHAR);
            break;
        default:
            if (key >= Qt::Key_A && key <= Qt::Key_Z) {
                if (keyEvent->modifiers() == Qt::NoModifier) {
                    insertChar(key);
                }
            }
        }
        return true;
    }
    default:
        return false;
    }
}

void DetViewSequenceEditor::setCursor(int newPos) {
    CHECK(newPos >= 0 && newPos <= view->getSequenceLength(), );
    if (cursor != newPos) {
        cursor = newPos;
        view->ensurePositionVisible(cursor);
        view->update();
    }
}

void DetViewSequenceEditor::navigate(int newPos, bool shiftPressed) {
    CHECK(newPos != cursor, );
    newPos = qBound(0, newPos, (int)view->getSequenceLength());

    DNASequenceSelection *selection = view->getSequenceContext()->getSequenceSelection();
    if (shiftPressed) {
        int extension = qAbs(cursor - newPos);
        if (selection->isEmpty()) {
            // if selection is empty - start a new one!
            selection->setRegion(U2Region(qMin(cursor, newPos), extension));
        } else {
            // expand selection
            U2Region r = selection->getSelectedRegions().first();
            selection->clear();

            if (r.contains(newPos) || (r.endPos() == newPos)) {
                if (r.length - extension != 0) {
                    // shorten the selection
                    if (r.startPos == cursor) {
                        selection->setRegion(U2Region(newPos, r.length - extension));
                    } else {
                        selection->setRegion(U2Region(r.startPos, r.length - extension));
                    }
                }
            } else {
                if (newPos < r.startPos && cursor == r.endPos()) {
                    selection->setRegion(U2Region(newPos, r.startPos - newPos));
                } else if (newPos > r.endPos() && cursor == r.startPos) {
                    selection->setRegion(U2Region(r.endPos(), newPos - r.endPos()));
                } else {
                    if (r.startPos == cursor) {
                        selection->setRegion(U2Region(newPos, r.length + extension));
                    } else {
                        selection->setRegion(U2Region(r.startPos, r.length + extension));
                    }
                }
            }
        }
        setCursor(newPos);
    } else {
        selection->clear();
        setCursor(newPos);
    }
}

void DetViewSequenceEditor::insertChar(int character) {
    U2SequenceObject *seqObj = view->getSequenceObject();
    SAFE_POINT(seqObj != NULL, "SeqObject is NULL", );
    CHECK(seqObj->getAlphabet()->contains(character), );    // TODO_SVEDIT: support alphabet changing, separate issue
    cancelSelectionResizing();

    const DNASequence seq(QByteArray(1, character));
    U2Region r;
    SequenceObjectContext *ctx = view->getSequenceContext();
    SAFE_POINT(ctx != NULL, "SequenceObjectContext", );
    if (ctx->getSequenceSelection()->isEmpty()) {
        r = U2Region(cursor, 0);
    } else {
        // ignore multuselection for now
        r = ctx->getSequenceSelection()->getSelectedRegions().first();
        ctx->getSequenceSelection()->clear();
    }
    modifySequence(seqObj, r, seq);

    setCursor(r.startPos + 1);
}

void DetViewSequenceEditor::deleteChar(int key) {
    CHECK(key == Qt::Key_Backspace || key == Qt::Key_Delete, );
    U2SequenceObject *seqObj = view->getSequenceObject();
    SAFE_POINT(seqObj != NULL, "SeqObject is NULL", );
    cancelSelectionResizing();

    U2Region regionToRemove;
    SequenceObjectContext *ctx = view->getSequenceContext();
    if (!ctx->getSequenceSelection()->isEmpty()) {
        setCursor(ctx->getSequenceSelection()->getSelectedRegions().first().startPos);
        QVector<U2Region> regions = ctx->getSequenceSelection()->getSelectedRegions();
        ctx->getSequenceSelection()->clear();
        if (regions.size() != 1) {
            qSort(regions);
            for (int i = 0; i < regions.size(); i++) {
                // can cause problems
                modifySequence(seqObj, regions[i], DNASequence());
            }
            return;
        } else {
            regionToRemove = regions.first();
        }
    } else {
        if (key == Qt::Key_Backspace) {
            CHECK(cursor > 0, );
            regionToRemove = U2Region(cursor - 1, 1);
        } else {
            CHECK(cursor < seqObj->getSequenceLength(), );
            regionToRemove = U2Region(cursor, 1);
        }
        setCursor(key == Qt::Key_Backspace ? cursor - 1 : cursor);
    }

    if (regionToRemove.length == view->getSequenceLength()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Delete the sequence"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Would you like to completely remove the sequence?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        int res = msgBox.exec();
        if (res == QMessageBox::No) {
            return;
        }
        Document *doc = seqObj->getDocument();
        SAFE_POINT(doc != NULL, "Document is NULL", );
        doc->removeObject(seqObj);
        return;
    }
    CHECK(!regionToRemove.isEmpty(), );
    modifySequence(seqObj, regionToRemove, DNASequence());
}

void DetViewSequenceEditor::modifySequence(U2SequenceObject *seqObj, const U2Region &region, const DNASequence &sequence) {
    Settings *s = AppContext::getSettings();
    U1AnnotationUtils::AnnotationStrategyForResize strategy =
        (U1AnnotationUtils::AnnotationStrategyForResize)s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_ANNOTATION_STRATEGY,
                                                                    U1AnnotationUtils::AnnotationStrategyForResize_Resize)
            .toInt();

    U2OpStatusImpl os;
    seqObj->replaceRegion(region, sequence, os);
    FixAnnotationsUtils::fixAnnotations(&os, seqObj, region, sequence, s->getValue(QString(SEQ_EDIT_SETTINGS_ROOT) + SEQ_EDIT_SETTINGS_RECALC_QUALIFIERS, false).toBool(), strategy);
    SAFE_POINT_OP(os, );
    ADVSequenceObjectContext *context = qobject_cast<ADVSequenceObjectContext *>(view->getSequenceContext());
    SAFE_POINT(context != NULL, L10N::nullPointerError("ADVSequenceObjectContext"), );
    context->getAnnotatedDNAView()->updateAutoAnnotations();
}

void DetViewSequenceEditor::cancelSelectionResizing() {
    view->cancelSelectionResizing();
}

void DetViewSequenceEditor::sl_editMode(bool active) {
    SequenceObjectContext *ctx = view->getSequenceContext();
    const QList<ADVSequenceWidget *> list = ctx->getSequenceWidgets();
    SAFE_POINT(!list.isEmpty(), "seq wgts list is empty", );
    ADVSequenceWidget *wgt = list.first();
    AnnotatedDNAView *dnaView = wgt->getAnnotatedDNAView();
    if (active) {
        // deactivate Delete shortcut
        dnaView->removeAnnsAndQsAction->setShortcut(QKeySequence());
        reset();
        view->installEventFilter(this);
        // Make detailed view visible
        ADVSingleSequenceWidget *sequenceWidget = qobject_cast<ADVSingleSequenceWidget *>(wgt);
        if (sequenceWidget) {
            sequenceWidget->setDetViewCollapsed(false);
        }
        animationTimer.start(500);
    } else {
        editAction->setDisabled(view->getSequenceObject()->isStateLocked());
        view->removeEventFilter(this);
        dnaView->removeAnnsAndQsAction->setShortcut(QKeySequence(Qt::Key_Delete));
        animationTimer.stop();
        view->update();
    }
}

void DetViewSequenceEditor::sl_changeCursorColor() {
    cursorColor = (cursorColor == QColor(Qt::black)) ? Qt::darkGray : Qt::black;
    view->update();
}

void DetViewSequenceEditor::sl_objectLockStateChanged() {
    editAction->setDisabled(!isEditMode() && view->getSequenceObject()->isStateLocked());
}

void DetViewSequenceEditor::sl_paste(Task *task) {
    PasteTask *pasteTask = qobject_cast<PasteTask *>(task);
    CHECK(pasteTask != NULL && !pasteTask->isCanceled(), );

    const QList<Document *> &docs = pasteTask->getDocuments();
    CHECK(docs.length() != 0, );

    U2OpStatusImpl os;
    const QList<DNASequence> &sequences = PasteUtils::getSequences(docs, os);
    if (sequences.isEmpty()) {
        coreLog.error(tr("No sequences detected in the pasted content."));
        return;
    }
    DNASequence seq;
    foreach (const DNASequence &dnaObj, sequences) {
        if (seq.alphabet == NULL) {
            seq.alphabet = dnaObj.alphabet;
        }
        const DNAAlphabet *newAlphabet = U2AlphabetUtils::deriveCommonAlphabet(dnaObj.alphabet, seq.alphabet);
        if (newAlphabet != NULL) {
            seq.alphabet = newAlphabet;
            seq.seq.append(dnaObj.seq);
        }
    }
    U2SequenceObject *seqObj = view->getSequenceObject();
    SAFE_POINT(seqObj != NULL, "SeqObject is NULL", );
    if (seqObj->getAlphabet()->getId() != seq.alphabet->getId()) {
        coreLog.error(tr("The sequence & clipboard content have different alphabet"));
        return;
    }

    modifySequence(seqObj, U2Region(cursor, 0), seq);

    setCursor(cursor + seq.length());
}

}    // namespace U2
