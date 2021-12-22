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

#include "MsaEditorWgt.h"

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include "MSAEditor.h"
#include "MSAEditorConsensusArea.h"
#include "MSAEditorOverviewArea.h"
#include "MSAEditorSequenceArea.h"
#include "MaEditorSplitters.h"
#include "MsaEditorNameList.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "MsaRowHeightController.h"
#include "ScrollController.h"
#include "phy_tree/MSAEditorMultiTreeViewer.h"

namespace U2 {

MsaEditorWgt::MsaEditorWgt(MSAEditor *editor,
                           MaEditorOverviewArea *overview,
                           MaEditorStatusBar *statusbar)
    : MaEditorWgt(editor),
      multiTreeViewer(nullptr),
      similarityStatistics(nullptr)
{
    overviewArea = overview;
    statusBar = statusbar;
    rowHeightController = new MsaRowHeightController(this);
    initActions();
    initWidgets(false, false);

    // For active MaEditorWgt tracking
    this->setAttribute(Qt::WA_Hover, true);
    eventFilter = new MaEditorWgtEventFilter(this, this);
    this->installEventFilter(eventFilter);

    setMinimumSize(sizeHint());
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

MSAEditor* MsaEditorWgt::getEditor() const {
    return qobject_cast<MSAEditor*>(editor);
}

MSAEditorSequenceArea* MsaEditorWgt::getSequenceArea() const {
    return qobject_cast<MSAEditorSequenceArea*>(sequenceArea);
}

void MsaEditorWgt::sl_onTabsCountChanged(int curTabsNumber) {
    if (curTabsNumber < 1) {
        delete multiTreeViewer;
        multiTreeViewer = nullptr;
        emit si_hideTreeOP();
    }
}

void MsaEditorWgt::createDistanceColumn(MSADistanceMatrix* matrix) {
    dataList->setMatrix(matrix);
    dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    MsaEditorAlignmentDependentWidget* statisticsWidget = new MsaEditorAlignmentDependentWidget(dataList);

    MaSplitterUtils::insertWidgetWithScale(nameAndSequenceAreasSplitter, statisticsWidget, 0.04, nameAreaContainer, 1);
}

void MsaEditorWgt::addTreeView(GObjectViewWindow* treeView) {
    if (multiTreeViewer == nullptr) {
        multiTreeViewer = new MSAEditorMultiTreeViewer(tr("Tree view"), getEditor());

        getEditor()->getUI()->addPhylTreeWidget(multiTreeViewer);
        multiTreeViewer->addTreeView(treeView);
        multiTreeViewer->setMinimumWidth(250);
        emit si_showTreeOP();
        connect(multiTreeViewer, SIGNAL(si_tabsCountChanged(int)), SLOT(sl_onTabsCountChanged(int)));
    } else {
        multiTreeViewer->addTreeView(treeView);
    }
}

void MsaEditorWgt::setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
    similarityStatistics->setSettings(settings);
}

void MsaEditorWgt::refreshSimilarityColumn() {
    dataList->updateWidget();
}

void MsaEditorWgt::showSimilarity() {
    if (nullptr == similarityStatistics) {
        SimilarityStatisticsSettings settings;
        settings.ma = getEditor()->getMaObject();
        settings.algoId = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmIds().at(0);
        settings.ui = this;

        dataList = new MsaEditorSimilarityColumn(this, new QScrollBar(Qt::Horizontal), &settings);
        dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        similarityStatistics = new MsaEditorAlignmentDependentWidget(dataList);

        MaSplitterUtils::insertWidgetWithScale(nameAndSequenceAreasSplitter, similarityStatistics, 0.04, nameAreaContainer, 1);
    } else {
        similarityStatistics->show();
    }
}

void MsaEditorWgt::hideSimilarity() {
    if (nullptr != similarityStatistics) {
        similarityStatistics->hide();
        similarityStatistics->cancelPendingTasks();
    }
}

const MsaEditorAlignmentDependentWidget* MsaEditorWgt::getSimilarityWidget() {
    return similarityStatistics;
}

void MsaEditorWgt::initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) {
    sequenceArea = new MSAEditorSequenceArea(this, shBar, cvBar);
}

void MsaEditorWgt::initOverviewArea(MaEditorOverviewArea *_overviewArea) {
    Q_ASSERT(_overviewArea);
    overviewArea = _overviewArea;
}

void MsaEditorWgt::initNameList(QScrollBar* nhBar) {
    nameList = new MsaEditorNameList(this, nhBar);
}

void MsaEditorWgt::initConsensusArea() {
    consensusArea = new MSAEditorConsensusArea(this);
}

void MsaEditorWgt::initStatusBar(MaEditorStatusBar *_statusBar) {
    Q_ASSERT(_statusBar);
    statusBar = _statusBar;
}

MSAEditorTreeViewer* MsaEditorWgt::getCurrentTree() const {
    if (nullptr == multiTreeViewer) {
        return nullptr;
    }
    GObjectViewWindow* page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    if (nullptr == page) {
        return nullptr;
    }
    return qobject_cast<MSAEditorTreeViewer*>(page->getObjectView());
}

MSAEditorMultiTreeViewer* MsaEditorWgt::getMultiTreeViewer() {
    return multiTreeViewer;
}

QSize MsaEditorWgt::sizeHint() const
{
    QSize s = QWidget::sizeHint();
    return QSize(s.width(), minimumSizeHint().height());
}

QSize MsaEditorWgt::minimumSizeHint() const
{
    QSize s = QWidget::sizeHint();
    int newHeight = consensusArea->size().height() +
                    /*sequenceArea->size().height() +*/
                    rowHeightController->getTotalAlignmentHeight() +
                    scrollController->getHorizontalScrollBar()->size().height() +
                    2;
    return QSize(s.width(), newHeight);
}

}  // namespace U2
