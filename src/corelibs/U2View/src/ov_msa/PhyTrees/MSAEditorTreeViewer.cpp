/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include "MsaEditorTreeTabArea.h"

#include <U2Core/AppContext.h>
#include <U2Core/PhyTree.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/GraphicsRectangularBranchItem.h>
#include <U2View/GraphicsButtonItem.h>
#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>

#include <QCursor>
#include <QDateTime>
#include <QMessageBox>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStack>
#include <QVBoxLayout>
#include <QWheelEvent>

namespace U2 {

MSAEditorTreeViewer::MSAEditorTreeViewer(const QString& viewName, GObject* obj, GraphicsRectangularBranchItem* _root, qreal s)
    : TreeViewer(viewName, obj, _root, s),
      refreshTreeAction(NULL),
      sortSeqAction(NULL),
      msa(NULL),
      syncMode(WithoutSynchronization),
      slotsAreConnected(false)
{
}

QWidget* MSAEditorTreeViewer::createWidget() {
    SAFE_POINT(ui == NULL, QString("MSAEditorTreeViewer::createWidget error"), NULL);
    QWidget* view = new QWidget();
    view->setObjectName("msa_editor_tree_view_container_widget");
    QVBoxLayout* vLayout = new QVBoxLayout();
    ui = new MSAEditorTreeViewerUI(this);

    QToolBar* toolBar = new QToolBar(tr("MSAEditor tree toolbar"));
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

    vLayout->setSpacing(0);
    vLayout->setMargin(0);
    vLayout->addWidget(toolBar);
    vLayout->addWidget(ui);
    view->setLayout(vLayout);

    return view;
}

void MSAEditorTreeViewer::setMSAEditor(MSAEditor *_msa) {
    msa = _msa;
    if (NULL != msa) {
        connect(ui, SIGNAL(si_zoomIn()), msa, SLOT(sl_zoomIn()));
        connect(ui, SIGNAL(si_zoomOut()), msa, SLOT(sl_zoomOut()));
        connect(ui, SIGNAL(si_resetZooming()), msa, SLOT(sl_resetZoom()));
    }
}

MSAEditor * MSAEditorTreeViewer::getMsaEditor() const {
    return msa;
}

void MSAEditorTreeViewer::setCreatePhyTreeSettings(const CreatePhyTreeSettings& _buildSettings) {
    buildSettings = _buildSettings;
    refreshTreeAction->setEnabled(true);
}

void MSAEditorTreeViewer::sl_refreshTree() {
    emit si_refreshTree(this);
}

bool MSAEditorTreeViewer::sync() {
    CHECK(msa != NULL, false);
    MSAEditorTreeViewerUI* treeViewerUI = qobject_cast<MSAEditorTreeViewerUI*>(ui);
    CHECK(treeViewerUI != NULL, false);
    if (treeViewerUI->canSynchronizeWithMSA(msa)) {
        syncMode = FullSynchronization;
        treeViewerUI->setSynchronizeMode(syncMode);

        CHECK(msa != NULL, false);
        MsaEditorWgt* msaUI = msa->getUI();
        connect(msaUI->editor->getMaObject(),  SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)),
                this,                           SLOT(sl_alignmentChanged(MultipleAlignment,MaModificationInfo)));
        connect(msaUI,                          SIGNAL(si_stopMaChanging(bool)),
                this,                           SLOT(sl_startTracking(bool)));

        connectSignals();
        sortSeqAction->setEnabled(true);
        treeViewerUI->sl_sortAlignment();
        treeViewerUI->highlightBranches();
        return true;
    }
    return false;
}

void MSAEditorTreeViewer::desync() {
    disconnectSignals();

    CHECK(msa != NULL, );
    MsaEditorWgt* msaUI = msa->getUI();
    CHECK(msaUI != NULL, );
    msaUI->getEditorNameList()->clearGroupsSelections();
    msaUI->getEditorNameList()->update();

    syncMode = WithoutSynchronization;
    MSAEditorTreeViewerUI* treeViewerUI = qobject_cast<MSAEditorTreeViewerUI*>(ui);
    treeViewerUI->setSynchronizeMode(syncMode);
    sortSeqAction->setEnabled(false);
}

