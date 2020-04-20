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

#include <QTreeWidget>

#include <primitives/GTTreeWidget.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include "GTBaseCompleter.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTBaseCompleter"
#define GT_METHOD_NAME "click"
void GTBaseCompleter::click(HI::GUITestOpStatus &os, QWidget *widgetCompleterFor, const QString &seqName) {
    QTreeWidget *tree = getCompleter(os, widgetCompleterFor);
    GT_CHECK(tree != nullptr, "tree widget is NULL");
    QTreeWidgetItem* item = GTTreeWidget::findItem(os, tree, seqName);
    GT_CHECK(item != NULL, "item not found");
    tree->scrollToItem(item);
    GTGlobals::sleep(100);
    QRect itemRect = tree->visualItemRect(item);
    QPoint globalCenter = tree->mapToGlobal(itemRect.center());
    GTMouseDriver::moveTo(globalCenter);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNames"
QStringList GTBaseCompleter::getNames(HI::GUITestOpStatus &os, QWidget *widgetCompleterFor) {
    QTreeWidget *tree = getCompleter(os, widgetCompleterFor);
    GT_CHECK_RESULT(tree != NULL, "tree widget is NULL", QStringList());
    QStringList result;
    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach(QTreeWidgetItem* item, items){
        result << item->text(0);
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isEmpty"
bool GTBaseCompleter::isEmpty(HI::GUITestOpStatus &os, QWidget *widgetCompleterFor) {
    QTreeWidget *tree = getCompleter(os, widgetCompleterFor);
    GT_CHECK_RESULT(tree != NULL, "tree widget is NULL", true);
    QStringList items = getNames(os, widgetCompleterFor);
    bool result = (items.count() == 1) && (items.at(0) == "");
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCompleter"
QTreeWidget* GTBaseCompleter::getCompleter(HI::GUITestOpStatus &os, QWidget *widgetCompleterFor) {
    GT_CHECK_RESULT(widgetCompleterFor != NULL, "Widget associated with completer not found", NULL);
    QTreeWidget* completer = widgetCompleterFor->findChild<QTreeWidget*>();
    GT_CHECK_RESULT(completer != NULL, "auto completer widget not found", NULL);
    return completer;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME


}
