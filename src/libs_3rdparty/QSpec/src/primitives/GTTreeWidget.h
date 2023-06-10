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
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTTreeWidget {
public:
    // expands treeWidget to the item and returns item's rect
    static QRect getItemRect(QTreeWidgetItem* item);

    static QPoint getItemCenter(QTreeWidgetItem* item);

    /**
     * Recursively expands the tree making the given item visible.
     * Fails if item is null or item is hidden or item can't be expanded (has no children).
     * Does nothing if the item already expanded.
     */
    static void expand(QTreeWidgetItem* item);

    /** Checks that tree item is expanded or fails. Waits for the item to be expanded if needed. */
    static void checkItemIsExpanded(QTreeWidgetItem* item);

    // Checks the tree item or unchecks it if it is already checked
    static void checkItem(QTreeWidgetItem* item, int column = 0, GTGlobals::UseMethod method = GTGlobals::UseMouse);

    // gets all items under root recursively and returns a list of them
    static QList<QTreeWidgetItem*> getItems(QTreeWidgetItem* root);
    static QList<QTreeWidgetItem*> getItems(QTreeWidget* treeWidget);
    static QStringList getItemNames(QTreeWidget* treeWidget);

    static QTreeWidgetItem* findItem(
        QTreeWidget* tree,
        const QString& text,
        QTreeWidgetItem* parent = nullptr,
        int column = 0,
        const GTGlobals::FindOptions& options = {},
        bool expandParent = true);

    static QList<QTreeWidgetItem*> findItems(
        QTreeWidget* tree,
        const QString& text,
        QTreeWidgetItem* parent = nullptr,
        int column = 0,
        const GTGlobals::FindOptions& options = {});

    static void click(QTreeWidgetItem* item, int column = -1, Qt::MouseButton button = Qt::LeftButton);  //  column == -1 - item center
    static void doubleClick(QTreeWidgetItem* item, int column = -1);

    // Returns item level, 0 - top level item
    static int getItemLevel(QTreeWidgetItem* item);

    /** Scroll to the item to guarantee the item is visible. */
    static void scrollToItem(QTreeWidgetItem* item);

    /** Returns visual string representation of the item for logging. */
    static QString toString(QTreeWidgetItem* item);

private:
    static QTreeWidgetItem* findItemPrivate(
        QTreeWidget* tree,
        const QString& text,
        QTreeWidgetItem* parent = nullptr,
        int column = 0,
        const GTGlobals::FindOptions& options = {});
};

}  // namespace HI