bool MSAEditorTreeViewer::isSynchronized() const {
    return syncMode == FullSynchronization;
}

void MSAEditorTreeViewer::connectSignals() {
    CHECK(slotsAreConnected == false, );
    CHECK(msa != NULL, );
    MsaEditorWgt* msaUI = msa->getUI();
    CHECK(msaUI != NULL, );
    MSAEditorTreeViewerUI* treeViewerUI = qobject_cast<MSAEditorTreeViewerUI*>(ui);
    CHECK(treeViewerUI != NULL, );

    connect(msaUI,                      SIGNAL(si_startMaChanging()),
            this,                       SLOT(sl_stopTracking()));

    connect(treeViewerUI,               SIGNAL(si_seqOrderChanged(const QStringList&)),
            msa,                        SLOT(sl_onSeqOrderChanged(const QStringList&)));
    connect(treeViewerUI,               SIGNAL(si_collapseModelChangedInTree(const QList<QStringList>&)),
            msaUI->getSequenceArea(),   SLOT(sl_setCollapsingRegions(const QList<QStringList>&)));
    connect(treeViewerUI,               SIGNAL(si_groupColorsChanged(const GroupColorSchema&)),
            msaUI->getEditorNameList(), SLOT(sl_onGroupColorsChanged(const GroupColorSchema&)));

    connect(msa,                        SIGNAL(si_referenceSeqChanged(qint64)),
            treeViewerUI,               SLOT(sl_onReferenceSeqChanged(qint64)));
    connect(msaUI->getSequenceArea(),   SIGNAL(si_visibleRangeChanged(QStringList, int)),
            treeViewerUI,               SLOT(sl_onVisibleRangeChanged(QStringList, int)));

    connect(msaUI->getSequenceArea(),   SIGNAL(si_selectionChanged(const QStringList&)),
            treeViewerUI,               SLOT(sl_selectionChanged(const QStringList&)));
    connect(msaUI->getEditorNameList(), SIGNAL(si_sequenceNameChanged(QString, QString)),
            treeViewerUI,               SLOT(sl_sequenceNameChanged(QString, QString)));

    slotsAreConnected = true;
}

void MSAEditorTreeViewer::disconnectSignals() {
    CHECK(slotsAreConnected == true, );
    CHECK(msa != NULL, );
    MsaEditorWgt* msaUI = msa->getUI();
    CHECK(msaUI != NULL, );
    MSAEditorTreeViewerUI* treeViewerUI = qobject_cast<MSAEditorTreeViewerUI*>(ui);
    CHECK(treeViewerUI != NULL, );

    disconnect(msaUI,                       SIGNAL(si_startMaChanging()),
               this,                        SLOT(sl_stopTracking()));

    disconnect(treeViewerUI,                SIGNAL(si_seqOrderChanged(const QStringList&)),
            msa,                            SLOT(sl_onSeqOrderChanged(const QStringList&)));
    disconnect(treeViewerUI,                SIGNAL(si_collapseModelChangedInTree(const QList<QStringList>&)),
            msaUI->getSequenceArea(),       SLOT(sl_setCollapsingRegions(const QList<QStringList>&)));
    disconnect(treeViewerUI,                SIGNAL(si_groupColorsChanged(const GroupColorSchema&)),
            msaUI->getEditorNameList(),     SLOT(sl_onGroupColorsChanged(const GroupColorSchema&)));

    disconnect(msa,                         SIGNAL(si_referenceSeqChanged(qint64)),
            treeViewerUI,                   SLOT(sl_onReferenceSeqChanged(qint64)));
    disconnect(msaUI->getSequenceArea(),    SIGNAL(si_visibleRangeChanged(QStringList, int)),
            treeViewerUI,                   SLOT(sl_onVisibleRangeChanged(QStringList, int)));
    disconnect(msaUI->getSequenceArea(),    SIGNAL(si_selectionChanged(const QStringList&)),
            treeViewerUI,                   SLOT(sl_selectionChanged(const QStringList&)));
    disconnect(msaUI->getEditorNameList(),  SIGNAL(si_sequenceNameChanged(QString, QString)),
            treeViewerUI,                   SLOT(sl_sequenceNameChanged(QString, QString)));

    slotsAreConnected = false;
}

