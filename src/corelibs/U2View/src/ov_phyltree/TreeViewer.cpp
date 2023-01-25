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

#include "TreeViewer.h"

#include <QBuffer>
#include <QClipboard>
#include <QFileInfo>
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
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OrderedToolbar.h>

#include "./item/TvBranchItem.h"
#include "./item/TvNodeItem.h"
#include "./item/TvRectangularBranchItem.h"
#include "./item/TvTextItem.h"
#include "./layout/TvCircularLayoutAlgorithm.h"
#include "./layout/TvRectangularLayoutAlgorithm.h"
#include "./layout/TvUnrootedLayoutAlgorithm.h"
#include "TreeViewerFactory.h"
#include "TreeViewerState.h"
#include "TreeViewerTasks.h"
#include "TreeViewerUtils.h"
#include "phyltree/BranchSettingsDialog.h"
#include "phyltree/TextSettingsDialog.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {

TreeViewer::TreeViewer(const QString& viewName, PhyTreeObject* _phyObject)
    : GObjectView(TreeViewerFactory::ID, viewName), phyObject(_phyObject) {
    GCOUNTER(cvar, "PhylTreeViewer");

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
    ui->updateFixedSizeItemScales();
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

    // Show Node Labels.
    showNodeLabelsAction = new QAction(tr("Show Node Labels"), ui);
    showNodeLabelsAction->setObjectName("showNodeLabelsAction");

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
    zoomInAction = new QAction(QIcon(":core/images/zoom_in.png"), tr("Zoom In"), ui);
    zoomInAction->setObjectName("zoomInTreeViewerAction");
    zoomOutAction = new QAction(QIcon(":core/images/zoom_out.png"), tr("Zoom Out"), ui);
    zoomOutAction->setObjectName("zoomOutTreeViewerAction");
    resetZoomAction = new QAction(QIcon(":core/images/zoom_whole.png"), tr("Reset Zoom"), ui);
    resetZoomAction->setObjectName("resetZoomTreeViewerAction");

    // Print Tree
    printAction = new QAction(QIcon(":/core/images/printer.png"), tr("Print Tree..."), ui);

    copyWholeTreeImageToClipboardAction = new QAction(tr("Copy to clipboard"));
    copyWholeTreeImageToClipboardAction->setObjectName("copyWholeTreeImageToClipboardAction");

    saveVisibleViewToFileAction = new QAction(tr("Save visible area to file..."), ui);
    saveVisibleViewToFileAction->setObjectName("saveVisibleViewToFileAction");

    saveWholeTreeToSvgAction = new QAction(tr("Save whole tree as SVG..."), ui);
    saveWholeTreeToSvgAction->setObjectName("saveWholeTreeToSvgAction");
}

void TreeViewer::setupLayoutSettingsMenu(QMenu* m) {
    m->addActions(layoutActionGroup->actions());
}

void TreeViewer::setupShowLabelsMenu(QMenu* m) const {
    m->addAction(nameLabelsAction);
    m->addAction(distanceLabelsAction);
}

void TreeViewer::setupExportTreeImageMenu(QMenu* m) const {
    m->addAction(copyWholeTreeImageToClipboardAction);
    m->addSeparator();
    m->addAction(saveVisibleViewToFileAction);
    m->addAction(saveWholeTreeToSvgAction);
}

void TreeViewer::buildStaticToolbar(QToolBar* tb) {
    // Layout
    auto layoutButton = new QToolButton(tb);
    auto layoutMenu = new QMenu(tr("Layout"), ui);
    setupLayoutSettingsMenu(layoutMenu);
    layoutButton->setDefaultAction(layoutMenu->menuAction());
    layoutButton->setPopupMode(QToolButton::InstantPopup);
    layoutButton->setIcon(QIcon(":core/images/tree_layout.png"));
    layoutButton->setObjectName("Layout");
    tb->addWidget(layoutButton);

    // Labels and Text Settings
    tb->addSeparator();
    auto showLabelsButton = new QToolButton();
    auto showLabelsMenu = new QMenu(tr("Show Labels"), ui);
    showLabelsButton->setObjectName("Show Labels");
    setupShowLabelsMenu(showLabelsMenu);
    showLabelsButton->setDefaultAction(showLabelsMenu->menuAction());
    showLabelsButton->setPopupMode(QToolButton::InstantPopup);
    showLabelsButton->setIcon(QIcon(":/core/images/text_ab.png"));
    tb->addWidget(showLabelsButton);

    tb->addAction(textSettingsAction);

    // Print and Capture
    tb->addSeparator();

    auto treeImageActionsButton = new QToolButton();
    treeImageActionsButton->setObjectName("treeImageActionsButton");
    auto exportTreeImageButtonMenu = new QMenu(tr("Tree image"), ui);
    setupExportTreeImageMenu(exportTreeImageButtonMenu);
    exportTreeImageButtonMenu->menuAction()->setObjectName("treeImageActionsButtonMenuAction");
    treeImageActionsButton->setDefaultAction(exportTreeImageButtonMenu->menuAction());
    treeImageActionsButton->setPopupMode(QToolButton::InstantPopup);
    treeImageActionsButton->setIcon(QIcon(":/core/images/cam2.png"));

    tb->addWidget(treeImageActionsButton);
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
    tb->addAction(zoomInAction);
    tb->addAction(zoomOutAction);
    tb->addAction(resetZoomAction);
}

void TreeViewer::buildMSAEditorStaticToolbar(QToolBar* tb) {
    buildStaticToolbar(tb);
    tb->removeAction(zoomInAction);
    tb->removeAction(zoomOutAction);
    tb->removeAction(resetZoomAction);
}

