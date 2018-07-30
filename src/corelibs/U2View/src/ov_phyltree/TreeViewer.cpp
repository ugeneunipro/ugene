/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QGraphicsLineItem>
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
#include <QVBoxLayout>
#include <QtMath>
#include <QBuffer>
#include <QFileInfo>
#include <QTextStream>

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/PhyTree.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OrderedToolbar.h>

#include <U2View/MSAEditorTreeViewer.h>

#include "CreateBranchesTask.h"
#include "CreateCircularBranchesTask.h"
#include "CreateRectangularBranchesTask.h"
#include "CreateUnrootedBranchesTask.h"
#include "GraphicsBranchItem.h"
#include "GraphicsButtonItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewer.h"
#include "TreeViewerFactory.h"
#include "TreeViewerState.h"
#include "TreeViewerTasks.h"
#include "TreeViewerUtils.h"
#include "phyltree/BranchSettingsDialog.h"
#include "phyltree/CreatePhyTreeDialogController.h"
#include "phyltree/TextSettingsDialog.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {


TreeViewer::TreeViewer(const QString& viewName, GObject* obj, GraphicsRectangularBranchItem* _root, qreal s):
    GObjectView(TreeViewerFactory::ID, viewName),
    treeSettingsAction(NULL),
    layoutGroup(NULL),
    rectangularLayoutAction(NULL),
    circularLayoutAction(NULL),
    unrootedLayoutAction(NULL),
    branchesSettingsAction(NULL),
    nameLabelsAction(NULL),
    nodeLabelsAction(NULL),
    distanceLabelsAction(NULL),
    textSettingsAction(NULL),
    contAction(NULL),
    zoomToSelAction(NULL),
    zoomToAllAction(NULL),
    zoomOutAction(NULL),
    printAction(NULL),
    captureTreeAction(NULL),
    exportAction(NULL),
    collapseAction(NULL),
    rerootAction(NULL),
    swapAction(NULL),
    root(_root),
    scale(s),
    ui(NULL)
{
    GCOUNTER( cvar, tvar, "PhylTreeViewer" );
    phyObject = qobject_cast<PhyTreeObject*>(obj);
    objects.append(phyObject);
    requiredObjects.append(phyObject);
    onObjectAdded(phyObject);
    connect(phyObject, SIGNAL(si_phyTreeChanged()), SLOT(sl_onPhyTreeChanged()));
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


qreal TreeViewer::getHorizontalZoom() const {
    return ui->getHorizontalZoom();
}
void TreeViewer::setHorizontalZoom(qreal z) {
    ui->setHorizontalZoom(z);
}
qreal TreeViewer::getVerticalZoom() const {
    return ui->getVerticalZoom();
}
void TreeViewer::setVerticalZoom(qreal z) {
    ui->setVerticalZoom(z);
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
    layoutGroup = new QActionGroup(ui);

    rectangularLayoutAction = new QAction(tr("Rectangular"), layoutGroup);
    rectangularLayoutAction->setCheckable(true);
    rectangularLayoutAction->setChecked(true);
    rectangularLayoutAction->setObjectName("Rectangular");

    circularLayoutAction = new QAction(tr("Circular"), layoutGroup);
    circularLayoutAction->setCheckable(true);
    circularLayoutAction->setObjectName("Circular");

    unrootedLayoutAction = new QAction(tr("Unrooted"), layoutGroup);
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
    contAction = new QAction(QIcon(":core/images/align_tree_labels.png"), tr("Align Labels"), ui);
    contAction->setCheckable(true);
    contAction->setObjectName("Align Labels");

    // Zooming
    zoomToSelAction = new QAction(QIcon(":core/images/zoom_in.png"), tr("Zoom In") , ui);
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

void TreeViewer::setupLayoutSettingsMenu(QMenu* m)
{
    m->addActions(layoutGroup->actions());
}

void TreeViewer::setupShowLabelsMenu(QMenu* m)
{
    m->addAction(nameLabelsAction);
    m->addAction(distanceLabelsAction);
}

void TreeViewer::setupCameraMenu(QMenu* m)
{
    m->addAction(captureTreeAction);
    m->addAction(exportAction);
}

void TreeViewer::buildStaticToolbar(QToolBar* tb)
{
    // Tree Settings
    tb->addAction(treeSettingsAction);

    // Layout
    QToolButton* layoutButton = new QToolButton(tb);
    QMenu* layoutMenu = new QMenu(tr("Layout"), ui);
    setupLayoutSettingsMenu(layoutMenu);
    layoutButton->setDefaultAction(layoutMenu->menuAction());
    layoutButton->setPopupMode(QToolButton::InstantPopup);
    layoutButton->setIcon(QIcon(":core/images/tree_layout.png"));
    layoutButton->setObjectName("Layout");
    tb->addWidget(layoutButton);

    // Branch Settings
    tb->addAction(branchesSettingsAction);

    tb->addSeparator();
    tb->addAction(collapseAction);
    tb->addAction(rerootAction);
    tb->addAction(swapAction);

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
    tb->addAction(contAction);
    // Zooming
    tb->addSeparator();
    tb->addAction(zoomToSelAction);
    tb->addAction(zoomOutAction);
    tb->addAction(zoomToAllAction);

    // Print and Capture
    tb->addSeparator();
    tb->addAction(printAction);

    QToolButton* cameraButton = new QToolButton();
    QMenu* cameraMenu = new QMenu(tr("Export Tree Image"), ui);
    setupCameraMenu(cameraMenu);
    cameraButton->setDefaultAction(cameraMenu->menuAction());
    cameraButton->setPopupMode(QToolButton::InstantPopup);
    cameraButton->setIcon(QIcon(":/core/images/cam2.png"));
    cameraMenu->menuAction()->setObjectName("Export Tree Image");
    cameraButton->setObjectName("cameraMenu");
    tb->addWidget(cameraButton);
}

void TreeViewer::buildMSAEditorStaticToolbar(QToolBar* tb) {
    buildStaticToolbar(tb);
    tb->removeAction(zoomToSelAction);
    tb->removeAction(zoomOutAction);
    tb->removeAction(zoomToAllAction);
}

void TreeViewer::buildStaticMenu(QMenu* m)
{
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

    m->addAction(contAction);
    // Zooming
    m->addSeparator();
    m->addAction(zoomToSelAction);
    m->addAction(zoomOutAction);
    m->addAction(zoomToAllAction);

    // Print and Capture
    m->addSeparator();
    m->addAction(printAction);

    QMenu* cameraMenu = new QMenu(tr("Export Tree Image"), ui);
    setupCameraMenu(cameraMenu);
    cameraMenu->setIcon(QIcon(":/core/images/cam2.png"));
    cameraMenu->menuAction()->setObjectName("Export Tree Image");
    m->addMenu(cameraMenu);

    m->addSeparator();

    GObjectView::buildStaticMenu(m);
    GUIUtils::disableEmptySubmenus(m);
}

QWidget* TreeViewer::createWidget() {
    assert(ui == NULL);
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

void TreeViewer::sl_onPhyTreeChanged() {
    ui->onPhyTreeChanged();
}



////////////////////////////
// TreeViewerUI

const qreal TreeViewerUI::ZOOM_COEF = 1.2;
const qreal TreeViewerUI::MINIMUM_ZOOM = 0.1;
const qreal TreeViewerUI::MAXIMUM_ZOOM = 100.0;
const int TreeViewerUI::MARGIN = 10;
const qreal TreeViewerUI::SIZE_COEF = 0.1;


TreeViewerUI::TreeViewerUI(TreeViewer* treeViewer):
    phyObject(treeViewer->getPhyObject()),
    root(treeViewer->getRoot()),
    maxNameWidth(0.0),
    verticalScale(1.0),
    horizontalScale(1.0),
    curTreeViewer(treeViewer),
    updatingFromOP(false),
    rectRoot(treeViewer->getRoot())
{
    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::PHYLOGENETIC_TREE).icon);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setScene(new QGraphicsScene());
    scene()->addItem(root);
    setScale(treeViewer->getScale());
    initializeSettings();
    addLegend();
    updateRect();

    treeViewer->createActions();
    connect(treeViewer->getNameLabelsAction(), SIGNAL(triggered(bool)), SLOT(sl_showNameLabelsTriggered(bool)));
    connect(treeViewer->getDistanceLabelsAction(), SIGNAL(triggered(bool)), SLOT(sl_showDistanceLabelsTriggered(bool)));
    connect(treeViewer->getPrintAction(), SIGNAL(triggered()), SLOT(sl_printTriggered()));
    connect(treeViewer->getCaptureTreeAction(), SIGNAL(triggered()), SLOT(sl_captureTreeTriggered()));
    connect(treeViewer->getExportAction(), SIGNAL(triggered()), SLOT(sl_exportTriggered()));
    connect(treeViewer->getContAction(), SIGNAL(triggered(bool)), SLOT(sl_contTriggered(bool)));
    connect(treeViewer->getRectangularLayoutAction(), SIGNAL(triggered(bool)), SLOT(sl_rectangularLayoutTriggered()));
    connect(treeViewer->getCircularLayoutAction(), SIGNAL(triggered(bool)), SLOT(sl_circularLayoutTriggered()));
    connect(treeViewer->getUnrootedLayoutAction(), SIGNAL(triggered(bool)), SLOT(sl_unrootedLayoutTriggered()));
    connect(treeViewer->getTextSettingsAction(), SIGNAL(triggered()), SLOT(sl_textSettingsTriggered()));
    connect(treeViewer->getTreeSettingsAction(), SIGNAL(triggered()), SLOT(sl_treeSettingsTriggered()));
    connect(treeViewer->getZoomToSelAction(), SIGNAL(triggered()), SLOT(sl_zoomToSel()));
    connect(treeViewer->getZoomOutAction(), SIGNAL(triggered()), SLOT(sl_zoomOut()));
    connect(treeViewer->getZoomToAllAction(), SIGNAL(triggered()), SLOT(sl_zoomToAll()));
    connect(treeViewer->getBranchesSettingsAction(), SIGNAL (triggered()), SLOT(sl_setSettingsTriggered()));
    connect(treeViewer->getCollapseAction(), SIGNAL(triggered()), SLOT(sl_collapseTriggered()));
    connect(treeViewer->getRerootAction(), SIGNAL(triggered()), SLOT(sl_rerootTriggered()));
    connect(treeViewer->getSwapAction(), SIGNAL(triggered()), SLOT(sl_swapTriggered()));

    zoomToAction = treeViewer->getZoomToSelAction();
    zoomOutAction = treeViewer->getZoomOutAction();
    zoomToAllAction = treeViewer->getZoomToAllAction();
    setColorAction = treeViewer->getBranchesSettingsAction();
    captureAction = treeViewer->getCaptureTreeAction();
    exportAction = treeViewer->getExportAction();
    collapseAction = treeViewer->getCollapseAction();
    rerootAction = treeViewer->getRerootAction();
    swapAction = treeViewer->getSwapAction();

    buttonPopup = new QMenu(this);

    //chrootAction->setEnabled(false); //not implemented yet

    buttonPopup->addAction(zoomToAction);
    buttonPopup->addAction(zoomOutAction);
    buttonPopup->addAction(zoomToAllAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(swapAction);
    swapAction->setEnabled(false);

    buttonPopup->addAction(rerootAction);
    rerootAction->setEnabled(false);

    buttonPopup->addAction(collapseAction);
    buttonPopup->addSeparator();

    buttonPopup->addAction(setColorAction);

    QMenu* cameraMenu = new QMenu(tr("Export Tree Image"), this);
    cameraMenu->addAction(captureAction);
    cameraMenu->addAction(exportAction);
    cameraMenu->menuAction()->setObjectName("Export Tree Image");
    cameraMenu->setIcon(QIcon(":/core/images/cam2.png"));
    buttonPopup->addMenu(cameraMenu);

    updateActionsState();
    setObjectName("treeView");
    updateTreeSettings();

    connect(rectRoot, SIGNAL(si_branchCollapsed(GraphicsRectangularBranchItem* )), SLOT(sl_onBranchCollapsed(GraphicsRectangularBranchItem*)));
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
    if(!updatingFromOP) {
        emit si_optionChanged(option, value);
    }
}

void TreeViewerUI::setTreeLayout(TreeLayout newLayout) {
    switch(newLayout) {
        case RECTANGULAR_LAYOUT:
            curTreeViewer->getRectangularLayoutAction()->setChecked(true);
            changeLayout(RECTANGULAR_LAYOUT);
            break;
        case CIRCULAR_LAYOUT:
            curTreeViewer->getCircularLayoutAction()->setChecked(true);
            changeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            curTreeViewer->getUnrootedLayoutAction()->setChecked(true);
            changeLayout(UNROOTED_LAYOUT);
            break;
        default:
            FAIL("Unrecognized tree layout",);
    }
}
TreeLayout TreeViewerUI::getTreeLayout() const {
    return static_cast<TreeLayout>(getOptionValue(TREE_LAYOUT).toUInt());
}
bool TreeViewerUI::layoutIsRectangular() const {
    return RECTANGULAR_LAYOUT == settings[TREE_LAYOUT].toInt();
}

void TreeViewerUI::onPhyTreeChanged() {
    layoutTask = new CreateRectangularBranchesTask(phyObject->getTree()->getRootNode());

    TaskSignalMapper *taskMapper = new TaskSignalMapper(layoutTask);
    connect(taskMapper, SIGNAL(si_taskFinished(Task*)), this, SLOT(sl_rectLayoutRecomputed()));

    TaskScheduler* scheduler = AppContext::getTaskScheduler();
    scheduler->registerTopLevelTask(layoutTask);

}

void TreeViewerUI::updateSettings(const OptionsMap &settings) {
    foreach(TreeViewOption curOption, settings.keys()){
        onSettingsChanged(curOption, settings[curOption]);
    }
}

void TreeViewerUI::changeOption(TreeViewOption option, const QVariant& newValue) {
    updatingFromOP = true;
    onSettingsChanged(option, newValue);
    updatingFromOP = false;
}

void TreeViewerUI::onSettingsChanged(TreeViewOption option, const QVariant& newValue) {
    SAFE_POINT(settings.keys().contains(option), "Unrecongnized option in TreeViewerUI::onSettingsChanged",);
    CHECK(newValue != getOptionValue(option), );
    setOptionValue(option, newValue);
    //Update tree view
    QAction* action = NULL;
    switch(option) {
        case TREE_LAYOUT:
            setTreeLayout(static_cast<TreeLayout>(newValue.toUInt()));
            break;
        case BRANCHES_TRANSFORMATION_TYPE:
        case WIDTH_COEF:
        case HEIGHT_COEF:
            updateTreeSettings();
            break;
        case LABEL_COLOR:
        case LABEL_FONT:
            updateTextSettings();
            break;
        case BRANCH_COLOR:
        case BRANCH_THICKNESS:
        case NODE_COLOR:
        case NODE_RADIUS:
            updateSettings();
            break;
        case SHOW_LABELS:
            action = curTreeViewer->getNameLabelsAction();
            changeNamesDisplay();
            action->setChecked(newValue.toBool());
            break;
        case SHOW_DISTANCES:
            action = curTreeViewer->getDistanceLabelsAction();
            showLabels(LabelType_Distance);
            action->setChecked(newValue.toBool());
            break;
        case SHOW_NODE_LABELS:
            changeNodeValuesDisplay();
            break;
        case ALIGN_LABELS:
            action = curTreeViewer->getContAction();
            changeLabelsAlignment();
            action->setChecked(newValue.toBool());
            break;
        case SCALEBAR_RANGE:
        case SCALEBAR_FONT_SIZE:
        case SCALEBAR_LINE_WIDTH:
            updateLegend();
            break;
        default:
            FAIL("Unrecongnized option in TreeViewerUI::onSettingsChanged",);
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

    if (QDialog::Accepted == dialog->result()) {
        updateSettings(dialog->getSettings());
    }
}

void TreeViewerUI::initializeSettings() {
    setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
    setOptionValue(BRANCHES_TRANSFORMATION_TYPE, DEFAULT);
    setOptionValue(SCALEBAR_RANGE, 30.0 / curTreeViewer->getScale());
    setOptionValue(SCALEBAR_FONT_SIZE, TreeViewerUtils::getFont().pointSize());
    setOptionValue(SCALEBAR_LINE_WIDTH, 1);

    setOptionValue(LABEL_COLOR, QColor(Qt::darkGray));
    setOptionValue(LABEL_FONT,  TreeViewerUtils::getFont());

    setOptionValue(SHOW_LABELS, true);
    setOptionValue(SHOW_DISTANCES,  !phyObject->haveNodeLabels());
    setOptionValue(SHOW_NODE_LABELS,  phyObject->haveNodeLabels());
    setOptionValue(ALIGN_LABELS, false);

    setOptionValue(BRANCH_COLOR, QColor(0, 0, 0));
    setOptionValue(BRANCH_THICKNESS, 1);

    setOptionValue(WIDTH_COEF, 1);
    setOptionValue(HEIGHT_COEF, 1);

    setOptionValue(NODE_RADIUS, 2);
    setOptionValue(NODE_COLOR, QColor(0, 0, 0));

    for(unsigned int i = 0; i < OPTION_ENUM_END; i++) {
        TreeViewOption option = static_cast<TreeViewOption>(i);
        SAFE_POINT(settings.keys().contains(option), "Not all options have been initialized",);
    }
}

void TreeViewerUI::updateSettings() {
    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();
    if(updatingItems.isEmpty()){
        updatingItems = items();

        QGraphicsLineItem* legendLine = dynamic_cast<QGraphicsLineItem*>(legend);
        if(legendLine){
            QPen legendPen;
            QColor branchColor = qvariant_cast<QColor>(getOptionValue(BRANCH_COLOR));
            legendPen.setColor(branchColor);
            legendLine->setPen(legendPen);
        }
    }

    foreach (QGraphicsItem *graphItem, updatingItems) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem) {
            branchItem->updateSettings(settings);
            if(branchItem->getCorrespondingItem()){
                branchItem->getCorrespondingItem()->updateSettings(settings);
            }
        }

        scene()->update();
    }

}
void TreeViewerUI::updateTextSettings(){
    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();
    QColor curColor = qvariant_cast<QColor>(getOptionValue(LABEL_COLOR));
    if(updatingItems.isEmpty()){
        updatingItems = items();

        QList<QGraphicsItem*> legendChildItems = legend->childItems();
        if(!legendChildItems.isEmpty()){
            QGraphicsSimpleTextItem* legendText = dynamic_cast<QGraphicsSimpleTextItem*>(legendChildItems.first());
            if(legendText){
                legendText->setBrush(curColor);
            }
        }
    }

    QFont curFont = qvariant_cast<QFont>(getOptionValue(LABEL_FONT));
    foreach (QGraphicsItem *graphItem, updatingItems) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem != NULL) {
            branchItem->updateTextSettings(qvariant_cast<QFont>(getOptionValue(LABEL_FONT)), curColor);
            if(branchItem->getCorrespondingItem()){
                branchItem->getCorrespondingItem()->updateTextSettings(curFont, curColor);
            }
        }
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != NULL) {
            buttonItem->updateSettings(getSettings());
        }

    }

    updateLayout();
    updateTreeSettings();
}
void TreeViewerUI::updateTreeSettings(bool setDefautZoom){

    qreal avgW = 0;
    TREE_TYPE type = static_cast<TREE_TYPE>(getOptionValue(BRANCHES_TRANSFORMATION_TYPE).toUInt());
    if(type != PHYLOGRAM){
        avgW = avgWidth();
        legend->setVisible(false);
    }else{
        legend->setVisible(true);
    }

    if(type == DEFAULT) {
        determineBranchLengths();
    }

    QStack<GraphicsBranchItem*> stack;
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsBranchItem *item = stack.pop();

        foreach (QGraphicsItem* ci, item->childItems()) {
            GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(ci);
            if (gbi != NULL) {
                stack.push(gbi);
                gbi->setLenghtCoef(qMax(1, item->getBranchLength() - gbi->getBranchLength()));
            }
        }

        if(item!=rectRoot){

            if(getOptionValue(TREE_LAYOUT).toInt() == RECTANGULAR_LAYOUT){
                GraphicsRectangularBranchItem *rectItem = dynamic_cast<GraphicsRectangularBranchItem*>(item);
                if(rectItem){
                    rectItem->setHeightCoef(getOptionValue(HEIGHT_COEF).toUInt());
                }
            }

            qreal coef = qMax(static_cast<qreal>(1.0), TreeViewerUI::SIZE_COEF * getOptionValue(WIDTH_COEF).toUInt());

            switch (type) {
            case DEFAULT:
                if(item->getDistanceText() != NULL){
                    if(item->getDistanceText()->text() == ""){
                        item->setDistanceText("0");
                    }
                }
                if(item->getNameText() == NULL) {
                    item->setWidth(avgW * getScale()* coef * item->getLengthCoef());
                }
                else {
                    item->setWidth(0);
                }
                break;
            case PHYLOGRAM:
                if(item->getDistanceText() != NULL){
                    if(item->getDistanceText()->text() == "0"){
                        item->setDistanceText("");
                    }
                }
                item->setWidth(qAbs(item->getDist()) * getScale() * coef);
                break;
            case CLADOGRAM:
                if(item->getDistanceText() != NULL){
                    if(item->getDistanceText()->text() == ""){
                        item->setDistanceText("0");
                    }
                }
                if(item->getNameText() == NULL) {
                    item->setWidth(avgW * getScale()* coef);
                }
                else {
                    item->setWidth(0);
                }
                break;
            default:
                FAIL("Unexpected tree type value",);
                break;
            }
        }
    }
    updateLegend();
    updateRect();
    scene()->update();

    showLabels(LabelType_Distance);
    showLabels(LabelType_SequnceName);
    bool alignLabels = getOptionValue(ALIGN_LABELS).toBool();
    if(alignLabels){
        updateLabelsAlignment();
    }

    defaultZoom();
    if(setDefautZoom) {
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    }
}

