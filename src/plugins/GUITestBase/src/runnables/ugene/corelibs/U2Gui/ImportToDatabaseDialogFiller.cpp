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

#include <QDir>
#include <QFileInfo>

#include <QApplication>
#include <QHeaderView>
#include <QTreeWidget>

#include "ImportToDatabaseDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include "primitives/GTMenu.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CommonImportOptionsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ItemToImportEditDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {
using namespace HI;
namespace {

QMap<QString, QStringList> convertProjectItemsMap(const QMap<QString, QVariant>& map) {
    QMap<QString, QStringList> result;
    foreach (const QString& key, map.keys()) {
        result.insert(key, map.value(key).toStringList());
    }
    return result;
}

}   // an unnamed namespace

#define GT_CLASS_NAME "GTUtilsDialog::ImportToDatabaseDialogFiller"

const QString ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEM = "ACTION_DATA__ITEM";
const QString ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEMS_LIST = "ACTION_DATA__ITEMS_LIST";
const QString ImportToDatabaseDialogFiller::Action::ACTION_DATA__DESTINATION_FOLDER = "ACTION_DATA__DESTINATION_FOLDER";
const QString ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST = "ACTION_DATA__PATHS_LIST";
const QString ImportToDatabaseDialogFiller::Action::ACTION_DATA__PROJECT_ITEMS_LIST = "ACTION_DATA__PROJECT_ITEMS_LIST";

ImportToDatabaseDialogFiller::Action::Action(ImportToDatabaseDialogFiller::Action::Type type, const QVariantMap &data) :
    type(type),
    data(data)
{
}

ImportToDatabaseDialogFiller::ImportToDatabaseDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions) :
    Filler(os, "ImportToDatabaseDialog"),
    actions(actions)
{
}

#define GT_METHOD_NAME "run"
void ImportToDatabaseDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog, "activeModalWidget is NULL");

    foreach (const Action &action, actions) {
        switch (action.type) {
        case Action::ADD_FILES:
            addFiles(action);
            break;
        case Action::ADD_DIRS:
            addDirs(action);
            break;
        case Action::ADD_PROJECT_ITEMS:
            addProjectItems(action);
            break;
        case Action::SELECT_ITEMS:
            selectItems(action);
            break;
        case Action::EDIT_DESTINATION_FOLDER:
            editDestinationFolder(action);
            break;
        case Action::EDIT_GENERAL_OPTIONS:
            editGeneralOptions(action);
            break;
        case Action::EDIT_PRIVATE_OPTIONS:
            editPrivateOptions(action);
            break;
        case Action::RESET_PRIVATE_OPTIONS:
            resetPrivateOptions(action);
            break;
        case Action::REMOVE:
            remove(action);
            break;
        case Action::IMPORT:
            import(action);
            break;
        case Action::CANCEL:
            cancel(action);
            break;
        default:
            GT_CHECK(false, "An unrecognized action");
        }
        CHECK_OP(os, );
        GTGlobals::sleep(200);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addFiles"
void ImportToDatabaseDialogFiller::addFiles(const Action &action) {
    GT_CHECK(Action::ADD_FILES == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__PATHS_LIST), "Not enough parameters to perform the action");

    const QStringList filePaths = action.data.value(Action::ACTION_DATA__PATHS_LIST).toStringList();
    foreach (const QString& filePath, filePaths) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, filePath));

        QWidget* addFilesButton = GTWidget::findWidget(os, "pbAddFiles");
        GT_CHECK(NULL != addFilesButton, "addFilesButton is NULL");
        GTWidget::click(os, addFilesButton);

        GTGlobals::sleep(200);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addDirs"
void ImportToDatabaseDialogFiller::addDirs(const Action& action) {
    GT_CHECK(Action::ADD_DIRS == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__PATHS_LIST), "Not enough parameters to perform the action");

    const QStringList dirPaths = action.data.value(Action::ACTION_DATA__PATHS_LIST).toStringList();
    foreach (const QString& dirPath, dirPaths) {
        QFileInfo fi(dirPath);
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, fi.dir().path(), fi.fileName(), GTFileDialogUtils::Choose));

        QWidget* addDirsButton = GTWidget::findWidget(os, "pbAddFolder");
        GT_CHECK(NULL != addDirsButton, "addDirsButton is NULL");
        GTWidget::click(os, addDirsButton);

        GTGlobals::sleep(200);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addProjectItems"
