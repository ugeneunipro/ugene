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

#include <assert.h>

#include <QMenu>
#include <QScrollBar>
#include <QSharedPointer>
#include <QWidget>

#include <U2Core/U2Assembly.h>

#include "AssemblyCellRenderer.h"
#include "AssemblyModel.h"
#include "AssemblyNavigationWidget.h"
#include "AssemblyReadsAreaHint.h"

namespace U2 {

class AssemblyBrowser;
class AssemblyBrowserUi;
class AssemblyReadsArea;

class AssemblyReadsArea : public QWidget {
    Q_OBJECT

    // (hotkey, description) pairs
    struct HotkeyDescription {
        QString key;
        QString desc;
        HotkeyDescription(const QString& k, const QString& d)
            : key(k), desc(d) {
        }
    };
    static const QList<HotkeyDescription> HOTKEY_DESCRIPTIONS;
    static QList<HotkeyDescription> initHotkeyDescriptions();

public:
    AssemblyReadsArea(AssemblyBrowserUi* ui, QScrollBar* hBar, QScrollBar* vBar);

    bool isReadHintEnabled();
    void setReadHintEnabled(bool enabled);

    bool isScrolling();
    void setScrolling(bool value);

    QList<QAction*> getCellRendererActions() {
        return cellRendererActions;
    }
    QAction* getOptimizeRenderAction() {
        return optimizeRenderAction;
    }

    static const QString ZOOM_LINK;

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void hideEvent(QHideEvent* e) override;
    bool event(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    void initRedraw();
    void connectSlots();
    void setupHScrollBar();
    void setupVScrollBar();

    void accumulateDelta(int delta);

    void drawAll();
    void drawReads(QPainter& p);

    void drawCurrentReadHighlight(QPainter& p);
    void drawReadsShadowing(QPainter& p);

    /** Find reads crossing vertical line in assembly.
        @param asmX position in assembly */
    QList<U2AssemblyRead> findReadsCrossingX(qint64 asmX);

    /** Find read laying under screen position.
        @param pos screen position */
    bool findReadOnPos(const QPoint& pos, U2AssemblyRead& read);

    /** Calculate rectangle corresponding to the read on screen */
    QRect calcReadRect(const U2AssemblyRead& read);

    /** Update hint data and position */
    void updateHint();

    /** Put welcome screen info on coveredRegionsLabel */
    void showWelcomeScreen();
    /** Put information screen that database is busy  */
    void showDdBusyScreen();

    int calcFontPointSize() const;

    void updateMenuActions();
    void exportReads(const QList<U2AssemblyRead>& reads);

    void createMenu();
    QMenu* createShadowingMenu();
    void shadowingMenuSetBind(bool enabled);

signals:
    void si_heightChanged();
    void si_mouseMovedToPos(const QPoint&);

public slots:
    void sl_hideHint();
    void sl_redraw();

private slots:
    void sl_onHScrollMoved(int pos);
    void sl_onVScrollMoved(int pos);
    void sl_zoomOperationPerformed();
    void sl_onCopyReadData();

    /** Copies current numeric X cursor position to the clipboard. */
    void sl_copyPositionToClipboard();

    void sl_onExportRead();
    void sl_onExportReadsOnScreen();
    void sl_onShadowingModeChanged(QAction* a);
    void sl_onBindShadowing();
    void sl_onShadowingJump();
    void sl_changeCellRenderer();
    void sl_onOptimizeRendering(bool enabled);
    void sl_onScrollPressed() {
        setScrolling(true);
    }
    void sl_onScrollReleased() {
        setScrolling(false);
    }

private:
    AssemblyBrowserUi* ui;
    AssemblyBrowser* browser;
    QSharedPointer<AssemblyModel> model;

    bool redraw;
    QPixmap cachedView;

    QScopedPointer<AssemblyCellRenderer> cellRenderer;

    CoveredRegionsLabel coveredRegionsLabel;
    QLabel bdBusyLabel;
    QScrollBar* hBar;
    QScrollBar* vBar;

    // Some variables for a workaround UGENE-3183
    static const int DEFAULT_MOUSE_DELTA;
    int wheelEventAccumulatedDelta;
    int wheelEventPrevDelta;

    // caches reads that are visible on a screen
    class ReadsCache {
    public:
        bool isEmpty() const {
            return data.isEmpty();
        }
        void clear() {
            data.clear();
            visibleBases = U2Region();
            visibleRows = U2Region();
            letterWidth = 0;
            xOffsetInAssembly = 0;
            yOffsetInAssembly = 0;
        }
        QList<U2AssemblyRead> data;
        U2Region visibleBases;
        U2Region visibleRows;
        int letterWidth;
        qint64 xOffsetInAssembly;
        qint64 yOffsetInAssembly;
    };
    ReadsCache cachedReads;
    QPoint curPos;

    struct HintData {
        HintData(QWidget* p)
            : updateHint(false), hint(p) {
        }
        bool updateHint;
        AssemblyReadsAreaHint hint;
        U2DataId curReadId;
    } hintData;

    // needed to move by-letter when scribbling
    class ReadsMover {
    public:
        ReadsMover()
            : cellWidth(0) {};
        ReadsMover(int cellWidth_, QPoint initPos)
            : lastPos(initPos), cellWidth(cellWidth_ ? cellWidth_ : 1) {
        }
        void handleEvent(QPoint newPos) {
            assert(cellWidth);
            QPoint diff_ = newPos - lastPos;
            lastPos = newPos;
            diff += diff_;
        }
        int getXunits() {
            assert(cellWidth);
            int result = diff.x() / cellWidth;
            diff.setX(diff.x() % cellWidth);
            return result;
        }
        int getYunits() {
            assert(cellWidth);
            int result = diff.y() / cellWidth;
            diff.setY(diff.y() % cellWidth);
            return result;
        }

    private:
        QPoint lastPos;
        QPoint diff;
        int cellWidth;
    } mover;

    bool shadowingEnabled;
    struct ShadowingData {
        ShadowingData()
            : mode(FREE), boundPos(0) {
        }

        enum ShadowingMode { FREE,
                             CENTERED,
                             BIND } mode;
        qint64 boundPos;
    } shadowingData;

    bool scribbling;
    int currentHotkeyIndex;
    bool hintEnabled;

    bool scrolling;
    bool optimizeRenderOnScroll;

    QMenu* readMenu;
    QAction* copyDataAction;
    QAction* exportReadAction;

    // shadowing actions
    QAction* disableShadowing;
    QAction* shadowingModeFree;
    QAction* shadowingModeCentered;
    QAction* shadowingBindHere;
    QAction* shadowingJump;

    QList<QAction*> cellRendererActions;
    QAction* optimizeRenderAction;
    int lockTimeout;
};

}  // namespace U2
