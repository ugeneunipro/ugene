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

#include <QList>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportAnnotationsDialog.h"
#include "ui_ExportAnnotationsDialog.h"

namespace U2 {

const QString ExportAnnotationsDialog::CSV_FORMAT_ID( "csv" );

ExportAnnotationsDialog::ExportAnnotationsDialog( const QString &filename, QWidget *parent )
    : QDialog( parent ), ui( new Ui_ExportAnnotationsDialog( ) )
{
    ui->setupUi( this );
    lastAddToProjectState = ui->addToProjectCheck->isChecked();
    new HelpButton(this, ui->buttonBox, "24748792");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController(filename);
    sl_formatChanged(saveController->getFormatIdToSave());
    window()->resize(window()->width(), 0);

}

ExportAnnotationsDialog::~ExportAnnotationsDialog( ) {
    delete ui;
}

void ExportAnnotationsDialog::initSaveController(const QString &filename) {
    SaveDocumentControllerConfig config;
    config.defaultDomain = "ExportAnnotationsDialogHelperDomain";
    config.defaultFileName = filename;
    config.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    config.fileDialogButton = ui->chooseFileButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.formatCombo = ui->formatsBox;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save annotations");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes.insert(GObjectTypes::ANNOTATION_TABLE);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    formatConstraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    formatConstraints.addFlagToExclude(DocumentFormatFlag_Hidden);
    formatConstraints.formatsToExclude << BaseDocumentFormats::VECTOR_NTI_SEQUENCE;

    saveController = new SaveDocumentController(config, formatConstraints, this);
    saveController->addFormat(CSV_FORMAT_ID, QString(CSV_FORMAT_ID).toUpper(), QStringList() << CSV_FORMAT_ID);

    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
    connect(ui->addToProjectCheck, SIGNAL(clicked(bool)), SLOT(sl_addToProjectStateChanged(bool)));

}

QString ExportAnnotationsDialog::filePath() const {
    return saveController->getSaveFileName();
}

bool ExportAnnotationsDialog::exportSequence() const {
    return ui->exportSequenceCheck->isChecked();
}

bool ExportAnnotationsDialog::exportSequenceNames() const {
    return ui->exportSequenceNameCheck->isChecked();
}

bool ExportAnnotationsDialog::addToProject() const {
    return ui->addToProjectCheck->isChecked();
}

void ExportAnnotationsDialog::sl_formatChanged(const QString &newFormatId) {
    const bool isCsvFormat = (CSV_FORMAT_ID == newFormatId);
    ui->exportSequenceCheck->setEnabled(isCsvFormat);
    ui->exportSequenceNameCheck->setEnabled(isCsvFormat);
    ui->addToProjectCheck->setEnabled(!isCsvFormat);
    ui->addToProjectCheck->setChecked(!isCsvFormat && lastAddToProjectState);
}

void ExportAnnotationsDialog::sl_addToProjectStateChanged(bool state){
    lastAddToProjectState = state;
}

QString ExportAnnotationsDialog::fileFormat( ) const {
    return saveController->getFormatIdToSave();
}

} // namespace U2
