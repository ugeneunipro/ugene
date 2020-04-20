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
#include <U2Core/SelectionUtils.h>
#include <U2Core/SequenceUtils.h>
#include <U2Core/TextUtils.h>
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
const qint64 ADVClipboard::MAX_COPY_SIZE_FOR_X86 = 100 * 1024 * 1024;

ADVClipboard::ADVClipboard(AnnotatedDNAView *c)
    : QObject(c) {
    ctx = c;
    //TODO: listen seqadded/seqremoved!!

    connect(ctx, SIGNAL(si_focusChanged(ADVSequenceWidget *, ADVSequenceWidget *)), SLOT(sl_onFocusedSequenceWidgetChanged(ADVSequenceWidget *, ADVSequenceWidget *)));

    foreach (ADVSequenceObjectContext *sCtx, ctx->getSequenceContexts()) {
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

    copyAnnotationSequenceAction = new QAction(QIcon(":/core/images/copy_annotation_sequence.png"), tr("Copy annotation direct strand"), this);
    copyAnnotationSequenceAction->setObjectName("action_copy_annotation_sequence");
    connect(copyAnnotationSequenceAction, SIGNAL(triggered()), SLOT(sl_copyAnnotationSequence()));

    copyComplementAnnotationSequenceAction = new QAction(QIcon(":/core/images/copy_complement_annotation_sequence.png"), tr("Copy annotation complementary 5'-3' strand"), this);
    copyComplementAnnotationSequenceAction->setObjectName("copy_complementary_annotation");
    connect(copyComplementAnnotationSequenceAction, SIGNAL(triggered()), SLOT(sl_copyComplementAnnotationSequence()));

    copyAnnotationSequenceTranslationAction = new QAction(QIcon(":/core/images/copy_annotation_translation.png"), tr("Copy annotation amino acids"), this);
    copyAnnotationSequenceTranslationAction->setObjectName("Copy annotation sequence translation");
    connect(copyAnnotationSequenceTranslationAction, SIGNAL(triggered()), SLOT(sl_copyAnnotationSequenceTranslation()));

    copyComplementAnnotationSequenceTranslationAction = new QAction(QIcon(":/core/images/copy_complement_annotation_translation.png"), tr("Copy annotation amino acids of complementary 5'-3' strand"), this);
    copyComplementAnnotationSequenceTranslationAction->setObjectName("copy_complement_annotation_translation");
    connect(copyComplementAnnotationSequenceTranslationAction, SIGNAL(triggered()), SLOT(sl_copyComplementAnnotationSequenceTranslation()));

    copyQualifierAction = new QAction(QIcon(":/core/images/copy_qualifier.png"), tr("Copy qualifier text"), this);
    copyQualifierAction->setEnabled(false);

    pasteSequenceAction = createPasteSequenceAction(this);
    updateActions();
}

QAction *ADVClipboard::getCopySequenceAction() const {
    return copySequenceAction;
}

QAction *ADVClipboard::getCopyComplementAction() const {
    return copyComplementSequenceAction;
}

QAction *ADVClipboard::getCopyTranslationAction() const {
    return copyTranslationAction;
}

QAction *ADVClipboard::getCopyComplementTranslationAction() const {
    return copyComplementTranslationAction;
}

QAction *ADVClipboard::getCopyAnnotationSequenceAction() const {
    return copyAnnotationSequenceAction;
}

QAction *ADVClipboard::getCopyComplementAnnotationSequenceAction() const {
    return copyComplementAnnotationSequenceAction;
}

QAction *ADVClipboard::getCopyAnnotationSequenceTranslationAction() const {
    return copyAnnotationSequenceTranslationAction;
}

QAction *ADVClipboard::getCopyComplementAnnotationSequenceTranslationAction() const {
    return copyComplementAnnotationSequenceTranslationAction;
}

QAction *ADVClipboard::getCopyQualifierAction() const {
    return copyQualifierAction;
}

QAction *ADVClipboard::getPasteSequenceAction() const {
    return pasteSequenceAction;
}

void ADVClipboard::connectSequence(ADVSequenceObjectContext *sCtx) {
    connect(sCtx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection *, const QVector<U2Region> &, const QVector<U2Region> &)),
            SLOT(sl_onDNASelectionChanged(LRegionsSelection *, const QVector<U2Region> &, const QVector<U2Region> &)));

    connect(sCtx->getAnnotatedDNAView()->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)),
            SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)));
}