void TreeViewerUI::determineBranchLengths() {
    QStack<GraphicsBranchItem*> stack;
    QQueue<GraphicsBranchItem*> childQueue;
    stack.push(rectRoot);
    while (!stack.empty()) {
        GraphicsBranchItem *item = stack.pop();
        int childsCounter = 0;

        foreach (QGraphicsItem* ci, item->childItems()) {
            GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(ci);
            if (gbi != NULL) {
                stack.push(gbi);
                childsCounter++;
            }
        }

        if(item == rectRoot) {
            continue;
        }

        if(0 == childsCounter) {
            childQueue.enqueue(item);
            item->setBranchLength(0);
            continue;
        }
    }
    while(!childQueue.isEmpty()) {
        GraphicsBranchItem *item = childQueue.dequeue();
        GraphicsBranchItem *parentItem = dynamic_cast<GraphicsBranchItem *>(item->getParentItem());
        if(NULL == parentItem) {
            continue;
        }

        parentItem->setBranchLength(qMax(parentItem->getBranchLength(), item->getBranchLength() + 1));

        childQueue.enqueue(parentItem);
    }
}

QVariantMap TreeViewerUI::getSettingsState() const {
    QString branchColor("branch_color");
    QString branchThisckness("branch_thickness");
    QVariantMap m;

    int i=0;
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem != NULL) {
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
    int i=0;
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem) {
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
    for (; i >= 0 && str[i] == '0'; --i) ;
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
    qreal coef  = qMax(static_cast<qreal>(1.0), TreeViewerUI::SIZE_COEF * getOptionValue(WIDTH_COEF).toUInt());
    qreal WIDTH = getOptionValue(SCALEBAR_RANGE).toDouble() * coef * curTreeViewer->getScale();

    qreal d = getOptionValue(SCALEBAR_RANGE).toDouble();
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i) ;
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