void MSAEditorTreeViewer::sl_startTracking(bool changed) {
    CHECK(msa != NULL, );
    MsaEditorWgt* msaUI = msa->getUI();
    CHECK(msaUI != NULL, );
    disconnect(msaUI,   SIGNAL(si_stopMaChanging(bool)),
               this,    SLOT(sl_startTracking(bool)));

    if (changed) {
        QObjectScopedPointer<QMessageBox> desyncQuestion = new QMessageBox( QMessageBox::Question,
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
            disconnect(msaUI->editor->getMaObject(),   SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)),
                       this,                            SLOT(sl_alignmentChanged(MultipleAlignment,MaModificationInfo)));

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

    } else {
        // alignment wasn't changed, the synchronization can remain
        bool ok = sync();
        if (!ok) {
            desync();
        }
        SAFE_POINT(ok, "Cannot synchronize the tree with the alignment", );
    }
}

void MSAEditorTreeViewer::sl_stopTracking() {
    disconnectSignals();
}

void MSAEditorTreeViewer::sl_alignmentChanged(const MultipleAlignment &/*ma*/, const MaModificationInfo &modInfo) {
    cachedModification = modInfo;

    bool connectionIsNotBrokenOnAlignmentChange = slotsAreConnected && (modInfo.rowContentChanged || modInfo.rowListChanged || modInfo.alignmentLengthChanged);
    if (connectionIsNotBrokenOnAlignmentChange) {
        // alignment was modified by undo-redo or outside of current msa editor
        MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
        SAFE_POINT(mdiManager != NULL, "MWMDIManager is NULL", );
        GObjectViewWindow* win = qobject_cast<GObjectViewWindow*>(mdiManager->getActiveWindow());
        if (win != NULL) {
            if (win->getObjectView() == msa) {
                // undo-redo at the same window
                disconnectSignals();
                sl_startTracking(true);
                return;
            }
        }

        // the change outside the current msa editor detected -- desync the tree
        CHECK(msa != NULL, );
        MsaEditorWgt* msaUI = msa->getUI();
        CHECK(msaUI != NULL, );
        disconnect(msaUI->editor->getMaObject(),   SIGNAL(si_alignmentChanged(MultipleAlignment,MaModificationInfo)),
                   this,                            SLOT(sl_alignmentChanged(MultipleAlignment,MaModificationInfo)));
        disconnect(msaUI,                          SIGNAL(si_stopMaChanging(bool)),
                   this,                           SLOT(sl_startTracking(bool)));
        desync();
    }
}

