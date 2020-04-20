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

#include "TreeWidgetUtils.h"

namespace U2 {

static void visitBranchDFS(QTreeWidgetItem* item, TreeWidgetVisitor* visitor) {
    if (visitor->isChildVisitRequired(item)) {
        for (int i = 0; i < item->childCount(); i++) {    
            QTreeWidgetItem* child = item->child(i);
            visitBranchDFS(child, visitor);
        }
    }
    visitor->visit(item);
}

void TreeWidgetUtils::visitDFS(QTreeWidget* tree, TreeWidgetVisitor* visitor) {
    for (int i = 0; i < tree->topLevelItemCount(); i++) {
        QTreeWidgetItem* child = tree->topLevelItem(i);
        visitBranchDFS(child, visitor);
    }
}

} //namespace
