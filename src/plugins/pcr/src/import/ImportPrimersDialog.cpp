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

#include <U2Core/AppContext.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/SharedConnectionsDialog.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/U2FileDialog.h>

#include "ImportPrimerFromObjectTask.h"
#include "ImportPrimersDialog.h"
#include "ImportPrimersFromFileTask.h"
#include "ImportPrimersFromFolderTask.h"
#include "ImportPrimersMultiTask.h"

namespace U2 {

const QString ImportPrimersDialog::LOCAL_FILES = QObject::tr("Local file(s)");
const QString ImportPrimersDialog::SHARED_DB = QObject::tr("Shared database");

ImportPrimersDialog::ImportPrimersDialog(QWidget *parent) :
    QDialog(parent),
    waitForConnection(false)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749035");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    init();
    connectSignals();
    sl_updateState();
}

void ImportPrimersDialog::sl_updateState() {
    const bool isLocalFilesMode = (LOCAL_FILES == cbSource->currentText());

    pbConnect->setVisible(!isLocalFilesMode);
    filesContainer->setVisible(isLocalFilesMode);
    objectsContainer->setVisible(!isLocalFilesMode);

    sl_selectionChanged();
    sl_contentChanged();
}

void ImportPrimersDialog::sl_connectClicked() {
    QObjectScopedPointer<SharedConnectionsDialog> connectionDialog = new SharedConnectionsDialog(this);
    connectionDialog->exec();
}

void ImportPrimersDialog::sl_addFileClicked() {
    LastUsedDirHelper dirHelper("ImportPrimersDialog");
    const QString filter = FormatUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);

    QFileDialog::Options additionalOptions;
    Q_UNUSED(additionalOptions);
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        additionalOptions = QFileDialog::DontUseNativeDialog;
    }
#endif
    const QStringList fileList = U2FileDialog::getOpenFileNames(this,
                                                                tr("Select primers to import"),
                                                                dirHelper.dir,
                                                                filter,
                                                                NULL,
                                                                QFileDialog::DontConfirmOverwrite | QFileDialog::ReadOnly | additionalOptions);
    CHECK(!fileList.isEmpty(), );
    dirHelper.url = QFileInfo(fileList.last()).absoluteFilePath();

    foreach (const QString &filePath, fileList) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/core/images/document.png"), filePath);
        item2file.insert(item, filePath);
        lwFiles->addItem(item);
    }
}

void ImportPrimersDialog::sl_removeFileClicked() {
    foreach (QListWidgetItem *item, lwFiles->selectedItems()) {
        item2file.remove(item);
        delete item;
    }
}

void ImportPrimersDialog::sl_addObjectClicked() {
    CHECK(!waitForConnection, );
    if (!ProjectUtils::areSharedDatabasesAvailable()) {
        waitForConnection = true;
        QPointer<SharedConnectionsDialog> connectDialog = new SharedConnectionsDialog(this);
        connect(connectDialog.data(), SIGNAL(si_connectionCompleted()), SLOT(sl_connectionComplete()));
        connectDialog->exec();
        return;
    }

    ProjectTreeControllerModeSettings settings = prepareProjectItemsSelectionSettings();
    QList<Folder> folders;
    QList<GObject *> objects;
    ProjectTreeItemSelectorDialog::selectObjectsAndFolders(settings, this, folders, objects);

    foreach (const Folder &folder, folders) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":U2Designer/images/directory.png"), folder.getFolderPath());
        item2folder.insert(item, folder);
        lwObjects->addItem(item);
    }

    foreach (GObject *object, objects) {
        QListWidgetItem *item = new QListWidgetItem(GObjectTypes::getTypeInfo(object->getGObjectType()).icon, object->getDocument()->getName() + ": " + object->getGObjectName());
        item2object.insert(item, object);
        lwObjects->addItem(item);
    }
}

void ImportPrimersDialog::sl_removeObjectClicked() {
    foreach (QListWidgetItem *item, lwObjects->selectedItems()) {
        item2folder.remove(item);
        item2object.remove(item);
        delete item;
    }
}

void ImportPrimersDialog::sl_connectionComplete() {
    SharedConnectionsDialog *connectionDialog = qobject_cast<SharedConnectionsDialog *>(sender());
    if (Q_LIKELY(NULL != connectionDialog)) {
        connectionDialog->deleteLater();
    } else {
        coreLog.error("ImportPrimersDialog::sl_connectionComplete(): an unexpected slot caller");
    }
    waitForConnection = false;
    sl_addObjectClicked();
}

void ImportPrimersDialog::sl_selectionChanged() {
    const bool isLocalFilesMode = (LOCAL_FILES == cbSource->currentText());
    QListWidget *listWidget = isLocalFilesMode ? lwFiles : lwObjects;
    QPushButton *removeButton = isLocalFilesMode ? pbRemoveFile : pbRemoveObject;
    removeButton->setEnabled(!listWidget->selectedItems().isEmpty());
}

void ImportPrimersDialog::sl_contentChanged() {
    const bool isLocalFilesMode = (LOCAL_FILES == cbSource->currentText());
    QListWidget *listWidget = isLocalFilesMode ? lwFiles : lwObjects;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(listWidget->count() > 0);
}

void ImportPrimersDialog::accept() {
    QList<Task *> tasks;
    if (LOCAL_FILES == cbSource->currentText()) {
        foreach (const QString &filePath, item2file) {
            tasks << new ImportPrimersFromFileTask(filePath);
        }
    } else {
        foreach (const Folder &folder, item2folder) {
            tasks << new ImportPrimersFromFolderTask(folder);
        }

        foreach (GObject *object, item2object) {
            tasks << new ImportPrimerFromObjectTask(object);
        }
    }

    if (!tasks.isEmpty()) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ImportPrimersMultiTask(tasks));
    }

    QDialog::accept();
}

void ImportPrimersDialog::init() {
    cbSource->addItem(LOCAL_FILES);
    cbSource->addItem(SHARED_DB);
}

void ImportPrimersDialog::connectSignals() {
    connect(cbSource, SIGNAL(currentIndexChanged(QString)), SLOT(sl_updateState()));
    connect(pbConnect, SIGNAL(clicked()), SLOT(sl_connectClicked()));
    connect(pbAddFile, SIGNAL(clicked()), SLOT(sl_addFileClicked()));
    connect(pbRemoveFile, SIGNAL(clicked()), SLOT(sl_removeFileClicked()));
    connect(pbAddObject, SIGNAL(clicked()), SLOT(sl_addObjectClicked()));
    connect(pbRemoveObject, SIGNAL(clicked()), SLOT(sl_removeObjectClicked()));
    connect(lwFiles, SIGNAL(itemSelectionChanged()), SLOT(sl_selectionChanged()));
    connect(lwObjects, SIGNAL(itemSelectionChanged()), SLOT(sl_selectionChanged()));
    connect(lwFiles->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(sl_contentChanged()));
    connect(lwFiles->model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(sl_contentChanged()));
    connect(lwObjects->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), SLOT(sl_contentChanged()));
    connect(lwObjects->model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), SLOT(sl_contentChanged()));
}

ProjectTreeControllerModeSettings ImportPrimersDialog::prepareProjectItemsSelectionSettings() const {
    ProjectTreeControllerModeSettings settings;
    settings.objectTypesToShow.insert(GObjectTypes::SEQUENCE);
    foreach (Document *document, AppContext::getProject()->getDocuments()) {
        if (!document->isDatabaseConnection()) {
            settings.excludeDocList << document;
        }
    }
    return settings;
}

}   // namespace U2
