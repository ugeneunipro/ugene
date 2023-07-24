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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTreeView.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTThread.h>

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

#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsProjectTreeView"

const QString GTUtilsProjectTreeView::widgetName = "documentTreeWidget";

int GTUtilsProjectTreeView::countTopLevelItems() {
    QTreeView* treeView = getTreeView();
    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR_RESULT(model != nullptr, "Model is NULL", {});

    return model->rowCount();
}

void GTUtilsProjectTreeView::checkProjectViewIsOpened() {
    GTWidget::findWidget(widgetName);
    GTThread::waitForMainThread();
}

void GTUtilsProjectTreeView::checkProjectViewIsClosed() {
    QWidget* documentTreeWidget = GTWidget::findWidget(widgetName, nullptr, {false});
    for (int time = 0; time < GT_OP_WAIT_MILLIS && documentTreeWidget != nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        documentTreeWidget = GTWidget::findWidget(widgetName, nullptr, {false});
    }
    GT_CHECK_RESULT(documentTreeWidget == nullptr, "Project view is opened, but is expected to be closed", )
}

void GTUtilsProjectTreeView::openView(GTGlobals::UseMethod method) {
    // Wait up to 3 seconds for the project view to be available.
    QWidget* documentTreeWidget = nullptr;
    for (int time = 0; time < 3000 && documentTreeWidget == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        documentTreeWidget = GTWidget::findWidget(widgetName, nullptr, {false});
    }
    if (documentTreeWidget == nullptr) {
        toggleView(method);
    }
    checkProjectViewIsOpened();
}

void GTUtilsProjectTreeView::toggleView(GTGlobals::UseMethod method) {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow is NULL");
    QMainWindow* qmw = mw->getQMainWindow();
    GT_CHECK(qmw != nullptr, "QMainWindow is NULL");

    // qmw->setFocus();

    switch (method) {
        case GTGlobals::UseKey:
        case GTGlobals::UseKeyBoard:
            GTKeyboardDriver::keyClick('1', Qt::AltModifier);
            break;
        case GTGlobals::UseMouse:
            GTWidget::click(GTWidget::findWidget("doc_label__project_view"));
            break;
        default:
            break;
    }

    GTGlobals::sleep(100);
    GTThread::waitForMainThread();
}

QPoint GTUtilsProjectTreeView::getItemCenter(const QModelIndex& itemIndex) {
    GT_CHECK_RESULT(itemIndex.isValid(), "Item index is invalid", QPoint());

    QTreeView* treeView = getTreeView();
    return getItemCenter(treeView, itemIndex);
}

QPoint GTUtilsProjectTreeView::getItemCenter(QTreeView* treeView, const QModelIndex& itemIndex) {
    GT_CHECK_RESULT(itemIndex.isValid(), "Item index is invalid", QPoint());
    GT_CHECK_RESULT(treeView != nullptr, "treeView is NULL", QPoint());

    QRect r = treeView->visualRect(itemIndex);

    return treeView->mapToGlobal(r.center());
}

namespace {

void editItemName(const QString& newItemName, GTGlobals::UseMethod invokeMethod) {
    switch (invokeMethod) {
        case GTGlobals::UseKey:
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_F2);
            break;
        case GTGlobals::UseMouse:
            GTUtilsDialog::waitForDialog(new PopupChooser({"Rename"}, GTGlobals::UseMouse));
            GTMouseDriver::click(Qt::RightButton);
            GTGlobals::sleep(300);
            break;
        default:
            GT_FAIL("An unsupported way of a rename procedure invocation", );
    }

    GTKeyboardDriver::keySequence(newItemName);
    GTGlobals::sleep(500);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);

    GTGlobals::sleep(500);
}

}  // namespace

void GTUtilsProjectTreeView::rename(const QString& itemName, const QString& newItemName, GTGlobals::UseMethod invokeMethod) {
    GTMouseDriver::moveTo(getItemCenter(itemName));
    editItemName(newItemName, invokeMethod);
}

