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

#include <QColorDialog>
#include <QHeaderView>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/U2SafePoints.h>

#include "AnnotHighlightTree.h"
#include "AnnotHighlightTreeItem.h"

namespace U2 {

const int AnnotHighlightTree::COL_NUM_ANNOT_NAME = 0;
const int AnnotHighlightTree::COL_NUM_COLOR = 1;

const int AnnotHighlightTree::COLOR_COLUMN_WIDTH = 60;
const int AnnotHighlightTree::INITIAL_TREE_HEIGHT = 25;


AnnotHighlightTree::AnnotHighlightTree()
{
    setObjectName("OP_ANNOT_HIGHLIGHT_TREE");

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setIndentation(0);

    setSelectionMode(SingleSelection);

    annotTreeHeight = INITIAL_TREE_HEIGHT;

    QStringList headerLabels;
    headerLabels << QObject::tr("Annotation") << QObject::tr("Color");
    setHeaderLabels(headerLabels);

    header()->setSectionResizeMode(COL_NUM_ANNOT_NAME, QHeaderView::Stretch);
    header()->setSectionResizeMode(COL_NUM_COLOR, QHeaderView::Fixed);
    header()->setStretchLastSection(false);
    header()->resizeSection(COL_NUM_COLOR, COLOR_COLUMN_WIDTH);

    setStyleSheet("QTreeWidget#OP_ANNOT_HIGHLIGHT_TREE { "
        "border-style: solid;"
        "border-color: palette(mid);"
        "border-width: 1px;"
        "background: white;"
        "margin-left: 5px;"
        "margin-right: 10px;"
        " }");

    // Emit signal that annotation type has changed
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        SLOT(sl_onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    // Listen for click on "Color" column
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
        SLOT(sl_onItemClicked(QTreeWidgetItem*, int)));
}

void AnnotHighlightTree::addItem(QString annotName, QColor annotColor)
{
    // Create and initialize the tree item
    QTreeWidgetItem* item = new AnnotHighlightTreeItem(annotName, annotColor);

    // Add the item and increase the tree height
    annotTreeHeight += AnnotHighlightTreeItem::ROW_HEIGHT;
    QTreeWidget::addTopLevelItem(item);
    updateGeometry();
}


void AnnotHighlightTree::clear()
{
    annotTreeHeight = INITIAL_TREE_HEIGHT;
    QTreeWidget::clear();
}


void AnnotHighlightTree::setFirstItemSelected()
{
    QTreeWidgetItem* firstItem = topLevelItem(0);
    SAFE_POINT(0 != firstItem, "There is no first item in the tree!",);
    setCurrentItem(firstItem);
}


QString AnnotHighlightTree::getFirstItemAnnotName()
{
    QTreeWidgetItem* firstItem = topLevelItem(0);
    SAFE_POINT(0 != firstItem, "There is no first item in the tree!", QString());

    QString annotName = firstItem->text(COL_NUM_ANNOT_NAME);
    return annotName;
}


QString AnnotHighlightTree::getCurrentItemAnnotName()
{
    QTreeWidgetItem* current = currentItem();
    if (0 != current) {
        return current->text(COL_NUM_ANNOT_NAME);
    }
    else {
        return QString();
    }
}


void AnnotHighlightTree::setItemSelectedWithAnnotName(QString annotName)
{
    CHECK(!annotName.isEmpty(), );
    QList<QTreeWidgetItem*> items = findItems(annotName, Qt::MatchExactly, COL_NUM_ANNOT_NAME);
    SAFE_POINT(1 == items.count(), "Exactly one tree item with the specified annotation name should have been found.",);

    setCurrentItem(items[0]);
}

void AnnotHighlightTree::sl_onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /* previous */ )
{
    if (0 != current) {
        QString annotName = current->text(COL_NUM_ANNOT_NAME);
        emit si_selectedItemChanged(annotName);
    }
}

void AnnotHighlightTree::sl_onItemClicked(QTreeWidgetItem* item, int column)
{
    if (column != COL_NUM_COLOR) {
        return;
    }

    AnnotHighlightTreeItem* annotHighlightItem = static_cast<AnnotHighlightTreeItem*>(item);

    QColorDialog::ColorDialogOption options = static_cast<QColorDialog::ColorDialogOption>(0);
    if (qgetenv(ENV_GUI_TEST) == "1") {
        options = QColorDialog::DontUseNativeDialog;
    }
    QColor annotColor = QColorDialog::getColor(annotHighlightItem->getColor(), this, "", options);
    if (annotColor.isValid()) {
        annotHighlightItem->setColor(annotColor);
        emit si_colorChanged(annotHighlightItem->getName(), annotColor);
    }
}


} // namespace
