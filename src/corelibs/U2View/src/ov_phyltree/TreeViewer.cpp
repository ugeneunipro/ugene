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

#include "TreeViewer.h"

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QQueue>
#include <QSplitter>
#include <QStack>
#include <QSvgGenerator>
#include <QTextStream>
#include <QTimer>
#include <QtMath>

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OrderedToolbar.h>

#include "CircularTreeLayoutAlgorithm.h"
#include "GraphicsBranchItem.h"
#include "GraphicsButtonItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "RectangularTreeLayoutAlgorithm.h"
#include "TreeViewerFactory.h"
#include "TreeViewerState.h"
#include "TreeViewerTasks.h"
#include "TreeViewerUtils.h"
#include "UnrootedTreeLayoutAlgorithm.h"
#include "phyltree/BranchSettingsDialog.h"
#include "phyltree/TextSettingsDialog.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {

TreeViewer::TreeViewer(const QString& viewName, PhyTreeObject* _phyObject)
    : GObjectView(TreeViewerFactory::ID, viewName), phyObject(_phyObject) {
    GCOUNTER(cvar, "PhylTreeViewer");

    root = RectangularTreeLayoutAlgorithm::buildTreeLayout(phyObject->getTree()->getRootNode());
    root->initDistanceText();

    objects.append(phyObject);
    requiredObjects.append(phyObject);
    onObjectAdded(phyObject);

    connect(phyObject, &PhyTreeObject::si_phyTreeChanged, this, [this] {
        CHECK(ui != nullptr, );
        ui->rebuildTreeLayout();
    });
}

QTransform TreeViewer::getTransform() const {
    return ui->transform();
}

void TreeViewer::setTransform(const QTransform& m) {
    ui->setTransform(m);
}

QVariantMap TreeViewer::saveState() {
    return TreeViewerState::saveState(this);
}

double TreeViewer::getZoomLevel() const {
    return ui->zoomLevel;
}

void TreeViewer::setZoomLevel(double zoomLevel) {
    ui->setZoomLevel(zoomLevel);
}

QVariantMap TreeViewer::getSettingsState() const {
    return ui->getSettingsState();
}

void TreeViewer::setSettingsState(const QVariantMap& m) {
    ui->setSettingsState(m);
}

Task* TreeViewer::updateViewTask(const QString& stateName, const QVariantMap& stateData) {
    return new UpdateTreeViewerTask(this, stateName, stateData);
}

OptionsPanel* TreeViewer::getOptionsPanel() {
    return optionsPanel;
}

void TreeViewer::createActions() {
    // Tree Settings
    treeSettingsAction = new QAction(QIcon(":core/images/phylip.png"), tr("Tree Settings..."), ui);

    // Layout
    layoutActionGroup = new QActionGroup(ui);

    rectangularLayoutAction = new QAction(tr("Rectangular"), layoutActionGroup);
    rectangularLayoutAction->setCheckable(true);
    rectangularLayoutAction->setChecked(true);
    rectangularLayoutAction->setObjectName("Rectangular");

    circularLayoutAction = new QAction(tr("Circular"), layoutActionGroup);
    circularLayoutAction->setCheckable(true);
    circularLayoutAction->setObjectName("Circular");

    unrootedLayoutAction = new QAction(tr("Unrooted"), layoutActionGroup);
    unrootedLayoutAction->setCheckable(true);
    unrootedLayoutAction->setObjectName("Unrooted");

    // Branch Settings
    branchesSettingsAction = new QAction(QIcon(":core/images/color_wheel.png"), tr("Branch Settings..."), ui);
    branchesSettingsAction->setObjectName("Branch Settings");
    collapseAction = new QAction(QIcon(":/core/images/collapse_tree.png"), tr("Collapse"), ui);
    collapseAction->setObjectName("Collapse");
    rerootAction = new QAction(QIcon(":/core/images/reroot.png"), tr("Reroot tree"), ui);
    rerootAction->setObjectName("Reroot tree");
    swapAction = new QAction(QIcon(":core/images/swap.png"), tr("Swap Siblings"), ui);
    swapAction->setObjectName("Swap Siblings");

    // Show Labels
    nameLabelsAction = new QAction(tr("Show Names"), ui);
    nameLabelsAction->setCheckable(true);
    nameLabelsAction->setChecked(true);
    nameLabelsAction->setObjectName("Show Names");
    // Show Node Labels
    nodeLabelsAction = new QAction(tr("Show Node Labels"), ui);
    nodeLabelsAction->setCheckable(phyObject->haveNodeLabels());
    nodeLabelsAction->setChecked(true);
    nodeLabelsAction->setObjectName("Show Names");

    distanceLabelsAction = new QAction(tr("Show Distances"), ui);
    distanceLabelsAction->setCheckable(true);
    distanceLabelsAction->setChecked(true);
    distanceLabelsAction->setObjectName("Show Distances");

    // Formatting
    textSettingsAction = new QAction(QIcon(":core/images/font.png"), tr("Formatting..."), ui);

    // Align Labels
    // Note: the icon is truncated to 15 px height to look properly in the main menu when it is checked
    alignTreeLabelsAction = new QAction(QIcon(":core/images/align_tree_labels.png"), tr("Align Labels"), ui);
    alignTreeLabelsAction->setCheckable(true);
    alignTreeLabelsAction->setObjectName("Align Labels");

    // Zooming
    zoomToSelectionAction = new QAction(QIcon(":core/images/zoom_in.png"), tr("Zoom In"), ui);
    zoomOutAction = new QAction(QIcon(":core/images/zoom_out.png"), tr("Zoom Out"), ui);
    zoomToAllAction = new QAction(QIcon(":core/images/zoom_whole.png"), tr("Reset Zooming"), ui);

    // Print Tree
    printAction = new QAction(QIcon(":/core/images/printer.png"), tr("Print Tree..."), ui);

    // Screen Capture
    captureTreeAction = new QAction(tr("Screen Capture..."), ui);
    captureTreeAction->setObjectName("Screen Capture");
    exportAction = new QAction(tr("Whole Tree as SVG..."), ui);
    exportAction->setObjectName("Whole Tree as SVG");
}

void TreeViewer::setupLayoutSettingsMenu(QMenu* m) {
    m->addActions(layoutActionGroup->actions());
}

void TreeViewer::setupShowLabelsMenu(QMenu* m) {
    m->addAction(nameLabelsAction);
    m->addAction(distanceLabelsAction);
}

void TreeViewer::setupExportTreeImageMenu(QMenu* m) {
    m->addAction(captureTreeAction);
    m->addAction(exportAction);
}

