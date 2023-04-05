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
#include <U2Core/GAutoDeleteList.h>
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
#include "phyltree/TextSettingsDialog.h"
#include "phyltree/TreeSettingsDialog.h"
namespace U2 {

TreeViewer::TreeViewer(const QString& viewName, PhyTreeObject* _phyObject, bool hasOptionsPanel)
    : GObjectViewController(TreeViewerFactory::ID, viewName), phyObject(_phyObject) {
    GCOUNTER(cvar, "PhylTreeViewer");
    if (hasOptionsPanel) {
        optionsPanelController = new OptionsPanelController(this);
        OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();
        GAutoDeleteList<OPFactoryFilterVisitorInterface> filters;
        filters.qlist.append(new OPFactoryFilterVisitor(ObjViewType_PhylogeneticTree));
        QList<OPWidgetFactory*> opWidgetFactoriesForSeqView = opWidgetFactoryRegistry->getRegisteredFactories(filters.qlist);
        for (OPWidgetFactory* factory : qAsConst(opWidgetFactoriesForSeqView)) {
            optionsPanelController->addGroup(factory);
        }
    }
    createActions();

    objects.append(phyObject);
    requiredObjects.append(phyObject);
    onObjectAdded(phyObject);

    connect(phyObject, &PhyTreeObject::si_phyTreeChanged, this, [this] {
        CHECK(ui != nullptr, );
        ui->switchTreeLayout(ui->getTreeLayoutType());
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
    zoomInAction = new QAction(QIcon(":core/images/zoom_in_tree.png"), tr("Zoom In"), ui);
    zoomInAction->setObjectName("zoomInTreeViewerAction");
    zoomOutAction = new QAction(QIcon(":core/images/zoom_out_tree.png"), tr("Zoom Out"), ui);
    zoomOutAction->setObjectName("zoomOutTreeViewerAction");

    zoom100Action = new QAction(QIcon(":core/images/zoom_1_1.png"), tr("Reset Zoom"), ui);
    zoom100Action->setObjectName("zoom100Action");

    zoomFitAction = new QAction(QIcon(":core/images/zoom_fit.png"), tr("Fit Zoom to Window"), ui);
    zoomFitAction->setObjectName("zoomFitAction");
    zoomFitAction->setCheckable(true);

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

    tb->addSeparator();
    tb->addAction(collapseAction);
    tb->addAction(rerootAction);
    tb->addAction(swapAction);

    // Zooming
    tb->addSeparator();
    tb->addAction(zoomInAction);
    tb->addAction(zoomOutAction);
    tb->addAction(zoom100Action);
    tb->addAction(zoomFitAction);
}

void TreeViewer::buildMenu(QMenu* m, const QString& type) {
    if (type != GObjectViewMenuType::STATIC) {
        GObjectViewController::buildMenu(m, type);
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
    m->addAction(zoom100Action);
    m->addAction(zoomFitAction);

    // Print and Capture
    m->addSeparator();
    m->addAction(printAction);

    auto treeImageActionsSubmenu = new QMenu(tr("Tree image"), ui);
    treeImageActionsSubmenu->menuAction()->setObjectName("treeImageActionsSubmenu");
    treeImageActionsSubmenu->setIcon(QIcon(":/core/images/cam2.png"));
    setupExportTreeImageMenu(treeImageActionsSubmenu);
    m->addMenu(treeImageActionsSubmenu);

    m->addSeparator();

    GObjectViewController::buildMenu(m, type);
    GUIUtils::disableEmptySubmenus(m);
}

QWidget* TreeViewer::createViewWidget(QWidget* parent) {
    SAFE_POINT(ui == nullptr, "View widget was already created", ui);
    ui = new TreeViewerUI(this, parent);
    return ui;
}

void TreeViewer::onAfterViewWindowInit() {
    ui->updateScene();
}

void TreeViewer::onObjectRenamed(GObject*, const QString&) {
    // update title
    OpenTreeViewerTask::updateTitle(this);
}

////////////////////////////
// TreeViewerUI

/** Zoom level change per single clickZoomInButton/clickZoomOutButton operation. */
static constexpr double ZOOM_LEVEL_STEP = 1.2;

/**
 * Minimum zoom level: 10x reduction.
 * TODO: a static value may be not enough for very big trees: make this value dynamic.
 */
static constexpr double MINIMUM_ZOOM_LEVEL = 0.1;

/** Maximum zoom level: 10x magnification. */
static const double MAXIMUM_ZOOM_LEVEL = 10.0;

/** Margins around the whole tree on the scene. On-screen pixels. */
static constexpr int TREE_MARGINS = 10;

static const QString SETTINGS_PATH = "tree_viewer";

static QHash<TreeViewOption, QString> createTreeOptionsSettingNameMap() {
#define INIT_OPTION_NAME(option) map[option] = QString(#option).toLower()
    QHash<TreeViewOption, QString> map;
    INIT_OPTION_NAME(BRANCHES_TRANSFORMATION_TYPE);
    INIT_OPTION_NAME(TREE_LAYOUT_TYPE);
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
    INIT_OPTION_NAME(SHOW_TIP_SHAPE);
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

    settings[TREE_LAYOUT_TYPE] = RECTANGULAR_LAYOUT;
    settings[BRANCHES_TRANSFORMATION_TYPE] = DEFAULT;
    settings[SCALEBAR_RANGE] = 0.05;  // Based on values from COI.aln.
    settings[SCALEBAR_FONT_SIZE] = 10;
    settings[SCALEBAR_LINE_WIDTH] = 1;
    settings[LABEL_COLOR] = QColor(Qt::darkGray);
    settings[LABEL_FONT_FAMILY] = "";  // System default.
    settings[LABEL_FONT_SIZE] = 12;
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
    settings[SHOW_TIP_SHAPE] = false;
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

TreeViewerUI::TreeViewerUI(TreeViewer* _treeViewer, QWidget* parent)
    : QGraphicsView(parent), phyObject(_treeViewer->getPhyObject()),
      treeViewer(_treeViewer) {
    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::PHYLOGENETIC_TREE).icon);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setScene(new QGraphicsScene());

    initializeSettings();
    switchTreeLayout(RECTANGULAR_LAYOUT);

    connect(treeViewer->nameLabelsAction, &QAction::triggered, this, &TreeViewerUI::sl_showNameLabelsTriggered);
    connect(treeViewer->distanceLabelsAction, &QAction::triggered, this, &TreeViewerUI::sl_showDistanceLabelsTriggered);
    connect(treeViewer->printAction, &QAction::triggered, this, &TreeViewerUI::sl_printTriggered);
    connect(treeViewer->copyWholeTreeImageToClipboardAction, &QAction::triggered, this, &TreeViewerUI::copyWholeTreeImageToClipboard);
    connect(treeViewer->saveVisibleViewToFileAction, &QAction::triggered, this, &TreeViewerUI::saveVisibleViewToFile);
    connect(treeViewer->saveWholeTreeToSvgAction, &QAction::triggered, this, &TreeViewerUI::saveWholeTreeToSvg);
    connect(treeViewer->alignTreeLabelsAction, &QAction::triggered, this, &TreeViewerUI::sl_contTriggered);
    connect(treeViewer->rectangularLayoutAction, &QAction::triggered, this, [this] { switchTreeLayout(RECTANGULAR_LAYOUT); });
    connect(treeViewer->circularLayoutAction, &QAction::triggered, this, [this] { switchTreeLayout(CIRCULAR_LAYOUT); });
    connect(treeViewer->unrootedLayoutAction, &QAction::triggered, this, [this] { switchTreeLayout(UNROOTED_LAYOUT); });
    connect(treeViewer->textSettingsAction, &QAction::triggered, this, &TreeViewerUI::sl_textSettingsTriggered);
    connect(treeViewer->treeSettingsAction, &QAction::triggered, this, &TreeViewerUI::sl_treeSettingsTriggered);
    connect(treeViewer->zoomInAction, &QAction::triggered, this, &TreeViewerUI::zoomIn);
    connect(treeViewer->zoomOutAction, &QAction::triggered, this, &TreeViewerUI::zoomOut);
    connect(treeViewer->zoom100Action, &QAction::triggered, this, &TreeViewerUI::zoomTo100);
    connect(treeViewer->zoomFitAction, &QAction::triggered, this, &TreeViewerUI::zoomFit);
    connect(treeViewer->collapseAction, &QAction::triggered, this, &TreeViewerUI::sl_collapseTriggered);
    connect(treeViewer->rerootAction, &QAction::triggered, this, &TreeViewerUI::sl_rerootTriggered);
    connect(treeViewer->swapAction, &QAction::triggered, this, &TreeViewerUI::sl_swapTriggered);

    buttonPopup = new QMenu(this);

    // chrootAction->setEnabled(false); //not implemented yet

    buttonPopup->addAction(treeViewer->zoomInAction);
    buttonPopup->addAction(treeViewer->zoomOutAction);
    buttonPopup->addAction(treeViewer->zoom100Action);
    buttonPopup->addAction(treeViewer->zoomFitAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(treeViewer->swapAction);
    treeViewer->swapAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->rerootAction);
    treeViewer->rerootAction->setEnabled(false);

    buttonPopup->addAction(treeViewer->collapseAction);
    buttonPopup->addSeparator();

    auto treeImageActionsMenu = new QMenu(tr("Tree image"), this);
    treeImageActionsMenu->menuAction()->setObjectName("treeImageActionsMenu");
    treeImageActionsMenu->addAction(treeViewer->copyWholeTreeImageToClipboardAction);
    treeImageActionsMenu->addSeparator();
    treeImageActionsMenu->addAction(treeViewer->saveVisibleViewToFileAction);
    treeImageActionsMenu->addAction(treeViewer->saveWholeTreeToSvgAction);
    treeImageActionsMenu->setIcon(QIcon(":/core/images/cam2.png"));
    buttonPopup->addMenu(treeImageActionsMenu);

    updateActions();
    setObjectName("treeView");
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
    settings[TREE_LAYOUT_TYPE] = RECTANGULAR_LAYOUT;
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

TreeLayoutType TreeViewerUI::getTreeLayoutType() const {
    return static_cast<TreeLayoutType>(getOption(TREE_LAYOUT_TYPE).toInt());
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
    if (option != TREE_LAYOUT_TYPE) {  // TREE_LAYOUT setting is updated as a part of 'switchTreeLayout' call below.
        saveOptionToSettings(option, newValue);
    }
    switch (option) {
        case TREE_LAYOUT_TYPE:
            switchTreeLayout(static_cast<TreeLayoutType>(newValue.toInt()));
            break;
        case BRANCHES_TRANSFORMATION_TYPE:
            switchTreeLayout(getTreeLayoutType());
            break;
        case BREADTH_SCALE_ADJUSTMENT_PERCENT:
        case BRANCH_CURVATURE:
            updateBranchGeometry(rectRoot);
            updateScene();
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
            treeViewer->alignTreeLabelsAction->setEnabled(newValue.toBool());
            treeViewer->nameLabelsAction->setChecked(newValue.toBool());
            updateScene();
            break;
        case SHOW_BRANCH_DISTANCE_LABELS:
            treeViewer->distanceLabelsAction->setChecked(newValue.toBool());
            updateScene();
            break;
        case SHOW_INNER_NODE_LABELS:
        case SHOW_NODE_SHAPE:
        case SHOW_TIP_SHAPE:
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

OptionsMap TreeViewerUI::getSelectionSettings() const {
    OptionsMap effectiveSettings = settings;
    effectiveSettings.insert(selectionSettingsDelta);
    return effectiveSettings;
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
        } else if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            nodeItem->updateSettings(selectionSettings);
        }
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
        } else if (auto legendText = dynamic_cast<TvTextItem*>(item)) {
            legendText->setBrush(qvariant_cast<QColor>(selectionSettings[LABEL_COLOR]));
        }
    }
}

/** Recalculates and assign 'steps to leaf' properties to every branch item in the rect-layout tree. */
static void updateStepsToLeafOnBranches(TvRectangularBranchItem* rectRoot) {
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

static void collectChildBranches(TvBranchItem* item, QList<TvBranchItem*>& result) {
    QList<QGraphicsItem*> childItems = item->childItems();
    for (auto childItem : qAsConst(childItems)) {
        if (auto childBranchItem = dynamic_cast<TvBranchItem*>(childItem)) {
            result << childBranchItem;
            collectChildBranches(childBranchItem, result);
        }
    }
}

/** Returns average branch distance in the tree. */
static double getAverageBranchDistance(TvBranchItem* root) {
    double sumOfDistances = 0;
    int countOfBranches = 0;
    QList<TvBranchItem*> branches;
    collectChildBranches(root, branches);
    for (TvBranchItem* branchItem : qAsConst(branches)) {
        sumOfDistances += qAbs(branchItem->getDist());
        countOfBranches++;
    }
    SAFE_POINT(countOfBranches > 0, "Count of branches is 0", 0);
    return sumOfDistances / countOfBranches;
}

/** Updates branches geometry to match current settings and distanceToViewScale. */
static void updateBranches(TvRectangularBranchItem* rectRoot, double distanceToViewScale, const TreeType& treeType, double breadthScale, double branchCurvature) {
    updateStepsToLeafOnBranches(rectRoot);
    double averageBranchDistance = getAverageBranchDistance(rectRoot);
    double breadthScaleAdjustment = breadthScale / 100;

    QList<TvBranchItem*> allBranches;
    collectChildBranches(rectRoot, allBranches);
    for (auto branchItem : qAsConst(allBranches)) {
        auto rectBranchItem = dynamic_cast<TvRectangularBranchItem*>(branchItem);
        SAFE_POINT(rectBranchItem != nullptr, "Not a rect item!", );
        rectBranchItem->setBreathScaleAdjustment(breadthScaleAdjustment);
        rectBranchItem->setCurvature(branchCurvature);

        switch (treeType) {
            case DEFAULT:
                if (branchItem->getDistanceTextItem() != nullptr && branchItem->getDistanceTextItem()->text() == "") {
                    branchItem->setDistanceText("0");
                }
                if (branchItem->getNameTextItem() == nullptr) {
                    branchItem->setWidth(averageBranchDistance * distanceToViewScale * branchItem->maxStepsToLeafParentDelta);
                } else {
                    branchItem->setWidth(0);
                }
                break;
            case PHYLOGRAM:
                if (branchItem->getDistanceTextItem() != nullptr && branchItem->getDistanceTextItem()->text() == "0") {
                    branchItem->setDistanceText("");
                }
                branchItem->setWidth(qAbs(branchItem->getDist()) * distanceToViewScale);
                break;
            case CLADOGRAM:
                if (branchItem->getDistanceTextItem() != nullptr && branchItem->getDistanceTextItem()->text() == "") {
                    branchItem->setDistanceText("0");
                }
                if (branchItem->getNameTextItem() == nullptr) {
                    branchItem->setWidth(averageBranchDistance * distanceToViewScale);
                } else {
                    branchItem->setWidth(0);
                }
                break;
            default:
                FAIL("Unexpected tree type value", );
                break;
        }
    }
}

void TreeViewerUI::updateScene() {
    SAFE_POINT(treeViewer != nullptr, "TreeViewerUI::updateScene tree viewer is null!", );

    updateLegend();
    updateLabelsVisibility();
    updateLabelsAlignment();

    // Shrink scene rect if needed to the minimal possible size.
    scene()->setSceneRect(scene()->itemsBoundingRect());

    if (treeViewer->zoomFitAction->isChecked()) {
        zoomFit();
    }

    scene()->update();
}

static QString branchColorSettingsKey("branch_color");
static QString branchThicknessSettingsKey("branch_thickness");

QVariantMap TreeViewerUI::getSettingsState() const {
    QVariantMap m;
    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
            OptionsMap branchSettings = branchItem->getSettings();
            m[branchColorSettingsKey + QString::number(i)] = qvariant_cast<QColor>(branchSettings[BRANCH_COLOR]);
            m[branchThicknessSettingsKey + QString::number(i)] = branchSettings[BRANCH_THICKNESS].toInt();
            i++;
        }
    }

    return m;
}

void TreeViewerUI::setSettingsState(const QVariantMap& m) {
    int i = 0;
    foreach (QGraphicsItem* graphItem, items()) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
            OptionsMap branchSettings = branchItem->getSettings();

            QVariant vColor = m[branchColorSettingsKey + QString::number(i)];
            if (vColor.type() == QVariant::Color) {
                branchSettings[BRANCH_COLOR] = vColor.value<QColor>();
            }

            QVariant vThickness = m[branchThicknessSettingsKey + QString::number(i)];
            if (vThickness.type() == QVariant::Int) {
                branchSettings[BRANCH_THICKNESS] = vThickness.toInt();
            }

            branchItem->updateSettings(branchSettings);
            i++;
        }
    }
}

