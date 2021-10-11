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

#include <QWidget>

#include <U2Core/global.h>

#include "MaEditorUtils.h"

class QGridLayout;
class QScrollBar;
class QVBoxLayout;

namespace U2 {

class MultilineBaseWidthController;
class DrawHelper;
class GScrollBar;
class MaEditorConsensusArea;
class MSAEditorOffsetsViewController;
class MaEditorStatusBar;
class MaEditor;
class MaEditorNameList;
class MaEditorMultilineOverviewArea;
class MaEditorSequenceArea;
class RowHeightController;
class MsaUndoRedoFramework;
class MultilineScrollController;
class SequenceAreaRenderer;

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorMultilineWgt : public QWidget {
    Q_OBJECT
public:
    MaEditorMultilineWgt(MaEditor *editor);

    /** Returns MA editor instance. The instance is always defined and is never null. */
    MaEditor *getEditor() const;

    MaEditorMultilineOverviewArea *getOverviewArea() const {
        return overviewArea;
    }

    MaEditorStatusBar *getStatusBar() const;

    MultilineScrollController *getScrollController() const {
        return scrollController;
    }

    MultilineBaseWidthController *getBaseWidthController() const {
        return baseWidthController;
    }

    RowHeightController *getRowHeightController() const {
        return rowHeightController;
    }

    /* If 'true' and collapse group has only 1 row it will have expand/collapse control. */
    bool isCollapsingOfSingleRowGroupsEnabled() const {
        return enableCollapsingOfSingleRowGroups;
    }

    int getSequenceAreaWidth() {
        // Need to override in subclass
        return 100;
    }

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();

private slots:

protected:
    virtual void initWidgets();
    virtual void initActions();

    virtual void initOverviewArea(MaEditorMultilineOverviewArea *overviewArea = nullptr) = 0;
    virtual void initStatusBar() = 0;

protected:
    MaEditor *const editor;
    MaEditorMultilineOverviewArea *overviewArea;
    MaEditorStatusBar *statusBar;

    bool enableCollapsingOfSingleRowGroups;
    MultilineScrollController *scrollController;
    MultilineBaseWidthController *baseWidthController;
    RowHeightController *rowHeightController;

public:

};

}  // namespace U2

#endif  // _U2_MA_EDITOR_MULTILINE_WGT_H_
