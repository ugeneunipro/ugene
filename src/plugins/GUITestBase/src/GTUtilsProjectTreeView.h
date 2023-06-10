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

#pragma once

#include <GTGlobals.h>

#include <QAbstractItemModel>

#include <U2Gui/ProjectTreeController.h>

class QTreeView;
class QTreeWidget;
class QTreeWidgetItem;

namespace U2 {
using namespace HI;

class GTUtilsProjectTreeView {
public:
    // clicks on item by mouse, renames item by keyboard
    static void rename(const QString& itemName, const QString& newItemName, GTGlobals::UseMethod invokeMethod = GTGlobals::UseKey);
    static void rename(const QModelIndex& itemIndex, const QString& newItemName, GTGlobals::UseMethod invokeMethod = GTGlobals::UseKey);

    /** Checks that project view is opened and fails if not. */
    static void checkProjectViewIsOpened();

    /** Checks that project view is closed and fails if not. */
    static void checkProjectViewIsClosed();

    static void openView(GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static void toggleView(GTGlobals::UseMethod method = GTGlobals::UseMouse);

    // returns center or item's rect
    // fails if the item wasn't found
    static QPoint getItemCenter(const QModelIndex& itemIndex);
    static QPoint getItemCenter(QTreeView* treeView, const QModelIndex& itemIndex);
    static QPoint getItemCenter(const QString& itemName);

    /** Locates item in the tree by name and scrolls to the item to make it visible. */
    static void scrollTo(const QString& itemName);

    /** Scrolls to the item to make it visible. */
    static void scrollToIndexAndMakeExpanded(QTreeView* treeView, const QModelIndex& index);

    static void doubleClickItem(const QModelIndex& itemIndex);
    static void doubleClickItem(const QString& itemName);
    static void click(const QString& itemName, Qt::MouseButton button = Qt::LeftButton);
    static void click(const QString& itemName, const QString& parentName, Qt::MouseButton button = Qt::LeftButton, const GTGlobals::FindOptions& itemOptions = {});

    static void callContextMenu(const QString& itemName);
    static void callContextMenu(const QString& itemName, const QString& parentName);
    static void callContextMenu(const QModelIndex& itemIndex);

    static QTreeView* getTreeView();
    static QModelIndex findIndex(const QString& itemName, const GTGlobals::FindOptions& options = {});
    static QModelIndex findIndex(QTreeView* treeView, const QString& itemName, const GTGlobals::FindOptions& options = {});
    static QModelIndex findIndex(const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options = {});
    static QModelIndex findIndex(QTreeView* treeView, const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options = {});
    static QModelIndex findIndex(const QStringList& itemPath, const GTGlobals::FindOptions& options = {});
    static QModelIndexList findIndeciesInProjectViewNoWait(
        const QString& itemName,
        const QModelIndex& parent = QModelIndex(),
        int parentDepth = 0,
        const GTGlobals::FindOptions& options = {});
    static QModelIndexList findIndiciesInTreeNoWait(
        QTreeView* treeView,
        const QString& itemName,
        const QModelIndex& parent = QModelIndex(),
        int parentDepth = 0,
        const GTGlobals::FindOptions& options = {});

    static void filterProject(const QString& searchField);
    static void filterProjectSequental(const QStringList& searchField, bool waitUntilSearchEnd);
    static QModelIndexList findFilteredIndexes(const QString& substring, const QModelIndex& parentIndex = QModelIndex());
    static void checkFilteredGroup(const QString& groupName, const QStringList& namesToCheck, const QStringList& alternativeNamesToCheck, const QStringList& excludedNames, const QStringList& skipGroupIfContains = QStringList());
    static void ensureFilteringIsDisabled();

    // returns true if the item exists, does not set error unlike findIndex method
    static bool checkItem(const QString& itemName, const GTGlobals::FindOptions& options = {});
    static bool checkItem(QTreeView* treeView, const QString& itemName, const GTGlobals::FindOptions& options = {});
    static bool checkItem(const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options = {});
    static bool checkItem(QTreeView* treeView, const QString& itemName, const QModelIndex& parent, const GTGlobals::FindOptions& options = {});

    static void checkNoItem(const QString& itemName);

    // the method does nothing if `acceptableTypes` is an empty set
    static void checkObjectTypes(const QSet<GObjectType>& acceptableTypes, const QModelIndex& parent = QModelIndex());
    static void checkObjectTypes(QTreeView* treeView, const QSet<GObjectType>& acceptableTypes, const QModelIndex& parent = QModelIndex());

    static QString getSelectedItem();

    static QFont getFont(QModelIndex index);
    static QIcon getIcon(QModelIndex index);

    static void itemModificationCheck(const QString& itemName, bool modified = true);
    static void itemModificationCheck(QModelIndex index, bool modified = true);

    static void itemActiveCheck(QModelIndex index, bool active = true);

    static bool isVisible();

    static void dragAndDrop(const QModelIndex& from, const QModelIndex& to);
    static void dragAndDrop(const QModelIndex& from, QWidget* to);
    static void dragAndDrop(const QStringList& from, QWidget* to);
    static void dragAndDropSeveralElements(QModelIndexList from, QModelIndex to);

    static void expandProjectView();

    static void markSequenceAsCircular(const QString& sequenceObjectName);

    // Get all documents names with their object names (database connections are processed incorrectly)
    static QMap<QString, QStringList> getDocuments();

    // Counts visible top level items in project tree (after applying filtering, if present).
    static int countTopLevelItems();

    static const QString widgetName;

private:
    static void sendDragAndDrop(const QPoint& enterPos, const QPoint& dropPos);
    static void sendDragAndDrop(const QPoint& enterPos, QWidget* dropWidget);
};

}  // namespace U2
