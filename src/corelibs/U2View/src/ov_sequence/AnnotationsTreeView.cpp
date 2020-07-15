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

#include "AnnotationsTreeView.h"

#include <QApplication>
#include <QClipboard>
#include <QDrag>
#include <QFileInfo>
#include <QHeaderView>
#include <QLineEdit>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QStack>
#include <QToolTip>
#include <QVBoxLayout>

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/ClipboardController.h>
#include <U2Core/DBXRefRegistry.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/SignalBlocker.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationDialog.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/EditQualifierDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/TreeWidgetUtils.h>

#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "AnnotatedDNAView.h"
#include "AutoAnnotationUtils.h"

namespace U2 {

/************************************************************************/
/* TreeSorter */
/************************************************************************/
class TreeSorter {
public:
    TreeSorter(AnnotationsTreeView *t)
        : w(t) {
        w->setSortingEnabled(false);
    }
    virtual ~TreeSorter() {
        w->setSortingEnabled(true);
    }

private:
    AnnotationsTreeView *w;
};

/************************************************************************/
/* AnnotationsTreeWidget */
/************************************************************************/
AnnotationsTreeWidget::AnnotationsTreeWidget(QWidget *parent)
    : QTreeWidget(parent) {
}

QTreeWidgetItem *AnnotationsTreeWidget::itemFromIndex(const QModelIndex &index) const {
    return QTreeWidget::itemFromIndex(index);
}

/************************************************************************/
/* AnnotationsTreeView */
/************************************************************************/
#define SETTINGS_ROOT QString("view_adv/annotations_tree_view/")
#define COLUMN_SIZES QString("columnSizes")

const int AnnotationsTreeView::COLUMN_NAME = 0;
const int AnnotationsTreeView::COLUMN_TYPE = 1;
const int AnnotationsTreeView::COLUMN_VALUE = 2;

const QString AnnotationsTreeView::annotationMimeType = "application/x-annotations-and-groups";
AVGroupItem *AnnotationsTreeView::dropDestination = NULL;
QList<Annotation *> AnnotationsTreeView::dndAdded = QList<Annotation *>();

AnnotationsTreeView::AnnotationsTreeView(AnnotatedDNAView *_ctx)
    : ctx(_ctx), dndHit(0) {
    lastMB = Qt::NoButton;
    lastClickedColumn = 0;

    tree = new AnnotationsTreeWidget(this);
    tree->setObjectName("annotations_tree_widget");

    tree->setSortingEnabled(true);
    tree->sortItems(0, Qt::AscendingOrder);

    tree->setColumnCount(3);
    headerLabels << tr("Name") << tr("Type") << tr("Value");

    tree->setHeaderLabels(headerLabels);
    tree->setUniformRowHeights(true);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->viewport()->installEventFilter(this);
    tree->setMouseTracking(true);
    tree->setAutoScroll(true);

    connect(tree, SIGNAL(itemEntered(QTreeWidgetItem *, int)), SLOT(sl_itemEntered(QTreeWidgetItem *, int)));
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(sl_itemClicked(QTreeWidgetItem *, int)));
    connect(tree, SIGNAL(itemPressed(QTreeWidgetItem *, int)), SLOT(sl_itemPressed(QTreeWidgetItem *)));
    connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), SLOT(sl_itemDoubleClicked(QTreeWidgetItem *, int)));
    connect(tree, SIGNAL(itemExpanded(QTreeWidgetItem *)), SLOT(sl_itemExpanded(QTreeWidgetItem *)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(tree);
    setLayout(layout);

    restoreWidgetState();

    connect(ctx, SIGNAL(si_buildPopupMenu(GObjectView *, QMenu *)), SLOT(sl_onBuildPopupMenu(GObjectView *, QMenu *)));
    connect(ctx, SIGNAL(si_annotationObjectAdded(AnnotationTableObject *)), SLOT(sl_onAnnotationObjectAdded(AnnotationTableObject *)));
    connect(ctx, SIGNAL(si_annotationObjectRemoved(AnnotationTableObject *)), SLOT(sl_onAnnotationObjectRemoved(AnnotationTableObject *)));
    connect(ctx, SIGNAL(si_sequenceAdded(ADVSequenceObjectContext *)), SLOT(sl_sequenceAdded(ADVSequenceObjectContext *)));
    connect(ctx, SIGNAL(si_sequenceRemoved(ADVSequenceObjectContext *)), SLOT(sl_sequenceRemoved(ADVSequenceObjectContext *)));
    QList<AnnotationTableObject *> aObjs = ctx->getAnnotationObjects(true);
    foreach (AnnotationTableObject *obj, aObjs) {
        sl_onAnnotationObjectAdded(obj);
    }
    const QList<ADVSequenceObjectContext *> seqContexts = ctx->getSequenceContexts();
    foreach (ADVSequenceObjectContext *context, seqContexts) {
        connectSequenceObjectContext(context);
    }
    connectAnnotationSelection();
    connectAnnotationGroupSelection();
    connect(tree, SIGNAL(itemSelectionChanged()), SLOT(sl_onItemSelectionChanged()));

    connect(AppContext::getAnnotationsSettingsRegistry(), SIGNAL(si_annotationSettingsChanged(const QStringList &)), SLOT(sl_onAnnotationSettingsChanged(const QStringList &)));

#define SORT_INTERVAL 500
    sortTimer.setInterval(SORT_INTERVAL);
    sortTimer.setSingleShot(true);
    connect(&sortTimer, SIGNAL(timeout()), SLOT(sl_sortTree()));

    addColumnIcon = QIcon(":core/images/add_column.png");
    removeColumnIcon = QIcon(":core/images/remove_column.png");

    /*
    pasteAction = new QAction(QIcon(":/core/images/paste.png"), tr("Paste annotations"), this);
    pasteAction->setObjectName("Paste annotations");
    pasteAction->setShortcut(QKeySequence(Qt::SHIFT| Qt::Key_V | Qt::ControlModifier));
    pasteAction->setShortcutContext(Qt::WidgetShortcut);
    connect(pasteAction, SIGNAL(triggered()), SLOT(sl_paste()));
    tree->addAction(pasteAction);
    */

    addAnnotationObjectAction = new QAction(tr("Objects with annotations..."), this);
    connect(addAnnotationObjectAction, SIGNAL(triggered()), SLOT(sl_onAddAnnotationObjectToView()));

    removeObjectsFromViewAction = new QAction(tr("Selected objects with annotations from view"), this);
    removeObjectsFromViewAction->setObjectName("Selected objects with annotations from view");
    removeObjectsFromViewAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Delete));
    removeObjectsFromViewAction->setShortcutContext(Qt::WidgetShortcut);
    connect(removeObjectsFromViewAction, SIGNAL(triggered()), SLOT(sl_removeObjectFromView()));
    tree->addAction(removeObjectsFromViewAction);

    removeAnnsAndQsAction = new QAction(tr("Selected annotations and qualifiers"), this);
    removeAnnsAndQsAction->setObjectName("Selected annotations and qualifiers");
    //hotkeys are set in AnnotatedDNAView::createWidget()
    connect(removeAnnsAndQsAction, SIGNAL(triggered()), SLOT(sl_removeAnnsAndQs()));
    tree->addAction(removeAnnsAndQsAction);

    copyQualifierURLAction = new QAction(tr("Copy qualifier URL"), this);
    connect(copyQualifierURLAction, SIGNAL(triggered()), SLOT(sl_onCopyQualifierURL()));

    toggleQualifierColumnAction = new QAction(tr("Toggle column"), this);
    toggleQualifierColumnAction->setObjectName("toggle_column");
    connect(toggleQualifierColumnAction, SIGNAL(triggered()), SLOT(sl_onToggleQualifierColumn()));

    removeColumnByHeaderClickAction = new QAction(tr("Hide column"), this);
    removeColumnByHeaderClickAction->setIcon(removeColumnIcon);
    connect(removeColumnByHeaderClickAction, SIGNAL(triggered()), SLOT(sl_onRemoveColumnByHeaderClick()));

    searchQualifierAction = new QAction(tr("Find qualifier..."), this);
    searchQualifierAction->setObjectName("find_qualifier_action");
    searchQualifierAction->setIcon(QIcon(":core/images/zoom_whole.png"));
    connect(searchQualifierAction, SIGNAL(triggered()), SLOT(sl_searchQualifier()));

    invertAnnotationSelectionAction = new QAction(tr("Invert annotation selection"), this);
    invertAnnotationSelectionAction->setObjectName("invert_selection_action");
    connect(invertAnnotationSelectionAction, SIGNAL(triggered()), SLOT(sl_invertSelection()));

    copyColumnTextAction = new QAction(tr("Copy column text"), this);
    connect(copyColumnTextAction, SIGNAL(triggered()), SLOT(sl_onCopyColumnText()));

    copyColumnURLAction = new QAction(tr("copy column URL"), this);
    connect(copyColumnURLAction, SIGNAL(triggered()), SLOT(sl_onCopyColumnURL()));

    editAction = new QAction(tr("Annotation"), this);
    editAction->setObjectName("edit_annotation_tree_item");
    editAction->setShortcut(QKeySequence(Qt::Key_F2));
    editAction->setShortcutContext(Qt::WindowShortcut);
    connect(editAction, SIGNAL(triggered()), SLOT(sl_edit()));
    tree->addAction(editAction);

    addQualifierAction = new QAction(tr("Qualifier..."), this);
#ifndef Q_OS_MAC
    addQualifierAction->setShortcut(QKeySequence(Qt::Key_Insert));
#else
    addQualifierAction->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));
#endif
    addQualifierAction->setShortcutContext(Qt::WindowShortcut);
    addQualifierAction->setObjectName("add_qualifier_action");
    connect(addQualifierAction, SIGNAL(triggered()), SLOT(sl_addQualifier()));
    tree->addAction(addQualifierAction);

    exportAutoAnnotationsGroup = new QAction(tr("Make auto-annotations persistent"), this);
    connect(exportAutoAnnotationsGroup, SIGNAL(triggered()), SLOT(sl_exportAutoAnnotationsGroup()));

    updateState();

    isDragging = false;
    resetDragAndDropData();
    tree->setAcceptDrops(true);
}

void AnnotationsTreeView::restoreWidgetState() {
    QMap<QString, QVariant> geom = AppContext::getSettings()->getValue(SETTINGS_ROOT + COLUMN_SIZES).toMap();
    bool ok = false;
    if (!geom.isEmpty()) {
        foreach (const QString &columnId, geom.keys()) {
            int columnIndex = columnId.toInt(&ok);
            if (!ok || columnIndex < 0 || columnIndex >= tree->columnCount()) {
                ok = false;
                break;
            }
            int width = geom[columnId].toInt(&ok);
            if (!ok || width <= 0) {
                ok = false;
                break;
            }
            tree->setColumnWidth(columnIndex, width);
        }
    }
    if (!ok) {
        tree->setColumnWidth(COLUMN_NAME, 300);
        tree->setColumnWidth(COLUMN_TYPE, 150);
    }
}

void AnnotationsTreeView::saveWidgetState() {
    QMap<QString, QVariant> geom;
    const int n = tree->columnCount();
    for (int i = 0; i < n; i++) {
        geom.insert(QString::number(i), tree->columnWidth(i));
    }
    AppContext::getSettings()->setValue(SETTINGS_ROOT + COLUMN_SIZES, geom);
}

AVGroupItem *AnnotationsTreeView::findGroupItem(AnnotationGroup *g) const {
    if (g->getParentGroup() == NULL) {
        const int n = tree->topLevelItemCount();
        for (int i = 0; i < n; i++) {
            AVItem *item = static_cast<AVItem *>(tree->topLevelItem(i));
            SAFE_POINT(AVItemType_Group == item->type, "Invalid item type!", NULL);
            AVGroupItem *groupItem = static_cast<AVGroupItem *>(item);
            if (groupItem->group == g) {
                return groupItem;
            }
        }
    } else {
        AVGroupItem *parentGroupItem = findGroupItem(g->getParentGroup());
        if (NULL != parentGroupItem) {
            const int n = parentGroupItem->childCount();
            for (int i = 0; i < n; i++) {
                AVItem *item = static_cast<AVItem *>(parentGroupItem->child(i));
                if (AVItemType_Group != item->type) {
                    continue;
                }
                AVGroupItem *gItem = static_cast<AVGroupItem *>(item);
                if (gItem->group == g) {
                    return gItem;
                }
            }
        }
    }
    return NULL;
}

AVAnnotationItem *AnnotationsTreeView::findAnnotationItem(const AVGroupItem *groupItem, Annotation *a) const {
    for (int i = 0, n = groupItem->childCount(); i < n; i++) {
        AVItem *item = static_cast<AVItem *>(groupItem->child(i));
        if (item->type != AVItemType_Annotation) {
            continue;
        }
        AVAnnotationItem *aItem = static_cast<AVAnnotationItem *>(item);
        if (aItem->annotation == a) {
            return aItem;
        }
    }
    return NULL;
}

AVAnnotationItem *AnnotationsTreeView::findAnnotationItem(AnnotationGroup *g, Annotation *a) const {
    AVGroupItem *groupItem = findGroupItem(g);
    if (NULL == groupItem) {
        return NULL;
    }
    return findAnnotationItem(groupItem, a);
}

/** This method is optimized to use annotation groups.
So can only be used for annotations that belongs to some object */
QList<AVAnnotationItem *> AnnotationsTreeView::findAnnotationItems(Annotation *a) const {
    QList<AVAnnotationItem *> res;

    SAFE_POINT(NULL != a->getGObject() && ctx->getAnnotationObjects(true).contains(a->getGObject()), "Invalid annotation table!", res);

    AnnotationGroup *g = a->getGroup();
    AVGroupItem *gItem = findGroupItem(g);
    SAFE_POINT(gItem != NULL, "AnnotationItemGroup not found!", res);
    AVAnnotationItem *aItem = findAnnotationItem(gItem, a);
    SAFE_POINT(aItem != NULL, "AnnotationItem not found!", res);
    res.append(aItem);

    return res;
}