void TreeViewerUI::wheelEvent(QWheelEvent *we) {
    qreal zoom1 = pow(ZOOM_COEF, we->delta() / 120.0);
    zooming(zoom1);
    we->accept();
}

void TreeViewerUI::zooming(qreal newzoom){
    zooming(newzoom, newzoom);
}

void TreeViewerUI::zooming(qreal horizontalZoom, qreal verticalZoom) {
    if(verticalZoom < 0 || horizontalZoom < 0){
        return ;
    }
    verticalZoom = verticalScale * verticalZoom;
    horizontalZoom = horizontalScale * horizontalZoom;

    verticalZoom = qMax(MINIMUM_ZOOM, verticalZoom);
    verticalZoom = qMin(MAXIMUM_ZOOM * qMax(getOptionValue(HEIGHT_COEF).toUInt() * TreeViewerUI::SIZE_COEF, static_cast<qreal>(1.0)), verticalZoom);
    horizontalZoom = qMax(MINIMUM_ZOOM, horizontalZoom);
    horizontalZoom = qMin(MAXIMUM_ZOOM * qMax(getOptionValue(WIDTH_COEF).toUInt() * TreeViewerUI::SIZE_COEF, static_cast<qreal>(1.0)), horizontalZoom);

    scale(horizontalZoom / horizontalScale, verticalZoom / verticalScale);
    verticalScale = verticalZoom;
    horizontalScale = horizontalZoom;
    updateActionsState();
}

