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
#include <U2Core/Settings.h>

#include <U2Designer/DesignerUtils.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/WorkflowUtils.h>

#include "WorkflowDocument.h"
#include "WorkflowMetaDialog.h"

namespace U2 {

#define LAST_DIR QString("workflowview/lastdir")

WorkflowMetaDialog::WorkflowMetaDialog(QWidget * p, const Metadata& meta)
    : QDialog(p),
      meta(meta),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "28966990");

    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    okButton = buttonBox->button(QDialogButtonBox::Ok);

    initSaveController();

    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_onSave()));
    connect(urlEdit, SIGNAL(textChanged(const QString & )), SLOT(sl_onURLChanged(const QString& )));
    connect(urlEdit, SIGNAL(textEdited (const QString & )), SLOT(sl_onURLChanged(const QString& )));

    okButton->setDisabled(meta.url.isEmpty());
    nameEdit->setText(meta.name);
    commentEdit->setText(meta.comment);
}

void WorkflowMetaDialog::sl_onSave() {
    assert(!WorkflowUtils::WD_FILE_EXTENSIONS.isEmpty());
    QString url = saveController->getSaveFileName();
    bool endsWithWDExt = false;
    foreach( const QString & ext, WorkflowUtils::WD_FILE_EXTENSIONS ) {
        assert(!ext.isEmpty());
        if(url.endsWith(ext)) {
            endsWithWDExt = true;
        }
    }
    if(!endsWithWDExt) {
        url.append("." + WorkflowUtils::WD_FILE_EXTENSIONS.first());
    }
    meta.url = url;
    meta.comment = commentEdit->toPlainText();
    meta.name = nameEdit->text();
    accept();
}

void WorkflowMetaDialog::sl_onURLChanged(const QString & text) {
    okButton->setDisabled(text.isEmpty());
}

void WorkflowMetaDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = LAST_DIR;
    config.defaultFileName = meta.url;
    config.defaultFormatId = WorkflowDocFormat::FORMAT_ID;
    config.fileDialogButton = browseButton;
    config.fileNameEdit = urlEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save workflow to file");
    config.rollFileName = false;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << WorkflowDocFormat::FORMAT_ID;

    saveController = new SaveDocumentController(config, formats, this);
}

}//namespace
