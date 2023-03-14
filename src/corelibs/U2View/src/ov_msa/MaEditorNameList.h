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

#include <QMenu>
#include <QRubberBand>
#include <QScrollBar>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2Region.h>

#include "MaCollapseModel.h"
#include "MsaEditorUserModStepController.h"

namespace U2 {

class GObjectViewController;
class MaEditor;
class MaEditorSelection;
class MaEditorWgt;
class MaModificationInfo;
class MaCollapsibleGroup;

class U2VIEW_EXPORT MaEditorNameList : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(MaEditorNameList)
public:
    MaEditorNameList(MaEditorWgt* ui, QScrollBar* nhBar);
    virtual ~MaEditorNameList();

    QSize getCanvasSize(const QList<int>& seqIdx) const;

    void drawNames(QPainter& painter, const QList<int>& maRows, bool drawSelection = false);

    QAction* getEditSequenceNameAction() const;
    QAction* getRemoveSequenceAction() const;

    /** Returns text to be rendered in the given row cell. */
    virtual QString getTextForRow(int maRowIndex);

public slots:
    void sl_removeSelectedRows();

protected slots:
    void sl_completeRedraw();
    void sl_editSequenceName();

private slots:
    /** Copies whole selected row content into clipboard without any formatting. */
    void sl_copyWholeRow();
    void sl_lockedStateChanged();
    void sl_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&);
    void sl_vScrollBarActionPerformed();
    void sl_completeUpdate();

protected slots:
    virtual void sl_selectionChanged(const MaEditorSelection& current, const MaEditorSelection& prev);
    virtual void sl_updateActions();

protected:
    virtual void updateScrollBar();

protected:
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void focusOutEvent(QFocusEvent* fe) override;
    void focusInEvent(QFocusEvent* fe) override;
    void wheelEvent(QWheelEvent* we) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;

    /**
     * Moves selection up to 'offset' rows up or down if possible: never changes selection height.
     * If 'resetXRange' is set to 'false' keeps X-range of the selection unchanged,
     *  otherwise resets X-range to the 'whole-row' selection.
     */
    void moveSelection(int offset, bool resetXRange);

    void scrollSelectionToView(bool fromStart);

    bool completeRedraw = true;

protected:
    void drawContent(QPainter& p);

public:
    qint64 sequenceIdAtPos(const QPoint& p);

    QFont getFont(bool selected) const;

signals:
    void si_sequenceNameChanged(const QString& prevName, const QString& newName);
    void si_startMaChanging();
    void si_stopMaChanging(bool modified);

protected:
    /**
     * Sets selection MA editor selection to the given state.
     * The method is called for all selection change events triggered in the name-list component.
     * May be overridden to adjust behavior. */
    virtual void setSelection(const MaEditorSelection& selection);

    void moveSelectedRegion(int shift);

    /**
     * Returns width required by the group expander element in the active row ordering mode.
     * The group expander requires space only if there are collapsible groups in the view.
     */
    int getGroupExpanderWidth() const;

    /**
     * Returns collapsible group related to the expand-collapse button located by the given screen coordinate.
     * Returns NULL if the given coordinate is not for expand-collapse block.
     */
    const MaCollapsibleGroup* getCollapsibleGroupByExpandCollapsePoint(const QPoint& point) const;

    virtual void drawAll();

    virtual void drawSelection(QPainter& p);

    virtual void drawSequenceItem(QPainter& painter, const QString& text, const U2Region& yRange, bool isSelected, bool isReference);

    virtual void drawCollapsibleSequenceItem(QPainter& painter, int rowIndex, const QString& name, const QRect& rect, bool isSelected, bool isCollapsed, bool isReference);

    virtual void drawChildSequenceItem(QPainter& painter, const QString& name, const QRect& rect, bool isSelected, bool isReference);

    virtual void drawBackground(QPainter& p, const QString& name, const QRect& rect, bool isReference);

    virtual void drawText(QPainter& p, const QString& name, const QRect& rect, bool selected);

    virtual void drawCollapsePrimitive(QPainter& p, bool collapsed, const QRect& rect);

    /*
     * Triggers expand collapse on the currently selected set of collapse group headers.
     *
     * Returns true if any group was expanded or collapsed as the result of the method call.
     */
    bool triggerExpandCollapseOnSelectedRow(bool collapse);

    QRect calculateTextRect(const U2Region& yRange, bool selected) const;
    QRect calculateExpandCollapseButtonRect(const QRect& itemRect) const;

    virtual int getAvailableWidth() const;

    /**
     * Converts multiple region selection into a continuous single region selection by moving all
     * selected regions close to the region with a 'stableRowIndex'.
     * Only the 'stableRowIndex' region is guaranteed to stay on the current position.
     * The order of sequences in the selection does not change.
     * */
    void groupSelectedSequencesIntoASingleRegion(int stableRowIndex, U2OpStatus& os);

    QObject* labels = nullptr;  // used in GUI tests
    MaEditorWgt* ui = nullptr;
    QScrollBar* nhBar = nullptr;
    QPoint mousePressPoint;
    bool isDragging = false;
    QRubberBand* rubberBand = nullptr;

public:
    QAction* editSequenceNameAction = nullptr;

    /** Copies whole selected rows. Ignores actual selected column range. */
    QAction* copyWholeRowAction = nullptr;

    // TODO: remove this action. It triggers the same code with ui->delSelectionAction and exists only to show a different text in the context menu.
    QAction* removeSequenceAction = nullptr;

protected:
    QPixmap* cachedView = nullptr;

    MsaEditorUserModStepController* changeTracker = nullptr;
    int maVersionBeforeMousePress = -1;

    static const int CROSS_SIZE = 9;
    static const int CHILDREN_OFFSET = 8;
    static const int MARGIN_TEXT_LEFT = 5;
    static const int MARGIN_TEXT_TOP = 2;
    static const int MARGIN_TEXT_BOTTOM = 2;

protected:
    MaEditor* editor = nullptr;
};

}  // namespace U2