void TreeViewer::buildStaticToolbar(QToolBar* tb) {
    // Layout
    QToolButton* layoutButton = new QToolButton(tb);
    QMenu* layoutMenu = new QMenu(tr("Layout"), ui);
    setupLayoutSettingsMenu(layoutMenu);
    layoutButton->setDefaultAction(layoutMenu->menuAction());
    layoutButton->setPopupMode(QToolButton::InstantPopup);
    layoutButton->setIcon(QIcon(":core/images/tree_layout.png"));
    layoutButton->setObjectName("Layout");
    tb->addWidget(layoutButton);

    // Labels and Text Settings
    tb->addSeparator();
    QToolButton* showLabelsButton = new QToolButton();
    QMenu* showLabelsMenu = new QMenu(tr("Show Labels"), ui);
    showLabelsButton->setObjectName("Show Labels");
    setupShowLabelsMenu(showLabelsMenu);
    showLabelsButton->setDefaultAction(showLabelsMenu->menuAction());
    showLabelsButton->setPopupMode(QToolButton::InstantPopup);
    showLabelsButton->setIcon(QIcon(":/core/images/text_ab.png"));
    tb->addWidget(showLabelsButton);

    tb->addAction(textSettingsAction);

    // Print and Capture
    tb->addSeparator();

    auto exportTreeImageButton = new QToolButton();
    auto exportTreeImageButtonMenu = new QMenu(tr("Export Tree Image"), ui);
    setupExportTreeImageMenu(exportTreeImageButtonMenu);
    exportTreeImageButton->setDefaultAction(exportTreeImageButtonMenu->menuAction());
    exportTreeImageButton->setPopupMode(QToolButton::InstantPopup);
    exportTreeImageButton->setIcon(QIcon(":/core/images/cam2.png"));
    exportTreeImageButtonMenu->menuAction()->setObjectName("Export Tree Image");
    exportTreeImageButton->setObjectName("exportTreeImageButton");
    tb->addWidget(exportTreeImageButton);
    tb->addAction(printAction);
    tb->addSeparator();

    // Tree Settings
    tb->addAction(treeSettingsAction);

    // Branch Settings
    tb->addAction(alignTreeLabelsAction);
    tb->addAction(branchesSettingsAction);

    tb->addSeparator();
    tb->addAction(collapseAction);
    tb->addAction(rerootAction);
    tb->addAction(swapAction);

    // Zooming
    tb->addSeparator();
    tb->addAction(zoomToSelectionAction);
    tb->addAction(zoomOutAction);
    tb->addAction(zoomToAllAction);
}

void TreeViewer::buildMSAEditorStaticToolbar(QToolBar* tb) {
    buildStaticToolbar(tb);
    tb->removeAction(zoomToSelectionAction);
    tb->removeAction(zoomOutAction);
    tb->removeAction(zoomToAllAction);
}

void TreeViewer::buildMenu(QMenu* m, const QString& type) {
    if (type != GObjectViewMenuType::STATIC) {
        GObjectView::buildMenu(m, type);
        return;
    }
    // Tree Settings
    m->addAction(treeSettingsAction);

    // Layout
    QMenu* layoutMenu = new QMenu(tr("Layout"), ui);
    setupLayoutSettingsMenu(layoutMenu);
    layoutMenu->setIcon(QIcon(":core/images/tree_layout.png"));
    m->addMenu(layoutMenu);

    // Branch Settings
    m->addAction(branchesSettingsAction);
    m->addAction(collapseAction);
    m->addAction(rerootAction);
    m->addAction(swapAction);

    // Labels and Text Settings
    m->addSeparator();

    QMenu* labelsMenu = new QMenu(tr("Show Labels"), ui);
    labelsMenu->menuAction()->setObjectName("show_labels_action");
    setupShowLabelsMenu(labelsMenu);
    labelsMenu->setIcon(QIcon(":/core/images/text_ab.png"));
    m->addMenu(labelsMenu);

    m->addAction(textSettingsAction);

    m->addAction(alignTreeLabelsAction);
    // Zooming
    m->addSeparator();
    m->addAction(zoomToSelectionAction);
    m->addAction(zoomOutAction);
    m->addAction(zoomToAllAction);

    // Print and Capture
    m->addSeparator();
    m->addAction(printAction);

    QMenu* cameraMenu = new QMenu(tr("Export Tree Image"), ui);
    setupExportTreeImageMenu(cameraMenu);
    cameraMenu->setIcon(QIcon(":/core/images/cam2.png"));
    cameraMenu->menuAction()->setObjectName("Export Tree Image");
    m->addMenu(cameraMenu);

    m->addSeparator();

    GObjectView::buildMenu(m, type);
    GUIUtils::disableEmptySubmenus(m);
}

QWidget* TreeViewer::createWidget() {
    assert(ui == nullptr);
    ui = new TreeViewerUI(this);

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_PhylogeneticTree));

    QList<OPWidgetFactory*> opWidgetFactoriesForSeqView = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    foreach (OPWidgetFactory* factory, opWidgetFactoriesForSeqView) {
        optionsPanel->addGroup(factory);
    }

    qDeleteAll(filters);

    return ui;
}

void TreeViewer::onObjectRenamed(GObject*, const QString&) {
    // update title
    OpenTreeViewerTask::updateTitle(this);
}

////////////////////////////
// TreeViewerUI

/** Zoom level change per single zoomIn/zoomOut operation. */
static constexpr double ZOOM_LEVEL_STEP = 1.2;

/** Minimum zoom level: 1/4 of the fit-to-view size. */
static constexpr double MINIMUM_ZOOM = 0.25;

/**
 * Maximum zoom level: 100x of the fit-to-view size.
 * TODO: a static value may be not enough for very big trees: make this value dynamic.
 */
static const double MAXIMUM_ZOOM = 100.0;

/** Margins around the whole tree on the scene. On-screen pixels. */
static constexpr int TREE_MARGINS = 10;

static constexpr double SIZE_COEF = 0.1;

