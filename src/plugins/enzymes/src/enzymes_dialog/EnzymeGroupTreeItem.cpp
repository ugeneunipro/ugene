/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "EnzymeGroupTreeItem.h"

#include "EnzymeTreeItem.h"

namespace U2 {

EnzymeGroupTreeItem::EnzymeGroupTreeItem(const QString& _s)
    : s(_s) {
    updateVisual();
}

void EnzymeGroupTreeItem::updateVisual() {
    int numChilds = childCount();
    checkedEnzymes.clear();
    for (int i = 0; i < numChilds; i++) {
        EnzymeTreeItem* item = static_cast<EnzymeTreeItem*>(child(i));
        if (item->checkState(0) == Qt::Checked) {
            checkedEnzymes.insert(item);
        }
    }
    QString text0 = s + " (" + QString::number(checkedEnzymes.size()) + ", " + QString::number(numChilds) + ")";
    setText(0, text0);

    if (numChilds > 0) {
        QString text4 = (static_cast<EnzymeTreeItem*>(child(0)))->enzyme->id;
        if (childCount() > 1) {
            text4 += " .. " + (static_cast<EnzymeTreeItem*>(child(numChilds - 1)))->enzyme->id;
        }
        setText(4, text4);
    }
}

bool EnzymeGroupTreeItem::operator<(const QTreeWidgetItem& other) const {
    if (other.parent() != nullptr) {
        return true;
    }
    int col = treeWidget()->sortColumn();
    return text(col) < other.text(col);
}
}  // namespace U2