void TreeViewerUI::mousePressEvent(QMouseEvent *e) {

    bool shiftPressed = e->modifiers() & Qt::ShiftModifier;
    bool leftButton = e->button() == Qt::LeftButton;

    if (leftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

    if (leftButton && !shiftPressed) {
        root->setSelectedRecurs(false, true); // clear selection
    }

    if (e->button() == Qt::RightButton) {

        buttonPopup->popup(e->globalPos());

        e->accept();
        return;
    }

    QGraphicsView::mousePressEvent(e);
    if (leftButton && !shiftPressed) {
        updateBrachSettings();
    }
    updateActionsState();
}

void TreeViewerUI::mouseReleaseEvent(QMouseEvent *e) {
    bool leftButton = e->button() == Qt::LeftButton;
    if (leftButton) {
        setDragMode(QGraphicsView::NoDrag);
    }
    e->accept();
}

void TreeViewerUI::resizeEvent(QResizeEvent *e) {
    QRectF rect = scene()->sceneRect();
    rect.setWidth(rect.width() / horizontalScale);
    rect.setHeight(rect.height() / verticalScale);
    rect.moveCenter(scene()->sceneRect().center());
    fitInView(rect, Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(e);
}

void TreeViewerUI::paint(QPainter &painter) {
    painter.setBrush(Qt::darkGray);
    painter.setFont(TreeViewerUtils::getFont());
    scene()->render(&painter);
}

void TreeViewerUI::updateRect() {
    SAFE_POINT(NULL != root, "Pointer to tree root is NULL",);
    QTransform viewTransform = transform();
    // the workaround for UGENE-3504
    if (qobject_cast<MSAEditorTreeViewerUI*>(this) != NULL
            && (qobject_cast<MSAEditorTreeViewerUI*>(this))->isCurTreeViewerSynchronized()) {
        viewTransform = QTransform();
    }
    QRectF rect = root->visibleChildrenBoundingRect(viewTransform) | root->sceneBoundingRect();
    rect.setLeft(rect.left() - MARGIN);
    rect.setRight(rect.right() + MARGIN);
    rect.setTop(rect.top() - MARGIN);
    rect.setBottom(rect.bottom() + legend->childrenBoundingRect().height() + MARGIN);
    legend->setPos(0, rect.bottom() - MARGIN);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::sl_swapTriggered() {
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem && buttonItem->isSelectedTop()) {
            buttonItem->swapSiblings();
            phyObject->onTreeChanged();
            break;
        }
    }
    qreal curHScale = horizontalScale;
    qreal curVScale = verticalScale;

    QTransform curTransform = viewportTransform();
    setTransformationAnchor(NoAnchor);

    redrawRectangularLayout();
    updateLayout();
    updateTreeSettings();

    setTransform(curTransform);
    horizontalScale = curHScale;
    verticalScale = curVScale;
    updateActionsState();
    setTransformationAnchor(AnchorUnderMouse);
}

void TreeViewerUI::sl_rerootTriggered() {
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem != NULL && buttonItem->isSelectedTop()) {
            buttonItem->rerootTree(phyObject);
            break;
        }
    }
}

