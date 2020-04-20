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

#ifndef GTTREEVIEW_H
#define GTTREEVIEW_H

#include "GTGlobals.h"
#include <QTreeView>

namespace HI{

class HI_EXPORT GTTreeView
{
public:
    //find index with data and role in the tree view
    static QModelIndex findIndex(GUITestOpStatus &os, QTreeView* treeView, QVariant data, Qt::ItemDataRole role = Qt::DisplayRole,
                                 const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    //find index with data and role for current parent index
    static QModelIndex findIndex(GUITestOpStatus &os, QTreeView* treeView, QVariant data, QModelIndex parent, Qt::ItemDataRole role = Qt::DisplayRole,
                                 const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    static QPoint getItemCenter(GUITestOpStatus &os, QTreeView* treeView, const QModelIndex &itemIndex);

private:
    static QModelIndexList findIndexes(GUITestOpStatus &os, QTreeView* treeView, QVariant data, Qt::ItemDataRole role = Qt::DisplayRole,
                                       QModelIndex parent = QModelIndex(), int depth = 0, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
};

}

#endif // GTTREEVIEW_H