//---------------------------------------------
// MSAEditorTreeViewerUI
//---------------------------------------------
MSAEditorTreeViewerUI::MSAEditorTreeViewerUI(MSAEditorTreeViewer* treeViewer)
    : TreeViewerUI(treeViewer), subgroupSelectorPos(0.0), groupColors(1, 0.86), curLayoutIsRectangular(true),
    curMSATreeViewer(treeViewer), syncMode(WithoutSynchronization), hasMinSize(false), hasMaxSize(false)
{
    connect(scene(), SIGNAL(sceneRectChanged(const QRectF&)), SLOT(sl_onSceneRectChanged(const QRectF&)));

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

ColorGenerator::ColorGenerator(int _countOfColors, qreal _lightness): countOfColors(_countOfColors), delta(0.1), hue(0.0), lightness(_lightness){
    satValues << 0.4 << 0.8 << 0.6 << 1.0;
    SAFE_POINT(lightness >= 0 && lightness <= 1.0, "ColorGenerator::ColorGenerator(int _countOfColors, qreal _lightness) _lightness must be in range (0.0, 1.0)",);
    CHECK(0 < countOfColors, );
    generateColors();
}
void ColorGenerator::setCountOfColors(int counts) {
    CHECK(0 < countOfColors, );
    countOfColors = counts;
    delta = (1.0 / countOfColors);
    generateColors();
}
QColor ColorGenerator::getColor(int index) {
    if(index >= 0 && index < colors.size()) {
        return colors.at(index);
    }
    else {
        return Qt::white;
    }
}
void ColorGenerator::generateColors() {
    srand(QDateTime::currentDateTime().toTime_t());
    int countOfAddedColors = countOfColors - colors.size();
    for(int i = 0; i < countOfAddedColors; i++) {
        QColor color;
        qreal saturation = satValues.at(i%satValues.size());
        color.setHslF(hue, saturation , lightness);
        int size = colors.size();
        if(size > 2) {
            colors.insert(qrand()%size, color);
        }
        else {
            colors.append(color);
        }
        hue += delta;
        hue = (hue > 1) ? hue - 1 : hue;
    }
}

void MSAEditorTreeViewerUI::mousePressEvent(QMouseEvent *e) {
    bool isLeftButton = e->button() == Qt::LeftButton;
    if(isLeftButton && abs(mapFromScene(subgroupSelectorPos, 0.0).x() - e->pos().x()) < 5) {
        subgroupSelectionMode = true;
    }
    else {
        TreeViewerUI::mousePressEvent(e);
    }
}

void MSAEditorTreeViewerUI::mouseReleaseEvent(QMouseEvent *e) {
    subgroupSelectionMode = false;
    TreeViewerUI::mouseReleaseEvent(e);
}

void MSAEditorTreeViewerUI::wheelEvent(QWheelEvent *we ) {
    if(!curLayoutIsRectangular || !curMSATreeViewer->isSynchronized()) {
        TreeViewerUI::wheelEvent(we);
        return;
    }
    bool toMin = we->delta() > 0;
    QScrollBar* hScrollBar = horizontalScrollBar();
    if(NULL != hScrollBar) {
        hScrollBar->triggerAction(toMin ? QAbstractSlider::SliderSingleStepSub : QAbstractSlider::SliderSingleStepAdd);
    }
    we->accept();
}

void MSAEditorTreeViewerUI::mouseMoveEvent(QMouseEvent *me) {
    if(!subgroupSelector->isVisible()) {
        TreeViewerUI::mouseMoveEvent(me);
        return;
    }
    QRectF boundingRect = scene()->itemsBoundingRect();
    QPointF sceneMousePos = mapToScene(me->pos());
    if(abs(mapFromScene(subgroupSelectorPos, 0.0).x() - me->pos().x()) < 5 && boundingRect.contains(sceneMousePos)) {
        setCursor(Qt::SplitHCursor);
    }
    else {
        setCursor(Qt::ArrowCursor);
    }
    if(true == subgroupSelectionMode) {
        qreal xPos = sceneMousePos.x();

        if(boundingRect.contains(sceneMousePos)) {
            subgroupSelectorPos = mapToScene(me->pos()).x();
        }
        else {
            if(xPos < boundingRect.left()) {
                subgroupSelectorPos = boundingRect.left() + 1;
            }
            if(xPos > boundingRect.right()) {
                subgroupSelectorPos = boundingRect.right() - 1;
            }
        }
        QRectF rect = scene()->sceneRect();
        subgroupSelector->setLine(subgroupSelectorPos, rect.bottom(), subgroupSelectorPos, rect.top());
        highlightBranches();
        //scene()->update();
    }
    else {
        TreeViewerUI::mouseMoveEvent(me);
    }
    me->accept();
}

void MSAEditorTreeViewerUI::sl_onSceneRectChanged( const QRectF& ) {
    QRectF rect = scene()->sceneRect();
    subgroupSelector->setLine(subgroupSelectorPos, rect.bottom(), subgroupSelectorPos, rect.top());
}

void MSAEditorTreeViewerUI::sl_selectionChanged(const QStringList& selection) {
    bool cleanSelection = true;
    QList<QGraphicsItem*> items = scene()->items();
    foreach(QGraphicsItem* item, items) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        QString str = QString(nameItem->text());
        foreach(const QString& seqname, selection) {
            bool res = seqname == str;
            res = res ? false : res;
        }
        if(selection.contains(nameItem->text(), Qt::CaseInsensitive)) {
            if(true == cleanSelection) {
                cleanSelection = false;
                getRoot()->setSelectedRecurs(false, true);
            }
            branchItem->setSelectedRecurs(true, false);
        }
        else
            branchItem->setSelectedRecurs(false, false);
    }
}