void GTUtilsProjectTreeView::rename(const QModelIndex& itemIndex, const QString& newItemName, GTGlobals::UseMethod invokeMethod) {
    GTMouseDriver::moveTo(getItemCenter(itemIndex));
    editItemName(newItemName, invokeMethod);
}

QPoint GTUtilsProjectTreeView::getItemCenter(const QString& itemName) {
    return getItemCenter(findIndex(itemName));
}

void GTUtilsProjectTreeView::scrollTo(const QString& itemName) {
    QModelIndex index = findIndex(itemName);
    scrollToIndexAndMakeExpanded(getTreeView(), index);
}

void GTUtilsProjectTreeView::scrollToIndexAndMakeExpanded(QTreeView* treeView, const QModelIndex& index) {
    GTTreeView::scrollToItem(treeView, index);
    GTTreeView::expand(treeView, index);
}

void GTUtilsProjectTreeView::doubleClickItem(const QModelIndex& itemIndex) {
    GTTreeView::doubleClick(getTreeView(), itemIndex);
}

void GTUtilsProjectTreeView::doubleClickItem(const QString& itemName) {
    doubleClickItem(findIndex(itemName));
}

void GTUtilsProjectTreeView::click(const QString& itemName, Qt::MouseButton button) {
    QTreeView* treeView = getTreeView();
    QModelIndex itemIndex = findIndex(treeView, itemName);
    QPoint p = getItemCenter(treeView, itemIndex);  // clicking in the center does not select the item (Linux).
    p.setX(p.x() + 1);
    p.setY(p.y() + 5);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click(button);
}

void GTUtilsProjectTreeView::click(const QString& itemName, const QString& parentName, Qt::MouseButton button, const GTGlobals::FindOptions& itemOptions) {
    QModelIndex parentIndex = findIndex(parentName);
    QModelIndex itemIndex = findIndex(itemName, parentIndex, itemOptions);
    scrollToIndexAndMakeExpanded(getTreeView(), itemIndex);

    GTMouseDriver::moveTo(getItemCenter(itemIndex));
    GTMouseDriver::click(button);
}

void GTUtilsProjectTreeView::callContextMenu(const QString& itemName) {
    click(itemName, Qt::RightButton);
}

void GTUtilsProjectTreeView::callContextMenu(const QString& itemName, const QString& parentName) {
    click(itemName, parentName, Qt::RightButton);
}

void GTUtilsProjectTreeView::callContextMenu(const QModelIndex& itemIndex) {
    GT_CHECK(itemIndex.isValid(), "Item index is not valid!");
    QTreeView* treeView = getTreeView();
    GT_CHECK(treeView != nullptr, "Tree widget is NULL");
    scrollToIndexAndMakeExpanded(treeView, itemIndex);
    GTMouseDriver::moveTo(getItemCenter(itemIndex));
    GTMouseDriver::click(Qt::RightButton);
}

QTreeView* GTUtilsProjectTreeView::getTreeView() {
    openView();
    return GTWidget::findTreeView(widgetName);
}

QModelIndex GTUtilsProjectTreeView::findIndex(const QString& itemName, const GTGlobals::FindOptions& options) {
    QTreeView* treeView = getTreeView();
    GT_CHECK_RESULT(treeView != nullptr, "Tree view is NULL", QModelIndex());
    return findIndex(treeView, itemName, QModelIndex(), options);
}

QModelIndex GTUtilsProjectTreeView::findIndex(QTreeView* treeView, const QString& itemName, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(treeView != nullptr, "Tree view is NULL", QModelIndex());
    return findIndex(treeView, itemName, QModelIndex(), options);
}

QModelIndex GTUtilsProjectTreeView::findIndex(const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options) {
    QTreeView* treeView = getTreeView();
    GT_CHECK_RESULT(treeView != nullptr, "Tree view is NULL", QModelIndex());
    return findIndex(treeView, itemName, parent, options);
}