static QString formatDistanceForScalebar(double distance) {
    QString str = QString::number(distance, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i) {
    }
    if (str[i] == '.') {
        --i;
    }
    str.truncate(i + 1);
    return str;
}

void TreeViewerUI::updateLegend() {
    if (legendItem != nullptr) {
        scene()->removeItem(legendItem);
        delete legendItem;
        legendItem = nullptr;
    }

    auto type = static_cast<TreeType>(getOption(BRANCHES_TRANSFORMATION_TYPE).toInt());
    CHECK(type == PHYLOGRAM, );

    QRectF sceneRectWithNoLegend = scene()->itemsBoundingRect();

    double scalebarRange = getScalebarDistanceRange();
    double legendLineLength = scalebarRange * distanceToViewScale;

    legendItem = new QGraphicsLineItem(0, 0, legendLineLength, 0);
    auto scalebarTextItem = new TvTextItem(legendItem, formatDistanceForScalebar(scalebarRange));

    QFont curFont = TreeViewerUtils::getFontFromSettings(settings);
    curFont.setPointSize(getOption(SCALEBAR_FONT_SIZE).toInt());
    scalebarTextItem->setFont(curFont);

    QPen legendPen = legendItem->pen();
    legendPen.setWidth(getOption(SCALEBAR_LINE_WIDTH).toInt());
    legendItem->setPen(legendPen);

    QRectF textRect = scalebarTextItem->boundingRect();
    scalebarTextItem->setPos(0.5 * (legendLineLength - textRect.width()), -textRect.height());

    // Place the legend into the center of the scene, below other items.
    legendItem->setPos(sceneRectWithNoLegend.left() + sceneRectWithNoLegend.width() / 2 - legendLineLength / 2,
                       sceneRectWithNoLegend.bottom() + textRect.height());
    scene()->addItem(legendItem);
}

