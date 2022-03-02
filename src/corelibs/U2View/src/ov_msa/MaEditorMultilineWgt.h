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

#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QWidget>

#include <U2Core/global.h>

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
class MsaUndoRedoFramework;
class MultilineScrollController;
class SequenceAreaRenderer;
class MSAEditorMultiTreeViewer;

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorMultilineWgt : public QWidget {
    Q_OBJECT
public:
    MaEditorMultilineWgt(MaEditor *editor);

    /** Returns MA editor instance. The instance is always defined and is never null. */
    MaEditor *getEditor() const;

    MaEditorOverviewArea *getOverviewArea() const {
        return overviewArea;
    }

    MaEditorStatusBar *getStatusBar() const;

    MultilineScrollController *getScrollController() const {
        return scrollController;
    }

    QScrollArea *getChildrenScrollArea() const {
        return scrollArea;
    }

    /* If 'true' and collapse group has only 1 row it will have expand/collapse control. */
    bool isCollapsingOfSingleRowGroupsEnabled() const { return enableCollapsingOfSingleRowGroups; }

    virtual MaEditorWgt *getUI(uint index = 0) const
    {
        if (index < uiChildCount && index < uiChildLength) {
            return uiChild[index];
        }
        return nullptr;
    }

    virtual uint getUIIndex(MaEditorWgt *_ui) const
    {
        if (_ui == nullptr) {
            return 0;
        }
        for (uint index = 0; index < uiChildCount && index < uiChildLength; index++) {
            if (_ui == uiChild[index]) {
                return index;
            }
        }
        return 0;
    }

    int getSequenceAreaWidth(uint index = 0) const;       // pixels
    int getFirstVisibleBase(uint index = 0) const;
    int getLastVisibleBase(uint index = 0) const;
    int getSequenceAreaBaseLen(uint index = 0) const;     // bases
    int getSequenceAreaBaseWidth(uint index = 0) const;   // pixels
    int getSequenceAreaAllBaseLen() const;                // bases
    int getSequenceAreaAllBaseWidth() const;              // pixels

    virtual MaEditorWgt *createChild(MaEditor *editor,
                                     MaEditorOverviewArea *overviewArea,
                                     MaEditorStatusBar *statusBar) = 0;
    virtual void deleteChild(int index) = 0;
    virtual void addChild(MaEditorWgt *child, int index = -1) = 0;
    virtual bool updateChildrenCount() = 0;

    uint getChildrenCount() const {
        return uiChildCount;
    }

    bool getMultilineMode() const {
        return multilineMode;
    }

    bool setMultilineMode(bool newmode);

    MaEditorWgt *getActiveChild();
    void setActiveChild(MaEditorWgt *child);

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();

public slots:
    /** Switches between Original and Sequence row orders. */
    void sl_toggleSequenceRowOrder(bool isOrderBySequence);

private slots:

protected:
    virtual void initWidgets();
    virtual void initActions();

    virtual void createChildren() = 0;
    virtual void updateChildren() = 0;

    virtual void initScrollArea(QScrollArea *_scrollArea = nullptr) = 0;
    virtual void initOverviewArea(MaEditorOverviewArea *overviewArea = nullptr) = 0;
    virtual void initStatusBar(MaEditorStatusBar *statusbar = nullptr) = 0;
    virtual void initChildrenArea(QGroupBox *_uiChildrenArea = nullptr) = 0;

protected:
    MaEditor *const editor;
    QScrollArea *scrollArea; // scroll area for multiline widget, it's widget is uiChildrenArea
    QGroupBox *uiChildrenArea;
    MaEditorOverviewArea *overviewArea;
    MaEditorStatusBar *statusBar;

    bool treeView = false;
    QSplitter *treeSplitter;

    QVector<MaEditorWgt *> uiChild;
    MaEditorWgt *activeChild = nullptr;
    uint uiChildLength = 0;
    uint uiChildCount = 0;
    bool multilineMode = false;

    bool enableCollapsingOfSingleRowGroups;
    MultilineScrollController *scrollController;

public:

};

}  // namespace U2

#endif  // _U2_MA_EDITOR_MULTILINE_WGT_H_