bool MSAEditorTreeViewerUI::canSynchronizeWithMSA(MSAEditor* msa) {
    if(!curLayoutIsRectangular) {
        return false;
    }
    QStringList seqsNames = msa->getMaObject()->getMultipleAlignment()->getRowNames();
    QList<QGraphicsItem*> items = scene()->items();

    int counter = 0;
    foreach(QGraphicsItem* item, items) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        QString nodeName = QString(nameItem->text());
        if(!seqsNames.contains(nodeName)) {
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
    QList<QGraphicsItem*> items = scene()->items();
    foreach(QGraphicsItem* item, items) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        if(prevName == nameItem->text()) {
            nameItem->setText(newName);
        }
    }
    scene()->update();
}

typedef QPair<qreal, QString> seqNameWithPos;

QStringList MSAEditorTreeViewerUI::getOrderedSeqNames() {
    QList<QGraphicsItem*> items = scene()->items();
    QList<seqNameWithPos> namesAndHeights;

    foreach(QGraphicsItem* item, items) {
        GraphicsRectangularBranchItem *branchItem = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        qreal y = branchItem->scenePos().y();
        QString str = nameItem->text();
        namesAndHeights.append(QPair<qreal, QString>(y, str));
    }
    qSort(namesAndHeights.begin(), namesAndHeights.end());

    QStringList seqNames;
    foreach(seqNameWithPos pair, namesAndHeights) {
        seqNames.append(pair.second);
    }

    getTreeSize();

    return seqNames;
}
U2Region MSAEditorTreeViewerUI::getTreeSize() {
    QList<QGraphicsItem*> items = scene()->items();

    QRectF sceneRect = scene()->sceneRect();
    qreal minYPos = sceneRect.top();
    qreal maxYPos = sceneRect.bottom();

    foreach(QGraphicsItem* item, items) {
        GraphicsRectangularBranchItem *branchItem = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        qreal y = branchItem->scenePos().y();
        minYPos = qMin(minYPos, y);
        maxYPos = qMax(maxYPos, y);
    }

    int minH = mapFromScene(0, minYPos).y();
    int maxH = mapFromScene(0, maxYPos).y();

    return U2Region(minH, maxH - minH);
}

void MSAEditorTreeViewerUI::setTreeLayout(TreeLayout newLayout) {
    TreeViewerUI::setTreeLayout(newLayout);
}

void MSAEditorTreeViewerUI::onLayoutChanged(const TreeLayout& layout) {
    if (layout == RECTANGULAR_LAYOUT && !curLayoutIsRectangular) {
        setTransform(rectangularTransform);
    }
    curLayoutIsRectangular = (RECTANGULAR_LAYOUT == layout);
    curMSATreeViewer->getSortSeqsAction()->setEnabled(false);
    if(curLayoutIsRectangular) {
        subgroupSelector->show();
        if (syncMode == FullSynchronization) {
            curMSATreeViewer->getSortSeqsAction()->setEnabled(true);
            MSAEditor* msa = curMSATreeViewer->getMsaEditor();
            CHECK(NULL != msa, );
            msa->getUI()->getSequenceArea()->onVisibleRangeChanged();
        }
    }
    else {
        subgroupSelector->hide();
    }
}

void MSAEditorTreeViewerUI::sl_onReferenceSeqChanged(qint64) {
    QList<QGraphicsItem*> items = scene()->items();
    foreach(QGraphicsItem* item, items) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem *>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
            continue;
        }
        QPen brush(Qt::white);
    }
    scene()->update();
}

void MSAEditorTreeViewerUI::sl_sortAlignment() {
    emit si_seqOrderChanged(getOrderedSeqNames());
}