QModelIndex GTUtilsProjectTreeView::findIndex(
    QTreeView* treeView,
    const QString& itemName,
    const QModelIndex& parent,
    const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(treeView != nullptr, "Tree view is NULL", QModelIndex());
    GT_CHECK_RESULT(!itemName.isEmpty(), "Item name is empty", QModelIndex());

    QModelIndexList foundIndexes;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && foundIndexes.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        foundIndexes = findIndiciesInTreeNoWait(treeView, itemName, parent, 0, options);
        if (!options.failIfNotFound) {
            break;
        }
    }
    if (foundIndexes.isEmpty()) {
        GT_CHECK_RESULT(!options.failIfNotFound, QString("Item with name %1 not found").arg(itemName), QModelIndex());
        return {};
    }
    GT_CHECK_RESULT(foundIndexes.size() == 1, QString("there are %1 items with name %2").arg(foundIndexes.size()).arg(itemName), QModelIndex());

    QModelIndex index = foundIndexes.at(0);
    scrollToIndexAndMakeExpanded(treeView, index);
    return index;
}

QModelIndex GTUtilsProjectTreeView::findIndex(const QStringList& itemPath, const GTGlobals::FindOptions& options) {
    QModelIndex item;
    foreach (const QString& itemName, itemPath) {
        GTGlobals::FindOptions itemOptions = options;
        itemOptions.depth = 1;
        item = findIndex(itemName, item, itemOptions);
    }
    return item;
}

QModelIndexList GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait(const QString& itemName, const QModelIndex& parent, int parentDepth, const GTGlobals::FindOptions& options) {
    QTreeView* treeView = getTreeView();
    GT_CHECK_RESULT(treeView != nullptr, "Tree widget is NULL", QModelIndexList());

    return findIndiciesInTreeNoWait(treeView, itemName, parent, parentDepth, options);
}

namespace {
bool compareStrings(const QString& pattern, const QString& data, Qt::MatchFlags matchPolicy) {
    if (matchPolicy.testFlag(Qt::MatchContains)) {
        return data.contains(pattern);
    } else if (matchPolicy.testFlag(Qt::MatchStartsWith)) {
        return data.startsWith(pattern) || pattern.startsWith(data);
    }
    return (data == pattern);
}
}  // namespace

QModelIndexList GTUtilsProjectTreeView::findIndiciesInTreeNoWait(
    QTreeView* treeView,
    const QString& itemName,
    const QModelIndex& parent,
    int parentDepth,
    const GTGlobals::FindOptions& options) {
    CHECK(options.depth == GTGlobals::FindOptions::INFINITE_DEPTH || parentDepth < options.depth, {});

    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR_RESULT(model != nullptr, "Model is NULL", {});

    auto proxyModel = qobject_cast<QSortFilterProxyModel*>(treeView->model());
    QModelIndexList foundIndicies;
    int rowCount = proxyModel == nullptr ? model->rowCount(parent) : proxyModel->rowCount(parent);
    for (int i = 0; i < rowCount; i++) {
        QModelIndex index = proxyModel == nullptr ? model->index(i, 0, parent) : proxyModel->index(i, 0, parent);
        QString s = index.data(Qt::DisplayRole).toString();

        GObject* object = ProjectViewModel::toObject(proxyModel == nullptr ? index : proxyModel->mapToSource(index));
        if (object != nullptr) {
            QString prefix = "[" + GObjectTypes::getTypeInfo(object->getGObjectType()).treeSign + "]";
            if (s.startsWith(prefix) || prefix == "[u]") {
                s = s.mid(prefix.length() + 1);
            }
        } else {
            QString unload = "[unloaded] ";
            if (s.startsWith(unload)) {
                s = s.mid(unload.length());
            }
            QRegExp loading("^\\[loading \\d+\\%\\] ");
            if (-1 != loading.indexIn(s)) {
                s = s.mid(loading.matchedLength());
            }
        }

        if (!itemName.isEmpty()) {
            if (compareStrings(itemName, s, options.matchPolicy)) {
                foundIndicies << index;
            } else {
                foundIndicies << findIndiciesInTreeNoWait(treeView, itemName, index, parentDepth + 1, options);
            }
        } else {
            foundIndicies << index;
            foundIndicies << findIndiciesInTreeNoWait(treeView, itemName, index, parentDepth + 1, options);
        }
    }

    return foundIndicies;
}