void TreeViewerUI::wheelEvent(QWheelEvent* we) {
    // Wheel + Shift changes zoom level. Wheel only -> scrolls.
    if (we->modifiers().testFlag(Qt::ControlModifier)) {
        double newZoomLevel = zoomLevel * pow(ZOOM_LEVEL_STEP, we->delta() / 120.0);
        setZoomLevel(newZoomLevel);
    }
    QGraphicsView::wheelEvent(we);
}

void TreeViewerUI::setZoomLevel(double newZoomLevel, bool cancelFitToViewMode) {
    if (cancelFitToViewMode) {
        SAFE_POINT(treeViewer != nullptr, "treeViewer is null!", );
        treeViewer->zoomFitAction->setChecked(false);
    }
    newZoomLevel = qBound(MINIMUM_ZOOM_LEVEL, newZoomLevel, MAXIMUM_ZOOM_LEVEL);
    CHECK(newZoomLevel != zoomLevel, );
    uiLog.trace("New zoom level: " + QString::number(newZoomLevel));
    if (newZoomLevel == 1) {
        resetTransform();
    } else {
        double scaleChange = newZoomLevel / zoomLevel;
        scale(scaleChange, scaleChange);
    }
    zoomLevel = newZoomLevel;

    updateFixedSizeItemScales();
    updateActions();
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
    // QT does not auto-reduce scene size when some item (a node circle) is shrank.
    scene()->setSceneRect(scene()->itemsBoundingRect());
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
    updateActions();
}

