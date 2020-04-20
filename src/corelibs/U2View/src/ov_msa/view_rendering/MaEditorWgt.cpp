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

#include <QGridLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/ExportImageDialog.h>

#include <U2View/MaEditorStatusBar.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/MSAEditorOverviewArea.h>
#include <U2View/UndoRedoFramework.h>

#include "MaEditorUtils.h"
#include "MaEditorWgt.h"
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
      seqArea(NULL),
      nameList(NULL),
      consArea(NULL),
      overviewArea(NULL),
      offsetsView(NULL),
      statusBar(NULL),
      nameAreaContainer(NULL),
      seqAreaHeader(NULL),
      seqAreaHeaderLayout(NULL),
      seqAreaLayout(NULL),
      nameAreaLayout(NULL),
      collapseModel(new MaCollapseModel(this, editor->getMaRowIds())),
      collapsibleMode(false),
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new ScrollController(editor, this, collapseModel)),
      baseWidthController(new BaseWidthController(this)),
      rowHeightController(NULL),
      drawHelper(new DrawHelper(this)),
      delSelectionAction(NULL),
      copySelectionAction(NULL),
      copyFormattedSelectionAction(NULL),
      pasteAction(NULL)
{
    undoFWK = new MsaUndoRedoFramework(this, editor->getMaObject());

    connect(getUndoAction(), SIGNAL(triggered()), SLOT(sl_countUndo()));
    connect(getRedoAction(), SIGNAL(triggered()), SLOT(sl_countRedo()));
}

QWidget* MaEditorWgt::createHeaderLabelWidget(const QString& text, Qt::Alignment alignment,
                                              QWidget* heightTarget, bool proxyMouseEventsToNameList) {
    QString labelHtml = QString("<p style=\"margin-right: 5px\">%1</p>").arg(text);
    return new MaLabelWidget(this,
                             heightTarget == NULL ? seqAreaHeader : heightTarget,
                             labelHtml, alignment, proxyMouseEventsToNameList);
}

ScrollController *MaEditorWgt::getScrollController() {
    return scrollController;
}

BaseWidthController *MaEditorWgt::getBaseWidthController() {
    return baseWidthController;
}

RowHeightController *MaEditorWgt::getRowHeightController() {
    return rowHeightController;
}

DrawHelper *MaEditorWgt::getDrawHelper() {
    return drawHelper;
}

QAction* MaEditorWgt::getUndoAction() const {
    QAction *a = undoFWK->getUndoAction();
    a->setObjectName("msa_action_undo");
    return a;
}

QAction* MaEditorWgt::getRedoAction() const {
    QAction *a = undoFWK->getRedoAction();
    a->setObjectName("msa_action_redo");
    return a;
}

void MaEditorWgt::sl_saveScreenshot(){
    CHECK(qobject_cast<MSAEditor*>(editor) != NULL, );
    MSAImageExportController controller(this);
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    QString fileName = GUrlUtils::fixFileName(editor->getMaObject()->getGObjectName());
    QObjectScopedPointer<ExportImageDialog> dlg = new ExportImageDialog(&controller, ExportImageDialog::MSA, fileName, ExportImageDialog::NoScaling, p);
    dlg->exec();
}