void GTUtilsProjectTreeView::filterProject(const QString& searchField) {
    openView();
    if (isOsMac()) {
        GTGlobals::sleep(3000);
    }
    GTLineEdit::setText("nameFilterEdit", searchField);
    GTGlobals::sleep(3000);
    GTUtilsTaskTreeView::waitTaskFinished();
}

void GTUtilsProjectTreeView::filterProjectSequental(const QStringList& searchField, bool waitUntilSearchEnd) {
    openView();
    foreach (const QString& str, searchField) {
        GTLineEdit::setText("nameFilterEdit", str);
        GTGlobals::sleep(3000);
    }
    if (waitUntilSearchEnd) {
        GTUtilsTaskTreeView::waitTaskFinished();
    }
}

QModelIndexList GTUtilsProjectTreeView::findFilteredIndexes(const QString& substring, const QModelIndex& parentIndex) {
    QModelIndexList result;

    QTreeView* treeView = getTreeView();
    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR_RESULT(nullptr != model, "Model is invalid", result);

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

void GTUtilsProjectTreeView::checkFilteredGroup(
    const QString& groupName,
    const QStringList& namesToCheck,
    const QStringList& alternativeNamesToCheck,
    const QStringList& excludedNames,
    const QStringList& skipGroupIfContains) {
    const QModelIndexList groupIndexes = findFilteredIndexes(groupName);
    CHECK_SET_ERR(groupIndexes.size() == 1, QString("Expected to find a single filter group. Found %1").arg(groupIndexes.size()));

    const QModelIndex group = groupIndexes.first();
    const int filteredItemsCount = group.model()->rowCount(group);
    CHECK_SET_ERR(filteredItemsCount > 0, "No project items have been filtered");
    for (int i = 0; i < filteredItemsCount; ++i) {
        QString childName = group.model()->index(i, 0, group).data().toString();
        bool notSkipGroup = true;
        foreach (const QString& checkToSkip, skipGroupIfContains) {
            if (childName.contains(checkToSkip, Qt::CaseInsensitive)) {
                notSkipGroup = false;
                break;
            }
        }
        CHECK_CONTINUE(notSkipGroup);

        foreach (const QString& nameToCheck, namesToCheck) {
            bool contains = childName.contains(nameToCheck, Qt::CaseInsensitive);
            CHECK_SET_ERR(contains, QString("Filtered item doesn't contain '%1'").arg(nameToCheck));
        }

        bool oneAlternativeFound = alternativeNamesToCheck.isEmpty();
        foreach (const QString& nameToCheck, alternativeNamesToCheck) {
            if (childName.contains(nameToCheck, Qt::CaseInsensitive)) {
                oneAlternativeFound = true;
                break;
            }
        }
        CHECK_SET_ERR(oneAlternativeFound, QString("Filtered item doesn't contain either of strings: '%1'").arg(alternativeNamesToCheck.join("', '")));

        foreach (const QString& nameToCheck, excludedNames) {
            bool doesNotContain = !childName.contains(nameToCheck, Qt::CaseInsensitive);
            CHECK_SET_ERR(doesNotContain, QString("Filtered item contains unexpectedly '%1'").arg(nameToCheck));
        }
    }
}

void GTUtilsProjectTreeView::checkFilteredResultIsEmpty() {
    QTreeView* treeView = getTreeView();
    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR(model->rowCount() == 0, "Filtered item not empty");
}

#define GT_METHOD_NAME "ensureFilteringIsDisabled"
void GTUtilsProjectTreeView::ensureFilteringIsDisabled() {
    openView();
    GTLineEdit::checkText("nameFilterEdit", nullptr, "");
}

bool GTUtilsProjectTreeView::checkItem(const QString& itemName, const GTGlobals::FindOptions& options) {
    QTreeView* treeView = getTreeView();
    return checkItem(treeView, itemName, QModelIndex(), options);
}

void GTUtilsProjectTreeView::checkNoItem(const QString& itemName) {
    bool isFound = checkItem(itemName, {false});
    GT_CHECK(!isFound, "Unexpected item was found in the project: " + itemName);
}

bool GTUtilsProjectTreeView::checkItem(const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options) {
    return checkItem(getTreeView(), itemName, parent, options);
}

bool GTUtilsProjectTreeView::checkItem(QTreeView* treeView, const QString& itemName, const GTGlobals::FindOptions& options) {
    return checkItem(treeView, itemName, QModelIndex(), options);
}

bool GTUtilsProjectTreeView::checkItem(QTreeView* treeView, const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(treeView != nullptr, "Tree view is NULL", false);
    GT_CHECK_RESULT(!itemName.isEmpty(), "Item name is empty", false);
    QModelIndexList indexList;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && indexList.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        indexList = findIndiciesInTreeNoWait(treeView, itemName, parent, 0, options);
        if (!options.failIfNotFound) {
            break;
        }
    }
    CHECK_SET_ERR_RESULT(!indexList.isEmpty() || !options.failIfNotFound, "Project tree item not found: " + itemName, false);
    return !indexList.isEmpty();
}

