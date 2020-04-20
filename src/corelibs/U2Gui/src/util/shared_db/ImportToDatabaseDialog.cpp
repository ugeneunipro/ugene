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

#include <QListView>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/CloneObjectTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ImportDirToDatabaseTask.h>
#include <U2Core/ImportDocumentToDatabaseTask.h>
#include <U2Core/ImportFileToDatabaseTask.h>
#include <U2Core/ImportObjectToDatabaseTask.h>
#include <U2Core/ImportToDatabaseTask.h>
#include <U2Core/Task.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/U2FileDialog.h>

#include "CommonImportOptionsDialog.h"
#include "ImportToDatabaseDialog.h"
#include "ItemToImportEditDialog.h"
#include "ui_ImportToDatabaseDialog.h"

namespace U2 {


ImportToDatabaseDialog::ImportToDatabaseDialog(Document *dbConnection, const QString &defaultFolder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_ImportToDatabaseDialog),
    dbConnection(dbConnection),
    baseFolder(U2DbiUtils::makeFolderCanonical(defaultFolder))
{
    ui->setupUi(this);
    DIR_HELPER_NAME = "import_to_database";
    FILES_AND_FOLDERS = tr("Files and folders");
    OBJECTS_AND_DOCUMENTS = tr("Documents and objects");
    
    new HelpButton(this, ui->buttonBox, "24749086");
    init();
    connectSignals();
    updateState();
}

ImportToDatabaseDialog::~ImportToDatabaseDialog() {
    delete ui;
}

void ImportToDatabaseDialog::sl_selectionChanged() {
    updateState();
}

void ImportToDatabaseDialog::sl_itemDoubleClicked(QTreeWidgetItem* item, int column) {
    CHECK(COLUMN_FOLDER == column, );
    ui->twOrders->editItem(item, column);
}

void ImportToDatabaseDialog::sl_itemChanged(QTreeWidgetItem *item, int column) {
    CHECK(COLUMN_FOLDER == column, );
    const QString dstFolder = item->text(column);
    item->setText(column, U2DbiUtils::makeFolderCanonical(dstFolder));
}

void ImportToDatabaseDialog::sl_customContextMenuRequested(const QPoint &position) {
    QTreeWidgetItem* item = ui->twOrders->currentItem();
    CHECK(NULL != item, );
    CHECK(isEssential(item), );

    ui->twOrders->setCurrentItem(item);

    QMenu m;
    m.setObjectName("popMenu");
    m.addAction(tr("Override options"), this, SLOT(sl_editOptions()));

    if (privateOptions.contains(item)) {
        m.addSeparator();
        m.addAction(tr("Reset to general options"), this, SLOT(sl_resetOptions()));
    }

    m.exec(ui->twOrders->mapToGlobal(position));
}

void ImportToDatabaseDialog::sl_resetOptions() {
    QTreeWidgetItem* item = ui->twOrders->currentItem();
    CHECK(NULL != item, );

    const ImportToDatabaseOptions oldOptions = privateOptions.take(item);
    updateItemsState(oldOptions, commonOptions);
    markItem(item, false);
}

void ImportToDatabaseDialog::sl_addFileClicked() {
    const QStringList fileList = getFilesToImport();
    foreach (const QString& url, fileList) {
        addFile(url);
    }

    updateState();
}

void ImportToDatabaseDialog::sl_addFolderClicked() {
    const QString url = getFolderToImport();
    addFolder(url);

    updateState();
}

void ImportToDatabaseDialog::sl_addObjectClicked() {
    QList<Document*> docsToImport;
    QList<GObject*> objsToImport;
    getProjectItemsToImport(docsToImport, objsToImport);

    addObjectsAndDocuments(docsToImport, objsToImport);

    updateState();
}

void ImportToDatabaseDialog::sl_optionsClicked() {
    QObjectScopedPointer<CommonImportOptionsDialog> optionsDialog = new CommonImportOptionsDialog(baseFolder, commonOptions, this);
    const int dialogResult = optionsDialog->exec();
    CHECK(!optionsDialog.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        const ImportToDatabaseOptions oldOptions = commonOptions;
        commonOptions = optionsDialog->getOptions();
        baseFolder = optionsDialog->getBaseFolder();
        updateItemsState(oldOptions, commonOptions);
    }
}

void ImportToDatabaseDialog::sl_editOptions() {
    QTreeWidgetItem* item = ui->twOrders->currentItem();
    CHECK(NULL != item, );

    ImportToDatabaseOptions currentOptions = privateOptions.value(item, commonOptions);
    QObjectScopedPointer<ItemToImportEditDialog> editDialog = new ItemToImportEditDialog(item->text(COLUMN_ITEM_TEXT), item->text(COLUMN_FOLDER), currentOptions, this);
    const int dialogResult = editDialog->exec();
    CHECK(!editDialog.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        ImportToDatabaseOptions newOptions = editDialog->getOptions();
        privateOptions.insert(item, newOptions);

        item->setText(COLUMN_FOLDER, editDialog->getFolder());
        updateItemState(item, currentOptions, newOptions);
        markItem(item, true);
    }
}

void ImportToDatabaseDialog::sl_removeClicked() {
    QList<QTreeWidgetItem*> selectedItems = ui->twOrders->selectedItems();
    removeItems(selectedItems);

    updateState();
}

void ImportToDatabaseDialog::sl_taskFinished() {
    Task* task = qobject_cast<Task*>(sender());
    CHECK(NULL != task, );
    CHECK(task->isFinished(), );
}

void ImportToDatabaseDialog::accept() {
    QList<Task*> importTasks;
    importTasks << createImportFilesTasks();
    importTasks << createImportFoldersTasks();
    importTasks << createimportObjectsTasks();
    importTasks << createImportDocumentsTasks();

    if (!importTasks.isEmpty()) {
        ImportToDatabaseTask* importTask = new ImportToDatabaseTask(importTasks, 1);
        AppContext::getTaskScheduler()->registerTopLevelTask(importTask);
    }

    QDialog::accept();
}

void ImportToDatabaseDialog::init() {
    ui->twOrders->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setObjectName("cancel_button");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setObjectName("import_button");
}

void ImportToDatabaseDialog::connectSignals() {
    connect(ui->twOrders,       SIGNAL(itemSelectionChanged()),                     SLOT(sl_selectionChanged()));
    connect(ui->twOrders,       SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),   SLOT(sl_itemDoubleClicked(QTreeWidgetItem*, int)));
    connect(ui->twOrders,       SIGNAL(itemChanged(QTreeWidgetItem*, int)),         SLOT(sl_itemChanged(QTreeWidgetItem*, int)));
    connect(ui->twOrders,       SIGNAL(customContextMenuRequested(QPoint)),         SLOT(sl_customContextMenuRequested(QPoint)));