void TreeViewerUI::collapseSelected() {
    QList<QGraphicsItem *> childItems = items();
    foreach (QGraphicsItem *graphItem, childItems) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);

        if (buttonItem && buttonItem->isSelectedTop()) {
            buttonItem->collapse();
            break;
        }
    }
}

void TreeViewerUI::updateBrachSettings() {
    QList<QGraphicsItem *> childItems = items();
    foreach (QGraphicsItem *graphItem, childItems) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);

        if (buttonItem && buttonItem->isSelectedTop()) {
            GraphicsBranchItem *branch = dynamic_cast<GraphicsBranchItem*>(buttonItem->parentItem());
            SAFE_POINT(NULL != branch, "Collapsing is impossible because button has not parent branch",);

            GraphicsBranchItem *parentBranch = dynamic_cast<GraphicsBranchItem*>(branch->parentItem());
            if(NULL != parentBranch) {
                setOptionValue(BRANCH_THICKNESS, branch->getSettings()[BRANCH_THICKNESS]);
                setOptionValue(BRANCH_COLOR, branch->getSettings()[BRANCH_COLOR]);
            }
            break;
        }
    }
}

bool TreeViewerUI::isSelectedCollapsed(){
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);

        if (buttonItem && buttonItem->isSelectedTop()) {
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

    qbuffer.seek(0); // move pointer to the buffer's start
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
        changeLayout(CIRCULAR_LAYOUT);
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        break;

    case UNROOTED_LAYOUT:
        changeLayout(UNROOTED_LAYOUT);
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        break;
    case RECTANGULAR_LAYOUT:
        //Do nothing
        show();
        break;
    }
}