TreeViewerUI::TreeViewerUI(TreeViewer* _treeViewer)
    : phyObject(_treeViewer->getPhyObject()),
      root(_treeViewer->getRoot()),
      treeViewer(_treeViewer),
      rectRoot(_treeViewer->getRoot()) {
    updateDistanceToViewScale();
    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::PHYLOGENETIC_TREE).icon);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setScene(new QGraphicsScene());
    scene()->addItem(root);
    initializeSettings();
    addLegend();
    updateRect();

    treeViewer->createActions();
    connect(treeViewer->nameLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_showNameLabelsTriggered(bool)));
    connect(treeViewer->distanceLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_showDistanceLabelsTriggered(bool)));
    connect(treeViewer->printAction, SIGNAL(triggered()), SLOT(sl_printTriggered()));
    connect(treeViewer->captureTreeAction, SIGNAL(triggered()), SLOT(sl_captureTreeTriggered()));
    connect(treeViewer->exportAction, SIGNAL(triggered()), SLOT(sl_exportTriggered()));
    connect(treeViewer->alignTreeLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_contTriggered(bool)));
    connect(treeViewer->rectangularLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_rectangularLayoutTriggered()));
    connect(treeViewer->circularLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_circularLayoutTriggered()));
    connect(treeViewer->unrootedLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_unrootedLayoutTriggered()));
    connect(treeViewer->textSettingsAction, SIGNAL(triggered()), SLOT(sl_textSettingsTriggered()));
    connect(treeViewer->treeSettingsAction, SIGNAL(triggered()), SLOT(sl_treeSettingsTriggered()));
    connect(treeViewer->zoomToSelectionAction, SIGNAL(triggered()), SLOT(sl_zoomToSel()));
    connect(treeViewer->zoomOutAction, SIGNAL(triggered()), SLOT(sl_zoomOut()));
    connect(treeViewer->zoomToAllAction, SIGNAL(triggered()), SLOT(sl_zoomToAll()));
    connect(treeViewer->branchesSettingsAction, SIGNAL(triggered()), SLOT(sl_setSettingsTriggered()));
    connect(treeViewer->collapseAction, SIGNAL(triggered()), SLOT(sl_collapseTriggered()));
    connect(treeViewer->rerootAction, SIGNAL(triggered()), SLOT(sl_rerootTriggered()));
    connect(treeViewer->swapAction, SIGNAL(triggered()), SLOT(sl_swapTriggered()));

    buttonPopup = new QMenu(this);

    // chrootAction->setEnabled(false); //not implemented yet

    buttonPopup->addAction(treeViewer->zoomToSelectionAction);
    buttonPopup->addAction(treeViewer->zoomOutAction);
    buttonPopup->addAction(treeViewer->zoomToAllAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(treeViewer->swapAction);
    treeViewer->swapAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->rerootAction);
    treeViewer->rerootAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->collapseAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(treeViewer->branchesSettingsAction);

    auto cameraMenu = new QMenu(tr("Export Tree Image"), this);
    cameraMenu->addAction(treeViewer->captureTreeAction);
    cameraMenu->addAction(treeViewer->exportAction);
    cameraMenu->menuAction()->setObjectName("Export Tree Image");
    cameraMenu->setIcon(QIcon(":/core/images/cam2.png"));
    buttonPopup->addMenu(cameraMenu);

    updateActionsState();
    setObjectName("treeView");
    updateScene(true);
    connect(root, &GraphicsBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
}

TreeViewerUI::~TreeViewerUI() {
    delete scene();
}

const QMap<TreeViewOption, QVariant>& TreeViewerUI::getSettings() const {
    return settings;
}

QVariant TreeViewerUI::getOptionValue(TreeViewOption option) const {
    return settings[option];
}

void TreeViewerUI::setOptionValue(TreeViewOption option, QVariant value) {
    settings[option] = value;
    if (!dontSendOptionChangedSignal) {
        emit si_optionChanged(option, value);
    }
}

void TreeViewerUI::setTreeLayout(const TreeLayout& newLayout) {
    switch (newLayout) {
        case RECTANGULAR_LAYOUT:
            treeViewer->rectangularLayoutAction->setChecked(true);
            changeTreeLayout(RECTANGULAR_LAYOUT);
            break;
        case CIRCULAR_LAYOUT:
            treeViewer->circularLayoutAction->setChecked(true);
            changeTreeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            treeViewer->unrootedLayoutAction->setChecked(true);
            changeTreeLayout(UNROOTED_LAYOUT);
            break;
        default:
            FAIL("Unrecognized tree layout", );
    }
}

TreeLayout TreeViewerUI::getTreeLayout() const {
    return static_cast<TreeLayout>(getOptionValue(TREE_LAYOUT).toInt());
}

void TreeViewerUI::updateSettings(const OptionsMap& newSettings) {
    QList<TreeViewOption> keys = newSettings.keys();
    for (const TreeViewOption& curOption : qAsConst(keys)) {
        onSettingsChanged(curOption, newSettings[curOption]);
    }
}

void TreeViewerUI::changeOption(TreeViewOption option, const QVariant& newValue) {
    dontSendOptionChangedSignal = true;
    onSettingsChanged(option, newValue);
    dontSendOptionChangedSignal = false;
}

void TreeViewerUI::onSettingsChanged(const TreeViewOption& option, const QVariant& newValue) {
    SAFE_POINT(settings.keys().contains(option), "Unrecognized option in TreeViewerUI::onSettingsChanged", );
    if (option != TREE_LAYOUT) {  // TREE_LAYOUT setting is updated as a part of 'setTreeLayout' call below.
        setOptionValue(option, newValue);
    }
    switch (option) {
        case TREE_LAYOUT:
            setTreeLayout(static_cast<TreeLayout>(newValue.toInt()));
            break;
        case BRANCHES_TRANSFORMATION_TYPE:
            // Recompute rect layout & re-created derived layout by 'changeTreeLayout()' if needed.
            updateRectLayoutBranches();
            changeTreeLayout(getTreeLayout());
            break;
        case WIDTH_COEF:
        case HEIGHT_COEF:
            updateScene(true);
            break;
        case LABEL_COLOR:
        case LABEL_FONT_TYPE:
        case LABEL_FONT_SIZE:
        case LABEL_FONT_BOLD:
        case LABEL_FONT_ITALIC:
        case LABEL_FONT_UNDERLINE:
            updateTextSettings(option);
            break;
        case BRANCH_COLOR:
        case BRANCH_THICKNESS:
        case NODE_COLOR:
        case NODE_RADIUS:
            updateSettings();
            break;
        case SHOW_LABELS:
            changeNamesDisplay();
            treeViewer->nameLabelsAction->setChecked(newValue.toBool());
            break;
        case SHOW_DISTANCES:
            showLabels(LabelType_Distance);
            treeViewer->distanceLabelsAction->setChecked(newValue.toBool());
            break;
        case SHOW_NODE_LABELS:
            changeNodeValuesDisplay();
            break;
        case ALIGN_LABELS:
            changeLabelsAlignment();
            treeViewer->alignTreeLabelsAction->setChecked(newValue.toBool());
            break;
        case SCALEBAR_RANGE:
        case SCALEBAR_FONT_SIZE:
        case SCALEBAR_LINE_WIDTH:
            updateLegend();
            break;
        default:
            FAIL("Unrecognized option in TreeViewerUI::onSettingsChanged", );
            break;
    }
}

void TreeViewerUI::sl_setSettingsTriggered() {
    sl_branchSettings();
}

void TreeViewerUI::sl_branchSettings() {
    QObjectScopedPointer<BranchSettingsDialog> dialog = new BranchSettingsDialog(this, getSettings());
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (dialog->result() == QDialog::Accepted) {
        updateSettings(dialog->getSettings());
    }
}

void TreeViewerUI::initializeSettings() {
    setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
    setOptionValue(BRANCHES_TRANSFORMATION_TYPE, DEFAULT);
    setOptionValue(SCALEBAR_RANGE, 30.0 / distanceToViewScale);
    setOptionValue(SCALEBAR_FONT_SIZE, TreeViewerUtils::getFont().pointSize());
    setOptionValue(SCALEBAR_LINE_WIDTH, 1);

    setOptionValue(LABEL_COLOR, QColor(Qt::darkGray));
    setOptionValue(LABEL_FONT_TYPE, TreeViewerUtils::getFont());
    setOptionValue(LABEL_FONT_SIZE, TreeViewerUtils::getFont().pointSize());
    setOptionValue(LABEL_FONT_BOLD, false);
    setOptionValue(LABEL_FONT_ITALIC, false);
    setOptionValue(LABEL_FONT_UNDERLINE, false);

    setOptionValue(SHOW_LABELS, true);
    setOptionValue(SHOW_DISTANCES, !phyObject->haveNodeLabels());
    setOptionValue(SHOW_NODE_LABELS, phyObject->haveNodeLabels());
    setOptionValue(ALIGN_LABELS, false);

    setOptionValue(BRANCH_COLOR, QColor(0, 0, 0));
    setOptionValue(BRANCH_THICKNESS, 1);

    setOptionValue(WIDTH_COEF, 1);
    setOptionValue(HEIGHT_COEF, 1);

    setOptionValue(NODE_RADIUS, 2);
    setOptionValue(NODE_COLOR, QColor(0, 0, 0));

    for (unsigned int i = 0; i < OPTION_ENUM_END; i++) {
        TreeViewOption option = static_cast<TreeViewOption>(i);
        SAFE_POINT(settings.keys().contains(option), "Not all options have been initialized", );
    }
}

