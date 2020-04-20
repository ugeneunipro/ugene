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

#ifndef _U2_IMPORT_TO_DATABASE_DIALOG_H_
#define _U2_IMPORT_TO_DATABASE_DIALOG_H_

#include <QDialog>
#include <QMap>

#include <U2Core/ImportToDatabaseOptions.h>

class QTreeWidgetItem;
class Ui_ImportToDatabaseDialog;

namespace U2 {

class Document;
class GObject;
class ProjectTreeController;
class Task;

class ImportToDatabaseDialog : public QDialog {
    Q_OBJECT

    enum HeaderType {
        FILE_AND_FOLDER,
        OBJECT_AND_DOCUMENT
    };

public:
    ImportToDatabaseDialog(Document* dbConnection, const QString& baseFolder, QWidget *parent = 0);
    ~ImportToDatabaseDialog();
    
private slots:
    void sl_selectionChanged();
    void sl_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void sl_itemChanged(QTreeWidgetItem* item, int column);
    void sl_customContextMenuRequested(const QPoint& position);
    void sl_resetOptions();

    void sl_addFileClicked();
    void sl_addFolderClicked();
    void sl_addObjectClicked();
    void sl_optionsClicked();
    void sl_editOptions();
    void sl_removeClicked();
    void sl_taskFinished();

    void accept();

private:
    void init();
    void connectSignals();
    void updateState();
    void updateItemsState(const ImportToDatabaseOptions& oldOptions, const ImportToDatabaseOptions& newOptions);
    void updateItemState(QTreeWidgetItem* item, const ImportToDatabaseOptions& oldOptions, const ImportToDatabaseOptions& newOptions);
    void markItem(QTreeWidgetItem* item, bool mark);
    bool isEssential(QTreeWidgetItem* item) const;

    QStringList getFilesToImport();
    QString getFolderToImport();
    void getProjectItemsToImport(QList<Document*>& docList, QList<GObject*>& objList);

    void addFolder(const QString& url);
    void addFile(const QString& url);
    void addObjectsAndDocuments(const QList<Document*>& docsToImport, const QList<GObject*>& objsToImport);
    void addDocument(Document* document);
    void addObject(GObject* object, QTreeWidgetItem* parent);

    void addSubObjects(Document* document, QTreeWidgetItem* docItem);

    void removeItems(QList<QTreeWidgetItem*> itemList);
    QList<QTreeWidgetItem*> removeRecursively(QTreeWidgetItem* item);

    QList<Task*> createImportFilesTasks() const;
    QList<Task*> createImportFoldersTasks() const;
    QList<Task*> createimportObjectsTasks() const;
    QList<Task*> createImportDocumentsTasks() const;

    QTreeWidgetItem* getHeaderItem(HeaderType headerType) const;

    void setTooltip(QTreeWidgetItem* item);
    void setFileTooltip(QTreeWidgetItem* item);
    void setFolderTooltip(QTreeWidgetItem* item);
    void setObjectTooltip(QTreeWidgetItem* item);
    void setDocumentTooltip(QTreeWidgetItem* item);

    Ui_ImportToDatabaseDialog* ui;
    Document* dbConnection;
    QString baseFolder;

    ImportToDatabaseOptions commonOptions;
    QMap<QTreeWidgetItem*, ImportToDatabaseOptions> privateOptions;

    QList<QTreeWidgetItem*> files;
    QList<QTreeWidgetItem*> folders;
    QMap<QTreeWidgetItem*, Document*> treeItem2Document;
    QMap<QTreeWidgetItem*, GObject*> treeItem2Object;

    QString DIR_HELPER_NAME;
    QString FILES_AND_FOLDERS;
    QString OBJECTS_AND_DOCUMENTS;

    static const int COLUMN_ITEM_TEXT = 0;
    static const int COLUMN_FOLDER = 1;
};

}   // namespace U2

#endif // _U2_IMPORT_TO_DATABASE_DIALOG_H_
