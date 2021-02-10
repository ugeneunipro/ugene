/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MSAEditorTreeViewer.h"

#include <QCursor>
#include <QDateTime>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStack>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/PhyTree.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/GraphicsButtonItem.h>
#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

#include "MsaEditorTreeTabArea.h"

namespace U2 {

MSAEditorTreeViewer::MSAEditorTreeViewer(const QString &viewName, GObject *obj, GraphicsRectangularBranchItem *_root, qreal s)
    : TreeViewer(viewName, obj, _root, s),
      refreshTreeAction(nullptr),
      sortSeqAction(nullptr),
      editor(nullptr),
      syncMode(WithoutSynchronization),
      slotsAreConnected(false) {
}

QWidget *MSAEditorTreeViewer::createWidget() {
    SAFE_POINT(ui == nullptr, QString("MSAEditorTreeViewer::createWidget error"), nullptr);

    auto view = new QWidget();
    view->setObjectName("msa_editor_tree_view_container_widget");

    auto viewLayout = new QVBoxLayout();
    ui = new MSAEditorTreeViewerUI(this);

    auto toolBar = new QToolBar(tr("MSAEditor tree toolbar"));
    buildMSAEditorStaticToolbar(toolBar);

    sortSeqAction = new QAction(QIcon(":core/images/sort_ascending.png"), tr("Sort alignment by tree"), ui);
    sortSeqAction->setObjectName("Sort Alignment");
    connect(sortSeqAction, SIGNAL(triggered()), ui, SLOT(sl_sortAlignment()));

    refreshTreeAction = new QAction(QIcon(":core/images/refresh.png"), tr("Refresh tree"), ui);
    refreshTreeAction->setObjectName("Refresh tree");
    refreshTreeAction->setEnabled(false);
    connect(refreshTreeAction, SIGNAL(triggered()), SLOT(sl_refreshTree()));

    toolBar->addSeparator();
    toolBar->addAction(refreshTreeAction);
    toolBar->addAction(sortSeqAction);

    viewLayout->setSpacing(0);
    viewLayout->setMargin(0);
    viewLayout->addWidget(toolBar);
    viewLayout->addWidget(ui);
    view->setLayout(viewLayout);

    return view;
}

void MSAEditorTreeViewer::setMSAEditor(MSAEditor *newEditor) {
    editor = newEditor;
    CHECK(editor != nullptr, );

    connect(ui, SIGNAL(si_zoomIn()), editor, SLOT(sl_zoomIn()));
    connect(ui, SIGNAL(si_zoomOut()), editor, SLOT(sl_zoomOut()));
    connect(ui, SIGNAL(si_resetZooming()), editor, SLOT(sl_resetZoom()));
}

MSAEditor *MSAEditorTreeViewer::getMsaEditor() const {
    return editor;
}

void MSAEditorTreeViewer::setCreatePhyTreeSettings(const CreatePhyTreeSettings &newBuildSettings) {
    buildSettings = newBuildSettings;
    refreshTreeAction->setEnabled(true);
}

void MSAEditorTreeViewer::sl_refreshTree() {
    emit si_refreshTree(this);
}

bool MSAEditorTreeViewer::sync() {
    CHECK(editor != nullptr, false);
    MSAEditorTreeViewerUI *treeViewerUI = qobject_cast<MSAEditorTreeViewerUI *>(ui);
    CHECK(treeViewerUI != nullptr, false);
    if (!treeViewerUI->canSynchronizeWithMSA(editor)) {
        return false;
    }

    syncMode = FullSynchronization;
    treeViewerUI->setSynchronizeMode(syncMode);

    CHECK(editor != nullptr, false);
    MsaEditorWgt *msaUI = editor->getUI();
    connect(msaUI->editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), this, SLOT(sl_alignmentChanged(MultipleAlignment, MaModificationInfo)));
    connect(msaUI, SIGNAL(si_stopMaChanging(bool)), this, SLOT(sl_startTracking(bool)));