void ADVClipboard::sl_onDNASelectionChanged(LRegionsSelection *, const QVector<U2Region> &, const QVector<U2Region> &) {
    updateActions();
}

void ADVClipboard::sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &) {
    updateActions();
}

void ADVClipboard::copySequenceSelection(const bool complement, const bool amino) {
    ADVSequenceObjectContext *seqCtx = getSequenceContext();
    if (seqCtx == nullptr) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), "No sequence selected!");
        return;
    }

    QString res;
    QVector<U2Region> regions = seqCtx->getSequenceSelection()->getSelectedRegions();
#ifdef UGENE_X86
    int totalLen = 0;
    foreach (const U2Region &r, regions) {
        totalLen += r.length;
    }
    if (totalLen > MAX_COPY_SIZE_FOR_X86) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), COPY_FAILED_MESSAGE);
        return;
    }
#endif

    if (!regions.isEmpty()) {
        U2SequenceObject *seqObj = seqCtx->getSequenceObject();
        DNATranslation *complTT = complement ? seqCtx->getComplementTT() : nullptr;
        DNATranslation *aminoTT = amino ? seqCtx->getAminoTT() : nullptr;
        U2OpStatus2Log os;
        QList<QByteArray> seqParts = U2SequenceUtils::extractRegions(seqObj->getSequenceRef(), regions, complTT, aminoTT, false, os);
        if (os.hasError()) {
            QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), tr("An error occurred during getting sequence data: %1").arg(os.getError()));
            return;
        }
        if (seqParts.size() == 1) {
            putIntoClipboard(seqParts.first());
            return;
        }
        res = U1SequenceUtils::joinRegions(seqParts);
    }
    putIntoClipboard(res);
}

void ADVClipboard::copyAnnotationSelection(const bool complement, const bool amino) {
    QByteArray res;
    const QList<Annotation *> &as = ctx->getAnnotationsSelection()->getAnnotations();
#ifdef UGENE_X86
    qint64 totalLen = 0;
    foreach (const Annotation *a, as) {
        totalLen += a->getRegionsLen();
    }
    if (totalLen > MAX_COPY_SIZE_FOR_X86) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), COPY_FAILED_MESSAGE);
        return;
    }
#endif

    //BUG528: add alphabet symbol role: insertion mark
    //TODO: reuse AnnotationSelection utils
    const char gapSym = '-';
    const int size = as.size();
    for (int i = 0; i < size; i++) {
        const Annotation *annotation = as.at(i);
        if (i != 0) {
            res.append('\n');    //?? generate sequence with len == region-len using default sym?
        }
        ADVSequenceObjectContext *seqCtx = ctx->getSequenceContext(annotation->getGObject());
        CHECK_OPERATIONS(seqCtx != nullptr, res.append(gapSym), continue);

        DNATranslation *complTT = complement ? seqCtx->getComplementTT() : nullptr;
        DNATranslation *aminoTT = amino ? seqCtx->getAminoTT() : nullptr;
        U2OpStatus2Log os;
        QList<QByteArray> parts = U2SequenceUtils::extractRegions(seqCtx->getSequenceRef(), annotation->getRegions(), complTT, aminoTT, annotation->isJoin(), os);
        CHECK_OP(os, );

        res += U1SequenceUtils::joinRegions(parts);
    }
    putIntoClipboard(res);
}