void GTUtilsProjectTreeView::checkObjectTypes(const QSet<GObjectType>& acceptableTypes, const QModelIndex& parent) {
    checkObjectTypes(getTreeView(), acceptableTypes, parent);
}

void GTUtilsProjectTreeView::checkObjectTypes(QTreeView* treeView, const QSet<GObjectType>& acceptableTypes, const QModelIndex& parent) {
    CHECK_SET_ERR(treeView != nullptr, "Tree view is null");
    CHECK_SET_ERR(!acceptableTypes.isEmpty(), "List of acceptable types is empty");

    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR(model != nullptr, "Tree model is null");

    auto proxyModel = qobject_cast<QSortFilterProxyModel*>(model);
    int rowCount = proxyModel == nullptr ? model->rowCount(parent) : proxyModel->rowCount(parent);
    for (int i = 0; i < rowCount; i++) {
        QModelIndex index = proxyModel == nullptr ? model->index(i, 0, parent) : proxyModel->mapToSource(proxyModel->index(i, 0, parent));
        GObject* object = ProjectViewModel::toObject(index);
        if (object != nullptr && Qt::NoItemFlags != model->flags(index) && !acceptableTypes.contains(object->getGObjectType()))
            CHECK_SET_ERR(object == nullptr || Qt::NoItemFlags == model->flags(index) || acceptableTypes.contains(object->getGObjectType()), "Object has unexpected type");

        if (object == nullptr) {
            checkObjectTypes(treeView, acceptableTypes, proxyModel == nullptr ? index : proxyModel->mapFromSource(index));
        }
    }
}

QString GTUtilsProjectTreeView::getSelectedItem() {
    QTreeView* treeView = getTreeView();
    GT_CHECK_RESULT(treeView != nullptr, "tree view is NULL", nullptr);

    QModelIndexList list = treeView->selectionModel()->selectedIndexes();
    GT_CHECK_RESULT(list.size() != 0, QString("there are no selected items"), "");
    GT_CHECK_RESULT(list.size() == 1, QString("there are %1 selected items").arg(list.size()), "");

    QModelIndex index = list.at(0);
    QString result = index.data(Qt::DisplayRole).toString();

    return result;
}

QFont GTUtilsProjectTreeView::getFont(QModelIndex index) {
    QTreeView* treeView = getTreeView();
    QAbstractItemModel* model = treeView->model();
    QFont result = qvariant_cast<QFont>(model->data(index, Qt::FontRole));
    return result;
}

QIcon GTUtilsProjectTreeView::getIcon(QModelIndex index) {
    QTreeView* treeView = getTreeView();
    QAbstractItemModel* model = treeView->model();
    QIcon result = qvariant_cast<QIcon>(model->data(index, Qt::DecorationRole));
    return result;
}

void GTUtilsProjectTreeView::itemModificationCheck(const QString& itemName, bool modified) {
    itemModificationCheck(findIndex(itemName), modified);
}

