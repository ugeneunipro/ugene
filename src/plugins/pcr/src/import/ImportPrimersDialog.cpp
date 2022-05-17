/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "ImportPrimersDialog.h"

#include <U2Core/AppContext.h>
#include <U2Core/FileFilters.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/U2FileDialog.h>

#include "ImportPrimerFromObjectTask.h"
#include "ImportPrimersFromFileTask.h"
#include "ImportPrimersFromFolderTask.h"
#include "ImportPrimersMultiTask.h"

namespace U2 {

ImportPrimersDialog::ImportPrimersDialog(QWidget* parent)
    : QDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930783");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    connectSignals();
    sl_updateState();
}

void ImportPrimersDialog::sl_updateState() {
    sl_selectionChanged();
    sl_contentChanged();
}

void ImportPrimersDialog::sl_addFileClicked() {
    LastUsedDirHelper dirHelper("ImportPrimersDialog");
    const QString filter = FileFilters::createFileFilterByObjectTypes({GObjectTypes::SEQUENCE});

    QStringList fileList = U2FileDialog::getOpenFileNames(this,
                                                          tr("Select primers to import"),
                                                          dirHelper.dir,
                                                          filter,
                                                          nullptr,
                                                          QFileDialog::DontConfirmOverwrite | QFileDialog::ReadOnly);
    CHECK(!fileList.isEmpty(), );
    dirHelper.url = QFileInfo(fileList.last()).absoluteFilePath();

    for (const QString& filePath : qAsConst(fileList)) {
        auto item = new QListWidgetItem(QIcon(":/core/images/document.png"), filePath);
        item2file.insert(item, filePath);
        lwFiles->addItem(item);
    }
}

void ImportPrimersDialog::sl_removeFileClicked() {
    foreach (QListWidgetItem* item, lwFiles->selectedItems()) {
        item2file.remove(item);
        delete item;
    }
}

void ImportPrimersDialog::sl_addObjectClicked() {
    ProjectTreeControllerModeSettings settings = prepareProjectItemsSelectionSettings();
    QList<Folder> folders;
    QList<GObject*> objects;
    ProjectTreeItemSelectorDialog::selectObjectsAndFolders(settings, this, folders, objects);

    foreach (const Folder& folder, folders) {
        QListWidgetItem* item = new QListWidgetItem(QIcon(":U2Designer/images/directory.png"), folder.getFolderPath());
        item2folder.insert(item, folder);
        lwObjects->addItem(item);
    }

    foreach (GObject* object, objects) {
        QListWidgetItem* item = new QListWidgetItem(GObjectTypes::getTypeInfo(object->getGObjectType()).icon, object->getDocument()->getName() + ": " + object->getGObjectName());
        item2object.insert(item, object);
        lwObjects->addItem(item);
    }
}

void ImportPrimersDialog::sl_removeObjectClicked() {
    foreach (QListWidgetItem* item, lwObjects->selectedItems()) {
        item2folder.remove(item);
        item2object.remove(item);
        delete item;
    }
}

void ImportPrimersDialog::sl_selectionChanged() {
    pbRemoveFile->setEnabled(!lwFiles->selectedItems().isEmpty());
}

void ImportPrimersDialog::sl_contentChanged() {
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(lwFiles->count() > 0);
}

void ImportPrimersDialog::accept() {
    QList<Task*> tasks;
    foreach (const QString& filePath, item2file) {
        tasks << new ImportPrimersFromFileTask(filePath);
    }
    if (!tasks.isEmpty()) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ImportPrimersMultiTask(tasks));
    }

    QDialog::accept();
}

void ImportPrimersDialog::connectSignals() {
    connect(pbAddFile, SIGNAL(clicked()), SLOT(sl_addFileClicked()));
    connect(pbRemoveFile, SIGNAL(clicked()), SLOT(sl_removeFileClicked()));
    connect(pbAddObject, SIGNAL(clicked()), SLOT(sl_addObjectClicked()));
    connect(pbRemoveObject, SIGNAL(clicked()), SLOT(sl_removeObjectClicked()));
    connect(lwFiles, SIGNAL(itemSelectionChanged()), SLOT(sl_selectionChanged()));
    connect(lwObjects, SIGNAL(itemSelectionChanged()), SLOT(sl_selectionChanged()));
    connect(lwFiles->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(sl_contentChanged()));
    connect(lwFiles->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(sl_contentChanged()));
    connect(lwObjects->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), SLOT(sl_contentChanged()));
    connect(lwObjects->model(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)), SLOT(sl_contentChanged()));
}

ProjectTreeControllerModeSettings ImportPrimersDialog::prepareProjectItemsSelectionSettings() const {
    ProjectTreeControllerModeSettings settings;
    settings.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    return settings;
}

}  // namespace U2
