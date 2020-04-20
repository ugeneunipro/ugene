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

#include <QDropEvent>
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QTreeView>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ProjectViewModel.h>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTThread.h>

#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsProjectTreeView"

const QString GTUtilsProjectTreeView::widgetName = "documentTreeWidget";

#define GT_METHOD_NAME "openView"
void GTUtilsProjectTreeView::openView(HI::GUITestOpStatus& os, GTGlobals::UseMethod method) {

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;

    QWidget *documentTreeWidget = GTWidget::findWidget(os, widgetName, NULL, options);
    if (!documentTreeWidget) {
        toggleView(os, method);
    }
    GTGlobals::sleep(100);
    GTThread::waitForMainThread();

    documentTreeWidget = GTWidget::findWidget(os, widgetName, NULL, options);
    GT_CHECK(documentTreeWidget != NULL, "Can't open document tree widget view, findWidget returned NULL");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "toggleView"
void GTUtilsProjectTreeView::toggleView(HI::GUITestOpStatus& os, GTGlobals::UseMethod method) {

    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != NULL, "MainWindow is NULL");
    QMainWindow *qmw = mw->getQMainWindow();
    GT_CHECK(qmw != NULL, "QMainWindow is NULL");

    //qmw->setFocus();

    switch (method) {
    case GTGlobals::UseKey:
    case GTGlobals::UseKeyBoard:
        GTKeyboardDriver::keyClick('1', Qt::AltModifier);
        break;
    case GTGlobals::UseMouse:
        GTWidget::click(os, GTWidget::findWidget(os, "doc_lable_project_view"));
        break;
    default:
        break;
    }

    GTGlobals::sleep(100);
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsProjectTreeView::getItemCenter(HI::GUITestOpStatus &os, const QModelIndex &itemIndex) {
    GT_CHECK_RESULT(itemIndex.isValid(), "Item index is invalid", QPoint());

    QTreeView *treeView = getTreeView(os);
    return getItemCenter(os, treeView, itemIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsProjectTreeView::getItemCenter(HI::GUITestOpStatus &os, QTreeView *treeView, const QModelIndex &itemIndex) {
    GT_CHECK_RESULT(itemIndex.isValid(), "Item index is invalid", QPoint());
    GT_CHECK_RESULT(NULL != treeView, "treeView is NULL", QPoint());

    QRect r = treeView->visualRect(itemIndex);

    return treeView->mapToGlobal(r.center());
}
#undef GT_METHOD_NAME

namespace {

void editItemName(HI::GUITestOpStatus &os, const QString &newItemName, GTGlobals::UseMethod invokeMethod) {
    switch (invokeMethod) {
    case GTGlobals::UseKey:
        GTMouseDriver::click();
        GTKeyboardDriver::keyClick(Qt::Key_F2);
        break;
    case GTGlobals::UseMouse:
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Rename", GTGlobals::UseMouse));
        GTMouseDriver::click(Qt::RightButton);
        GTGlobals::sleep(300);
        break;
    default:
        os.setError("An unsupported way of a rename procedure invocation");
        return;
    }

    GTKeyboardDriver::keySequence(newItemName);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    GTGlobals::sleep(500);
}

}

#define GT_METHOD_NAME "rename"
void GTUtilsProjectTreeView::rename(HI::GUITestOpStatus &os, const QString &itemName, const QString &newItemName, GTGlobals::UseMethod invokeMethod) {
    GTMouseDriver::moveTo(getItemCenter(os, itemName));
    editItemName(os, newItemName, invokeMethod);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "rename"
void GTUtilsProjectTreeView::rename(HI::GUITestOpStatus &os, const QModelIndex& itemIndex, const QString &newItemName, GTGlobals::UseMethod invokeMethod) {
    GTMouseDriver::moveTo(getItemCenter(os, itemIndex));
    editItemName(os, newItemName, invokeMethod);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsProjectTreeView::getItemCenter(HI::GUITestOpStatus &os, const QString &itemName) {
    return getItemCenter(os, findIndex(os, itemName));
}
#undef GT_METHOD_NAME



#define GT_METHOD_NAME "scrollTo"
void GTUtilsProjectTreeView::scrollTo(HI::GUITestOpStatus &os, const QString &itemName)
{
    QTreeView* treeView = getTreeView(os);
    GT_CHECK(treeView != NULL, "tree view not found");

    treeView->scrollTo(findIndex(os, itemName));

}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickItem"
void GTUtilsProjectTreeView::doubleClickItem(HI::GUITestOpStatus &os, const QModelIndex &itemIndex) {
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");
    getTreeView(os)->scrollTo(itemIndex);

    GTMouseDriver::moveTo(getItemCenter(os, itemIndex));
    GTMouseDriver::doubleClick();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickItem"
void GTUtilsProjectTreeView::doubleClickItem(HI::GUITestOpStatus &os, const QString &itemName) {
    doubleClickItem(os, findIndex(os, itemName));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTUtilsProjectTreeView::click(HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton button) {
    QModelIndex itemIndex = findIndex(os, itemName);
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");
    getTreeView(os)->scrollTo(itemIndex);

    QPoint p = getItemCenter(os, itemIndex); // clicking on the center does not select the item (Linux)
    p.setX(p.x() + 1);
    p.setY(p.y() + 5);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTUtilsProjectTreeView::click(HI::GUITestOpStatus &os, const QString& itemName, const QString &parentName, Qt::MouseButton button) {
    QModelIndex parentIndex = findIndex(os, parentName);
    GT_CHECK(parentIndex.isValid(), "Parent item index is invalid");
    QModelIndex itemIndex = findIndex(os, itemName, parentIndex);
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");
    getTreeView(os)->scrollTo(itemIndex);

    GTMouseDriver::moveTo(getItemCenter(os, itemIndex));
    GTMouseDriver::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenu"
void GTUtilsProjectTreeView::callContextMenu(GUITestOpStatus &os, const QString &itemName) {
    click(os, itemName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeWidget"
void GTUtilsProjectTreeView::callContextMenu(GUITestOpStatus &os, const QString &itemName, const QString &parentName) {
    click(os, itemName, parentName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeWidget"
QTreeView* GTUtilsProjectTreeView::getTreeView(HI::GUITestOpStatus &os) {

    openView(os);

    QTreeView *treeView = qobject_cast<QTreeView*>(GTWidget::findWidget(os, widgetName));
    return treeView;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QModelIndex GTUtilsProjectTreeView::findIndex(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options) {
    QTreeView *treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", QModelIndex());
    return findIndex(os, treeView, itemName, QModelIndex(), options);
}

#define GT_METHOD_NAME "findItem"
QModelIndex GTUtilsProjectTreeView::findIndex(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const GTGlobals::FindOptions &options) {
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", QModelIndex());
    return findIndex(os, treeView, itemName, QModelIndex(), options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QModelIndex GTUtilsProjectTreeView::findIndex(HI::GUITestOpStatus &os, const QString &itemName, const QModelIndex& parent, const GTGlobals::FindOptions &options) {
    QTreeView *treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", QModelIndex());
    return findIndex(os, treeView, itemName, parent, options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QModelIndex GTUtilsProjectTreeView::findIndex(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options) {
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", QModelIndex());
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", QModelIndex());

    QModelIndexList foundIndexes = findIndecies(os, treeView, itemName, parent, 0, options);
    if (foundIndexes.isEmpty()) {
        if(options.failIfNotFound){
            GT_CHECK_RESULT(foundIndexes.size() != 0, QString("Item with name %1 not found").arg(itemName), QModelIndex());
        } else {
            return QModelIndex();
        }
    }

    GT_CHECK_RESULT(foundIndexes.size() == 1, QString("there are %1 items with name %2").arg(foundIndexes.size()).arg(itemName), QModelIndex());

    treeView->scrollTo(foundIndexes.at(0));
    return foundIndexes.at(0);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findIndex"
QModelIndex GTUtilsProjectTreeView::findIndex(GUITestOpStatus &os, const QStringList &itemPath, const GTGlobals::FindOptions &options) {
    QModelIndex item;
    foreach (const QString &itemName, itemPath) {
        item = findIndex(os, itemName, item, options);
    }
    return item;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findIndecies"
QModelIndexList GTUtilsProjectTreeView::findIndecies(HI::GUITestOpStatus &os, const QString &itemName, const QModelIndex &parent, int parentDepth, const GTGlobals::FindOptions &options) {
    QTreeView *treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "Tree widget is NULL", QModelIndexList());

    return findIndecies(os, treeView, itemName, parent, parentDepth, options);
}
#undef GT_METHOD_NAME

namespace {
    bool compareStrings(const QString &pattern, const QString &data, Qt::MatchFlags matchPolicy) {
        if (matchPolicy.testFlag(Qt::MatchContains)) {
            return data.contains(pattern);
        } else if (matchPolicy.testFlag(Qt::MatchStartsWith)) {
            return data.startsWith(pattern) || pattern.startsWith(data);
        }
        return (data == pattern);
    }
}

#define GT_METHOD_NAME "findIndecies"
QModelIndexList GTUtilsProjectTreeView::findIndecies(HI::GUITestOpStatus &os,
                                                     QTreeView* treeView,
                                                    const QString &itemName,
                                                    const QModelIndex& parent,
                                                    int parentDepth,
                                                    const GTGlobals::FindOptions &options)
{
    QModelIndexList foundIndecies;
    CHECK(GTGlobals::FindOptions::INFINITE_DEPTH == options.depth || parentDepth < options.depth, foundIndecies);

    QAbstractItemModel *model = treeView->model();
    CHECK_SET_ERR_RESULT(NULL != model, "Model is NULL", foundIndecies);

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(treeView->model());

    const int rowCount = NULL == proxyModel ? model->rowCount(parent) : proxyModel->rowCount(parent);
    for (int i = 0; i < rowCount; i++) {
        const QModelIndex index = NULL == proxyModel ? model->index(i, 0, parent) : proxyModel->index(i, 0, parent);
        QString s = index.data(Qt::DisplayRole).toString();

        GObject* object = ProjectViewModel::toObject(NULL == proxyModel ? index : proxyModel->mapToSource(index));
        if (NULL != object) {
            const QString prefix = "[" + GObjectTypes::getTypeInfo(object->getGObjectType()).treeSign + "]";
            if (s.startsWith(prefix) || prefix == "[u]") {
                s = s.mid(prefix.length() + 1);
            }
        } else {
            const QString unload = "[unloaded] ";
            if (s.startsWith(unload)){
                s = s.mid(unload.length());
            }
            const QRegExp loading("^\\[loading \\d+\\%\\] ");
            if (-1 != loading.indexIn(s)) {
                s = s.mid(loading.matchedLength());
            }
        }

        if (!itemName.isEmpty()) {
            if (compareStrings(itemName, s, options.matchPolicy)) {
                foundIndecies << index;
            } else {
                foundIndecies << findIndecies(os, treeView, itemName, index, parentDepth + 1, options);
            }
        } else {
            foundIndecies << index;
            foundIndecies << findIndecies(os, treeView, itemName, index, parentDepth + 1, options);
        }
    }

    return foundIndecies;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "filterProject"
void GTUtilsProjectTreeView::filterProject(HI::GUITestOpStatus &os, const QString &searchField) {
    openView(os);
    QLineEdit *nameFilterEdit = GTWidget::findExactWidget<QLineEdit *>(os, "nameFilterEdit");
    GTLineEdit::setText(os, nameFilterEdit, searchField);
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "filterProjectSequental"
void GTUtilsProjectTreeView::filterProjectSequental(HI::GUITestOpStatus &os, const QStringList &searchField, bool waitUntilSearchEnd) {
    openView(os);
    QLineEdit *nameFilterEdit = GTWidget::findExactWidget<QLineEdit *>(os, "nameFilterEdit");
    foreach(const QString& str, searchField) {
        GTLineEdit::setText(os, nameFilterEdit, str);
        GTGlobals::sleep(3000);
    }
    if (waitUntilSearchEnd) {
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findFilteredIndexes"
QModelIndexList GTUtilsProjectTreeView::findFilteredIndexes(HI::GUITestOpStatus &os, const QString &substring, const QModelIndex &parentIndex) {
    QModelIndexList result;

    QTreeView *treeView = getTreeView(os);
    QAbstractItemModel *model = treeView->model();
    CHECK_SET_ERR_RESULT(NULL != model, "Model is invalid", result);

    const int rowcount = model->rowCount(parentIndex);
    for (int i = 0; i < rowcount; i++) {
        const QModelIndex index = model->index(i, 0, parentIndex);
        const QString itemName = index.data().toString();

        if (itemName.contains(substring)) {
            result << index;
        }
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkFilteredGroup"
void GTUtilsProjectTreeView::checkFilteredGroup(HI::GUITestOpStatus &os,
                                                const QString &groupName,
                                                const QStringList &namesToCheck,
                                                const QStringList &alternativeNamesToCheck,
                                                const QStringList &excludedNames,
                                                const QStringList& skipGroupIfContains) {
    const QModelIndexList groupIndexes = findFilteredIndexes(os, groupName);
    CHECK_SET_ERR(groupIndexes.size() == 1, QString("Expected to find a single filter group. Found %1").arg(groupIndexes.size()));

    const QModelIndex group = groupIndexes.first();
    const int filteredItemsCount = group.model()->rowCount(group);
    CHECK_SET_ERR(filteredItemsCount > 0, "No project items have been filtered");
    for (int i = 0; i < filteredItemsCount; ++i) {
        const QString childName = group.child(i, 0).data().toString();
        bool notSkipGroup = true;
        foreach(const QString& checkToSkip, skipGroupIfContains) {
            if (childName.contains(checkToSkip, Qt::CaseInsensitive)){
                notSkipGroup = false;
                break;
            }
        }
        CHECK_CONTINUE(notSkipGroup);

        foreach (const QString &nameToCheck, namesToCheck) {
            bool contains = childName.contains(nameToCheck, Qt::CaseInsensitive);
            CHECK_SET_ERR(contains, QString("Filtered item doesn't contain '%1'").arg(nameToCheck));
        }

        bool oneAlternativeFound = alternativeNamesToCheck.isEmpty();
        foreach (const QString &nameToCheck, alternativeNamesToCheck) {
            if (childName.contains(nameToCheck, Qt::CaseInsensitive)) {
                oneAlternativeFound = true;
                break;
            }
        }
        CHECK_SET_ERR(oneAlternativeFound, QString("Filtered item doesn't contain either of strings: '%1'").arg(alternativeNamesToCheck.join("', '")));

        foreach(const QString &nameToCheck, excludedNames) {
            bool doesNotContain = !childName.contains(nameToCheck, Qt::CaseInsensitive);
            CHECK_SET_ERR(doesNotContain, QString("Filtered item contains unexpectedly '%1'").arg(nameToCheck));
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "ensureFilteringIsDisabled"
void GTUtilsProjectTreeView::ensureFilteringIsDisabled(GUITestOpStatus &os) {
    openView(os);
    GTLineEdit::checkText(os, "nameFilterEdit", NULL, "");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
bool GTUtilsProjectTreeView::checkItem(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options) {
    QTreeView *treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", false);
    return checkItem(os, treeView, itemName, QModelIndex(), options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
bool GTUtilsProjectTreeView::checkItem(HI::GUITestOpStatus &os, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options) {
    QTreeView *treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", false);
    return checkItem(os, treeView, itemName, parent, options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
bool GTUtilsProjectTreeView::checkItem(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const GTGlobals::FindOptions &options) {
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", false);
    return checkItem(os, treeView, itemName, QModelIndex(), options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
bool GTUtilsProjectTreeView::checkItem(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options) {
    GT_CHECK_RESULT(treeView != NULL, "Tree view is NULL", false);
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", false);

    QModelIndexList foundIndexes = findIndecies(os, treeView, itemName, parent, 0, options);
    return !foundIndexes.isEmpty();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkObjectTypes"
void GTUtilsProjectTreeView::checkObjectTypes(HI::GUITestOpStatus &os, const QSet<GObjectType> &acceptableTypes, const QModelIndex &parent) {
    checkObjectTypes(os, getTreeView(os),  acceptableTypes, parent);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkObjectTypes"
void GTUtilsProjectTreeView::checkObjectTypes(HI::GUITestOpStatus &os, QTreeView *treeView, const QSet<GObjectType> &acceptableTypes, const QModelIndex &parent) {
    CHECK_SET_ERR(NULL != treeView, "Invalid tree view detected");
    CHECK(!acceptableTypes.isEmpty(), );

    QAbstractItemModel *model = treeView->model();
    CHECK_SET_ERR(NULL != model, "Invalid view model detected");

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model);

    const int rowCount = NULL == proxyModel ? model->rowCount(parent) : proxyModel->rowCount(parent);
    for (int i = 0; i < rowCount; i++) {
        const QModelIndex index = NULL == proxyModel ? model->index(i, 0, parent) : proxyModel->mapToSource(proxyModel->index(i, 0, parent));
        GObject *object = ProjectViewModel::toObject(index);
        if (NULL != object && Qt::NoItemFlags != model->flags(index) && !acceptableTypes.contains(object->getGObjectType()))
            CHECK_SET_ERR(NULL == object || Qt::NoItemFlags == model->flags(index) || acceptableTypes.contains(object->getGObjectType()), "Object has unexpected type");

        if (NULL == object) {
            checkObjectTypes(os, treeView, acceptableTypes, NULL == proxyModel ? index : proxyModel->mapFromSource(index));
            CHECK_OP_BREAK(os);
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedItem"
QString GTUtilsProjectTreeView::getSelectedItem(HI::GUITestOpStatus &os)
{
    QTreeView* treeView = getTreeView(os);
    GT_CHECK_RESULT(treeView != NULL, "tree view is NULL", NULL);

    QModelIndexList list = treeView->selectionModel()->selectedIndexes();
    GT_CHECK_RESULT(list.size() != 0, QString("there are no selected items"), "");
    GT_CHECK_RESULT(list.size() == 1, QString("there are %1 selected items").arg(list.size()), "");

    QModelIndex index = list.at(0);
    QString result = index.data(Qt::DisplayRole).toString();

    return result;

}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFont"
QFont GTUtilsProjectTreeView::getFont(HI::GUITestOpStatus &os, QModelIndex index){
    QTreeView* treeView = getTreeView(os);
    QAbstractItemModel* model = treeView->model();
    QFont result = qvariant_cast<QFont>(model->data(index, Qt::FontRole));
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getIcon"
QIcon GTUtilsProjectTreeView::getIcon(HI::GUITestOpStatus &os, QModelIndex index){
    QTreeView* treeView = getTreeView(os);
    QAbstractItemModel* model = treeView->model();
    QIcon result = qvariant_cast<QIcon>(model->data(index, Qt::DecorationRole));
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "itemModificationCheck"
void GTUtilsProjectTreeView::itemModificationCheck(HI::GUITestOpStatus &os, const QString &itemName, bool modified) {
    itemModificationCheck(os, findIndex(os, itemName), modified);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "itemModificationCheck"
void GTUtilsProjectTreeView::itemModificationCheck(HI::GUITestOpStatus &os, QModelIndex index, bool modified ){
    GT_CHECK(index.isValid(), "item is valid");
    QVariant data = index.data(Qt::TextColorRole);
    bool modState = !(QVariant() == data);
    GT_CHECK(modState == modified, "Document's " + index.data(Qt::DisplayRole).toString() + " modification state not equal with expected");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "itemActiveCheck"
void GTUtilsProjectTreeView::itemActiveCheck(HI::GUITestOpStatus &os, QModelIndex index, bool active) {
    GT_CHECK(index.isValid(), "item is NULL");
    QVariant data = index.data(Qt::FontRole);

    bool modState = !(QVariant() == data);
    GT_CHECK(modState == active, "Document's " + index.data(Qt::FontRole).toString() + " active state not equal with expected");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isVisible"
bool GTUtilsProjectTreeView::isVisible( HI::GUITestOpStatus &os ){
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *documentTreeWidget = GTWidget::findWidget(os, widgetName, NULL, options);
    if (documentTreeWidget) {
        return true;
    }else{
        return false;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDrop"
void GTUtilsProjectTreeView::dragAndDrop(HI::GUITestOpStatus &os, const QModelIndex &from, const QModelIndex &to) {
    sendDragAndDrop(os, getItemCenter(os, from), getItemCenter(os, to));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDrop"
void GTUtilsProjectTreeView::dragAndDrop(HI::GUITestOpStatus &os, const QModelIndex &from, QWidget *to) {
    sendDragAndDrop(os, getItemCenter(os, from), to);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "dragAndDrop"
void GTUtilsProjectTreeView::dragAndDrop(GUITestOpStatus &os, const QStringList &from, QWidget *to) {
    dragAndDrop(os, findIndex(os, from), to);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isVisible"
void GTUtilsProjectTreeView::dragAndDropSeveralElements(HI::GUITestOpStatus &os, QModelIndexList from, QModelIndex to) {
    QTreeView *treeView = getTreeView(os);

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (QModelIndex index, from){
        treeView->scrollTo(index);
        GTMouseDriver::moveTo(getItemCenter(os, index));
        GTMouseDriver::click();
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    QPoint enterPos = getItemCenter(os, from.at(0));
    QPoint dropPos = getItemCenter(os, to);

    sendDragAndDrop(os, enterPos, dropPos);
}
#undef GT_METHOD_NAME

void GTUtilsProjectTreeView::sendDragAndDrop(HI::GUITestOpStatus & /*os*/, const QPoint &enterPos, const QPoint &dropPos) {
    GTMouseDriver::dragAndDrop(enterPos, dropPos);
    GTGlobals::sleep(1000);
}

void GTUtilsProjectTreeView::sendDragAndDrop(HI::GUITestOpStatus &os, const QPoint &enterPos, QWidget *dropWidget) {
    sendDragAndDrop(os, enterPos, GTWidget::getWidgetCenter(os, dropWidget));
}

void GTUtilsProjectTreeView::expandProjectView(HI::GUITestOpStatus &os){
    QSplitter* splitter = GTWidget::findExactWidget<QSplitter*>(os, "splitter", GTWidget::findWidget(os, "project_view"));
    splitter->setSizes(QList<int>()<<splitter->height()<<0);
}

#define GT_METHOD_NAME "markSequenceAsCircular"
void GTUtilsProjectTreeView::markSequenceAsCircular(HI::GUITestOpStatus &os, const QString &sequenceObjectName) {
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Mark as circular"));
    click(os, sequenceObjectName, Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDocuments"
QMap<QString, QStringList> GTUtilsProjectTreeView::getDocuments(GUITestOpStatus &os) {
    ensureFilteringIsDisabled(os);
    GTGlobals::FindOptions options(false, Qt::MatchContains, 1);
    const QModelIndexList documentsItems = findIndecies(os, "", QModelIndex(), 0, options);

    QMap<QString, QStringList> documents;
    foreach (const QModelIndex &documentItem, documentsItems) {
        const QModelIndexList objectsItems = findIndecies(os, "", documentItem, 0, options);
        QStringList objects;
        foreach (const QModelIndex &objectItem, objectsItems) {
            objects << objectItem.data().toString();
        }
        documents.insert(documentItem.data().toString(), objects);
    }

    return documents;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