void TreeViewerUI::updateSettings() {
    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();
    if (updatingItems.isEmpty()) {
        updatingItems = items();

        if (auto legendLineItem = dynamic_cast<QGraphicsLineItem*>(legend)) {
            QPen legendPen;
            QColor branchColor = qvariant_cast<QColor>(getOptionValue(BRANCH_COLOR));
            legendPen.setColor(branchColor);
            legendLineItem->setPen(legendPen);
        }
    }

    for (QGraphicsItem* graphItem : qAsConst(updatingItems)) {
        if (auto branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem)) {
            branchItem->updateSettings(settings);
            if (branchItem->correspondingRectangularBranchItem != nullptr) {
                branchItem->correspondingRectangularBranchItem->updateSettings(settings);
            }
        }
        scene()->update();
    }
}

static QSet<QGraphicsItem*> getAllLevelChildItems(QGraphicsItem* item) {
    QSet<QGraphicsItem*> result;
    QList<QGraphicsItem*> directChildren = item->childItems();
    for (auto directChild : qAsConst(directChildren)) {
        result += directChild;
        result += getAllLevelChildItems(directChild);
    }
    return result;
}

void TreeViewerUI::updateTextSettings(const TreeViewOption& option) {
    QSet<QGraphicsItem*> itemsToUpdate = scene()->selectedItems().toSet();
    if (itemsToUpdate.isEmpty()) {
        itemsToUpdate = scene()->items().toSet();
    } else {
        QSet<QGraphicsItem*> rootItems = itemsToUpdate;
        for (auto item : qAsConst(rootItems)) {
            itemsToUpdate += getAllLevelChildItems(item);
        }
    }

    QVariant optionValue = getOptionValue(option);
    for (auto item : qAsConst(itemsToUpdate)) {
        if (auto branchItem = dynamic_cast<GraphicsBranchItem*>(item)) {
            if (option == LABEL_COLOR || option == LABEL_FONT_TYPE || option == LABEL_FONT_SIZE ||
                option == LABEL_FONT_BOLD || option == LABEL_FONT_ITALIC || option == LABEL_FONT_UNDERLINE) {
                branchItem->updateTextProperty(option, optionValue);
                if (branchItem->correspondingRectangularBranchItem != nullptr) {
                    branchItem->correspondingRectangularBranchItem->updateTextProperty(option, optionValue);
                }
            }
        } else if (auto buttonItem = dynamic_cast<GraphicsButtonItem*>(item)) {
            buttonItem->updateSettings(getSettings());
        } else if (auto legendText = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
            if (option == LABEL_COLOR) {
                legendText->setBrush(qvariant_cast<QColor>(optionValue));
            }
        }
    }

    updateLayout();
    updateScene(true);
}

void TreeViewerUI::updateRectLayoutBranches() {
    TreeType type = static_cast<TreeType>(getOptionValue(BRANCHES_TRANSFORMATION_TYPE).toInt());
    legend->setVisible(type == PHYLOGRAM);

    updateStepsToLeafOnBranches();
    double averageBranchDistance = getAverageBranchDistance();
    int heightCoef = getOptionValue(HEIGHT_COEF).toInt();

    QStack<GraphicsBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsBranchItem* item = stack.pop();
        const QList<QGraphicsItem*>& childItems = item->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<GraphicsBranchItem*>(childItem)) {
                stack.push(childBranchItem);
            }
        }

        if (item == rectRoot) {
            continue;
        }

        auto rectItem = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        SAFE_POINT(rectItem != nullptr, "Not a rect root!", );
        rectItem->setHeightCoef(heightCoef);

        double coef = qMax(1.0, SIZE_COEF * getOptionValue(WIDTH_COEF).toInt());

        switch (type) {
            case DEFAULT:
                if (item->getDistanceTextItem() != nullptr && item->getDistanceTextItem()->text() == "") {
                    item->setDistanceText("0");
                }
                if (item->getNameTextItem() == nullptr) {
                    item->setWidth(averageBranchDistance * distanceToViewScale * coef * item->maxStepsToLeafParentDelta);
                } else {
                    item->setWidth(0);
                }
                break;
            case PHYLOGRAM:
                if (item->getDistanceTextItem() != nullptr && item->getDistanceTextItem()->text() == "0") {
                    item->setDistanceText("");
                }
                item->setWidth(qAbs(item->getDist()) * distanceToViewScale * coef);
                break;
            case CLADOGRAM:
                if (item->getDistanceTextItem() != nullptr && item->getDistanceTextItem()->text() == "") {
                    item->setDistanceText("0");
                }
                if (item->getNameTextItem() == nullptr) {
                    item->setWidth(averageBranchDistance * distanceToViewScale * coef);
                } else {
                    item->setWidth(0);
                }
                break;
            default:
                FAIL("Unexpected tree type value", );
                break;
        }
    }
}

void TreeViewerUI::updateScene(bool fitSceneToView) {
    updateRectLayoutBranches();
    updateLegend();
    updateRect();
    scene()->update();

    showLabels(LabelType_Distance);
    showLabels(LabelType_SequenceName);
    bool alignLabels = getOptionValue(ALIGN_LABELS).toBool();
    if (alignLabels) {
        updateLabelsAlignment();
    }

    defaultZoom();
    if (fitSceneToView) {
        fitIntoView();
    }
}

void TreeViewerUI::updateStepsToLeafOnBranches() {
    QStack<GraphicsBranchItem*> stack;
    QQueue<GraphicsBranchItem*> childQueue;
    // Assign 'maxStepsToLeaf' value.
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsBranchItem* branchItem = stack.pop();
        branchItem->maxStepsToLeaf = 0;
        branchItem->maxStepsToLeafParentDelta = 0;
        int childBranchCount = 0;
        const QList<QGraphicsItem*>& childItems = branchItem->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<GraphicsBranchItem*>(childItem)) {
                stack.push(childBranchItem);
                childBranchCount++;
            }
        }
        if (branchItem == rectRoot) {
            continue;
        }
        if (childBranchCount == 0) {
            childQueue.enqueue(branchItem);
            continue;
        }
    }
    while (!childQueue.isEmpty()) {
        GraphicsBranchItem* childBranchItem = childQueue.dequeue();
        if (auto parentBranchItem = dynamic_cast<GraphicsBranchItem*>(childBranchItem->parentItem())) {
            parentBranchItem->maxStepsToLeaf = qMax(parentBranchItem->maxStepsToLeaf, childBranchItem->maxStepsToLeaf + 1);
            childQueue.enqueue(parentBranchItem);
        }
    }
    // Assign 'maxStepsToLeafParentDelta' value.
    SAFE_POINT(stack.isEmpty(), "Stack must be empty", );
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsBranchItem* branchItem = stack.pop();
        const QList<QGraphicsItem*>& childItems = branchItem->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<GraphicsBranchItem*>(childItem)) {
                stack.push(childBranchItem);
                childBranchItem->maxStepsToLeafParentDelta = branchItem->maxStepsToLeaf - childBranchItem->maxStepsToLeaf;
                SAFE_POINT(childBranchItem->maxStepsToLeafParentDelta > 0, "Invalid maxStepsToLeafParentDelta", );
            }
        }
    }
}