void ImportToDatabaseDialogFiller::addProjectItems(const Action &action) {
    GT_CHECK(Action::ADD_PROJECT_ITEMS == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__PROJECT_ITEMS_LIST), "Not enough parameters to perform the action");

    const QMap<QString, QStringList> projectItems = convertProjectItemsMap(action.data.value(Action::ACTION_DATA__PROJECT_ITEMS_LIST).toMap());
    GTUtilsDialog::waitForDialog(os, new ProjectTreeItemSelectorDialogFiller(os, projectItems));

    QWidget* addProjectItemsButton = GTWidget::findWidget(os, "pbAddObjects");
    GT_CHECK(NULL != addProjectItemsButton, "addProjectItemsButton is NULL");
    GTWidget::click(os, addProjectItemsButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItems"
void ImportToDatabaseDialogFiller::selectItems(const Action &action) {
    GT_CHECK(Action::SELECT_ITEMS == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__ITEMS_LIST), "Not enough parameters to perform the action");

    const QStringList itemList = action.data.value(Action::ACTION_DATA__ITEMS_LIST).toStringList();
    GT_CHECK(!itemList.isEmpty(), "Items list to select is empty");

    if (itemList.size() > 1) {
        GTKeyboardDriver::keyPress(Qt::Key_Control);
    }

    foreach (const QString& itemText, itemList) {
        const QPoint itemCenter = getItemCenter(itemText);
        GTMouseDriver::moveTo(itemCenter);
        GTMouseDriver::click();
    }

    GTKeyboardDriver::keyRelease( Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "editDestinationFolder"
void ImportToDatabaseDialogFiller::editDestinationFolder(const Action &action) {
    GT_CHECK(Action::EDIT_DESTINATION_FOLDER == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__ITEM), "Not enough parameters to perform the action");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__DESTINATION_FOLDER), "Not enough parameters to perform the action");

    const QPoint itemCenter = getFolderColumnCenter(action.data.value(Action::ACTION_DATA__ITEM).toString());
    GTMouseDriver::moveTo(itemCenter);
    GTMouseDriver::doubleClick();

    const QString dstFolder = action.data.value(Action::ACTION_DATA__DESTINATION_FOLDER).toString();
    GTKeyboardDriver::keySequence(dstFolder);
    GTKeyboardDriver::keyClick( Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "editGeneralOptions"
void ImportToDatabaseDialogFiller::editGeneralOptions(const Action &action) {
    GT_CHECK(Action::EDIT_GENERAL_OPTIONS == action.type, "Invalid action type");

    GTUtilsDialog::waitForDialog(os, new CommonImportOptionsDialogFiller(os, action.data));

    QWidget* optionsButton = GTWidget::findWidget(os, "pbOptions");
    GT_CHECK(NULL != optionsButton, "optionsButton is NULL");
    GTWidget::click(os, optionsButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "editPrivateOptions"
void ImportToDatabaseDialogFiller::editPrivateOptions(const Action &action) {
    GT_CHECK(Action::EDIT_PRIVATE_OPTIONS == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__ITEM), "Not enough parameters to perform the action");

    GTUtilsDialog::waitForDialog(os, new ItemToImportEditDialogFiller(os, action.data));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Override options"));

    const QPoint itemCenter = getItemCenter(action.data.value(Action::ACTION_DATA__ITEM).toString());
    GTMouseDriver::moveTo(itemCenter);
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resetPrivateOptions"
void ImportToDatabaseDialogFiller::resetPrivateOptions(const Action &action) {
    GT_CHECK(Action::RESET_PRIVATE_OPTIONS == action.type, "Invalid action type");
    GT_CHECK(action.data.contains(Action::ACTION_DATA__ITEM), "Not enough parameters to perform the action");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Reset to general options"));
    const QPoint itemCenter = getItemCenter(action.data.value(Action::ACTION_DATA__ITEM).toString());
    GTMouseDriver::moveTo(itemCenter);
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "remove"
void ImportToDatabaseDialogFiller::remove(const Action &action) {
    GT_CHECK(Action::REMOVE == action.type, "Invalid action type");

    QWidget* removeButton = GTWidget::findWidget(os, "pbRemove");
    GT_CHECK(NULL != removeButton, "removeButton is NULL");
    GTWidget::click(os, removeButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "import"
void ImportToDatabaseDialogFiller::import(const Action &action) {
    GT_CHECK(Action::IMPORT == action.type, "Invalid action type");

    QWidget* importButton = GTWidget::findWidget(os, "import_button");
    GT_CHECK(NULL != importButton, "importButton is NULL");
    GTWidget::click(os, importButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "cancel"
void ImportToDatabaseDialogFiller::cancel(const Action &action) {
    GT_CHECK(Action::CANCEL == action.type, "Invalid action type");

    QWidget* cancelButton = GTWidget::findWidget(os, "cancel_button");
    GT_CHECK(NULL != cancelButton, "cancelButton is NULL");
    GTWidget::click(os, cancelButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint ImportToDatabaseDialogFiller::getItemCenter(const QString &text) {
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "twOrders", dialog));
    GT_CHECK_RESULT(NULL != treeWidget, "treeWidget is NULL", QPoint());

    QTreeWidgetItem* item = findItem(text);
    CHECK_OP(os, QPoint());

    const QPoint headerOffset = QPoint(0, treeWidget->header()->height());
    return treeWidget->mapToGlobal(treeWidget->visualItemRect(item).center() + headerOffset);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFolderColumnCenter"
QPoint ImportToDatabaseDialogFiller::getFolderColumnCenter(const QString &text) {
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "twOrders", dialog));
    GT_CHECK_RESULT(NULL != treeWidget, "treeWidget is NULL", QPoint());

    const QPoint itemCenter = treeWidget->mapFromGlobal(getItemCenter(text));
    const QPoint columnCenter(treeWidget->columnViewportPosition(1) + treeWidget->columnWidth(1) / 2, itemCenter.y());
    return treeWidget->mapToGlobal(columnCenter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem *ImportToDatabaseDialogFiller::findItem(const QString &text) {
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os, "twOrders", dialog));
    GT_CHECK_RESULT(NULL != treeWidget, "treeWidget is NULL", NULL);

    QList<QTreeWidgetItem*> items = treeWidget->findItems(text, Qt::MatchFlags(Qt::MatchExactly | Qt::MatchRecursive));
    GT_CHECK_RESULT(!items.isEmpty(), "Item was not found", NULL);
    GT_CHECK_RESULT(items.size() == 1, "Several items were found unexpectedly", NULL);
    return items.first();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
