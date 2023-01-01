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

#include "ADVClipboard.h"

#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QTextStream>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/GUIUtils.h>

#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "AnnotatedDNAView.h"

#ifdef Q_OS_WIN
#    include <Windows.h>
#endif

namespace U2 {

const QString ADVClipboard::COPY_FAILED_MESSAGE = QApplication::translate("ADVClipboard", "Cannot put sequence data into the clipboard buffer.\n"
                                                                                          "Probably, the data are too big.");
ADVClipboard::ADVClipboard(AnnotatedDNAView* c)
    : QObject(c), ctx(c) {
    // TODO: listen seqadded/seqremoved!!

    connect(ctx, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget*, ADVSequenceWidget*)), SLOT(sl_onActiveSequenceChanged()));

    foreach (ADVSequenceObjectContext* sCtx, ctx->getSequenceContexts()) {
        connectSequence(sCtx);
    }

    copySequenceAction = new QAction(QIcon(":/core/images/copy_sequence.png"), tr("Copy selected sequence"), this);
    copySequenceAction->setObjectName("Copy sequence");
    copySequenceAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    connect(copySequenceAction, SIGNAL(triggered()), SLOT(sl_copySequence()));

    copyComplementSequenceAction = new QAction(QIcon(":/core/images/copy_complement_sequence.png"), tr("Copy selected complementary 5'-3' sequence"), this);
    copyComplementSequenceAction->setObjectName("Copy reverse complement sequence");
    copyComplementSequenceAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    connect(copyComplementSequenceAction, SIGNAL(triggered()), SLOT(sl_copyComplementSequence()));

    copyTranslationAction = new QAction(QIcon(":/core/images/copy_translation.png"), tr("Copy amino acids"), this);
    copyTranslationAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    copyTranslationAction->setObjectName(ADV_COPY_TRANSLATION_ACTION);
    connect(copyTranslationAction, SIGNAL(triggered()), SLOT(sl_copyTranslation()));

    copyComplementTranslationAction = new QAction(QIcon(":/core/images/copy_complement_translation.png"), tr("Copy amino acids of complementary 5'-3' strand"), this);
    copyComplementTranslationAction->setObjectName("Copy reverse complement translation");
    copyComplementTranslationAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    connect(copyComplementTranslationAction, SIGNAL(triggered()), SLOT(sl_copyComplementTranslation()));

    copyAnnotationSequenceAction = new QAction(QIcon(":/core/images/copy_annotation_sequence.png"), tr("Copy annotation sequence"), this);
    copyAnnotationSequenceAction->setObjectName("action_copy_annotation_sequence");
    connect(copyAnnotationSequenceAction, SIGNAL(triggered()), SLOT(sl_copyAnnotationSequence()));

    copyAnnotationSequenceTranslationAction = new QAction(QIcon(":/core/images/copy_annotation_translation.png"), tr("Copy annotation amino acids"), this);
    copyAnnotationSequenceTranslationAction->setObjectName("Copy annotation sequence translation");
    connect(copyAnnotationSequenceTranslationAction, SIGNAL(triggered()), SLOT(sl_copyAnnotationSequenceTranslation()));

    copyQualifierAction = new QAction(QIcon(":/core/images/copy_qualifier.png"), tr("Copy qualifier text"), this);
    copyQualifierAction->setEnabled(false);

    pasteSequenceAction = createPasteSequenceAction(this);
    updateActions();
}

QAction* ADVClipboard::getCopySequenceAction() const {
    return copySequenceAction;
}

QAction* ADVClipboard::getCopyComplementAction() const {
    return copyComplementSequenceAction;
}

QAction* ADVClipboard::getCopyTranslationAction() const {
    return copyTranslationAction;
}

QAction* ADVClipboard::getCopyComplementTranslationAction() const {
    return copyComplementTranslationAction;
}

QAction* ADVClipboard::getCopyAnnotationSequenceAction() const {
    return copyAnnotationSequenceAction;
}

QAction* ADVClipboard::getCopyAnnotationSequenceTranslationAction() const {
    return copyAnnotationSequenceTranslationAction;
}

QAction* ADVClipboard::getCopyQualifierAction() const {
    return copyQualifierAction;
}

QAction* ADVClipboard::getPasteSequenceAction() const {
    return pasteSequenceAction;
}

void ADVClipboard::connectSequence(ADVSequenceObjectContext* sCtx) {
    connect(sCtx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(sl_onDNASelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));

    connect(sCtx->getAnnotatedDNAView()->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)),
            SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&)));
}