    connectSignals();
    sortSeqAction->setEnabled(true);
    treeViewerUI->sl_sortAlignment();
    treeViewerUI->highlightBranches();

    // Trigger si_visibleRangeChanged that will make tree widget update geometry to the correct scale. TODO: create a better API for this.
    editor->getUI()->getSequenceArea()->onVisibleRangeChanged();

    return true;
}

void MSAEditorTreeViewer::desync() {
    disconnectSignals();

    CHECK(editor != nullptr, );
    MsaEditorWgt *msaUI = editor->getUI();
    CHECK(msaUI != nullptr, );

    msaUI->getEditorNameList()->clearGroupsSelections();
    msaUI->getEditorNameList()->update();

    syncMode = WithoutSynchronization;
    MSAEditorTreeViewerUI *treeViewerUI = qobject_cast<MSAEditorTreeViewerUI *>(ui);
    treeViewerUI->setSynchronizeMode(syncMode);
    sortSeqAction->setEnabled(false);
}

bool MSAEditorTreeViewer::isSynchronized() const {
    return syncMode == FullSynchronization;
}

void MSAEditorTreeViewer::connectSignals() {
    CHECK(!slotsAreConnected, );
    CHECK(editor != nullptr, );

    MsaEditorWgt *msaUI = editor->getUI();
    CHECK(msaUI != nullptr, );

    auto treeViewerUI = qobject_cast<MSAEditorTreeViewerUI *>(ui);
    CHECK(treeViewerUI != nullptr, );

    connect(msaUI, SIGNAL(si_startMaChanging()), this, SLOT(sl_stopTracking()));

    connect(treeViewerUI, SIGNAL(si_collapseModelChangedInTree(const QList<QStringList> &)), msaUI->getSequenceArea(), SLOT(sl_setVirtualGroupingMode(const QList<QStringList> &)));
    connect(treeViewerUI, SIGNAL(si_groupColorsChanged(const GroupColorSchema &)), msaUI->getEditorNameList(), SLOT(sl_onGroupColorsChanged(const GroupColorSchema &)));

    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), treeViewerUI, SLOT(sl_onReferenceSeqChanged(qint64)));
    connect(msaUI->getSequenceArea(), SIGNAL(si_visibleRangeChanged(QStringList, int)), treeViewerUI, SLOT(sl_onVisibleRangeChanged(QStringList, int)));

    connect(msaUI->getSequenceArea(), SIGNAL(si_selectionChanged(const QStringList &)), treeViewerUI, SLOT(sl_selectionChanged(const QStringList &)));
    connect(msaUI->getEditorNameList(), SIGNAL(si_sequenceNameChanged(QString, QString)), treeViewerUI, SLOT(sl_sequenceNameChanged(QString, QString)));

    slotsAreConnected = true;
}

void MSAEditorTreeViewer::disconnectSignals() {
    CHECK(slotsAreConnected, );
    CHECK(editor != nullptr, );

    MsaEditorWgt *msaUI = editor->getUI();
    CHECK(msaUI != nullptr, );

    auto treeViewerUI = qobject_cast<MSAEditorTreeViewerUI *>(ui);
    CHECK(treeViewerUI != nullptr, );

    disconnect(msaUI, SIGNAL(si_startMaChanging()), this, SLOT(sl_stopTracking()));

    disconnect(treeViewerUI, SIGNAL(si_collapseModelChangedInTree(const QList<QStringList> &)), msaUI->getSequenceArea(), SLOT(sl_setVirtualGroupingMode(const QList<QStringList> &)));
    disconnect(treeViewerUI, SIGNAL(si_groupColorsChanged(const GroupColorSchema &)), msaUI->getEditorNameList(), SLOT(sl_onGroupColorsChanged(const GroupColorSchema &)));

    disconnect(editor, SIGNAL(si_referenceSeqChanged(qint64)), treeViewerUI, SLOT(sl_onReferenceSeqChanged(qint64)));
    disconnect(msaUI->getSequenceArea(), SIGNAL(si_visibleRangeChanged(QStringList, int)), treeViewerUI, SLOT(sl_onVisibleRangeChanged(QStringList, int)));
    disconnect(msaUI->getSequenceArea(), SIGNAL(si_selectionChanged(const QStringList &)), treeViewerUI, SLOT(sl_selectionChanged(const QStringList &)));
    disconnect(msaUI->getEditorNameList(), SIGNAL(si_sequenceNameChanged(QString, QString)), treeViewerUI, SLOT(sl_sequenceNameChanged(QString, QString)));

    slotsAreConnected = false;
}

