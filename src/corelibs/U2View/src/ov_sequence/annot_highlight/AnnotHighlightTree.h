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

#ifndef _U2_ANNOT_HIGHLIGHT_TREE_H_
#define _U2_ANNOT_HIGHLIGHT_TREE_H_

#include <QTreeWidget>

namespace U2 {

class AnnotationSettings;

class AnnotHighlightTree : public QTreeWidget
{
    Q_OBJECT
public:
    AnnotHighlightTree();
    virtual void addItem(QString annotName, QColor annotColor);
    virtual void clear();

    /**
     * Set the first item of the tree as current item (and therefore selected)
     * The tree should have items to call this function!
     */
    void setFirstItemSelected();

    /**
     * Returns the annotation name the first tree item.
     * The tree should have items to call this function!
     */
    QString getFirstItemAnnotName();

     /**
      * Returns the annotation name of the currently selected tree item,
      * or an empty string.
      */
    QString getCurrentItemAnnotName();

    /**
     * Set the tree item with the specified annotation name as current tree item (and therefore selected)
     * The tree should have the item to call this function!
     */
    void setItemSelectedWithAnnotName(QString annotName);

    /** Numbers of columns and their headers */
    static const int COL_NUM_ANNOT_NAME;
    static const int COL_NUM_COLOR;

signals:
    void si_selectedItemChanged(QString annotName);
    void si_colorChanged(QString annotName, QColor annotColor);

private slots:
    void sl_onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void sl_onItemClicked(QTreeWidgetItem* item, int column);

private:
    /** Do not use these methods to add an item to the tree, use addItem(...) instead */
    virtual void addTopLevelItem(QTreeWidgetItem* item) { QTreeWidget::addTopLevelItem(item); }
    virtual void addTopLevelItems(const QList<QTreeWidgetItem*>& items) { QTreeWidget::addTopLevelItems(items); }
    virtual void insertTopLevelItem(int index, QTreeWidgetItem* item) { QTreeWidget::insertTopLevelItem(index, item); }
    virtual void insertTopLevelItems(int index,  const QList<QTreeWidgetItem *>& items) { QTreeWidget::insertTopLevelItems(index, items); }

    /** Size of the tree elements */
    static const int COLOR_COLUMN_WIDTH;

    static const int INITIAL_TREE_HEIGHT;
    static const int ROW_HEIGHT;

    int annotTreeHeight;
};

} // namespace

#endif
