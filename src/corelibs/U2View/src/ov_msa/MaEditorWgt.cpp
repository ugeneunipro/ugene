/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorStatusBar.h>
#include <U2View/MsaEditor.h>
#include <U2View/MsaEditorConsensusArea.h>
#include <U2View/MsaEditorOffsetsView.h>
#include <U2View/MsaEditorOverviewArea.h>
#include <U2View/MsaEditorSequenceArea.h>

#include "MaEditorSplitters.h"
#include "MaEditorUtils.h"
#include "SequenceAreaRenderer.h"
#include "ov_msa/BaseWidthController.h"
#include "ov_msa/DrawHelper.h"
#include "ov_msa/ScrollController.h"

namespace U2 {

bool MaEditorWgtEventFilter::eventFilter(QObject* obj, QEvent* event) {
    // TODO:ichebyki
    // Maybe need to check QEvent::FocusIn || QEvent::Enter
    // Also,there is a question about children (QEvent::ChildAdded)

    // Please, don't forget about QWidget::setAttribute(Qt::WA_Hover, true);
    if (event->type() == QEvent::HoverEnter) {
        maEditorWgt->getEditor()->getMainWidget()->setActiveChild(maEditorWgt);
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

/************************************************************************/
/* MaEditorWgt */
/************************************************************************/
MaEditorWgt::MaEditorWgt(MaEditor* _editor, QWidget* parent)
    : QWidget(parent), editor(_editor),
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
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new ScrollController(editor, this)),
      baseWidthController(new BaseWidthController(this)),
      rowHeightController(nullptr),
      drawHelper(new DrawHelper(this)),
      delSelectionAction(nullptr),
      copySelectionAction(nullptr),
      copyFormattedSelectionAction(nullptr),
      pasteAction(nullptr),
      pasteBeforeAction(nullptr),
      cutSelectionAction(nullptr) {
    SAFE_POINT(editor != nullptr, "MaEditor is null!", );
    setFocusPolicy(Qt::ClickFocus);
}

QWidget* MaEditorWgt::createHeaderLabelWidget(const QString& text, Qt::Alignment alignment, QWidget* heightTarget, bool proxyMouseEventsToNameList) {
    QString labelHtml = QString("<p style=\"margin-right: 5px\">%1</p>").arg(text);
    return new MaLabelWidget(this,
                             heightTarget == nullptr ? seqAreaHeader : heightTarget,
                             labelHtml,
                             alignment,
                             proxyMouseEventsToNameList);
}

void MaEditorWgt::initWidgets(bool addStatusBar, bool addOverviewArea) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 100);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    auto horizontalSequenceScrollBar = new GScrollBar(Qt::Horizontal);
    horizontalSequenceScrollBar->setObjectName("horizontal_sequence_scroll");
    horizontalSequenceScrollBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    horizontalSequenceScrollBar->setFocusPolicy(Qt::StrongFocus);

    auto horizontalNameListScrollBar = new QScrollBar(Qt::Horizontal);
    horizontalNameListScrollBar->setObjectName("horizontal_names_scroll");
    horizontalNameListScrollBar->setFocusPolicy(Qt::StrongFocus);

    auto verticalSequenceScrollbar = new GScrollBar(Qt::Vertical);
    verticalSequenceScrollbar->setObjectName("vertical_sequence_scroll");
    verticalSequenceScrollbar->setFocusPolicy(Qt::StrongFocus);

    initSeqArea(horizontalSequenceScrollBar, verticalSequenceScrollbar);
    scrollController->init(horizontalSequenceScrollBar, verticalSequenceScrollbar);
    sequenceArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    initNameList(horizontalNameListScrollBar);
    nameList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    initConsensusArea();
    initOverviewArea(overviewArea);
    initStatusBar(statusBar);

    offsetsViewController = new MSAEditorOffsetsViewController(this, editor, sequenceArea);
    offsetsViewController->leftWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    offsetsViewController->rightWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    seqAreaHeader = new QWidget(this);
    seqAreaHeader->setObjectName("alignment_header_widget");
    seqAreaHeaderLayout = new QVBoxLayout();
    seqAreaHeaderLayout->setContentsMargins(0, 0, 0, 0);
    seqAreaHeaderLayout->setSpacing(0);
    seqAreaHeaderLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QWidget* label1 = createHeaderLabelWidget();
    QWidget* label2 = createHeaderLabelWidget();

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
    seqAreaLayout->addWidget(verticalSequenceScrollbar, 1, 3);

    seqAreaLayout->addWidget(horizontalSequenceScrollBar, 2, 0, 1, 3);

    seqAreaLayout->setRowStretch(1, 1);
    seqAreaLayout->setColumnStretch(1, 1);

    QWidget* seqAreaContainer = new QWidget();
    seqAreaContainer->setLayout(seqAreaLayout);

