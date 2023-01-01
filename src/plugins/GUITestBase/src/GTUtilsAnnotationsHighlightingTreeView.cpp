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

#include "GTUtilsAnnotationsHighlightingTreeView.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QMainWindow>
#include <QTreeWidget>

#include <U2Core/ProjectModel.h>

#include <U2Gui/MainWindow.h>

#include <U2View/AnnotHighlightTreeItem.h>

#include "GTUtilsMdi.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "utils/GTKeyboardUtils.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsAnnotHighlightingTreeView"

const QString GTUtilsAnnotHighlightingTreeView::widgetName = "OP_ANNOT_HIGHLIGHT_TREE";

#define GT_METHOD_NAME "getTreeWidget"
QTreeWidget* GTUtilsAnnotHighlightingTreeView::getTreeWidget(HI::GUITestOpStatus& os) {
    auto treeWidget = GTWidget::findTreeWidget(os, widgetName, nullptr, false);

    if (treeWidget == nullptr) {
        GTWidget::click(os, GTWidget::findWidget(os, "OP_ANNOT_HIGHLIGHT"));
        GTGlobals::sleep(3000);
    }

    return GTWidget::findTreeWidget(os, widgetName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTUtilsAnnotHighlightingTreeView::findItem(HI::GUITestOpStatus& os, const QString& itemName, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", nullptr);

    QTreeWidget* treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget is NULL", nullptr);

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        AnnotHighlightTreeItem* vi = static_cast<AnnotHighlightTreeItem*>(treeWidget->topLevelItem(i));
        if (vi->getName() == itemName) {
            return vi;
        }
    }
    GT_CHECK_RESULT(options.failIfNotFound == false, "Item " + itemName + " not found in tree widget", nullptr);

    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedItem"
QString GTUtilsAnnotHighlightingTreeView::getSelectedItem(HI::GUITestOpStatus& os) {
    QTreeWidget* treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget is NULL", nullptr);

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        AnnotHighlightTreeItem* vi = static_cast<AnnotHighlightTreeItem*>(treeWidget->topLevelItem(i));
        if (vi->isSelected()) {
            return vi->getName();
        }
    }

    return QString();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsAnnotHighlightingTreeView::getItemCenter(HI::GUITestOpStatus& os, const QString& itemName) {
    QTreeWidgetItem* item = findItem(os, itemName);
    GT_CHECK_RESULT(item != nullptr, "Item " + itemName + " is NULL", QPoint());

    return GTTreeWidget::getItemCenter(os, item);
}
#undef GT_METHOD_NAME

void GTUtilsAnnotHighlightingTreeView::click(HI::GUITestOpStatus& os, const QString& itemName) {
    QTreeWidgetItem* item = findItem(os, itemName);
    QTreeWidget* tree = getTreeWidget(os);
    tree->scrollToItem(item);
    GTThread::waitForMainThread();

    QPoint p = getItemCenter(os, itemName);
    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
}

#define GT_METHOD_NAME "getItemCenter"
QColor GTUtilsAnnotHighlightingTreeView::getItemColor(HI::GUITestOpStatus& os, const QString& itemName) {
    QTreeWidgetItem* item = findItem(os, itemName);
    QIcon icon = item->icon(1);
    QPixmap pixmap = icon.pixmap(5, 5);
    QImage img = pixmap.toImage();
    QRgb rgb = img.pixel(QPoint(2, 2));
    QColor result = QColor(rgb);
    return result;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
