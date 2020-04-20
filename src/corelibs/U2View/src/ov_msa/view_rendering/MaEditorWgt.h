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
    MaEditorWgt(MaEditor* editor);

    QWidget* createHeaderLabelWidget(const QString& text = QString(),
                                     Qt::Alignment ali = Qt::AlignCenter,
                                     QWidget* heightTarget = NULL,
                                     bool proxyMouseEventsToNameList = true);

    MaEditor*                       getEditor() const { return editor; }
    MaEditorSequenceArea*           getSequenceArea() const { return seqArea; }
    MaEditorNameList*               getEditorNameList() { return nameList; }
    MaEditorConsensusArea*          getConsensusArea() { return consArea; }
    MaEditorOverviewArea*           getOverviewArea() { return overviewArea; }
    MSAEditorOffsetsViewController* getOffsetsViewController() { return offsetsView; }
    ScrollController *              getScrollController();
    BaseWidthController *           getBaseWidthController();
    RowHeightController *           getRowHeightController();
    DrawHelper *                    getDrawHelper();

    QAction* getUndoAction() const;
    QAction* getRedoAction() const;

    QAction* getDelSelectionAction() const { return delSelectionAction; }
    QAction* getCopySelectionAction() const { return copySelectionAction; }
    QAction* getCopyFormattedSelectionAction() const { return copyFormattedSelectionAction; }
    QAction* getPasteAction() const { return pasteAction; }

    /* Returns if collapsible mode is enabled or not.
     *
     * Note: the collapsible model is used regardless if collapsible mode is enabled or not,
     * but have different features: in the collapsible mode the model can contains group of multiple sequences
     * or reordered rows.
     */
    bool isCollapsibleMode() const { return collapsibleMode; }
    void setCollapsibleMode(bool collapse) { collapsibleMode = collapse; }
    MaCollapseModel* getCollapseModel() const { return collapseModel; }

    /* If 'true' and collapse group has only 1 row it will have expand/collapse control. */
    bool isCollapsingOfSingleRowGroupsEnabled() const { return enableCollapsingOfSingleRowGroups; }

    QWidget* getHeaderWidget() const { return seqAreaHeader; }
    MsaUndoRedoFramework* getUndoRedoFramework() { return undoFWK; }

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

    virtual void initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) = 0;
    virtual void initOverviewArea() = 0;
    virtual void initNameList(QScrollBar* nhBar) = 0;
    virtual void initConsensusArea() = 0;
    virtual void initStatusBar() = 0;

protected:
    MaEditor*                       editor;
    MaEditorSequenceArea*           seqArea;
    MaEditorNameList*               nameList;
    MaEditorConsensusArea*          consArea;
    MaEditorOverviewArea*           overviewArea;
    MSAEditorOffsetsViewController* offsetsView;
    MaEditorStatusBar*              statusBar;

    QWidget*                        nameAreaContainer;
    QWidget*                        seqAreaHeader;
    QVBoxLayout*                    seqAreaHeaderLayout;

    QGridLayout*                    seqAreaLayout;
    QVBoxLayout*                    nameAreaLayout;
    MaSplitterController            maSplitter;

    MsaUndoRedoFramework*           undoFWK;

    MaCollapseModel*                collapseModel;
    bool                            collapsibleMode;
    bool                            enableCollapsingOfSingleRowGroups;
    ScrollController *              scrollController;
    BaseWidthController *           baseWidthController;
    RowHeightController *           rowHeightController;
    DrawHelper *                    drawHelper;

    QAction                         *delSelectionAction;
    QAction                         *copySelectionAction;
    QAction                         *copyFormattedSelectionAction;
    QAction                         *pasteAction;
};

} // namespace

#endif // _U2_MA_EDITOR_WGT_H_

