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

#include "MaEditorWgt.h"

#include <QGridLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/ExportImageDialog.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/MSAEditorOverviewArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorStatusBar.h>
#include <U2View/UndoRedoFramework.h>

#include "MaEditorUtils.h"
#include "SequenceAreaRenderer.h"
#include "ov_msa/Export/MSAImageExportTask.h"
#include "ov_msa/helpers/BaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/ScrollController.h"

namespace U2 {

/************************************************************************/
/* MaEditorWgt */
/************************************************************************/
MaEditorWgt::MaEditorWgt(MaEditor *editor)
    : editor(editor),
      sequenceArea(nullptr),
      nameList(nullptr),
      consensusArea(nullptr),
      overviewArea(nullptr),
      offsetsViewController(nullptr),
      statusBar(nullptr),
      nameAreaContainer(nullptr),
      seqAreaHeader(nullptr),
      seqAreaHeaderLayout(nullptr),
      seqAreaLayout(nullptr),
      nameAreaLayout(nullptr),
      collapseModel(new MaCollapseModel(this, editor->getMaRowIds())),
      virtualOrderMode(false),
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new ScrollController(editor, this, collapseModel)),
      baseWidthController(new BaseWidthController(this)),
      rowHeightController(nullptr),
      drawHelper(new DrawHelper(this)),
      delSelectionAction(nullptr),
      copySelectionAction(nullptr),
      copyFormattedSelectionAction(nullptr),
      pasteAction(nullptr),
      pasteBeforeAction(nullptr),
      cutSelectionAction(nullptr) {
    undoFWK = new MsaUndoRedoFramework(this, editor->getMaObject());
    setFocusPolicy(Qt::ClickFocus);

    connect(getUndoAction(), SIGNAL(triggered()), SLOT(sl_countUndo()));
    connect(getRedoAction(), SIGNAL(triggered()), SLOT(sl_countRedo()));
}

QWidget *MaEditorWgt::createHeaderLabelWidget(const QString &text, Qt::Alignment alignment, QWidget *heightTarget, bool proxyMouseEventsToNameList) {
    QString labelHtml = QString("<p style=\"margin-right: 5px\">%1</p>").arg(text);
    return new MaLabelWidget(this,
                             heightTarget == nullptr ? seqAreaHeader : heightTarget,
                             labelHtml,
                             alignment,
                             proxyMouseEventsToNameList);
}

QAction *MaEditorWgt::getUndoAction() const {
    QAction *a = undoFWK->getUndoAction();
    a->setObjectName("msa_action_undo");
    return a;
}

QAction *MaEditorWgt::getRedoAction() const {
    QAction *a = undoFWK->getRedoAction();
    a->setObjectName("msa_action_redo");
    return a;
}

void MaEditorWgt::sl_saveScreenshot() {
    CHECK(qobject_cast<MSAEditor *>(editor) != nullptr, );
    MSAImageExportController controller(this);
    QWidget *p = (QWidget *)AppContext::getMainWindow()->getQMainWindow();
    QString fileName = GUrlUtils::fixFileName(editor->getMaObject()->getGObjectName());
    QObjectScopedPointer<ExportImageDialog> dlg = new ExportImageDialog(&controller, ExportImageDialog::MSA, fileName, ExportImageDialog::NoScaling, p);
    dlg->exec();
}