void MSAEditorTreeViewerUI::highlightBranches() {
    OptionsMap settings = rectRoot->getSettings();
    settings[BRANCH_COLOR] = static_cast<int>(Qt::black);
    if(rectRoot) {
        rectRoot->updateSettings(settings);
        rectRoot->updateChildSettings(settings);
    }

    QStack<GraphicsRectangularBranchItem*> graphicsItems;
    QList<GraphicsRectangularBranchItem*> groupRoots;
    QList<GraphicsRectangularBranchItem*> collaspedRoots;
    graphicsItems.push(getRectRoot());

    int countOfListNodes = getListNodesOfTree().size();
    if(groupColors.getCountOfColors() < countOfListNodes) {
        groupColors.setCountOfColors(countOfListNodes);
    }
    do {
        GraphicsRectangularBranchItem* node = graphicsItems.pop();
        if(!node->isVisible()) {
            continue;
        }
        qreal node1Pos = node->sceneBoundingRect().left();
        qreal node2Pos = node->sceneBoundingRect().right();
        if (node->isCollapsed() && node2Pos < subgroupSelectorPos && node1Pos < subgroupSelectorPos) {
            collaspedRoots.append(node);
            continue;
        }
        if(node2Pos > subgroupSelectorPos && node1Pos < subgroupSelectorPos && node->getNameText() == NULL) {
            groupRoots.append(node);
            continue;
        }
        const QList<QGraphicsItem*>& childs = node->childItems();
        foreach(QGraphicsItem* curItem, childs) {
            GraphicsRectangularBranchItem *branchItem = dynamic_cast<GraphicsRectangularBranchItem*>(curItem);
            if(NULL == branchItem) {
                continue;
            }
            graphicsItems.append(branchItem);
        }
    } while(!graphicsItems.isEmpty());

    if (groupRoots.isEmpty() || groupRoots.size() == 1) {
        emit si_groupColorsChanged(GroupColorSchema());
        return;
    }
    groupRoots << collaspedRoots;

    int colorIndex = 0;
    QMap<PhyNode*, QColor> colorSchema;

    foreach(GraphicsRectangularBranchItem* branchItem, groupRoots) {
        PhyNode* secondNode = NULL;
        if(NULL != branchItem->getPhyBranch()) {
            secondNode = branchItem->getPhyBranch()->node2;
        }
        else {
            continue;
        }
        if(colorSchema.contains(secondNode)) {
            OptionsMap settings = branchItem->getSettings();
            settings[BRANCH_COLOR] = colorSchema[secondNode];
            branchItem->updateSettings(settings);
            branchItem->updateChildSettings(settings);
        }
        else {
            colorSchema[secondNode] = groupColors.getColor(colorIndex);
            OptionsMap settings = branchItem->getSettings();
            settings[BRANCH_COLOR] = colorSchema[secondNode];
            branchItem->updateSettings(settings);
            branchItem->updateChildSettings(settings);
            colorIndex++;
        }
    }

    GroupColorSchema groupColorSchema;
    QList<QGraphicsItem*> items = scene()->items();
    foreach(QGraphicsItem* item, items) {
        GraphicsRectangularBranchItem *branchItem = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL == nameItem) {
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
    bool fullSync = (FullSynchronization == syncMode);
    QList<QGraphicsItem*> items = scene()->items();
    foreach(QGraphicsItem* item, items) {
        GraphicsButtonItem* buttonItem = dynamic_cast<GraphicsButtonItem*>(item);
        if(NULL != buttonItem) {
            buttonItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        }
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL != nameItem) {
            nameItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, fullSync);
        }
        QGraphicsSimpleTextItem* distanceText = branchItem->getDistanceText();
        if(NULL != distanceText) {
            distanceText->setFlag(QGraphicsItem::ItemIgnoresTransformations, fullSync);
        }
    }
    updateRect();
}

bool MSAEditorTreeViewerUI::isCurTreeViewerSynchronized() const {
    return curMSATreeViewer->isSynchronized();
}

QList<GraphicsBranchItem*> MSAEditorTreeViewerUI::getListNodesOfTree() {
    QList<QGraphicsItem*> items = scene()->items();
    QList<GraphicsBranchItem*> result;
    foreach(QGraphicsItem* item, items) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(item);
        if(NULL == branchItem) {
            continue;
        }
        QGraphicsSimpleTextItem* nameItem = branchItem->getNameText();
        if(NULL != nameItem) {
            result.append(branchItem);
        }
    }
    return result;
}