QVariantMap TreeViewerUI::getSettingsState() const {
    QString branchColor("branch_color");
    QString branchThisckness("branch_thickness");
    QVariantMap m;

    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        GraphicsBranchItem* branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem != nullptr) {
            OptionsMap branchSettings = branchItem->getSettings();
            m[branchColor] = qvariant_cast<QColor>(branchSettings[BRANCH_COLOR]);
            m[branchThisckness + i] = branchSettings[BRANCH_THICKNESS].toInt();
            i++;
        }
    }

    return m;
}

void TreeViewerUI::setSettingsState(const QVariantMap& m) {
    QString branchColor("branch_color");
    QString branchThisckness("branch_thickness");
    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        if (auto branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem)) {
            OptionsMap branchSettings = branchItem->getSettings();

            QVariant vColor = m[branchColor + i];
            if (vColor.type() == QVariant::Color) {
                branchSettings[BRANCH_COLOR] = vColor.value<QColor>();
            }

            QVariant vThickness = m[branchThisckness + i];
            if (vThickness.type() == QVariant::Int) {
                branchSettings[BRANCH_THICKNESS] = vThickness.toInt();
            }

            branchItem->updateSettings(branchSettings);
            i++;
        }
    }
}

void TreeViewerUI::addLegend() {
    qreal d = getOptionValue(SCALEBAR_RANGE).toReal();
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i)
        ;
    if (str[i] == '.') {
        --i;
    }
    str.truncate(i + 1);

    legend = new QGraphicsLineItem(0, 0, 0, 0);
    scalebarText = new QGraphicsSimpleTextItem("", legend);
    updateLegend();

    scene()->addItem(legend);
}

void TreeViewerUI::updateLegend() {
    qreal coef = qMax(1.0, SIZE_COEF * getOptionValue(WIDTH_COEF).toInt());
    qreal WIDTH = getOptionValue(SCALEBAR_RANGE).toDouble() * coef * distanceToViewScale;

    qreal d = getOptionValue(SCALEBAR_RANGE).toDouble();
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i)
        ;
    if (str[i] == '.')
        --i;
    str.truncate(i + 1);

    scalebarText->setText(str);

    QFont curFont = TreeViewerUtils::getFont();
    curFont.setPointSize(getOptionValue(SCALEBAR_FONT_SIZE).toInt());
    scalebarText->setFont(curFont);

    QPen curPen = legend->pen();
    curPen.setWidth(getOptionValue(SCALEBAR_LINE_WIDTH).toInt());
    legend->setPen(curPen);

    legend->setLine(0, 0, WIDTH, 0);

    QRectF rect = scalebarText->boundingRect();
    scalebarText->setPos(0.5 * (WIDTH - rect.width()), -rect.height());
}

void TreeViewerUI::wheelEvent(QWheelEvent* we) {
    double newZoomLevel = zoomLevel * pow(ZOOM_LEVEL_STEP, we->delta() / 120.0);
    setZoomLevel(newZoomLevel);
    we->accept();
}

void TreeViewerUI::setZoomLevel(double newZoomLevel) {
    CHECK(newZoomLevel >= MINIMUM_ZOOM && newZoomLevel <= MAXIMUM_ZOOM, );

    uiLog.trace("New zoom level: " + QString::number(newZoomLevel));
    double scaleChange = newZoomLevel / zoomLevel;
    zoomLevel = newZoomLevel;

    double nodeScale = zoomLevel * (isRectangularLayoutMode() ? 1.2 : 0.4);
    QList<QGraphicsItem*> itemList = scene()->items();
    for (QGraphicsItem* item : qAsConst(itemList)) {
        if (auto nodeItem = dynamic_cast<GraphicsButtonItem*>(item)) {
            nodeItem->setScale(1 / nodeScale);
        }
    }
    scale(scaleChange, scaleChange);
    updateActionsState();
}

void TreeViewerUI::mousePressEvent(QMouseEvent* e) {
    lastMousePressPos = e->globalPos();
    isSelectionStateManagedByChildOnClick = false;
    QList<QGraphicsItem*> selectedItemBeforeRightClick;
    if (e->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
    } else if (e->button() == Qt::RightButton) {
        selectedItemBeforeRightClick = scene()->selectedItems();
        QTimer::singleShot(0, this, [this] { buttonPopup->popup(lastMousePressPos); });
    }
    QGraphicsView::mousePressEvent(e);

    // Calling QGraphicsView::mousePressEvent() for both right/left clicks to deliver right/left clicks to children.
    // QGraphicsView::mousePressEvent() resets the selection when clicked on empty space.
    // Restore it back if not processed by child for right clicks (context menu).
    if (e->button() == Qt::RightButton && !isSelectionStateManagedByChildOnClick) {
        for (auto item : qAsConst(selectedItemBeforeRightClick)) {
            item->setSelected(true);
        }
    }
    updateActionsState();
}

void TreeViewerUI::mouseReleaseEvent(QMouseEvent* e) {
    setDragMode(QGraphicsView::NoDrag);
    bool isLeftButton = e->button() == Qt::LeftButton;
    bool isDragEvent = isLeftButton && (e->globalPos() - lastMousePressPos).manhattanLength() >= QApplication::startDragDistance();
    if (!isSelectionStateManagedByChildOnClick && isLeftButton && !isDragEvent) {
        root->setSelectedRecurs(false, true);  // Clear selection on any right button click with no shift.
    }
    updateActionsState();
    updateBranchSettings();
    e->accept();
}

void TreeViewerUI::resizeEvent(QResizeEvent* e) {
    fitIntoView();
    QGraphicsView::resizeEvent(e);
}

void TreeViewerUI::fitIntoView() {
    QRectF rect = scene()->sceneRect();
    rect.setWidth(rect.width() / zoomLevel);
    rect.setHeight(rect.height() / zoomLevel);
    rect.moveCenter(scene()->sceneRect().center());
    fitInView(rect, Qt::KeepAspectRatio);
}

void TreeViewerUI::paint(QPainter& painter) {
    painter.setBrush(Qt::darkGray);
    painter.setFont(TreeViewerUtils::getFont());
    scene()->render(&painter);
}

