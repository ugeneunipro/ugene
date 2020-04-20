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
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>
#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "EditSequenceDialogController.h"
#include "ui_EditSequenceDialog.h"

namespace U2{

//////////////////////////////////////////////////////////////////////////
//SeqPasterEventFilter
bool SeqPasterEventFilter::eventFilter( QObject* obj, QEvent *event ){
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (Qt::Key_Return == keyEvent->key()) {
            emit si_enterPressed();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

SeqPasterEventFilter::SeqPasterEventFilter( QObject* parent )
:QObject(parent){

}

//////////////////////////////////////////////////////////////////////////
//EditSequenceDialogController
EditSequenceDialogController::EditSequenceDialogController(const EditSequencDialogConfig &cfg, QWidget *p)
    : QDialog(p),
      filter(""),
      pos(1),
      saveController(NULL),
      config(cfg)
{
    ui = new Ui_EditSequenceDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748769");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    addSeqpasterWidget();
    w->disableCustomSettings();
    w->setPreferredAlphabet(cfg.alphabet);

    //selection
    ui->selectionGroupBox->setEnabled(false);
    if (!cfg.selectionRegions.isEmpty()){
        ui->selectionLineEdit->setText(U1AnnotationUtils::buildLocationString(cfg.selectionRegions));
    }
    connect(ui->beforeSelectionToolButton, SIGNAL(clicked()), this, SLOT(sl_beforeSlectionClicked()));
    connect(ui->afterSelectionToolButton, SIGNAL(clicked()), this, SLOT(sl_afterSlectionClicked()));

    seqEndPos = cfg.source.length + 1;

    ui->insertPositionSpin->setMinimum(1);
    ui->insertPositionSpin->setMaximum(seqEndPos);

    if ((1 < cfg.position) && (cfg.position < seqEndPos)) {
        ui->insertPositionSpin->setValue(cfg.position);
    }

    if (cfg.mode == EditSequenceMode_Insert) {
        setWindowTitle(tr("Insert Sequence"));
        if (!cfg.selectionRegions.isEmpty()) {
            ui->selectionGroupBox->setEnabled(true);
            sl_beforeSlectionClicked();
        }
    } else {
        setWindowTitle(tr("Replace sequence"));
        ui->splitRB->setEnabled(false);
        ui->split_separateRB->setEnabled(false);
        ui->insertPositionBox->setEnabled(false);
        w->selectText();
    }

    initSaveController();

    connect(ui->mergeAnnotationsBox, SIGNAL(toggled(bool)), this, SLOT(sl_mergeAnnotationsToggled()));
    connect(ui->startPosToolButton, SIGNAL(clicked()), this, SLOT(sl_startPositionliClicked()));
    connect(ui->endPosToolButton, SIGNAL(clicked()), this, SLOT(sl_endPositionliClicked()));

    //event filter
    SeqPasterEventFilter* evFilter= new SeqPasterEventFilter(this);
    w->setEventFilter(evFilter);
    connect(evFilter, SIGNAL(si_enterPressed()), this, SLOT(sl_enterPressed()));
}

void EditSequenceDialogController::accept(){
    QString validationError = w->validate();
    if(!validationError.isEmpty()){
        QMessageBox::critical(this, this->windowTitle(), validationError);
        return;
    }

    if ((w->getSequences().isEmpty() || w->getSequences().first().seq == config.initialText)
            && config.mode == EditSequenceMode_Replace ) {
        QDialog::reject();
        return;
    }

    if(!modifyCurrentDocument()){
        const QString url = saveController->getSaveFileName();
        QFileInfo fi(url);
        QDir dirToSave(fi.dir());
        if (!dirToSave.exists()){
            QMessageBox::critical(this, this->windowTitle(), tr("Folder to save is not exists"));
            return;
        }
        if(url.isEmpty()){
            QMessageBox::critical(this, this->windowTitle(), tr("Entered path is empty"));
            return;
        }
        if(fi.baseName().isEmpty()){
            QMessageBox::critical(this, this->windowTitle(), tr("Filename is empty"));
            return;
        }
    }
    pos = ui->insertPositionSpin->value() - 1;

    QDialog::accept();
}

void EditSequenceDialogController::addSeqpasterWidget(){
    w = new SeqPasterWidgetController(this, config.initialText, true);
    ui->globalLayout->insertWidget(0, w);

}

int EditSequenceDialogController::getPosToInsert() const {
    return pos;
}

U1AnnotationUtils::AnnotationStrategyForResize EditSequenceDialogController::getAnnotationStrategy() const {
    if (ui->resizeRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    } else if(ui->splitRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Joined;
    } else if(ui->split_separateRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate;
    } else {
        assert(ui->removeRB->isChecked());
        return U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }
}

void EditSequenceDialogController::sl_mergeAnnotationsToggled() {
    const QString fastaFormatName = DocumentFormatUtils::getFormatNameById(BaseDocumentFormats::FASTA);
    CHECK(!fastaFormatName.isEmpty(), );

    if (ui->mergeAnnotationsBox->isChecked()) {
        ui->formatBox->removeItem(ui->formatBox->findText(fastaFormatName));
    } else {
        ui->formatBox->addItem(fastaFormatName);
    }
    ui->formatBox->model()->sort(0);
}

DNASequence EditSequenceDialogController::getNewSequence() const {
    return w->getSequences().isEmpty() ? DNASequence() : w->getSequences().first();
}

GUrl EditSequenceDialogController::getDocumentPath() const {
    if (modifyCurrentDocument()) {
        return GUrl();
    } else {
        return GUrl(saveController->getSaveFileName());
    }
}

EditSequenceDialogController::~EditSequenceDialogController() {
    delete ui;
}

bool EditSequenceDialogController::mergeAnnotations() const {
    return (ui->mergeAnnotationsBox->isChecked() && !modifyCurrentDocument());
}

bool EditSequenceDialogController::recalculateQualifiers() const {
    return ui->recalculateQualsCheckBox->isChecked();
}

DocumentFormatId EditSequenceDialogController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

bool EditSequenceDialogController::modifyCurrentDocument() const {
    return !ui->saveToAnotherBox->isChecked();
}

void EditSequenceDialogController::initSaveController() {
    SaveDocumentControllerConfig conf;
    conf.defaultFormatId = BaseDocumentFormats::FASTA;
    conf.fileDialogButton = ui->browseButton;
    conf.fileNameEdit = ui->filepathEdit;
    conf.formatCombo = ui->formatBox;
    conf.parentWidget = this;
    conf.saveTitle = tr("Select file to save...");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::FASTA
                                                                      << BaseDocumentFormats::PLAIN_GENBANK;

    saveController = new SaveDocumentController(conf, formats, this);
}

void EditSequenceDialogController::sl_startPositionliClicked(){
    ui->insertPositionSpin->setValue(1);
}

void EditSequenceDialogController::sl_endPositionliClicked(){
    ui->insertPositionSpin->setValue(seqEndPos);
}

void EditSequenceDialogController::sl_beforeSlectionClicked(){
    SAFE_POINT(!config.selectionRegions.isEmpty(), "No selection", );
    U2Region containingregion = U2Region::containingRegion(config.selectionRegions);
    ui->insertPositionSpin->setValue(containingregion.startPos + 1);
}

void EditSequenceDialogController::sl_afterSlectionClicked(){
    SAFE_POINT(!config.selectionRegions.isEmpty(), "No selection", );
    U2Region containingregion = U2Region::containingRegion(config.selectionRegions);
    ui->insertPositionSpin->setValue(containingregion.endPos() + 1);
}

void EditSequenceDialogController::sl_enterPressed(){
    accept();
}

} // U2
