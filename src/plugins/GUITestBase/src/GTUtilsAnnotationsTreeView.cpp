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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QMainWindow>
#include <QTreeWidget>

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotationsTreeView.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditQualifierDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsAnnotationsTreeView"

#define GT_METHOD_NAME "getTreeWidget"
QTreeWidget* GTUtilsAnnotationsTreeView::getTreeWidget(HI::GUITestOpStatus& os) {
    QWidget* sequenceView = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    return GTWidget::findTreeWidget(os, "annotations_tree_widget", sequenceView);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addAnnotationsTableFromProject"
void GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject(HI::GUITestOpStatus& os, const QString& tableName) {
    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, GTUtilsProjectTreeView::findIndex(os, tableName), GTUtilsAnnotationsTreeView::getTreeWidget(os));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAVItemName"
QString GTUtilsAnnotationsTreeView::getAVItemName(HI::GUITestOpStatus& os, AVItem* avItem) {
    GT_CHECK_RESULT(avItem != nullptr, "avItem is NULL", "");

    switch (avItem->type) {
        case AVItemType_Annotation: {
            auto avAnnotationItem = dynamic_cast<AVAnnotationItem*>(avItem);
            GT_CHECK_RESULT(avAnnotationItem != nullptr, "avAnnotationItem is NULL", "");
            return avAnnotationItem->annotation->getName();
        } break;

        case AVItemType_Group: {
            auto avGroupItem = dynamic_cast<AVGroupItem*>(avItem);
            GT_CHECK_RESULT(avGroupItem != nullptr, "avAnnotationItem is NULL", "");
            return avGroupItem->group->getName();
        } break;

        case AVItemType_Qualifier: {
            auto avQualifierItem = dynamic_cast<AVQualifierItem*>(avItem);
            GT_CHECK_RESULT(avQualifierItem != nullptr, "avQualifierItem is NULL", "");
            return avQualifierItem->qName;
        } break;

        default:
            break;
    }

    return "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getQualifierValue"
QString GTUtilsAnnotationsTreeView::getQualifierValue(HI::GUITestOpStatus& os, const QString& qualifierName, QTreeWidgetItem* annotationItem) {
    GT_CHECK_RESULT(annotationItem != nullptr, "annotationItem item is null", "");
    GTTreeWidget::expand(os, annotationItem);
    QTreeWidgetItem* qualifierItem = findItem(os, qualifierName, annotationItem);
    return qualifierItem->text(AnnotationsTreeView::COLUMN_VALUE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getQualifierValue"
QString GTUtilsAnnotationsTreeView::getQualifierValue(HI::GUITestOpStatus& os, const QString& qualifierName, const QString& annotationName) {
    return getQualifierValue(os, qualifierName, findItem(os, annotationName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegions"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegions(HI::GUITestOpStatus& os) {
    QTreeWidget* treeWidget = getTreeWidget(os);
    QList<U2Region> res;
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        auto annotationItem = dynamic_cast<AVAnnotationItem*>(item);
        CHECK_CONTINUE(annotationItem != nullptr);
        res.append(annotationItem->annotation->getRegions().toList());
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedAnnotatedRegions"
QList<U2Region> GTUtilsAnnotationsTreeView::getSelectedAnnotatedRegions(HI::GUITestOpStatus& os) {
    QTreeWidget* treeWidget = getTreeWidget(os);
    QList<U2Region> res;
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        auto annotationItem = dynamic_cast<AVAnnotationItem*>(item);
        CHECK_CONTINUE(annotationItem != nullptr);
        CHECK_CONTINUE(annotationItem->isSelected());
        res.append(annotationItem->annotation->getRegions().toList());
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationRegionString"
QString GTUtilsAnnotationsTreeView::getAnnotationRegionString(HI::GUITestOpStatus& os, const QString& annotationName) {
    QTreeWidgetItem* annotationItem = findItem(os, annotationName);
    GT_CHECK_RESULT(nullptr != annotationItem, "Annotation item is NULL", "");
    return annotationItem->text(AnnotationsTreeView::COLUMN_VALUE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationType"
QString GTUtilsAnnotationsTreeView::getAnnotationType(HI::GUITestOpStatus& os, const QString& annotationName) {
    QTreeWidgetItem* annotationItem = findItem(os, annotationName);
    return annotationItem->text(AnnotationsTreeView::COLUMN_TYPE);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findFirstAnnotation"
QTreeWidgetItem* GTUtilsAnnotationsTreeView::findFirstAnnotation(HI::GUITestOpStatus& os, const GTGlobals::FindOptions& options, bool expandParent) {
    QTreeWidget* treeWidget = getTreeWidget(os);
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
        for (QTreeWidgetItem* item : qAsConst(treeItems)) {
            auto avItem = dynamic_cast<AVItem*>(item);
            GT_CHECK_RESULT(avItem != nullptr, "Cannot convert QTreeWidgetItem to AVItem", nullptr);
            if (avItem->type == AVItemType_Annotation) {
                if (expandParent) {
                    GTTreeWidget::expand(os, avItem->parent());
                }
                return item;
            }
        }
        if (!options.failIfNotFound) {
            return nullptr;
        }
    }
    GT_FAIL("Tree widget is empty", nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTUtilsAnnotationsTreeView::findItemWithIndex(HI::GUITestOpStatus& os, const QString& itemName, int index, bool expandParent) {
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", nullptr);

    QTreeWidget* treeWidget = getTreeWidget(os);
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    int i = 0;
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        QString treeItemName = item->text(0);
        if (treeItemName == itemName) {
            i++;
            if (i == index) {
                if (expandParent && item->parent() != nullptr) {
                    GTTreeWidget::expand(os, item->parent());
                }
                return item;
            }
        }
    }

    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTUtilsAnnotationsTreeView::findItem(HI::GUITestOpStatus& os,
                                                      const QString& itemName,
                                                      QTreeWidgetItem* parentItem,
                                                      const GTGlobals::FindOptions& options,
                                                      bool expandParent) {
    GT_CHECK_RESULT(!itemName.isEmpty(), "Item name is empty", nullptr);
    if (parentItem == nullptr) {
        parentItem = getTreeWidget(os)->invisibleRootItem();
    }
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(parentItem);
        for (QTreeWidgetItem* item : qAsConst(treeItems)) {
            QString treeItemName = item->text(0);
            if (treeItemName == itemName) {
                if (expandParent && item->parent() != nullptr) {
                    GTTreeWidget::expand(os, item->parent());
                }
                return item;
            }
        }
        if (!options.failIfNotFound) {
            return nullptr;
        }
    }
    GT_FAIL("Item " + itemName + " not found in tree widget", nullptr);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItems"
QList<QTreeWidgetItem*> GTUtilsAnnotationsTreeView::findItems(HI::GUITestOpStatus& os, const QString& itemName, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(!itemName.isEmpty(), "Item name is empty", {});

    QList<QTreeWidgetItem*> result;
    QTreeWidget* treeWidget = getTreeWidget(os);
    for (int time = 0; time < GT_OP_WAIT_MILLIS && result.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
        for (QTreeWidgetItem* item : qAsConst(treeItems)) {
            QString treeItemName = item->text(0);
            if (treeItemName == itemName) {
                result.append(item);
            }
        }
        if (!options.failIfNotFound) {
            return result;
        }
    }
    GT_CHECK_RESULT(!result.isEmpty(), "Item '" + itemName + "' was not found in tree widget", result);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandItem"
QTreeWidgetItem* GTUtilsAnnotationsTreeView::expandItem(HI::GUITestOpStatus& os, const QString& itemName) {
    auto item = findItem(os, itemName);
    GTTreeWidget::expand(os, item);
    return item;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGroupNames"
QStringList GTUtilsAnnotationsTreeView::getGroupNames(HI::GUITestOpStatus& os, const QString& annotationTableName) {
    QList<QTreeWidgetItem*> annotationTableItems;
    if (!annotationTableName.isEmpty()) {
        annotationTableItems << findItem(os, annotationTableName);
    } else {
        QTreeWidget* treeWidget = getTreeWidget(os);
        QTreeWidgetItem* rootItem = treeWidget->invisibleRootItem();
        for (int i = 0; i < rootItem->childCount(); i++) {
            annotationTableItems << rootItem->child(i);
        }
    }

    QStringList groupNames;
    for (QTreeWidgetItem* annotationTableItem : qAsConst(annotationTableItems)) {
        for (int i = 0; i < annotationTableItem->childCount(); i++) {
            groupNames << annotationTableItem->child(i)->text(AnnotationsTreeView::COLUMN_NAME);
        }
    }
    return groupNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotationNamesOfGroup"
QStringList GTUtilsAnnotationsTreeView::getAnnotationNamesOfGroup(HI::GUITestOpStatus& os, const QString& groupName) {
    QStringList names;
    QTreeWidgetItem* groupItem = findItem(os, groupName);
    for (int i = 0; i < groupItem->childCount(); i++) {
        auto avItem = dynamic_cast<AVItem*>(groupItem->child(i));
        GT_CHECK_RESULT(avItem != nullptr, "Cannot convert QTreeWidgetItem to AVItem", QStringList());
        names << getAVItemName(os, avItem);
    }
    return names;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegionsOfGroup"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(HI::GUITestOpStatus& os, const QString& groupName) {
    QList<U2Region> regions;
    QTreeWidgetItem* groupItem = findItem(os, groupName);
    for (int i = 0; i < groupItem->childCount(); i++) {
        auto avItem = dynamic_cast<AVItem*>(groupItem->child(i));
        GT_CHECK_RESULT(avItem != nullptr, "Cannot convert QTreeWidgetItem to AVItem", {});
        auto item = dynamic_cast<AVAnnotationItem*>(avItem);
        GT_CHECK_RESULT(item != nullptr, "sdf", regions);
        regions << item->annotation->getRegions().toList();
    }
    return regions;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAnnotatedRegionsOfGroup"
QList<U2Region> GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup(HI::GUITestOpStatus& os, const QString& groupName, const QString& parentName) {
    QList<U2Region> regions;
    QTreeWidgetItem* parentItem = findItem(os, parentName);
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(parentItem);
    for (QTreeWidgetItem* childItem : qAsConst(treeItems)) {
        QString treeItemName = childItem->text(0);
        if (treeItemName == groupName) {
            for (int i = 0; i < childItem->childCount(); i++) {
                auto avItem = dynamic_cast<AVItem*>(childItem->child(i));
                GT_CHECK_RESULT(nullptr != avItem, "Cannot convert QTreeWidgetItem to AVItem", {});
                auto item = dynamic_cast<AVAnnotationItem*>(avItem);
                GT_CHECK_RESULT(item != nullptr, "sdf", regions);
                regions << item->annotation->getRegions().toList();
            }
        }
    }
    return regions;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findRegion"
bool GTUtilsAnnotationsTreeView::findRegion(HI::GUITestOpStatus& os, const QString& itemName, const U2Region& r) {
    auto item = dynamic_cast<AVAnnotationItem*>(GTUtilsAnnotationsTreeView::findItem(os, itemName));
    CHECK_SET_ERR_RESULT(item != nullptr, "Item " + itemName + " not found", false);
    Annotation* ann = item->annotation;

    U2Region neededRegion(r.startPos - 1, r.length - r.startPos + 1);

    bool found = false;
    QVector<U2Region> regions = ann->getRegions();
    for (const U2Region& annotationRegion : qAsConst(regions)) {
        if (annotationRegion.contains(neededRegion)) {
            found = true;
            break;
        }
    }
    return found;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedItem"
QString GTUtilsAnnotationsTreeView::getSelectedItem(HI::GUITestOpStatus& os) {
    QTreeWidget* treeWidget = getTreeWidget(os);
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        if (item->isSelected()) {
            return getAVItemName(os, dynamic_cast<AVItem*>(item));
        }
    }
    return "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getAllSelectedItem"
QList<QTreeWidgetItem*> GTUtilsAnnotationsTreeView::getAllSelectedItems(HI::GUITestOpStatus& os) {
    QList<QTreeWidgetItem*> res;

    QTreeWidget* treeWidget = getTreeWidget(os);
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
    for (QTreeWidgetItem* item : qAsConst(treeItems)) {
        if (item->isSelected()) {
            res << item;
        }
    }
    return res;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsAnnotationsTreeView::getItemCenter(HI::GUITestOpStatus& os, const QString& itemName) {
    QTreeWidgetItem* item = findItem(os, itemName);
    return GTTreeWidget::getItemCenter(os, item);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createQualifier"
void GTUtilsAnnotationsTreeView::createQualifier(HI::GUITestOpStatus& os, const QString& qualifierName, const QString& qualifierValue, const QString& annotationName) {
    QList<QTreeWidgetItem*> annotations = findItems(os, {annotationName});
    CHECK_SET_ERR(annotations.size() == 1,
                  "createQualifier can be run only for a 1 annotation today. Found: " +
                      QString::number(annotations.size()) + " with name: " + annotationName);
    createQualifier(os, qualifierName, qualifierValue, annotations[0]);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createQualifier"
void GTUtilsAnnotationsTreeView::createQualifier(HI::GUITestOpStatus& os, const QString& qualifierName, const QString& qualifierValue, QTreeWidgetItem* annotation) {
    selectItems(os, {annotation});
    GTUtilsDialog::waitForDialog(os, new EditQualifierFiller(os, qualifierName, qualifierValue));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"ADV_MENU_ADD", "add_qualifier_action"}));
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, annotation));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemsByText"
void GTUtilsAnnotationsTreeView::selectItemsByName(HI::GUITestOpStatus& os, const QStringList& itemNames) {
    GT_CHECK_RESULT(!itemNames.empty(), "List of items to select is empty", );
    QList<QTreeWidgetItem*> items;
    for (const QString& name : qAsConst(itemNames)) {
        items << findItems(os, name);
    }
    selectItems(os, items);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItems"
void GTUtilsAnnotationsTreeView::selectItems(HI::GUITestOpStatus& os, const QList<QTreeWidgetItem*>& items) {
    GT_CHECK_RESULT(!items.empty(), "List of items to select is empty", );

    // Prepare the tree: make all items accessible (with parents expanded).
    for (QTreeWidgetItem* item : qAsConst(items)) {
        QTreeWidgetItem* parentItem = item->parent();
        if (parentItem != nullptr) {
            GTTreeWidget::expand(os, parentItem);
        }
    }

    // Click on the first item to remove current selection. After this point only the first item is selected.
    GTTreeWidget::scrollToItem(os, items.first());
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, items.first()));
    GTMouseDriver::click();

    for (QTreeWidgetItem* item : qAsConst(items)) {
        if (!item->isSelected()) {
            GTTreeWidget::scrollToItem(os, item);
            GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
            GTKeyboardDriver::keyPress(Qt::Key_Control);
            GTMouseDriver::click();
            GTKeyboardDriver::keyRelease(Qt::Key_Control);
            GTThread::waitForMainThread();
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickItem"
void GTUtilsAnnotationsTreeView::clickItem(HI::GUITestOpStatus& os, const QString& itemName, int itemIndex, bool isDoubleClick) {
    GT_CHECK_RESULT(!itemName.isEmpty(), "Empty item name", );

    QTreeWidgetItem* item = findItemWithIndex(os, itemName, itemIndex);
    GTTreeWidget::scrollToItem(os, item);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    if (isDoubleClick) {
        GTMouseDriver::doubleClick();
    } else {
        GTMouseDriver::click();
    }
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createAnnotation"
void GTUtilsAnnotationsTreeView::createAnnotation(HI::GUITestOpStatus& os, const QString& groupName, const QString& annotationName, const QString& location, bool createNewTable, const QString& saveTo) {
    QTreeWidget* annotationsTreeView = getTreeWidget(os);
    GTWidget::click(os, annotationsTreeView);
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, createNewTable, groupName, annotationName, location, saveTo));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteItem"
void GTUtilsAnnotationsTreeView::deleteItem(HI::GUITestOpStatus& os, const QString& itemName) {
    deleteItem(os, findItem(os, itemName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteItem"
void GTUtilsAnnotationsTreeView::deleteItem(HI::GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK(item != nullptr, "Item is NULL");
    selectItems(os, QList<QTreeWidgetItem*>() << item);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTThread::waitForMainThread();
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnItem"
void GTUtilsAnnotationsTreeView::callContextMenuOnItem(HI::GUITestOpStatus& os, QTreeWidgetItem* item) {
    GTTreeWidget::scrollToItem(os, item);
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnItem"
void GTUtilsAnnotationsTreeView::callContextMenuOnItem(HI::GUITestOpStatus& os, const QString& itemName) {
    callContextMenuOnItem(os, findItem(os, itemName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenuOnQualifier"
void GTUtilsAnnotationsTreeView::callContextMenuOnQualifier(HI::GUITestOpStatus& os, const QString& annotationName, const QString& qualifierName) {
    QTreeWidgetItem* annotationItem = findItem(os, annotationName);
    GTTreeWidget::expand(os, annotationItem);
    QTreeWidgetItem* qualifierItem = findItem(os, qualifierName, annotationItem);
    callContextMenuOnItem(os, qualifierItem);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoAnnotations"
void GTUtilsAnnotationsTreeView::checkNoAnnotations(HI::GUITestOpStatus& os) {
    QTreeWidgetItem* annotationItem = findFirstAnnotation(os, {false});
    CHECK_SET_ERR(annotationItem == nullptr, "There should be no annotations");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