void ADVClipboard::sl_onDNASelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&) {
    updateActions();
}

void ADVClipboard::sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&) {
    updateActions();
}

void ADVClipboard::copySequenceSelection(bool complement, bool amino) {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    if (seqCtx == nullptr) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), "No sequence selected!");
        return;
    }

    QString res;
    QVector<U2Region> regions = seqCtx->getSequenceSelection()->getSelectedRegions();
    if (!regions.isEmpty()) {
        U2SequenceObject* seqObj = seqCtx->getSequenceObject();
        DNATranslation* complTT = complement ? seqCtx->getComplementTT() : nullptr;
        DNATranslation* aminoTT = amino ? seqCtx->getAminoTT() : nullptr;
        U2OpStatus2Log os;
        QList<QByteArray> seqParts = U2SequenceUtils::extractRegions(seqObj->getSequenceRef(), regions, complTT, aminoTT, false, os);
        if (os.hasError()) {
            QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), tr("An error occurred during getting sequence data: %1").arg(os.getError()));
            return;
        }
        res = U1SequenceUtils::joinRegions(seqParts);
    }
    putIntoClipboard(res);
}

void ADVClipboard::copyAnnotationSelection(const bool amino) {
    const QList<Annotation*>& selectedAnnotationList = ctx->getAnnotationsSelection()->getAnnotations();
    QByteArray resultText;
    for (auto annotation : qAsConst(selectedAnnotationList)) {
        if (!resultText.isEmpty()) {
            resultText.append('\n');
        }
        ADVSequenceObjectContext* seqCtx = ctx->getSequenceContext(annotation->getGObject());
        if (seqCtx == nullptr) {
            resultText.append(U2Msa::GAP_CHAR);  // insert gap instead of the sequence, if the sequence is not available.
            continue;
        }
        DNATranslation* complTT = annotation->getStrand().isComplementary() ? seqCtx->getComplementTT() : nullptr;
        DNATranslation* aminoTT = amino ? seqCtx->getAminoTT() : nullptr;
        U2OpStatus2Log os;
        // BUG528: add alphabet symbol role: insertion mark and use it instead of the U2Msa::GAP_CHAR
        const U2EntityRef& sequenceObjectRef = seqCtx->getSequenceRef();
        QByteArray annotationSequence = AnnotationSelection::getSequenceUnderAnnotation(sequenceObjectRef, annotation, complTT, aminoTT, os);
        resultText.append(annotationSequence);
        CHECK_OP(os, );
    }
    putIntoClipboard(resultText);
}

void ADVClipboard::putIntoClipboard(const QString& data) {
    CHECK(!data.isEmpty(), );
    try {
        QApplication::clipboard()->setText(data);
    } catch (...) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), COPY_FAILED_MESSAGE);
    }
}

void ADVClipboard::sl_copySequence() {
    copySequenceSelection(false, false);
}

void ADVClipboard::sl_copyComplementSequence() {
    copySequenceSelection(true, false);
}

void ADVClipboard::sl_copyTranslation() {
    copySequenceSelection(false, true);
}

void ADVClipboard::sl_copyComplementTranslation() {
    copySequenceSelection(true, true);
}

void ADVClipboard::sl_copyAnnotationSequence() {
    copyAnnotationSelection(false);
}

void ADVClipboard::sl_copyAnnotationSequenceTranslation() {
    copyAnnotationSelection(true);
}

void ADVClipboard::sl_setCopyQualifierActionStatus(bool isEnabled, QString text) {
    copyQualifierAction->setEnabled(isEnabled);
    copyQualifierAction->setText(text);
}

