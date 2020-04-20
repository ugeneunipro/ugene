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

#include <QTreeView>

#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ProjectViewModel.h>

#include "GTDatabaseConfig.h"
#include "utils/GTUtilsDialog.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AddFolderDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportToDatabaseDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsSharedDatabaseDocument"

#define GT_METHOD_NAME "connectToTestDatabase"
Document* GTUtilsSharedDatabaseDocument::connectToTestDatabase(HI::GUITestOpStatus &os) {
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTThread::waitForMainThread();

    CHECK_SET_ERR_RESULT(!lt.hasError(), "errors in log", NULL);

    return GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, conName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "connectToUgenePublicDatabase"
Document *GTUtilsSharedDatabaseDocument::connectToUgenePublicDatabase(HI::GUITestOpStatus &os) {
    GTLogTracer lt;
    QString conName = "UGENE public database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Connect to UGENE shared database...");

    CHECK_SET_ERR_RESULT(!lt.hasError(), "errors in log", NULL);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    return GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, conName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDatabaseDocumentByName"
Document *GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(HI::GUITestOpStatus &os, const QString &name) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(!name.isEmpty(), "Name is empty", NULL);

    const QModelIndex databaseDocIndex = GTUtilsProjectTreeView::findIndex(os, name);
    GT_CHECK_RESULT(databaseDocIndex.isValid(), QString("Can't find the document with name '%1'").arg(name), NULL);

    return ProjectViewModel::toDocument(databaseDocIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "disconnectDatabase"
void GTUtilsSharedDatabaseDocument::disconnectDatabase(HI::GUITestOpStatus &os, Document *databaseDoc) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    disconnectDatabase(os, databaseDoc->getName());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "disconnectDatabase"
void GTUtilsSharedDatabaseDocument::disconnectDatabase(HI::GUITestOpStatus &os, const QString &name) {
    Q_UNUSED(os);
    GTUtilsDocument::removeDocument(os, name);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getFolderItem"
QModelIndex GTUtilsSharedDatabaseDocument::getItemIndex(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath, bool mustExist) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != databaseDoc, "databaseDoc is NULL", QModelIndex());
    GT_CHECK_RESULT(!itemPath.isEmpty(), "Folder path is empty", QModelIndex());

    const QStringList folders = itemPath.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);

    QModelIndex itemIndex = GTUtilsProjectTreeView::findIndex(os, databaseDoc->getName());
    CHECK(!folders.isEmpty(), itemIndex);

    GTGlobals::FindOptions options;
    options.depth = 1;
    options.failIfNotFound = mustExist;
    foreach (const QString& folder, folders) {
        itemIndex = GTUtilsProjectTreeView::findIndex(os, folder, itemIndex, options);
        CHECK_OP_BREAK(os);
        CHECK_BREAK(itemIndex.isValid());
    }

    return itemIndex;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createFolder"
void GTUtilsSharedDatabaseDocument::createFolder(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &parentFolderPath, const QString &newFolderName) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!parentFolderPath.isEmpty(), "Parent folder path is empty");
    GT_CHECK(!newFolderName.isEmpty(), "New folder's name is empty");
    GT_CHECK(!newFolderName.contains(U2ObjectDbi::PATH_SEP), "New folder's name contains invalid characters");

    QModelIndex parentFolderIndex = getItemIndex(os, databaseDoc, parentFolderPath);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__CREATE_FOLDER));
    GTUtilsDialog::waitForDialog(os, new AddFolderDialogFiller(os, newFolderName, GTGlobals::UseMouse));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, parentFolderIndex));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createPath"
