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

#ifndef GTTREEVIEW_H
#define GTTREEVIEW_H

#include <QTreeView>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTTreeView {
public:
    // find index with data and role in the tree view
    static QModelIndex findIndex(GUITestOpStatus& os, QTreeView* tree, QVariant data, Qt::ItemDataRole role = Qt::DisplayRole, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    // find index with data and role for current parent index
    static QModelIndex findIndex(GUITestOpStatus& os, QTreeView* tree, QVariant data, QModelIndex parent, Qt::ItemDataRole role = Qt::DisplayRole, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    static QPoint getItemCenter(GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex);

    /** Expands given item. */
    static void expand(GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex);

    /** Clicks given item. */
    static void click(GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex, const Qt::Key& keyModifier = Qt::Key_unknown);

    /** Clicks given item. */
    static void doubleClick(GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex);

    /** Scrolls the item into view. */
    static void scrollToItem(GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex);

    /** Checks that tree item is expanded or fails. Waits for the item to be expanded if needed. */
    static void checkItemIsExpanded(HI::GUITestOpStatus& os, QTreeView* tree, const QModelIndex& itemIndex);

private:
    static QModelIndexList findIndexes(GUITestOpStatus& os,
                                       QTreeView* tree,
                                       QVariant data,
                                       Qt::ItemDataRole role = Qt::DisplayRole,
                                       QModelIndex parent = QModelIndex(),
                                       int depth = 0,
                                       const GTGlobals::FindOptions& = GTGlobals::FindOptions());
};

}  // namespace HI

#endif  // GTTREEVIEW_H