void MSAEditorTreeViewer::sl_startTracking(bool changed) {
    CHECK(editor != nullptr, );
    MsaEditorWgt *msaUI = editor->getUI();

    CHECK(msaUI != nullptr, );
    disconnect(msaUI, SIGNAL(si_stopMaChanging(bool)), this, SLOT(sl_startTracking(bool)));

    if (!changed) {
        // alignment wasn't changed, the synchronization can remain
        bool ok = sync();
        if (!ok) {
            desync();
        }
        SAFE_POINT(ok, "Cannot synchronize the tree with the alignment", );
        return;
    }
    QObjectScopedPointer<QMessageBox> desyncQuestion = new QMessageBox(QMessageBox::Question,
                                                                       tr("Alignment Modification Confirmation"),
                                                                       tr("The alignment has been modified.\n\n"
                                                                          "All phylogenetic tree(s), opened in the same view, "
                                                                          "will be no more synchronized with the alignment."));
    desyncQuestion->setInformativeText(tr("Do you want to confirm the modification?"));
    desyncQuestion->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    desyncQuestion->button(QMessageBox::No)->setText(tr("Cancel"));
    desyncQuestion->button(QMessageBox::Yes)->setText(tr("Confirm"));
    desyncQuestion->setDefaultButton(QMessageBox::No);
    desyncQuestion->setModal(true);

    int res = desyncQuestion->exec();
    if (res == QMessageBox::No) {
        // undo the change and synchronize
        disconnect(msaUI->editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), this, SLOT(sl_alignmentChanged(MultipleAlignment, MaModificationInfo)));

        if (cachedModification.type != MaModificationType_Undo) {
            if (!msaUI->getUndoAction()->isEnabled()) {
                desync();
                FAIL("Processing the alignment change, but undo-redo stack is empty!", );
            }
            msaUI->getUndoAction()->trigger();
        } else {
            if (!msaUI->getRedoAction()->isEnabled()) {
                desync();
                FAIL("Processing the alignment change, but undo-redo stack is empty!", );
            }
            msaUI->getRedoAction()->trigger();
        }
        bool ok = sync();
        if (!ok) {
            desync();
        }
        SAFE_POINT(ok, "Cannot synchronize the tree with the alignment", );
    } else {
        // break the connection completely
        desync();
    }
}

void MSAEditorTreeViewer::sl_stopTracking() {
    disconnectSignals();
}

void MSAEditorTreeViewer::sl_alignmentChanged(const MultipleAlignment & /*ma*/, const MaModificationInfo &modInfo) {
    cachedModification = modInfo;

    bool connectionIsNotBrokenOnAlignmentChange = slotsAreConnected && (modInfo.rowContentChanged || modInfo.rowListChanged || modInfo.alignmentLengthChanged);
    if (connectionIsNotBrokenOnAlignmentChange) {
        // alignment was modified by undo-redo or outside of current msa editor
        MWMDIManager *mdiManager = AppContext::getMainWindow()->getMDIManager();
        SAFE_POINT(mdiManager != nullptr, "MWMDIManager is NULL", );
        auto objectViewWindow = qobject_cast<GObjectViewWindow *>(mdiManager->getActiveWindow());
        if (objectViewWindow != nullptr) {
            if (objectViewWindow->getObjectView() == editor) {
                // undo-redo at the same window
                disconnectSignals();
                sl_startTracking(true);
                return;
            }
        }

        // the change outside the current msa editor detected -- desync the tree
        CHECK(editor != nullptr, );
        MsaEditorWgt *msaUI = editor->getUI();

        CHECK(msaUI != nullptr, );
        disconnect(msaUI->editor->getMaObject(), SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)), this, SLOT(sl_alignmentChanged(MultipleAlignment, MaModificationInfo)));
        disconnect(msaUI, SIGNAL(si_stopMaChanging(bool)), this, SLOT(sl_startTracking(bool)));
        desync();
    }
}