void TreeViewerUI::mouseReleaseEvent(QMouseEvent* e) {
    setDragMode(QGraphicsView::NoDrag);
    bool isLeftButton = e->button() == Qt::LeftButton;
    bool isDragEvent = isLeftButton && (e->globalPos() - lastMousePressPos).manhattanLength() >= QApplication::startDragDistance();
    if (!isSelectionStateManagedByChildOnClick && isLeftButton && !isDragEvent) {
        root->setSelectedRecursively(false);  // Clear selection on any right button click with no shift.
    }
    updateActions();
    updateSettingsOnSelectionChange();
    e->accept();
}

void TreeViewerUI::resizeEvent(QResizeEvent* e) {
    QGraphicsView::resizeEvent(e);
    if (treeViewer->zoomFitAction->isChecked()) {
        zoomFit();
    }
}

void TreeViewerUI::paint(QPainter& painter) {
    painter.setBrush(Qt::darkGray);
    scene()->render(&painter);
}

void TreeViewerUI::saveSelectionAndCollapseStates() {
    treeState = {};
    CHECK(root != nullptr, );
    QList<TvBranchItem*> branches;
    collectChildBranches(root, branches);
    treeState.isRootSelected = root->isSelected();
    for (auto branch : qAsConst(branches)) {
        if (treeState.selectionRootBranch == nullptr && branch->isSelected()) {
            treeState.selectionRootBranch = branch->phyBranch;
        }
        if (branch->isCollapsed()) {
            treeState.collapsedBranches << branch->phyBranch;
        }
    }
}

