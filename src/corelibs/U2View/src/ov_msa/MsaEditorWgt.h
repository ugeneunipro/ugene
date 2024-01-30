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

#pragma once

#include "MaEditorWgt.h"

namespace U2 {

class GObjectViewWindow;
class MsaDistanceMatrix;
class MsaEditor;
class MsaEditorAlignmentDependentWidget;
class MsaEditorMultiTreeViewer;
class MsaEditorOverviewArea;
class MsaEditorSimilarityColumn;
class MsaEditorTreeViewer;
class SimilarityStatisticsSettings;
class MsaEditorWgtEventFilter;

class U2VIEW_EXPORT MsaEditorWgt : public MaEditorWgt {
    Q_OBJECT
    // todo: make public accessors:
    friend class MsaEditorTreeViewer;
    friend class MsaEditorSimilarityColumn;

public:
    MsaEditorWgt(MsaEditor* editor,
                 QWidget* parent,
                 MaEditorOverviewArea* overview = nullptr,
                 MaEditorStatusBar* statusbar = nullptr);

    MsaEditor* getEditor() const;

    MsaEditorSequenceArea* getSequenceArea() const;

    void createDistanceColumn(MsaDistanceMatrix* matrix);

    void addTreeView(GObjectViewWindow* treeView);

    void setSimilaritySettings(const SimilarityStatisticsSettings* settings);
    const SimilarityStatisticsSettings* getSimilaritySettings();

    void refreshSimilarityColumn();

    void showSimilarity();
    void hideSimilarity();

    MsaEditorAlignmentDependentWidget* getSimilarityWidget() const;

    MsaEditorTreeViewer* getCurrentTree() const;

    MsaEditorMultiTreeViewer* getMultiTreeViewer() const;

    void initOverviewArea(MaEditorOverviewArea* overviewArea = nullptr) override;
    void initStatusBar(MaEditorStatusBar* statusBar = nullptr) override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    MsaEditorWgtEventFilter* getEventFilter() const {
        return eventFilter;
    };

private slots:
    void sl_onTabsCountChanged(int tabsCount);
signals:
    void si_showTreeOP();
    void si_hideTreeOP();

protected:
    void initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) override;
    void initNameList(QScrollBar* nhBar) override;
    void initConsensusArea() override;

private:
    MsaEditorSimilarityColumn* dataList = nullptr;
    MsaEditorMultiTreeViewer* multiTreeViewer = nullptr;
    MsaEditorAlignmentDependentWidget* similarityStatistics = nullptr;
    MsaEditorWgtEventFilter* eventFilter = nullptr;


};

class U2VIEW_EXPORT MsaEditorWgtEventFilter : public QObject {
    Q_OBJECT
public:
    MsaEditorWgtEventFilter(MsaEditorWgt* maeditorwgt)
        : QObject(maeditorwgt), maEditorWgt(maeditorwgt) {
    }

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    MsaEditorWgt* maEditorWgt;
};


}  // namespace U2