QList<AVAnnotationItem *> AnnotationsTreeView::findAnnotationItems(const AVGroupItem *gi) const {
    QList<AVAnnotationItem *> annotationItems;
    for (int i = 0; i < gi->childCount(); ++i) {
        AVItem *childItem = dynamic_cast<AVItem *>(gi->child(i));
        SAFE_POINT(NULL != childItem, "Can't cast 'QTreeWidgetItem *' to 'AVItem *'", QList<AVAnnotationItem *>());
        switch (childItem->type) {
        case AVItemType_Annotation: {
            AVAnnotationItem *annotationItem = dynamic_cast<AVAnnotationItem *>(childItem);
            SAFE_POINT(NULL != annotationItem, "Can't cast 'AVItem *' to 'AVAnnotationItem *'", QList<AVAnnotationItem *>());
            annotationItems << annotationItem;
            break;
        }
        case AVItemType_Group: {
            AVGroupItem *groupItem = dynamic_cast<AVGroupItem *>(childItem);
            SAFE_POINT(NULL != groupItem, "Can't cast 'AVItem *' to 'AVGroupItem *'", QList<AVAnnotationItem *>());
            annotationItems << findAnnotationItems(groupItem);
            break;
        }
        default:;    // Do nothing
        }
    }
    return annotationItems;
}

void AnnotationsTreeView::removeGroupAnnotationsFromCache(const AVGroupItem *groupItem) {
    CHECK(!selectedAnnotation.isEmpty(), );
    const QSet<AVAnnotationItem *> annotationItems = findAnnotationItems(groupItem).toSet();
    foreach (AVAnnotationItem *annotationItem, selectedAnnotation.keys()) {
        if (annotationItems.contains(annotationItem)) {
            selectedAnnotation.remove(annotationItem);
        }
    }
}

void AnnotationsTreeView::connectSequenceObjectContext(ADVSequenceObjectContext *advContext) {
    connect(advContext, SIGNAL(si_annotationActivated(Annotation *, int)), SLOT(sl_annotationActivated(Annotation *, int)));
    connect(advContext, SIGNAL(si_annotationDoubleClicked(Annotation *, int)), SLOT(sl_annotationDoubleClicked(Annotation *, int)));
    connect(advContext, SIGNAL(si_clearSelectedAnnotationRegions()), SLOT(sl_clearSelectedAnnotations()));
}

void AnnotationsTreeView::disconnectSequenceObjectContext(ADVSequenceObjectContext *advContext) {
    advContext->disconnect(this);
}

void AnnotationsTreeView::connectAnnotationSelection() {
    connect(ctx->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)),
            SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)));
}

void AnnotationsTreeView::connectAnnotationGroupSelection() {
    connect(ctx->getAnnotationsGroupSelection(),
            SIGNAL(si_selectionChanged(AnnotationGroupSelection *, const QList<AnnotationGroup *> &, const QList<AnnotationGroup *> &)),
            SLOT(sl_onAnnotationGroupSelectionChanged(AnnotationGroupSelection *, const QList<AnnotationGroup *> &, const QList<AnnotationGroup *> &)));
}

void AnnotationsTreeView::sl_onItemSelectionChanged() {
    AnnotationSelection *annotationsSelection = ctx->getAnnotationsSelection();
    QList<Annotation *> selectedAnnotationsBefore = annotationsSelection->getAnnotations();
    annotationsSelection->disconnect(this);
    annotationsSelection->clear();

    AnnotationGroupSelection *annotationsGroupSelection = ctx->getAnnotationsGroupSelection();
    annotationsGroupSelection->disconnect(this);
    annotationsGroupSelection->clear();

    bool annotationActivated = false;
    QList<QTreeWidgetItem *> selectedTreeItems = tree->selectedItems();
    foreach (QTreeWidgetItem *i, selectedTreeItems) {
        AVItem *item = static_cast<AVItem *>(i);
        if (item->type == AVItemType_Annotation) {
            AVAnnotationItem *annotationItem = static_cast<AVAnnotationItem *>(item);
            Annotation *annotation = annotationItem->annotation;
            SAFE_POINT(NULL != annotation->getGObject(), "Invalid annotation table!", );
            // Activate the first new annotation from the selected tree items.
            if (!annotationActivated && !selectedAnnotationsBefore.contains(annotation)) {
                emitAnnotationActivated(annotation);
                annotationActivated = true;
            }
            annotationsSelection->add(annotation);
        } else if (item->type == AVItemType_Group) {
            AVGroupItem *groupItem = static_cast<AVGroupItem *>(item);
            annotationsGroupSelection->addToSelection(groupItem->group);
        }
    }

    // make the text of Edit action correct
    if (selectedTreeItems.size() == 1) {
        switch (static_cast<AVItem *>(selectedTreeItems.last())->type) {
        case AVItemType_Group:
            editAction->setText(tr("Group"));
            break;
        case AVItemType_Qualifier:
            editAction->setText(tr("Qualifier"));
            break;
        case AVItemType_Annotation:
            editAction->setText(tr("Annotation"));
            break;
        default:
            FAIL("Invalid annotation tree item type", );
        }
    }

    connectAnnotationSelection();
    connectAnnotationGroupSelection();
    updateState();
}

void AnnotationsTreeView::sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &added, const QList<Annotation *> &removed) {
    tree->disconnect(this, SIGNAL(sl_onItemSelectionChanged()));
    clearSelectedNotAnnotations();
    foreach (Annotation *a, removed) {
        AnnotationGroup *g = a->getGroup();
        AVAnnotationItem *item = findAnnotationItem(g, a);
        if (NULL == item) {
            continue;
        }
        if (item->isSelected()) {
            item->setSelected(false);
        }
    }
    AVAnnotationItem *toVisible = NULL;
    QList<AVAnnotationItem *> selectedItems;
    foreach (Annotation *a, added) {
        AnnotationGroup *g = a->getGroup();
        AVAnnotationItem *item = findAnnotationItem(g, a);
        if (NULL == item) {
            continue;
        }
        if (!item->isSelected()) {
            item->setSelected(true);
            selectedItems.append(item);
            for (QTreeWidgetItem *p = item->parent(); NULL != p; p = p->parent()) {
                if (!p->isExpanded()) {
                    p->setExpanded(true);
                }
            }
        }
        toVisible = item;
    }

    if (!selectedItems.isEmpty()) {
        tree->setCurrentItem(selectedItems.last(), 0, QItemSelectionModel::NoUpdate);
        editAction->setText(tr("Annotation"));
    }

    connect(tree, SIGNAL(itemSelectionChanged()), SLOT(sl_onItemSelectionChanged()));

    //make item visible if special conditions are met
    if (toVisible != NULL && added.size() == 1) {
        tree->scrollToItem(toVisible, QAbstractItemView::EnsureVisible);
    }
    updateState();
}

void AnnotationsTreeView::sl_onAnnotationGroupSelectionChanged(AnnotationGroupSelection *s, const QList<AnnotationGroup *> &added, const QList<AnnotationGroup *> &removed) {
    Q_UNUSED(s);

    foreach (AnnotationGroup *g, removed) {
        AVGroupItem *item = findGroupItem(g);
        if (item->isSelected()) {
            item->setSelected(false);
        }
    }

    foreach (AnnotationGroup *g, added) {
        AVGroupItem *item = findGroupItem(g);
        if (!item->isSelected()) {
            item->setSelected(true);
        }
    }

    if (added.size() == 1) {
        AVGroupItem *item = findGroupItem(added.first());
        tree->scrollToItem(item, QAbstractItemView::EnsureVisible);
    }
}

void AnnotationsTreeView::sl_onAnnotationObjectAdded(AnnotationTableObject *obj) {
    TreeSorter ts(this);
    Q_UNUSED(ts);

    SAFE_POINT(findGroupItem(obj->getRootGroup()) == NULL, "Invalid annotation group!", );

    AVGroupItem *groupItem = buildGroupTree(NULL, obj->getRootGroup(), false);
    SAFE_POINT(NULL != groupItem, "creating AVGroupItem failed", );
    tree->addTopLevelItem(groupItem);
    connect(obj, SIGNAL(si_onAnnotationsAdded(const QList<Annotation *> &)), SLOT(sl_onAnnotationsAdded(const QList<Annotation *> &)));
    connect(obj, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation *> &)), SLOT(sl_onAnnotationsRemoved(const QList<Annotation *> &)));
    connect(obj, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification> &)), SLOT(sl_onAnnotationsModified(const QList<AnnotationModification> &)));

    connect(obj, SIGNAL(si_onGroupCreated(AnnotationGroup *)), SLOT(sl_onGroupCreated(AnnotationGroup *)));
    connect(obj, SIGNAL(si_onGroupRemoved(AnnotationGroup *, AnnotationGroup *)), SLOT(sl_onGroupRemoved(AnnotationGroup *, AnnotationGroup *)));
    connect(obj, SIGNAL(si_onGroupRenamed(AnnotationGroup *)), SLOT(sl_onGroupRenamed(AnnotationGroup *)));

    connect(obj, SIGNAL(si_modifiedStateChanged()), SLOT(sl_annotationObjectModifiedStateChanged()));
    connect(obj, SIGNAL(si_nameChanged(const QString &)), SLOT(sl_onAnnotationObjectRenamed(const QString &)));
}

void AnnotationsTreeView::sl_onAnnotationObjectRemoved(AnnotationTableObject *obj) {
    TreeSorter ts(this);
    Q_UNUSED(ts);

    AVGroupItem *groupItem = findGroupItem(obj->getRootGroup());
    removeGroupAnnotationsFromCache(groupItem);
    delete groupItem;

    obj->disconnect(this);
}

void AnnotationsTreeView::sl_onAnnotationObjectRenamed(const QString &) {
    AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(sender());
    AVGroupItem *gi = findGroupItem(ao->getRootGroup());
    SAFE_POINT(NULL != gi, "Failed to find annotations object on rename!", );
    gi->updateVisual();
}

namespace {

bool doesGroupPresentInList(const QList<AnnotationGroup *> &list, AnnotationGroup *group) {
    foreach (AnnotationGroup *groupFromList, list) {
        if (groupFromList == group || groupFromList->isParentOf(group)) {
            return true;
        }
    }
    return false;
}

}    // namespace

void AnnotationsTreeView::sl_onAnnotationsAdded(const QList<Annotation *> &as) {
    TreeSorter ts(this);

    QSet<AVGroupItem *> toUpdate;
    QList<AnnotationGroup *> createdGroups;
    foreach (Annotation *a, as) {
        AnnotationGroup *ag = a->getGroup();
        if (doesGroupPresentInList(createdGroups, ag)) {
            continue;
        }
        AVGroupItem *gi = findGroupItem(ag);
        if (NULL != gi) {
            buildAnnotationTree(gi, a);
        } else {
            AnnotationGroup *childGroup = ag;
            while (true) {
                gi = findGroupItem(childGroup->getParentGroup());
                if (gi != NULL) {
                    break;
                }
                childGroup = childGroup->getParentGroup();
            }
            SAFE_POINT(NULL != gi, "AnnotationsTreeView::sl_onAnnotationsAdded: childGroup not found", );
            buildGroupTree(gi, childGroup);
            createdGroups << childGroup;    // if a group item has been built it already contains corresponding annotation items
                // so in further iterations we skip child annotations of this group
        }
        SAFE_POINT(NULL != gi, "Invalid annotation view item!", );
        toUpdate.insert(gi);

        if (dndAdded.contains(a)) {
            dndHit++;
            if (dndHit == dndAdded.size()) {
                if (!ctx->areAnnotationsInRange(dndAdded)) {
                    QMessageBox::warning(this, L10N::warningTitle(), tr("At least one dragged annotation is out of the sequence range!"));
                }
                dndHit = 0;
            }
        }
    }
    while (!toUpdate.isEmpty()) {
        AVGroupItem *i = *toUpdate.begin();
        toUpdate.remove(i);
        i->updateVisual();
        AVGroupItem *p = dynamic_cast<AVGroupItem *>(i->parent());
        if (p != NULL) {
            toUpdate.insert(p);
        }
    }
}

void AnnotationsTreeView::sl_onAnnotationsRemoved(const QList<Annotation *> &as) {
    TreeSorter ts(this);
    Q_UNUSED(ts);

    tree->disconnect(this, SIGNAL(sl_onItemSelectionChanged()));

    AnnotationTableObject *aObj = qobject_cast<AnnotationTableObject *>(sender());
    SAFE_POINT(aObj != NULL, "Invalid annotation table detected!", );
    AVGroupItem *groupItem = findGroupItem(aObj->getRootGroup());
    QHash<AVGroupItem *, int> groups2RemovedCount;

    foreach (Annotation *a, as) {
        QList<AVAnnotationItem *> aItems;
        groupItem->findAnnotationItems(aItems, a);
        foreach (AVAnnotationItem *ai, aItems) {
            selectedAnnotation.remove(ai);

            AVGroupItem *parentGroup = static_cast<AVGroupItem *>(ai->parent());
            if (groups2RemovedCount.contains(parentGroup)) {
                ++groups2RemovedCount[parentGroup];
            } else {
                groups2RemovedCount.insert(parentGroup, 1);
            }
            delete ai;
        }
    }
    foreach (AVGroupItem *g, groups2RemovedCount.keys()) {
        g->updateVisual(groups2RemovedCount[g]);
    }

    connect(tree, SIGNAL(itemSelectionChanged()), SLOT(sl_onItemSelectionChanged()));
    sl_onItemSelectionChanged();
}