void TreeViewerUI::updateRect() {
    SAFE_POINT(root != nullptr, "Pointer to tree root is NULL", );
    QTransform viewTransform = transform();
    QRectF rect = root->visibleChildrenBoundingRect(viewTransform) | root->sceneBoundingRect();
    rect.setLeft(rect.left() - TREE_MARGINS);
    rect.setRight(rect.right() + TREE_MARGINS);
    rect.setTop(rect.top() - TREE_MARGINS);
    rect.setBottom(rect.bottom() + legend->childrenBoundingRect().height() + TREE_MARGINS);
    legend->setPos(0, rect.bottom() - TREE_MARGINS);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::sl_swapTriggered() {
    QList<QGraphicsItem*> graphItems = items();
    for (auto graphItem : qAsConst(graphItems)) {
        auto buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
            buttonItem->swapSiblings();
            phyObject->onTreeChanged();
            break;
        }
    }
    double zoomLevelBefore = zoomLevel;
    QTransform curTransform = viewportTransform();
    setTransformationAnchor(NoAnchor);

    recalculateRectangularLayout();
    updateLayout();
    updateScene(true);

    setTransform(curTransform);
    zoomLevel = zoomLevelBefore;
    updateActionsState();
    setTransformationAnchor(AnchorUnderMouse);
}

void TreeViewerUI::sl_rerootTriggered() {
    foreach (QGraphicsItem* graphItem, items()) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
            buttonItem->rerootTree(phyObject);
            break;
        }
    }
}

void TreeViewerUI::collapseSelected() {
    QList<QGraphicsItem*> childItems = items();
    foreach (QGraphicsItem* graphItem, childItems) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
            buttonItem->collapse();
            break;
        }
    }
}

void TreeViewerUI::updateBranchSettings() {
    QList<QGraphicsItem*> childItems = items();
    GraphicsBranchItem* branch = root;
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        auto buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
            branch = dynamic_cast<GraphicsBranchItem*>(buttonItem->parentItem());
            SAFE_POINT(branch != nullptr, "Collapsing is impossible because button has not parent branch", );
            break;
        }
    }
    setOptionValue(BRANCH_THICKNESS, branch->getSettings()[BRANCH_THICKNESS]);
    setOptionValue(BRANCH_COLOR, branch->getSettings()[BRANCH_COLOR]);
    QFont font;
    QColor color;
    if (branch->getDistanceTextItem() != nullptr) {
        font = branch->getDistanceTextItem()->font();
        color = branch->getDistanceTextItem()->brush().color();
    }
    bool isCustomFont = font != QFont();
    bool isCustomColor = color != QColor();
    if (isCustomFont || isCustomColor) {
        dontSendOptionChangedSignal = true;
        setOptionValue(LABEL_FONT_TYPE, font);
        setOptionValue(LABEL_FONT_SIZE, font.pointSize());
        setOptionValue(LABEL_FONT_BOLD, font.bold());
        setOptionValue(LABEL_FONT_ITALIC, font.italic());
        setOptionValue(LABEL_FONT_UNDERLINE, font.underline());
        setOptionValue(LABEL_COLOR, color);
        dontSendOptionChangedSignal = false;
    }
    emit si_updateBranch();
}

bool TreeViewerUI::isSelectedCollapsed() {
    foreach (QGraphicsItem* graphItem, items()) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
            return buttonItem->isCollapsed();
        }
    }
    return false;
}

void TreeViewerUI::sl_collapseTriggered() {
    collapseSelected();
}

void TreeViewerUI::sl_captureTreeTriggered() {
    Document* doc = phyObject->getDocument();
    const GUrl& url = doc->getURL();
    const QString& fileName = url.baseFileName();

    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(viewport(), ExportImageDialog::PHYTreeView, fileName, ExportImageDialog::NoScaling, this);
    dialog->exec();
}

void TreeViewerUI::sl_exportTriggered() {
    QString fileName = phyObject->getDocument()->getName();
    QString format = "SVG - Scalable Vector Graphics (*.svg)";
    TreeViewerUtils::saveImageDialog(format, fileName, format);
    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo dir(QFileInfo(fileName).absolutePath());
    if (!dir.exists() || !dir.isWritable()) {
        QMessageBox::critical(this, tr("Error"), tr("Selected dir is read only: %1").arg(dir.absoluteFilePath()));
        return;
    }

    QRect rect = scene()->sceneRect().toRect();
    rect.moveTo(0, 0);

    QBuffer qbuffer;
    qbuffer.open(QBuffer::ReadWrite);

    QSvgGenerator generator;
    generator.setOutputDevice(&qbuffer);
    generator.setSize(rect.size());
    generator.setViewBox(rect);

    QPainter painter;
    painter.begin(&generator);
    paint(painter);
    painter.end();

    qbuffer.seek(0);  // move pointer to the buffer's start
    QString svgText(qbuffer.readAll());

    if (svgText.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to generate SVG image."));
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open file for writing: %1").arg(fileName));
    }
    QTextStream stream(&file);
    stream << svgText << endl;
}

void TreeViewerUI::sl_contTriggered(bool on) {
    onSettingsChanged(ALIGN_LABELS, on);
}

void TreeViewerUI::changeLabelsAlignment() {
    updateLabelsAlignment();

    TreeLayout curLayout = getTreeLayout();
    switch (curLayout) {
        case CIRCULAR_LAYOUT:
            changeTreeLayout(CIRCULAR_LAYOUT);
            fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
            break;

        case UNROOTED_LAYOUT:
            changeTreeLayout(UNROOTED_LAYOUT);
            fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
            break;
        case RECTANGULAR_LAYOUT:
            // Do nothing
            show();
            break;
    }
}

void TreeViewerUI::sl_rectangularLayoutTriggered() {
    changeTreeLayout(RECTANGULAR_LAYOUT);
}

void TreeViewerUI::sl_circularLayoutTriggered() {
    changeTreeLayout(CIRCULAR_LAYOUT);
}

void TreeViewerUI::sl_unrootedLayoutTriggered() {
    changeTreeLayout(UNROOTED_LAYOUT);
}

/** Expands every collapsed branch in tree. */
static void makeLayoutNotCollapsed(GraphicsBranchItem* branch) {
    if (branch->isCollapsed()) {
        branch->toggleCollapsedState();
    }
    QList<QGraphicsItem*> childItems = branch->childItems();
    for (auto child : qAsConst(childItems)) {
        if (auto childBranch = dynamic_cast<GraphicsBranchItem*>(child)) {
            makeLayoutNotCollapsed(childBranch);
        }
    }
}

