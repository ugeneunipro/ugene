/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "AssemblyAnnotationsTreeItem.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

AssemblyAnnotationsTreeItem::AssemblyAnnotationsTreeItem(const QVariantList _data,
                    AssemblyAnnotationsTreeItem* _parent) : data(_data), parent(_parent) {
    if (nullptr != parent) {
        parent->children.append(this);
    }
}

AssemblyAnnotationsTreeItem::~AssemblyAnnotationsTreeItem() {
    qDeleteAll(children);
}

AssemblyAnnotationsTreeItem* AssemblyAnnotationsTreeItem::getChild(const int row) const {
    SAFE_POINT(children.size() >= row, "Non exsisting row", nullptr);

    return children.value(row);
}

AssemblyAnnotationsTreeItem* AssemblyAnnotationsTreeItem::takeChild(const int row) {
    SAFE_POINT(children.size() >= row, "Non exsisting row", nullptr);

    return children.takeAt(row);
}

AssemblyAnnotationsTreeItem* AssemblyAnnotationsTreeItem::getParent() const {
    return parent;
}

QVariant AssemblyAnnotationsTreeItem::getData(int column) const {
    SAFE_POINT(data.size() >= column, "Non exsisting column", QVariant());

    return data.value(column);
}

int AssemblyAnnotationsTreeItem::childrenCount() const {
    return children.size();
}

int AssemblyAnnotationsTreeItem::columnCount() const {
    return data.size();
}

int AssemblyAnnotationsTreeItem::getRowNum() const {
    CHECK(nullptr != parent, 0);
    return parent->children.indexOf(const_cast<AssemblyAnnotationsTreeItem*>(this));
}

}