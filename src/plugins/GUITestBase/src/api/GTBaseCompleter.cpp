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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QTreeWidget>

#include "GTBaseCompleter.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTBaseCompleter"
void GTBaseCompleter::click(QWidget* widgetCompleterFor, const QString& seqName) {
    QTreeWidget* tree = getCompleter(widgetCompleterFor);
    GT_CHECK(tree != nullptr, "tree widget is NULL");
    QTreeWidgetItem* item = GTTreeWidget::findItem(tree, seqName);
    GT_CHECK(item != nullptr, "item not found");
    GTTreeWidget::scrollToItem(item);
    QRect itemRect = tree->visualItemRect(item);
    QPoint globalCenter = tree->mapToGlobal(itemRect.center());
    GTMouseDriver::moveTo(globalCenter);
    GTMouseDriver::click();
}

QStringList GTBaseCompleter::getNames(QWidget* widgetCompleterFor) {
    QTreeWidget* tree = getCompleter(widgetCompleterFor);
    GT_CHECK_RESULT(tree != nullptr, "tree widget is NULL", QStringList());
    QStringList result;
    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item, items) {
        result << item->text(0);
    }
    return result;
}

bool GTBaseCompleter::isEmpty(QWidget* widgetCompleterFor) {
    QTreeWidget* tree = getCompleter(widgetCompleterFor);
    GT_CHECK_RESULT(tree != nullptr, "tree widget is NULL", true);
    QStringList items = getNames(widgetCompleterFor);
    bool result = (items.count() == 1) && (items.at(0) == "");
    return result;
}

QTreeWidget* GTBaseCompleter::getCompleter(QWidget* widgetCompleterFor) {
    GT_CHECK_RESULT(widgetCompleterFor != nullptr, "Widget associated with completer not found", nullptr);
    QTreeWidget* completer = widgetCompleterFor->findChild<QTreeWidget*>();
    GT_CHECK_RESULT(completer != nullptr, "auto completer widget not found", nullptr);
    return completer;
}

#undef GT_CLASS_NAME

}  // namespace U2
