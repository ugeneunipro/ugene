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

    MaEditor* getEditor() const override;
    MaEditorOverviewArea* getOverviewArea() const override;
    MaEditorStatusBar* getStatusBar();

    MultilineScrollController* getScrollController() const override;
    QScrollArea* getChildrenScrollArea() const override;

    MaEditorWgt* getLineWidget(int index) const override;
    void updateSize() override;

    void addPhylTreeWidget(MsaEditorMultiTreeViewer* newMultiTreeViewer);
    void delPhylTreeWidget();
    MsaEditorMultiTreeViewer* getPhylTreeWidget() const;
    MsaEditorTreeViewer* getCurrentTree() const;

    void setSimilaritySettings(const SimilarityStatisticsSettings* settings) override;
    void refreshSimilarityColumn() override;
    void showSimilarity() override;
    void hideSimilarity() override;

    bool moveSelection(int key, bool shift, bool ctrl) override;

    MaEditorStatusBar* getStatusBar() const override;

    // Return MaEditorWgt widget which has input focus
    MaEditorWgt* getActiveChild() const override;
    void setActiveChild(MaEditorWgt* child) override;

    // Return lines count in multiline widget
    int getChildrenCount() const override;

    // Current multiline mode
    bool getMultilineMode() const override;

    int getSequenceAreaWidth(int index) const override;
    int getFirstVisibleBase(int index) const override;
    int getLastVisibleBase(int index) const override;
    int getSequenceAreaBaseLen(int index) const override;
    int getSequenceAreaBaseWidth(int index) const override;
    int getSequenceAreaAllBaseLen() const override;
    int getSequenceAreaAllBaseWidth() const override;

    // Set multiline mode
    // If mode was changed return true
    // Else return false
    bool setMultilineMode(bool enabled) override;

    int getUIIndex(MaEditorWgt* _ui) const override;

public slots:
    void sl_changeColorSchemeOutside(const QString& id);
    void sl_changeColorScheme(const QString& id);
    void sl_onPosChangeRequest(int position);
    void sl_triggerUseDots(int checkState);
    void sl_cursorPositionChanged(const QPoint&);
    void sl_setAllNameAndSequenceAreasSplittersSizes(int pos, int index);
    void sl_goto() override;
    void sl_toggleSequenceRowOrder(bool isOrderBySequence) override;

protected:
    void initWidgets() override;
    void initScrollArea() override;
    void initOverviewArea() override;
    void initStatusBar() override;
    void initChildrenArea() override;
    void createChildren() override;
    void updateChildren() override;
    MaEditorWgt* createChild(MaEditor* editor,
                             MaEditorOverviewArea* overviewArea,
                             MaEditorStatusBar* statusBar) override;
    void addChild(MaEditorWgt* child) override;

public:
    MsaEditor* const editor;

private:
    MsaEditorMultiTreeViewer* multiTreeViewer = nullptr;
    MsaEditorTreeViewer* treeViewer = nullptr;
    MultilineScrollController* scrollController = nullptr;

    QScrollArea* scrollArea = nullptr;  // scroll area for multiline widget, it's widget is uiChildrenArea
    QGroupBox* uiChildrenArea = nullptr;
    MaEditorOverviewArea* overviewArea = nullptr;

    bool treeView = false;
    QSplitter* treeSplitter;

    QVector<MaEditorWgt*> uiChild;

    // For correct display of Overview. `wgt` may have already been removed, or may still exist, so we need handles.
    struct ActiveChild {
        MaEditorWgt* wgt = nullptr;
        QMetaObject::Connection startChangingHandle;
        QMetaObject::Connection stopChangingHandle;
    };

    ActiveChild activeChild;
    int uiChildLength = 0;
    int uiChildCount = 0;
    bool multilineMode = false;
    MaEditorStatusBar* statusBar = nullptr;
};

}  // namespace U2