//---------------------------------------------
// MSAEditorTreeViewerUI
//---------------------------------------------
MSAEditorTreeViewerUI::MSAEditorTreeViewerUI(MSAEditorTreeViewer *treeViewer)
    : TreeViewerUI(treeViewer), subgroupSelectorPos(0.0), groupColors(1, 0.86), isRectangularLayout(true),
      curMSATreeViewer(treeViewer), syncMode(WithoutSynchronization), hasMinSize(false), hasMaxSize(false) {
    connect(scene(), SIGNAL(sceneRectChanged(const QRectF &)), SLOT(sl_onSceneRectChanged(const QRectF &)));

    QRectF rect = scene()->sceneRect();
    subgroupSelector = scene()->addLine(0.0, rect.bottom(), 0.0, rect.top(), QPen(QColor(103, 138, 186), 0));

    setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

void MSAEditorTreeViewerUI::sl_zoomToAll() {
    emit si_resetZooming();
}

void MSAEditorTreeViewerUI::sl_zoomToSel() {
    emit si_zoomIn();
}

void MSAEditorTreeViewerUI::sl_zoomOut() {
    emit si_zoomOut();
}

ColorGenerator::ColorGenerator(int _countOfColors, qreal _lightness)
    : countOfColors(_countOfColors), delta(0.1), hue(0.0), lightness(_lightness) {
    satValues << 0.4 << 0.8 << 0.6 << 1.0;
    SAFE_POINT(lightness >= 0 && lightness <= 1.0, "ColorGenerator::ColorGenerator(int _countOfColors, qreal _lightness) _lightness must be in range (0.0, 1.0)", );
    CHECK(0 < countOfColors, );
    generateColors();
}

void ColorGenerator::setCountOfColors(int counts) {
    CHECK(0 < countOfColors, );
    countOfColors = counts;
    delta = (1.0 / countOfColors);
    generateColors();
}

QColor ColorGenerator::getColor(int index) const {
    if (index >= 0 && index < colors.size()) {
        return colors.at(index);
    } else {
        return Qt::white;
    }
}
void ColorGenerator::generateColors() {
    srand(QDateTime::currentDateTime().toTime_t());
    int countOfAddedColors = countOfColors - colors.size();
    for (int i = 0; i < countOfAddedColors; i++) {
        QColor color;
        qreal saturation = satValues.at(i % satValues.size());
        color.setHslF(hue, saturation, lightness);
        int size = colors.size();
        if (size > 2) {
            colors.insert(qrand() % size, color);
        } else {
            colors.append(color);
        }
        hue += delta;
        hue = (hue > 1) ? hue - 1 : hue;
    }
}

void MSAEditorTreeViewerUI::mousePressEvent(QMouseEvent *e) {
    bool isLeftButton = e->button() == Qt::LeftButton;
    if (isLeftButton && abs(mapFromScene(subgroupSelectorPos, 0.0).x() - e->pos().x()) < 5) {
        subgroupSelectionMode = true;
    } else {
        TreeViewerUI::mousePressEvent(e);
    }
}

void MSAEditorTreeViewerUI::mouseReleaseEvent(QMouseEvent *e) {
    subgroupSelectionMode = false;
    TreeViewerUI::mouseReleaseEvent(e);
}

void MSAEditorTreeViewerUI::wheelEvent(QWheelEvent *we) {
    if (!isRectangularLayout || !curMSATreeViewer->isSynchronized()) {
        TreeViewerUI::wheelEvent(we);
        return;
    }
    bool toMin = we->delta() > 0;
    QScrollBar *hScrollBar = horizontalScrollBar();
    if (hScrollBar != nullptr) {
        hScrollBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    }
    we->accept();
}

void MSAEditorTreeViewerUI::mouseMoveEvent(QMouseEvent *me) {
    if (!subgroupSelector->isVisible()) {
        TreeViewerUI::mouseMoveEvent(me);
        return;
    }
    QRectF boundingRect = scene()->itemsBoundingRect();
    QPointF sceneMousePos = mapToScene(me->pos());
    if (abs(mapFromScene(subgroupSelectorPos, 0.0).x() - me->pos().x()) < 5 && boundingRect.contains(sceneMousePos)) {
        setCursor(Qt::SplitHCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    if (subgroupSelectionMode) {
        qreal xPos = sceneMousePos.x();

        if (boundingRect.contains(sceneMousePos)) {
            subgroupSelectorPos = mapToScene(me->pos()).x();
        } else {
            if (xPos < boundingRect.left()) {
                subgroupSelectorPos = boundingRect.left() + 1;
            }
            if (xPos > boundingRect.right()) {
                subgroupSelectorPos = boundingRect.right() - 1;
            }
        }
        QRectF rect = scene()->sceneRect();
        subgroupSelector->setLine(subgroupSelectorPos, rect.bottom(), subgroupSelectorPos, rect.top());
        highlightBranches();
        //scene()->update();
    } else {
        TreeViewerUI::mouseMoveEvent(me);
    }
    me->accept();
}

void MSAEditorTreeViewerUI::sl_onSceneRectChanged(const QRectF &) {
    QRectF rect = scene()->sceneRect();
    subgroupSelector->setLine(subgroupSelectorPos, rect.bottom(), subgroupSelectorPos, rect.top());
}

void MSAEditorTreeViewerUI::sl_selectionChanged(const QStringList &selectedSequenceNameList) {
    bool cleanSelection = true;
    QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : qAsConst(items)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        if (selectedSequenceNameList.contains(nameItem->text(), Qt::CaseInsensitive)) {
            if (cleanSelection) {
                cleanSelection = false;
                getRoot()->setSelectedRecurs(false, true);
            }
            branchItem->setSelectedRecurs(true, false);
        } else {
            branchItem->setSelectedRecurs(false, false);
        }
    }
}

bool MSAEditorTreeViewerUI::canSynchronizeWithMSA(MSAEditor *msa) {
    if (!isRectangularLayout) {
        return false;
    }
    QStringList seqsNames = msa->getMaObject()->getMultipleAlignment()->getRowNames();
    QList<QGraphicsItem *> items = scene()->items();

    int counter = 0;
    for (QGraphicsItem *item : qAsConst(items)) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        QString nodeName = QString(nameItem->text());
        if (!seqsNames.contains(nodeName)) {
            return false;
        }
        counter++;
    }
    if (counter != seqsNames.size()) {
        return false;
    }
    return true;
}

void MSAEditorTreeViewerUI::sl_sequenceNameChanged(QString prevName, QString newName) {
    QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : qAsConst(items)) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        if (prevName == nameItem->text()) {
            nameItem->setText(newName);
        }
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::setTreeLayout(TreeLayout newLayout) {
    TreeViewerUI::setTreeLayout(newLayout);
}

void MSAEditorTreeViewerUI::onLayoutChanged(const TreeLayout &layout) {
    if (layout == RECTANGULAR_LAYOUT && !isRectangularLayout) {
        setTransform(rectangularTransform);
    }
    isRectangularLayout = (RECTANGULAR_LAYOUT == layout);
    curMSATreeViewer->getSortSeqsAction()->setEnabled(false);
    if (isRectangularLayout) {
        subgroupSelector->show();
        if (syncMode == FullSynchronization) {
            curMSATreeViewer->getSortSeqsAction()->setEnabled(true);
            MSAEditor *msa = curMSATreeViewer->getMsaEditor();
            CHECK(msa != nullptr, );
            msa->getUI()->getSequenceArea()->onVisibleRangeChanged();
        }
    } else {
        subgroupSelector->hide();
    }
}

void MSAEditorTreeViewerUI::sl_onReferenceSeqChanged(qint64) {
    QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : qAsConst(items)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        //TODO: heh?
        QPen brush(Qt::white);
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::sl_sortAlignment() {
    // Apply current tree order & collapsing to the alignment.
    QList<QStringList> groupList = getGroupingStateForMsa(rectRoot);
    emit si_collapseModelChangedInTree(groupList);
}

void MSAEditorTreeViewerUI::highlightBranches() {
    OptionsMap rootSettings = rectRoot->getSettings();
    rootSettings[BRANCH_COLOR] = static_cast<int>(Qt::black);
    if (rectRoot) {
        rectRoot->updateSettings(rootSettings);
        rectRoot->updateChildSettings(rootSettings);
    }

    QStack<GraphicsRectangularBranchItem *> graphicsItemList;
    QList<GraphicsRectangularBranchItem *> groupRootItemList;
    QList<GraphicsRectangularBranchItem *> collapsedRootItemList;
    graphicsItemList.push(getRectRoot());

    int countOfListNodes = getBranchItemsWithNames().size();
    if (groupColors.getCountOfColors() < countOfListNodes) {
        groupColors.setCountOfColors(countOfListNodes);
    }
    do {
        GraphicsRectangularBranchItem *node = graphicsItemList.pop();
        if (!node->isVisible()) {
            continue;
        }
        qreal node1Pos = node->sceneBoundingRect().left();
        qreal node2Pos = node->sceneBoundingRect().right();
        if (node->isCollapsed() && node2Pos < subgroupSelectorPos && node1Pos < subgroupSelectorPos) {
            collapsedRootItemList.append(node);
            continue;
        }
        if (node2Pos > subgroupSelectorPos && node1Pos < subgroupSelectorPos && node->getNameText() == NULL) {
            groupRootItemList.append(node);
            continue;
        }
        const QList<QGraphicsItem *> childItemList = node->childItems();
        for (QGraphicsItem *curItem : qAsConst(childItemList)) {
            auto branchItem = dynamic_cast<GraphicsRectangularBranchItem *>(curItem);
            if (branchItem == nullptr) {
                continue;
            }
            graphicsItemList.append(branchItem);
        }
    } while (!graphicsItemList.isEmpty());

    if (groupRootItemList.size() <= 1) {
        emit si_groupColorsChanged(GroupColorSchema());
        return;
    }
    groupRootItemList << collapsedRootItemList;

    int colorIndex = 0;
    QMap<PhyNode *, QColor> colorSchema;

    for (GraphicsRectangularBranchItem *branchItem : qAsConst(groupRootItemList)) {
        PhyNode *secondNode = NULL;
        if (branchItem->getPhyBranch() != nullptr) {
            secondNode = branchItem->getPhyBranch()->node2;
        } else {
            continue;
        }
        if (colorSchema.contains(secondNode)) {
            OptionsMap settings = branchItem->getSettings();
            settings[BRANCH_COLOR] = colorSchema[secondNode];
            branchItem->updateSettings(settings);
            branchItem->updateChildSettings(settings);
        } else {
            colorSchema[secondNode] = groupColors.getColor(colorIndex);
            OptionsMap settings = branchItem->getSettings();
            settings[BRANCH_COLOR] = colorSchema[secondNode];
            branchItem->updateSettings(settings);
            branchItem->updateChildSettings(settings);
            colorIndex++;
        }
    }

    GroupColorSchema groupColorSchema;
    QList<QGraphicsItem *> sceneItemList = scene()->items();
    for (QGraphicsItem *item : qAsConst(sceneItemList)) {
        auto branchItem = dynamic_cast<GraphicsRectangularBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem == nullptr) {
            continue;
        }
        QString name = nameItem->text();
        groupColorSchema[nameItem->text()] = qvariant_cast<QColor>(branchItem->getSettings()[BRANCH_COLOR]);
    }
    emit si_groupColorsChanged(groupColorSchema);
}

void MSAEditorTreeViewerUI::resizeEvent(QResizeEvent *e) {
    rectangularTransform = transform();
    QGraphicsView::resizeEvent(e);
    e->accept();
}

void MSAEditorTreeViewerUI::setSynchronizeMode(SynchronizationMode newSyncMode) {
    syncMode = newSyncMode;
    bool isFullSync = syncMode == FullSynchronization;
    QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : qAsConst(items)) {
        auto buttonItem = dynamic_cast<GraphicsButtonItem *>(item);
        if (buttonItem != nullptr) {
            buttonItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        }
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem != nullptr) {
            nameItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, isFullSync);
        }
        QGraphicsSimpleTextItem *distanceTextItem = branchItem->getDistanceText();
        if (distanceTextItem != nullptr) {
            distanceTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, isFullSync);
        }
    }
    updateRect();
}

