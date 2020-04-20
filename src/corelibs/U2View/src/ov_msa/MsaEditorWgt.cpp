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

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include "MSAEditor.h"
#include "MSAEditorConsensusArea.h"
#include "MsaEditorNameList.h"
#include "MSAEditorOverviewArea.h"
#include "MSAEditorSequenceArea.h"
#include "MsaEditorSimilarityColumn.h"
#include "MsaEditorStatusBar.h"
#include "MsaEditorWgt.h"
#include "helpers/MsaRowHeightController.h"
#include "PhyTrees/MSAEditorMultiTreeViewer.h"

namespace U2 {

MsaEditorWgt::MsaEditorWgt(MSAEditor* editor)
    : MaEditorWgt(editor),
      multiTreeViewer(NULL),
      similarityStatistics(NULL) {
    rowHeightController = new MsaRowHeightController(this);
    initActions();
    initWidgets();
}

MSAEditor *MsaEditorWgt::getEditor() const {
    return qobject_cast<MSAEditor* >(editor);
}

MSAEditorSequenceArea* MsaEditorWgt::getSequenceArea() const {
    return qobject_cast<MSAEditorSequenceArea* >(seqArea);
}

void MsaEditorWgt::sl_onTabsCountChanged(int curTabsNumber) {
    if(curTabsNumber < 1) {
        maSplitter.removeWidget(multiTreeViewer);
        delete multiTreeViewer;
        multiTreeViewer = NULL;
        emit si_hideTreeOP();
        nameList->clearGroupsSelections();
    }
}

void MsaEditorWgt::createDistanceColumn(MSADistanceMatrix* matrix) {
    dataList->setMatrix(matrix);
    dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    MsaEditorAlignmentDependentWidget* statisticsWidget = new MsaEditorAlignmentDependentWidget(dataList);

    maSplitter.addWidget(nameAreaContainer, statisticsWidget, 0.04, 1);
}

void MsaEditorWgt::addTreeView(GObjectViewWindow* treeView) {
    if (NULL == multiTreeViewer) {
        multiTreeViewer = new MSAEditorMultiTreeViewer(tr("Tree view"), getEditor());
        maSplitter.addWidget(nameAreaContainer, multiTreeViewer, 0.35);
        multiTreeViewer->addTreeView(treeView);
        emit si_showTreeOP();
        connect(multiTreeViewer, SIGNAL(si_tabsCountChanged(int)), SLOT(sl_onTabsCountChanged(int)));
    }
    else {
        multiTreeViewer->addTreeView(treeView);
    }
}

void MsaEditorWgt::setSimilaritySettings( const SimilarityStatisticsSettings* settings ) {
    similarityStatistics->setSettings(settings);
}

void MsaEditorWgt::refreshSimilarityColumn() {
    dataList->updateWidget();
}

void MsaEditorWgt::showSimilarity() {
    if(NULL == similarityStatistics) {
        SimilarityStatisticsSettings settings;
        settings.ma = getEditor()->getMaObject();
        settings.algoId = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmIds().at(0);
        settings.ui = this;

        dataList = new MsaEditorSimilarityColumn(this, new QScrollBar(Qt::Horizontal), &settings);
        dataList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        similarityStatistics = new MsaEditorAlignmentDependentWidget(dataList);

        maSplitter.addWidget(nameAreaContainer, similarityStatistics, 0.04, 1);
    }
    else {
        similarityStatistics->show();
    }

}

void MsaEditorWgt::hideSimilarity() {
    if(NULL != similarityStatistics) {
        similarityStatistics->hide();
        similarityStatistics->cancelPendingTasks();
    }
}

const MsaEditorAlignmentDependentWidget *MsaEditorWgt::getSimilarityWidget() {
    return similarityStatistics;
}

void MsaEditorWgt::initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) {
    seqArea = new MSAEditorSequenceArea(this, shBar, cvBar);
}

void MsaEditorWgt::initOverviewArea() {
    overviewArea = new MSAEditorOverviewArea(this);
}

void MsaEditorWgt::initNameList(QScrollBar *nhBar) {
    nameList = new MsaEditorNameList(this, nhBar);
}

void MsaEditorWgt::initConsensusArea() {
    consArea = new MSAEditorConsensusArea(this);
}

void MsaEditorWgt::initStatusBar() {
    statusBar = new MsaEditorStatusBar(editor->getMaObject(), seqArea);
}

MSAEditorTreeViewer* MsaEditorWgt::getCurrentTree() const
{
    if(NULL == multiTreeViewer) {
        return NULL;
    }
    GObjectViewWindow* page = qobject_cast<GObjectViewWindow*>(multiTreeViewer->getCurrentWidget());
    if(NULL == page) {
        return NULL;
    }
    return qobject_cast<MSAEditorTreeViewer*>(page->getObjectView());
}

MSAEditorMultiTreeViewer *MsaEditorWgt::getMultiTreeViewer(){
    return multiTreeViewer;
}

}   // namespace U2
