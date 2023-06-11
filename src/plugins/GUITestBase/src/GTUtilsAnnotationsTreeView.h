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

#include "GTGlobals.h"

class QTreeWidget;
class QTreeWidgetItem;

namespace U2 {
using namespace HI;

class AVItem;
class U2Region;

class GTUtilsAnnotationsTreeView {
public:
    static QTreeWidget* getTreeWidget();

    static void addAnnotationsTableFromProject(const QString& tableName);

    // returns center or item's rect
    // fails if the item wasn't found
    static QPoint getItemCenter(const QString& itemName);

    static QTreeWidgetItem* findFirstAnnotation(const GTGlobals::FindOptions& options = {}, bool expandParent = true);
    static QTreeWidgetItem* findItem(
        const QString& itemName,
        QTreeWidgetItem* parentItem = nullptr,
        const GTGlobals::FindOptions& = {},
        bool expandParent = true);
    static QTreeWidgetItem* findItemWithIndex(const QString& itemName, int index, bool expandParent = true);
    static QList<QTreeWidgetItem*> findItems(const QString& itemName, const GTGlobals::FindOptions& = {});

    /** Find and expands single item by name. Fails if the item is not found or if multiple items with this name are present. */
    static QTreeWidgetItem* expandItem(const QString& itemName);

    static QStringList getGroupNames(const QString& annotationTableName = "");

    static QStringList getAnnotationNamesOfGroup(const QString& groupName);
    static QList<U2Region> getAnnotatedRegionsOfGroup(const QString& groupName);
    static QList<U2Region> getAnnotatedRegionsOfGroup(const QString& groupName, const QString& parentName);

    // finds given region in annotation with a given name. U2Region: visible {begin, end} positions.
    static bool findRegion(const QString& itemName, const U2Region& region);

    static QString getSelectedItem();
    static QList<QTreeWidgetItem*> getAllSelectedItems();
    static QString getAVItemName(AVItem* avItem);

    /** Returns qualifier value of the annotation item. Expands item if it is not expanded to make qualifiers visible. */
    static QString getQualifierValue(const QString& qualifierName, QTreeWidgetItem* annotationItem);

    /** Returns qualifier value of the item by item name. Expands item if it is not expanded to make qualifiers visible. */
    static QString getQualifierValue(const QString& qualifierName, const QString& annotationName);

    static QList<U2Region> getAnnotatedRegions();
    static QList<U2Region> getSelectedAnnotatedRegions();
    static QString getAnnotationRegionString(const QString& annotationName);
    static QString getAnnotationType(const QString& annotationName);

    static void createQualifier(const QString& qualifierName, const QString& qualifierValue, const QString& annotationName);
    static void createQualifier(const QString& qualifierName, const QString& qualifierValue, QTreeWidgetItem* annotation);

    static void selectItemsByName(const QStringList& items);
    static void selectItems(const QList<QTreeWidgetItem*>& items);

    static void clickItem(const QString& item, int itemIndex, bool isDoubleClick);

    // location string format: 1..51
    static void createAnnotation(const QString& groupName, const QString& annotationName, const QString& location, bool createNewTable = true, const QString& saveTo = "");

    static void deleteItem(const QString& itemName);
    static void deleteItem(QTreeWidgetItem* item);

    static void callContextMenuOnItem(QTreeWidgetItem* item);
    static void callContextMenuOnItem(const QString& itemName);
    static void callContextMenuOnQualifier(const QString& parentName, const QString& qualifierName);

    /** Checks that there are no annotations in the annotations view. */
    static void checkNoAnnotations();

    /**
     * Checks annotations in the annotation view with @annotationRegions in the @groupName annotation group.
     */
    static void checkAnnotationRegions(const QString& groupName, const QList<QPair<int, int>>& annotationRegionsStartAndEnd);
};

}  // namespace U2