void AnnotationsTreeView::sl_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications) {
    foreach (const AnnotationModification &annotationModification, annotationModifications) {
        switch (annotationModification.type) {
        case AnnotationModification_NameChanged:
        case AnnotationModification_LocationChanged:
        case AnnotationModification_TypeChanged: {
            QList<AVAnnotationItem *> aItems = findAnnotationItems(annotationModification.annotation);
            assert(!aItems.isEmpty());
            foreach (AVAnnotationItem *ai, aItems) {
                ai->updateVisual(ATVAnnUpdateFlag_BaseColumns);
            }
        } break;

        case AnnotationModification_QualifierRemoved: {
            const QualifierModification &qm = static_cast<const QualifierModification &>(annotationModification);
            QList<AVAnnotationItem *> aItems = findAnnotationItems(qm.annotation);
            foreach (AVAnnotationItem *ai, aItems) {
                ai->removeQualifier(qm.getQualifier());
            }
        } break;
        case AnnotationModification_QualifierAdded: {
            const QualifierModification &qm = static_cast<const QualifierModification &>(annotationModification);
            QList<AVAnnotationItem *> aItems = findAnnotationItems(qm.annotation);
            foreach (AVAnnotationItem *ai, aItems) {
                if (ai->isExpanded() || ai->childCount() > 1) {    //if item was expanded - add real qualifier items
                    ai->addQualifier(qm.getQualifier());
                } else {
                    ai->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);    //otherwise process indicator only
                }
            }
        } break;
        case AnnotationModification_AddedToGroup: {
            const AnnotationGroupModification &gmd = static_cast<const AnnotationGroupModification &>(annotationModification);
            AVGroupItem *gi = findGroupItem(gmd.getGroup());
            SAFE_POINT(NULL != gi, L10N::nullPointerError("annotation view group item"), );
            buildAnnotationTree(gi, gmd.annotation);
            gi->updateVisual();
        } break;

        case AnnotationModification_RemovedFromGroup: {
            const AnnotationGroupModification &gmd = static_cast<const AnnotationGroupModification &>(annotationModification);
            AVAnnotationItem *ai = findAnnotationItem(gmd.getGroup(), gmd.annotation);
            SAFE_POINT(NULL != ai, L10N::nullPointerError("annotation view item"), );
            AVGroupItem *gi = dynamic_cast<AVGroupItem *>(ai->parent());
            selectedAnnotation.remove(ai);
            delete ai;
            gi->updateVisual();
        } break;
        }
    }
}

void AnnotationsTreeView::sl_onGroupCreated(AnnotationGroup *g) {
    CHECK(NULL == findGroupItem(g), );
    AVGroupItem *pg = g->getParentGroup() == NULL ? NULL : findGroupItem(g->getParentGroup());
    buildGroupTree(pg, g);
    pg->updateVisual();
}

void AnnotationsTreeView::sl_onGroupRemoved(AnnotationGroup *parent, AnnotationGroup *g) {
    AVGroupItem *pg = findGroupItem(parent);
    if (NULL == pg) {
        return;
    }

    tree->disconnect(this, SIGNAL(sl_onItemSelectionChanged()));

    removeGroupAnnotationsFromCache(pg);

    for (int i = 0, n = pg->childCount(); i < n; i++) {
        AVItem *item = static_cast<AVItem *>(pg->child(i));
        if (AVItemType_Group == item->type && (static_cast<AVGroupItem *>(item))->group == g) {
            if (NULL != item->parent()) {
                item->parent()->removeChild(item);
            }
            delete item;
            break;
        }
    }

    pg->updateVisual();

    connect(tree, SIGNAL(itemSelectionChanged()), SLOT(sl_onItemSelectionChanged()));
}

void AnnotationsTreeView::sl_onGroupRenamed(AnnotationGroup *g) {
    AVGroupItem *gi = findGroupItem(g);
    SAFE_POINT(NULL != gi, "Invalid view item detected!", );
    gi->updateVisual();
}

AVGroupItem *AnnotationsTreeView::buildGroupTree(AVGroupItem *parentGroupItem, AnnotationGroup *g, bool areAnnotationsNew) {
    AVGroupItem *groupItem = new AVGroupItem(this, parentGroupItem, g);
    const QList<AnnotationGroup *> subgroups = g->getSubgroups();
    foreach (AnnotationGroup *subgroup, subgroups) {
        buildGroupTree(groupItem, subgroup);
    }
    const QList<Annotation *> annotations = g->getAnnotations();
    foreach (Annotation *a, annotations) {
        buildAnnotationTree(groupItem, a, areAnnotationsNew);
    }
    groupItem->updateVisual();
    return groupItem;
}

AVAnnotationItem *AnnotationsTreeView::buildAnnotationTree(AVGroupItem *parentGroup, Annotation *a, bool areAnnotationsNew) {
    if (!areAnnotationsNew) {
        AVAnnotationItem *annotationItem = findAnnotationItem(parentGroup, a);
        CHECK(NULL == annotationItem, annotationItem);
    }

    AVAnnotationItem *annotationItem = new AVAnnotationItem(parentGroup, a);
    const QVector<U2Qualifier> qualifiers = a->getQualifiers();
    if (!qualifiers.isEmpty()) {
        annotationItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    return annotationItem;
}

void AnnotationsTreeView::populateAnnotationQualifiers(AVAnnotationItem *ai) {
    SAFE_POINT(QTreeWidgetItem::ShowIndicator == ai->childIndicatorPolicy(), "Invalid policy indicator detected!", );
    SAFE_POINT(0 == ai->childCount(), "Unexpected tree item's child count!", );
    const QVector<U2Qualifier> qualifiers = ai->annotation->getQualifiers();
    foreach (const U2Qualifier &q, qualifiers) {
        AVQualifierItem *curQualifierItem = new AVQualifierItem(ai, q);
        Q_UNUSED(curQualifierItem);
    }
    ai->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
}

class SettingsUpdater : public TreeWidgetVisitor {
public:
    SettingsUpdater(const QStringList &cs)
        : changedSettings(cs) {
    }

    bool isChildVisitRequired(QTreeWidgetItem *ti) {
        AVItem *item = static_cast<AVItem *>(ti);
        assert(item->type != AVItemType_Qualifier);
        return item->type == AVItemType_Group;    //visit children only for group items
    }

    void visit(QTreeWidgetItem *ti) {
        AVItem *item = static_cast<AVItem *>(ti);
        if (item->type == AVItemType_Group) {
            AVGroupItem *gi = static_cast<AVGroupItem *>(item);
            gi->updateVisual();
        } else if (item->type == AVItemType_Annotation) {
            AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(item);
            QString name = ai->annotation->getName();
            if (changedSettings.contains(name)) {
                ai->updateVisual(ATVAnnUpdateFlag_BaseColumns);
            }
        }
    }

private:
    QStringList changedSettings;
};

void AnnotationsTreeView::sl_onAnnotationSettingsChanged(const QStringList &changedSettings) {
    // first clear colors cache for changed annotations
    QMap<QString, QIcon> &cache = AVAnnotationItem::getIconsCache();
    foreach (const QString &name, changedSettings) {
        cache.remove(name);
    }
    // second -> update all annotations
    SettingsUpdater su(changedSettings);
    TreeWidgetUtils::visitDFS(tree, &su);
}

void AnnotationsTreeView::updateColumnContextActions(AVItem *item, int col) {
    copyColumnTextAction->setEnabled(item != NULL && (col >= 3 || (item->type == AVItemType_Annotation && col == 1)) && !item->text(col).isEmpty());
    copyColumnURLAction->setEnabled(item != NULL && col >= 3 && item->isColumnLinked(col));
    if (!copyColumnTextAction->isEnabled()) {
        copyColumnTextAction->setText(tr("Copy column text"));
    } else {
        QString colName;
        if (col >= 3) {
            assert(qColumns.size() > col - 3);
            colName = qColumns[col - 3];
            copyColumnTextAction->setText(tr("Copy column '%1' text").arg(colName));
        } else {
            AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(item);
            copyColumnTextAction->setText(tr("Copy '%1' annotation location").arg(ai->annotation->getName()));
        }
    }

    if (!copyColumnURLAction->isEnabled()) {
        copyColumnURLAction->setText(tr("copy column URL"));
    } else {
        assert(qColumns.size() > col - 3);
        QString colName = qColumns[col - 3];
        copyColumnURLAction->setText(tr("Copy column '%1' URL").arg(colName));
    }
}

void AnnotationsTreeView::sl_onBuildPopupMenu(GObjectView *, QMenu *m) {
    // Add actions that not depends on the point popup is called
    adjustMenu(m);

    QPoint globalPos = QCursor::pos();
    QPoint treePos = tree->mapFromGlobal(globalPos);
    if (!tree->rect().contains(treePos)) {
        return;
    }

    // Check is popup is called for column header
    QHeaderView *header = tree->header();
    QPoint headerPoint = header->mapFromGlobal(globalPos);
    if (header->rect().contains(headerPoint)) {
        int idx = header->logicalIndexAt(headerPoint);
        if (idx >= 3) {
            assert(idx - 3 < qColumns.size());
            lastClickedColumn = idx;
            removeColumnByHeaderClickAction->setText(tr("Hide '%1' column").arg(qColumns[lastClickedColumn - 3]));
            QAction *first = m->actions().first();
            m->insertAction(first, removeColumnByHeaderClickAction);
            m->insertSeparator(first);
        }
        return;
    }

    //Ensure that item clicked is in the tree selection. Do not destroy multi-selection if present
    QList<QTreeWidgetItem *> selItems = tree->selectedItems();
    QPoint viewportPos = tree->viewport()->mapFromGlobal(globalPos);
    if (selItems.size() <= 1) {
        QTreeWidgetItem *item = tree->itemAt(viewportPos);
        if (item != NULL) {
            if (selItems.size() == 1 && selItems.first() != item) {
                tree->setItemSelected(selItems.first(), false);
            }
            tree->setItemSelected(item, true);
        }
    }

    //Update column sensitive actions that appears only in context menu
    selItems = tree->selectedItems();
    lastClickedColumn = tree->columnAt(viewportPos.x());
    updateColumnContextActions(selItems.size() == 1 ? static_cast<AVItem *>(selItems.first()) : static_cast<AVItem *>(NULL), lastClickedColumn);

    if (selItems.size() == 1) {
        AVItem *avItem = static_cast<AVItem *>(selItems.first());
        AnnotationTableObject *aObj = avItem->getAnnotationTableObject();
        if (AutoAnnotationsSupport::isAutoAnnotation(aObj) && !aObj->getAnnotations().isEmpty()) {
            if (avItem->parent() != NULL) {
                m->addAction(exportAutoAnnotationsGroup);
            }
        }
    }

    //Add active context actions to the top level menu
    QList<QAction *> contextActions;
    contextActions << toggleQualifierColumnAction;

    QList<QAction *> copySubmenuActions;
    copySubmenuActions << copyQualifierURLAction << copyColumnTextAction
                       << copyColumnURLAction;

    QMenu *copyMenu = GUIUtils::findSubMenu(m, ADV_MENU_COPY);
    SAFE_POINT(copyMenu != NULL, "copyMenu", );
    foreach (QAction *a, copySubmenuActions) {
        if (a->isEnabled()) {
            copyMenu->addAction(a);
        }
    }
    int nActive = 0;
    QAction *first = m->actions().first();
    m->insertAction(first, searchQualifierAction);
    m->insertAction(first, invertAnnotationSelectionAction);

    QMenu *editMenu = GUIUtils::findSubMenu(m, ADV_MENU_EDIT);
    SAFE_POINT(editMenu != NULL, L10N::nullPointerError(ADV_MENU_EDIT), );
    if (editAction->isEnabled()) {
        editMenu->addAction(editAction);
    }

    m->insertSeparator(first);
    foreach (QAction *a, contextActions) {
        if (a->isEnabled()) {
            nActive++;
            m->insertAction(first, a);
        }
    }
    if (nActive > 0) {
        m->insertSeparator(first);
    }
}

void AnnotationsTreeView::adjustMenu(QMenu *m) const {
    QMenu *addMenu = GUIUtils::findSubMenu(m, ADV_MENU_ADD);
    SAFE_POINT(addMenu != NULL, "addMenu", );
    addMenu->addAction(addAnnotationObjectAction);
    addMenu->addAction(addQualifierAction);

    QMenu *removeMenu = GUIUtils::findSubMenu(m, ADV_MENU_REMOVE);
    SAFE_POINT(removeMenu != NULL, "removeMenu", );
    removeMenu->addAction(removeObjectsFromViewAction);
    removeMenu->addAction(removeAnnsAndQsAction);
}

void AnnotationsTreeView::sl_paste() {
    PasteFactory *pasteFactory = AppContext::getPasteFactory();
    SAFE_POINT(pasteFactory != nullptr, "pasteFactory is null", );

    bool addToProject = !(tree && tree->hasFocus());
    PasteTask *task = pasteFactory->createPasteTask(addToProject);
    CHECK(task != nullptr,);
    if (!addToProject) {
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_pasteFinished(Task *)));
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void AnnotationsTreeView::sl_pasteFinished(Task *_pasteTask) {
    if (!tree || !tree->hasFocus()) {
        return;
    }

    PasteTask *pasteTask = qobject_cast<PasteTask *>(_pasteTask);
    if (NULL == pasteTask || pasteTask->isCanceled()) {
        return;
    }
    const QList<Document *> &docs = pasteTask->getDocuments();
    if (docs.length() == 0) {
        return;
    }
    foreach (Document *doc, docs) {
        foreach (GObject *annObj, doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE)) {
            ctx->tryAddObject(annObj);
        }
    }
}

void AnnotationsTreeView::sl_onAddAnnotationObjectToView() {
    ProjectTreeControllerModeSettings s;
    s.objectTypesToShow.insert(GObjectTypes::ANNOTATION_TABLE);
    s.groupMode = ProjectTreeGroupMode_ByDocument;
    foreach (GObject *o, ctx->getObjects()) {
        s.excludeObjectList.append(o);
    }
    QList<GObject *> objs = ProjectTreeItemSelectorDialog::selectObjects(s, this);
    foreach (GObject *obj, objs) {
        assert(obj->getGObjectType() == GObjectTypes::ANNOTATION_TABLE);
        ctx->tryAddObject(obj);
    }
}

static QList<AVGroupItem *> selectGroupItems(const QList<QTreeWidgetItem *> &items, TriState readOnly, TriState rootOnly) {
    QList<AVGroupItem *> res;
    foreach (QTreeWidgetItem *i, items) {
        AVItem *item = static_cast<AVItem *>(i);
        if (item->type == AVItemType_Group) {
            AVGroupItem *gItem = static_cast<AVGroupItem *>(item);
            if (rootOnly != TriState_Unknown) {
                bool groupIsRoot = gItem->parent() == NULL;
                if ((rootOnly == TriState_Yes && !groupIsRoot) || (rootOnly == TriState_No && groupIsRoot)) {
                    continue;
                }
            }
            if (readOnly != TriState_Unknown) {
                bool groupReadOnly = gItem->isReadonly();
                if ((readOnly == TriState_Yes && !groupReadOnly) || (readOnly == TriState_No && groupReadOnly)) {
                    continue;
                }
            }
            res.append(gItem);
        }
    }
    return res;
}

