/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_TREE_WIDGET_H_
#define _HI_GT_TREE_WIDGET_H_

#include "GTGlobals.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace HI {

class HI_EXPORT GTTreeWidget {
public:
    // expands treeWidget to the item and returns item's rect
    static QRect getItemRect(GUITestOpStatus &os, QTreeWidgetItem* item);

    static QPoint getItemCenter(GUITestOpStatus &os, QTreeWidgetItem* item);

    // recursively expands the tree making the given item visible
    // fails if item is NULL, item is hidden or item wasn't expanded
    static void expand(GUITestOpStatus &os, QTreeWidgetItem* item);

    // Checks the tree item or unchecks it if it is already checked
    static void checkItem(GUITestOpStatus &os, QTreeWidgetItem *item, int column = 0, GTGlobals::UseMethod method = GTGlobals::UseMouse);

    // gets all items under root recursively and returns a list of them
    static QList<QTreeWidgetItem*> getItems(QTreeWidgetItem* root);
    static QList<QTreeWidgetItem*> getItems(GUITestOpStatus &os, QTreeWidget *treeWidget);
    static QStringList getItemNames(GUITestOpStatus &os, QTreeWidget *treeWidget);

    static QTreeWidgetItem * findItem(GUITestOpStatus &os, QTreeWidget *tree, const QString &text, QTreeWidgetItem *parent = NULL, int column = 0, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static QList<QTreeWidgetItem *> findItems(GUITestOpStatus &os, QTreeWidget *tree, const QString &text, QTreeWidgetItem *parent = NULL, int column = 0, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void click(GUITestOpStatus &os, QTreeWidgetItem* item, int column = -1);  //  column == -1 - item center

    // Returns item level, 0 - top level item
    static int getItemLevel(GUITestOpStatus &os, QTreeWidgetItem *item);
private:
    static QTreeWidgetItem * findItemPrivate(GUITestOpStatus &os, QTreeWidget *tree, const QString &text, QTreeWidgetItem *parent = NULL, int column = 0, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
};

} // namespace

#endif
