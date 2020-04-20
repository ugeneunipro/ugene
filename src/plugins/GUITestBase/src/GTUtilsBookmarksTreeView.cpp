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

#include <QMainWindow>
#include <QTreeWidget>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <utils/GTUtilsDialog.h>

#include <U2Core/ProjectModel.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewTreeController.h>

#include <U2View/AnnotationsTreeView.h>

#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsBookmarksTreeView"

const QString GTUtilsBookmarksTreeView::widgetName = ACTION_BOOKMARK_TREE_VIEW;

#define GT_METHOD_NAME "getTreeWidget"
QTreeWidget* GTUtilsBookmarksTreeView::getTreeWidget(GUITestOpStatus &os) {

    QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, widgetName, NULL, false));

    if (!treeWidget) {
        GTUtilsProjectTreeView::toggleView(os);
        GTGlobals::sleep(3000);
    }

    treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, widgetName));
    return treeWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTUtilsBookmarksTreeView::findItem(GUITestOpStatus &os, const QString &itemName, const GTGlobals::FindOptions &options) {

    GT_CHECK_RESULT(itemName.isEmpty() == false, "Item name is empty", NULL);

    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    for(int i =0; i< treeWidget->topLevelItemCount(); i++) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(treeWidget->topLevelItem(i));
        if (vi->viewName == itemName) {
            return vi;
        }
        if (vi->isRootItem()){
            QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(vi);
            foreach (QTreeWidgetItem* item, treeItems) {
                if (item->text(0) == itemName) {
                    return item;
                }
            }
        }
    }
    GT_CHECK_RESULT(options.failIfNotFound == false, "Item " + itemName + " not found in tree widget", NULL);

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedItem"
QString GTUtilsBookmarksTreeView::getSelectedItem(GUITestOpStatus &os)
{
    QTreeWidget *treeWidget = getTreeWidget(os);
    GT_CHECK_RESULT(treeWidget != NULL, "Tree widget is NULL", NULL);

    for(int i =0; i< treeWidget->topLevelItemCount(); i++) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(treeWidget->topLevelItem(i));
        if (vi->isSelected()) {
            return vi->viewName;
        }
    }

    return QString();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addBookmark"
void GTUtilsBookmarksTreeView::addBookmark(GUITestOpStatus &os, const QString &viewName, const QString &bookmarkName) {
    Q_UNUSED(os);
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_ADD_BOOKMARK));
    GTMouseDriver::moveTo(getItemCenter(os, viewName));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(500);

    if (!bookmarkName.isEmpty()) {
        QWidget *bookmarkLineEdit = getTreeWidget(os)->itemWidget(getTreeWidget(os)->currentItem(), 0);
        GTLineEdit::setText(os, qobject_cast<QLineEdit *>(bookmarkLineEdit), bookmarkName);
    }
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "deleteBookmark"
void GTUtilsBookmarksTreeView::deleteBookmark(GUITestOpStatus &os, const QString &bookmarkName) {
    clickBookmark(os, bookmarkName);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickBookmark"
void GTUtilsBookmarksTreeView::clickBookmark(GUITestOpStatus &os, const QString &bookmarkName) {
    GTMouseDriver::moveTo(getItemCenter(os, bookmarkName));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickBookmark"
void GTUtilsBookmarksTreeView::doubleClickBookmark(GUITestOpStatus &os, const QString &bookmarkName) {
    GTMouseDriver::moveTo(getItemCenter(os, bookmarkName));
    GTMouseDriver::doubleClick();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsBookmarksTreeView::getItemCenter(GUITestOpStatus &os, const QString &itemName) {

    QTreeWidgetItem* item = findItem(os, itemName);
    GT_CHECK_RESULT(item != NULL, "Item " + itemName + " is NULL", QPoint());

    return GTTreeWidget::getItemCenter(os, item);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