bool MSAEditorTreeViewerUI::isCurTreeViewerSynchronized() const {
    return curMSATreeViewer->isSynchronized();
}

QList<GraphicsBranchItem *> MSAEditorTreeViewerUI::getBranchItemsWithNames() const {
    QList<QGraphicsItem *> sceneItemList = scene()->items();
    QList<GraphicsBranchItem *> result;
    for (QGraphicsItem *item : qAsConst(sceneItemList)) {
        auto branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if (branchItem == nullptr) {
            continue;
        }
        QGraphicsSimpleTextItem *nameItem = branchItem->getNameText();
        if (nameItem != nullptr) {
            result.append(branchItem);
        }
    }
    return result;
}

void MSAEditorTreeViewerUI::onSettingsChanged(TreeViewOption option, const QVariant &newValue) {
    bool isSizeSynchronized = (syncMode == FullSynchronization && isRectangularLayout);
    if (!isSizeSynchronized) {
        TreeViewerUI::onSettingsChanged(option, newValue);
        return;
    }
    switch (option) {
        case WIDTH_COEF:
            updateTreeSettings(true);
            break;
        case HEIGHT_COEF:
            break;
        case BRANCHES_TRANSFORMATION_TYPE:
            updateTreeSettings(true);
            break;
        default:
            TreeViewerUI::onSettingsChanged(option, newValue);
            break;
    }
}