    connect(ui->pbAddFiles,     SIGNAL(clicked()),  SLOT(sl_addFileClicked()));
    connect(ui->pbAddFolder,    SIGNAL(clicked()),  SLOT(sl_addFolderClicked()));
    connect(ui->pbAddObjects,   SIGNAL(clicked()),  SLOT(sl_addObjectClicked()));
    connect(ui->pbOptions,      SIGNAL(clicked()),  SLOT(sl_optionsClicked()));
    connect(ui->pbRemove,       SIGNAL(clicked()),  SLOT(sl_removeClicked()));
}

void ImportToDatabaseDialog::updateState() {
    const QItemSelection selectedIndexes = ui->twOrders->selectionModel()->selection();
    const bool isSomethingSelected = !selectedIndexes.isEmpty();
    const bool isOrdersFilled = ui->twOrders->topLevelItemCount() > 0;

    ui->pbRemove->setEnabled(isSomethingSelected);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isOrdersFilled);
}

void ImportToDatabaseDialog::updateItemsState(const ImportToDatabaseOptions &oldOptions, const ImportToDatabaseOptions &newOptions) {
    foreach (QTreeWidgetItem* item, folders) {
        if (privateOptions.contains(item)) {
            continue;
        }
        updateItemState(item, oldOptions, newOptions);
    }
}