void GTUtilsSharedDatabaseDocument::createPath(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &path) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(path.startsWith(U2ObjectDbi::ROOT_FOLDER), "Path is not in the canonical form");

    QString parentFolder = U2ObjectDbi::ROOT_FOLDER;
    const QStringList folders = path.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
    foreach (const QString& folder, folders) {
        bool alreadyExist = getItemIndex(os, databaseDoc, parentFolder + U2ObjectDbi::PATH_SEP + folder, false).isValid();
        if (!alreadyExist) {
            createFolder(os, databaseDoc, parentFolder, folder);
        }
        parentFolder += U2ObjectDbi::PATH_SEP + folder;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemPath"
QString GTUtilsSharedDatabaseDocument::getItemPath(HI::GUITestOpStatus &os, const QModelIndex &itemIndex) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(itemIndex.isValid(), "Item index is invalid", QString());

    ProjectViewModel::Type itemType = ProjectViewModel::itemType(itemIndex);
    switch (itemType) {
    case ProjectViewModel::DOCUMENT:
        return U2ObjectDbi::ROOT_FOLDER;

    case ProjectViewModel::FOLDER: {
        Folder* folder = ProjectViewModel::toFolder(itemIndex);
        GT_CHECK_RESULT(NULL != folder, "Can't convert item to folder", QString());
        return folder->getFolderPath();
    }

    case ProjectViewModel::OBJECT: {
        QString folderPath;
        const QModelIndex parentItemIndex = itemIndex.parent();
        GT_CHECK_RESULT(parentItemIndex.isValid(), "Parent item index of the object item is invalid", QString());

        ProjectViewModel::Type parentItemType = ProjectViewModel::itemType(parentItemIndex);
        if (ProjectViewModel::DOCUMENT == parentItemType) {
            folderPath = U2ObjectDbi::ROOT_FOLDER;
        } else if (ProjectViewModel::FOLDER == parentItemType) {
            Folder* folder = ProjectViewModel::toFolder(parentItemIndex);
            GT_CHECK_RESULT(NULL != folder, "Can't convert parent item to folder", QString());
            folderPath = folder->getFolderPath();
        } else {
            GT_CHECK_RESULT(false, "Can't recognize the parent item", QString());
        }

        GObject* object = ProjectViewModel::toObject(itemIndex);
        GT_CHECK_RESULT(NULL != object, "Can't convert item to object", QString());
        return folderPath + U2ObjectDbi::PATH_SEP + object->getGObjectName();
    }

    default:
        GT_CHECK_RESULT(false, "Can't recognize the item", QString());
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expantToItem"
void GTUtilsSharedDatabaseDocument::expantToItem(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!itemPath.isEmpty(), "Item path is empty");

    QStringList folders = itemPath.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
    GTGlobals::FindOptions findOptions;
    findOptions.depth = 1;
    const QModelIndex databaseDocIndex = GTUtilsProjectTreeView::findIndex(os, databaseDoc->getName(), findOptions);

    QTreeView* projectTreeView = GTUtilsProjectTreeView::getTreeView(os);
    GT_CHECK(NULL != projectTreeView, "Project tree view not found");
    projectTreeView->expand(databaseDocIndex);

    CHECK(!folders.isEmpty(), );
    folders.pop_back();

    QModelIndex prevFolderIndex = databaseDocIndex;
    foreach (const QString& folder, folders) {
        const QModelIndex folderIndex = GTUtilsProjectTreeView::findIndex(os, folder, prevFolderIndex, findOptions);
        GTUtilsProjectTreeView::doubleClickItem(os, folderIndex);
        prevFolderIndex = folderIndex;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expantToItem"
void GTUtilsSharedDatabaseDocument::expantToItem(HI::GUITestOpStatus &os, Document *databaseDoc, const QModelIndex &itemIndex) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");

    const QString itemPath = getItemPath(os, itemIndex);
    expantToItem(os, databaseDoc, itemPath);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickItem"
void GTUtilsSharedDatabaseDocument::doubleClickItem(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!itemPath.isEmpty(), "Item path is empty");

    expantToItem(os, databaseDoc, itemPath);
    const QModelIndex itemIndex = getItemIndex(os, databaseDoc, itemPath);
    GTUtilsProjectTreeView::doubleClickItem(os, itemIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickItem"
void GTUtilsSharedDatabaseDocument::doubleClickItem(HI::GUITestOpStatus &os, Document *databaseDoc, const QModelIndex &itemIndex) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");

    expantToItem(os, databaseDoc, itemIndex);
    GTUtilsProjectTreeView::doubleClickItem(os, itemIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openView"
void GTUtilsSharedDatabaseDocument::openView(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!itemPath.isEmpty(), "Item path is empty");

    doubleClickItem(os, databaseDoc, itemPath);
    GTGlobals::sleep(100);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openView"
void GTUtilsSharedDatabaseDocument::openView(HI::GUITestOpStatus &os, Document *databaseDoc, const QModelIndex &itemIndex) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");

    doubleClickItem(os, databaseDoc, itemIndex);
    GTGlobals::sleep(100);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callImportDialog"
void GTUtilsSharedDatabaseDocument::callImportDialog(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!itemPath.isEmpty(), "Item path is empty");

    const QModelIndex itemIndex = getItemIndex(os, databaseDoc, itemPath);
    callImportDialog(os, databaseDoc, itemIndex);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callImportDialog"
void GTUtilsSharedDatabaseDocument::callImportDialog(HI::GUITestOpStatus &os, Document *databaseDoc, const QModelIndex &itemIndex) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(itemIndex.isValid(), "Item index is invalid");

    expantToItem(os, databaseDoc, itemIndex);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__IMPORT_TO_DATABASE, GTGlobals::UseMouse));

    GTUtilsProjectTreeView::getTreeView(os)->scrollTo(itemIndex);
    const QPoint itemCenter = GTUtilsProjectTreeView::getItemCenter(os, itemIndex);
    GTMouseDriver::moveTo(itemCenter);
    GTMouseDriver::click(Qt::RightButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "ensureItemExists"
void GTUtilsSharedDatabaseDocument::ensureItemExists(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &itemPath) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");

    const QModelIndex itemIndex = getItemIndex(os, databaseDoc, itemPath);
    GT_CHECK(itemIndex.isValid(), QString("Item is invalid, item's path: '%1'").arg(itemPath));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "ensureItemsExist"
void GTUtilsSharedDatabaseDocument::ensureItemsExist(HI::GUITestOpStatus &os, Document *databaseDoc, const QStringList &itemsPaths) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");

    foreach (const QString& itemPath, itemsPaths) {
        ensureItemExists(os, databaseDoc, itemPath);
        CHECK_OP(os, );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "ensureThereAraNoItemsExceptListed"
void GTUtilsSharedDatabaseDocument::ensureThereAreNoItemsExceptListed(HI::GUITestOpStatus &os, Document *databaseDoc, const QString& parentPath, const QStringList &itemsPaths) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");

    ensureItemsExist(os, databaseDoc, itemsPaths);
    CHECK_OP(os, );

    const QModelIndex parentIndex = getItemIndex(os, databaseDoc, parentPath);
    const QModelIndexList subIndecies = GTUtilsProjectTreeView::findIndecies(os, "", parentIndex);
    GT_CHECK(subIndecies.size() == itemsPaths.size(), QString("Parent item contains %1 subitems, expected % subitems").arg(subIndecies.size()).arg(itemsPaths.size()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "importFiles"
void GTUtilsSharedDatabaseDocument::importFiles(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &dstFolderPath, const QStringList &filesPaths, const QVariantMap& options) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!filesPaths.isEmpty(), "Files paths are not provided");

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFileAction;
    addFileAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, filesPaths);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFileAction);

    if (!options.isEmpty()) {
        actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, options);
    }

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));

    createPath(os, databaseDoc, dstFolderPath);
    GTGlobals::sleep();
    callImportDialog(os, databaseDoc, dstFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(15000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "importDirs"
void GTUtilsSharedDatabaseDocument::importDirs(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &dstFolderPath, const QStringList &dirsPaths, const QVariantMap& options) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!dirsPaths.isEmpty(), "Dirs paths are not provided");

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addDirAction;
    addDirAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, dirsPaths);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_DIRS, addDirAction);

    if (!options.isEmpty()) {
        actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, options);
    }

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));

    createPath(os, databaseDoc, dstFolderPath);
    callImportDialog(os, databaseDoc, dstFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(15000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "importProjectItems"
void GTUtilsSharedDatabaseDocument::importProjectItems(HI::GUITestOpStatus &os, Document *databaseDoc, const QString &dstFolderPath, const QMap<QString, QStringList> &projectItems, const QVariantMap &options) {
    Q_UNUSED(os);
    GT_CHECK(NULL != databaseDoc, "databaseDoc is NULL");
    GT_CHECK(!projectItems.isEmpty(), "Project items are not provided");

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addProjectItemAction;
    addProjectItemAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PROJECT_ITEMS_LIST, convertProjectItemsPaths(projectItems));
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_PROJECT_ITEMS, addProjectItemAction);

    if (!options.isEmpty()) {
        actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, options);
    }

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));

    createPath(os, databaseDoc, dstFolderPath);
    callImportDialog(os, databaseDoc, dstFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(15000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "convertProjectItemsPaths"
const QVariant GTUtilsSharedDatabaseDocument::convertProjectItemsPaths(const QMap<QString, QStringList> &projectItems) {
    QMap<QString, QVariant> result;
    foreach (const QString& documentName, projectItems.keys()) {
        result.insert(documentName, projectItems[documentName]);
    }
    return result;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