void TreeViewerUI::restoreSelectionAndCollapseStates() {
    QList<TvBranchItem*> branches;
    branches << root;
    collectChildBranches(root, branches);
    if (treeState.isRootSelected) {
        root->setSelectedRecursively(true);
    }
    std::reverse(branches.begin(), branches.end());  // Collapse children first.
    for (auto branch : qAsConst(branches)) {
        if (branch != root && branch->phyBranch != nullptr && branch->phyBranch == treeState.selectionRootBranch) {
            branch->setSelectedRecursively(true);
        }
        if (treeState.collapsedBranches.contains(branch->phyBranch)) {
            branch->toggleCollapsedState();
        }
    }
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
            switchTreeLayout(getTreeLayoutType());
            break;
        }
    }
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

    TvTextItem* distanceTextItem = branch->getDistanceTextItem();
    if (distanceTextItem != nullptr) {
        QFont font = distanceTextItem->font();
        newSelectionSettingsDelta[LABEL_FONT_FAMILY] = font.family();
        newSelectionSettingsDelta[LABEL_FONT_SIZE] = font.pointSize();
        newSelectionSettingsDelta[LABEL_FONT_BOLD] = font.bold();
        newSelectionSettingsDelta[LABEL_FONT_ITALIC] = font.italic();
        newSelectionSettingsDelta[LABEL_FONT_UNDERLINE] = font.underline();
        newSelectionSettingsDelta[LABEL_COLOR] = distanceTextItem->brush().color();
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
    QList<QGraphicsItem*> childItems = items();
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        auto nodeItem = dynamic_cast<TvNodeItem*>(graphItem);
        if (nodeItem != nullptr && nodeItem->isSelectionRoot()) {
            nodeItem->toggleCollapsedState();
            break;
        }
    }
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
    TreeLayoutType layoutType = getTreeLayoutType();
    if (layoutType != RECTANGULAR_LAYOUT) {
        switchTreeLayout(layoutType);
    } else {  // Re-use current layout.
        updateScene();
    }
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

