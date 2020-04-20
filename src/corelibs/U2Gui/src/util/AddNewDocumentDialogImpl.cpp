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

#include <QPushButton>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "AddNewDocumentDialogImpl.h"
#include "DocumentFormatComboboxController.h"

namespace U2 {

//BUG:419: add label to dialog with state description!

#define SETTINGS_LASTFORMAT     "add_new_document/last_format"
#define SETTINGS_LASTDIR        "add_new_document/last_dir"

AddNewDocumentDialogImpl::AddNewDocumentDialogImpl(QWidget* p, AddNewDocumentDialogModel& m, const DocumentFormatConstraints& c) 
    : QDialog(p),
      model(m)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748813");
    if (model.format.isEmpty()) {
        model.format = AppContext::getSettings()->getValue(SETTINGS_LASTFORMAT, QString("")).toString();
    }
    
    formatController = new DocumentFormatComboboxController(this, documentTypeCombo, c, model.format);
    model.successful = false;
    
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    SaveDocumentControllerConfig conf;
    conf.defaultFileName = model.url;
    conf.defaultFormatId = model.format;
    conf.fileDialogButton = documentURLButton;
    conf.fileNameEdit = documentURLEdit;
    conf.formatCombo = documentTypeCombo;
    conf.parentWidget = this;
    conf.saveTitle = tr("Save File");
    conf.defaultDomain = SETTINGS_LASTDIR;
    conf.compressCheckbox = gzipCheckBox;

    saveController = new SaveDocumentController(conf, formatController->getFormatsInCombo(), this);
}

void AddNewDocumentDialogImpl::accept() {
    model.format = saveController->getFormatIdToSave();
    model.url = saveController->getSaveFileName();
    if (model.url.isEmpty()) {
        QMessageBox::critical(this, tr("Invalid Document Location"), tr("Document location is empty"));
        documentURLEdit->setFocus();
        return;
    }
    model.io = gzipCheckBox->isChecked() ? BaseIOAdapters::GZIPPED_LOCAL_FILE : BaseIOAdapters::LOCAL_FILE;
    model.successful = true;
    AppContext::getSettings()->setValue(SETTINGS_LASTFORMAT, model.format);
    AppContext::getSettings()->setValue(SETTINGS_LASTDIR, QFileInfo(model.url).absoluteDir().absolutePath());
    QDialog::accept();
}

void AddNewDocumentDialogController::run(QWidget* p, AddNewDocumentDialogModel& m, const DocumentFormatConstraints& c) {
    Project* proj = AppContext::getProject();
    if (proj->isStateLocked()) {
        QMessageBox::critical(NULL, L10N::errorTitle(), AddNewDocumentDialogImpl::tr("Project is locked"));
        m.successful = false;
        return;
    }

    QObjectScopedPointer<AddNewDocumentDialogImpl> d = new AddNewDocumentDialogImpl(p, m, c);
    d->exec();
    CHECK(!d.isNull(), );

    m = d->model;
    assert(proj->findDocumentByURL(m.url) == NULL);
}

}   // namespace U2