void ADVClipboard::putIntoClipboard(const QString &data) {
    CHECK(!data.isEmpty(), );
#ifdef UGENE_X86
    if (data.size() > MAX_COPY_SIZE_FOR_X86) {
        QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), COPY_FAILED_MESSAGE);
        return;
    }
#endif
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
    copyAnnotationSelection(false, false);
}

void ADVClipboard::sl_copyComplementAnnotationSequence() {
    copyAnnotationSelection(true, false);
}

void ADVClipboard::sl_copyAnnotationSequenceTranslation() {
    copyAnnotationSelection(false, true);
}

void ADVClipboard::sl_copyComplementAnnotationSequenceTranslation() {
    copyAnnotationSelection(true, true);
}

void ADVClipboard::sl_setCopyQualifierActionStatus(bool isEnabled, QString text) {
    copyQualifierAction->setEnabled(isEnabled);
    copyQualifierAction->setText(text);
}

void ADVClipboard::updateActions() {
    ADVSequenceObjectContext *seqCtx = getSequenceContext();
    CHECK(nullptr != seqCtx, );

    DNASequenceSelection *sel = seqCtx->getSequenceSelection();
    SAFE_POINT(nullptr != sel, "DNASequenceSelection isn't found.", );

    const DNAAlphabet *alphabet = seqCtx->getAlphabet();
    SAFE_POINT(nullptr != alphabet, "DNAAlphabet isn't found.", );

    const bool isNucleic = alphabet->isNucleic();
    if (!isNucleic) {
        copyTranslationAction->setVisible(false);
        copyComplementSequenceAction->setVisible(false);
        copyComplementTranslationAction->setVisible(false);

        copyAnnotationSequenceAction->setText("Copy annotation");
        copyComplementAnnotationSequenceAction->setVisible(false);
        copyAnnotationSequenceTranslationAction->setVisible(false);
        copyComplementAnnotationSequenceTranslationAction->setVisible(false);
    }

    auto setActionsEnabled =
        [](QList<QAction*> copyActions, const bool setEnabled) {
        foreach(QAction * action, copyActions) {
                action->setEnabled(setEnabled);
            }
        };
    auto setActionsShortcuted =
        [](QAction *copy,
           QAction *copyComplement,
           QAction *copyTranslation,
           QAction *copyComplementTranslation,
           const bool setShortcuted)
    {
        copy->setShortcut(setShortcuted ? QKeySequence(Qt::CTRL | Qt::Key_C) : QKeySequence());
        copyComplement->setShortcut(setShortcuted ? QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C) : QKeySequence());
        copyTranslation->setShortcut(setShortcuted ? QKeySequence(Qt::CTRL | Qt::Key_T) : QKeySequence());
        copyComplementTranslation->setShortcut(setShortcuted ? QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T) : QKeySequence());
    };
    auto setActionsEnabledAndShortcuted =
        [&setActionsEnabled, &setActionsShortcuted](QAction *copy,
                                                    QAction *copyComplement,
                                                    QAction *copyTranslation,
                                                    QAction *copyComplementTranslation,
                                                    const bool setEnabledAndShortcuted) {
        setActionsEnabled(QList<QAction *>() = {copy, copyComplement, copyTranslation, copyComplementTranslation}, setEnabledAndShortcuted);
        setActionsShortcuted(copy, copyComplement, copyTranslation, copyComplementTranslation, setEnabledAndShortcuted);
    };

    const bool selectionIsNotEmpty = !sel->getSelectedRegions().isEmpty();
    const bool hasAnnotationSelection = !ctx->getAnnotationsSelection()->isEmpty();
    if (!selectionIsNotEmpty && !hasAnnotationSelection) {
        setActionsEnabled(QList<QAction *>() = { copySequenceAction,
                                                copyComplementSequenceAction,
                                                 copyTranslationAction,
                                                 copyComplementTranslationAction },
                          false);
        setActionsShortcuted(copySequenceAction,
                             copyComplementSequenceAction,
                             copyTranslationAction,
                             copyComplementTranslationAction,
                             true);
        setActionsEnabledAndShortcuted(copyAnnotationSequenceAction,
                                       copyComplementAnnotationSequenceAction,
                                       copyAnnotationSequenceTranslationAction,
                                       copyComplementAnnotationSequenceTranslationAction,
                                       false);
    } else if (selectionIsNotEmpty && !hasAnnotationSelection) {
        setActionsEnabledAndShortcuted(copySequenceAction,
                                       copyComplementSequenceAction,
                                       copyTranslationAction,
                                       copyComplementTranslationAction,
                                       true);
        setActionsEnabledAndShortcuted(copyAnnotationSequenceAction,
                                       copyComplementAnnotationSequenceAction,
                                       copyAnnotationSequenceTranslationAction,
                                       copyComplementAnnotationSequenceTranslationAction,
                                       false);
    } else if (!selectionIsNotEmpty && hasAnnotationSelection) {
        setActionsEnabledAndShortcuted(copySequenceAction,
                                       copyComplementSequenceAction,
                                       copyTranslationAction,
                                       copyComplementTranslationAction,
                                       false);
        setActionsEnabledAndShortcuted(copyAnnotationSequenceAction,
                                       copyComplementAnnotationSequenceAction,
                                       copyAnnotationSequenceTranslationAction,
                                       copyComplementAnnotationSequenceTranslationAction,
                                       true);
    } else if (selectionIsNotEmpty && hasAnnotationSelection) {
        setActionsEnabledAndShortcuted(copySequenceAction,
                                       copyComplementSequenceAction,
                                       copyTranslationAction,
                                       copyComplementTranslationAction,
                                       true);
        setActionsEnabled(QList<QAction *>() = { copyAnnotationSequenceAction,
                                                 copyComplementAnnotationSequenceAction,
                                                 copyAnnotationSequenceTranslationAction,
                                                 copyComplementAnnotationSequenceTranslationAction },
                          true);
        setActionsShortcuted(copyAnnotationSequenceAction,
                             copyComplementAnnotationSequenceAction,
                             copyAnnotationSequenceTranslationAction,
                             copyComplementAnnotationSequenceTranslationAction,
                             false);
    } else {
        FAIL("Unexpected selection type", );
    }
}