void TreeViewer::buildMenu(QMenu* m, const QString& type) {
    if (type != GObjectViewMenuType::STATIC) {
        GObjectView::buildMenu(m, type);
        return;
    }
    // Tree Settings
    m->addAction(treeSettingsAction);

    // Layout
    auto layoutMenu = new QMenu(tr("Layout"), ui);
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

    auto labelsMenu = new QMenu(tr("Show Labels"), ui);
    labelsMenu->menuAction()->setObjectName("show_labels_action");
    setupShowLabelsMenu(labelsMenu);
    labelsMenu->setIcon(QIcon(":/core/images/text_ab.png"));
    m->addMenu(labelsMenu);

    m->addAction(textSettingsAction);

    m->addAction(alignTreeLabelsAction);
    // Zooming
    m->addSeparator();
    m->addAction(zoomInAction);
    m->addAction(zoomOutAction);
    m->addAction(resetZoomAction);

    // Print and Capture
    m->addSeparator();
    m->addAction(printAction);

    auto treeImageActionsSubmenu = new QMenu(tr("Tree image"), ui);
    treeImageActionsSubmenu->menuAction()->setObjectName("treeImageActionsSubmenu");
    treeImageActionsSubmenu->setIcon(QIcon(":/core/images/cam2.png"));
    setupExportTreeImageMenu(treeImageActionsSubmenu);
    m->addMenu(treeImageActionsSubmenu);

    m->addSeparator();

    GObjectView::buildMenu(m, type);
    GUIUtils::disableEmptySubmenus(m);
}

QWidget* TreeViewer::createWidget() {
    SAFE_POINT(ui == nullptr, "createWidget: UI is not null", ui);
    ui = new TreeViewerUI(this);

    optionsPanel = new OptionsPanel(this);
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();

    QList<OPFactoryFilterVisitorInterface*> filters;
    filters.append(new OPFactoryFilterVisitor(ObjViewType_PhylogeneticTree));

    QList<OPWidgetFactory*> opWidgetFactoriesForSeqView = opWidgetFactoryRegistry->getRegisteredFactories(filters);
    for (OPWidgetFactory* factory : qAsConst(opWidgetFactoriesForSeqView)) {
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

/** Zoom level change per single clickZoomInButton/clickZoomOutButton operation. */
static constexpr double ZOOM_LEVEL_STEP = 1.2;

/** Minimum zoom level: 1/4 of the fit-to-view size. */
static constexpr double MINIMUM_ZOOM_LEVEL = 0.25;

/**
 * Maximum zoom level: 100x of the fit-to-view size.
 * TODO: a static value may be not enough for very big trees: make this value dynamic.
 */
static const double MAXIMUM_ZOOM_LEVEL = 100.0;

/** Margins around the whole tree on the scene. On-screen pixels. */
static constexpr int TREE_MARGINS = 10;

static constexpr double SIZE_COEF = 0.1;

static const QString SETTINGS_PATH = "tree_viewer";

static QHash<TreeViewOption, QString> createTreeOptionsSettingNameMap() {
#define INIT_OPTION_NAME(option) map[option] = QString(#option).toLower()
    QHash<TreeViewOption, QString> map;
    INIT_OPTION_NAME(BRANCHES_TRANSFORMATION_TYPE);
    INIT_OPTION_NAME(TREE_LAYOUT);
    INIT_OPTION_NAME(BREADTH_SCALE_ADJUSTMENT_PERCENT);
    INIT_OPTION_NAME(BRANCH_CURVATURE);
    INIT_OPTION_NAME(LABEL_COLOR);
    INIT_OPTION_NAME(LABEL_FONT_FAMILY);
    INIT_OPTION_NAME(LABEL_FONT_SIZE);
    INIT_OPTION_NAME(LABEL_FONT_BOLD);
    INIT_OPTION_NAME(LABEL_FONT_ITALIC);
    INIT_OPTION_NAME(LABEL_FONT_UNDERLINE);
    INIT_OPTION_NAME(BRANCH_COLOR);
    INIT_OPTION_NAME(BRANCH_THICKNESS);
    INIT_OPTION_NAME(SHOW_NODE_SHAPE);
    INIT_OPTION_NAME(NODE_COLOR);
    INIT_OPTION_NAME(NODE_RADIUS);
    INIT_OPTION_NAME(SHOW_BRANCH_DISTANCE_LABELS);
    INIT_OPTION_NAME(SHOW_INNER_NODE_LABELS);
    INIT_OPTION_NAME(SHOW_LEAF_NODE_LABELS);
    INIT_OPTION_NAME(ALIGN_LEAF_NODE_LABELS);
    INIT_OPTION_NAME(SCALEBAR_RANGE);
    INIT_OPTION_NAME(SCALEBAR_FONT_SIZE);
    INIT_OPTION_NAME(SCALEBAR_LINE_WIDTH);
    return map;
}

static const QHash<TreeViewOption, QString> treeOptionSettingNames = createTreeOptionsSettingNameMap();

/** Returns serialized name of the tree option used to store settings in file. */
static QString getTreeOptionSettingName(const TreeViewOption& option) {
    QString name = treeOptionSettingNames.value(option);
    SAFE_POINT(!name.isEmpty(), "Unsupported option: " + QString::number(option), "");
    return name;
}

static OptionsMap createDefaultTreeOptionsSettings() {
    OptionsMap settings;

    settings[TREE_LAYOUT] = RECTANGULAR_LAYOUT;
    settings[BRANCHES_TRANSFORMATION_TYPE] = DEFAULT;
    settings[SCALEBAR_RANGE] = 30.0;
    settings[SCALEBAR_FONT_SIZE] = 8;
    settings[SCALEBAR_LINE_WIDTH] = 1;
    settings[LABEL_COLOR] = QColor(Qt::darkGray);
    settings[LABEL_FONT_FAMILY] = "";  // System default.
    settings[LABEL_FONT_SIZE] = 8;
    settings[LABEL_FONT_BOLD] = false;
    settings[LABEL_FONT_ITALIC] = false;
    settings[LABEL_FONT_UNDERLINE] = false;
    settings[SHOW_LEAF_NODE_LABELS] = true;
    settings[SHOW_BRANCH_DISTANCE_LABELS] = true;
    settings[SHOW_INNER_NODE_LABELS] = false;
    settings[ALIGN_LEAF_NODE_LABELS] = false;
    settings[BRANCH_COLOR] = QColor(0, 0, 0);
    settings[BRANCH_THICKNESS] = 1;
    settings[BREADTH_SCALE_ADJUSTMENT_PERCENT] = 100;
    settings[BRANCH_CURVATURE] = 0;
    settings[SHOW_NODE_SHAPE] = false;
    // TODO: these 2 options are not shown and not used. Make them used again & use correct defaults.
    settings[NODE_RADIUS] = 2;
    settings[NODE_COLOR] = QColor(0, 0, 0);

    for (int i = 0; i < OPTION_ENUM_END; i++) {
        auto option = static_cast<TreeViewOption>(i);
        SAFE_POINT(settings.contains(option), "Not all options have been initialized", settings);
    }
    return settings;
}

static const OptionsMap defaultSettings = createDefaultTreeOptionsSettings();

/** Stores the given tree setting as default into UGENE's settings file. */
static void storeOptionValueInAppSettings(const TreeViewOption& option, const QVariant& value) {
    QString settingName = getTreeOptionSettingName(option);
    if (value != defaultSettings.value(option)) {
        AppContext::getSettings()->setValue(SETTINGS_PATH + "/" + settingName, value);
    } else {
        AppContext::getSettings()->remove(SETTINGS_PATH + "/" + settingName);
    }
}

TreeViewerUI::TreeViewerUI(TreeViewer* _treeViewer)
    : phyObject(_treeViewer->getPhyObject()),
      treeViewer(_treeViewer) {
    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::PHYLOGENETIC_TREE).icon);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setScene(new QGraphicsScene());

    initializeSettings();
    addLegend();
    rebuildTreeLayout();
    updateDistanceToViewScale();

    updateRect();

    treeViewer->createActions();
    connect(treeViewer->nameLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_showNameLabelsTriggered(bool)));
    connect(treeViewer->distanceLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_showDistanceLabelsTriggered(bool)));
    connect(treeViewer->printAction, SIGNAL(triggered()), SLOT(sl_printTriggered()));
    connect(treeViewer->copyWholeTreeImageToClipboardAction, &QAction::triggered, this, &TreeViewerUI::copyWholeTreeImageToClipboard);
    connect(treeViewer->saveVisibleViewToFileAction, &QAction::triggered, this, &TreeViewerUI::saveVisibleViewToFile);
    connect(treeViewer->saveWholeTreeToSvgAction, &QAction::triggered, this, &TreeViewerUI::saveWholeTreeToSvg);
    connect(treeViewer->alignTreeLabelsAction, SIGNAL(triggered(bool)), SLOT(sl_contTriggered(bool)));
    connect(treeViewer->rectangularLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_rectangularLayoutTriggered()));
    connect(treeViewer->circularLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_circularLayoutTriggered()));
    connect(treeViewer->unrootedLayoutAction, SIGNAL(triggered(bool)), SLOT(sl_unrootedLayoutTriggered()));
    connect(treeViewer->textSettingsAction, SIGNAL(triggered()), SLOT(sl_textSettingsTriggered()));
    connect(treeViewer->treeSettingsAction, SIGNAL(triggered()), SLOT(sl_treeSettingsTriggered()));
    connect(treeViewer->zoomInAction, &QAction::triggered, this, &TreeViewerUI::zoomIn);
    connect(treeViewer->zoomOutAction, &QAction::triggered, this, &TreeViewerUI::zoomOut);
    connect(treeViewer->resetZoomAction, &QAction::triggered, this, &TreeViewerUI::resetZoom);
    connect(treeViewer->branchesSettingsAction, SIGNAL(triggered()), SLOT(sl_setSettingsTriggered()));
    connect(treeViewer->collapseAction, SIGNAL(triggered()), SLOT(sl_collapseTriggered()));
    connect(treeViewer->rerootAction, SIGNAL(triggered()), SLOT(sl_rerootTriggered()));
    connect(treeViewer->swapAction, SIGNAL(triggered()), SLOT(sl_swapTriggered()));

    buttonPopup = new QMenu(this);

    // chrootAction->setEnabled(false); //not implemented yet

    buttonPopup->addAction(treeViewer->zoomInAction);
    buttonPopup->addAction(treeViewer->zoomOutAction);
    buttonPopup->addAction(treeViewer->resetZoomAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(treeViewer->swapAction);
    treeViewer->swapAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->rerootAction);
    treeViewer->rerootAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->collapseAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(treeViewer->branchesSettingsAction);

    auto treeImageActionsMenu = new QMenu(tr("Tree image"), this);
    treeImageActionsMenu->menuAction()->setObjectName("treeImageActionsMenu");
    treeImageActionsMenu->addAction(treeViewer->copyWholeTreeImageToClipboardAction);
    treeImageActionsMenu->addSeparator();
    treeImageActionsMenu->addAction(treeViewer->saveVisibleViewToFileAction);
    treeImageActionsMenu->addAction(treeViewer->saveWholeTreeToSvgAction);
    treeImageActionsMenu->setIcon(QIcon(":/core/images/cam2.png"));
    buttonPopup->addMenu(treeImageActionsMenu);

    updateActionsState();
    setObjectName("treeView");
    updateRectLayoutBranches();
    connect(root, &TvBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
}