static QList<AVAnnotationItem *> selectAnnotationItems(const QList<QTreeWidgetItem *> &items, TriState readOnly) {
    QList<AVAnnotationItem *> res;
    foreach (QTreeWidgetItem *i, items) {
        AVItem *item = static_cast<AVItem *>(i);
        if (item->type == AVItemType_Annotation) {
            AVAnnotationItem *aItem = static_cast<AVAnnotationItem *>(item);
            if (readOnly != TriState_Unknown) {
                bool aReadOnly = aItem->isReadonly();
                if ((readOnly == TriState_Yes && !aReadOnly) || (readOnly == TriState_No && aReadOnly)) {
                    continue;
                }
            }
            res.append(aItem);
        }
    }
    return res;
}

static QList<AVQualifierItem *> selectQualifierItems(const QList<QTreeWidgetItem *> &items, TriState readOnly) {
    QList<AVQualifierItem *> res;
    foreach (QTreeWidgetItem *i, items) {
        AVItem *item = static_cast<AVItem *>(i);
        if (item->type == AVItemType_Qualifier) {
            AVQualifierItem *qItem = static_cast<AVQualifierItem *>(item);
            if (readOnly != TriState_Unknown) {
                bool qReadOnly = qItem->isReadonly();
                if ((readOnly == TriState_Yes && !qReadOnly) || (readOnly == TriState_No && qReadOnly)) {
                    continue;
                }
            }
            res.append(qItem);
        }
    }
    return res;
}

void AnnotationsTreeView::sl_removeObjectFromView() {
    if (isDragging) {
        return;
    }

    QList<AVGroupItem *> topLevelGroups = selectGroupItems(tree->selectedItems(), TriState_Unknown, TriState_Yes);
    QList<GObject *> objects;
    foreach (AVGroupItem *gItem, topLevelGroups) {
        objects.append(gItem->group->getGObject());
    }

    foreach (GObject *obj, objects) {
        SAFE_POINT(obj->getGObjectType() == GObjectTypes::ANNOTATION_TABLE, "Unexpected object type", );
        if (AutoAnnotationsSupport::isAutoAnnotation(obj)) {
            continue;
        }
        ctx->removeObject(obj);
    }
}

static bool groupDepthInverseComparator(const AVGroupItem *i1, const AVGroupItem *i2) {
    int depth1 = i1->group->getGroupDepth();
    int depth2 = i2->group->getGroupDepth();
    return depth1 > depth2;
}

void AnnotationsTreeView::sl_removeAnnsAndQs() {
    //remove selected qualifiers first (cache them, since different qualifier items with equal name/val are not distinguished)
    if (isDragging) {
        return;
    }

    QList<AVQualifierItem *> qualifierItemsToRemove = selectQualifierItems(tree->selectedItems(), TriState_No);
    QList<U2Qualifier> qualsToRemove;
    qualsToRemove.reserve(qualifierItemsToRemove.size());
    QList<Annotation *> qualAnnotations;
    qualAnnotations.reserve(qualifierItemsToRemove.size());

    for (int i = 0, n = qualifierItemsToRemove.size(); i < n; i++) {
        AVQualifierItem *qi = qualifierItemsToRemove[i];
        AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(qi->parent());
        qualAnnotations << ai->annotation;
        qualsToRemove << U2Qualifier(qi->qName, qi->qValue);
    }
    for (int i = 0, n = qualifierItemsToRemove.size(); i < n; i++) {
        Annotation *a = qualAnnotations.at(i);
        const U2Qualifier &q = qualsToRemove.at(i);
        a->removeQualifier(q);
    }

    //remove selected annotations now
    QList<AVAnnotationItem *> annotationItemsToRemove = selectAnnotationItems(tree->selectedItems(), TriState_No);

    QMultiMap<AnnotationGroup *, Annotation *> annotationsByGroup;
    foreach (AVAnnotationItem *aItem, annotationItemsToRemove) {
        SAFE_POINT(!aItem->annotation->getGObject()->isStateLocked(), "Attempting to remove annotations from locked table!", );
        AnnotationGroup *ag = (static_cast<AVGroupItem *>(aItem->parent())->group);
        annotationsByGroup.insert(ag, aItem->annotation);
    }

    QList<AnnotationGroup *> agroups = annotationsByGroup.uniqueKeys();
    foreach (AnnotationGroup *ag, agroups) {
        const QList<Annotation *> annotations = annotationsByGroup.values(ag);
        ag->removeAnnotations(annotations);
    }

    //now remove selected groups
    QList<AVGroupItem *> groupItemsToRemove = selectGroupItems(tree->selectedItems(), TriState_No, TriState_No);

    qSort(groupItemsToRemove.begin(), groupItemsToRemove.end(), groupDepthInverseComparator);
    //now remove all groups
    foreach (AVGroupItem *gi, groupItemsToRemove) {
        AnnotationGroup *pg = gi->group->getParentGroup();
        pg->removeSubgroup(gi->group);
    }
}

void AnnotationsTreeView::updateState() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();

    QList<AVGroupItem *> topLevelGroups = selectGroupItems(items, TriState_Unknown, TriState_Yes);

    bool hasAutoAnnotationObjects = false;
    foreach (AVGroupItem *item, topLevelGroups) {
        AnnotationTableObject *aObj = item->getAnnotationTableObject();
        if (AutoAnnotationsSupport::isAutoAnnotation(aObj)) {
            hasAutoAnnotationObjects = true;
            break;
        }
    }

    removeObjectsFromViewAction->setEnabled(!topLevelGroups.isEmpty() && !hasAutoAnnotationObjects);

    QList<AVGroupItem *> nonRootModGroups = selectGroupItems(items, TriState_No, TriState_No);
    QList<AVAnnotationItem *> modAnnotations = selectAnnotationItems(items, TriState_No);
    QList<AVQualifierItem *> modQualifiers = selectQualifierItems(items, TriState_No);
    removeAnnsAndQsAction->setEnabled(!nonRootModGroups.isEmpty() || !modAnnotations.isEmpty() || !modQualifiers.isEmpty());

    bool hasOnly1QualifierSelected = items.size() == 1 && (static_cast<AVItem *>(items.first()))->type == AVItemType_Qualifier;
    QString qName = hasOnly1QualifierSelected ? (static_cast<AVQualifierItem *>(items.first()))->qName : QString("");

    emit si_setCopyQualifierActionStatus(hasOnly1QualifierSelected, hasOnly1QualifierSelected ? tr("Copy qualifier '%1' value").arg(qName) : tr("Copy qualifier text"));

    bool hasOnly1QualifierSelectedWithURL = hasOnly1QualifierSelected && (static_cast<AVItem *>(items.first()))->isColumnLinked(1);
    copyQualifierURLAction->setEnabled(hasOnly1QualifierSelectedWithURL);
    copyQualifierURLAction->setText(hasOnly1QualifierSelectedWithURL ? tr("Copy qualifier '%1' URL").arg(qName) : tr("Copy qualifier URL"));

    toggleQualifierColumnAction->setEnabled(hasOnly1QualifierSelected);
    bool hasColumn = qColumns.contains(qName);
    toggleQualifierColumnAction->setText(!hasOnly1QualifierSelected ? tr("Toggle column") : (qColumns.contains(qName) ? tr("Hide '%1' column") : tr("Add '%1' column")).arg(qName));

    toggleQualifierColumnAction->setIcon(hasOnly1QualifierSelected ? (hasColumn ? removeColumnIcon : addColumnIcon) : QIcon());

    QTreeWidgetItem *ciBase = tree->currentItem();
    AVItem *ci = static_cast<AVItem *>(ciBase);
    bool editableItemSelected = items.size() == 1 && ci != NULL && ci == items.first() && !ci->isReadonly();
    editAction->setEnabled(editableItemSelected);

    bool hasEditableAnnotationContext = editableItemSelected && (ci->type == AVItemType_Annotation || ci->type == AVItemType_Qualifier);
    addQualifierAction->setEnabled(hasEditableAnnotationContext);
}

static bool isReadOnly(QTreeWidgetItem *item) {
    for (; item; item = item->parent()) {
        AVItem *itemi = dynamic_cast<AVItem *>(item);
        AnnotationTableObject *obj;
        switch (itemi->type) {
        case AVItemType_Group:
            obj = dynamic_cast<AVGroupItem *>(itemi)->group->getGObject();
            break;
        case AVItemType_Annotation:
            obj = dynamic_cast<AVAnnotationItem *>(itemi)->annotation->getGObject();
            break;
        default:
            continue;
        }
        if (obj->isStateLocked()) {
            return true;
        }
    }
    return false;
}

void AnnotationsTreeView::resetDragAndDropData() {
    dndCopyOnly = false;
    dndSelItems.clear();
    dropDestination = NULL;
}

bool AnnotationsTreeView::eventFilter(QObject *o, QEvent *e) {
    if (o != tree->viewport()) {
        return false;
    }
    const QEvent::Type etype = e->type();
    switch (etype) {
    case QEvent::ToolTip: {
        const QHelpEvent *he = static_cast<const QHelpEvent *>(e);
        const QPoint globalPos = he->globalPos();
        const QPoint viewportPos = tree->viewport()->mapFromGlobal(globalPos);
        const QTreeWidgetItem *item = tree->itemAt(viewportPos);
        if (NULL != item) {
            const AVItem *avi = static_cast<const AVItem *>(item);
            if (AVItemType_Annotation == avi->type) {
                const AVAnnotationItem *ai = static_cast<const AVAnnotationItem *>(avi);
                const ADVSequenceObjectContext *sc = ctx->getSequenceContext(ai->getAnnotationTableObject());
                const bool seqObjectContextValid = (NULL != sc);
                const QString tip = Annotation::getQualifiersTip(ai->annotation->getData(), 15, seqObjectContextValid ? sc->getSequenceObject() : NULL, seqObjectContextValid ? sc->getComplementTT() : NULL);
                if (!tip.isEmpty()) {
                    QToolTip::showText(he->globalPos(), tip);
                    return true;
                }
            }
        }
        return false;
    }
    case QEvent::MouseButtonRelease:
        lastMB = ((QMouseEvent *)e)->button();
        isDragging = false;
        return false;
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
        if (me->modifiers() == Qt::NoModifier && me->button() == Qt::LeftButton) {
            QTreeWidgetItem *item = tree->itemAt(dragStartPos = me->pos());
            if (item != NULL) {
                AVItemType type = dynamic_cast<AVItem *>(item)->type;
                if (type == AVItemType_Annotation || type == AVItemType_Group) {
                    if (!tree->selectedItems().contains(item)) {
                        tree->setCurrentItem(item);
                    }
                    isDragging = true;
                    return false;
                }
            }
        }
        isDragging = false;
        return false;
    }
    case QEvent::MouseMove: {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);
        if (!(me->buttons() & Qt::LeftButton) || !isDragging) {
            return false;
        }
        if ((me->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance()) {
            return true;
        }
        initiateDragAndDrop(me);
        return true;
    }
    case QEvent::DragEnter: {
        QDragEnterEvent *de = dynamic_cast<QDragEnterEvent *>(e);
        if (de->mimeData()->hasFormat(annotationMimeType)) {
            de->acceptProposedAction();
            uiLog.trace("Drag enter event in Annotations view : format is OK");
            return true;
        }
        uiLog.trace("Drag enter event in Annotations view : unsupported mime format!");
        return false;
    }
    case QEvent::DragMove: {    // update current D&D state
        QDragMoveEvent *de = dynamic_cast<QDragMoveEvent *>(e);
        if (de->mimeData()->hasFormat(annotationMimeType)) {
            QTreeWidgetItem *item = tree->itemAt(de->pos());
            if (item == NULL) {
                de->ignore();
                uiLog.trace("Drag event in Annotations view : invalid destination");
                return true;
            }
            if (isReadOnly(item)) {
                de->ignore();
                uiLog.trace(QString("Drag event in Annotations view : destination is read-only: %1").arg(item->text(0)));
                return true;
            }
            Qt::DropAction dndAction = (de->keyboardModifiers() & Qt::ShiftModifier) ? Qt::CopyAction : Qt::MoveAction;
            if (dndAction == Qt::MoveAction && dndCopyOnly) {
                de->ignore();
                uiLog.trace("Drag event in Annotations view : source is read-only, can't move");
                return true;
            }

            for (AVItem *itemi = dynamic_cast<AVItem *>(item); itemi != NULL; itemi = dynamic_cast<AVItem *>(itemi->parent())) {
                if (itemi->type == AVItemType_Group) {
                    for (int i = 0, s = dndSelItems.size(); i < s; ++i) {
                        AVItem *dndItem = dndSelItems[i];
                        if (dndItem == itemi) {    // do not allow drop group into itself
                            de->ignore();
                            uiLog.trace("Drag event in Annotations view : can't move group into itself!");
                            return true;
                        }
                    }
                }
            }
            if (item->parent() == NULL) {    //destination is root item
                for (int i = 0, s = dndSelItems.size(); i < s; ++i) {
                    AVItem *dndItem = dndSelItems[i];
                    if (dndItem->type == AVItemType_Annotation) {
                        de->ignore();
                        // root group can't have annotations -> problem with store/load invariant..
                        uiLog.trace("Drag event in Annotations view : can't move annotations, destination group is not found!");
                        return true;
                    }
                }
            }
            de->setDropAction(dndAction);
            de->accept();
            uiLog.trace(QString("Drag event in Annotations view, status: OK, type: %1").arg(dndAction == Qt::MoveAction ? "Move" : "Copy"));
            return true;
        }
        return false;
    }
    case QEvent::Drop: {
        uiLog.trace("Drop event in Annotations view");
        QDropEvent *de = dynamic_cast<QDropEvent *>(e);
        const QMimeData *mime = de->mimeData();
        if (mime->hasFormat(annotationMimeType)) {
            QTreeWidgetItem *item = tree->itemAt(de->pos());
            if (item != NULL) {
                AVItem *avItem = dynamic_cast<AVItem *>(item);
                while (avItem != NULL && avItem->type != AVItemType_Group) {
                    avItem = dynamic_cast<AVItem *>(avItem->parent());
                }
                if (avItem != NULL) {
                    dropDestination = static_cast<AVGroupItem *>(avItem);
                    SAFE_POINT(dropDestination != NULL, "dropDestination is NULL", false);
                    if (de->keyboardModifiers() & Qt::ShiftModifier) {
                        de->setDropAction(Qt::CopyAction);
                    } else {
                        de->setDropAction(Qt::MoveAction);
                    }
                    de->accept();
                    uiLog.trace(QString("Drop event in Annotations view : destination group %1").arg(dropDestination->group->getGroupPath()));
                } else {
                    uiLog.trace("Drop event in Annotations view : something weird, can't derive destination group!");
                }
                return true;
            } else {
                uiLog.trace("Drop event in Annotations view : invalid destination");
            }
        } else {
            uiLog.trace("Drop event in Annotations view : unknown MIME format");
            return false;
        }
        de->ignore();
        return true;
    }
    default:
        return false;
    }
}