void TreeViewerUI::sl_rectangularLayoutTriggered() {
    changeLayout(RECTANGULAR_LAYOUT);
}

void TreeViewerUI::sl_circularLayoutTriggered() {
    changeLayout(CIRCULAR_LAYOUT);
}

void TreeViewerUI::sl_unrootedLayoutTriggered() {
    changeLayout(UNROOTED_LAYOUT);
}

void TreeViewerUI::changeLayout(TreeLayout newLayout) {
    root->setSelectedRecurs(false, true); // clear selection
    setOptionValue(TREE_LAYOUT, newLayout);

    switch(newLayout) {
        case RECTANGULAR_LAYOUT:
            scene()->removeItem(root);
            if(!rectRoot){
                redrawRectangularLayout();
            }
            root = rectRoot;
            scene()->addItem(root);
            defaultZoom();
            updateRect();
            break;
        case CIRCULAR_LAYOUT:
            if(getScale() <= GraphicsRectangularBranchItem::DEFAULT_WIDTH){
                layoutTask = new CreateCircularBranchesTask(rectRoot, true);
            }else{
                layoutTask = new CreateCircularBranchesTask(rectRoot);
            }
            connect(layoutTask, SIGNAL(si_stateChanged()), SLOT(sl_layoutRecomputed()));
            break;
        case UNROOTED_LAYOUT:
            layoutTask = new CreateUnrootedBranchesTask(rectRoot);
            connect(layoutTask, SIGNAL(si_stateChanged()), SLOT(sl_layoutRecomputed()));
            break;
    }
    if (newLayout != RECTANGULAR_LAYOUT) {
        TaskScheduler* scheduler = AppContext::getTaskScheduler();
        scheduler->registerTopLevelTask(layoutTask);
    }
    updateTreeSettings();
    onLayoutChanged(newLayout);
}

