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

#ifndef _U2_MSA_EDITOR_MULTILINE_WGT_H_
#define _U2_MSA_EDITOR_MULTILINE_WGT_H_

#include "MaEditorMultilineWgt.h"

namespace U2 {

class GObjectViewWindow;
class MSADistanceMatrix;
class MSAEditor;
class MsaEditorAlignmentDependentWidget;
class MSAEditorMultiTreeViewer;
class MsaEditorWgt;
class MSAEditorOverviewArea;
class MsaEditorStatusBar;
class MsaEditorSimilarityColumn;
class MSAEditorTreeViewer;
class MsaMultilineScrollArea;
class SimilarityStatisticsSettings;

class U2VIEW_EXPORT MsaEditorMultilineWgt : public MaEditorMultilineWgt {
    Q_OBJECT

public:
    MsaEditorMultilineWgt(MSAEditor *editor, bool multiline);

    MSAEditor *getEditor() const;
    MaEditorOverviewArea *getOverview();
    MaEditorStatusBar *getStatusBar();

    MaEditorWgt *getUI(uint index = 0) const override;
    void updateSize(bool recurse = true) override;

    void addPhylTreeWidget(MSAEditorMultiTreeViewer *multiTreeViewer);
    void delPhylTreeWidget();
    MSAEditorMultiTreeViewer *getPhylTreeWidget() const {
        return multiTreeViewer;
    };
    MSAEditorTreeViewer* getCurrentTree() const;

signals:
    void si_showOffsets(bool);

public slots:
    void sl_changeColorSchemeOutside(const QString& id);

protected:
    void initScrollArea(QScrollArea *_scrollArea = nullptr) override;
    void initOverviewArea(MaEditorOverviewArea *overviewArea = nullptr) override;
    void initStatusBar(MaEditorStatusBar *_statusBar = nullptr) override;
    void initChildrenArea(QGroupBox *_uiChildrenArea) override;
    void createChildren() override;
    void updateChildren() override;
    MaEditorWgt *createChild(MaEditor *editor,
                             MaEditorOverviewArea *overviewArea,
                             MaEditorStatusBar *statusBar) override;
    void deleteChild(int index) override;
    void addChild(MaEditorWgt *child, int index = -1) override;
    bool updateChildrenCount() override;

private:
    MSAEditorMultiTreeViewer *multiTreeViewer;
    MSAEditorTreeViewer* treeViewer;

};

}  // namespace U2

#endif  // _U2_MSA_EDITOR_MULTILINE_WGT_H_