TreeViewerUI::~TreeViewerUI() {
    delete scene();
}

void TreeViewerUI::initializeSettings() {
    QList<TreeViewOption> optionKeys = treeOptionSettingNames.keys();
    for (auto option : qAsConst(optionKeys)) {
        QString settingName = getTreeOptionSettingName(option);
        settings[option] = AppContext::getSettings()->getValue(SETTINGS_PATH + "/" + settingName, defaultSettings[option]);
    }
    // Tree viewer can't be started with a non-rectangular layout today.
    settings[TREE_LAYOUT] = RECTANGULAR_LAYOUT;
}

/** Returns true if the option can be applied to the selection only. */
static bool isSelectionScopeOption(const U2::TreeViewOption& option) {
    switch (option) {
        case BRANCH_COLOR:
        case BRANCH_THICKNESS:
        case LABEL_COLOR:
        case LABEL_FONT_BOLD:
        case LABEL_FONT_ITALIC:
        case LABEL_FONT_SIZE:
        case LABEL_FONT_FAMILY:
        case LABEL_FONT_UNDERLINE:
            return true;
        default:
            return false;
    }
}

QVariant TreeViewerUI::getOption(const TreeViewOption& option) const {
    if (isSelectionScopeOption(option) && selectionSettingsDelta.contains(option)) {
        return selectionSettingsDelta[option];
    }
    SAFE_POINT(settings.contains(option), "Unsupported option", {});
    return settings[option];
}