void MaEditorWgt::initWidgets() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 200);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    GScrollBar* shBar = new GScrollBar(Qt::Horizontal);
    shBar->setObjectName("horizontal_sequence_scroll");
    shBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QScrollBar* nhBar = new QScrollBar(Qt::Horizontal);
    nhBar->setObjectName("horizontal_names_scroll");
    GScrollBar* cvBar = new GScrollBar(Qt::Vertical);
    cvBar->setObjectName("vertical_sequence_scroll");

    initSeqArea(shBar, cvBar);
    scrollController->init(shBar, cvBar);
    seqArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    initOverviewArea();

    initNameList(nhBar);
    nameList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    initConsensusArea();
    initStatusBar();

    offsetsView = new MSAEditorOffsetsViewController(this, editor, seqArea);
    offsetsView->getLeftWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    offsetsView->getRightWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    seqAreaHeader = new QWidget(this);
    seqAreaHeader->setObjectName("alignment_header_widget");
    seqAreaHeaderLayout = new QVBoxLayout();
    seqAreaHeaderLayout->setContentsMargins(0, 0, 0, 0);
    seqAreaHeaderLayout->setSpacing(0);
    seqAreaHeaderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QWidget* label1 = createHeaderLabelWidget();
    QWidget* label2 = createHeaderLabelWidget();

    seqAreaHeaderLayout->addWidget(consArea);
    seqAreaHeader->setLayout(seqAreaHeaderLayout);

    seqAreaLayout = new QGridLayout();
    seqAreaLayout->setContentsMargins(0, 0, 0, 0);
    seqAreaLayout->setSpacing(0);

    seqAreaLayout->addWidget(label1, 0, 0);
    seqAreaLayout->addWidget(seqAreaHeader, 0, 1);
    seqAreaLayout->addWidget(label2, 0, 2, 1, 2);

    seqAreaLayout->addWidget(offsetsView->getLeftWidget(), 1, 0);
    seqAreaLayout->addWidget(seqArea, 1, 1);
    seqAreaLayout->addWidget(offsetsView->getRightWidget(), 1, 2);
    seqAreaLayout->addWidget(cvBar, 1, 3);

    seqAreaLayout->addWidget(shBar, 2, 0, 1, 3);

    seqAreaLayout->setRowStretch(1, 1);
    seqAreaLayout->setColumnStretch(1, 1);

    QWidget* seqAreaContainer = new QWidget();
    seqAreaContainer->setLayout(seqAreaLayout);

    QWidget *label;
    label = createHeaderLabelWidget(tr("Consensus:"), Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter), consArea, false);
    label->setMinimumHeight(consArea->height());
    label->setObjectName("consensusLabel");

    nameAreaLayout = new QVBoxLayout();
    nameAreaLayout->setContentsMargins(0, 0, 0, 0);
    nameAreaLayout->setSpacing(0);
    nameAreaLayout->addWidget(label);
    nameAreaLayout->addWidget(nameList);
    nameAreaLayout->addWidget(nhBar);

    nameAreaContainer = new QWidget();
    nameAreaContainer->setLayout(nameAreaLayout);
    nameAreaContainer->setStyleSheet("background-color: white;");
    nhBar->setStyleSheet("background-color: normal;"); // avoid white background of scrollbar set 1 line above.

    nameAreaContainer->setMinimumWidth(15); // splitter uses min-size to collapse a widget
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

    connect(collapseModel, SIGNAL(si_toggled()), offsetsView, SLOT(sl_updateOffsets()));
    connect(collapseModel, SIGNAL(si_toggled()), seqArea,     SLOT(sl_modelChanged()));
    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));

    connect(delSelectionAction, SIGNAL(triggered()), seqArea, SLOT(sl_delCurrentSelection()));

    nameList->addAction(delSelectionAction);
}

void MaEditorWgt::initActions() {
    // SANGER_TODO: check why delAction is not added
    delSelectionAction = new QAction(tr("Remove selection"), this);
    delSelectionAction->setObjectName("Remove selection");
#ifndef Q_OS_MAC
    // Shortcut was wrapped with ifndef to workaround UGENE-6676.
    // On Qt5.12.6 the issue cannot be reproduced, so shortcut should be restored.
    delSelectionAction->setShortcut(QKeySequence::Delete);
    delSelectionAction->setShortcutContext(Qt::WidgetShortcut);
#endif

    copySelectionAction = new QAction(tr("Copy selection"), this);
    copySelectionAction->setObjectName("copy_selection");
    copySelectionAction->setShortcut(QKeySequence::Copy);
    copySelectionAction->setShortcutContext(Qt::WidgetShortcut);
    copySelectionAction->setToolTip(QString("%1 (%2)").arg(copySelectionAction->text())
        .arg(copySelectionAction->shortcut().toString()));

    addAction(copySelectionAction);

    copyFormattedSelectionAction = new QAction(QIcon(":core/images/copy_sequence.png"), tr("Copy formatted"), this);
    copyFormattedSelectionAction->setObjectName("copy_formatted");
    copyFormattedSelectionAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    copyFormattedSelectionAction->setShortcutContext(Qt::WidgetShortcut);
    copyFormattedSelectionAction->setToolTip(QString("%1 (%2)").arg(copyFormattedSelectionAction->text())
        .arg(copyFormattedSelectionAction->shortcut().toString()));

    addAction(copyFormattedSelectionAction);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setObjectName("paste");
    pasteAction->setShortcuts(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WidgetShortcut);
    pasteAction->setToolTip(QString("%1 (%2)").arg(pasteAction->text())
        .arg(pasteAction->shortcut().toString()));

    addAction(pasteAction);
}

void MaEditorWgt::sl_countUndo() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, tr("Undo"), editor->getFactoryId());
}

void MaEditorWgt::sl_countRedo() {
    GRUNTIME_NAMED_COUNTER(cvar, tvar, tr("Redo"), editor->getFactoryId());
}

} // namespace
