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

#ifndef _U2_MA_EDITOR_MULTILINE_WGT_H_
#define _U2_MA_EDITOR_MULTILINE_WGT_H_

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Gui/PositionSelector.h>

#include "MaEditorUtils.h"

class QGridLayout;
class QGroupBox;
class QScrollBar;
class QVBoxLayout;

namespace U2 {

class DrawHelper;
class GScrollBar;
class MaEditorConsensusArea;
class MSAEditorOffsetsViewController;
class MaEditorStatusBar;
class MaEditor;
class MaEditorNameList;
class MaEditorOverviewArea;
class MaEditorSequenceArea;
class RowHeightController;
class MultilineScrollController;
class SequenceAreaRenderer;
class SimilarityStatisticsSettings;
class MSAEditorMultiTreeViewer;

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorMultilineWgt : public QWidget {
    Q_OBJECT
public:
    explicit MaEditorMultilineWgt(MaEditor* editor);

    /** Returns MA editor instance. The instance is always defined and is never null. */
    MaEditor* getEditor() const;

    // Return overview area widget
    MaEditorOverviewArea* getOverviewArea() const;

    // Status bar widget
    MaEditorStatusBar* getStatusBar() const;

    // Get multiline scroll controller
    MultilineScrollController* getScrollController() const;

    // Get scroll area which contain all MaEditorWidget(s)
    QScrollArea* getChildrenScrollArea() const;

    // Get MaEditorWgt from multiline widget by index
    // Can be nullptr
    virtual MaEditorWgt* getUI(int index) const;

    // Get index of the known MaEditorWgt from multiline widget
    // If not found will be 0
    virtual int getUIIndex(MaEditorWgt* _ui) const;

    virtual void updateSize();

    int getSequenceAreaWidth(int index) const;  // pixels
    int getFirstVisibleBase(int index) const;
    int getLastVisibleBase(int index) const;
    int getSequenceAreaBaseLen(int index) const;  // bases
    int getSequenceAreaBaseWidth(int index) const;  // pixels
    int getSequenceAreaAllBaseLen() const;  // bases
    int getSequenceAreaAllBaseWidth() const;  // pixels

    virtual MaEditorWgt* createChild(MaEditor* editor,
                                     MaEditorOverviewArea* overviewArea,
                                     MaEditorStatusBar* statusBar) = 0;
    virtual void deleteChild(int index) = 0;
    virtual void addChild(MaEditorWgt* child) = 0;

    // Return lines count in multiline widget
    int getChildrenCount() const {
        return uiChildCount;
    }

    // Current multiline mode
    bool getMultilineMode() const {
        return multilineMode;
    }

    // Set multiline mode
    // If mode was changed return true
    // Else return false
    bool setMultilineMode(bool newmode);

    // Return MaEditorWgt widget which has input focus
    MaEditorWgt* getActiveChild();
    void setActiveChild(MaEditorWgt* child);

    virtual void setSimilaritySettings(const SimilarityStatisticsSettings* settings) {
        Q_UNUSED(settings);
    };
    virtual void refreshSimilarityColumn() {};
    virtual void showSimilarity() {};
    virtual void hideSimilarity() {};

    virtual bool moveSelection(int key, bool shift, bool ctrl) {
        Q_UNUSED(key);
        Q_UNUSED(shift);
        Q_UNUSED(ctrl);
        return false;
    }

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();
    void si_maEditorUIChanged();

public slots:
    /** Switches between Original and Sequence row orders. */
    void sl_toggleSequenceRowOrder(bool isOrderBySequence);
    virtual void sl_goto() {};

private slots:

protected:
    virtual void initWidgets();
    virtual void initActions();

    virtual void createChildren() = 0;
    virtual void updateChildren() = 0;

    virtual void initScrollArea() = 0;
    virtual void initOverviewArea() = 0;
    virtual void initStatusBar() = 0;
    virtual void initChildrenArea() = 0;

private:
    // For correct display of Overview. `wgt` may have already been removed, or may still exist, so we need handles.
    struct ActiveChild {
        MaEditorWgt* wgt = nullptr;
        QMetaObject::Connection startChangingHandle;
        QMetaObject::Connection stopChangingHandle;
    };

protected:
    MaEditor* const editor;
    QScrollArea* scrollArea = nullptr;  // scroll area for multiline widget, it's widget is uiChildrenArea
    QGroupBox* uiChildrenArea = nullptr;
    MaEditorOverviewArea* overviewArea = nullptr;
    MaEditorStatusBar* statusBar = nullptr;

    bool treeView = false;
    QSplitter* treeSplitter;

    QVector<MaEditorWgt*> uiChild;
    ActiveChild activeChild;
    int uiChildLength = 0;
    int uiChildCount = 0;
    bool multilineMode = false;

    MultilineScrollController* scrollController = nullptr;

public:
};

}  // namespace U2

#endif  // _U2_MA_EDITOR_MULTILINE_WGT_H_