void ADVClipboard::updateActions() {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    CHECK(seqCtx != nullptr, );

    DNASequenceSelection* sel = seqCtx->getSequenceSelection();
    SAFE_POINT(nullptr != sel, "DNASequenceSelection isn't found.", );

    const DNAAlphabet* alphabet = seqCtx->getAlphabet();
    SAFE_POINT(nullptr != alphabet, "DNAAlphabet isn't found.", );

    const bool isNucleic = alphabet->isNucleic();
    if (!isNucleic) {
        copyTranslationAction->setVisible(false);
        copyComplementSequenceAction->setVisible(false);
        copyComplementTranslationAction->setVisible(false);

        copyAnnotationSequenceAction->setText(tr("Copy annotation"));
        copyAnnotationSequenceTranslationAction->setVisible(false);
    }

    auto setActionsEnabled =
        [](const QList<QAction*>& copyActions, const bool isEnabled) {
            for (QAction* action : qAsConst(copyActions)) {
                if (action != nullptr) {
                    action->setEnabled(isEnabled);
                }
            }
        };
    auto setActionShortcutsEnabled =
        [](const QList<QAction*>& copyActions, const bool isShortcutEnabled) {
            SAFE_POINT(copyActions.size() == 4, "copyActions size must be 4!", );
            // [0] is copy sequence direct.
            copyActions[0]->setShortcut(isShortcutEnabled ? QKeySequence(Qt::CTRL | Qt::Key_C) : QKeySequence());

            // [1] is copy sequence complement.
            if (copyActions[1] != nullptr) {
                copyActions[1]->setShortcut(isShortcutEnabled ? QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C) : QKeySequence());
            }

            // [2] is copy amino direct.
            copyActions[2]->setShortcut(isShortcutEnabled ? QKeySequence(Qt::CTRL | Qt::Key_T) : QKeySequence());

            // [3] is copy amino complement.
            if (copyActions[3] != nullptr) {
                copyActions[3]->setShortcut(isShortcutEnabled ? QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T) : QKeySequence());
            }
        };

    auto setActionsAndShortcutsEnabled =
        [&setActionsEnabled, &setActionShortcutsEnabled](const QList<QAction*>& copyActions, const bool isEnabled) {
            setActionsEnabled(copyActions, isEnabled);
            setActionShortcutsEnabled(copyActions, isEnabled);
        };

    const bool hasSequenceSelection = !sel->getSelectedRegions().isEmpty();
    const bool hasAnnotationSelection = !ctx->getAnnotationsSelection()->isEmpty();
    // Create lists of 4 selection actions for sequence & annotation selections: copy/copyComplement/copyTranslation/copyComplementTranslation.
    QList<QAction*> sequenceActions = QList<QAction*>() << copySequenceAction << copyComplementSequenceAction << copyTranslationAction << copyComplementTranslationAction;
    QList<QAction*> annotationActions = QList<QAction*>() << copyAnnotationSequenceAction << nullptr << copyAnnotationSequenceTranslationAction << nullptr;
    if (!hasSequenceSelection && !hasAnnotationSelection) {
        setActionsEnabled(sequenceActions, false);
        setActionShortcutsEnabled(sequenceActions, true);  // Assign shortcuts to the currently disabled actions, so they will visually appear in the menu.
        setActionsAndShortcutsEnabled(annotationActions, false);
    } else if (hasSequenceSelection && hasAnnotationSelection) {
        setActionsAndShortcutsEnabled(sequenceActions, true);
        setActionsEnabled(annotationActions, true);
        setActionShortcutsEnabled(annotationActions, false);
    } else {
        setActionsAndShortcutsEnabled(sequenceActions, hasSequenceSelection);
        setActionsAndShortcutsEnabled(annotationActions, hasAnnotationSelection);
    }
}

void ADVClipboard::addCopyMenu(QMenu* m) {
    QMenu* copyMenu = new QMenu(tr("Copy/Paste"), m);
    copyMenu->menuAction()->setObjectName(ADV_MENU_COPY);

    copyMenu->addAction(copySequenceAction);
    copyMenu->addAction(copyComplementSequenceAction);
    copyMenu->addAction(copyTranslationAction);
    copyMenu->addAction(copyComplementTranslationAction);
    copyMenu->addSeparator();
    copyMenu->addAction(copyAnnotationSequenceAction);
    copyMenu->addAction(copyAnnotationSequenceTranslationAction);
    copyMenu->addSeparator();
    copyMenu->addAction(copyQualifierAction);
    copyMenu->addSeparator();
    copyMenu->addAction(pasteSequenceAction);

    m->addMenu(copyMenu);
}

QAction* ADVClipboard::createPasteSequenceAction(QObject* parent) {
    QAction* action = new QAction(QIcon(":/core/images/paste.png"), tr("Paste sequence"), parent);
    action->setObjectName("Paste sequence");
    action->setShortcuts(QKeySequence::Paste);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    return action;
}

ADVSequenceObjectContext* ADVClipboard::getSequenceContext() const {
    return ctx->getActiveSequenceContext();
}

void ADVClipboard::sl_onActiveSequenceChanged() {
    updateActions();
}
}  // namespace U2
