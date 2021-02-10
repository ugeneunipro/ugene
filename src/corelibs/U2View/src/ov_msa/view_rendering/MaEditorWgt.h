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

#ifndef _U2_MA_EDITOR_WGT_H_
#define _U2_MA_EDITOR_WGT_H_

#include <QWidget>

#include <U2Core/global.h>

#include "MaEditorUtils.h"

class QGridLayout;
class QScrollBar;
class QVBoxLayout;

namespace U2 {

class BaseWidthController;
class DrawHelper;
class GScrollBar;
class MaCollapseModel;
class MaEditorConsensusArea;
class MSAEditorOffsetsViewController;
class MaEditorStatusBar;
class MaEditor;
class MaEditorNameList;
class MaEditorOverviewArea;
class MaEditorSequenceArea;
class RowHeightController;
class MsaUndoRedoFramework;
class ScrollController;
class SequenceAreaRenderer;

/************************************************************************/
/* MaEditorWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorWgt : public QWidget {
    Q_OBJECT
public:
    MaEditorWgt(MaEditor *editor);

    QWidget *createHeaderLabelWidget(const QString &text = QString(),
                                     Qt::Alignment ali = Qt::AlignCenter,
                                     QWidget *heightTarget = NULL,
                                     bool proxyMouseEventsToNameList = true);

    MaEditor *getEditor() const {
        return editor;
    }

    MaEditorSequenceArea *getSequenceArea() const {
        return sequenceArea;
    }

    MaEditorNameList *getEditorNameList() const {
        return nameList;
    }

    MaEditorConsensusArea *getConsensusArea() const {
        return consensusArea;
    }

    MaEditorOverviewArea *getOverviewArea() const {
        return overviewArea;
    }

    MSAEditorOffsetsViewController *getOffsetsViewController() const {
        return offsetsViewController;
    }

    ScrollController *getScrollController() const {
        return scrollController;
    }

    BaseWidthController *getBaseWidthController() const {
        return baseWidthController;
    }

    RowHeightController *getRowHeightController() const {
        return rowHeightController;
    }

    DrawHelper *getDrawHelper() const {
        return drawHelper;
    }

    QAction *getUndoAction() const;

    QAction *getRedoAction() const;

    /* Returns 'true' if the virtual ordering/grouping mode is enabled. See docs for 'virtualOrderMode' for more details. */
    bool isVirtualOrderMode() const;

    /*
     * Sets virtual grouping mode ON or OFF.
     * A trivial method that updates the flag in the view model with no other actions/callbacks.
     */
    void setVirtualOrderMode(bool flag);

    MaCollapseModel *getCollapseModel() const {
        return collapseModel;
    }

    /* If 'true' and collapse group has only 1 row it will have expand/collapse control. */
    bool isCollapsingOfSingleRowGroupsEnabled() const {
        return enableCollapsingOfSingleRowGroups;
    }

    QWidget *getHeaderWidget() const {
        return seqAreaHeader;
    }

    MsaUndoRedoFramework *getUndoRedoFramework() const {
        return undoFWK;
    }

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();

public slots:
    void sl_saveScreenshot();

private slots:
    void sl_countUndo();
    void sl_countRedo();

protected:
    virtual void initWidgets();
    virtual void initActions();

    virtual void initSeqArea(GScrollBar *shBar, GScrollBar *cvBar) = 0;
    virtual void initOverviewArea() = 0;
    virtual void initNameList(QScrollBar *nhBar) = 0;
    virtual void initConsensusArea() = 0;
    virtual void initStatusBar() = 0;

protected:
    MaEditor *editor;
    MaEditorSequenceArea *sequenceArea;
    MaEditorNameList *nameList;
    MaEditorConsensusArea *consensusArea;
    MaEditorOverviewArea *overviewArea;
    MSAEditorOffsetsViewController *offsetsViewController;
    MaEditorStatusBar *statusBar;

    QWidget *nameAreaContainer;
    QWidget *seqAreaHeader;
    QVBoxLayout *seqAreaHeaderLayout;

    QGridLayout *seqAreaLayout;
    QVBoxLayout *nameAreaLayout;
    MaSplitterController maSplitter;

    MsaUndoRedoFramework *undoFWK;

    MaCollapseModel *collapseModel;

    /**
     * If true, the 'collapseModel' holds virtual ordering/grouping state.
     * Otherwise 'collapseModel' contains a trivial (1:1) mapping of in-view row indexes to the file indexes that are always the same.
     *
     * This virtual state is never saved to MA file and is preserved in the current view only in runtime.
     *
     * Examples of use of virtual grouping/ordering state:
     *  - Group of sequences with the common content: reorder + groups.
     *  - Tree sync mode: reorder + groups.
     *  - Any other mode that requires grouping & expand-collapse capabilities.
     *
     * Examples of non-virtual (in-file order modifications):
     *  - Sort by name, length, leading gap size.
     *  - Drag & drop or rows in the non-virutalOrderingMode.
     *
     *  The virtual ordering/grouping can be applied to read-only alignments.
     *  Any changes to the order while in the virtualOrderMode are not saved to the MA file.
     */
    bool virtualOrderMode;

    bool enableCollapsingOfSingleRowGroups;
    ScrollController *scrollController;
    BaseWidthController *baseWidthController;
    RowHeightController *rowHeightController;
    DrawHelper *drawHelper;

public:
    QAction *delSelectionAction;
    QAction *copySelectionAction;
    QAction *copyFormattedSelectionAction;
    QAction *pasteAction;
    QAction *pasteBeforeAction;
    QAction *cutSelectionAction;
};

}    // namespace U2

#endif    // _U2_MA_EDITOR_WGT_H_
