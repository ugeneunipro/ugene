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

#include <QMainWindow>
#include <QTreeWidget>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <U2View/AnnotationsTreeView.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsAnnotationsTreeView"

const QString GTUtilsAnnotationsTreeView::widgetName = "annotations_tree_widget";

QTreeWidget* GTUtilsAnnotationsTreeView::getTreeWidget(HI::GUITestOpStatus &os) {

    QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, widgetName, GTUtilsMdi::activeWindow(os)));
    return treeWidget;
}

#define GT_METHOD_NAME "addAnnotationsTableFromProject"
void GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject(HI::GUITestOpStatus &os, const QString &tableName) {
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, tableName), GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAVItemName"
QString GTUtilsAnnotationsTreeView::getAVItemName(HI::GUITestOpStatus &os, AVItem* avItem) {

    GT_CHECK_RESULT(avItem != NULL, "avItem is NULL", "");

    switch (avItem->type) {
        case AVItemType_Annotation:
            {
                AVAnnotationItem* avAnnotationItem = (AVAnnotationItem*)avItem;
                GT_CHECK_RESULT(avAnnotationItem != NULL, "avAnnotationItem is NULL", "");

                Annotation *annotation = avAnnotationItem->annotation;
                return annotation->getName();
            }
            break;

        case AVItemType_Group:
            {
                AVGroupItem* avGroupItem = (AVGroupItem*)avItem;
                GT_CHECK_RESULT(avGroupItem!= NULL, "avAnnotationItem is NULL", "");

                AnnotationGroup *group= avGroupItem->group;
                return group->getName();
            }
            break;

        case AVItemType_Qualifier:
            {
                AVQualifierItem* avQualifierItem = (AVQualifierItem*)avItem;
                GT_CHECK_RESULT(avQualifierItem != NULL, "avQualifierItem is NULL", "");
                return avQualifierItem->qName;
            }
            break;

        default:
            break;
    }

    return "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getQualifierValue"
QString GTUtilsAnnotationsTreeView::getQualifierValue(HI::GUITestOpStatus &os, const QString &qualifierName, QTreeWidgetItem *parentItem) {
    GTTreeWidget::getItemCenter(os, parentItem);
    QTreeWidgetItem *qualifierItem = findItem(os, qualifierName, parentItem);
    GT_CHECK_RESULT(NULL != qualifierItem, "Qualifier item not found", "");
    return qualifierItem->text(AnnotationsTreeView::COLUMN_VALUE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getQualifierValue"
QString GTUtilsAnnotationsTreeView::getQualifierValue(HI::GUITestOpStatus &os, const QString &qualName, const QString &parentName) {
    getItemCenter(os, parentName);
    QTreeWidgetItem* parent = findItem(os, parentName);
    GT_CHECK_RESULT(parent != NULL, "Parent item not found", "");

    QTreeWidgetItem *qualItem = findItem(os, qualName, parent);
    GT_CHECK_RESULT(NULL != qualItem, "Qualifier item not found", "");
    return qualItem->text(AnnotationsTreeView::COLUMN_VALUE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegions"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegions(HI::GUITestOpStatus &os) {
    QList<U2Region> res;

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", res);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        AVAnnotationItem* annotationItem = dynamic_cast<AVAnnotationItem*>(item);
        CHECK_OPERATION(annotationItem != NULL, continue);

        Annotation *ann = annotationItem->annotation;
        res.append(ann->getRegions().toList());
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedAnnotatedRegions"
QList<U2Region> GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(HI::GUITestOpStatus &os) {
    QList<U2Region> res;

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", res);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach(QTreeWidgetItem* item, treeItems) {
        AVAnnotationItem* annotationItem = dynamic_cast<AVAnnotationItem*>(item);
        CHECK_OPERATION(annotationItem != NULL, continue);
        CHECK_CONTINUE(annotationItem->isSelected());

        Annotation *ann = annotationItem->annotation;
        res.append(ann->getRegions().toList());
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationRegionString"
QString GTUtilsAnnotationsTreeView::getAnnotationRegionString(HI::GUITestOpStatus &os, const QString &annotationName) {
    QTreeWidgetItem * annotationItem = findItem(os, annotationName);
    GT_CHECK_RESULT(NULL != annotationItem, "Annotation item is NULL", "");
    return annotationItem->text(AnnotationsTreeView::COLUMN_VALUE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationType"
QString GTUtilsAnnotationsTreeView::getAnnotationType(HI::GUITestOpStatus &os, const QString &annotationName) {
    QTreeWidgetItem * annotationItem = findItem(os, annotationName);
    GT_CHECK_RESULT(NULL != annotationItem, "Annotation item is NULL", "");
    return annotationItem->text(AnnotationsTreeView::COLUMN_TYPE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findFirstAnnotation"
QTreeWidgetItem * GTUtilsAnnotationsTreeView::findFirstAnnotation(HI::GUITestOpStatus &os, const GTGlobals::FindOptions &options) {
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem *item, treeItems) {
        AVItem *avItem = dynamic_cast<AVItem*>(item);
        GT_CHECK_RESULT(NULL != avItem, "Cannot convert QTreeWidgetItem to AVItem", NULL);
        if (AVItemType_Annotation == avItem->type) {
            return item;
        }
    }
    GT_CHECK_RESULT(options.failIfNotFound == false, "No items in tree widget", NULL);
    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem * GTUtilsAnnotationsTreeView::findItem(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options) {

    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", NULL);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            return item;
        }
    }
    GT_CHECK_RESULT(options.failIfNotFound == false, QString("Item '%1' not found in tree widget").arg(itemName), NULL);

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTUtilsAnnotationsTreeView::findItemWithIndex(HI::GUITestOpStatus &os, const QString &itemName, const int index) {
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", NULL);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    int i = 0;
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            i++;
            if (i == index) {
                return item;
            }
        }
    }

    return NULL;
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "findItem"
QTreeWidgetItem * GTUtilsAnnotationsTreeView::findItem(HI::GUITestOpStatus &os, const QString &itemName, QTreeWidgetItem* parentItem, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", NULL);

    if (parentItem == NULL) {
        return findItem(os, itemName, options);
    }

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(parentItem);
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            return item;
        }
    }
    GT_CHECK_RESULT(options.failIfNotFound == false, "Item " + itemName + " not found in tree widget", NULL);

    return NULL;
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItems"
QList<QTreeWidgetItem*> GTUtilsAnnotationsTreeView::findItems(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options) {
    QList<QTreeWidgetItem*> result;
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", result);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", result);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            result.append(item);
        }
    }
    GT_CHECK_RESULT(!options.failIfNotFound || !result.isEmpty(), "Item " + itemName + " not found in tree widget", result);

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGroupNames"
QStringList GTUtilsAnnotationsTreeView::getGroupNames(HI::GUITestOpStatus &os, const QString &annotationTableName) {
    QList<QTreeWidgetItem *> annotationTableItems;
    if (!annotationTableName.isEmpty()) {
        annotationTableItems << findItem(os, annotationTableName);
    } else {
        QTreeWidget *treeWidget = getTreeWidget(os);
        QTreeWidgetItem *rootItem = treeWidget->invisibleRootItem();
        for (int i = 0; i < rootItem->childCount(); i++) {
            annotationTableItems << rootItem->child(i);
        }
    }

    QStringList groupNames;
    foreach (QTreeWidgetItem *annotationTableItem, annotationTableItems) {
        for (int i = 0; i < annotationTableItem->childCount(); i++) {
            groupNames << annotationTableItem->child(i)->text(AnnotationsTreeView::COLUMN_NAME);
        }
    }

    return groupNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationNamesOfGroup"
QStringList GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(HI::GUITestOpStatus &os, const QString &groupName) {
    Q_UNUSED(os);
    QStringList names;
    QTreeWidgetItem *groupItem = findItem(os, groupName);
    for (int i = 0; i < groupItem->childCount(); i++) {
        AVItem *avItem = dynamic_cast<AVItem *>(groupItem->child(i));
        GT_CHECK_RESULT(NULL != avItem, "Cannot convert QTreeWidgetItem to AVItem", QStringList());
        names << getAVItemName(os, avItem);
    }
    return names;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegionsOfGroup"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(HI::GUITestOpStatus &os, const QString &groupName) {
    Q_UNUSED(os);
    QList<U2Region> regions;
    QTreeWidgetItem *groupItem = findItem(os, groupName);
    GT_CHECK_RESULT(groupItem != NULL, QString("Cannot find group item '%1'").arg(groupName), regions);
    for (int i = 0; i < groupItem->childCount(); i++) {
        AVItem *avItem = dynamic_cast<AVItem *>(groupItem->child(i));
        GT_CHECK_RESULT(NULL != avItem, "Cannot convert QTreeWidgetItem to AVItem", QList<U2Region>());
        AVAnnotationItem* item = (AVAnnotationItem*)avItem;
        GT_CHECK_RESULT(item != NULL, "sdf", regions);
        regions << item->annotation->getRegions().toList();
    }
    return regions;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegionsOfGroup"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(HI::GUITestOpStatus &os, const QString &groupName, const QString &parentName) {
    Q_UNUSED(os);
    QList<U2Region> regions;
    QTreeWidgetItem *parentItem = findItem(os, parentName);
    GT_CHECK_RESULT( parentItem != NULL, "Parent item not found!", regions);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(parentItem);
    foreach (QTreeWidgetItem* childItem, treeItems) {
        QString treeItemName = childItem->text(0);
        if (treeItemName == groupName) {
            for (int i = 0; i < childItem->childCount(); i++) {
                AVItem *avItem = dynamic_cast<AVItem *>(childItem->child(i));
                GT_CHECK_RESULT(NULL != avItem, "Cannot convert QTreeWidgetItem to AVItem", QList<U2Region>());
                AVAnnotationItem* item = (AVAnnotationItem*)avItem;
                GT_CHECK_RESULT(item != NULL, "sdf", regions);
                regions << item->annotation->getRegions().toList();
            }
        }
    }
    return regions;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findRegion"
bool GTUtilsAnnotationsTreeView::findRegion(HI::GUITestOpStatus &os, const QString &itemName, const U2Region& r) {
    AVAnnotationItem* item = (AVAnnotationItem*)GTUtilsAnnotationsTreeView::findItem(os, itemName);
    CHECK_SET_ERR_RESULT(item != NULL, "Item " + itemName + " not found", false);
    Annotation *ann = item->annotation;

    U2Region neededRegion(r.startPos - 1, r.length - r.startPos + 1);

    bool found = false;
    QVector<U2Region> regions = ann->getRegions();
    foreach (const U2Region& r, regions) {
        if (r.contains(neededRegion)) {
            found = true;
            break;
        }
    }

    return found;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedItem"
QString GTUtilsAnnotationsTreeView::getSelectedItem(HI::GUITestOpStatus &os)
{
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        if (item->isSelected()) {
            return getAVItemName(os, dynamic_cast<AVItem *>(item));
        }
    }

    return QString();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAllSelectedItem"
QList<QTreeWidgetItem*> GTUtilsAnnotationsTreeView::getAllSelectedItems(HI::GUITestOpStatus &os)
{
    QList<QTreeWidgetItem*> res;

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", res);

    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    foreach (QTreeWidgetItem* item, treeItems) {
        if (item->isSelected()) {
            QString s = item->text(0);
            res << item;
        }
    }

    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsAnnotationsTreeView::getItemCenter(HI::GUITestOpStatus &os, const QString &itemName) {

    QTreeWidgetItem* item = findItem(os, itemName);
    GT_CHECK_RESULT(item != NULL, "Item " + itemName + " is NULL", QPoint());

    return GTTreeWidget::getItemCenter(os, item);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createQualifier"
void GTUtilsAnnotationsTreeView::createQualifier(HI::GUITestOpStatus &os, const QString &qualName, const QString &qualValue, const QString &parentName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ADD" << "add_qualifier_action"));
    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, qualName, qualValue));
    GTMouseDriver::moveTo(getItemCenter(os, parentName));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItems"
void GTUtilsAnnotationsTreeView::selectItems(HI::GUITestOpStatus &os, const QStringList &items) {
    GT_CHECK_RESULT(items.size() != 0, "List of items to select is empty", );
    // remove previous selection
    QPoint p = getItemCenter(os, items.first());
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    foreach (const QString& item, items) {
        QPoint p = getItemCenter(os, item);
        GTMouseDriver::moveTo(p);

        QTreeWidgetItem* treeItem = findItem(os, item);
        GT_CHECK_RESULT(treeItem != NULL, "Tree item is NULL", );
        if (!treeItem->isSelected()) {
            GTMouseDriver::click();
        }
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItems"
void GTUtilsAnnotationsTreeView::selectItems(HI::GUITestOpStatus &os, const QList<QTreeWidgetItem *> &items) {
    GT_CHECK_RESULT(items.size() != 0, "List of items to select is empty", );

    // remove previous selection
    QPoint p = GTTreeWidget::getItemCenter(os, items.first());
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    GTKeyboardDriver::keyPress(Qt::Key_Control);

    foreach (QTreeWidgetItem *item, items) {
        const QPoint p = GTTreeWidget::getItemCenter(os, item);
        GTMouseDriver::moveTo(p);

        GT_CHECK_RESULT(item != NULL, "Tree item is NULL", );
        if (!item->isSelected()) {
            GTMouseDriver::click();
        }
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickItem"
void GTUtilsAnnotationsTreeView::clickItem(HI::GUITestOpStatus &os, const QString &item, const int numOfItem, bool isDoubleClick) {
    GT_CHECK_RESULT(!item.isEmpty(), "Empty item name", );

    QTreeWidgetItem* wgtItem = findItemWithIndex(os, item, numOfItem);
    GT_CHECK_RESULT(wgtItem != NULL, "Item " + item + " is NULL", );

    QPoint p = GTTreeWidget::getItemCenter(os, wgtItem);
    GTMouseDriver::moveTo(p);
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click();
    }
    GTGlobals::sleep();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createAnnotation"
void GTUtilsAnnotationsTreeView::createAnnotation(HI::GUITestOpStatus &os, const QString &groupName, const QString &annotationName, const QString &location, bool createNewTable, const QString &saveTo) {
    QTreeWidget *annotationsTreeView = getTreeWidget(os);
    GT_CHECK(NULL != annotationsTreeView, "No annotation tree view");
    GTWidget::click(os, annotationsTreeView);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, createNewTable, groupName, annotationName, location, saveTo));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteItem"
void GTUtilsAnnotationsTreeView::deleteItem(HI::GUITestOpStatus &os, const QString &itemName) {
    deleteItem(os, findItem(os, itemName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteItem"
void GTUtilsAnnotationsTreeView::deleteItem(HI::GUITestOpStatus &os, QTreeWidgetItem *item) {
    GT_CHECK(item != NULL, "Item is NULL");
    selectItems(os, QList<QTreeWidgetItem *>() << item);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnItem"
void GTUtilsAnnotationsTreeView::callContextMenuOnItem(HI::GUITestOpStatus &os, QTreeWidgetItem *item) {
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnItem"
void GTUtilsAnnotationsTreeView::callContextMenuOnItem(HI::GUITestOpStatus &os, const QString &itemName) {
    GTMouseDriver::moveTo(getItemCenter(os, itemName));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnQualifier"
void GTUtilsAnnotationsTreeView::callContextMenuOnQualifier(HI::GUITestOpStatus &os, const QString &parentName, const QString &qualifierName) {
    getItemCenter(os, parentName);
    callContextMenuOnItem(os, qualifierName);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
