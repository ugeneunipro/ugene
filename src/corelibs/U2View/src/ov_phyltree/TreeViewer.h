/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
class GraphicsBranchItem;
class GraphicsButtonItem;
class GraphicsRectangularBranchItem;
class CreateBranchesTask;

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
    GraphicsRectangularBranchItem* getRoot() const {
        return root;
    }
    void setRoot(GraphicsRectangularBranchItem* rectRoot) {
        root = rectRoot;
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
    QAction* nodeLabelsAction = nullptr;
    QAction* distanceLabelsAction = nullptr;
    QAction* textSettingsAction = nullptr;
    QAction* alignTreeLabelsAction = nullptr;

    QAction* zoomToSelectionAction = nullptr;
    QAction* zoomToAllAction = nullptr;
    QAction* zoomOutAction = nullptr;

    QAction* printAction = nullptr;
    QAction* captureTreeAction = nullptr;
    QAction* exportAction = nullptr;

    QAction* collapseAction = nullptr;
    QAction* rerootAction = nullptr;
    QAction* swapAction = nullptr;

private:
    QActionGroup* layoutActionGroup = nullptr;

    QByteArray state;
    PhyTreeObject* phyObject = nullptr;
    GraphicsRectangularBranchItem* root = nullptr;

    void setupLayoutSettingsMenu(QMenu* m);
    void setupShowLabelsMenu(QMenu* m);
    void setupExportTreeImageMenu(QMenu* m);

protected:
    TreeViewerUI* ui = nullptr;
};

class U2VIEW_EXPORT TreeViewerUI : public QGraphicsView {
    Q_OBJECT
    Q_DISABLE_COPY(TreeViewerUI)
    friend class TreeViewer;

public:
    TreeViewerUI(TreeViewer* treeViewer);
    virtual ~TreeViewerUI();

    const QMap<TreeViewOption, QVariant>& getSettings() const;
    QVariant getOptionValue(TreeViewOption option) const;
    void setOptionValue(TreeViewOption option, QVariant value);

    void updateSettings(const OptionsMap& settings);
    void changeOption(TreeViewOption option, const QVariant& newValue);

    QVariantMap getSettingsState() const;
    void setSettingsState(const QVariantMap& m);

    TreeLayout getTreeLayout() const;

    bool isRectangularLayoutMode() const {
        return getTreeLayout() == RECTANGULAR_LAYOUT;
    }

    bool isCircularLayoutMode() const {
        return getTreeLayout() == CIRCULAR_LAYOUT;
    }

    void onPhyTreeChanged();

    bool isOnlyLeafSelected() const;

    void updateRect();

    /** Returns current root item of the tree. */
    GraphicsBranchItem* getRoot() const;

    bool isSelectionStateManagedByChildOnClick = false;
signals:
    /* emits when branch settings is updated */
    void si_updateBranch();

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

    virtual void onLayoutChanged(const TreeLayout&) {
    }

    /**
     * Recomputes scene layout and triggers redraw.
     * Updates legend, scene rect, label alignment and other UI properties.
     * If 'fitSceneToView' is true calls fitInView() for the result scene.
     */
    virtual void updateScene(bool fitSceneToView);

    /** Updates parameter of rect-layout branches using current settings. */
    void updateRectLayoutBranches();

    virtual void onSettingsChanged(const TreeViewOption& option, const QVariant& newValue);

signals:
    void si_optionChanged(TreeViewOption option, const QVariant& value);

protected slots:
    virtual void sl_swapTriggered();
    virtual void sl_collapseTriggered();
    virtual void sl_onBranchCollapsed(GraphicsBranchItem*);
    virtual void sl_zoomToAll();
    virtual void sl_zoomToSel();
    virtual void sl_zoomOut();

private slots:
    void sl_printTriggered();
    void sl_captureTreeTriggered();
    void sl_contTriggered(bool on);
    void sl_exportTriggered();
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
    void updateDistanceToViewScale();
    void rebuildTreeLayout();

    void setNewTreeLayout(GraphicsBranchItem* newRoot, const TreeLayout& treeLayout);

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

    void updateSettings();

    void updateLayout();

    void updateTextSettings(const TreeViewOption& option);

    void updateBranchSettings();

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
    void changeNodeValuesDisplay();
    void changeLabelsAlignment();

    void initializeSettings();

    PhyTreeObject* phyObject = nullptr;

    /** Currently shown tree. Can be rect, circular or unrooted one. */
    GraphicsBranchItem* root = nullptr;

    double maxNameWidth = 0;

    /**
     * Scale of the view. Changed on zoom-in/zoom-out.
     * ZoomLevel = 1 is equal to the Fit-Into-View mode (when window is resized zoom level does not change, but on-screen size of elements changes.
     */
    double zoomLevel = 1;

    /** Used to compute on-screen length of every branch: length = distance * distanceToScreenScale. */
    double distanceToViewScale = 1;

    QGraphicsLineItem* legend = nullptr;
    QGraphicsSimpleTextItem* scalebarText = nullptr;
    QMenu* buttonPopup = nullptr;

    TreeViewer* treeViewer = nullptr;
    OptionsMap settings;
    bool dontSendOptionChangedSignal = false;

protected:
    GraphicsRectangularBranchItem* rectRoot = nullptr;

    /** View global pos of the last mouse-press event. */
    QPoint lastMousePressPos;
};

}  // namespace U2
#endif
