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

#include <QGraphicsView>
#include <QMap>
#include <QScrollBar>
#include <QToolBar>
#include <QToolButton>
#include <QTransform>

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PhyTree.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/Task.h>

#include <U2Gui/ObjectViewModel.h>

#include "TreeSettings.h"

namespace U2 {

class GObjectViewController;
class TreeViewerUI;
class TvBranchItem;
class TvNodeItem;
class TvTextItem;
class TvRectangularBranchItem;

class TreeViewer : public GObjectViewController {
    Q_OBJECT
public:
    TreeViewer(const QString& viewName, PhyTreeObject* phyTreeObject, bool hasOptionsPanel = true);

    // from GObjectView
    void buildStaticToolbar(QToolBar* tb) override;
    void buildMenu(QMenu* m, const QString& type) override;

    void createActions();

    QVariantMap saveState() override;

    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;

    PhyTreeObject* getPhyObject() const {
        return phyObject;
    }

    void setZoomLevel(double zoomLevel);

    double getZoomLevel() const;

    QTransform getTransform() const;

    void setTransform(const QTransform& m);

    QVariantMap getSettingsState() const;

    void setSettingsState(const QVariantMap& m);

    void onAfterViewWindowInit() override;

    TreeViewerUI* getTreeViewerUI() {
        return ui;
    }

protected:
    QWidget* createViewWidget(QWidget* parent) override;
    void onObjectRenamed(GObject* obj, const QString& oldName) override;

public:
    QAction* treeSettingsAction = nullptr;
    QAction* rectangularLayoutAction = nullptr;
    QAction* circularLayoutAction = nullptr;
    QAction* unrootedLayoutAction = nullptr;

    QAction* nameLabelsAction = nullptr;
    QAction* showNodeLabelsAction = nullptr;
    QAction* distanceLabelsAction = nullptr;
    QAction* textSettingsAction = nullptr;
    QAction* alignTreeLabelsAction = nullptr;

    QAction* zoomInAction = nullptr;
    QAction* zoomOutAction = nullptr;
    QAction* zoom100Action = nullptr;
    QAction* zoomFitAction = nullptr;

    QAction* printAction = nullptr;
    QAction* saveVisibleViewToFileAction = nullptr;
    QAction* saveWholeTreeToSvgAction = nullptr;
    QAction* copyWholeTreeImageToClipboardAction = nullptr;

    QAction* collapseAction = nullptr;
    QAction* rerootAction = nullptr;
    QAction* swapAction = nullptr;

    PhyTreeObject* const phyObject;

private:
    QActionGroup* layoutActionGroup = nullptr;

    QByteArray state;

    void setupLayoutSettingsMenu(QMenu* m);
    void setupShowLabelsMenu(QMenu* m) const;
    void setupExportTreeImageMenu(QMenu* m) const;

protected:
    TreeViewerUI* ui = nullptr;
};

/** Serialized tree state. Used to restore states like collapse/selection on tree re-layouts. */
class U2VIEW_EXPORT TvTreeState {
public:
    bool isRootSelected = false;
    const PhyBranch* selectionRootBranch = nullptr;
    QSet<const PhyBranch*> collapsedBranches;
};

class U2VIEW_EXPORT TreeViewerUI : public QGraphicsView {
    Q_OBJECT
    Q_DISABLE_COPY(TreeViewerUI)
    friend class TreeViewer;

public:
    explicit TreeViewerUI(TreeViewer* treeViewer, QWidget* parent);
    ~TreeViewerUI() override;

    /** Returns option value by looking up first in 'selectionSettings and next in the 'setting'. */
    QVariant getOption(const TreeViewOption& option) const;

    QVariantMap getSettingsState() const;
    void setSettingsState(const QVariantMap& state);

    /** Returns current settings adjusted by 'selectionSettings'. */
    QMap<TreeViewOption, QVariant> getSelectionSettings() const;

    TreeLayoutType getTreeLayoutType() const;

    bool isRectangularLayoutMode() const {
        return getTreeLayoutType() == RECTANGULAR_LAYOUT;
    }

    /** Returns current root item of the tree. */
    TvBranchItem* getRoot() const;

    bool isSelectionStateManagedByChildOnClick = false;

    /** Makes 1 zoom-in step, until maximum zoom limit is reached. */
    void zoomIn();

    /** Makes 1 zoom-out step, until minimum zoom limit is reached. */
    void zoomOut();

    /** Resets zoom to 100%. */
    void zoomTo100();

    /** Adjusts zoom so the tree fits into the view. */
    void zoomFit();

    /** Updates single option. */
    void updateOption(const TreeViewOption& option, const QVariant& newValue);

