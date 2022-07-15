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

#include "ExportPrimersDialog.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportPrimersToDatabaseTask.h"
#include "ExportPrimersToLocalFileTask.h"

namespace U2 {

ExportPrimersDialog::ExportPrimersDialog(const QList<Primer>& primers, QWidget* parent)
    : QDialog(parent),
      primers(primers) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930783");

    init();
    connectSignals();
}

void ExportPrimersDialog::accept() {
    GUIUtils::setWidgetWarningStyle(leFilePath, false);

    U2OpStatusImpl os;
    GUrlUtils::validateLocalFileUrl(GUrl(saveController->getSaveFileName()), os);
    if (os.isCoR()) {
        GUIUtils::setWidgetWarningStyle(leFilePath, true);
        return;
    }

    Task* exportTask = new ExportPrimersToLocalFileTask(primers, saveController->getFormatIdToSave(), saveController->getSaveFileName());
    AppContext::getTaskScheduler()->registerTopLevelTask(exportTask);

    QDialog::accept();
}

void ExportPrimersDialog::init() {
    initSaveController();
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
    if (AppContext::getProject() != nullptr) {
        connectProjectSignals();
    }
}

void ExportPrimersDialog::connectProjectSignals() {
    connect(AppContext::getProject(), SIGNAL(si_documentAdded(Document*)), SLOT(sl_documentAdded(Document*)), Qt::UniqueConnection);
    connect(AppContext::getProject(), SIGNAL(si_documentRemoved(Document*)), SLOT(sl_documentRemoved(Document*)), Qt::UniqueConnection);
}

}  // namespace U2
