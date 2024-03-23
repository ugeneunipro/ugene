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
class MSAEditorOffsetsViewController;
class MaEditor;
class MaEditorConsensusArea;
class MaEditorNameList;
class MaEditorOverviewArea;
class MaEditorSequenceArea;
class MaEditorStatusBar;
class RowHeightController;
class ScrollController;
class SequenceAreaRenderer;

/************************************************************************/
/* MaEditorWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorWgt : public QWidget {
    Q_OBJECT
public:
    MaEditorWgt(MaEditor* editor, QWidget* parent);

    QWidget* createHeaderLabelWidget(const QString& text = QString(),
                                     Qt::Alignment ali = Qt::AlignCenter,
                                     QWidget* heightTarget = nullptr,
                                     bool proxyMouseEventsToNameList = true);

    /** Returns MA editor instance. The instance is always defined and is never null. */
    MaEditor* getEditor() const;

    MaEditorSequenceArea* getSequenceArea() const;

    MaEditorNameList* getEditorNameList() const;

    MaEditorConsensusArea* getConsensusArea() const;

    MaEditorOverviewArea* getOverviewArea() const;

    void setOverviewArea(MaEditorOverviewArea* overview);

    MaEditorStatusBar* getStatusBar() const;

    void setStatusBar(MaEditorStatusBar* statusbar);

    MSAEditorOffsetsViewController* getOffsetsViewController() const;

    ScrollController* getScrollController() const;

    BaseWidthController* getBaseWidthController() const;

    RowHeightController* getRowHeightController() const;

    DrawHelper* getDrawHelper() const;

    /* If 'true' and collapse group has only 1 row it will have expand/collapse control. */
    bool isCollapsingOfSingleRowGroupsEnabled() const;

    QWidget* getHeaderWidget() const;

    QSplitter* getMainSplitter() const;

    QSplitter* getNameAndSequenceAreasSplitter() {
        return nameAndSequenceAreasSplitter;
    }

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();

protected:
    virtual void initWidgets(bool addStatusBar = true, bool addOverviewArea = true);
    virtual void initActions();

    virtual void initSeqArea(GScrollBar* shBar, GScrollBar* cvBar) = 0;
    virtual void initOverviewArea(MaEditorOverviewArea* overviewArea = nullptr) = 0;
    virtual void initNameList(QScrollBar* nhBar) = 0;
    virtual void initConsensusArea() = 0;
    virtual void initStatusBar(MaEditorStatusBar* statusBar = nullptr) = 0;

protected:
    MaEditor* const editor;
    MaEditorSequenceArea* sequenceArea;
    MaEditorNameList* nameList;
    MaEditorConsensusArea* consensusArea;
    MaEditorOverviewArea* overviewArea;
    MSAEditorOffsetsViewController* offsetsViewController;
    MaEditorStatusBar* statusBar;

    /** Horizontal splitter in the main layout.  Separates main widgets (NameList + SequenceArea), Exclude List and Overview. */
    QSplitter* mainSplitter = nullptr;

    QWidget* nameAreaContainer;
    QWidget* seqAreaHeader;
    QVBoxLayout* seqAreaHeaderLayout;

    QGridLayout* seqAreaLayout;
    QVBoxLayout* nameAreaLayout;

    /** Vertical splitter with NameList & SequenceArea stacked horizontally. */
    QSplitter* nameAndSequenceAreasSplitter = nullptr;

    bool enableCollapsingOfSingleRowGroups;
    ScrollController* scrollController;
    BaseWidthController* baseWidthController;
    RowHeightController* rowHeightController;
    DrawHelper* drawHelper;

public:
    QAction* delSelectionAction;
    QAction* copySelectionAction;
    QAction* copyFormattedSelectionAction;
    QAction* pasteAction;
    QAction* pasteBeforeAction;
    QAction* cutSelectionAction;
};

}  // namespace U2