void TreeViewerUI::changeTreeLayout(const TreeLayout& newTreeLayout) {
    switch (newTreeLayout) {
        case RECTANGULAR_LAYOUT: {
            setNewTreeLayout(rectRoot, newTreeLayout);
            break;
        }
        case CIRCULAR_LAYOUT: {
            // TODO: support collapsed state transfer.
            makeLayoutNotCollapsed(root);  // Clients are subscribed to 'root'. Expand of the layout emits notifications.
            makeLayoutNotCollapsed(rectRoot);  // Root state & child layout states must be synchronized.
            bool degeneratedCase = distanceToViewScale <= GraphicsRectangularBranchItem::DEFAULT_WIDTH;
            setNewTreeLayout(CircularTreeLayoutAlgorithm::convert(rectRoot, degeneratedCase), newTreeLayout);
            break;
        }
        case UNROOTED_LAYOUT: {
            makeLayoutNotCollapsed(root);  // See comments for CIRCULAR_LAYOUT.
            makeLayoutNotCollapsed(rectRoot);
            setNewTreeLayout(UnrootedTreeLayoutAlgorithm::convert(rectRoot), newTreeLayout);
            break;
        }
    }
}

void TreeViewerUI::rebuildTreeLayout() {
    auto newRectRoot = RectangularTreeLayoutAlgorithm::buildTreeLayout(phyObject->getTree()->getRootNode());
    CHECK_EXT(newRectRoot != nullptr, uiLog.error(tr("Failed to build tree layout.")), );
    CHECK(newRectRoot != nullptr, );
    rectRoot = newRectRoot;
    switch (getTreeLayout()) {
        case CIRCULAR_LAYOUT:
            setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
            changeTreeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
            changeTreeLayout(UNROOTED_LAYOUT);
            break;
        case RECTANGULAR_LAYOUT:
            setNewTreeLayout(rectRoot, RECTANGULAR_LAYOUT);
            break;
    }
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    updateScene(true);
    updateSettings();
    updateTextSettings(LABEL_COLOR);
    updateTextSettings(LABEL_FONT_TYPE);
    updateTextSettings(LABEL_FONT_SIZE);
    updateTextSettings(LABEL_FONT_BOLD);
    updateTextSettings(LABEL_FONT_ITALIC);
    updateTextSettings(LABEL_FONT_UNDERLINE);
}

void TreeViewerUI::sl_onBranchCollapsed(GraphicsBranchItem*) {
    // In rectangular mode perform a complete re-layout of the tree, so there is no empty space left.
    // TODO: do the same in circular & unrooted layouts.
    CHECK(isRectangularLayoutMode(), );

    QTransform curTransform = viewportTransform();
    setTransformationAnchor(NoAnchor);

    recalculateRectangularLayout();
    updateScene(false);

    setTransform(curTransform);
    updateActionsState();
    setTransformationAnchor(AnchorUnderMouse);
}

void TreeViewerUI::setNewTreeLayout(GraphicsBranchItem* newRoot, const TreeLayout& treeLayout) {
    root->setSelectedRecurs(false, true);  // clear selection
    setOptionValue(TREE_LAYOUT, treeLayout);

    scene()->removeItem(root);

    disconnect(root, &GraphicsBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
    root = newRoot;
    connect(newRoot, &GraphicsBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);

    scene()->addItem(root);
    defaultZoom();
    updateRect();

    updateScene(true);
    onLayoutChanged(treeLayout);

    bool showNames = getOptionValue(SHOW_LABELS).toBool();
    bool showDistances = getOptionValue(SHOW_DISTANCES).toBool();

    changeNodeValuesDisplay();

    if (!showNames || !showDistances) {
        LabelTypes lt;
        if (!showDistances) {
            lt |= LabelType_Distance;
        }
        if (!showNames) {
            lt |= LabelType_SequenceName;
        }
        showLabels(lt);
    }
    show();

    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void TreeViewerUI::showLabels(LabelTypes labelTypes) {
    QStack<GraphicsBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }
    maxNameWidth = 0.0;
    while (!stack.isEmpty()) {
        GraphicsBranchItem* node = stack.pop();
        if (labelTypes.testFlag(LabelType_SequenceName)) {
            if (node->getNameTextItem() != nullptr) {
                node->setVisible(getOptionValue(SHOW_LABELS).toBool());
                maxNameWidth = qMax(maxNameWidth, node->getNameTextItem()->sceneBoundingRect().width());
            }
        }
        if (labelTypes.testFlag(LabelType_Distance)) {
            if (node->getDistanceTextItem() != nullptr) {
                node->getDistanceTextItem()->setVisible(getOptionValue(SHOW_DISTANCES).toBool());
            }
        }
        foreach (QGraphicsItem* item, node->childItems()) {
            GraphicsBranchItem* bitem = dynamic_cast<GraphicsBranchItem*>(item);
            if (bitem != nullptr) {
                stack.push(bitem);
            }
        }
    }
}

void TreeViewerUI::sl_showNameLabelsTriggered(bool on) {
    onSettingsChanged(SHOW_LABELS, on);
}

void TreeViewerUI::changeNamesDisplay() {
    bool showNames = getOptionValue(SHOW_LABELS).toBool();
    treeViewer->alignTreeLabelsAction->setEnabled(showNames);

    showLabels(LabelType_SequenceName);
    QRectF rect = sceneRect();
    rect.setWidth(rect.width() + (showNames ? 1 : -1) * maxNameWidth);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::changeNodeValuesDisplay() {
    const QList<QGraphicsItem*> itemList = scene()->items();
    for (QGraphicsItem* curItem : qAsConst(itemList)) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(curItem);
        if (buttonItem != nullptr) {
            buttonItem->updateSettings(getSettings());
        }
    }
}

void TreeViewerUI::sl_showDistanceLabelsTriggered(bool on) {
    onSettingsChanged(SHOW_DISTANCES, on);
}

void TreeViewerUI::sl_printTriggered() {
    QPrinter printer;
    QObjectScopedPointer<QPrintDialog> dialog = new QPrintDialog(&printer, this);
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (dialog->result() != QDialog::Accepted)
        return;

    QPainter painter(&printer);
    paint(painter);
}

void TreeViewerUI::sl_textSettingsTriggered() {
    QObjectScopedPointer<TextSettingsDialog> dialog = new TextSettingsDialog(this, getSettings());
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (QDialog::Accepted == dialog->result()) {
        updateSettings(dialog->getSettings());
        if (getOptionValue(ALIGN_LABELS).toBool()) {
            QStack<GraphicsBranchItem*> stack;
            stack.push(root);
            if (root != rectRoot) {
                stack.push(rectRoot);
            }
            while (!stack.empty()) {
                GraphicsBranchItem* item = stack.pop();
                if (item->getNameTextItem() == nullptr) {
                    foreach (QGraphicsItem* citem, item->childItems()) {
                        GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(citem);
                        if (gbi != nullptr) {
                            stack.push(gbi);
                        }
                    }
                } else {
                    item->setWidth(0);
                }
            }
            updateRect();
            changeLabelsAlignment();
        }
    }
}

void TreeViewerUI::sl_treeSettingsTriggered() {
    QObjectScopedPointer<TreeSettingsDialog> dialog = new TreeSettingsDialog(this, getSettings(), getTreeLayout() == RECTANGULAR_LAYOUT);
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (QDialog::Accepted == dialog->result()) {
        updateSettings(dialog->getSettings());
    }
}

void TreeViewerUI::sl_zoomToSel() {
    QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
    if (selectedItems.isEmpty()) {
        setZoomLevel(ZOOM_LEVEL_STEP);
    } else {
        GraphicsButtonItem* topButton = nullptr;
        foreach (QGraphicsItem* graphItem, selectedItems) {
            GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
            if (buttonItem != nullptr && buttonItem->isPathToRootSelected()) {
                topButton = buttonItem;
                break;
            }
        }
        if (!topButton) {
            setZoomLevel(ZOOM_LEVEL_STEP);
        } else {
            defaultZoom();
            QGraphicsItem* topItem = topButton->parentItem();
            QRectF rect = topItem->mapRectToScene(topItem->childrenBoundingRect());
            QRectF rect1 = scene()->sceneRect();
            qreal zoom1 = qMin(rect1.width() / rect.width(), rect1.height() / rect.height());
            setZoomLevel(zoom1);
            centerOn(rect.center());
        }
    }
}

void TreeViewerUI::sl_zoomOut() {
    setZoomLevel(1.0 / ZOOM_LEVEL_STEP);
}

void TreeViewerUI::sl_zoomToAll() {
    defaultZoom();
}

void TreeViewerUI::defaultZoom() {
    setZoomLevel(1);
}

void TreeViewerUI::recalculateRectangularLayout() {
    int current = 0;
    rectRoot->recalculateBranches(current, phyObject->getTree()->getRootNode());
    updateDistanceToViewScale();
}

void TreeViewerUI::updateDistanceToViewScale() {
    double minDistance = -2;
    double maxDistance = 0;

    QStack<GraphicsRectangularBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsRectangularBranchItem* item = stack.pop();
        minDistance = minDistance == -2 ? item->getDist() : qMin(item->getDist(), minDistance);
        maxDistance = qMax(item->getDist(), maxDistance);
        QList<QGraphicsItem*> childItems = item->childItems();
        for (QGraphicsItem* ci : qAsConst(childItems)) {
            if (auto gbi = dynamic_cast<GraphicsRectangularBranchItem*>(ci)) {
                stack.push(gbi);
            }
        }
    }
    if (minDistance == 0) {
        minDistance = GraphicsRectangularBranchItem::EPSILON;
    }
    if (maxDistance == 0) {
        maxDistance = GraphicsRectangularBranchItem::EPSILON;
    }
    double minDistScale = GraphicsRectangularBranchItem::DEFAULT_WIDTH / minDistance;
    double maxDistScale = GraphicsRectangularBranchItem::MAXIMUM_WIDTH / maxDistance;
    distanceToViewScale = qMin(minDistScale, maxDistScale);

    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsRectangularBranchItem* item = stack.pop();
        item->setWidth(item->getWidth() * distanceToViewScale);
        QList<QGraphicsItem*> childItems = item->childItems();
        for (QGraphicsItem* ci : qAsConst(childItems)) {
            if (auto gbi = dynamic_cast<GraphicsRectangularBranchItem*>(ci)) {
                stack.push(gbi);
            }
        }
    }
}