/** Recalculates distanceToViewScale, minDistance, maxDistance. */
static double computeDistanceToViewScale(TvRectangularBranchItem* rectRoot) {
    static constexpr int DEFAULT_MAX_WIDTH_PER_BRANCH = 500;
    static constexpr double minDistanceForScale = 0.0000000001;

    bool isFirstItem = true;
    double minDistance = 0;
    double maxDistance = 0;

    QStack<TvRectangularBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        TvRectangularBranchItem* item = stack.pop();
        minDistance = isFirstItem ? item->getDist() : qMin(item->getDist(), minDistance);
        maxDistance = isFirstItem ? item->getDist() : qMax(item->getDist(), maxDistance);
        isFirstItem = false;
        QList<QGraphicsItem*> childItems = item->childItems();
        for (QGraphicsItem* ci : qAsConst(childItems)) {
            if (auto gbi = dynamic_cast<TvRectangularBranchItem*>(ci)) {
                stack.push(gbi);
            }
        }
    }
    double minDistanceForViewScale = qMax(minDistance, minDistanceForScale);
    double maxDistanceForViewScale = qMax(maxDistance, minDistanceForScale);

    double minDistScale = TvRectangularBranchItem::DEFAULT_WIDTH / minDistanceForViewScale;
    double maxDistScale = DEFAULT_MAX_WIDTH_PER_BRANCH / maxDistanceForViewScale;
    return qMin(minDistScale, maxDistScale);
}

