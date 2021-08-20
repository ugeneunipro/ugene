/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
#include "MsaEditorNameList.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "helpers/MsaRowHeightController.h"
#include "phy_tree/MSAEditorMultiTreeViewer.h"

namespace U2 {

MsaEditorWgt::MsaEditorWgt(MSAEditor *editor)
    : MaEditorWgt(editor),
      multiTreeViewer(nullptr),
      similarityStatistics(nullptr) {
    rowHeightController = new MsaRowHeightController(this);
    initActions();
    initWidgets();
}

MSAEditor *MsaEditorWgt::getEditor() const {
    return qobject_cast<MSAEditor *>(editor);
}

MSAEditorSequenceArea *MsaEditorWgt::getSequenceArea() const {
    return qobject_cast<MSAEditorSequenceArea *>(sequenceArea);
}

void MsaEditorWgt::sl_onTabsCountChanged(int curTabsNumber) {
    if (curTabsNumber < 1) {
        maSplitter.removeWidget(multiTreeViewer);
        delete multiTreeViewer;
        multiTreeViewer = nullptr;
        emit si_hideTreeOP();
    }
}

void MsaEditorWgt::createDistanceColumn(MSADistanceMatrix *matrix) {
    dataList->setMatrix(matrix);
    dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    MsaEditorAlignmentDependentWidget *statisticsWidget = new MsaEditorAlignmentDependentWidget(dataList);

    maSplitter.addWidget(nameAreaContainer, statisticsWidget, 0.04, 1);
}

void MsaEditorWgt::addTreeView(GObjectViewWindow *treeView) {
    if (nullptr == multiTreeViewer) {
        multiTreeViewer = new MSAEditorMultiTreeViewer(tr("Tree view"), getEditor());
        maSplitter.addWidget(nameAreaContainer, multiTreeViewer, 0.35);
        multiTreeViewer->addTreeView(treeView);
        emit si_showTreeOP();
        connect(multiTreeViewer, SIGNAL(si_tabsCountChanged(int)), SLOT(sl_onTabsCountChanged(int)));
    } else {
        multiTreeViewer->addTreeView(treeView);
    }
}

void MsaEditorWgt::setSimilaritySettings(const SimilarityStatisticsSettings *settings) {
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

        maSplitter.addWidget(nameAreaContainer, similarityStatistics, 0.04, 1);
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

const MsaEditorAlignmentDependentWidget *MsaEditorWgt::getSimilarityWidget() {
    return similarityStatistics;
}

void MsaEditorWgt::initSeqArea(GScrollBar *shBar, GScrollBar *cvBar) {
    sequenceArea = new MSAEditorSequenceArea(this, shBar, cvBar);
}

void MsaEditorWgt::initOverviewArea() {
    overviewArea = new MSAEditorOverviewArea(this);
}

void MsaEditorWgt::initNameList(QScrollBar *nhBar) {
    nameList = new MsaEditorNameList(this, nhBar);
}

void MsaEditorWgt::initConsensusArea() {
    consensusArea = new MSAEditorConsensusArea(this);
}

void MsaEditorWgt::initStatusBar() {
    statusBar = new MsaEditorStatusBar(getEditor());
}

MSAEditorTreeViewer *MsaEditorWgt::getCurrentTree() const {
    if (nullptr == multiTreeViewer) {
        return nullptr;
    }
    GObjectViewWindow *page = qobject_cast<GObjectViewWindow *>(multiTreeViewer->getCurrentWidget());
    if (nullptr == page) {
        return nullptr;
    }
    return qobject_cast<MSAEditorTreeViewer *>(page->getObjectView());
}

MSAEditorMultiTreeViewer *MsaEditorWgt::getMultiTreeViewer() {
    return multiTreeViewer;
}

}  // namespace U2