    /** Updates all options from the 'changedOptions' map. */
    void updateOptions(const QMap<TreeViewOption, QVariant>& changedOptions);

protected:
    void wheelEvent(QWheelEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    /** Sets zoom to the given level. Unchecks 'zoomFitAreaAction' if 'cancelFitToViewMode' is true. */
    void setZoomLevel(double newZoomLevel, bool cancelFitToViewMode = true);

    /**
     * Recomputes scene layout and triggers redraw.
     * Updates legend, scene rect, label alignment and other UI properties.
     */
    virtual void updateScene();

signals:
    /** Emitted when option is changed: either due to selection change or an explicit option update. */
    void si_optionChanged(const TreeViewOption& option, const QVariant& value);

protected slots:
    virtual void sl_swapTriggered();
    virtual void sl_collapseTriggered();
    virtual void sl_onBranchCollapsed(TvBranchItem*);

private slots:
    void sl_printTriggered();
    void sl_contTriggered(bool on);
    void sl_showNameLabelsTriggered(bool on);
    void sl_showDistanceLabelsTriggered(bool on);
    void sl_textSettingsTriggered();
    void sl_treeSettingsTriggered();
    void sl_rerootTriggered();

private:
    /**
     * Returns true if there is a selection and only part of the tree is selected.
     * In case of partial selection some UI styling (fonts/colors) are applied only to the selected part of the tree and
     * are not stored as view default.
     * See 'isSelectionScopeOption' to check which option can be applied only to selection.
     */
    bool hasPartialSelection() const;

    /**
     * Sets single option value to the settings only (does not handle view update like in updateOption).
     * Updates only selection settings if there is an active selection and 'option' can be 'selection-only' option.
     */
    void saveOptionToSettings(const TreeViewOption& option, const QVariant& value);

    double getScalebarDistanceRange() const;

    /** Copies whole tree image to clipboard. */
    void copyWholeTreeImageToClipboard();

    /** Opens ExportImageDialog for visible tree area. */
    void saveVisibleViewToFile();

    /** Opens SVG file selector and exports the whole tree as SVG. */
    void saveWholeTreeToSvg();

    /** Update scales of fixed size elements so the elements keeps their on-screen sizes not changed on view zoom/resize ops. */
    void updateFixedSizeItemScales();

    /** Returns list of fixed size elements: the elements that do not change their on screen dimensions regardless of the current zoom level. */
    QList<QGraphicsItem*> getFixedSizeItems() const;

    void applyNewTreeLayout(TvBranchItem* newRoot, TvRectangularBranchItem* newRectRoot, const TreeLayoutType& layoutType);

    enum LabelType {
        LabelType_SequenceName = 1,
        LabelType_Distance = 2
    };

    void paint(QPainter& painter);

    /** Updates 'visible' state for labels based on the current options. */
    void updateLabelsVisibility();

    // Scalebar
    void updateLegend();

    /** Updates 'selectionSettings' every time selection is changed. */
    void updateSettingsOnSelectionChange();

    bool isSelectedCollapsed();

    void updateActions();

    void updateLabelsAlignment();

    /** Calculates new tree layout of the given type and sets it to as active on the scene. */
    void switchTreeLayout(const TreeLayoutType& newLayoutType);

    /** Updates settings for selected items only. If there is no selection updates setting for all items. */
    void updateTextOptionOnSelectedItems();

    /** Updates settings for selected items only. If there is no selection updates setting for all items. */
    void updateTreeSettingsOnSelectedItems();

    /** Updates settings for all nodes in the tree. */
    void updateTreeSettingsOnAllNodes();

    void changeLabelsAlignment();

    void initializeSettings();

    /** Recomputes branch geometry: width, height, curvature. */
    void updateBranchGeometry(TvRectangularBranchItem* rootBranch) const;

    /** Saves current scene state into 'treeState'. */
    void saveSelectionAndCollapseStates();

    /** Restores scene state from the current 'treeState'. */
    void restoreSelectionAndCollapseStates();

public:
    PhyTreeObject* const phyObject = nullptr;
    TreeViewer* const treeViewer = nullptr;

protected:
    /** Currently shown tree. Can be rect, circular or unrooted one. */
    TvBranchItem* root = nullptr;

    TvRectangularBranchItem* rectRoot = nullptr;

    /** View global pos of the last mouse-press event. */
    QPoint lastMousePressPos;

private:
    /**
     * Scale of the view. Changed on zoom-in/zoom-out.
     * ZoomLevel = 1 is equal to the Fit-Into-View mode (when window is resized zoom level does not change, but on-screen size of elements changes.
     */
    double zoomLevel = 1;

    /** Used to compute on-screen length of every branch: length = distance * distanceToScreenScale. */
    double distanceToViewScale = 1;

    /** Legend item. Not null only in 'PHYLOGRAM' mode. */
    QGraphicsLineItem* legendItem = nullptr;
    QMenu* buttonPopup = nullptr;

    QMap<TreeViewOption, QVariant> settings;

    QMap<TreeViewOption, QVariant> selectionSettingsDelta;

    /** Current visible labels state on the scene. May be different from the state in options when is stale. */
    QFlags<LabelType> visibleLabelTypes;

    /** Snapshot of the collapsed/selected properties. Used to restore state on layout change. */
    TvTreeState treeState;
};
}  // namespace U2