void TreeViewerUI::saveOptionToSettings(const TreeViewOption& option, const QVariant& value) {
    if (isSelectionScopeOption(option) && hasPartialSelection()) {
        CHECK(selectionSettingsDelta[option] != value, );
        selectionSettingsDelta[option] = value;
    } else {
        CHECK(settings[option] != value, );
        settings[option] = value;
        storeOptionValueInAppSettings(option, value);
    }
    emit si_optionChanged(option, value);
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
    return static_cast<TreeLayout>(getOption(TREE_LAYOUT).toInt());
}

bool TreeViewerUI::hasPartialSelection() const {
    QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
    return !selectedItems.isEmpty() && !selectedItems.contains(root);
}

void TreeViewerUI::updateOptions(const U2::OptionsMap& changedOptions) {
    QList<TreeViewOption> keys = changedOptions.keys();
    for (const TreeViewOption& curOption : qAsConst(keys)) {
        updateOption(curOption, changedOptions[curOption]);
    }
}

void TreeViewerUI::updateOption(const TreeViewOption& option, const QVariant& newValue) {
    CHECK(getOption(option) != newValue, );
    if (option != TREE_LAYOUT) {  // TREE_LAYOUT setting is updated as a part of 'setTreeLayout' call below.
        saveOptionToSettings(option, newValue);
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
        case BREADTH_SCALE_ADJUSTMENT_PERCENT:
        case BRANCH_CURVATURE:
            updateScene(true);
            break;
        case LABEL_COLOR:
        case LABEL_FONT_FAMILY:
        case LABEL_FONT_SIZE:
        case LABEL_FONT_BOLD:
        case LABEL_FONT_ITALIC:
        case LABEL_FONT_UNDERLINE:
            updateTextOptionOnSelectedItems();
            break;
        case BRANCH_COLOR:
        case BRANCH_THICKNESS:
        case NODE_COLOR:
        case NODE_RADIUS:
            updateTreeSettingsOnSelectedItems();
            break;
        case SHOW_LEAF_NODE_LABELS:
            changeNamesDisplay();
            treeViewer->nameLabelsAction->setChecked(newValue.toBool());
            break;
        case SHOW_BRANCH_DISTANCE_LABELS:
            showLabels(LabelType_Distance);
            treeViewer->distanceLabelsAction->setChecked(newValue.toBool());
            break;
        case SHOW_INNER_NODE_LABELS:
        case SHOW_NODE_SHAPE:
            updateTreeSettingsOnAllNodes();
            break;
        case ALIGN_LEAF_NODE_LABELS:
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

OptionsMap TreeViewerUI::getSelectionSettings() const {
    OptionsMap effectiveSettings = settings;
    effectiveSettings.insert(selectionSettingsDelta);
    return effectiveSettings;
}

void TreeViewerUI::sl_branchSettings() {
    QObjectScopedPointer<BranchSettingsDialog> dialog = new BranchSettingsDialog(this, getSelectionSettings());
    dialog->exec();
    CHECK(!dialog.isNull() && dialog->result() == QDialog::Accepted, );
    updateOptions(dialog->getSettings());
}

void TreeViewerUI::updateTreeSettingsOnSelectedItems() {
    OptionsMap selectionSettings = getSelectionSettings();

    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();
    if (updatingItems.isEmpty()) {
        updatingItems = items();

        if (auto legendLineItem = dynamic_cast<QGraphicsLineItem*>(legendItem)) {
            QPen legendPen;
            QColor branchColor = qvariant_cast<QColor>(getOption(BRANCH_COLOR));
            legendPen.setColor(branchColor);
            legendLineItem->setPen(legendPen);
        }
    }

    for (QGraphicsItem* item : qAsConst(updatingItems)) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(item)) {
            branchItem->updateSettings(selectionSettings);
            if (branchItem->correspondingRectangularBranchItem != nullptr) {
                branchItem->correspondingRectangularBranchItem->updateSettings(selectionSettings);
            }
        } else if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            nodeItem->updateSettings(selectionSettings);
        }
    }
    scene()->update();
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

void TreeViewerUI::updateTextOptionOnSelectedItems() {
    OptionsMap selectionSettings = getSelectionSettings();
    QSet<QGraphicsItem*> itemsToUpdate = scene()->selectedItems().toSet();
    if (itemsToUpdate.isEmpty()) {
        itemsToUpdate = scene()->items().toSet();
    } else {
        QSet<QGraphicsItem*> rootItems = itemsToUpdate;
        for (auto item : qAsConst(rootItems)) {
            itemsToUpdate += getAllLevelChildItems(item);
        }
    }

    for (auto item : qAsConst(itemsToUpdate)) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(item)) {
            branchItem->updateSettings(selectionSettings);
            if (branchItem->correspondingRectangularBranchItem != nullptr) {
                branchItem->correspondingRectangularBranchItem->updateSettings(selectionSettings);
            }
        } else if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            nodeItem->updateSettings(selectionSettings);
        } else if (auto legendText = dynamic_cast<TvTextItem*>(item)) {
            legendText->setBrush(qvariant_cast<QColor>(selectionSettings[LABEL_COLOR]));
        }
    }
}

void TreeViewerUI::updateRectLayoutBranches() {
    auto type = static_cast<TreeType>(getOption(BRANCHES_TRANSFORMATION_TYPE).toInt());
    legendItem->setVisible(type == PHYLOGRAM);

    updateStepsToLeafOnBranches();
    double averageBranchDistance = getAverageBranchDistance();
    double breadthScaleAdjustment = getOption(BREADTH_SCALE_ADJUSTMENT_PERCENT).toDouble() / 100;
    double branchCurvature = getOption(BRANCH_CURVATURE).toDouble();

    QStack<TvBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        TvBranchItem* item = stack.pop();
        const QList<QGraphicsItem*>& childItems = item->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<TvBranchItem*>(childItem)) {
                stack.push(childBranchItem);
            }
        }

        if (item == rectRoot) {
            continue;
        }

        auto rectItem = dynamic_cast<TvRectangularBranchItem*>(item);
        SAFE_POINT(rectItem != nullptr, "Not a rect root!", );
        rectItem->setBreathScaleAdjustment(breadthScaleAdjustment);
        rectItem->setCurvature(branchCurvature);

        double coef = qMax(1.0, SIZE_COEF);

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

    showLabels(LabelType_Distance);
    showLabels(LabelType_SequenceName);
    bool alignLabels = getOption(ALIGN_LEAF_NODE_LABELS).toBool();
    if (alignLabels) {
        updateLabelsAlignment();
    }

    if (fitSceneToView) {
        fitIntoView();
    }
}