bool AnnotationsTreeView::initiateDragAndDrop(QMouseEvent *) {
    uiLog.trace("Starting drag & drop in annotations view");

    resetDragAndDropData();

    // Now filter selection: keep only parent items in the items list, remove qualifiers, check if 'move' is allowed
    QList<QTreeWidgetItem *> initialSelItems = tree->selectedItems();    // whole selection will be moved
    dndCopyOnly = false;    // allow 'move' by default first
    for (int i = 0, n = initialSelItems.size(); i < n; i++) {
        AVItem *itemi = dynamic_cast<AVItem *>(initialSelItems[i]);
        SAFE_POINT(NULL != itemi, L10N::nullPointerError("Annotation tree item"), false);
        AnnotationTableObject *ao = itemi->getAnnotationTableObject();
        SAFE_POINT(NULL != ao, L10N::nullPointerError("annotation table object"), false);
        if (AutoAnnotationsSupport::isAutoAnnotation(ao)) {
            //  only allow to drag top-level auto annotations groups
            if (!(itemi->type == AVItemType_Group && itemi->parent() != NULL)) {
                continue;
            }
        }
        if (!dndCopyOnly && isReadOnly(itemi)) {
            dndCopyOnly = true;
        }
        if (itemi->type == AVItemType_Annotation) {
            // if annotation group is in the selection -> remove annotation from the list, it will be moved/copied automatically with group
            bool addItem = true;
            for (QTreeWidgetItem *cur = itemi->parent(); cur != NULL; cur = cur->parent()) {
                if (initialSelItems.contains(cur)) {
                    addItem = false;
                    break;
                }
            }
            if (addItem) {
                dndSelItems.append(itemi);
            }
        } else if (itemi->type == AVItemType_Qualifier) {    // remove qualifiers from selection
            itemi->setSelected(false);
        } else {
            SAFE_POINT(itemi->type == AVItemType_Group, "An unexpected tree item type", false);
            if (itemi->parent() == NULL) {    // object level group -> add all subgroups
                for (int j = 0, m = itemi->childCount(); j < m; j++) {
                    AVItem *citem = dynamic_cast<AVItem *>(itemi->child(j));
                    SAFE_POINT(citem->type == AVItemType_Group || citem->type == AVItemType_Annotation, "An unexpected child tree item type", false);
                    dndSelItems.append(citem);
                }
            } else {
                dndSelItems.append(itemi);
            }
        }
    }
    if (dndSelItems.isEmpty()) {
        resetDragAndDropData();
        uiLog.trace("No items to drag & drop");
        return false;
    }

    // serialize selected items : // TODO: actually we do not need it today, so skipping this step
    //QStringList annotationsAndGroupsRefs;
    QByteArray serializedReferences;
    serializedReferences.append(serializedReferences);

    // initiate dragging
    QMimeData *mimeData = new QMimeData();
    mimeData->setData(annotationMimeType, serializedReferences);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    Qt::DropAction dndAction = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);
    if (dndAction == Qt::MoveAction || dndAction == Qt::CopyAction) {
        finishDragAndDrop(dndAction);
    } else {
        uiLog.trace("Drag & drop in Annotations view was ignored");
    }
    resetDragAndDropData();
    return true;
}

typedef QPair<AnnotationGroup *, QString> GroupChangeInfo;
typedef QPair<GroupChangeInfo, Annotation *> AnnotationDnDInfo;

static void collectAnnotationDnDInfo(AnnotationGroup *ag, const QString &destGroupPath, QList<AnnotationDnDInfo> &annotationsToProcess) {
    foreach (Annotation *a, ag->getAnnotations()) {
        annotationsToProcess << AnnotationDnDInfo(GroupChangeInfo(ag, destGroupPath), a);
    }
    const QString newDestGroupPath = destGroupPath + "/" + ag->getName();
    foreach (AnnotationGroup *sag, ag->getSubgroups()) {
        collectAnnotationDnDInfo(sag, newDestGroupPath, annotationsToProcess);
    }
}

void AnnotationsTreeView::finishDragAndDrop(Qt::DropAction dndAction) {
    SAFE_POINT(dropDestination != NULL, "dropDestination is NULL", );
    AnnotationTableObject *dstObject = dropDestination->getAnnotationTableObject();

    // Can not drag anything to auto-annotation object
    if (AutoAnnotationsSupport::isAutoAnnotation(dstObject)) {
        return;
    }

    const QString destGroupPath = dropDestination->group->getGroupPath();

    QList<AnnotationGroup *> affectedGroups;
    QList<AnnotationDnDInfo> affectedAnnotations;
    QList<Task *> moveAutoAnnotationTasks;
    QStringList manualCreationGroups;

    for (int i = 0, n = dndSelItems.size(); i < n; ++i) {
        AVItem *selItem = dndSelItems.at(i);
        SAFE_POINT(NULL != selItem->parent(), "Invalid tree item parent", );    // we never have no top-level object items in dndSelItems
        if (selItem->type == AVItemType_Annotation) {
            AVGroupItem *fromGroupItem = dynamic_cast<AVGroupItem *>(selItem->parent());
            AVAnnotationItem *ai = dynamic_cast<AVAnnotationItem *>(selItem);
            affectedAnnotations << AnnotationDnDInfo(GroupChangeInfo(fromGroupItem->group, destGroupPath), ai->annotation);
        } else {
            AVGroupItem *movedGroupItem = dynamic_cast<AVGroupItem *>(selItem);
            if (movedGroupItem->group->getParentGroup() == dropDestination->group) {
                continue;    // can't drop group into itself
            }

            // auto-annotations have to be handled differently
            if (AutoAnnotationsSupport::isAutoAnnotation(movedGroupItem->getAnnotationTableObject())) {
                GObjectReference dstRef(dstObject);
                ADVSequenceObjectContext *seqCtx = ctx->getSequenceInFocus();
                Task *t = new ExportAutoAnnotationsGroupTask(movedGroupItem->getAnnotationGroup(), dstRef, seqCtx);
                moveAutoAnnotationTasks.append(t);
                continue;
            }
            QString toGroupPath = destGroupPath + (destGroupPath.isEmpty() ? "" : "/") + movedGroupItem->group->getName();
            if (!movedGroupItem->group->hasAnnotations()) {
                //this group will not be created as part of annotation move process -> create it manually
                manualCreationGroups.append(toGroupPath);
            }
            collectAnnotationDnDInfo(movedGroupItem->group, toGroupPath, affectedAnnotations);
            affectedGroups.append(movedGroupItem->group);
        }
    }

    uiLog.trace(QString("Finishing drag & drop in Annotations view, affected groups: %1 , top-level annotations: %2")
                    .arg(affectedGroups.size())
                    .arg(affectedAnnotations.size()));

    // Move or Copy annotation reference inside of the object
    dndAdded.clear();
    QList<Annotation *> dndToRemove;    // used to remove dragged annotations at once in case of Qt::MoveAction (see below)
    QList<AnnotationGroup *> srcGroupList;    // used to remove dragged annotations at once in case of Qt::MoveAction (see below)
    QList<AnnotationGroup *> dstGroupList;    // used to add all dragged annotations at once (see below)
    foreach (const AnnotationDnDInfo &adnd, affectedAnnotations) {
        const QString &toGroupPath = adnd.first.second;
        AnnotationGroup *dstGroup = dstObject->getRootGroup()->getSubgroup(toGroupPath, true);
        if (dstGroup == dstObject->getRootGroup()) {    // root group can't have annotations -> problem with store/load invariant..
            continue;
        }
        AnnotationGroup *srcGroup = adnd.first.first;
        Annotation *srcAnnotation = adnd.second;
        Annotation *dstAnnotation = srcAnnotation;
        if (srcAnnotation->getGObject() != dstObject) {
            dstAnnotation = dstGroup->addAnnotations(QList<SharedAnnotationData>() << srcAnnotation->getData()).first();
        }
        bool doAdd = !dstGroup->getAnnotations().contains(dstAnnotation);
        bool doRemove = dndAction == Qt::MoveAction && doAdd;
        if (doAdd) {
            dndAdded.append(dstAnnotation);
            dstGroupList.append(dstGroup);
        }
        if (doRemove) {
            srcGroupList.append(srcGroup);
            dndToRemove.append(srcAnnotation);
        }
    }

    // Add and remove the dragged annotations to the receiver AnnotationsTreeView at once.
    // It is required for the case of cross-view drag and drop.
    int i = 0;
    foreach (AnnotationGroup *g, dstGroupList) {
        g->addAnnotations(QList<SharedAnnotationData>() << dndAdded.at(i)->getData());
        i++;
    }

    i = 0;
    QMap<AnnotationTableObject *, QList<AnnotationModification>> annotationModifications;
    foreach (AnnotationGroup *g, srcGroupList) {
        annotationModifications[g->getGObject()] << AnnotationGroupModification(AnnotationModification_RemovedFromGroup, dndToRemove.at(i), g);
        i++;
    }
    foreach (AnnotationTableObject *annotationTableObject, annotationModifications.keys()) {
        annotationTableObject->emit_onAnnotationsModified(annotationModifications[annotationTableObject]);
    }

    // Process groups
    if (dndAction == Qt::MoveAction) {
        foreach (AnnotationGroup *ag, affectedGroups) {
            ag->getParentGroup()->removeSubgroup(ag);
        }
    }
    // manually create empty group items
    foreach (const QString &path, manualCreationGroups) {
        dstObject->getRootGroup()->getSubgroup(path, true);
    }

    // make auto-annotations persistent
    foreach (Task *t, moveAutoAnnotationTasks) {
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
}

void AnnotationsTreeView::sl_itemEntered(QTreeWidgetItem *i, int column) {
    AVItem *item = static_cast<AVItem *>(i);
    Qt::CursorShape newShape = Qt::ArrowCursor;
    Qt::CursorShape currentShape = tree->cursor().shape();
    if (item != NULL) {
        if (item->isColumnLinked(column)) {
            newShape = Qt::PointingHandCursor;
        }
    }
    if (newShape == Qt::PointingHandCursor || ((newShape == Qt::ArrowCursor && currentShape == Qt::PointingHandCursor))) {
        tree->setCursor(newShape);
    }
}

void AnnotationsTreeView::sl_itemDoubleClicked(QTreeWidgetItem *i, int) {
    AVItem *item = static_cast<AVItem *>(i);

    if (item->type == AVItemType_Annotation) {
        AVAnnotationItem *ai = dynamic_cast<AVAnnotationItem *>(item);

        QVector<U2Region> annotationRegions = ai->annotation->getRegions();
        SAFE_POINT(!annotationRegions.isEmpty(), "Annotation regions are empty", );

        annotationDoubleClicked(ai, annotationRegions.toList());
    }

    if (item->type == AVItemType_Qualifier) {
        AVQualifierItem *qi = static_cast<AVQualifierItem *>(item);
        editQualifierItem(qi);
    }
}

void AnnotationsTreeView::sl_itemClicked(QTreeWidgetItem *i, int column) {
    AVItem *item = static_cast<AVItem *>(i);
    if (lastMB != Qt::LeftButton || item == NULL || !item->isColumnLinked(column)) {
        return;
    }
    QString fileUrl = item->getFileUrl(column);
    if (!fileUrl.isEmpty()) {
        Task *task = new LoadRemoteDocumentAndAddToProjectTask(fileUrl);
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    } else {
        GUIUtils::runWebBrowser(item->buildLinkURL(column));
    }
}

void AnnotationsTreeView::sl_itemPressed(QTreeWidgetItem *i) {
    AVItem *item = static_cast<AVItem *>(i);

    if (item->type == AVItemType_Annotation) {
        AVAnnotationItem *ai = dynamic_cast<AVAnnotationItem *>(item);
        QMap<AVAnnotationItem *, QList<U2Region>> sortedAnnotationSelections = sortAnnotationSelection(QList<AnnotationTableObject *>() << ai->getAnnotationTableObject());
        annotationClicked(ai, sortedAnnotationSelections);
    }
}

void AnnotationsTreeView::sl_itemExpanded(QTreeWidgetItem *qi) {
    AVItem *i = static_cast<AVItem *>(qi);
    if (i->type != AVItemType_Annotation) {
        return;
    }
    AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(i);
    if (ai->childCount() == 0 && !ai->annotation->getQualifiers().isEmpty()) {
        SAFE_POINT(ai->childIndicatorPolicy() == QTreeWidgetItem::ShowIndicator, "Unexpected tree child indicator policy", );
        populateAnnotationQualifiers(ai);
        ai->updateVisual(ATVAnnUpdateFlag_BaseColumns);
    } else {
        SAFE_POINT(ai->childIndicatorPolicy() == QTreeWidgetItem::DontShowIndicatorWhenChildless, "Unexpected tree child indicator policy", );
    }
}

void AnnotationsTreeView::sl_annotationActivated(Annotation *annotation, int regionIndex) {
    Q_UNUSED(regionIndex);
    sl_annotationClicked(annotation);
}

//TODO: refactor this method
//UTI-155
void AnnotationsTreeView::sl_annotationClicked(Annotation *annotation) {
    AnnotationSelection *annotationSelection = ctx->getAnnotationsSelection();

    const QList<AVAnnotationItem *> annotationItems = findAnnotationItems(annotation);
    CHECK(annotationItems.size() == 1, );
    AVAnnotationItem *item = annotationItems.first();

    const ADVSequenceObjectContext *advctx = qobject_cast<ADVSequenceObjectContext *>(sender());
    SAFE_POINT(advctx != NULL, "Incorrect sender", );

    QList<AnnotationTableObject *> annotationObjects = advctx->getAnnotationObjects().toList();
    QMap<AVAnnotationItem *, QList<U2Region>> sortedAnnotationSelections = sortAnnotationSelection(annotationObjects);

    expandItemRecursevly(item->parent());

    annotationSelection->add(item->annotation);
    annotationClicked(item, sortedAnnotationSelections, item->annotation->getRegions().toList());
}

//TODO: refactor this method
//UTI-155
//See review of UGENE-5936 for details
void AnnotationsTreeView::sl_annotationDoubleClicked(Annotation *annotation, int regionIndex) {
    QList<U2Region> annotationRegions = annotation->getRegions().toList();
    SAFE_POINT(0 <= regionIndex && regionIndex < annotationRegions.size(), "Unexpected region index", );

    const U2Region &indexRegion = annotationRegions[regionIndex];
    QList<U2Region> regionsToSelect = {indexRegion};
    AnnotationTableObject *tableObject = annotation->getGObject();
    SAFE_POINT(nullptr != tableObject, "AnnotationTableObject isn't found", );

    ADVSequenceObjectContext *sequenceContext = ctx->getSequenceContext(tableObject);
    SAFE_POINT(nullptr != sequenceContext, "ADVSequenceObjectContext isn't found", );

    U2SequenceObject *sequenceObject = sequenceContext->getSequenceObject();
    SAFE_POINT(nullptr != sequenceObject, "U2SequenceObject isn't found", );

    const int sequenceLength = sequenceObject->getSequenceLength();
    if (sequenceObject->isCircular() && (indexRegion.startPos == 0 || indexRegion.endPos() == sequenceLength)) {
        foreach (const U2Region &reg, annotationRegions) {
            CHECK_CONTINUE(reg != annotationRegions[regionIndex]);
            CHECK_CONTINUE(reg.startPos == 0 || reg.endPos() == sequenceLength);

            regionsToSelect.append(reg);
            break;
        }
    }
    QList<AVAnnotationItem *> annotationItems = findAnnotationItems(annotation);
    foreach (AVAnnotationItem *item, annotationItems) {
        expandItemRecursevly(item->parent());
        {
            SignalBlocker blocker(tree);
            item->setSelected(true);
        }
        annotationDoubleClicked(item, regionsToSelect);
    }
}

void AnnotationsTreeView::expandItemRecursevly(QTreeWidgetItem *item) {
    CHECK(item != NULL, );
    QTreeWidgetItem *parent = item->parent();
    if (parent != NULL) {
        expandItemRecursevly(parent);
    }
    tree->expandItem(item);
}

//In case of several sequences, we need to sort only selections of the current sequence
QMap<AVAnnotationItem *, QList<U2Region>> AnnotationsTreeView::sortAnnotationSelection(QList<AnnotationTableObject *> annotationObjects) {
    QMap<AVAnnotationItem *, QList<U2Region>> sortedAnnotation;
    foreach (AVAnnotationItem *key, selectedAnnotation.keys()) {
        AnnotationTableObject *tableObject = key->getAnnotationTableObject();
        if (annotationObjects.contains(tableObject)) {
            sortedAnnotation.insert(key, selectedAnnotation[key]);
        }
    }

    return sortedAnnotation;
}

void AnnotationsTreeView::sl_clearSelectedAnnotations() {
    const ADVSequenceObjectContext *advctx = qobject_cast<ADVSequenceObjectContext *>(sender());
    SAFE_POINT(advctx != NULL, "Incorrect sender", );

    QList<AnnotationTableObject *> annotationObjects = advctx->getAnnotationObjects().toList();
    QMap<AVAnnotationItem *, QList<U2Region>> currentAnnotationSelections = sortAnnotationSelection(annotationObjects);

    foreach (AVAnnotationItem *key, currentAnnotationSelections.keys()) {
        selectedAnnotation.remove(key);
    }
}

void AnnotationsTreeView::sl_sequenceAdded(ADVSequenceObjectContext *advContext) {
    connectSequenceObjectContext(advContext);
}

void AnnotationsTreeView::sl_sequenceRemoved(ADVSequenceObjectContext *advContext) {
    disconnectSequenceObjectContext(advContext);
}

//TODO: refactoring of annotationClicked and annotationDoubleClicked methods.
//It's too difficult to understand what's going on in this methods
//UTI-155
void AnnotationsTreeView::annotationClicked(AVAnnotationItem *item, QMap<AVAnnotationItem *, QList<U2Region>> selectedAnnotations, const QList<U2Region> &selectedRegions) {
    ADVSequenceObjectContext *seqObjCtx = ctx->getSequenceContext(item->getAnnotationTableObject());
    SAFE_POINT(seqObjCtx != NULL, "ADVSequenceObjectContext is NULL", );
    emitAnnotationActivated(item->annotation);

    DNASequenceSelection *sequenceSelection = seqObjCtx->getSequenceSelection();
    SAFE_POINT(sequenceSelection != NULL, "DNASequenceSelection is NULL", );

    Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    const bool controlOfShiftPressed = km.testFlag(Qt::ControlModifier) || km.testFlag(Qt::ShiftModifier);
    if (!controlOfShiftPressed) {
        sequenceSelection->clear();
        foreach (AVAnnotationItem *key, selectedAnnotations.keys()) {
            selectedAnnotation.remove(key);
        }
    } else {
        QVector<U2Region> toSelect;
        if (!selectedRegions.isEmpty() &&
            selectedAnnotations.value(item).contains(selectedRegions.first()) &&
            selectedAnnotations.value(item).contains(selectedRegions.last())) {
            selectedAnnotation[item].removeOne(selectedRegions.first());
            selectedAnnotations[item].removeOne(selectedRegions.first());
            if (selectedRegions.size() == 2) {
                selectedAnnotation[item].removeOne(selectedRegions.last());
                selectedAnnotations[item].removeOne(selectedRegions.last());
            }
            if (selectedAnnotation[item].isEmpty()) {
                selectedAnnotation.remove(item);
            }
        }
        foreach (AVAnnotationItem *annItem, selectedAnnotations.keys()) {
            foreach (U2Region newRegion, selectedAnnotations.value(annItem)) {
                foreach (const U2Region &toSel, toSelect) {
                    if (toSel.intersects(newRegion)) {
                        newRegion = U2Region::containingRegion(newRegion, toSel);
                        toSelect.removeOne(toSel);
                    }
                }
                toSelect << newRegion;
            }
        }
        sequenceSelection->clear();
        sequenceSelection->setSelectedRegions(toSelect);
    }
}

void AnnotationsTreeView::annotationDoubleClicked(AVAnnotationItem *item, const QList<U2Region> &selectedRegions) {
    selectedAnnotation[item] << selectedRegions;

    ADVSequenceObjectContext *seqObjCtx = ctx->getSequenceContext(item->getAnnotationTableObject());
    SAFE_POINT(seqObjCtx != NULL, "ADVSequenceObjectContext is NULL", );
    emitAnnotationActivated(item->annotation);

    DNASequenceSelection *sequenceSelection = seqObjCtx->getSequenceSelection();
    SAFE_POINT(sequenceSelection != NULL, "DNASequenceSelection is NULL", );

    AnnotationSelection *annotationSelection = seqObjCtx->getAnnotationsSelection();
    SAFE_POINT(annotationSelection != NULL, "AnnotationSelection is NULL", );

    annotationSelection->add(item->annotation);

    QList<U2Region> regionsToSelect = selectedRegions;
    const QVector<U2Region> regions = sequenceSelection->getSelectedRegions();
    foreach (const U2Region &reg, regions) {
        foreach (const U2Region &selectedRegion, selectedRegions) {
            if (reg.intersects(selectedRegion)) {
                sequenceSelection->removeRegion(reg);
                regionsToSelect.removeOne(selectedRegion);
                regionsToSelect << U2Region::containingRegion(reg, selectedRegion);
            }
        }
    }
    foreach (const U2Region &reg, regionsToSelect) {
        sequenceSelection->addRegion(reg);
    }
}

void AnnotationsTreeView::emitAnnotationActivated(Annotation *annotation) {
    ADVSequenceObjectContext *seqObjCtx = ctx->getSequenceContext(annotation->getGObject());
    disconnectSequenceObjectContext(seqObjCtx);
    seqObjCtx->emitAnnotationActivated(annotation, -1);
    connectSequenceObjectContext(seqObjCtx);
}

void AnnotationsTreeView::clearSelectedNotAnnotations() {
    foreach (QTreeWidgetItem *item, tree->selectedItems()) {
        AVItem *aVItem = static_cast<AVItem *>(item);
        if (aVItem != nullptr && aVItem->type != AVItemType_Annotation) {
            item->setSelected(false);
        }
    }
}

void AnnotationsTreeView::sl_onCopyQualifierValue() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    assert(items.size() == 1);
    AVItem *item = static_cast<AVItem *>(items.first());
    assert(item->type == AVItemType_Qualifier);
    AVQualifierItem *qi = static_cast<AVQualifierItem *>(item);
    QApplication::clipboard()->setText(qi->qValue);
}