void ADVClipboard::addCopyMenu(QMenu *m) {
    QMenu *copyMenu = new QMenu(tr("Copy/Paste"), m);
    copyMenu->menuAction()->setObjectName(ADV_MENU_COPY);

    copyMenu->addAction(copySequenceAction);
    copyMenu->addAction(copyComplementSequenceAction);
    copyMenu->addAction(copyTranslationAction);
    copyMenu->addAction(copyComplementTranslationAction);
    copyMenu->addSeparator();
    copyMenu->addAction(copyAnnotationSequenceAction);
    copyMenu->addAction(copyComplementAnnotationSequenceAction);
    copyMenu->addAction(copyAnnotationSequenceTranslationAction);
    copyMenu->addAction(copyComplementAnnotationSequenceTranslationAction);
    copyMenu->addSeparator();
    copyMenu->addAction(copyQualifierAction);
    copyMenu->addSeparator();
    copyMenu->addAction(pasteSequenceAction);

    m->addMenu(copyMenu);
}

QAction *ADVClipboard::createPasteSequenceAction(QObject *parent) {
    QAction *action = new QAction(QIcon(":/core/images/paste.png"), tr("Paste sequence"), parent);
    action->setObjectName("Paste sequence");
    action->setShortcuts(QKeySequence::Paste);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    return action;
}

ADVSequenceObjectContext *ADVClipboard::getSequenceContext() const {
    return ctx->getSequenceInFocus();
}

void ADVClipboard::sl_onFocusedSequenceWidgetChanged(ADVSequenceWidget *oldW, ADVSequenceWidget *newW) {
    Q_UNUSED(oldW);
    Q_UNUSED(newW);
    updateActions();
}
}    // namespace U2