void TreeViewerUI::updateBranchGeometry(TvRectangularBranchItem* rootBranch) const {
    // Upscale the recalculated tree to 'distanceToViewScale'.
    auto treeType = static_cast<TreeType>(getOption(BRANCHES_TRANSFORMATION_TYPE).toInt());
    double breadthScale = getOption(BREADTH_SCALE_ADJUSTMENT_PERCENT).toDouble();
    double branchCurvature = getOption(BRANCH_CURVATURE).toDouble();
    updateBranches(rootBranch, distanceToViewScale, treeType, breadthScale, branchCurvature);
}

void TreeViewerUI::switchTreeLayout(const TreeLayoutType& newLayoutType) {
    saveSelectionAndCollapseStates();
    PhyNode* phyRoot = phyObject->getTree()->getRootNode();
    TvRectangularBranchItem* newRectRoot = TvRectangularLayoutAlgorithm::buildTvTreeHierarchy(phyRoot);
    TvRectangularLayoutAlgorithm::recalculateTreeLayout(newRectRoot, phyRoot);
    CHECK_EXT(newRectRoot != nullptr, uiLog.error(tr("Failed to build tree layout.")), );

    distanceToViewScale = computeDistanceToViewScale(newRectRoot);
    updateBranchGeometry(newRectRoot);

    TvBranchItem* newRoot = newLayoutType == CIRCULAR_LAYOUT
                                ? TvCircularLayoutAlgorithm::convert(newRectRoot, distanceToViewScale <= TvRectangularBranchItem::DEFAULT_WIDTH)
                                : (newLayoutType == UNROOTED_LAYOUT
                                       ? TvUnrootedLayoutAlgorithm::convert(newRectRoot)
                                       : newRectRoot);
    applyNewTreeLayout(newRoot, newRectRoot, newLayoutType);
    restoreSelectionAndCollapseStates();
}

void TreeViewerUI::sl_onBranchCollapsed(TvBranchItem*) {
    CHECK(isRectangularLayoutMode(), );
    PhyNode* phyRoot = phyObject->getTree()->getRootNode();
    TvRectangularLayoutAlgorithm::recalculateTreeLayout(rectRoot, phyRoot);
    updateBranchGeometry(rectRoot);
    updateScene();
    updateActions();
}

void TreeViewerUI::applyNewTreeLayout(TvBranchItem* newRoot, TvRectangularBranchItem* newRectRoot, const TreeLayoutType& layoutType) {
    if (root != nullptr) {
        scene()->removeItem(root);
        if (rectRoot != root) {
            delete rectRoot;
        }
        delete root;
    }
    root = newRoot;
    rectRoot = newRectRoot;
    connect(root, &TvBranchItem::si_branchCollapsed, this, &TreeViewerUI::sl_onBranchCollapsed);
    scene()->addItem(root);

    switch (layoutType) {
        case RECTANGULAR_LAYOUT:
            treeViewer->rectangularLayoutAction->setChecked(true);
            break;
        case CIRCULAR_LAYOUT:
            treeViewer->circularLayoutAction->setChecked(true);
            break;
        case UNROOTED_LAYOUT:
            treeViewer->unrootedLayoutAction->setChecked(true);
            break;
    }

    saveOptionToSettings(TREE_LAYOUT_TYPE, layoutType);
    updateTreeSettingsOnAllNodes();
    updateTreeSettingsOnSelectedItems();
    updateTextOptionOnSelectedItems();
    updateScene();

    if (!treeViewer->zoomFitAction->isChecked()) {
        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();
        if (layoutType == RECTANGULAR_LAYOUT) {  // Show top-right corner: start of the name list.
            hBar->setValue(hBar->maximum());
            vBar->setValue(vBar->minimum());
        } else {  // Center view.
            hBar->setValue((hBar->minimum() + hBar->maximum()) / 2);
            vBar->setValue((vBar->minimum() + vBar->maximum()) / 2);
        }
    }
}