void AnnotationsTreeView::sl_onCopyQualifierURL() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    assert(items.size() == 1);
    AVItem *item = static_cast<AVItem *>(items.first());
    if (item->isColumnLinked(1)) {
        QApplication::clipboard()->setText(item->buildLinkURL(1));
    }
}

void AnnotationsTreeView::sl_onCopyColumnText() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    assert(items.size() == 1);
    AVItem *item = static_cast<AVItem *>(items.first());
    QApplication::clipboard()->setText(item->text(lastClickedColumn));
}

void AnnotationsTreeView::sl_onCopyColumnURL() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    assert(items.size() == 1);
    AVItem *item = static_cast<AVItem *>(items.first());
    QApplication::clipboard()->setText(item->buildLinkURL(lastClickedColumn));
}

void AnnotationsTreeView::sl_onToggleQualifierColumn() {
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    assert(items.size() == 1);
    AVItem *item = static_cast<AVItem *>(items.first());
    assert(item->type == AVItemType_Qualifier);
    AVQualifierItem *qi = static_cast<AVQualifierItem *>(item);
    if (qColumns.contains(qi->qName)) {
        removeQualifierColumn(qi->qName);
    } else {
        addQualifierColumn(qi->qName);
    }
}

void AnnotationsTreeView::sl_onRemoveColumnByHeaderClick() {
    if (isDragging) {
        return;
    }

    assert(lastClickedColumn >= 3);
    assert(lastClickedColumn - 3 <= qColumns.size());
    removeQualifierColumn(qColumns[lastClickedColumn - 3]);
}

void AnnotationsTreeView::sl_searchQualifier() {
    QObjectScopedPointer<SearchQualifierDialog> d = new SearchQualifierDialog(this, this);
    d->exec();
}

