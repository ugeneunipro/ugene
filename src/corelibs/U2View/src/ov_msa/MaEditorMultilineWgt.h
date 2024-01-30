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
class MsaEditorMultiTreeViewer;

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
class U2VIEW_EXPORT MaEditorMultilineWgt : public QWidget {
    Q_OBJECT
public:
    explicit MaEditorMultilineWgt(QWidget* parent);

    // Get multiline scroll controller
    virtual MultilineScrollController* getScrollController() const = 0;

    // Get scroll area which contain all MaEditorWidget(s)
    virtual QScrollArea* getChildrenScrollArea() const = 0;

    virtual void updateSize() = 0;

    virtual int getSequenceAreaWidth(int index) const = 0;  // pixels
    virtual int getFirstVisibleBase(int index) const = 0;
    virtual int getLastVisibleBase(int index) const = 0;
    virtual int getSequenceAreaBaseLen(int index) const = 0;  // bases
    virtual int getSequenceAreaBaseWidth(int index) const = 0;  // pixels
    virtual int getSequenceAreaAllBaseLen() const = 0;  // bases
    virtual int getSequenceAreaAllBaseWidth() const = 0;  // pixels

    virtual MaEditorWgt* createChild(MaEditor* editor,
                                     MaEditorOverviewArea* overviewArea,
                                     MaEditorStatusBar* statusBar) = 0;

    virtual void addChild(MaEditorWgt* child) = 0;

    // Return MaEditorWgt widget which has input focus
    virtual MaEditorWgt* getActiveChild() const = 0;
    virtual void setActiveChild(MaEditorWgt* child) = 0;

    virtual void setSimilaritySettings(const SimilarityStatisticsSettings* settings) = 0;
    virtual void refreshSimilarityColumn() = 0;
    virtual void showSimilarity() = 0;
    virtual void hideSimilarity() = 0;

    virtual bool moveSelection(int key, bool shift, bool ctrl) = 0;

    // Return lines count in multiline widget
    virtual int getChildrenCount() const = 0;

    // Current multiline mode
    virtual bool getMultilineMode() const = 0;

signals:
    void si_startMaChanging();
    void si_stopMaChanging(bool modified = false);
    void si_completeRedraw();
    void si_maEditorUIChanged();

public slots:
    /** Switches between Original and Sequence row orders. */
    virtual void sl_toggleSequenceRowOrder(bool isOrderBySequence) = 0;
    virtual void sl_goto() = 0;

protected:
    virtual void initWidgets() = 0;

    virtual void createChildren() = 0;
    virtual void updateChildren() = 0;

    virtual void initScrollArea() = 0;
    virtual void initOverviewArea() = 0;
    virtual void initStatusBar() = 0;
    virtual void initChildrenArea() = 0;

private:
};

}  // namespace U2
