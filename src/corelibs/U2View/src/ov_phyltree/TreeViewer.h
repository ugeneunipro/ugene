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

#ifndef _U2_TREE_VIEWER_H_
#define _U2_TREE_VIEWER_H_

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

class GObjectView;
class TreeViewerUI;
class TvBranchItem;
class TvNodeItem;
class TvTextItem;
class TvRectangularBranchItem;

class TreeViewer : public GObjectView {
    Q_OBJECT
public:
    TreeViewer(const QString& viewName, PhyTreeObject* phyTreeObject);

    // from GObjectView
    void buildStaticToolbar(QToolBar* tb) override;
    void buildMenu(QMenu* m, const QString& type) override;

    void buildMSAEditorStaticToolbar(QToolBar* tb);

    void createActions();

    QVariantMap saveState() override;

    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;

    OptionsPanel* getOptionsPanel() override;

    PhyTreeObject* getPhyObject() const {
        return phyObject;
    }

    void setZoomLevel(double zoomLevel);

    double getZoomLevel() const;

    QTransform getTransform() const;

    void setTransform(const QTransform& m);

    QVariantMap getSettingsState() const;

    void setSettingsState(const QVariantMap& m);

    TreeViewerUI* getTreeViewerUI() {
        return ui;
    }

protected:
    QWidget* createWidget() override;
    void onObjectRenamed(GObject* obj, const QString& oldName) override;

public:
    QAction* treeSettingsAction = nullptr;
    QAction* rectangularLayoutAction = nullptr;
    QAction* circularLayoutAction = nullptr;
    QAction* unrootedLayoutAction = nullptr;

    QAction* branchesSettingsAction = nullptr;

    QAction* nameLabelsAction = nullptr;
    QAction* showNodeLabelsAction = nullptr;
    QAction* distanceLabelsAction = nullptr;
    QAction* textSettingsAction = nullptr;
    QAction* alignTreeLabelsAction = nullptr;

    QAction* zoomInAction = nullptr;
    QAction* resetZoomAction = nullptr;
    QAction* zoomOutAction = nullptr;

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

class U2VIEW_EXPORT TreeViewerUI : public QGraphicsView {
    Q_OBJECT
    Q_DISABLE_COPY(TreeViewerUI)
    friend class TreeViewer;

public:
    explicit TreeViewerUI(TreeViewer* treeViewer);
    ~TreeViewerUI() override;

    /** Returns option value by looking up first in 'selectionSettings and next in the 'setting'. */
    QVariant getOption(const TreeViewOption& option) const;

    QVariantMap getSettingsState() const;
    void setSettingsState(const QVariantMap& m);

    /** Returns current settings adjusted by 'selectionSettings'. */
    OptionsMap getSelectionSettings() const;

    TreeLayout getTreeLayout() const;

    bool isRectangularLayoutMode() const {
        return getTreeLayout() == RECTANGULAR_LAYOUT;
    }

    bool isOnlyLeafSelected() const;

    void updateRect();

    /** Returns current root item of the tree. */
    TvBranchItem* getRoot() const;

    bool isSelectionStateManagedByChildOnClick = false;

    /** Makes 1 zoom-in step, until maximum zoom limit is reached. */
    void zoomIn();

    /** Makes 1 zoom-out step, until minimum zoom limit is reached. */
    void zoomOut();

    /** Resets zoom. Fits the tree into view. */
    void resetZoom();

    /** Updates single option. */
    void updateOption(const TreeViewOption& option, const QVariant& newValue);

    /** Updates all options from the 'changedOptions' map. */
    void updateOptions(const OptionsMap& changedOptions);

protected:
    void wheelEvent(QWheelEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    virtual void setTreeLayout(const TreeLayout& newLayout);

    void setZoomLevel(double newZoomLevel);

    void defaultZoom();

    /** Fits current scene into the view, so the whole tree is visible. Does not change aspect ratio. **/
    void fitIntoView();

    /**
     * Recomputes scene layout and triggers redraw.
     * Updates legend, scene rect, label alignment and other UI properties.
     * If 'fitSceneToView' is true calls fitInView() for the result scene.
     */
    virtual void updateScene(bool fitSceneToView);

    /** Updates parameter of rect-layout branches using current settings. */
    void updateRectLayoutBranches();

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
    void sl_rectangularLayoutTriggered();
    void sl_circularLayoutTriggered();
    void sl_unrootedLayoutTriggered();
    void sl_textSettingsTriggered();
    void sl_treeSettingsTriggered();
    void sl_rerootTriggered();

    void sl_setSettingsTriggered();
    void sl_branchSettings();

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

    void updateDistanceToViewScale();
    void rebuildTreeLayout();

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

    void setNewTreeLayout(TvBranchItem* newRoot, const TreeLayout& treeLayout);

    enum LabelType {
        LabelType_SequenceName = 1,
        LabelType_Distance = 2
    };
    typedef QFlags<LabelType> LabelTypes;

    void paint(QPainter& painter);
    void showLabels(LabelTypes labelTypes);
    // Scalebar
    void addLegend();
    void updateLegend();

    void collapseSelected();

    void updateLayout();

    /** Updates 'selectionSettings' every time selection is changed. */
    void updateSettingsOnSelectionChange();

    void recalculateRectangularLayout();
    bool isSelectedCollapsed();

    void updateActionsState();

    /** Returns average branch distance in the tree. */
    double getAverageBranchDistance() const;

    void updateLabelsAlignment();

    /** Recalculates and assign 'steps to leaf' properties to every branch item in the rect-layout tree. */
    void updateStepsToLeafOnBranches();

    void changeTreeLayout(const TreeLayout& newTreeLayout);
    void changeNamesDisplay();

    /** Updates settings for selected items only. If there is no selection updates setting for all items. */
    void updateTextOptionOnSelectedItems();

    /** Updates settings for selected items only. If there is no selection updates setting for all items. */
    void updateTreeSettingsOnSelectedItems();

    /** Updates settings for all nodes in the tree. */
    void updateTreeSettingsOnAllNodes();

    void changeLabelsAlignment();

    void initializeSettings();

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
    double maxNameWidth = 0;

    /**
     * Scale of the view. Changed on zoom-in/zoom-out.
     * ZoomLevel = 1 is equal to the Fit-Into-View mode (when window is resized zoom level does not change, but on-screen size of elements changes.
     */
    double zoomLevel = 1;

    /** Used to compute on-screen length of every branch: length = distance * distanceToScreenScale. */
    double distanceToViewScale = 1;

    QGraphicsLineItem* legendItem = nullptr;
    TvTextItem* scalebarTextItem = nullptr;
    QMenu* buttonPopup = nullptr;

    /** Settings for the whole view. Saved & restored on view creation and applied to all new elements by default. */
    OptionsMap settings;

    /** Settings override for the currently selected items. Contains only option that override 'settings' for the current selection. */
    OptionsMap selectionSettingsDelta;
};

}  // namespace U2
#endif