void MSAEditorTreeViewerUI::onSettingsChanged(TreeViewOption option, const QVariant& newValue) {
    bool isSizeSynchronized = (syncMode == FullSynchronization && curLayoutIsRectangular);
    if(!isSizeSynchronized) {
        TreeViewerUI::onSettingsChanged(option, newValue);
        return;
    }
    switch(option) {
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
    MSAEditor* msa = curMSATreeViewer->getMsaEditor();
    CHECK(NULL != msa, );
    msa->getUI()->getSequenceArea()->onVisibleRangeChanged();
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
    SAFE_POINT(height > 0, QString("Argument 'height' in function 'MSAEditorTreeViewerUI::sl_onVisibleRangeChanged' less then 1"),);
    CHECK(curLayoutIsRectangular,);
    QList<GraphicsBranchItem*> items = getListNodesOfTree();
    QRectF rect;
    zooming(1.0, 1.0/getVerticalZoom());
    foreach(GraphicsBranchItem* item, items) {
        QGraphicsSimpleTextItem* nameText = item->getNameText();
        if(NULL == nameText) {
            continue;
        }
        QGraphicsItem* parentItem = item->getParentItem();
        //Check that node is not collapsed
        if(NULL == parentItem || !parentItem->isVisible()) {
            continue;
        }

        if(visibleSeqs.contains(nameText->text())) {
            if(rect.isNull()) {
                rect = nameText->sceneBoundingRect();
            }
            else {
                rect = rect.united(nameText->sceneBoundingRect());
            }
        }
    }
    CHECK(rect.height() > 0, );
    QRectF sceneRect = transform().mapRect(rect);

    qreal zoom = qreal(height) / sceneRect.height();
    centerOn(rect.center());
    zooming(1.0, zoom);
}

void MSAEditorTreeViewerUI::sl_onBranchCollapsed(GraphicsRectangularBranchItem* branch) {
    TreeViewerUI::sl_onBranchCollapsed(branch);
    QList<QStringList> collapsedGroups = MSAEditorTreeViewerUtils::getCollapsedGroups(rectRoot);
    emit si_collapseModelChangedInTree(collapsedGroups);
}


QList<QStringList> MSAEditorTreeViewerUtils ::getCollapsedGroups(const GraphicsBranchItem* root) {
    QList<QStringList> result;

    QStack<const GraphicsBranchItem*> treeBranches;
    treeBranches.push(root);

    do {
        const GraphicsBranchItem* parentBranch = treeBranches.pop();

        if(parentBranch->isCollapsed()) {
            result.append(getSeqsNamesInBranch(parentBranch));
            continue;
        }

        QList<QGraphicsItem*> childs = parentBranch ->childItems();

        foreach(QGraphicsItem* graphItem, childs) {
            GraphicsBranchItem* childrenBranch = dynamic_cast<GraphicsBranchItem*>(graphItem);
            if(NULL == childrenBranch) {
                continue;
            }
            treeBranches.push(childrenBranch);
        }
    } while(!treeBranches.isEmpty());

    return result;
}

QStringList MSAEditorTreeViewerUtils::getSeqsNamesInBranch(const GraphicsBranchItem* branch) {
    QStringList seqNames;
    QStack<const GraphicsBranchItem*> treeBranches;
    treeBranches.push(branch);

    do {
        const GraphicsBranchItem* parentBranch = treeBranches.pop();

        QList<QGraphicsItem*> childs = parentBranch ->childItems();

        foreach(QGraphicsItem* graphItem, childs) {
            GraphicsBranchItem* childrenBranch = dynamic_cast<GraphicsBranchItem*>(graphItem);
            if(NULL == childrenBranch) {
                continue;
            }
            QGraphicsSimpleTextItem* nameItem = childrenBranch ->getNameText();
            if(NULL == nameItem) {
                treeBranches.push(childrenBranch);
                continue;
            }

            QString seqName = nameItem->text();
            if(!seqName.isEmpty()) {
                seqNames.append(seqName);
                continue;
            }
            treeBranches.push(childrenBranch);
        }
    } while(!treeBranches.isEmpty());

    return seqNames;
}


}//namespace