void TreeViewerUI::updateLabelsVisibility() {
    bool isDistanceLabelVisible = getOption(SHOW_BRANCH_DISTANCE_LABELS).toBool();
    bool isSequenceNameLabelVisible = getOption(SHOW_LEAF_NODE_LABELS).toBool();

    QFlags<LabelType> newVisibleLabelTypes;
    newVisibleLabelTypes.setFlag(LabelType_Distance, isDistanceLabelVisible);
    newVisibleLabelTypes.setFlag(LabelType_SequenceName, isSequenceNameLabelVisible);
    CHECK(newVisibleLabelTypes != visibleLabelTypes, );

    visibleLabelTypes = newVisibleLabelTypes;

    QStack<TvBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }
    while (!stack.isEmpty()) {
        TvBranchItem* branchItem = stack.pop();
        if (auto sequenceNameLabel = branchItem->getNameTextItem()) {
            sequenceNameLabel->setVisible(isSequenceNameLabelVisible);
        }
        if (auto distanceLabel = branchItem->getDistanceTextItem()) {
            distanceLabel->setVisible(isDistanceLabelVisible);
        }
        QList<QGraphicsItem*> childItems = branchItem->childItems();
        for (auto item : qAsConst(childItems)) {
            if (auto childBranchItem = dynamic_cast<TvBranchItem*>(item)) {
                stack.push(childBranchItem);
            }
        }
    }
}

void TreeViewerUI::sl_showNameLabelsTriggered(bool on) {
    updateOption(SHOW_LEAF_NODE_LABELS, on);
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

void TreeViewerUI::zoomTo100() {
    setZoomLevel(1);
}

void TreeViewerUI::zoomFit() {
    CHECK(treeViewer->zoomFitAction->isChecked(), );
    QRectF sceneRect = scene()->itemsBoundingRect();
    double sceneWidth = sceneRect.width() + 2 * TREE_MARGINS;
    double sceneHeight = sceneRect.height() + 2 * TREE_MARGINS;
    double newZoomLevelX = viewport()->width() / sceneWidth;
    double newZoomLevelY = viewport()->height() / sceneHeight;
    double newZoomLevel = qMin(newZoomLevelX, newZoomLevelY);
    setZoomLevel(newZoomLevel, false);
}

double TreeViewerUI::getScalebarDistanceRange() const {
    static constexpr double minDistanceForScalebar = 0.0000000001;
    double scalebarRangeFromSettings = getOption(SCALEBAR_RANGE).toDouble();
    // TODO: it is unsafe to re-use scalebar-range from settings (between different trees),
    //  because different trees have different distance values (scales).
    double maxDistance = 0;
    QList<TvBranchItem*> allBranches;
    collectChildBranches(rectRoot, allBranches);
    for (auto branch : qAsConst(allBranches)) {
        maxDistance = qMax(branch->getDist(), maxDistance);
    }
    return qBound(minDistanceForScalebar, scalebarRangeFromSettings, maxDistance);
}

void TreeViewerUI::updateActions() {
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

    bool hasSelection = !updatingItems.isEmpty();

    bool isLeafSelection = false;
    if (hasSelection) {
        isLeafSelection = true;
        for (QGraphicsItem* graphItem : qAsConst(updatingItems)) {
            if (auto branchItem = dynamic_cast<TvBranchItem*>(graphItem)) {
                if (!branchItem->isLeaf()) {
                    isLeafSelection = false;
                    break;
                }
            }
        }
    }

    bool isRootSelected = root->isSelected();
    treeViewer->collapseAction->setEnabled(hasSelection && !isLeafSelection && !isRootSelected);
    treeViewer->swapAction->setEnabled(hasSelection && !isLeafSelection && !isRootSelected);

    bool isRootedLayout = getTreeLayoutType() != UNROOTED_LAYOUT;
    treeViewer->rerootAction->setEnabled(hasSelection && !isLeafSelection && !isRootSelected && isRootedLayout);
}

void TreeViewerUI::updateLabelsAlignment() {
    bool isRightAlign = getOption(SHOW_LEAF_NODE_LABELS).toBool() && getOption(ALIGN_LEAF_NODE_LABELS).toBool();  // Align is ON only if names are visible.
    QStack<TvBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }

    double sceneRightPos = scene()->sceneRect().right();
    QList<TvBranchItem*> branchItems;
    double labelsShift = 0;
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
            double newWidth = 0;
            if (isRightAlign) {
                QRectF textRect = nameText->sceneBoundingRect();
                double textRightPos = textRect.right();
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
}

TvBranchItem* TreeViewerUI::getRoot() const {
    return root;
}

}  // namespace U2