void TreeViewerUI::updateStepsToLeafOnBranches() {
    QStack<TvBranchItem*> stack;
    QQueue<TvBranchItem*> childQueue;
    // Assign 'maxStepsToLeaf' value.
    stack.push(rectRoot);
    while (!stack.empty()) {
        TvBranchItem* branchItem = stack.pop();
        branchItem->maxStepsToLeaf = 0;
        branchItem->maxStepsToLeafParentDelta = 0;
        int childBranchCount = 0;
        const QList<QGraphicsItem*>& childItems = branchItem->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<TvBranchItem*>(childItem)) {
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
        TvBranchItem* childBranchItem = childQueue.dequeue();
        if (auto parentBranchItem = dynamic_cast<TvBranchItem*>(childBranchItem->parentItem())) {
            parentBranchItem->maxStepsToLeaf = qMax(parentBranchItem->maxStepsToLeaf, childBranchItem->maxStepsToLeaf + 1);
            childQueue.enqueue(parentBranchItem);
        }
    }
    // Assign 'maxStepsToLeafParentDelta' value.
    SAFE_POINT(stack.isEmpty(), "Stack must be empty", );
    stack.push(rectRoot);
    while (!stack.empty()) {
        TvBranchItem* branchItem = stack.pop();
        const QList<QGraphicsItem*>& childItems = branchItem->childItems();
        for (QGraphicsItem* childItem : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<TvBranchItem*>(childItem)) {
                stack.push(childBranchItem);
                childBranchItem->maxStepsToLeafParentDelta = branchItem->maxStepsToLeaf - childBranchItem->maxStepsToLeaf;
                SAFE_POINT(childBranchItem->maxStepsToLeafParentDelta > 0, "Invalid maxStepsToLeafParentDelta", );
            }
        }
    }
}

QVariantMap TreeViewerUI::getSettingsState() const {
    QString branchColor("branch_color");
    QString branchThickness("branch_thickness");
    QVariantMap m;

    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
            OptionsMap branchSettings = branchItem->getSettings();
            m[branchColor] = qvariant_cast<QColor>(branchSettings[BRANCH_COLOR]);
            m[branchThickness + i] = branchSettings[BRANCH_THICKNESS].toInt();
            i++;
        }
    }

    return m;
}

void TreeViewerUI::setSettingsState(const QVariantMap& m) {
    QString branchColor("branch_color");
    QString branchThickness("branch_thickness");
    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
            OptionsMap branchSettings = branchItem->getSettings();

            QVariant vColor = m[branchColor + i];
            if (vColor.type() == QVariant::Color) {
                branchSettings[BRANCH_COLOR] = vColor.value<QColor>();
            }

            QVariant vThickness = m[branchThickness + i];
            if (vThickness.type() == QVariant::Int) {
                branchSettings[BRANCH_THICKNESS] = vThickness.toInt();
            }

            branchItem->updateSettings(branchSettings);
            i++;
        }
    }
}

void TreeViewerUI::addLegend() {
    double d = getOption(SCALEBAR_RANGE).toDouble();
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i)
        ;
    if (str[i] == '.') {
        --i;
    }
    str.truncate(i + 1);

    legendItem = new QGraphicsLineItem(0, 0, 0, 0);
    scalebarTextItem = new TvTextItem(legendItem, "");
    updateLegend();

    scene()->addItem(legendItem);
}

void TreeViewerUI::updateLegend() {
    double coef = qMax(1.0, SIZE_COEF);
    double WIDTH = getOption(SCALEBAR_RANGE).toDouble() * coef * distanceToViewScale;

    double d = getOption(SCALEBAR_RANGE).toDouble();
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i)
        ;
    if (str[i] == '.')
        --i;
    str.truncate(i + 1);

    scalebarTextItem->setText(str);

    QFont curFont = TreeViewerUtils::getFontFromSettings(settings);
    curFont.setPointSize(getOption(SCALEBAR_FONT_SIZE).toInt());
    scalebarTextItem->setFont(curFont);

    QPen curPen = legendItem->pen();
    curPen.setWidth(getOption(SCALEBAR_LINE_WIDTH).toInt());
    legendItem->setPen(curPen);

    legendItem->setLine(0, 0, WIDTH, 0);

    QRectF rect = scalebarTextItem->boundingRect();
    scalebarTextItem->setPos(0.5 * (WIDTH - rect.width()), -rect.height());
}

void TreeViewerUI::wheelEvent(QWheelEvent* we) {
    double newZoomLevel = zoomLevel * pow(ZOOM_LEVEL_STEP, we->delta() / 120.0);
    setZoomLevel(newZoomLevel);
    we->accept();
}

void TreeViewerUI::setZoomLevel(double newZoomLevel) {
    CHECK(newZoomLevel >= MINIMUM_ZOOM_LEVEL && newZoomLevel <= MAXIMUM_ZOOM_LEVEL, );

    uiLog.trace("New zoom level: " + QString::number(newZoomLevel));
    double scaleChange = newZoomLevel / zoomLevel;
    zoomLevel = newZoomLevel;
    scale(scaleChange, scaleChange);
    updateFixedSizeItemScales();
    updateActionsState();
}

QList<QGraphicsItem*> TreeViewerUI::getFixedSizeItems() const {
    QList<QGraphicsItem*> result;
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            result.append(nodeItem);
        }
    }
    return result;
}

void TreeViewerUI::updateFixedSizeItemScales() {
    double sceneToScreenScale = qMin(transform().m11(), transform().m22());
    QList<QGraphicsItem*> fixedSizeItems = getFixedSizeItems();
    for (QGraphicsItem* item : qAsConst(fixedSizeItems)) {
        item->setScale(1 / sceneToScreenScale);  // Scale back to screen coordinates.
        if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            // Scale back node labels. Only node circle must stay fixed size.
            if (nodeItem->labelItem != nullptr) {
                nodeItem->labelItem->setScale(sceneToScreenScale);
            }
        }
    }
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
        root->setSelectedRecursively(false);  // Clear selection on any right button click with no shift.
    }
    updateActionsState();
    updateSettingsOnSelectionChange();
    e->accept();
}

