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

#include <QDir>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AnnotationData.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ModifySequenceObjectTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>
#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "RemovePartFromSequenceDialogController.h"
#include "ui_RemovePartFromSequenceDialog.h"

namespace U2{

RemovePartFromSequenceDialogController::RemovePartFromSequenceDialogController(U2Region _toDelete, 
                                                                               U2Region _source, 
                                                                               const QString & docUrl,
                                                                               QWidget *p)
    : QDialog(p),
      toDelete(_toDelete),
      source(_source),
      ui(new Ui_RemovePartFromSequenceDialog),
      saveController(NULL)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748769");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Remove"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    
    initSaveController(docUrl);

    SharedAnnotationData ad(new AnnotationData);
    ad->location->regions << toDelete;
    ui->removeLocationEdit->setText(U1AnnotationUtils::buildLocationString(ad));

    connect(ui->mergeAnnotationsBox, SIGNAL(toggled(bool)), this, SLOT(sl_mergeAnnotationsToggled(bool)));
}

void RemovePartFromSequenceDialogController::accept() {
    QString genbankRegion = ui->removeLocationEdit->text();
    U2Location location;
    Genbank::LocationParser::parseLocation(genbankRegion.toLatin1().constData(), genbankRegion.length(), location);
    if (location->isMultiRegion()){
        QMessageBox::critical(this, this->windowTitle(), tr("There must be only one region to delete"));
        return;
    }    
    if (location->isEmpty()){
        QMessageBox::critical(this, this->windowTitle(), tr("Unable to parse region to delete"));
        return;
    }
    toDelete = location->regions.first();

    if (toDelete == source) {
        QMessageBox::critical(this, this->windowTitle(), tr("Cannot remove the whole sequence"));
        return;
    }
    
    if (toDelete.startPos < source.startPos || toDelete.endPos() > source.endPos()){
        QMessageBox::critical(this, this->windowTitle(), tr("Region to delete is out of sequence bounds"));
        return;
    }

    QDialog::accept();
}

U1AnnotationUtils::AnnotationStrategyForResize RemovePartFromSequenceDialogController::getStrategy(){
    if(ui->removeRB->isChecked()){
        return U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }else{
        assert(ui->resizeRB->isChecked());
        return U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    }
}

U2Region RemovePartFromSequenceDialogController::getRegionToDelete() const {
    return toDelete;
}

bool RemovePartFromSequenceDialogController::recalculateQualifiers() const {
    return ui->recalculateQualsCheckBox->isChecked();
}

void RemovePartFromSequenceDialogController::sl_mergeAnnotationsToggled( bool ) {
    const QString fastaFormatName = DocumentFormatUtils::getFormatNameById(BaseDocumentFormats::FASTA);
    CHECK(!fastaFormatName.isEmpty(), );

    if (ui->mergeAnnotationsBox->isChecked()) {
        ui->formatBox->removeItem(ui->formatBox->findText(fastaFormatName));
    } else {
        ui->formatBox->addItem(fastaFormatName);
    }
    ui->formatBox->model()->sort(0);
}

void RemovePartFromSequenceDialogController::initSaveController(const QString &docUrl) {
    const QFileInfo fi(docUrl);

    SaveDocumentControllerConfig config;
    config.defaultFileName = fi.absoluteDir().absolutePath() + "/" + fi.baseName() + "_new" + "." + fi.completeSuffix();
    config.defaultFormatId = BaseDocumentFormats::FASTA;
    config.fileDialogButton = ui->browseButton;
    config.fileNameEdit = ui->filepathEdit;
    config.formatCombo = ui->formatBox;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save...");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::FASTA
                                                                      << BaseDocumentFormats::PLAIN_GENBANK;

    saveController = new SaveDocumentController(config, formats, this);
}

bool RemovePartFromSequenceDialogController::modifyCurrentDocument() const {
    return !ui->saveToAnotherBox->isChecked();
}

QString RemovePartFromSequenceDialogController::getNewDocumentPath() const {
    return saveController->getSaveFileName();
}

bool RemovePartFromSequenceDialogController::mergeAnnotations() const {
    return (ui->mergeAnnotationsBox->isChecked() && !modifyCurrentDocument());
}

DocumentFormatId RemovePartFromSequenceDialogController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

RemovePartFromSequenceDialogController::~RemovePartFromSequenceDialogController() {
    delete ui;
}

}//ns
