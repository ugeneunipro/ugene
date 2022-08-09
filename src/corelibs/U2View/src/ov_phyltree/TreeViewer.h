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
    TreeViewer(const QString& viewName, GObject* obj, GraphicsRectangularBranchItem* root, qreal scale);

    // from GObjectView
    void buildStaticToolbar(QToolBar* tb) override;
    void buildMenu(QMenu* m, const QString& type) override;

    void buildMSAEditorStaticToolbar(QToolBar* tb);

    void createActions();

    QVariantMap saveState() override;

    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;

    OptionsPanel* getOptionsPanel() override;

    QAction* getPrintAction() const {
        return printAction;
    }
    QAction* getContAction() const {
        return alignTreeLabelsAction;
    }
    QAction* getNameLabelsAction() const {
        return nameLabelsAction;
    }
    QAction* getDistanceLabelsAction() const {
        return distanceLabelsAction;
    }
    QAction* getCaptureTreeAction() const {
        return captureTreeAction;
    }
    QAction* getExportAction() const {
        return exportAction;
    }
    QAction* getRectangularLayoutAction() const {
        return rectangularLayoutAction;
    }
    QAction* getCircularLayoutAction() const {
        return circularLayoutAction;
    }
    QAction* getUnrootedLayoutAction() const {
        return unrootedLayoutAction;
    }
    QAction* getTextSettingsAction() const {
        return textSettingsAction;
    }
    QAction* getTreeSettingsAction() const {
        return treeSettingsAction;
    }
    QAction* getBranchesSettingsAction() const {
        return branchesSettingsAction;
    }
    QAction* getZoomToSelAction() const {
        return zoomToSelAction;
    }
    QAction* getZoomOutAction() const {
        return zoomOutAction;
    }
    QAction* getZoomToAllAction() const {
        return zoomToAllAction;
    }
    QAction* getCollapseAction() const {
        return collapseAction;
    }
    QAction* getRerootAction() const {
        return rerootAction;
    }
    QAction* getSwapAction() const {
        return swapAction;
    }

    PhyTreeObject* getPhyObject() const {
        return phyObject;
    }
    GraphicsRectangularBranchItem* getRoot() const {
        return root;
    }
    void setRoot(GraphicsRectangularBranchItem* rectRoot) {
        root = rectRoot;
    }

    qreal getScale() const {
        return scale;
    }

    void setScale(qreal newScale) {
        scale = newScale;
    }

    qreal getHorizontalZoom() const;

    void setHorizontalZoom(qreal z);

    qreal getVerticalZoom() const;

    void setVerticalZoom(qreal z);

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

private slots:
    void sl_onPhyTreeChanged();

private:
    QAction* treeSettingsAction = nullptr;

    QActionGroup* layoutGroup = nullptr;
    QAction* rectangularLayoutAction = nullptr;
    QAction* circularLayoutAction = nullptr;
    QAction* unrootedLayoutAction = nullptr;

    QAction* branchesSettingsAction = nullptr;

    QAction* nameLabelsAction = nullptr;
    QAction* nodeLabelsAction = nullptr;
    QAction* distanceLabelsAction = nullptr;
    QAction* textSettingsAction = nullptr;
    QAction* alignTreeLabelsAction = nullptr;

    QAction* zoomToSelAction = nullptr;
    QAction* zoomToAllAction = nullptr;
    QAction* zoomOutAction = nullptr;

    QAction* printAction = nullptr;
    QAction* captureTreeAction = nullptr;
    QAction* exportAction = nullptr;

    QAction* collapseAction = nullptr;
    QAction* rerootAction = nullptr;
    QAction* swapAction = nullptr;

    QByteArray state;
    PhyTreeObject* phyObject = nullptr;
    GraphicsRectangularBranchItem* root = nullptr;
    qreal scale = 1;

    void setupLayoutSettingsMenu(QMenu* m);
    void setupShowLabelsMenu(QMenu* m);
    void setupExportTreeImageMenu(QMenu* m);

protected:
    TreeViewerUI* ui = nullptr;
};

class U2VIEW_EXPORT TreeViewerUI : public QGraphicsView {
    Q_OBJECT
    Q_DISABLE_COPY(TreeViewerUI)

public:
    TreeViewerUI(TreeViewer* treeViewer);
    virtual ~TreeViewerUI();

    static const qreal ZOOM_COEF;
    static const qreal MINIMUM_ZOOM;
    static const qreal MAXIMUM_ZOOM;
    static const int MARGIN;
    static const qreal SIZE_COEF;

    const QMap<TreeViewOption, QVariant>& getSettings() const;
    QVariant getOptionValue(TreeViewOption option) const;
    void setOptionValue(TreeViewOption option, QVariant value);

    void updateSettings(const OptionsMap& settings);
    void changeOption(TreeViewOption option, const QVariant& newValue);

    qreal getHorizontalZoom() const {
        return horizontalScale;
    }
    void setHorizontalZoom(qreal z) {
        horizontalScale = z;
    }

    qreal getVerticalZoom() const {
        return verticalScale;
    }
    void setVerticalZoom(qreal z) {
        verticalScale = z;
    }

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

    /** Returns root item for the tree. */
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

    void setZoom(qreal newZoom);

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

    virtual void onSettingsChanged(const TreeViewOption& option, const QVariant& newValue);

signals:
    void si_optionChanged(TreeViewOption option, const QVariant& value);

protected slots:
    virtual void sl_swapTriggered();
    virtual void sl_collapseTriggered();
    virtual void sl_rectLayoutRecomputed();
    virtual void sl_onBranchCollapsed(GraphicsRectangularBranchItem*);
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
    void sl_layoutRecomputed();
    void sl_textSettingsTriggered();
    void sl_treeSettingsTriggered();
    void sl_rerootTriggered();

    void sl_setSettingsTriggered();
    void sl_branchSettings();

private:
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

    void redrawRectangularLayout();
    bool isSelectedCollapsed();

    void setScale(qreal s) {
        view_scale = s;
    }
    qreal getScale() {
        return view_scale;
    }

    void updateActionsState();

    qreal avgWidth();

    void updateLabelsAlignment();

    void determineBranchLengths();

    void changeLayout(TreeLayout newLayout);
    void changeNamesDisplay();
    void changeNodeValuesDisplay();
    void changeLabelsAlignment();

    void initializeSettings();

    PhyTreeObject* phyObject = nullptr;
    GraphicsBranchItem* root = nullptr;
    qreal maxNameWidth = 0;
    qreal verticalScale = 1;
    qreal horizontalScale = 1;
    qreal view_scale;
    CreateBranchesTask* layoutTask = nullptr;
    QGraphicsLineItem* legend = nullptr;
    QGraphicsSimpleTextItem* scalebarText = nullptr;
    QMenu* buttonPopup = nullptr;

    const TreeViewer* curTreeViewer = nullptr;

    QAction* swapAction = nullptr;
    QAction* rerootAction = nullptr;
    QAction* collapseAction = nullptr;
    QAction* setColorAction = nullptr;
    QAction* captureAction = nullptr;
    QAction* exportAction = nullptr;

    OptionsMap settings;
    bool dontSendOptionChangedSignal = false;

protected:
    GraphicsRectangularBranchItem* rectRoot = nullptr;
    QAction* zoomToAction = nullptr;
    QAction* zoomOutAction = nullptr;
    QAction* zoomToAllAction = nullptr;

    /** View global pos of the last mouse-press event. */
    QPoint lastMousePressPos;
};

}  // namespace U2
#endif