void TreeViewerUI::sl_rectLayoutRecomputed() {
    bool taskIsFailed = layoutTask->getState() != Task::State_Finished || layoutTask->hasError();
    CHECK(!taskIsFailed, );

    GraphicsRectangularBranchItem* rootNode = dynamic_cast<GraphicsRectangularBranchItem*>(layoutTask->getResult());
    CHECK(NULL != rootNode,);
    disconnect(rectRoot, SIGNAL(si_branchCollapsed(GraphicsRectangularBranchItem* )), this, SLOT(sl_onBranchCollapsed(GraphicsRectangularBranchItem*)));
    rectRoot = rootNode;
    connect(rectRoot, SIGNAL(si_branchCollapsed(GraphicsRectangularBranchItem* )), SLOT(sl_onBranchCollapsed(GraphicsRectangularBranchItem*)));

    switch (getTreeLayout())
    {
    case CIRCULAR_LAYOUT:
        setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
        changeLayout(CIRCULAR_LAYOUT);
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        break;

    case UNROOTED_LAYOUT:
        setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
        changeLayout(UNROOTED_LAYOUT);
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        break;
    case RECTANGULAR_LAYOUT:
        scene()->removeItem(root);
        root = rootNode;
        scene()->addItem(root);
        defaultZoom();
        updateRect();

        break;
    }
    updateTreeSettings();
    updateSettings();
    updateTextSettings();
}

void TreeViewerUI::sl_onBranchCollapsed(GraphicsRectangularBranchItem*) {
    QTransform curTransform = viewportTransform();
    setTransformationAnchor(NoAnchor);

    qreal curScale = getScale();
    redrawRectangularLayout();
    setScale(curScale);
    updateTreeSettings(false);

    setTransform(curTransform);
    updateActionsState();
    setTransformationAnchor(AnchorUnderMouse);
}

