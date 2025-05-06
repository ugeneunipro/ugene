/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/MsaDistanceAlgorithmRegistry.h>

#include "MaEditorSplitters.h"
#include "MsaEditor.h"
#include "MsaEditorConsensusArea.h"
#include "MsaEditorNameList.h"
#include "MsaEditorOverviewArea.h"
#include "MsaEditorSequenceArea.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "MsaRowHeightController.h"
#include "ScrollController.h"
#include "phy_tree/MsaEditorMultiTreeViewer.h"
#include "phy_tree/MsaEditorTreeViewer.h"

namespace U2 {

MsaEditorWgt::MsaEditorWgt(MsaEditor* editor,
                           QWidget* parent,
                           MaEditorOverviewArea* overview,
                           MaEditorStatusBar* statusbar)
    : MaEditorWgt(editor, parent) {
    overviewArea = overview;
    statusBar = statusbar;
    rowHeightController = new MsaRowHeightController(this);
    initActions();
    initWidgets(false, false);

    // For active MaEditorWgt tracking
    this->setAttribute(Qt::WA_Hover, true);
    eventFilter = new MsaEditorWgtEventFilter(this);
    this->installEventFilter(eventFilter);

    setMinimumSize(minimumSizeHint());
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

MsaEditor* MsaEditorWgt::getEditor() const {
    return qobject_cast<MsaEditor*>(editor);
}

MsaEditorSequenceArea* MsaEditorWgt::getSequenceArea() const {
    return qobject_cast<MsaEditorSequenceArea*>(sequenceArea);
}

void MsaEditorWgt::sl_onTabsCountChanged(int curTabsNumber) {
    if (curTabsNumber < 1) {
        qobject_cast<MsaEditorMultilineWgt*>(getEditor()->getMainWidget())->delPhylTreeWidget();
        emit si_hideTreeOP();
    }
}

void MsaEditorWgt::createDistanceColumn(MsaDistanceMatrix* matrix) {
    dataList->setMatrix(matrix);
    dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    auto statisticsWidget = new MsaEditorAlignmentDependentWidget(this, dataList);

    MaSplitterUtils::insertWidgetWithScale(nameAndSequenceAreasSplitter, statisticsWidget, 0.04, nameAreaContainer, 1);
}

void MsaEditorWgt::addTreeView(GObjectViewWindow* treeView) {
    auto mui = qobject_cast<MsaEditorMultilineWgt*>(getEditor()->getMainWidget());

    if (mui->getPhylTreeWidget() == nullptr) {
        auto multiTreeViewer = new MsaEditorMultiTreeViewer(tr("Tree view"), getEditor());

        mui->addPhylTreeWidget(multiTreeViewer);
        multiTreeViewer->addTreeView(treeView);
        multiTreeViewer->setMinimumWidth(250);
        emit si_showTreeOP();
        connect(multiTreeViewer, SIGNAL(si_tabsCountChanged(int)), SLOT(sl_onTabsCountChanged(int)));
    } else {
        mui->getPhylTreeWidget()->addTreeView(treeView);
    }
}

void MsaEditorWgt::setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
    similarityStatisticsSettings = SimilarityStatisticsSettings(*settings);
    emit si_similaritySettingsChanged(similarityStatisticsSettings);
}

const SimilarityStatisticsSettings* MsaEditorWgt::getSimilaritySettings() {
    if (similarityStatistics != nullptr) {
        return static_cast<const SimilarityStatisticsSettings*>(
            similarityStatistics->getSettings());
    }
    return &similarityStatisticsSettings;
}

void MsaEditorWgt::refreshSimilarityColumn() {
    dataList->updateWidget();
}

void MsaEditorWgt::showSimilarity() {
    if (similarityStatistics == nullptr) {
        similarityStatisticsSettings.algoId = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmIds().at(0);

        dataList = new MsaEditorSimilarityColumn(this, &similarityStatisticsSettings);
        dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        similarityStatistics = new MsaEditorAlignmentDependentWidget(this, dataList);

        MaSplitterUtils::insertWidgetWithScale(nameAndSequenceAreasSplitter, similarityStatistics, 0.04, nameAreaContainer, 1);
    } else {
        similarityStatistics->show();
    }
}

void MsaEditorWgt::hideSimilarity() {
    if (similarityStatistics != nullptr) {
        similarityStatistics->hide();
        similarityStatistics->cancelPendingTasks();
    }
}

MsaEditorAlignmentDependentWidget* MsaEditorWgt::getSimilarityWidget() const {
    return similarityStatistics;
}

void MsaEditorWgt::initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) {
    sequenceArea = new MsaEditorSequenceArea(this, shBar, cvBar);
}

void MsaEditorWgt::initOverviewArea(MaEditorOverviewArea* _overviewArea) {
    Q_ASSERT(_overviewArea);
    overviewArea = _overviewArea;
}

void MsaEditorWgt::initNameList(QScrollBar* nhBar) {
    nameList = new MsaEditorNameList(this, nhBar);
}

void MsaEditorWgt::initConsensusArea() {
    consensusArea = new MsaEditorConsensusArea(this);
}

void MsaEditorWgt::initStatusBar(MaEditorStatusBar* _statusBar) {
    Q_ASSERT(_statusBar);
    statusBar = _statusBar;
}

MsaEditorTreeViewer* MsaEditorWgt::getCurrentTree() const {
    CHECK(multiTreeViewer != nullptr, nullptr);

    auto page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    CHECK(page != nullptr, nullptr);

    return qobject_cast<MsaEditorTreeViewer*>(page->getObjectView());
}

MsaEditorMultiTreeViewer* MsaEditorWgt::getMultiTreeViewer() const {
    return qobject_cast<MsaEditorMultilineWgt*>(getEditor()->getMainWidget())->getPhylTreeWidget();
}

QSize MsaEditorWgt::sizeHint() const {
    QSize s = QWidget::sizeHint();
    if (editor->isMultilineMode()) {
        return QSize(s.width(), minimumSizeHint().height());
    }
    return s;
}

QSize MsaEditorWgt::minimumSizeHint() const {
    QSize s = QWidget::minimumSizeHint();
    if (editor->isMultilineMode()) {
        int newHeight = consensusArea->size().height() +
                        qMax(qMax(sequenceArea->minimumSizeHint().height(),
                                  nameList->minimumSizeHint().height()),
                             (editor->getRowHeight() + 1)) +
                        5;
        return QSize(s.width(), newHeight);
    }
    return s;
}


bool MsaEditorWgtEventFilter::eventFilter(QObject* obj, QEvent* event) {
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

}  // namespace U2