void TreeViewerUI::resizeEvent(QResizeEvent* e) {
    QGraphicsView::resizeEvent(e);
    updateScene(true);
}

void TreeViewerUI::fitIntoView() {
    // First hide all fixed size items, so they do not affect current size estimation.
    QList<QGraphicsItem*> fixedSizeItems = getFixedSizeItems();
    QSet<QGraphicsItem*> fixedSizeSelectedItems;
    for (QGraphicsItem* item : qAsConst(fixedSizeItems)) {
        if (item->isSelected()) {
            fixedSizeSelectedItems << item;  // Invisible item loosing isSelected flag, so keep it in a special collection and restore later.
        }
        item->setVisible(false);
    }

    // Set new scene rect with margins.
    updateRect();

    // Fit to screen scene rect. Preserve zoom.
    QRectF sceneRect = scene()->sceneRect();
    QRectF sceneRectWithZoomEffect = sceneRect;
    sceneRectWithZoomEffect.setWidth(sceneRect.width() / zoomLevel);
    sceneRectWithZoomEffect.setHeight(sceneRect.height() / zoomLevel);
    sceneRectWithZoomEffect.moveCenter(sceneRect.center());
    fitInView(sceneRectWithZoomEffect, Qt::KeepAspectRatio);

    // Re-scale fixed size items to preserve the size and make them visible.
    updateFixedSizeItemScales();
    for (QGraphicsItem* item : qAsConst(fixedSizeItems)) {
        item->setVisible(true);
        if (fixedSizeSelectedItems.contains(item)) {
            item->setSelected(true);
        }
    }

    // Re-apply margins with fixed size items visible.
    updateRect();
}

void TreeViewerUI::paint(QPainter& painter) {
    painter.setBrush(Qt::darkGray);
    scene()->render(&painter);
}

void TreeViewerUI::updateRect() {
    SAFE_POINT(root != nullptr, "Pointer to tree root is NULL", );
    QTransform viewTransform = transform();
    QRectF rect = root->visibleChildrenBoundingRect(viewTransform) | root->sceneBoundingRect();
    rect.setLeft(rect.left() - TREE_MARGINS);
    rect.setRight(rect.right() + TREE_MARGINS);
    rect.setTop(rect.top() - TREE_MARGINS);
    rect.setBottom(rect.bottom() + legendItem->childrenBoundingRect().height() + TREE_MARGINS);
    legendItem->setPos(0, rect.bottom() - TREE_MARGINS);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::sl_swapTriggered() {
    QList<QGraphicsItem*> graphItems = items();
    for (auto graphItem : qAsConst(graphItems)) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            PhyNode* phyNode = nodeItem->getPhyNode();
            SAFE_POINT(phyNode != nullptr, "Can't swap siblings of the root node with no phyNode!", );
            phyNode->invertOrderOrChildBranches();
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
    QList<QGraphicsItem*> childItems = items();
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            auto phyNode = nodeItem->getPhyNode();
            phyObject->rerootPhyTree(phyNode);
            break;
        }
    }
}

void TreeViewerUI::collapseSelected() {
    QList<QGraphicsItem*> childItems = items();
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            nodeItem->toggleCollapsedState();
            break;
        }
    }
}

void TreeViewerUI::updateSettingsOnSelectionChange() {
    OptionsMap newSelectionSettingsDelta;
    QList<QGraphicsItem*> childItems = items();
    TvBranchItem* branch = root;
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            branch = dynamic_cast<TvBranchItem*>(nodeItem->parentItem());
            break;
        }
    }
    newSelectionSettingsDelta[BRANCH_THICKNESS] = branch->getSettings()[BRANCH_THICKNESS];
    newSelectionSettingsDelta[BRANCH_COLOR] = branch->getSettings()[BRANCH_COLOR];

    QFont font;
    QColor color;
    if (branch->getDistanceTextItem() != nullptr) {
        font = branch->getDistanceTextItem()->font();
        color = branch->getDistanceTextItem()->brush().color();
    }
    bool isCustomFont = font != qvariant_cast<QFont>(settings[LABEL_FONT_FAMILY]);
    bool isCustomColor = color != qvariant_cast<QColor>(settings[LABEL_COLOR]);
    if (isCustomFont || isCustomColor) {
        newSelectionSettingsDelta[LABEL_FONT_FAMILY] = font.family();
        newSelectionSettingsDelta[LABEL_FONT_SIZE] = font.pointSize();
        newSelectionSettingsDelta[LABEL_FONT_BOLD] = font.bold();
        newSelectionSettingsDelta[LABEL_FONT_ITALIC] = font.italic();
        newSelectionSettingsDelta[LABEL_FONT_UNDERLINE] = font.underline();
        newSelectionSettingsDelta[LABEL_COLOR] = color;
    }
    // Remove settings that are the same as default.
    QList<TreeViewOption> newSelectionSettingsDeltaKeys = newSelectionSettingsDelta.keys();
    OptionsMap changedSettings;
    for (auto option : qAsConst(newSelectionSettingsDeltaKeys)) {
        QVariant value = newSelectionSettingsDelta[option];
        if (value == settings[option]) {
            newSelectionSettingsDelta.remove(option);
        }
        if (value != selectionSettingsDelta[option]) {
            changedSettings[option] = value;
        }
    }

    CHECK(!changedSettings.isEmpty(), )
    selectionSettingsDelta = newSelectionSettingsDelta;
    QList<TreeViewOption> changedSettingsKeys = changedSettings.keys();
    for (auto option : qAsConst(changedSettingsKeys)) {
        emit si_optionChanged(option, changedSettings[option]);
    }
}

bool TreeViewerUI::isSelectedCollapsed() {
    foreach (QGraphicsItem* graphItem, items()) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            return nodeItem->isCollapsed();
        }
    }
    return false;
}

void TreeViewerUI::sl_collapseTriggered() {
    collapseSelected();
}

