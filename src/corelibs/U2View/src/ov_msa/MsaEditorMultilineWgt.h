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

#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>

#include <U2Core/global.h>

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
class MaEditorSequenceArea;
class MultilineScrollController;

// Helper function to properly support widget sizes.
namespace MsaSizeUtil {
void updateMinHeightIfPossible(MaEditorSequenceArea* heightFrom, QWidget* setTo);
}

class U2VIEW_EXPORT MsaEditorMultilineWgt : public QWidget {
    Q_OBJECT

public:
    MsaEditorMultilineWgt(MsaEditor* editor, QWidget* parent, bool isWrapMode);

    MsaEditor* getEditor() const;
    MsaEditorOverviewArea* getOverviewArea() const;
    MsaEditorStatusBar* getStatusBar();

    MultilineScrollController* getScrollController() const;
    QScrollArea* getChildrenScrollArea() const;

    MsaEditorWgt* getLineWidget(int index) const;
    void updateSize();

    void addPhylTreeWidget(MsaEditorMultiTreeViewer* newMultiTreeViewer);
    void delPhylTreeWidget();
    MsaEditorMultiTreeViewer* getPhylTreeWidget() const;
    MsaEditorTreeViewer* getCurrentTree() const;

    void setSimilaritySettings(const SimilarityStatisticsSettings* settings);
    void refreshSimilarityColumn();
    void showSimilarity();
    void hideSimilarity();

    bool moveSelection(int key, bool shift, bool ctrl);

    MsaEditorStatusBar* getStatusBar() const;

    // Return MaEditorWgt widget which has input focus
    MsaEditorWgt* getActiveChild() const;
    void setActiveChild(MsaEditorWgt* child);

    // Return lines count in multiline widget
    int getLineWidgetCount() const;

    // Current multiline mode
    bool isWrapMode() const;

    int getSequenceAreaWidth(int index) const;
    int getFirstVisibleBase(int index) const;
    int getLastVisibleBase(int index) const;
    int getSequenceAreaBaseLen(int index) const;
    int getSequenceAreaBaseWidth(int index) const;
    int getSequenceAreaAllBaseLen() const;
    int getSequenceAreaAllBaseWidth() const;

    // Set multiline mode
    // If mode was changed return true
    // Else return false
    bool setMultilineMode(bool enabled);

    int getUIIndex(MsaEditorWgt* _ui) const;

public slots:
    void sl_changeColorSchemeOutside(const QString& id);
    void sl_changeColorScheme(const QString& id);
    void sl_onPosChangeRequest(int position);
    void sl_triggerUseDots(int checkState);
    void sl_cursorPositionChanged(const QPoint&);
    void sl_setAllNameAndSequenceAreasSplittersSizes(int pos, int index);
    void sl_goto();
    void sl_toggleSequenceRowOrder(bool isOrderBySequence);

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();
    void si_maEditorUIChanged();

protected:
    void initWidgets();
    void initScrollArea();
    void initOverviewArea();
    void initStatusBar();
    void initChildrenArea();
    void createChildren();
    void updateChildren();
    MsaEditorWgt* createChild(MsaEditor* editor, MsaEditorOverviewArea* overviewArea, MsaEditorStatusBar* statusBar);
    void addChild(MsaEditorWgt* child);

public:
    MsaEditor* const editor;

private:
    MsaEditorMultiTreeViewer* multiTreeViewer = nullptr;
    MsaEditorTreeViewer* treeViewer = nullptr;
    MultilineScrollController* scrollController = nullptr;

    QScrollArea* scrollArea = nullptr;  // scroll area for multiline widget, it's widget is uiChildrenArea
    QGroupBox* uiChildrenArea = nullptr;
    MsaEditorOverviewArea* overviewArea = nullptr;

    bool treeView = false;
    QSplitter* treeSplitter;

    QVector<MsaEditorWgt*> uiChild;

    // For correct display of Overview. `wgt` may have already been removed, or may still exist, so we need handles.
    struct ActiveChild {
        MsaEditorWgt* wgt = nullptr;
        QMetaObject::Connection startChangingHandle;
        QMetaObject::Connection stopChangingHandle;
    };

    ActiveChild activeChild;
    int uiChildLength = 0;
    int uiChildCount = 0;
    bool multilineMode = false;
    MsaEditorStatusBar* statusBar = nullptr;
};

}  // namespace U2