void ImportToDatabaseDialog::updateItemState(QTreeWidgetItem *item, const ImportToDatabaseOptions &oldOptions, const ImportToDatabaseOptions &newOptions) {
    if (folders.contains(item)) {
        if (oldOptions.createSubfolderForTopLevelFolder && !newOptions.createSubfolderForTopLevelFolder) {
            QString dstFolder = item->text(COLUMN_FOLDER);
            const QString srcFolderName = QFileInfo(item->text(COLUMN_ITEM_TEXT)).fileName();
            if (dstFolder.endsWith(srcFolderName)) {
                dstFolder.chop(srcFolderName.size());
            }
            item->setText(COLUMN_FOLDER, U2DbiUtils::makeFolderCanonical(dstFolder));
        } else if (!oldOptions.createSubfolderForTopLevelFolder && newOptions.createSubfolderForTopLevelFolder) {
            const QString dstFolder = item->text(COLUMN_FOLDER);
            const QString srcFolderName = QFileInfo(item->text(COLUMN_ITEM_TEXT)).fileName();
            item->setText(COLUMN_FOLDER, U2DbiUtils::makeFolderCanonical(dstFolder) + U2ObjectDbi::PATH_SEP + srcFolderName);
        }
    }
}

void ImportToDatabaseDialog::markItem(QTreeWidgetItem *item, bool mark) {
    CHECK(NULL != item, );

    QFont fontItemText = item->font(COLUMN_ITEM_TEXT);
    fontItemText.setBold(mark);
    item->setFont(COLUMN_ITEM_TEXT, fontItemText);

    QFont fontFolder = item->font(COLUMN_FOLDER);
    fontFolder.setBold(mark);
    item->setFont(COLUMN_FOLDER, fontFolder);

    setTooltip(item);
}

bool ImportToDatabaseDialog::isEssential(QTreeWidgetItem *item) const {
    return files.contains(item) ||
            folders.contains(item) ||
            treeItem2Document.contains(item) ||
            treeItem2Object.contains(item);
}

QStringList ImportToDatabaseDialog::getFilesToImport() {
    LastUsedDirHelper dirHelper(DIR_HELPER_NAME);

    QFileDialog::Options additionalOptions;
    Q_UNUSED(additionalOptions);
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        additionalOptions = QFileDialog::DontUseNativeDialog;
    }

    const QStringList fileList = U2FileDialog::getOpenFileNames(this,
                                                               tr("Select files to import"),
                                                               dirHelper.dir,
                                                               "",
                                                               NULL,
                                                               QFileDialog::DontConfirmOverwrite | QFileDialog::ReadOnly | additionalOptions);
    CHECK(!fileList.isEmpty(), fileList);
    dirHelper.url = QFileInfo(fileList.last()).absoluteFilePath();

    return fileList;
}

QString ImportToDatabaseDialog::getFolderToImport() {
    LastUsedDirHelper dirHelper(DIR_HELPER_NAME);

    QFileDialog::Options additionalOptions;
    Q_UNUSED(additionalOptions);
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        additionalOptions = QFileDialog::DontUseNativeDialog;
    }
#endif

    const QString dir = U2FileDialog::getExistingDirectory(this,
                                                          tr("Select a folder to import"),
                                                          dirHelper.dir,
                                                          QFileDialog::ShowDirsOnly | additionalOptions);
    CHECK(!dir.isEmpty(), dir);
    dirHelper.url = QFileInfo(dir).absoluteFilePath() + "/";

    return dir;
}

void ImportToDatabaseDialog::getProjectItemsToImport(QList<Document*>& docList, QList<GObject*>& objList) {
    ProjectTreeControllerModeSettings settings;

    // do not show objects from the current database
    QList<GObject*> docObjects = dbConnection->getObjects();
    foreach (GObject* obj, docObjects) {
        settings.excludeObjectList.append(obj);
    }

    settings.objectTypesToShow += dbConnection->getDocumentFormat()->getSupportedObjectTypes();

    ProjectTreeItemSelectorDialog::selectObjectsAndDocuments(settings, this, docList, objList);
}

void ImportToDatabaseDialog::addFolder(const QString& url) {
    CHECK(!url.isEmpty(), );

    const QString dstFolder = commonOptions.createSubfolderForTopLevelFolder
            ? U2DbiUtils::makeFolderCanonical(baseFolder + U2ObjectDbi::PATH_SEP + QFileInfo(url).fileName())
            : baseFolder;

    QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << url << dstFolder);
    newItem->setIcon(COLUMN_ITEM_TEXT, QIcon(":U2Designer/images/directory.png"));
    newItem->setFlags(Qt::ItemIsEditable | newItem->flags());
    setFolderTooltip(newItem);
    folders << newItem;

    QTreeWidgetItem* headerItem = getHeaderItem(FILE_AND_FOLDER);
    headerItem->addChild(newItem);
    headerItem->setExpanded(true);
}