void TreeViewerUI::copyWholeTreeImageToClipboard() {
    QRect rect = mapFromScene(sceneRect()).boundingRect();
    if (rect.width() > GUIUtils::MAX_SAFE_PIXMAP_WIDTH || rect.height() > GUIUtils::MAX_SAFE_PIXMAP_HEIGHT) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Image is too large. Please zoom out."));
        return;
    }
    QPixmap pixmap = viewport()->grab(rect);
    if (pixmap.isNull()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Failed to copy image."));
        return;
    }
    QApplication::clipboard()->setImage(pixmap.toImage());
}

void TreeViewerUI::saveVisibleViewToFile() {
    Document* doc = phyObject->getDocument();
    const GUrl& url = doc->getURL();
    const QString& fileName = url.baseFileName();

    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(viewport(), ExportImageDialog::PHYTreeView, fileName, ExportImageDialog::NoScaling, this);
    dialog->exec();
}

void TreeViewerUI::saveWholeTreeToSvg() {
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

    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QSvgGenerator generator;
    generator.setOutputDevice(&buffer);
    generator.setSize(rect.size());
    generator.setViewBox(rect);

    QPainter painter;
    painter.begin(&generator);
    paint(painter);
    painter.end();

    buffer.seek(0);  // move pointer to the buffer's start
    QString svgText(buffer.readAll());

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
    updateOption(ALIGN_LEAF_NODE_LABELS, on);
}