void MaEditorWgt::initWidgets() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 200);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    GScrollBar *shBar = new GScrollBar(Qt::Horizontal);
    shBar->setObjectName("horizontal_sequence_scroll");
    shBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QScrollBar *nameListHorizontalScrollBar = new QScrollBar(Qt::Horizontal);
    nameListHorizontalScrollBar->setObjectName("horizontal_names_scroll");
    GScrollBar *cvBar = new GScrollBar(Qt::Vertical);
    cvBar->setObjectName("vertical_sequence_scroll");

    initSeqArea(shBar, cvBar);
    scrollController->init(shBar, cvBar);
    sequenceArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    initOverviewArea();

    initNameList(nameListHorizontalScrollBar);
    nameList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    initConsensusArea();
    initStatusBar();

    offsetsViewController = new MSAEditorOffsetsViewController(this, editor, sequenceArea);
    offsetsViewController->leftWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    offsetsViewController->rightWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    seqAreaHeader = new QWidget(this);
    seqAreaHeader->setObjectName("alignment_header_widget");
    seqAreaHeaderLayout = new QVBoxLayout();
    seqAreaHeaderLayout->setContentsMargins(0, 0, 0, 0);
    seqAreaHeaderLayout->setSpacing(0);
    seqAreaHeaderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QWidget *label1 = createHeaderLabelWidget();
    QWidget *label2 = createHeaderLabelWidget();

    seqAreaHeaderLayout->addWidget(consensusArea);
    seqAreaHeader->setLayout(seqAreaHeaderLayout);

    seqAreaLayout = new QGridLayout();
    seqAreaLayout->setContentsMargins(0, 0, 0, 0);
    seqAreaLayout->setSpacing(0);

    seqAreaLayout->addWidget(label1, 0, 0);
    seqAreaLayout->addWidget(seqAreaHeader, 0, 1);
    seqAreaLayout->addWidget(label2, 0, 2, 1, 2);

    seqAreaLayout->addWidget(offsetsViewController->leftWidget, 1, 0);
    seqAreaLayout->addWidget(sequenceArea, 1, 1);
    seqAreaLayout->addWidget(offsetsViewController->rightWidget, 1, 2);
    seqAreaLayout->addWidget(cvBar, 1, 3);

    seqAreaLayout->addWidget(shBar, 2, 0, 1, 3);

    seqAreaLayout->setRowStretch(1, 1);
    seqAreaLayout->setColumnStretch(1, 1);

    QWidget *seqAreaContainer = new QWidget();
    seqAreaContainer->setLayout(seqAreaLayout);

    QWidget *consensusLabel = createHeaderLabelWidget(tr("Consensus:"), Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter), consensusArea, false);
    consensusLabel->setMinimumHeight(consensusArea->height());
    consensusLabel->setObjectName("consensusLabel");

    nameAreaLayout = new QVBoxLayout();
    nameAreaLayout->setContentsMargins(0, 0, 0, 0);
    nameAreaLayout->setSpacing(0);
    nameAreaLayout->addWidget(consensusLabel);
    nameAreaLayout->addWidget(nameList);
    nameAreaLayout->addWidget(nameListHorizontalScrollBar);

    nameAreaContainer = new QWidget();
    nameAreaContainer->setLayout(nameAreaLayout);
    nameAreaContainer->setStyleSheet("background-color: white;");
    nameListHorizontalScrollBar->setStyleSheet("background-color: normal;");    // avoid white background of scrollbar set 1 line above.

    nameAreaContainer->setMinimumWidth(15);    // splitter uses min-size to collapse a widget
    maSplitter.addWidget(nameAreaContainer, 0, 0.1);
    maSplitter.addWidget(seqAreaContainer, 1, 3);

    QVBoxLayout *maContainerLayout = new QVBoxLayout();
    maContainerLayout->setContentsMargins(0, 0, 0, 0);
    maContainerLayout->setSpacing(0);

    maContainerLayout->addWidget(maSplitter.getSplitter());
    maContainerLayout->setStretch(0, 1);
    maContainerLayout->addWidget(statusBar);

    QWidget *maContainer = new QWidget(this);
    maContainer->setLayout(maContainerLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(maContainer);
    mainSplitter->setStretchFactor(0, 2);

    if (overviewArea->isResizable()) {
        mainSplitter->addWidget(overviewArea);
        mainSplitter->setCollapsible(1, false);
    } else {
        maContainerLayout->addWidget(overviewArea);
    }
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);

    connect(collapseModel, SIGNAL(si_toggled()), offsetsViewController, SLOT(sl_updateOffsets()));
    connect(collapseModel, SIGNAL(si_toggled()), sequenceArea, SLOT(sl_modelChanged()));
    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));

    connect(delSelectionAction, SIGNAL(triggered()), sequenceArea, SLOT(sl_delCurrentSelection()));
}

void MaEditorWgt::initActions() {
    // SANGER_TODO: check why delAction is not added
    delSelectionAction = new QAction(tr("Remove selection"), this);
    delSelectionAction->setObjectName("Remove selection");
#ifndef Q_OS_MAC
    // Shortcut was wrapped with ifndef to workaround UGENE-6676.
    // On Qt5.12.6 the issue cannot be reproduced, so shortcut should be restored.
    delSelectionAction->setShortcut(QKeySequence::Delete);
    delSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
#endif
    addAction(delSelectionAction);

    copySelectionAction = new QAction(tr("Copy"), this);
    copySelectionAction->setObjectName("copy_selection");
    copySelectionAction->setShortcut(QKeySequence::Copy);
    copySelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    copySelectionAction->setToolTip(QString("%1 (%2)").arg(copySelectionAction->text()).arg(copySelectionAction->shortcut().toString()));
    addAction(copySelectionAction);

    copyFormattedSelectionAction = new QAction(QIcon(":core/images/copy_sequence.png"), tr("Copy (custom format)"), this);
    copyFormattedSelectionAction->setObjectName("copy_formatted");
    copyFormattedSelectionAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    copyFormattedSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    copyFormattedSelectionAction->setToolTip(QString("%1 (%2)").arg(copyFormattedSelectionAction->text()).arg(copyFormattedSelectionAction->shortcut().toString()));
    addAction(copyFormattedSelectionAction);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setObjectName("paste");
    pasteAction->setShortcuts(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    pasteAction->setToolTip(QString("%1 (%2)").arg(pasteAction->text()).arg(pasteAction->shortcut().toString()));
    addAction(pasteAction);

    pasteBeforeAction = new QAction(tr("Paste (before selection)"), this);
    pasteBeforeAction->setObjectName("paste_before");
    pasteBeforeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_V));
    pasteBeforeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    pasteBeforeAction->setToolTip(QString("%1 (%2)").arg(pasteBeforeAction->text()).arg(pasteAction->shortcut().toString()));
    addAction(pasteBeforeAction);

    cutSelectionAction = new QAction(tr("Cut"), this);
    cutSelectionAction->setObjectName("cut_selection");
    cutSelectionAction->setShortcut(QKeySequence::Cut);
    cutSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    cutSelectionAction->setToolTip(QString("%1 (%2)").arg(cutSelectionAction->text()).arg(cutSelectionAction->shortcut().toString()));
    addAction(cutSelectionAction);

    addAction(getUndoAction());
    addAction(getRedoAction());
}

void MaEditorWgt::sl_countUndo() {
    GCounter::increment("Undo", editor->getFactoryId());
}

void MaEditorWgt::sl_countRedo() {
    GCounter::increment("Redo", editor->getFactoryId());
}

bool MaEditorWgt::isVirtualOrderMode() const {
    return virtualOrderMode;
}

void MaEditorWgt::setVirtualOrderMode(bool flag) {
    virtualOrderMode = flag;
}

}    // namespace U2