void MSAEditorTreeViewerUI::updateTreeSettings(bool setDefaultZoom) {
    TreeViewerUI::updateTreeSettings(setDefaultZoom);
    MSAEditor *msaEditor = curMSATreeViewer->getMsaEditor();
    CHECK(msaEditor != nullptr, );
    msaEditor->getUI()->getSequenceArea()->onVisibleRangeChanged();
    updateRect();
}

void MSAEditorTreeViewerUI::sl_rectLayoutRecomputed() {
    QMatrix curMatrix = matrix();
    TreeViewerUI::sl_rectLayoutRecomputed();
    if (syncMode == FullSynchronization) {
        curMSATreeViewer->sync();
    }
    setMatrix(curMatrix);
}

void MSAEditorTreeViewerUI::sl_onVisibleRangeChanged(QStringList visibleSeqs, int height) {
    SAFE_POINT(height > 0, QString("Argument 'height' in function 'MSAEditorTreeViewerUI::sl_onVisibleRangeChanged' less then 1"), );
    CHECK(isRectangularLayout, );
    QList<GraphicsBranchItem *> branchItemList = getBranchItemsWithNames();
    QRectF rect;
    setZoom(1.0, 1.0 / getVerticalZoom());
    for (GraphicsBranchItem *nodeItem : qAsConst(branchItemList)) {
        QGraphicsSimpleTextItem *nameText = nodeItem->getNameText();
        if (nameText == nullptr) {
            continue;
        }
        QGraphicsItem *parentItem = nodeItem->getParentItem();
        //Check that node is not collapsed
        if (parentItem == nullptr || !parentItem->isVisible()) {
            continue;
        }

        if (visibleSeqs.contains(nameText->text())) {
            rect = rect.isNull() ? nameText->sceneBoundingRect() : rect.united(nameText->sceneBoundingRect());
        }
    }
    CHECK(rect.height() > 0, );
    QRectF sceneRect = transform().mapRect(rect);

    qreal zoom = qreal(height) / sceneRect.height();
    centerOn(rect.center());
    setZoom(1.0, zoom);
}