    QWidget* consensusLabel = createHeaderLabelWidget(tr("Consensus:"), Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter), consensusArea, false);
    consensusLabel->setMinimumHeight(consensusArea->height());
    consensusLabel->setObjectName("consensusLabel");

    nameAreaLayout = new QVBoxLayout();
    nameAreaLayout->setContentsMargins(0, 0, 0, 0);
    nameAreaLayout->setSpacing(0);
    nameAreaLayout->addWidget(consensusLabel);
    nameAreaLayout->addWidget(nameList);
    nameAreaLayout->addWidget(horizontalNameListScrollBar);

    nameAreaContainer = new QWidget();
    nameAreaContainer->setLayout(nameAreaLayout);
    nameAreaContainer->setStyleSheet("background-color: white;");
    horizontalNameListScrollBar->setStyleSheet("background-color: normal;");  // avoid white background of scrollbar set 1 line above.

    nameAreaContainer->setMinimumWidth(15);  // Splitter uses min-size to collapse a widget

    mainSplitter = new QSplitter(Qt::Vertical, this);
    nameAndSequenceAreasSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
    nameAndSequenceAreasSplitter->setObjectName("name_and_sequence_areas_splitter");
    nameAndSequenceAreasSplitter->addWidget(nameAreaContainer);
    nameAndSequenceAreasSplitter->addWidget(seqAreaContainer);
    nameAndSequenceAreasSplitter->setSizes({50, 100});  // Initial proportions of the name & sequence are (splitter has no real size at this moment).

    QVBoxLayout* maContainerLayout = new QVBoxLayout();
    maContainerLayout->setContentsMargins(0, 0, 0, 0);
    maContainerLayout->setSpacing(0);

    maContainerLayout->addWidget(nameAndSequenceAreasSplitter);
    maContainerLayout->setStretch(0, 1);

    if (addStatusBar) {
        maContainerLayout->addWidget(statusBar);
    }

    QWidget* maContainer = new QWidget(this);
    maContainer->setLayout(maContainerLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(maContainer);
    mainSplitter->setStretchFactor(0, 2);

    if (addOverviewArea) {
        auto wgt = qobject_cast<MsaEditorWgt*>(this);
        if (wgt == nullptr) {
            mainSplitter->addWidget(overviewArea);
            mainSplitter->setCollapsible(1, false);
            MaSplitterUtils::updateFixedSizeHandleStyle(mainSplitter);
        } else {
            maContainerLayout->addWidget(overviewArea);
        }
    }
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);

    connect(editor->getCollapseModel(), SIGNAL(si_toggled()), offsetsViewController, SLOT(sl_updateOffsets()));
    connect(editor->getCollapseModel(), SIGNAL(si_toggled()), sequenceArea, SLOT(sl_modelChanged()));

    connect(delSelectionAction, SIGNAL(triggered()), sequenceArea, SLOT(sl_delCurrentSelection()));
}

void MaEditorWgt::initActions() {
    // SANGER_TODO: check why delAction is not added
    delSelectionAction = new QAction(tr("Remove selection"), this);
    delSelectionAction->setObjectName("Remove selection");
#ifndef Q_OS_DARWIN
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
}

MaEditor* MaEditorWgt::getEditor() const {
    return editor;
}

MaEditorSequenceArea* MaEditorWgt::getSequenceArea() const {
    return sequenceArea;
}

MaEditorNameList* MaEditorWgt::getEditorNameList() const {
    return nameList;
}

MaEditorConsensusArea* MaEditorWgt::getConsensusArea() const {
    return consensusArea;
}

BaseWidthController* MaEditorWgt::getBaseWidthController() const {
    return baseWidthController;
}

RowHeightController* MaEditorWgt::getRowHeightController() const {
    return rowHeightController;
}

DrawHelper* MaEditorWgt::getDrawHelper() const {
    return drawHelper;
}

bool MaEditorWgt::isCollapsingOfSingleRowGroupsEnabled() const {
    return enableCollapsingOfSingleRowGroups;
}

QWidget* MaEditorWgt::getHeaderWidget() const {
    return seqAreaHeader;
}

QSplitter* MaEditorWgt::getMainSplitter() const {
    return mainSplitter;
}

MaEditorOverviewArea* MaEditorWgt::getOverviewArea() const {
    return overviewArea;
}

void MaEditorWgt::setOverviewArea(MaEditorOverviewArea* overview) {
    overviewArea = overview;
}

MaEditorStatusBar* MaEditorWgt::getStatusBar() const {
    return statusBar;
}

void MaEditorWgt::setStatusBar(MaEditorStatusBar* statusbar) {
    statusBar = statusbar;
}

MSAEditorOffsetsViewController* MaEditorWgt::getOffsetsViewController() const {
    return offsetsViewController;
}

ScrollController* MaEditorWgt::getScrollController() const {
    return scrollController;
}

}  // namespace U2
