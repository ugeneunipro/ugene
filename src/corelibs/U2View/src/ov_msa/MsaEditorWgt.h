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

#ifndef _U2_MSA_EDITOR_WGT_H_
#define _U2_MSA_EDITOR_WGT_H_

#include "MaEditorWgt.h"

namespace U2 {

class GObjectViewWindow;
class MSADistanceMatrix;
class MSAEditor;
class MsaEditorAlignmentDependentWidget;
class MSAEditorMultiTreeViewer;
class MSAEditorOverviewArea;
class MsaEditorSimilarityColumn;
class MSAEditorTreeViewer;
class SimilarityStatisticsSettings;

class U2VIEW_EXPORT MsaEditorWgt : public MaEditorWgt {
    Q_OBJECT
    // todo: make public accessors:
    friend class MSAEditorTreeViewer;
    friend class MsaEditorSimilarityColumn;

public:
    MsaEditorWgt(MSAEditor *editor,
                 MaEditorOverviewArea *overview = nullptr,
                 MaEditorStatusBar *statusbar = nullptr);

    MSAEditor* getEditor() const;

    MSAEditorSequenceArea* getSequenceArea() const;

    void createDistanceColumn(MSADistanceMatrix* matrix);

    void addTreeView(GObjectViewWindow* treeView);

    void setSimilaritySettings(const SimilarityStatisticsSettings* settings);

    void refreshSimilarityColumn();

    void showSimilarity();
    void hideSimilarity();

    const MsaEditorAlignmentDependentWidget* getSimilarityWidget();

    MSAEditorTreeViewer* getCurrentTree() const;

    MSAEditorMultiTreeViewer* getMultiTreeViewer();

    void initOverviewArea(MaEditorOverviewArea *overviewArea = nullptr) override;
    void initStatusBar(MaEditorStatusBar *statusBar = nullptr) override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    void sl_onTabsCountChanged(int tabsCount);
signals:
    void si_showTreeOP();
    void si_hideTreeOP();

protected:
    void initSeqArea(GScrollBar *shBar, GScrollBar *cvBar);
    void initNameList(QScrollBar *nhBar);
    void initConsensusArea();

private:
    MsaEditorSimilarityColumn* dataList;
    MSAEditorMultiTreeViewer* multiTreeViewer;
    MsaEditorAlignmentDependentWidget* similarityStatistics;
    MSAEditorTreeViewer* treeViewer;
};

}  // namespace U2

#endif  // _U2_MSA_EDITOR_WGT_H_