void MSAEditorTreeViewerUI::sl_onBranchCollapsed(GraphicsRectangularBranchItem *branch) {
    TreeViewerUI::sl_onBranchCollapsed(branch);
    sl_sortAlignment();
}

QList<QStringList> MSAEditorTreeViewerUI::getGroupingStateForMsa(const GraphicsBranchItem *root) const {
    QList<QStringList> groupList;

    // treeBranchStack is used here for Depth-First-Search algorithm implementation with no recursion.
    QStack<const GraphicsBranchItem *> treeBranchStack;
    treeBranchStack.push(root);

    while (!treeBranchStack.isEmpty()) {
        const GraphicsBranchItem *branchItem = treeBranchStack.pop();
        if (branchItem->isCollapsed()) {
            groupList.append(MSAEditorTreeViewerUtils::getSeqsNamesInBranch(branchItem));
            continue;
        }

        QGraphicsSimpleTextItem *branchNameItem = branchItem->getNameText();
        if (branchNameItem != nullptr && !branchNameItem->text().isEmpty()) {
            // Add this leaf of as a separate non-grouped sequence to the list.
            groupList.append({branchNameItem->text()});
            continue;
        }

        QList<QGraphicsItem *> childItemList = branchItem->childItems();

        // Sort items by Y, so virtual order will be the same with the tree.
        std::sort(childItemList.begin(), childItemList.end(), [](QGraphicsItem *item1, QGraphicsItem *item2) {
            return item1->y() - item2->y();
        });

        for (QGraphicsItem *childItem : qAsConst(childItemList)) {
            auto childBranchItem = dynamic_cast<GraphicsBranchItem *>(childItem);
            if (childBranchItem == nullptr) {
                continue;
            }
            treeBranchStack.push(childBranchItem);
        }
    }

    return groupList;
}

QStringList MSAEditorTreeViewerUtils::getSeqsNamesInBranch(const GraphicsBranchItem *branch) {
    QStringList seqNames;
    QStack<const GraphicsBranchItem *> treeBranches;
    treeBranches.push(branch);

    do {
        const GraphicsBranchItem *parentBranch = treeBranches.pop();

        QList<QGraphicsItem *> childItemList = parentBranch->childItems();
        for (QGraphicsItem *graphItem : qAsConst(childItemList)) {
            auto childrenBranch = dynamic_cast<GraphicsBranchItem *>(graphItem);
            if (childrenBranch == nullptr) {
                continue;
            }
            QGraphicsSimpleTextItem *nameItem = childrenBranch->getNameText();
            if (nameItem == nullptr) {
                treeBranches.push(childrenBranch);
                continue;
            }

            QString seqName = nameItem->text();
            if (!seqName.isEmpty()) {
                seqNames.append(seqName);
                continue;
            }
            treeBranches.push(childrenBranch);
        }
    } while (!treeBranches.isEmpty());

    return seqNames;
}

}    // namespace U2