double TreeViewerUI::getAverageBranchDistance() const {
    double sumOfDistances = 0;
    int countOfBranches = 0;
    QList<QGraphicsItem*> updatingItems = scene()->items();
    for (QGraphicsItem* graphItem : qAsConst(updatingItems)) {
        if (auto branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem)) {
            sumOfDistances += qAbs(branchItem->getDist());
            countOfBranches++;
        }
    }
    return sumOfDistances / countOfBranches;
}

void TreeViewerUI::updateActionsState() {
    int widthCoeff = getOptionValue(WIDTH_COEF).toInt();
    treeViewer->zoomToSelectionAction->setEnabled(zoomLevel < MAXIMUM_ZOOM * qMax(widthCoeff * SIZE_COEF, 1.0));
    treeViewer->zoomOutAction->setEnabled(zoomLevel > MINIMUM_ZOOM);

    if (isSelectedCollapsed()) {
        treeViewer->collapseAction->setText(QObject::tr("Expand"));
        treeViewer->collapseAction->setIcon(QIcon(":/core/images/expand_tree.png"));
    } else {
        treeViewer->collapseAction->setText(QObject::tr("Collapse"));
        treeViewer->collapseAction->setIcon(QIcon(":/core/images/collapse_tree.png"));
    }

    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();

    bool thereIsSelection = !updatingItems.isEmpty();
    bool rootIsSelected = root->isSelected();
    treeViewer->collapseAction->setEnabled(thereIsSelection && !rootIsSelected);

    bool treeIsRooted = getTreeLayout() != UNROOTED_LAYOUT;
    bool treeIsCircular = getTreeLayout() == CIRCULAR_LAYOUT;
    treeViewer->swapAction->setEnabled(thereIsSelection && treeIsRooted && (!treeIsCircular || !isOnlyLeafSelected()));
    treeViewer->rerootAction->setEnabled(thereIsSelection && !rootIsSelected && treeIsRooted);
}

void TreeViewerUI::updateLayout() {
    TreeLayout tmpL = getTreeLayout();
    setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
    switch (tmpL) {
        case CIRCULAR_LAYOUT:
            changeTreeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            changeTreeLayout(UNROOTED_LAYOUT);
            break;
        case RECTANGULAR_LAYOUT:
            // here to please compiler
            break;
    }
}

void TreeViewerUI::updateLabelsAlignment() {
    bool on = getOptionValue(ALIGN_LABELS).toBool();
    QStack<GraphicsBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }

    if (!getOptionValue(SHOW_LABELS).toBool()) {
        return;
    }

    qreal sceneRightPos = scene()->sceneRect().right();
    QList<GraphicsBranchItem*> branchItems;
    qreal labelsShift = 0;
    while (!stack.empty()) {
        GraphicsBranchItem* item = stack.pop();
        QGraphicsSimpleTextItem* nameText = item->getNameTextItem();
        if (nameText == nullptr) {
            foreach (QGraphicsItem* citem, item->childItems()) {
                GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(citem);
                if (gbi != nullptr) {
                    stack.push(gbi);
                }
            }
        } else {
            branchItems.append(item);
            qreal newWidth = 0;
            if (on) {
                QRectF textRect = nameText->sceneBoundingRect();
                qreal textRightPos = textRect.right();
                if (nameText->flags().testFlag(QGraphicsItem::ItemIgnoresTransformations)) {
                    QRectF transformedRect = transform().inverted().mapRect(textRect);
                    textRect.setWidth(transformedRect.width());
                    textRightPos = textRect.right();
                }
                newWidth = sceneRightPos - (textRightPos + GraphicsBranchItem::TEXT_SPACING);
                labelsShift = qMin(newWidth, labelsShift);
            }
            item->setWidth(newWidth);
        }
    }
    if (labelsShift < 0) {
        foreach (GraphicsBranchItem* curItem, branchItems) {
            curItem->setWidth(curItem->getWidth() - labelsShift);
        }
    }
    updateRect();
}

bool TreeViewerUI::isOnlyLeafSelected() const {
    int selectedItems = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != nullptr && buttonItem->isNodeSelected()) {
            selectedItems++;
        }
    }
    return selectedItems == 2;
}

GraphicsBranchItem* TreeViewerUI::getRoot() const {
    return root;
}

}  // namespace U2