void ImportToDatabaseDialog::addFile(const QString& url) {
    CHECK(!url.isEmpty(), );

    QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << url << baseFolder);
    newItem->setIcon(COLUMN_ITEM_TEXT, QIcon(":/core/images/document.png"));
    newItem->setFlags(Qt::ItemIsEditable | newItem->flags());
    setFileTooltip(newItem);
    files << newItem;

    QTreeWidgetItem* headerItem = getHeaderItem(FILE_AND_FOLDER);
    headerItem->addChild(newItem);
    headerItem->setExpanded(true);
}

void ImportToDatabaseDialog::addObjectsAndDocuments(const QList<Document*>& docsToImport, const QList<GObject*>& objsToImport) {
    foreach (Document* doc, docsToImport) {
        addDocument(doc);
    }

    foreach (GObject* object, objsToImport) {
        addObject(object, NULL);
    }
}

void ImportToDatabaseDialog::addDocument(Document* document) {
    CHECK(NULL != document, );

    QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << document->getName() << baseFolder);
    newItem->setIcon(COLUMN_ITEM_TEXT, QIcon(":/core/images/document.png"));
    newItem->setFlags(Qt::ItemIsEditable | newItem->flags());
    setDocumentTooltip(newItem);
    treeItem2Document.insert(newItem, document);

    QTreeWidgetItem* headerItem = getHeaderItem(OBJECT_AND_DOCUMENT);
    headerItem->addChild(newItem);
    headerItem->setExpanded(true);

    addSubObjects(document, newItem);
}

void ImportToDatabaseDialog::addObject(GObject *object, QTreeWidgetItem* parent) {
    CHECK(NULL != object, );
    CHECK(NULL != object->getDocument(), );

    const QString objectText = "[" + GObjectTypes::getTypeInfo(object->getGObjectType()).treeSign + "] " + object->getGObjectName();

    QString dstFolder = baseFolder;
    if (NULL != parent &&
        NULL != treeItem2Document.value(parent, NULL) &&
        commonOptions.createSubfolderForEachDocument) {
        dstFolder = baseFolder + U2ObjectDbi::PATH_SEP + treeItem2Document[parent]->getName();
        dstFolder = U2DbiUtils::makeFolderCanonical(dstFolder);
    }

    QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << objectText << dstFolder);

    if (NULL == parent) {
        treeItem2Object[newItem] = object;
        setObjectTooltip(newItem);
        parent = getHeaderItem(OBJECT_AND_DOCUMENT);
        treeItem2Object.insert(newItem, object);
    }

    newItem->setIcon(COLUMN_ITEM_TEXT, GObjectTypes::getTypeInfo(object->getGObjectType()).icon);
    newItem->setFlags(Qt::ItemIsEditable | newItem->flags());

    parent->addChild(newItem);
    parent->setExpanded(true);
}

void ImportToDatabaseDialog::addSubObjects(Document* document, QTreeWidgetItem* docItem) {
    foreach (GObject* object, document->getObjects()) {
        addObject(object, docItem);
    }
}

void ImportToDatabaseDialog::removeItems(QList<QTreeWidgetItem *> itemList) {
    CHECK(!itemList.isEmpty(), );

    QSet<QTreeWidgetItem*> parents;
    QSet<QTreeWidgetItem*> removedItems;
    foreach (QTreeWidgetItem* item, itemList) {
        if (NULL != item->parent()) {
            parents << item->parent();
        }

        if (!removedItems.contains(item)) {
            removedItems.unite(removeRecursively(item).toSet());
        }
    }

    QList<QTreeWidgetItem*> emptyParents;
    foreach (QTreeWidgetItem* parent, parents) {
        if (!removedItems.contains(parent) && 0 == parent->childCount()) {
            emptyParents << parent;
        }
    }

    removeItems(emptyParents);
}