void GTUtilsProjectTreeView::itemModificationCheck(QModelIndex index, bool modified) {
    GT_CHECK(index.isValid(), "item is valid");
    QVariant data = index.data(Qt::TextColorRole);
    bool modState = !(QVariant() == data);
    GT_CHECK(modState == modified, "Document's " + index.data(Qt::DisplayRole).toString() + " modification state not equal with expected");
}

void GTUtilsProjectTreeView::itemActiveCheck(QModelIndex index, bool active) {
    GT_CHECK(index.isValid(), "item is NULL");
    QVariant data = index.data(Qt::FontRole);

    bool modState = !(QVariant() == data);
    GT_CHECK(modState == active, "Document's " + index.data(Qt::FontRole).toString() + " active state not equal with expected");
}

bool GTUtilsProjectTreeView::isVisible() {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    auto documentTreeWidget = GTWidget::findWidget(widgetName, nullptr, options);
    if (documentTreeWidget) {
        return true;
    } else {
        return false;
    }
}

void GTUtilsProjectTreeView::dragAndDrop(const QModelIndex& from, const QModelIndex& to) {
    sendDragAndDrop(getItemCenter(from), getItemCenter(to));
}

void GTUtilsProjectTreeView::dragAndDrop(const QModelIndex& from, QWidget* to) {
    sendDragAndDrop(getItemCenter(from), to);
}

void GTUtilsProjectTreeView::dragAndDrop(const QStringList& from, QWidget* to) {
    dragAndDrop(findIndex(from), to);
}

void GTUtilsProjectTreeView::dragAndDropSeveralElements(QModelIndexList from, QModelIndex to) {
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (QModelIndex index, from) {
        scrollToIndexAndMakeExpanded(getTreeView(), index);
        GTMouseDriver::moveTo(getItemCenter(index));
        GTMouseDriver::click();
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    QPoint enterPos = getItemCenter(from.at(0));
    QPoint dropPos = getItemCenter(to);

    sendDragAndDrop(enterPos, dropPos);
}

void GTUtilsProjectTreeView::sendDragAndDrop(const QPoint& enterPos, const QPoint& dropPos) {
    GTMouseDriver::dragAndDrop(enterPos, dropPos);
    GTGlobals::sleep(1000);
}

void GTUtilsProjectTreeView::sendDragAndDrop(const QPoint& enterPos, QWidget* dropWidget) {
    sendDragAndDrop(enterPos, GTWidget::getWidgetVisibleCenterGlobal(dropWidget));
}

void GTUtilsProjectTreeView::expandProjectView() {
    class SetSizesScenario : public CustomScenario {
    public:
        SetSizesScenario(QSplitter* _splitter)
            : splitter(_splitter) {
        }
        void run() override {
            splitter->setSizes(QList<int>() << splitter->height() << 0);
        }
        QSplitter* splitter = nullptr;
    };
    auto splitter = GTWidget::findSplitter("splitter", GTWidget::findWidget("project_view"));
    GTThread::runInMainThread(new SetSizesScenario(splitter));
}

void GTUtilsProjectTreeView::markSequenceAsCircular(const QString& sequenceObjectName) {
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Mark as circular"}));
    click(sequenceObjectName, Qt::RightButton);
}

QMap<QString, QStringList> GTUtilsProjectTreeView::getDocuments() {
    ensureFilteringIsDisabled();
    GTGlobals::FindOptions options(false, Qt::MatchContains, 1);
    QModelIndexList documentsItems = findIndeciesInProjectViewNoWait("", QModelIndex(), 0, options);

    QMap<QString, QStringList> documents;
    for (const QModelIndex& documentItem : qAsConst(documentsItems)) {
        const QModelIndexList objectsItems = findIndeciesInProjectViewNoWait("", documentItem, 0, options);
        QStringList objects;
        foreach (const QModelIndex& objectItem, objectsItems) {
            objects << objectItem.data().toString();
        }
        documents.insert(documentItem.data().toString(), objects);
    }

    return documents;
}

#undef GT_CLASS_NAME

}  // namespace U2
