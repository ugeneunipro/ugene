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

#ifndef _U2_OUTPUTFILEDIALOG_H_
#define _U2_OUTPUTFILEDIALOG_H_

#include "ui_CreateDirectoryDialog.h"
#include "ui_OutputFileDialog.h"

namespace U2 {

class CompletionFiller;
class FSItem;
class RFSTreeModel;
class RunFileSystem;

class OutputFileDialog : public QDialog, public Ui_OutputFileDialog {
    Q_OBJECT
public:
    OutputFileDialog(RunFileSystem *rfs, bool saveDir, CompletionFiller *filler, QWidget *parent);
    QString getResult() const;
    bool isSaveToFileSystem() const;

private slots:
    void sl_textChanged();
    void sl_selectionChanged();
    void sl_addDir();
    void sl_saveToFS();

private:
    void updateSaveButton();
    QString selectedPath() const;
    FSItem * selectedItem() const;
    void updateFocus();
    void setupSettings();

private:
    RunFileSystem *rfs;
    bool saveDir;
    RFSTreeModel *model;
    QItemSelectionModel *selectionModel;
    bool saveToFileSystem;
};

class RFSTreeModel : public QAbstractItemModel {
public:
    RFSTreeModel(FSItem *rootItem, bool saveDir, QObject *parent);
    ~RFSTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    FSItem * toItem(const QModelIndex &index) const;
    QString getPath(FSItem *target) const;
    QModelIndex addDir(const QModelIndex &index, const QString &dirName);

private:
    FSItem *superRootItem;
    bool saveDir;
};

class CreateDirectoryDialog : public QDialog, public Ui_CreateDirectoryDialog {
    Q_OBJECT
public:
    CreateDirectoryDialog(RunFileSystem *rfs, const QString &parentDir, QWidget *parent);

    QString getResult() const;

private slots:
    void sl_textChanged();

private:
    RunFileSystem *rfs;
    QString parentDir;
};

} // U2

#endif // _U2_OUTPUTFILEDIALOG_H_