void TreeViewerUI::sl_layoutRecomputed() {
    if (layoutTask->getState() != Task::State_Finished || layoutTask->hasError()) {
        return;
    }

    scene()->removeItem(root);
    root = layoutTask->getResult();
    scene()->addItem(root);
    defaultZoom();
    updateRect();

    bool showNames = getOptionValue(SHOW_LABELS).toBool();
    bool showDistances = getOptionValue(SHOW_DISTANCES).toBool();

    changeNodeValuesDisplay();

    if (!showNames || !showDistances) {
        LabelTypes lt;
        if (!showDistances) {
            lt |= LabelType_Distance;
        }
        if (!showNames) {
            lt |= LabelType_SequnceName;
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
        GraphicsBranchItem *node = stack.pop();
        if (labelTypes.testFlag(LabelType_SequnceName)) {
            if (node->getNameText() != NULL) {
                node->setVisible(getOptionValue(SHOW_LABELS).toBool());
                maxNameWidth = qMax(maxNameWidth, node->getNameText()->sceneBoundingRect().width());
            }
        }
        if (labelTypes.testFlag(LabelType_Distance)) {
            if (node->getDistanceText() != NULL) {
                node->getDistanceText()->setVisible(getOptionValue(SHOW_DISTANCES).toBool());
            }
        }
        foreach (QGraphicsItem* item, node->childItems()) {
            GraphicsBranchItem *bitem = dynamic_cast<GraphicsBranchItem*>(item);
            if (bitem != NULL) {
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
    QAction* contAction = curTreeViewer->getContAction();
    contAction->setEnabled(showNames);

    showLabels(LabelType_SequnceName);
    QRectF rect = sceneRect();
    rect.setWidth(rect.width() + (showNames ? 1 : -1) * maxNameWidth);
    scene()->setSceneRect(rect);
}

void TreeViewerUI::changeNodeValuesDisplay() {
    QList<QGraphicsItem *> allItems = scene()->items();
    foreach(QGraphicsItem* curItem, allItems) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(curItem);
        if(NULL != buttonItem) {
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

void TreeViewerUI::sl_textSettingsTriggered(){
    QObjectScopedPointer<TextSettingsDialog> dialog = new TextSettingsDialog(this, getSettings());
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (QDialog::Accepted == dialog->result()) {
         updateSettings(dialog->getSettings());
         if(getOptionValue(ALIGN_LABELS).toBool()){
             QStack<GraphicsBranchItem*> stack;
             stack.push(root);
             if (root != rectRoot) {
                 stack.push(rectRoot);
             }
             while (!stack.empty()) {
                 GraphicsBranchItem* item = stack.pop();
                 if (item->getNameText() == NULL) {
                     foreach (QGraphicsItem* citem, item->childItems()) {
                         GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(citem);
                         if (gbi != NULL) {
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

void TreeViewerUI::sl_treeSettingsTriggered(){
    QObjectScopedPointer<TreeSettingsDialog> dialog = new TreeSettingsDialog(this, getSettings(), getTreeLayout() == RECTANGULAR_LAYOUT);
    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (QDialog::Accepted == dialog->result()) {
        updateSettings(dialog->getSettings());
    }
}

void TreeViewerUI::sl_zoomToSel(){
    QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
    if(selectedItems.isEmpty()){
         zooming(ZOOM_COEF);
    }else{
        GraphicsButtonItem *topButton = NULL;
        foreach (QGraphicsItem *graphItem, selectedItems) {
            GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);

            if (buttonItem && buttonItem->isSelectedTop()) {
                topButton = buttonItem;
                break;
            }
        }
        if(!topButton){
            zooming(ZOOM_COEF);
        }else{
            defaultZoom();
            QGraphicsItem *topItem = topButton->parentItem();
            QRectF rect = topItem->mapRectToScene(topItem->childrenBoundingRect());
            QRectF rect1 = scene()->sceneRect();
            qreal zoom1 = qMin(rect1.width() / rect.width(), rect1.height() / rect.height());
            zooming(zoom1);
            centerOn(rect.center());
        }
    }
}
void TreeViewerUI::sl_zoomOut(){
    zooming(1.0/(float)ZOOM_COEF);
}
void TreeViewerUI::sl_zoomToAll(){
    defaultZoom();
}

void TreeViewerUI::defaultZoom(){
    zooming(1.0/horizontalScale, 1.0/verticalScale);
}

void TreeViewerUI::redrawRectangularLayout(){
    int current = 0;
    qreal minDistance = -2, maxDistance = 0;
    GraphicsRectangularBranchItem* item = rectRoot;

    item->redrawBranches(current, minDistance, maxDistance, phyObject->getTree()->getRootNode());

    item->setWidthW(0);
    item->setDist(0);
    item->setHeightW(0);

    if(minDistance == 0){
        minDistance = GraphicsRectangularBranchItem::EPSILON;
    }
    if(maxDistance == 0){
        maxDistance = GraphicsRectangularBranchItem::EPSILON;
    }

    qreal minDistScale = GraphicsRectangularBranchItem::DEFAULT_WIDTH / (qreal)minDistance;
    qreal maxDistScale = GraphicsRectangularBranchItem::MAXIMUM_WIDTH / (qreal)maxDistance;

    qreal scale = qMin(minDistScale, maxDistScale);
    setScale(scale);
}

qreal TreeViewerUI::avgWidth(){
    qreal sumW = 0;
    int count = 0;

    QList<QGraphicsItem*> updatingItems = scene()->items();

    foreach (QGraphicsItem *graphItem, updatingItems) {

        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem) {
            sumW += qAbs(branchItem->getDist());
            count++;
        }
    }

    return sumW/(qreal)count;
}

void TreeViewerUI::updateActionsState(){

    zoomToAction->setEnabled(horizontalScale < MAXIMUM_ZOOM * qMax(getOptionValue(WIDTH_COEF).toUInt() * TreeViewerUI::SIZE_COEF, static_cast<qreal>(1.0)));
    zoomOutAction->setEnabled(horizontalScale > MINIMUM_ZOOM);

    if(isSelectedCollapsed()){
        collapseAction->setText(QObject::tr("Expand"));
        collapseAction->setIcon(QIcon(":/core/images/expand_tree.png"));
    }else{
        collapseAction->setText(QObject::tr("Collapse"));
        collapseAction->setIcon(QIcon(":/core/images/collapse_tree.png"));
    }

    QList<QGraphicsItem*> updatingItems = scene()->selectedItems();

    bool thereIsSelection = !updatingItems.isEmpty();
    bool rootIsSelected = root->isSelected();
    collapseAction->setEnabled(thereIsSelection && !rootIsSelected);

    bool treeIsRooted = getTreeLayout() != UNROOTED_LAYOUT;
    bool treeIsCircular = getTreeLayout() == CIRCULAR_LAYOUT;
    swapAction->setEnabled(thereIsSelection && treeIsRooted && (!treeIsCircular || !isOnlyLeafSelected()));
    rerootAction->setEnabled(thereIsSelection && !rootIsSelected && treeIsRooted);
}

void TreeViewerUI::updateLayout()
{
    TreeLayout tmpL = getTreeLayout();
    setOptionValue(TREE_LAYOUT, RECTANGULAR_LAYOUT);
    switch(tmpL){
        case CIRCULAR_LAYOUT:
            changeLayout(CIRCULAR_LAYOUT);
            break;
        case UNROOTED_LAYOUT:
            changeLayout(UNROOTED_LAYOUT);
            break;
        case RECTANGULAR_LAYOUT:
            //here to please compiler
            break;
    }
}

void TreeViewerUI::updateLabelsAlignment()
{
    bool on = getOptionValue(ALIGN_LABELS).toBool();
    QStack<GraphicsBranchItem*> stack;
    stack.push(root);
    if (root != rectRoot) {
        stack.push(rectRoot);
    }

    if(!getOptionValue(SHOW_LABELS).toBool()) {
        return;
    }

    qreal sceneRightPos = scene()->sceneRect().right();
    QList<GraphicsBranchItem*> branchItems;
    qreal labelsShift = 0;
    while (!stack.empty()) {
        GraphicsBranchItem* item = stack.pop();
        QGraphicsSimpleTextItem* nameText = item->getNameText();
        if (nameText == NULL) {
            foreach (QGraphicsItem* citem, item->childItems()) {
                GraphicsBranchItem* gbi = dynamic_cast<GraphicsBranchItem*>(citem);
                if (gbi != NULL) {
                    stack.push(gbi);
                }
            }
        } else {
            branchItems.append(item);
            qreal newWidth = 0;
            if(on){
                QRectF textRect= nameText->sceneBoundingRect();
                qreal textRightPos = textRect.right();
                if(nameText->flags().testFlag(QGraphicsItem::ItemIgnoresTransformations)) {
                    QRectF transformedRect = transform().inverted().mapRect(textRect);
                    textRect.setWidth(transformedRect.width());
                    textRightPos = textRect.right();
                }
                newWidth = sceneRightPos - (textRightPos + GraphicsBranchItem::TextSpace);
                labelsShift = qMin(newWidth, labelsShift);
            }
            item->setWidth(newWidth);
        }
    }
    if(labelsShift < 0) {
        foreach(GraphicsBranchItem* curItem,branchItems) {
            curItem->setWidth(curItem->getWidth() - labelsShift);
        }
    }
    updateRect();
}

bool TreeViewerUI::isOnlyLeafSelected() const {
    int selectedItems = 0;
    foreach (QGraphicsItem *graphItem, items()) {
        GraphicsButtonItem *buttonItem = dynamic_cast<GraphicsButtonItem*>(graphItem);
        if (buttonItem && buttonItem->getIsSelected()) {
            selectedItems++;
        }
    }
    return selectedItems == 2;
}

}//namespace