QList<QTreeWidgetItem *> ImportToDatabaseDialog::removeRecursively(QTreeWidgetItem *item) {
    QList<QTreeWidgetItem*> removedItems;
    CHECK(NULL != item, removedItems);

    for (int i = 0; i < item->childCount(); i++) {
        removedItems << removeRecursively(item->child(i));
    }

    removedItems << item;

    files.removeAll(item);
    folders.removeAll(item);
    treeItem2Document.remove(item);
    treeItem2Object.remove(item);

    delete item;

    return removedItems;
}

QList<Task*> ImportToDatabaseDialog::createImportFilesTasks() const {
    QList<Task*> tasks;
    foreach (QTreeWidgetItem* fileTreeItem, files) {
        tasks << new ImportFileToDatabaseTask(fileTreeItem->text(COLUMN_ITEM_TEXT),
                                              dbConnection->getDbiRef(),
                                              fileTreeItem->text(COLUMN_FOLDER),
                                              privateOptions.value(fileTreeItem, commonOptions));
    }
    return tasks;
}

QList<Task *> ImportToDatabaseDialog::createImportFoldersTasks() const {
    QList<Task*> tasks;
    foreach (QTreeWidgetItem* folderTreeItem, folders) {
        tasks << new ImportDirToDatabaseTask(folderTreeItem->text(COLUMN_ITEM_TEXT),
                                             dbConnection->getDbiRef(),
                                             folderTreeItem->text(COLUMN_FOLDER),
                                             privateOptions.value(folderTreeItem, commonOptions));
    }
    return tasks;
}

QList<Task *> ImportToDatabaseDialog::createimportObjectsTasks() const {
    QList<Task*> tasks;
    foreach (QTreeWidgetItem* objectTreeItem, treeItem2Object.keys()) {
        tasks << new ImportObjectToDatabaseTask(treeItem2Object[objectTreeItem],
                                                dbConnection->getDbiRef(),
                                                objectTreeItem->text(COLUMN_FOLDER));
    }
    return tasks;
}

QList<Task *> ImportToDatabaseDialog::createImportDocumentsTasks() const {
    QList<Task*> tasks;
    foreach (QTreeWidgetItem* documentTreeItem, treeItem2Document.keys()) {
        tasks << new ImportDocumentToDatabaseTask(treeItem2Document[documentTreeItem],
                                                  dbConnection->getDbiRef(),
                                                  documentTreeItem->text(COLUMN_FOLDER),
                                                  privateOptions.value(documentTreeItem, commonOptions));
    }
    return tasks;
}

QTreeWidgetItem* ImportToDatabaseDialog::getHeaderItem(HeaderType headerType) const {
    const QString itemText = (FILE_AND_FOLDER == headerType ? FILES_AND_FOLDERS : OBJECTS_AND_DOCUMENTS);
    const QList<QTreeWidgetItem*> itemsList = ui->twOrders->findItems(itemText, Qt::MatchExactly);

    if (itemsList.isEmpty()) {
        QTreeWidgetItem* headerItem = new QTreeWidgetItem(QStringList() << itemText);
        ui->twOrders->addTopLevelItem(headerItem);
        return headerItem;
    }

    return itemsList.first();
}

void ImportToDatabaseDialog::setTooltip(QTreeWidgetItem *item) {
    if (files.contains(item)) {
        setFileTooltip(item);
    } else if (folders.contains(item)) {
        setFolderTooltip(item);
    } if (treeItem2Document.contains(item)) {
        setDocumentTooltip(item);
    } if (treeItem2Object.contains(item)) {
        setObjectTooltip(item);
    }
}