void TreeViewerUI::changeLabelsAlignment() {
    updateLabelsAlignment();

    TreeLayout curLayout = getTreeLayout();
    switch (curLayout) {
        case CIRCULAR_LAYOUT:
            changeTreeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            changeTreeLayout(UNROOTED_LAYOUT);
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
static void makeLayoutNotCollapsed(TvBranchItem* branch) {
    CHECK(branch != nullptr, );
    if (branch->isCollapsed()) {
        branch->toggleCollapsedState();
    }
    QList<QGraphicsItem*> childItems = branch->childItems();
    for (auto child : qAsConst(childItems)) {
        if (auto childBranch = dynamic_cast<TvBranchItem*>(child)) {
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
            bool degeneratedCase = distanceToViewScale <= TvRectangularBranchItem::DEFAULT_WIDTH;
            setNewTreeLayout(TvCircularLayoutAlgorithm::convert(rectRoot, degeneratedCase), newTreeLayout);
            break;
        }
        case UNROOTED_LAYOUT: {
            makeLayoutNotCollapsed(root);  // See comments for CIRCULAR_LAYOUT.
            makeLayoutNotCollapsed(rectRoot);
            setNewTreeLayout(TvUnrootedLayoutAlgorithm::convert(rectRoot), newTreeLayout);
            break;
        }
    }
}

void TreeViewerUI::rebuildTreeLayout() {
    auto newRectRoot = TvRectangularLayoutAlgorithm::buildTreeLayout(phyObject->getTree()->getRootNode());
    updateTreeSettingsOnAllNodes();
    CHECK_EXT(newRectRoot != nullptr, uiLog.error(tr("Failed to build tree layout.")), );
    CHECK(newRectRoot != nullptr, );
    rectRoot = newRectRoot;
    switch (getTreeLayout()) {
        case CIRCULAR_LAYOUT:
            changeTreeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            changeTreeLayout(UNROOTED_LAYOUT);
            break;
        case RECTANGULAR_LAYOUT:
            changeTreeLayout(RECTANGULAR_LAYOUT);
            break;
    }
}

void TreeViewerUI::sl_onBranchCollapsed(TvBranchItem*) {
    // In rectangular mode perform a complete re-layout of the tree, so there is no empty space left.
    // TODO: do the same in circular & unrooted layouts.
    CHECK(isRectangularLayoutMode(), );
    recalculateRectangularLayout();
    updateScene(false);
    updateFixedSizeItemScales();
    updateActionsState();
}

void TreeViewerUI::setNewTreeLayout(TvBranchItem* newRoot, const TreeLayout& treeLayout) {
    if (root != nullptr) {
        root->setSelectedRecursively(false);
        scene()->removeItem(root);
        disconnect(root, &TvBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
    }
    root = newRoot;
    connect(root, &TvBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
    scene()->addItem(root);

    saveOptionToSettings(TREE_LAYOUT, treeLayout);

    bool showNames = getOption(SHOW_LEAF_NODE_LABELS).toBool();
    bool showDistances = getOption(SHOW_BRANCH_DISTANCE_LABELS).toBool();

    // TODO: cleanup labels logic.
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
    updateTreeSettingsOnAllNodes();
    updateTreeSettingsOnSelectedItems();
    updateTextOptionOnSelectedItems();
    updateScene(true);
}

void TreeViewerUI::showLabels(LabelTypes labelTypes) {
    QStack<TvBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }
    maxNameWidth = 0.0;
    while (!stack.isEmpty()) {
        TvBranchItem* branchItem = stack.pop();
        if (labelTypes.testFlag(LabelType_SequenceName)) {
            if (branchItem->getNameTextItem() != nullptr) {
                branchItem->setVisible(getOption(SHOW_LEAF_NODE_LABELS).toBool());
                maxNameWidth = qMax(maxNameWidth, branchItem->getNameTextItem()->sceneBoundingRect().width());
            }
        }
        if (labelTypes.testFlag(LabelType_Distance)) {
            if (branchItem->getDistanceTextItem() != nullptr) {
                branchItem->getDistanceTextItem()->setVisible(getOption(SHOW_BRANCH_DISTANCE_LABELS).toBool());
            }
        }
        foreach (QGraphicsItem* item, branchItem->childItems()) {
            if (auto childBranchItem = dynamic_cast<TvBranchItem*>(item)) {
                stack.push(childBranchItem);
            }
        }
    }
}

void TreeViewerUI::sl_showNameLabelsTriggered(bool on) {
    updateOption(SHOW_LEAF_NODE_LABELS, on);
}

void TreeViewerUI::changeNamesDisplay() {
    bool showNames = getOption(SHOW_LEAF_NODE_LABELS).toBool();
    treeViewer->alignTreeLabelsAction->setEnabled(showNames);

    showLabels(LabelType_SequenceName);
    QRectF rect = sceneRect();
    rect.setWidth(rect.width() + (showNames ? 1 : -1) * maxNameWidth);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::updateTreeSettingsOnAllNodes() {
    const QList<QGraphicsItem*> itemList = scene()->items();
    for (QGraphicsItem* curItem : qAsConst(itemList)) {
        if (auto nodeItem = dynamic_cast<TvNodeItem*>(curItem)) {
            nodeItem->updateSettings(settings);
        }
    }
    scene()->update();
}

void TreeViewerUI::sl_showDistanceLabelsTriggered(bool on) {
    updateOption(SHOW_BRANCH_DISTANCE_LABELS, on);
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
    QObjectScopedPointer<TextSettingsDialog> dialog = new TextSettingsDialog(this, getSelectionSettings());
    dialog->exec();
    CHECK(!dialog.isNull() && dialog->result() == QDialog::Accepted, );

    updateOptions(dialog->getSettings());
    if (getOption(ALIGN_LEAF_NODE_LABELS).toBool()) {
        QStack<TvBranchItem*> stack;
        stack.push(root);
        if (root != rectRoot) {
            stack.push(rectRoot);
        }
        while (!stack.empty()) {
            TvBranchItem* item = stack.pop();
            if (item->getNameTextItem() == nullptr) {
                foreach (QGraphicsItem* childItem, item->childItems()) {
                    if (auto gbi = dynamic_cast<TvBranchItem*>(childItem)) {
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

void TreeViewerUI::sl_treeSettingsTriggered() {
    QObjectScopedPointer<TreeSettingsDialog> dialog = new TreeSettingsDialog(this, settings);
    dialog->exec();
    CHECK(!dialog.isNull() && dialog->result() == QDialog::Accepted, );
    updateOptions(dialog->getSettings());
}

void TreeViewerUI::zoomIn() {
    double newZoomLevel = zoomLevel * ZOOM_LEVEL_STEP;
    setZoomLevel(newZoomLevel);
}

void TreeViewerUI::zoomOut() {
    double newZoomLevel = zoomLevel / ZOOM_LEVEL_STEP;
    setZoomLevel(newZoomLevel);
}

void TreeViewerUI::resetZoom() {
    defaultZoom();
}

void TreeViewerUI::defaultZoom() {
    setZoomLevel(1);
}

void TreeViewerUI::recalculateRectangularLayout() {
    TvRectangularLayoutAlgorithm::recalculateTreeLayout(rectRoot, phyObject->getTree()->getRootNode());
    updateDistanceToViewScale();
}

void TreeViewerUI::updateDistanceToViewScale() {
    double minDistance = -2;
    double maxDistance = 0;

    QStack<TvRectangularBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        TvRectangularBranchItem* item = stack.pop();
        minDistance = minDistance == -2 ? item->getDist() : qMin(item->getDist(), minDistance);
        maxDistance = qMax(item->getDist(), maxDistance);
        QList<QGraphicsItem*> childItems = item->childItems();
        for (QGraphicsItem* ci : qAsConst(childItems)) {
            if (auto gbi = dynamic_cast<TvRectangularBranchItem*>(ci)) {
                stack.push(gbi);
            }
        }
    }
    if (minDistance == 0) {
        minDistance = TvRectangularBranchItem::EPSILON;
    }
    if (maxDistance == 0) {
        maxDistance = TvRectangularBranchItem::EPSILON;
    }
    double minDistScale = TvRectangularBranchItem::DEFAULT_WIDTH / minDistance;
    double maxDistScale = TvRectangularBranchItem::MAXIMUM_WIDTH / maxDistance;
    distanceToViewScale = qMin(minDistScale, maxDistScale);

    stack.push(rectRoot);
    while (!stack.empty()) {
        TvRectangularBranchItem* item = stack.pop();
        item->setWidth(item->getWidth() * distanceToViewScale);
        QList<QGraphicsItem*> childItems = item->childItems();
        for (QGraphicsItem* ci : qAsConst(childItems)) {
            if (auto gbi = dynamic_cast<TvRectangularBranchItem*>(ci)) {
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
        if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
            sumOfDistances += qAbs(branchItem->getDist());
            countOfBranches++;
        }
    }
    return sumOfDistances / countOfBranches;
}

void TreeViewerUI::updateActionsState() {
    treeViewer->zoomInAction->setEnabled(zoomLevel * ZOOM_LEVEL_STEP < MAXIMUM_ZOOM_LEVEL);
    treeViewer->zoomOutAction->setEnabled(zoomLevel / ZOOM_LEVEL_STEP > MINIMUM_ZOOM_LEVEL);

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
    treeViewer->swapAction->setEnabled(thereIsSelection &&
                                       treeIsRooted &&
                                       (!treeIsCircular || !isOnlyLeafSelected()) &&
                                       !root->isSelected());
    treeViewer->rerootAction->setEnabled(thereIsSelection && !rootIsSelected && treeIsRooted);
}

void TreeViewerUI::updateLayout() {
    TreeLayout tmpL = getTreeLayout();
    saveOptionToSettings(TREE_LAYOUT, RECTANGULAR_LAYOUT);
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
    bool on = getOption(ALIGN_LEAF_NODE_LABELS).toBool();
    QStack<TvBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }

    if (!getOption(SHOW_LEAF_NODE_LABELS).toBool()) {
        return;
    }

    qreal sceneRightPos = scene()->sceneRect().right();
    QList<TvBranchItem*> branchItems;
    qreal labelsShift = 0;
    while (!stack.empty()) {
        TvBranchItem* item = stack.pop();
        TvTextItem* nameText = item->getNameTextItem();
        if (nameText == nullptr) {
            foreach (QGraphicsItem* childItem, item->childItems()) {
                if (auto branchItem = dynamic_cast<TvBranchItem*>(childItem)) {
                    stack.push(branchItem);
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
                newWidth = sceneRightPos - (textRightPos + TvBranchItem::TEXT_SPACING);
                labelsShift = qMin(newWidth, labelsShift);
            }
            item->setWidth(newWidth);
        }
    }
    if (labelsShift < 0) {
        foreach (TvBranchItem* curItem, branchItems) {
            curItem->setWidth(curItem->getWidth() - labelsShift);
        }
    }
    updateRect();
}

bool TreeViewerUI::isOnlyLeafSelected() const {
    int selectedItems = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelected()) {
            selectedItems++;
        }
    }
    return selectedItems == 2;
}

TvBranchItem* TreeViewerUI::getRoot() const {
    return root;
}

}  // namespace U2
