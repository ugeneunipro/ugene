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

#pragma once

#include "MaEditorMultilineWgt.h"

namespace U2 {

class GObjectViewWindow;
class MsaDistanceMatrix;
class MsaEditor;
class MsaEditorAlignmentDependentWidget;
class MsaEditorMultiTreeViewer;
class MsaEditorWgt;
class MsaEditorOverviewArea;
class MsaEditorStatusBar;
class MsaEditorSimilarityColumn;
class MsaEditorTreeViewer;
class MsaMultilineScrollArea;
class SimilarityStatisticsSettings;

// Helper function to properly support widget sizes.
namespace MsaSizeUtil {
void updateMinHeightIfPossible(MaEditorSequenceArea* heightFrom, QWidget* setTo);
}

class U2VIEW_EXPORT MsaEditorMultilineWgt : public MaEditorMultilineWgt {
    Q_OBJECT

public:
    MsaEditorMultilineWgt(MsaEditor* editor, QWidget* parent, bool multiline);

    MsaEditor* getEditor() const;
    MaEditorOverviewArea* getOverview();
    MaEditorStatusBar* getStatusBar();

    MaEditorWgt* getUI(int index) const override;
    void updateSize() override;

    void addPhylTreeWidget(MsaEditorMultiTreeViewer* newMultiTreeViewer);
    void delPhylTreeWidget();
    MsaEditorMultiTreeViewer* getPhylTreeWidget() const {
        return multiTreeViewer;
    };
    MsaEditorTreeViewer* getCurrentTree() const;

    void setSimilaritySettings(const SimilarityStatisticsSettings* settings) override;
    void refreshSimilarityColumn() override;
    void showSimilarity() override;
    void hideSimilarity() override;

    bool moveSelection(int key, bool shift, bool ctrl) override;

signals:

public slots:
    void sl_changeColorSchemeOutside(const QString& id);
    void sl_changeColorScheme(const QString& id);
    void sl_onPosChangeRequest(int position);
    void sl_triggerUseDots(int checkState);
    void sl_cursorPositionChanged(const QPoint&);
    void sl_setAllNameAndSequenceAreasSplittersSizes(int pos, int index);
    void sl_goto() override;

protected:
    void initScrollArea() override;
    void initOverviewArea() override;
    void initStatusBar() override;
    void initChildrenArea() override;
    void createChildren() override;
    void updateChildren() override;
    MaEditorWgt* createChild(MaEditor* editor,
                             MaEditorOverviewArea* overviewArea,
                             MaEditorStatusBar* statusBar) override;
    void deleteChild(int index) override;
    void addChild(MaEditorWgt* child) override;

private:
    MsaEditorMultiTreeViewer* multiTreeViewer;
    MsaEditorTreeViewer* treeViewer;
};

}  // namespace U2
