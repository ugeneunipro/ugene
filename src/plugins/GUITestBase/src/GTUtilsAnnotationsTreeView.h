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

#ifndef _U2_GUI_ANNOTATIONS_TREE_VIEW_UTILS_H_
#define _U2_GUI_ANNOTATIONS_TREE_VIEW_UTILS_H_

#include "GTGlobals.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace U2 {
using namespace HI;

class AVItem;
class U2Region;

class GTUtilsAnnotationsTreeView {
public:
    static QTreeWidget* getTreeWidget(HI::GUITestOpStatus &os);

    static void addAnnotationsTableFromProject(HI::GUITestOpStatus &os, const QString &tableName);

    // returns center or item's rect
    // fails if the item wasn't found
    static QPoint getItemCenter(HI::GUITestOpStatus &os, const QString &itemName);

    static QTreeWidgetItem * findFirstAnnotation(HI::GUITestOpStatus &os, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QTreeWidgetItem * findItem(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QTreeWidgetItem * findItem(HI::GUITestOpStatus &os, const QString &itemName, QTreeWidgetItem* parentItem, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QTreeWidgetItem * findItemWithIndex(HI::GUITestOpStatus &os, const QString &itemName, const int index);
    static QList<QTreeWidgetItem*> findItems(HI::GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    static QStringList getGroupNames(HI::GUITestOpStatus &os, const QString &annotationTableName = "");

    static QStringList getAnnotationNamesOfGroup(HI::GUITestOpStatus &os, const QString &groupName);
    static QList<U2Region> getAnnotatedRegionsOfGroup(HI::GUITestOpStatus &os, const QString &groupName);
    static QList<U2Region> getAnnotatedRegionsOfGroup(HI::GUITestOpStatus &os, const QString &groupName, const QString &parentName);

    // finds given region in annotation with a given name. U2Region: visible {begin, end} positions.
    static bool findRegion(HI::GUITestOpStatus &os, const QString &itemName, const U2Region& region);

    static QString getSelectedItem(HI::GUITestOpStatus &os);
    static QList<QTreeWidgetItem*> getAllSelectedItems(HI::GUITestOpStatus &os);
    static QString getAVItemName(HI::GUITestOpStatus &os, AVItem* avItem);
    static QString getQualifierValue(HI::GUITestOpStatus &os, const QString &qualifierName, QTreeWidgetItem *parentItem);
    static QString getQualifierValue(HI::GUITestOpStatus &os, const QString &qualName, const QString &parentName);
    static QList<U2Region> getAnnotatedRegions(HI::GUITestOpStatus &os);
    static QList<U2Region> getSelectedAnnotatedRegions(HI::GUITestOpStatus &os);
    static QString getAnnotationRegionString(HI::GUITestOpStatus &os, const QString &annotationName);
    static QString getAnnotationType(HI::GUITestOpStatus &os, const QString &annotationName);

    static void createQualifier(HI::GUITestOpStatus &os, const QString &qualName, const QString &qualValue, const QString &parentName);

    static void selectItems(HI::GUITestOpStatus &os, const QStringList& items);
    static void selectItems(HI::GUITestOpStatus &os, const QList<QTreeWidgetItem *> &items);

    static void clickItem(HI::GUITestOpStatus &os, const QString &item, const int numOfItem, bool isDoubleClick);

    // location string format: 1..51
    static void createAnnotation(HI::GUITestOpStatus &os, const QString &groupName, const QString &annotationName, const QString &location, bool createNewTable = true, const QString &saveTo = "");

    static void deleteItem(HI::GUITestOpStatus &os, const QString &itemName);
    static void deleteItem(HI::GUITestOpStatus &os, QTreeWidgetItem *item);

    static void callContextMenuOnItem(HI::GUITestOpStatus &os, QTreeWidgetItem *item);
    static void callContextMenuOnItem(HI::GUITestOpStatus &os, const QString &itemName);
    static void callContextMenuOnQualifier(HI::GUITestOpStatus &os, const QString &parentName, const QString &qualifierName);

    static const QString widgetName;
};

} // namespace

#endif
