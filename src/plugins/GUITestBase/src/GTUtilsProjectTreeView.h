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

#ifndef _U2_GUI_PROJECT_TREE_VIEW_UTILS_H_
#define _U2_GUI_PROJECT_TREE_VIEW_UTILS_H_

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
    static void rename(HI::GUITestOpStatus &os, const QString &itemName, const QString &newItemName, GTGlobals::UseMethod invokeMethod = GTGlobals::UseKey);
    static void rename(HI::GUITestOpStatus &os, const QModelIndex &itemIndex, const QString &newItemName, GTGlobals::UseMethod invokeMethod = GTGlobals::UseKey);

    /** Checks that project view is opened and fails if not. */
    static void checkProjectViewIsOpened(HI::GUITestOpStatus &os);

    /** Checks that project view is closed and fails if not. */
    static void checkProjectViewIsClosed(HI::GUITestOpStatus &os);

    static void openView(HI::GUITestOpStatus &os, GTGlobals::UseMethod method = GTGlobals::UseMouse);
    static void toggleView(HI::GUITestOpStatus &os, GTGlobals::UseMethod method = GTGlobals::UseMouse);

    /** Checks that tree item is expanded or fails. Waits for the item to be expanded if needed. */
    static void checkItemIsExpanded(HI::GUITestOpStatus &os, QTreeView *treeView, const QModelIndex &itemIndex);

    // returns center or item's rect
    // fails if the iteProjectm wasn't found
    static QPoint getItemCenter(HI::GUITestOpStatus &os, const QModelIndex &itemIndex);
    static QPoint getItemCenter(HI::GUITestOpStatus &os, QTreeView *treeView, const QModelIndex &itemIndex);
    static QPoint getItemCenter(HI::GUITestOpStatus &os, const QString &itemName);

    /** Locates item in the tree by name and scrolls to the item to make it visible. */
    static void scrollTo(HI::GUITestOpStatus &os, const QString &itemName);

    /** Scrolls to the item to make it visible. */
    static void scrollToIndexAndMakeExpanded(HI::GUITestOpStatus &os, QTreeView *treeView, const QModelIndex &index);

    static void doubleClickItem(HI::GUITestOpStatus &os, const QModelIndex &itemIndex);
    static void doubleClickItem(HI::GUITestOpStatus &os, const QString &itemName);
    static void click(HI::GUITestOpStatus &os, const QString &itemName, Qt::MouseButton button = Qt::LeftButton);
    static void click(HI::GUITestOpStatus &os, const QString &itemName, const QString &parentName, Qt::MouseButton button = Qt::LeftButton);

    static void callContextMenu(HI::GUITestOpStatus &os, const QString &itemName);
    static void callContextMenu(HI::GUITestOpStatus &os, const QString &itemName, const QString &parentName);
    static void callContextMenu(HI::GUITestOpStatus &os, const QModelIndex &itemIndex);

    static QTreeView *getTreeView(HI::GUITestOpStatus &os);
    static QModelIndex findIndex(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndex findIndex(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndex findIndex(HI::GUITestOpStatus &os, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndex findIndex(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndex findIndex(HI::GUITestOpStatus &os, const QStringList &itemPath, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndexList findIndeciesInProjectViewNoWait(HI::GUITestOpStatus &os,
                                        const QString &itemName,
                                        const QModelIndex &parent = QModelIndex(),
                                        int parentDepth = 0,
                                        const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QModelIndexList findIndeciesInTreeNoWait(HI::GUITestOpStatus &os,
                                        QTreeView *treeView,
                                        const QString &itemName,
                                        const QModelIndex &parent = QModelIndex(),
                                        int parentDepth = 0,
                                        const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void filterProject(HI::GUITestOpStatus &os, const QString &searchField);
    static void filterProjectSequental(HI::GUITestOpStatus &os, const QStringList &searchField, bool waitUntilSearchEnd);
    static QModelIndexList findFilteredIndexes(HI::GUITestOpStatus &os, const QString &substring, const QModelIndex &parentIndex = QModelIndex());
    static void checkFilteredGroup(HI::GUITestOpStatus &os, const QString &groupName, const QStringList &namesToCheck, const QStringList &alternativeNamesToCheck, const QStringList &excludedNames, const QStringList &skipGroupIfContains = QStringList());
    static void ensureFilteringIsDisabled(HI::GUITestOpStatus &os);

    // returns true if the item exists, does not set error unlike findIndex method
    static bool checkItem(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static bool checkItem(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static bool checkItem(HI::GUITestOpStatus &os, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static bool checkItem(HI::GUITestOpStatus &os, QTreeView *treeView, const QString &itemName, const QModelIndex &parent, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void checkNoItem(HI::GUITestOpStatus &os, const QString &itemName);

    // the method does nothing if `acceptableTypes` is an empty set
    static void checkObjectTypes(HI::GUITestOpStatus &os, const QSet<GObjectType> &acceptableTypes, const QModelIndex &parent = QModelIndex());
    static void checkObjectTypes(HI::GUITestOpStatus &os, QTreeView *treeView, const QSet<GObjectType> &acceptableTypes, const QModelIndex &parent = QModelIndex());

    static QString getSelectedItem(HI::GUITestOpStatus &os);

    static QFont getFont(HI::GUITestOpStatus &os, QModelIndex index);
    static QIcon getIcon(HI::GUITestOpStatus &os, QModelIndex index);

    static void itemModificationCheck(HI::GUITestOpStatus &os, const QString &itemName, bool modified = true);
    static void itemModificationCheck(HI::GUITestOpStatus &os, QModelIndex index, bool modified = true);

    static void itemActiveCheck(HI::GUITestOpStatus &os, QModelIndex index, bool active = true);

    static bool isVisible(HI::GUITestOpStatus &os);

    static void dragAndDrop(HI::GUITestOpStatus &os, const QModelIndex &from, const QModelIndex &to);
    static void dragAndDrop(HI::GUITestOpStatus &os, const QModelIndex &from, QWidget *to);
    static void dragAndDrop(HI::GUITestOpStatus &os, const QStringList &from, QWidget *to);
    static void dragAndDropSeveralElements(HI::GUITestOpStatus &os, QModelIndexList from, QModelIndex to);

    static void expandProjectView(HI::GUITestOpStatus &os);

    static void markSequenceAsCircular(HI::GUITestOpStatus &os, const QString &sequenceObjectName);

    // Get all documents names with their object names (database connections are processed incorrectly)
    static QMap<QString, QStringList> getDocuments(HI::GUITestOpStatus &os);

    static const QString widgetName;

private:
    static void sendDragAndDrop(HI::GUITestOpStatus &os, const QPoint &enterPos, const QPoint &dropPos);
    static void sendDragAndDrop(HI::GUITestOpStatus &os, const QPoint &enterPos, QWidget *dropWidget);
};

}    // namespace U2

#endif
