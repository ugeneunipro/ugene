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

#ifndef _U2_GUI_ANNOTATIONS_HIGHLIGHTING_TREE_VIEW_UTILS_H_
#define _U2_GUI_ANNOTATIONS_HIGHLIGHTING_TREE_VIEW_UTILS_H_

#include "GTGlobals.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace U2 {
using namespace HI;
class GTUtilsAnnotHighlightingTreeView {
public:
    static QTreeWidget* getTreeWidget(HI::GUITestOpStatus &os);

    // returns center or item's rect
    // fails if the item wasn't found
    static QPoint getItemCenter(HI::GUITestOpStatus &os, const QString &itemName);
    static void click(HI::GUITestOpStatus &os, const QString &itemName);

    static QTreeWidgetItem* findItem(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QString getSelectedItem(HI::GUITestOpStatus &os);

    static QColor getItemColor(HI::GUITestOpStatus &os, const QString &itemName);

    static const QString widgetName;
};

} // namespace

#endif
