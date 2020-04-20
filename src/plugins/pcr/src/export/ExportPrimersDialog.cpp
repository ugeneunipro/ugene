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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/SharedConnectionsDialog.h>
#include <U2Gui/U2FileDialog.h>

#include "ExportPrimersDialog.h"
#include "ExportPrimersToDatabaseTask.h"
#include "ExportPrimersToLocalFileTask.h"

namespace U2 {

const QString ExportPrimersDialog::LOCAL_FILE = QObject::tr("Local file");
const QString ExportPrimersDialog::SHARED_DB = QObject::tr("Shared database");

ExportPrimersDialog::ExportPrimersDialog(const QList<Primer> &primers, QWidget *parent) :
    QDialog(parent),
    primers(primers)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749035");

    init();
    connectSignals();
    sl_updateState();
}

void ExportPrimersDialog::sl_updateState() {
    fileContainer->setVisible(isFileMode());
    databaseContainer->setVisible(!isFileMode());
    folderContainer->setEnabled(0 < cbDatabase->count());
}

void ExportPrimersDialog::sl_connect() {
    const bool projectExists = (NULL != AppContext::getProject());
    QPointer<SharedConnectionsDialog> connectionDialog = new SharedConnectionsDialog(this);
    if (projectExists) {
        connectProjectSignals();
    } else {
        connect(connectionDialog.data(), SIGNAL(si_connectionCompleted()), SLOT(sl_connectionCompleted()));
    }

    connectionDialog->exec();
    CHECK(!connectionDialog.isNull(), );

    if (projectExists) {
        delete connectionDialog;
    }
}

void ExportPrimersDialog::sl_connectionCompleted() {
    QObject *connectionDialog = sender();
    if (Q_LIKELY(NULL != connectionDialog)) {
        connectionDialog->deleteLater();
    }
    initDatabases();
    sl_updateState();
}

void ExportPrimersDialog::sl_documentAdded(Document *document) {
    CHECK(document->isDatabaseConnection(), );
    cbDatabase->addItem(document->getName(), QVariant::fromValue<U2DbiRef>(document->getDbiRef()));
    if (cbDatabase->currentIndex() == -1) {
        cbDatabase->setCurrentIndex(0);
    }
    sl_updateState();
}

void ExportPrimersDialog::sl_documentRemoved(Document *document) {
    CHECK(document->isDatabaseConnection(), );
    for (int i = 0; i < cbDatabase->count(); i++) {
        if (cbDatabase->itemData(i).value<U2DbiRef>() == document->getDbiRef()) {
            cbDatabase->removeItem(i);
        }
    }
    sl_updateState();
}

void ExportPrimersDialog::sl_folderBrowse() {
    const Folder folder = ProjectTreeItemSelectorDialog::selectFolder(this);
    if (!folder.getFolderPath().isEmpty()) {
        leFolder->setText(folder.getFolderPath());
    }
}

void ExportPrimersDialog::accept() {
    GUIUtils::setWidgetWarning(leFilePath, false);
    GUIUtils::setWidgetWarning(cbDatabase, false);
    GUIUtils::setWidgetWarning(leFolder, false);

    U2OpStatusImpl os;
    GUrlUtils::validateLocalFileUrl(GUrl(saveController->getSaveFileName()), os);
    if (LOCAL_FILE == cbExport->currentText() && os.isCoR()) {
        GUIUtils::setWidgetWarning(leFilePath, true);
        return;
    }

    if (SHARED_DB == cbExport->currentText() && cbDatabase->currentText().isEmpty()) {
        GUIUtils::setWidgetWarning(cbDatabase, true);
        return;
    }

    if (SHARED_DB == cbExport->currentText() && !leFolder->text().startsWith(U2ObjectDbi::ROOT_FOLDER)) {
        GUIUtils::setWidgetWarning(leFolder, true);
        return;
    }

    Task *exportTask = NULL;
    if (LOCAL_FILE == cbExport->currentText()) {
        exportTask = new ExportPrimersToLocalFileTask(primers, saveController->getFormatIdToSave(), saveController->getSaveFileName());
    } else {
        exportTask = new ExportPrimersToDatabaseTask(primers, cbDatabase->itemData(cbDatabase->currentIndex()).value<U2DbiRef>(), leFolder->text());
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(exportTask);

    QDialog::accept();
}

void ExportPrimersDialog::init() {
    cbExport->addItem(LOCAL_FILE);
    cbExport->addItem(SHARED_DB);

    initSaveController();
    initDatabases();
}

void ExportPrimersDialog::initDatabases() {
    while (0 < cbDatabase->count()) {
        cbDatabase->removeItem(0);
    }

    const QList<Document *> connectedDatabases = ProjectUtils::getConnectedSharedDatabases();
    foreach (Document *database, connectedDatabases) {
        cbDatabase->addItem(database->getName(), QVariant::fromValue<U2DbiRef>(database->getDbiRef()));
    }
}

void ExportPrimersDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = "ExportPrimersDialog";
    config.defaultFileName = GUrlUtils::getDefaultDataPath() + "/primers.gb";
    config.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    config.fileDialogButton = tbFileBrowse;
    config.fileNameEdit = leFilePath;
    config.formatCombo = cbFormat;
    config.parentWidget = this;
    config.saveTitle = tr("Export to");

    DocumentFormatConstraints constraints;
    constraints.supportedObjectTypes.insert(GObjectTypes::SEQUENCE);
    constraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    constraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    constraints.addFlagToExclude(DocumentFormatFlag_Hidden);
    if (primers.size() > 1) {
        constraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    }

    saveController = new SaveDocumentController(config, constraints, this);
}

void ExportPrimersDialog::connectSignals() {
    connect(cbExport, SIGNAL(currentIndexChanged(int)), SLOT(sl_updateState()));
    connect(tbConnect, SIGNAL(clicked()), SLOT(sl_connect()));
    connect(tbFolderBrowse, SIGNAL(clicked()), SLOT(sl_folderBrowse()));
    if (NULL != AppContext::getProject()) {
        connectProjectSignals();
    }
}

void ExportPrimersDialog::connectProjectSignals(){
    connect(AppContext::getProject(), SIGNAL(si_documentAdded(Document *)), SLOT(sl_documentAdded(Document *)), Qt::UniqueConnection);
    connect(AppContext::getProject(), SIGNAL(si_documentRemoved(Document*)), SLOT(sl_documentRemoved(Document *)), Qt::UniqueConnection);
}

bool ExportPrimersDialog::isFileMode() const {
    return LOCAL_FILE == cbExport->currentText();
}

}   // namespace U2
