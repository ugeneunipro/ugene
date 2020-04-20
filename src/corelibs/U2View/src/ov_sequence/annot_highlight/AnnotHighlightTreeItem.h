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

#ifndef _U2_ANNOT_HIGHLIGHT_TREE_ITEM_H_
#define _U2_ANNOT_HIGHLIGHT_TREE_ITEM_H_


#include <U2Core/AnnotationSettings.h>

#include <QTreeWidgetItem>

namespace U2 {

class U2VIEW_EXPORT AnnotHighlightTreeItem : public QTreeWidgetItem {
public:
    AnnotHighlightTreeItem(const QString& annotName, const QColor& annotColor);

    inline QString getName() { return annotName; }

    inline QColor getColor() { return annotColor; }
    void setColor(const QColor& _annotColor);

    static const int ROW_HEIGHT;
    static const int COLOR_ICON_SIZE;

private:
    void drawColorCell();

    QString annotName;
    QColor  annotColor;
};

} // namespace


#endif