void ImportToDatabaseDialog::setFileTooltip(QTreeWidgetItem *item) {
    ImportToDatabaseOptions currentOptions = privateOptions.value(item, commonOptions);
    QString tooltip;

    if (privateOptions.contains(item)) {
        tooltip += tr("This file will be imported with its own options.\n\n");
    }

    tooltip += tr("File:\n") +
               item->text(COLUMN_ITEM_TEXT) +
               "\n\n" +
               tr("Import to: ") +
               item->text(COLUMN_FOLDER);

    if (currentOptions.createSubfolderForEachFile) {
        tooltip += "\n" + tr("A folder for file objects will be created");
    }

    if (currentOptions.importUnknownAsUdr) {
        tooltip += "\n" + tr("If file is not recognized, it will be imported as binary data");
    }

    switch (currentOptions.multiSequencePolicy) {
        case ImportToDatabaseOptions::SEPARATE:
            tooltip += "\n" + tr("If file contains more than one sequence, they will imported as separate objects");
            break;
        case ImportToDatabaseOptions::MERGE:
            tooltip += "\n" + tr("If file contains more than one sequence, they will imported as single sequence with several 'Unknown' bases as separator") +
                       "\n" + tr("Size of separator: %1").arg(currentOptions.mergeMultiSequencePolicySeparatorSize);
            break;
        case ImportToDatabaseOptions::MALIGNMENT:
            tooltip += "\n" + tr("If file contains more than one sequence, they will be joined into the multiple alignment");
            break;
    }

    item->setToolTip(COLUMN_ITEM_TEXT, tooltip);
    item->setToolTip(COLUMN_FOLDER, tooltip);
}

void ImportToDatabaseDialog::setFolderTooltip(QTreeWidgetItem *item) {
    ImportToDatabaseOptions currentOptions = privateOptions.value(item, commonOptions);
    QString tooltip;

    if (privateOptions.contains(item)) {
        tooltip += tr("This folder will be imported with its own options.\n\n");
    }

    tooltip += tr("Folder:\n") +
              item->text(COLUMN_ITEM_TEXT) +
              "\n\n" +
              tr("Import to: ") +
              item->text(COLUMN_FOLDER) +
              "\n";

    if (currentOptions.processFoldersRecursively) {
        tooltip += "\n" + tr("The folder will be processed recursively");
    }

    if (currentOptions.createSubfolderForEachFile) {
        tooltip += "\n" + tr("A folder for each file will be created");
    }

    if (currentOptions.importUnknownAsUdr) {
        tooltip += "\n" + tr("If file is not recognized, it will be imported as binary data");
    }

    switch (currentOptions.multiSequencePolicy) {
        case ImportToDatabaseOptions::SEPARATE:
            tooltip += "\n" + tr("If file contains more than one sequence, they will imported as separate objects");
            break;
        case ImportToDatabaseOptions::MERGE:
            tooltip += "\n" + tr("If file contains more than one sequence, they will imported as single sequence with several 'Unknown' bases as separator") +
                       "\n" + tr("Size of separator: %1").arg(currentOptions.mergeMultiSequencePolicySeparatorSize);
            break;
        case ImportToDatabaseOptions::MALIGNMENT:
            tooltip += "\n" + tr("If file contains more than one sequence, they will be joined into the multiple alignment");
            break;
    }

    item->setToolTip(COLUMN_ITEM_TEXT, tooltip);
    item->setToolTip(COLUMN_FOLDER, tooltip);
}

void ImportToDatabaseDialog::setObjectTooltip(QTreeWidgetItem *item) {
    const QString typeName = GObjectTypes::getTypeInfo(treeItem2Object[item]->getGObjectType()).name;
    QString tooltip;

    if (privateOptions.contains(item)) {
        tooltip += tr("This folder will be imported with its own options.\n\n");
    }

    tooltip = tr("The ") + typeName + tr(" from document ") +
              treeItem2Object[item]->getDocument()->getName() +
              ":\n" +
              item->text(COLUMN_ITEM_TEXT) +
              "\n\n" +
              tr("Import to: ") +
              item->text(COLUMN_FOLDER);

    item->setToolTip(COLUMN_ITEM_TEXT, tooltip);
    item->setToolTip(COLUMN_FOLDER, tooltip);
}

void ImportToDatabaseDialog::setDocumentTooltip(QTreeWidgetItem *item) {
    QString tooltip;

    if (privateOptions.contains(item)) {
        tooltip += tr("This folder will be imported with its own options.\n\n");
    }

    tooltip = tr("Document:\n") +
              item->text(COLUMN_ITEM_TEXT) +
              "\n\n" +
              tr("Import to: ") +
              item->text(COLUMN_FOLDER);

    item->setToolTip(COLUMN_ITEM_TEXT, tooltip);
    item->setToolTip(COLUMN_FOLDER, tooltip);
}

}   // namespace U2