void AnnotationsTreeView::sl_invertSelection() {
    QItemSelectionModel *selectionModel = tree->selectionModel();
    QItemSelection originalSelection = selectionModel->selection();
    QItemSelection unselectedAnnotations;
    QItemSelection selectedAnnotations;

    QStack<QModelIndex> stack;
    stack.push(tree->rootIndex());

    QAbstractItemModel *itemModel = tree->model();
    while (!stack.isEmpty()) {
        const QModelIndex parent = stack.pop();
        AVAnnotationItem *aItem = dynamic_cast<AVAnnotationItem *>(tree->itemFromIndex(parent));
        if (NULL != aItem) {
            if (originalSelection.contains(parent)) {
                selectedAnnotations.select(parent, parent);
            } else {
                unselectedAnnotations.select(parent, parent);
            }
        }

        const int childCount = itemModel->rowCount(parent);
        for (int i = 0; i < childCount; ++i) {
            const QModelIndex child = itemModel->index(i, 0, parent);
            if (child.isValid()) {
                stack.push(child);
            }
        }
    }
    selectionModel->select(selectedAnnotations, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
    selectionModel->select(unselectedAnnotations, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void AnnotationsTreeView::updateAllAnnotations(ATVAnnUpdateFlags flags) {
    QString emptyFilter;
    for (int i = 0; i < tree->topLevelItemCount(); i++) {
        AVGroupItem *gi = static_cast<AVGroupItem *>(tree->topLevelItem(i));
        gi->updateAnnotations(emptyFilter, flags);
    }
}

void AnnotationsTreeView::addQualifierColumn(const QString &q) {
    TreeSorter ts(this);

    qColumns.append(q);
    int nColumns = headerLabels.size() + qColumns.size();
    tree->setColumnCount(nColumns);
    tree->setHeaderLabels(headerLabels + qColumns);
    tree->setColumnWidth(nColumns - 2, nColumns - 3 == 1 ? 200 : 100);
    updateAllAnnotations(ATVAnnUpdateFlag_QualColumns);

    updateState();
}

void AnnotationsTreeView::removeQualifierColumn(const QString &q) {
    bool ok = qColumns.removeOne(q);
    if (!ok) {
        return;
    }

    TreeSorter ts(this);

    tree->setColumnCount(headerLabels.size() + qColumns.size());
    tree->setHeaderLabels(headerLabels + qColumns);
    updateAllAnnotations(ATVAnnUpdateFlag_QualColumns);

    updateState();
}

#define COLUMN_NAMES "ATV_COLUMNS"

void AnnotationsTreeView::saveState(QVariantMap &map) const {
    map.insert(COLUMN_NAMES, QVariant(qColumns));

    QStringList columns = map.value(COLUMN_NAMES).toStringList();
    assert(columns == qColumns);
}

void AnnotationsTreeView::updateState(const QVariantMap &map) {
    QStringList columns = map.value(COLUMN_NAMES).toStringList();

    if (columns != qColumns && !columns.isEmpty()) {
        TreeSorter ts(this);
        foreach (QString q, qColumns) {
            removeQualifierColumn(q);
        }
        foreach (QString q, columns) {
            addQualifierColumn(q);
        }
    }
}

void AnnotationsTreeView::setSortingEnabled(bool v) {
    if (sortTimer.isActive()) {
        sortTimer.stop();
    }
    if (v) {
        sortTimer.start();
    } else {
        tree->setSortingEnabled(false);
    }
}

void AnnotationsTreeView::sl_sortTree() {
    tree->setSortingEnabled(true);
}

void AnnotationsTreeView::sl_edit() {
    AVItem *item = static_cast<AVItem *>(tree->currentItem());
    editItem(item);
}

void AnnotationsTreeView::editGroupItem(AVGroupItem *gi) {
    SAFE_POINT(gi != NULL, "Item is NULL", );
    SAFE_POINT(gi->group->getParentGroup() != NULL, "Attempting renaming of root group!", );    //not a root group
    QString oldName = gi->group->getName();
    QString newName = renameDialogHelper(gi, oldName, tr("Edit Group"));
    AnnotationGroup *parentAnnotationGroup = gi->group->getParentGroup();
    if (newName != oldName && AnnotationGroup::isValidGroupName(newName, false) && parentAnnotationGroup->getSubgroup(newName, false) == NULL) {
        gi->group->setName(newName);
        gi->group->getGObject()->setModified(true);
        gi->updateVisual();
    }
}

void AnnotationsTreeView::editQualifierItem(AVQualifierItem *qi) {
    SAFE_POINT(qi != NULL, "Item is NULL", );
    AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(qi->parent());
    U2Qualifier q;
    bool ro = qi->isReadonly();
    bool ok = editQualifierDialogHelper(qi, ro, q);
    QString simplifiedValue = AVQualifierItem::simplifyText(q.value);
    if (!ro && ok && (q.name != qi->qName || simplifiedValue != qi->qValue)) {
        Annotation *a = (static_cast<AVAnnotationItem *>(qi->parent()))->annotation;
        a->removeQualifier(U2Qualifier(qi->qName, qi->qValue));
        a->addQualifier(q);
        AVQualifierItem *qi = ai->findQualifierItem(q.name, q.value);
        tree->setCurrentItem(qi);
        tree->scrollToItem(qi);
    }
}

void AnnotationsTreeView::editAnnotationItem(AVAnnotationItem *ai) {
    QVector<U2Region> l = ai->annotation->getRegions();
    QList<ADVSequenceObjectContext *> soList = ctx->findRelatedSequenceContexts(ai->annotation->getGObject());
    SAFE_POINT(1 == soList.size(), "Invalid sequence context count!", );
    ADVSequenceObjectContext *so = soList.first();

    CreateAnnotationModel m;
    m.sequenceObjectRef = GObjectReference(so->getSequenceGObject());
    m.defaultIsNewDoc = false;
    m.groupName = ai->getAnnotationGroup()->getName();
    m.data = ai->annotation->getData();
    QString oldDescription = ai->annotation->findFirstQualifierValue(GBFeatureUtils::QUALIFIER_NOTE);
    m.description = oldDescription;
    m.hideAnnotationTableOption = true;
    m.hideGroupName = true;
    m.useAminoAnnotationTypes = so->getAminoTT() == NULL;

    m.annotationObjectRef = GObjectReference(ai->getAnnotationTableObject());
    m.sequenceLen = so->getSequenceLength();
    QObjectScopedPointer<CreateAnnotationDialog> dlg = new CreateAnnotationDialog(this, m);
    QString helpPageId = "17467567";    //! the line is important for the script that changes the documentation links
    dlg->updateAppearance(tr("Edit Annotation"), helpPageId, tr("Edit"));
    const int result = dlg->exec();

    if (result == QDialog::Accepted) {
        QString newName = m.data->name;
        if (newName != ai->annotation->getName()) {
            ai->annotation->setName(newName);
            QList<AVAnnotationItem *> ais = findAnnotationItems(ai->annotation);
            foreach (AVAnnotationItem *a, ais) {
                a->updateVisual(ATVAnnUpdateFlag_BaseColumns);
            }
        }
        U2Location location = m.data->location;
        if (!location->regions.isEmpty() && l != location->regions) {
            ai->annotation->updateRegions(location->regions);
        }

        U2FeatureType newType = m.data->type;
        if (newType != ai->annotation->getType()) {
            ai->annotation->setType(newType);
        }

        if ((m.description.isEmpty() && oldDescription != QString::null) || (m.description != oldDescription)) {
            ai->annotation->removeQualifier(U2Qualifier(GBFeatureUtils::QUALIFIER_NOTE, oldDescription));
        }
        if (!m.description.isEmpty() && m.description != oldDescription) {
            ai->annotation->addQualifier(U2Qualifier(GBFeatureUtils::QUALIFIER_NOTE, m.description));
        }

        ai->annotation->setLocationOperator(location->op);
        ai->annotation->setStrand(location->strand);
        ai->annotation->getGObject()->setModified(true);
    }
}

void AnnotationsTreeView::moveDialogToItem(QTreeWidgetItem *item, QDialog *d) {
    if (item == NULL) {
        return;
    }
    tree->scrollToItem(item);

    //try place dialog right below or right above the item
    d->layout()->update();
    QRect itemRect = tree->visualItemRect(item).translated(tree->viewport()->mapToGlobal(QPoint(0, 0)));
    QSize dialogSize = d->layout()->minimumSize();
    QRect dialogRect(0, 0, dialogSize.width(), dialogSize.height() + 35);    //+35 -> estimation for a title bar
    QRect widgetRect = rect().translated(mapToGlobal(QPoint(0, 0)));
    QRect finalDialogRect = dialogRect.translated(itemRect.bottomLeft());
    if (!widgetRect.contains(finalDialogRect)) {
        finalDialogRect = dialogRect.translated(itemRect.topLeft()).translated(QPoint(0, -dialogRect.height()));
    }
    if (widgetRect.contains(finalDialogRect)) {
        d->move(finalDialogRect.topLeft());
    }
}

QString AnnotationsTreeView::renameDialogHelper(AVItem *i, const QString &defText, const QString &title) {
    QObjectScopedPointer<QDialog> d = new QDialog(this);
    d->setWindowTitle(title);
    QVBoxLayout *l = new QVBoxLayout();
    d->setLayout(l);

    QLineEdit *edit = new QLineEdit(d.data());
    edit->setText(defText);
    edit->setSelection(0, defText.length());
    connect(edit, SIGNAL(returnPressed()), d.data(), SLOT(accept()));
    l->addWidget(edit);

    moveDialogToItem(i, d.data());

    const int rc = d->exec();
    CHECK(!d.isNull(), "");

    if (rc == QDialog::Rejected) {
        return defText;
    }
    return edit->text();
}

bool AnnotationsTreeView::editQualifierDialogHelper(AVQualifierItem *i, bool ro, U2Qualifier &q) {
    QObjectScopedPointer<EditQualifierDialog> d = new EditQualifierDialog(this, U2Qualifier(i == NULL ? "new_qualifier" : i->qName, i == NULL ? "" : i->qValue), ro, i != NULL);
    moveDialogToItem(i == NULL ? tree->currentItem() : i, d.data());
    const int rc = d->exec();
    CHECK(!d.isNull(), false);
    q = d->getModifiedQualifier();
    return rc == QDialog::Accepted;
}

void AnnotationsTreeView::editItem(AVItem *item) {
    if (item->isReadonly()) {
        return;
    }
    if (item->type == AVItemType_Group) {
        AVGroupItem *gi = static_cast<AVGroupItem *>(item);
        editGroupItem(gi);
    } else if (item->type == AVItemType_Annotation) {
        AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(item);
        editAnnotationItem(ai);
    } else {
        SAFE_POINT(item->type == AVItemType_Qualifier, "Unexpected annotation view item's qualifier!", );
        AVQualifierItem *qi = static_cast<AVQualifierItem *>(item);
        editQualifierItem(qi);
    }
}

void AnnotationsTreeView::sl_addQualifier() {
    AVItem *item = static_cast<AVItem *>(tree->currentItem());
    if (item->isReadonly() || item->type == AVItemType_Group) {
        return;
    }
    U2Qualifier q;
    bool ok = editQualifierDialogHelper(NULL, false, q);
    if (ok) {
        assert(!q.name.isEmpty());
        AVAnnotationItem *ai = item->type == AVItemType_Annotation ? static_cast<AVAnnotationItem *>(item) : static_cast<AVAnnotationItem *>(item->parent());
        Annotation *a = ai->annotation;
        a->addQualifier(q);
        ai->setExpanded(true);
        AVQualifierItem *qi = ai->findQualifierItem(q.name, q.value);
        tree->setCurrentItem(qi);
        tree->scrollToItem(qi);
    }
}

void AnnotationsTreeView::sl_annotationObjectModifiedStateChanged() {
    AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(sender());
    SAFE_POINT(NULL != ao, "Invalid annotation table!", );
    AVGroupItem *gi = findGroupItem(ao->getRootGroup());
    SAFE_POINT(NULL != gi, "Invalid annotation view item detected!", );
    gi->updateVisual();
}

AVItem *AnnotationsTreeView::currentItem() {
    return static_cast<AVItem *>(tree->currentItem());
}

void AnnotationsTreeView::sl_exportAutoAnnotationsGroup() {
    AVItem *item = static_cast<AVItem *>(tree->currentItem());
    AnnotationGroup *ag = item->getAnnotationGroup();

    ADVSequenceObjectContext *seqCtx = ctx->getSequenceInFocus();

    CreateAnnotationModel m;
    m.hideAnnotationType = true;
    m.hideAnnotationName = true;
    m.hideLocation = true;
    m.groupName = ag->getName();
    m.sequenceObjectRef = GObjectReference(seqCtx->getSequenceObject());

    QObjectScopedPointer<CreateAnnotationDialog> dlg = new CreateAnnotationDialog(this, m);
    dlg->setWindowTitle(tr("Create Permanent Annotation"));
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        ExportAutoAnnotationsGroupTask *task = new ExportAutoAnnotationsGroupTask(ag, m.annotationObjectRef, seqCtx, m.description);
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

//////////////////////////////////////////////////////////////////////////
/// Tree model

AVItem::AVItem(QTreeWidgetItem *parent, AVItemType _type)
    : QTreeWidgetItem(parent), type(_type) {
}

bool AVItem::isReadonly() const {
    SAFE_POINT(parent() != NULL, "Invalid annotation parent item!", false);
    return (static_cast<AVItem *>(parent())->isReadonly());
}

AnnotationsTreeView *AVItem::getAnnotationTreeView() const {
    SAFE_POINT(parent() != NULL, "Invalid annotation parent item!", NULL);
    return (static_cast<AVItem *>(parent())->getAnnotationTreeView());
}

AnnotationTableObject *AVItem::getAnnotationTableObject() const {
    SAFE_POINT(parent() != NULL, "Invalid annotation parent item!", NULL);
    return (static_cast<AVItem *>(parent())->getAnnotationTableObject());
}

AnnotationGroup *AVItem::getAnnotationGroup() {
    SAFE_POINT(parent() != NULL, "Invalid annotation parent item!", getAnnotationTableObject()->getRootGroup());
    return (static_cast<AVItem *>(parent())->getAnnotationGroup());
}

bool AVItem::processLinks(const QString &qName, const QString &qValue, int col) {
    bool linked = false;
    if (qName == "db_xref") {
        QStringList l = qValue.split(":");
        QString dbName = l[0];
        DBXRefInfo info = AppContext::getDBXRefRegistry()->getRefByKey(dbName);
        linked = !info.url.isEmpty();
        setToolTip(col, info.comment);
        if (linked) {
            setData(col, Qt::UserRole, true);
        }
    }

    if (linked) {
        QFont f = font(col);
        f.setUnderline(true);
        setFont(col, f);
        setForeground(col, Qt::blue);
    }
    return linked;
}

bool AVItem::isColumnLinked(int col) const {
    return data(col, Qt::UserRole).type() == QVariant::Bool;
}

QString AVItem::buildLinkURL(int col) const {
    assert(isColumnLinked(col));
    QString qValue = text(col);
    QStringList split = qValue.split(":");
    QString type = split.first();
    QString id = split.size() < 2 ? QString("") : split[1];
    QString url = AppContext::getDBXRefRegistry()->getRefByKey(type).url.arg(id);
    return url;
}

QString AVItem::getFileUrl(int col) const {
    assert(isColumnLinked(col));
    QStringList split = text(col).split(":");
    QString type = split.first();
    QString fileUrl = AppContext::getDBXRefRegistry()->getRefByKey(type).fileUrl;
    if (!fileUrl.isEmpty()) {
        QString id = split.size() < 2 ? QString("") : split[1];
        return fileUrl.arg(id);
    }
    return fileUrl;
}

AVGroupItem::AVGroupItem(AnnotationsTreeView *atv, AVGroupItem *parent, AnnotationGroup *g)
    : AVItem(parent, AVItemType_Group), group(g), atv(atv) {
    updateVisual();
}

const QIcon &AVGroupItem::getGroupIcon() {
    static QIcon groupIcon(":/core/images/group_green_active.png");
    return groupIcon;
}

const QIcon &AVGroupItem::getDocumentIcon() {
    static QIcon groupIcon(":/core/images/gobject.png");
    return groupIcon;
}

void AVGroupItem::updateVisual(int removedAnnotationCount) {
    SAFE_POINT(0 <= removedAnnotationCount, "Invalid removed item count!", )
    if (parent() == NULL) {    // document item
        AnnotationTableObject *aobj = group->getGObject();
        Document *doc = aobj->getDocument();
        QString text = aobj->getGObjectName();
        if (doc != NULL) {
            const QString docShortName = aobj->getDocument()->getName();
            SAFE_POINT(!docShortName.isEmpty(), "Invalid document name detected!", );
            text = group->getGObject()->getGObjectName() + " [" + docShortName + "]";
            if (aobj->isTreeItemModified()) {
                text += " *";
            }
        }
        setText(AnnotationsTreeView::COLUMN_NAME, text);
        setIcon(AnnotationsTreeView::COLUMN_NAME, getDocumentIcon());
        GUIUtils::setMutedLnF(this, !aobj->hasAnnotations(), false);
    } else {    // usual groups with annotations
        int na = group->getAnnotations().size() - removedAnnotationCount;
        int ng = group->getSubgroups().size();
        const QString nameString = group->getName() + "  " + QString("(%1, %2)").arg(ng).arg(na);
        setText(AnnotationsTreeView::COLUMN_NAME, nameString);
        setIcon(AnnotationsTreeView::COLUMN_NAME, getGroupIcon());

        // if all child items are muted -> mute this group too
        bool showDisabled = childCount() > 0;    //empty group is not disabled
        for (int i = 0; i < childCount(); i++) {
            QTreeWidgetItem *childItem = child(i);
            if (!GUIUtils::isMutedLnF(childItem)) {
                showDisabled = false;
                break;
            }
        }
        GUIUtils::setMutedLnF(this, showDisabled, false);
    }
}

void AVGroupItem::updateAnnotations(const QString &nameFilter, ATVAnnUpdateFlags f) {
    bool noFilter = nameFilter.isEmpty();
    for (int j = 0; j < childCount(); j++) {
        AVItem *item = static_cast<AVItem *>(child(j));
        if (item->type == AVItemType_Group) {
            AVGroupItem *level1 = static_cast<AVGroupItem *>(item);
            if (noFilter || level1->group->getName() == nameFilter) {
                level1->updateAnnotations(nameFilter, f);
            }
        } else {
            SAFE_POINT(item->type == AVItemType_Annotation, "Unexpected tree item type", );
            AVAnnotationItem *aItem = static_cast<AVAnnotationItem *>(item);
            if (noFilter || aItem->annotation->getName() == nameFilter) {
                aItem->updateVisual(f);
            }
        }
    }
}

bool AVGroupItem::isReadonly() const {
    //documents names are not editable
    GObject *obj = group->getGObject();
    bool readOnly = obj->isStateLocked() || AutoAnnotationsSupport::isAutoAnnotation(obj);
    return group->getParentGroup() == NULL ? true : readOnly;
}

void AVGroupItem::findAnnotationItems(QList<AVAnnotationItem *> &result, Annotation *a) const {
    for (int i = 0, n = childCount(); i < n; i++) {
        AVItem *item = static_cast<AVItem *>(child(i));
        if (item->type == AVItemType_Group) {
            AVGroupItem *gi = static_cast<AVGroupItem *>(item);
            gi->findAnnotationItems(result, a);
        } else if (item->type == AVItemType_Annotation) {
            AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(item);
            if (ai->annotation == a) {
                result.append(ai);
            }
        }
    }
}

AnnotationTableObject *AVGroupItem::getAnnotationTableObject() const {
    return group->getGObject();
}

AnnotationGroup *AVGroupItem::getAnnotationGroup() {
    return group;
}

AVAnnotationItem::AVAnnotationItem(AVGroupItem *parent, Annotation *a)
    : AVItem(parent, AVItemType_Annotation), annotation(a) {
    updateVisual(ATVAnnUpdateFlags(ATVAnnUpdateFlag_BaseColumns | ATVAnnUpdateFlag_QualColumns));
    hasNumericQColumns = false;
}

#define MAX_ICONS_CACHE_SIZE 500

QMap<QString, QIcon> &AVAnnotationItem::getIconsCache() {
    static QMap<QString, QIcon> iconsCache;
    return iconsCache;
}

void AVAnnotationItem::updateVisual(ATVAnnUpdateFlags f) {
    const SharedAnnotationData &aData = annotation->getData();
    AnnotationSettingsRegistry *registry = AppContext::getAnnotationsSettingsRegistry();
    const AnnotationSettings *as = registry->getAnnotationSettings(aData);

    if (f.testFlag(ATVAnnUpdateFlag_BaseColumns)) {
        QMap<QString, QIcon> &cache = getIconsCache();
        QIcon icon = cache.value(aData->name);
        if (icon.isNull()) {
            const QColor iconColor = as->visible ? as->color : Qt::lightGray;
            icon = GUIUtils::createSquareIcon(iconColor, 9);
            if (cache.size() > MAX_ICONS_CACHE_SIZE) {
                cache.clear();
            }
            cache[aData->name] = icon;
        }
        SAFE_POINT(!icon.isNull(), "Unable to initialize icon for annotation tree item!", );

        setIcon(AnnotationsTreeView::COLUMN_NAME, icon);
        setText(AnnotationsTreeView::COLUMN_NAME, aData->name);
        locationString = U1AnnotationUtils::buildLocationString(aData);
        setText(AnnotationsTreeView::COLUMN_TYPE, U2FeatureTypes::getVisualName(aData->type));
        setText(AnnotationsTreeView::COLUMN_VALUE, locationString);
    }

    if (f.testFlag(ATVAnnUpdateFlag_QualColumns)) {
        //setup custom qualifiers columns
        AnnotationsTreeView *atv = getAnnotationTreeView();
        SAFE_POINT(NULL != atv, "Invalid annotations tree view!", );
        const QStringList &colNames = atv->getQualifierColumnNames();
        hasNumericQColumns = false;
        for (int i = 0, n = colNames.size(); i < n; i++) {
            const int col = 3 + i;
            const QString colName = colNames[i];
            const QString colText = aData->findFirstQualifierValue(colName);
            setText(3 + i, colText);
            const bool linked = processLinks(colName, colText, col);
            if (!linked) {
                bool ok = false;
                double d = colText.toDouble(&ok);
                if (ok) {
                    setData(col, Qt::UserRole, d);
                    hasNumericQColumns = true;
                }
            }
        }
    }

    GUIUtils::setMutedLnF(this, !as->visible, true);
}

QVariant AVAnnotationItem::data(int col, int role) const {
    if (col == AnnotationsTreeView::COLUMN_VALUE && role == Qt::DisplayRole) {
        if (locationString.isEmpty()) {
            const SharedAnnotationData &data = annotation->getData();
            locationString = U1AnnotationUtils::buildLocationString(data);
        }
        return locationString;
    }

    return QTreeWidgetItem::data(col, role);
}

bool AVAnnotationItem::operator<(const QTreeWidgetItem &other) const {
    int col = treeWidget()->sortColumn();
    const AVItem &avItem = static_cast<const AVItem &>(other);
    if (avItem.type != AVItemType_Annotation) {
        return text(col) < other.text(col);
    }
    const AVAnnotationItem &ai = static_cast<const AVAnnotationItem &>(other);
    if (col == AnnotationsTreeView::COLUMN_NAME) {
        const SharedAnnotationData &aData1 = annotation->getData();
        const SharedAnnotationData &aData2 = ai.annotation->getData();
        if (aData1->name == aData2->name) {
            // for annotations with equal names we compare locations
            // this allows to avoid resorting on lazy qualifier loading
            return aData1->location->regions[0] < aData2->location->regions[0];
        }
        return aData1->name < aData2->name;
    }
    if (col == AnnotationsTreeView::COLUMN_VALUE || (isColumnNumeric(col) && ai.isColumnNumeric(col))) {
        double oval = ai.getNumericVal(col);
        double mval = getNumericVal(col);
        return mval < oval;
    }
    return text(col) < other.text(col);
}

bool AVAnnotationItem::isColumnNumeric(int col) const {
    if (col == AnnotationsTreeView::COLUMN_NAME) {
        return false;
    }
    if (col == AnnotationsTreeView::COLUMN_VALUE) {
        return true;
    }
    if (!hasNumericQColumns) {
        return false;
    }
    return data(col, Qt::UserRole).type() == QVariant::Double;
}

double AVAnnotationItem::getNumericVal(int col) const {
    if (col == AnnotationsTreeView::COLUMN_VALUE) {
        const U2Region &r = annotation->getLocation()->regions[0];
        return r.startPos;
    }
    bool ok = false;
    double d = data(col, Qt::UserRole).toDouble(&ok);
    assert(ok);
    return d;
}

void AVAnnotationItem::removeQualifier(const U2Qualifier &q) {
    AVQualifierItem *qi = findQualifierItem(q.name, q.value);
    CHECK(qi != NULL, );
    delete qi;

    updateVisual(ATVAnnUpdateFlag_QualColumns);
}

void AVAnnotationItem::addQualifier(const U2Qualifier &q) {
    AVQualifierItem *qi = new AVQualifierItem(this, q);
    Q_UNUSED(qi);
    updateVisual(ATVAnnUpdateFlag_QualColumns);
}

AVQualifierItem *AVAnnotationItem::findQualifierItem(const QString &name, const QString &val) const {
    QString simplifiedValue = AVQualifierItem::simplifyText(val);
    for (int i = 0, n = childCount(); i < n; i++) {
        AVQualifierItem *qi = static_cast<AVQualifierItem *>(child(i));
        if (qi->qName == name && qi->qValue == simplifiedValue) {
            return qi;
        }
    }
    return NULL;
}

AVQualifierItem::AVQualifierItem(AVAnnotationItem *parent, const U2Qualifier &q)
    : AVItem(parent, AVItemType_Qualifier), qName(q.name), qValue(q.value) {
    setText(AnnotationsTreeView::COLUMN_NAME, qName);
    qValue = simplifyText(qValue);
    setText(AnnotationsTreeView::COLUMN_VALUE, qValue);

    processLinks(qName, qValue, AnnotationsTreeView::COLUMN_VALUE);
}

QString AVQualifierItem::simplifyText(const QString &origValue) {
    QString res = origValue;
    res.replace("\t", "    ");
    res.replace("\r", "");
    res.replace("\n", " ");
    res = res.trimmed();
    return res;
}

FindQualifierTask::FindQualifierTask(AnnotationsTreeView *treeView, const FindQualifierTaskSettings &settings)
    : Task(tr("Searching for a qualifier"), TaskFlag_None), treeView(treeView), qname(settings.name), qvalue(settings.value), groupToSearchIn(settings.groupToSearchIn), isExactMatch(settings.isExactMatch), searchAll(settings.searchAll), foundResult(false), indexOfResult(settings.prevIndex), resultAnnotation(settings.prevAnnotation) {
}

void FindQualifierTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }

    stateInfo.setProgress(0);
    AVItem *rootGroup = groupToSearchIn;

    int childCount = rootGroup->childCount();
    int startIdx = getStartIndexGroup(rootGroup);
    for (int i = startIdx; i < childCount && !stateInfo.isCanceled(); i++) {
        bool found = false;
        stateInfo.setProgress(i / childCount);
        AVItem *child = static_cast<AVItem *>(rootGroup->child(i));
        if (child->type == AVItemType_Annotation) {
            findInAnnotation(child, found);
        } else if (child->type == AVItemType_Group) {
            findInGroup(child, found);
        }
        if (!foundQuals.isEmpty()) {
            if (!rootGroup->isExpanded()) {
                if (!toExpand.contains(rootGroup)) {
                    toExpand.enqueue(rootGroup);
                }
            }
            if (!searchAll) {
                break;
            }
        }
    }
    foundResult = !foundQuals.isEmpty();
    stateInfo.setProgress(100);
}
static inline bool matchWords(const QString &enteredW, const QString &realW, bool isExactMatch) {
    return enteredW.isEmpty() ? true : (isExactMatch ? 0 == realW.compare(enteredW, Qt::CaseInsensitive) : realW.contains(enteredW, Qt::CaseInsensitive));
}
void FindQualifierTask::findInAnnotation(AVItem *annotation, bool &found) {
    AVAnnotationItem *ai = static_cast<AVAnnotationItem *>(annotation);
    const QVector<U2Qualifier> &quals = ai->annotation->getQualifiers();
    int qual_size = quals.size();
    int startIdx = getStartIndexAnnotation(ai);
    for (int j = startIdx; j < qual_size && !stateInfo.isCanceled(); j++) {
        const U2Qualifier &qual = quals.at(j);
        QString simplifiedValue = AVQualifierItem::simplifyText(qual.value);
        bool matchName = matchWords(qname, qual.name, isExactMatch);
        bool matchValue = matchWords(qvalue, simplifiedValue, isExactMatch);
        bool match = matchName && matchValue;
        if (match) {
            //matched annotation is always first in the queue
            //toExpand.enqueue(annotation);
            found = true;
            resultAnnotation = annotation;
            indexOfResult = j;

            foundQuals.append(QPair<AVAnnotationItem *, int>(ai, indexOfResult));

            if (!searchAll) {
                break;
            }
        }
    }
}

void FindQualifierTask::findInGroup(AVItem *group, bool &found) {
    int startIdx = getStartIndexGroup(group);
    for (int i = startIdx; i < group->childCount() && !stateInfo.isCanceled(); i++) {
        found = false;
        AVItem *child = static_cast<AVItem *>(group->child(i));
        if (child->type == AVItemType_Annotation) {
            findInAnnotation(child, found);
        } else if (child->type == AVItemType_Group) {
            findInGroup(child, found);
        }
        if (found) {
            if (!group->isExpanded()) {
                if (!toExpand.contains(group)) {
                    toExpand.enqueue(group);
                }
            }
            if (!searchAll) {
                break;
            }
        }
    }
}

int FindQualifierTask::getStartIndexGroup(AVItem *group) {
    int result = 0;

    if (resultAnnotation) {
        AVItem *parentGroup = dynamic_cast<AVItem *>(resultAnnotation->parent());
        if (parentGroup) {
            if (parentGroup != group) {    //if parent group is a subgroup of group seek to its index
                int idx = group->indexOfChild(parentGroup);
                if (idx != -1) {
                    result = idx;
                }
            } else {    //if annotation is in the same group seek to its idx
                int idx = group->indexOfChild(resultAnnotation);
                if (idx != -1) {
                    result = idx;
                }
            }
        }
    }
    return result;
}

int FindQualifierTask::getStartIndexAnnotation(AVItem *annotation) {
    int result = 0;

    if (resultAnnotation) {
        if (resultAnnotation == annotation) {
            result = indexOfResult + 1;    //start from the next qualifier in the annotation
        }
    }

    return result;
}
typedef QPair<AVAnnotationItem *, int> QualPair;

Task::ReportResult FindQualifierTask::report() {
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    AVItem *qual = NULL;
    int qualsSize = foundQuals.size();
    if (qualsSize > 0) {
        treeView->getTreeWidget()->clearSelection();
    }

    foreach (const QualPair &p, foundQuals) {
        AVAnnotationItem *ai = p.first;
        SAFE_POINT(NULL != ai, L10N::nullPointerError("annotation item"), ReportResult_Finished);
        if (!ai->isExpanded()) {
            treeView->getTreeWidget()->expandItem(ai);
            treeView->sl_itemExpanded(ai);
        }

        if (foundResult && ai) {
            const U2Qualifier u2qual = ai->annotation->getQualifiers().at(p.second);
            qual = ai->findQualifierItem(u2qual.name, u2qual.value);
            if (qual == NULL) {
                continue;
            }

            qual->setSelected(true);
            qual->parent()->setSelected(true);
        }
        if (isCanceled()) {
            return ReportResult_Finished;
        }
    }

    foreach (AVItem *item, toExpand) {
        treeView->getTreeWidget()->expandItem(item);
    }

    if (qual && qualsSize == 1) {
        treeView->getTreeWidget()->scrollToItem(qual);
    }

    return ReportResult_Finished;
}

}    // namespace U2
