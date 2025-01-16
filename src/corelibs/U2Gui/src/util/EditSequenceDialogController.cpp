/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "EditSequenceDialogController.h"

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
#include <U2Gui/U2LongLongValidator.h>

#include "ui_EditSequenceDialog.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// SeqPasterEventFilter
bool SeqPasterEventFilter::eventFilter(QObject* obj, QEvent* event) {
    if (QEvent::KeyPress == event->type()) {
        auto keyEvent = static_cast<QKeyEvent*>(event);
        if (Qt::Key_Return == keyEvent->key()) {
            emit si_enterPressed();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

SeqPasterEventFilter::SeqPasterEventFilter(QObject* parent)
    : QObject(parent) {
}

//////////////////////////////////////////////////////////////////////////
// EditSequenceDialogVirtualController
EditSequenceDialogVirtualController::EditSequenceDialogVirtualController(const EditSequencDialogConfig& cfg, QWidget* p, const QString& helpId)
    : QDialog(p),
      filter(""),
      saveController(nullptr),
      config(cfg) {
    ui = new Ui_EditSequenceDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, helpId);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // selection
    ui->selectionGroupBox->setEnabled(false);
    if (!cfg.selectionRegions.isEmpty()) {
        ui->selectionLineEdit->setText(U1AnnotationUtils::buildLocationString(cfg.selectionRegions));
    }
    connect(ui->beforeSelectionToolButton, SIGNAL(clicked()), this, SLOT(sl_beforeSlectionClicked()));
    connect(ui->afterSelectionToolButton, SIGNAL(clicked()), this, SLOT(sl_afterSlectionClicked()));

    seqEndPos = cfg.source.length + 1;

    ui->insertPositionLineEdit->setValidator(new U2LongLongValidator(1, seqEndPos, ui->insertPositionLineEdit));

    qint64 initValue = 1;
    if ((1 < cfg.position) && (cfg.position < seqEndPos)) {
        initValue = cfg.position;
    }
    ui->insertPositionLineEdit->setText(QString::number(initValue));

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
    }

    initSaveController();

    connect(ui->mergeAnnotationsBox, SIGNAL(toggled(bool)), this, SLOT(sl_mergeAnnotationsToggled()));
    connect(ui->startPosToolButton, SIGNAL(clicked()), this, SLOT(sl_startPositionliClicked()));
    connect(ui->endPosToolButton, SIGNAL(clicked()), this, SLOT(sl_endPositionliClicked()));
}

void EditSequenceDialogVirtualController::accept() {
    if (!modifyCurrentDocument()) {
        const QString url = saveController->getSaveFileName();
        QFileInfo fi(url);
        QDir dirToSave(fi.dir());
        if (!dirToSave.exists()) {
            QMessageBox::critical(this, this->windowTitle(), tr("Folder to save is not exists"));
            return;
        }
        if (url.isEmpty()) {
            QMessageBox::critical(this, this->windowTitle(), tr("Entered path is empty"));
            return;
        }
        if (fi.baseName().isEmpty()) {
            QMessageBox::critical(this, this->windowTitle(), tr("Filename is empty"));
            return;
        }
    }

    auto insertPositionText = ui->insertPositionLineEdit->text();
    int posUnused = 0; // Not used by this validator
    auto state = ui->insertPositionLineEdit->validator()->validate(insertPositionText, posUnused);
    if (state != QValidator::State::Acceptable) {
        QMessageBox::critical(this, this->windowTitle(), tr("Incorrect position to insert, should be from 1 to %1").arg(seqEndPos));
        return;
    }

    CHECK(state == QValidator::State::Acceptable, )

    bool ok = false;
    qint64 value = insertPositionText.toULongLong(&ok);
    SAFE_POINT(ok, "Should be number", QDialog::reject());

    pos = value - 1;

    QDialog::accept();
}

qint64 EditSequenceDialogVirtualController::getPosToInsert() const {
    return pos;
}

U1AnnotationUtils::AnnotationStrategyForResize EditSequenceDialogVirtualController::getAnnotationStrategy() const {
    if (ui->resizeRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Resize;
    } else if (ui->splitRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Joined;
    } else if (ui->split_separateRB->isChecked()) {
        return U1AnnotationUtils::AnnotationStrategyForResize_Split_To_Separate;
    } else {
        assert(ui->removeRB->isChecked());
        return U1AnnotationUtils::AnnotationStrategyForResize_Remove;
    }
}

void EditSequenceDialogVirtualController::sl_mergeAnnotationsToggled() {
    const QString fastaFormatName = DocumentFormatUtils::getFormatNameById(BaseDocumentFormats::FASTA);
    CHECK(!fastaFormatName.isEmpty(), );

    if (ui->mergeAnnotationsBox->isChecked()) {
        ui->formatBox->removeItem(ui->formatBox->findText(fastaFormatName));
    } else {
        ui->formatBox->addItem(fastaFormatName);
    }
    ui->formatBox->model()->sort(0);
}

GUrl EditSequenceDialogVirtualController::getDocumentPath() const {
    if (modifyCurrentDocument()) {
        return GUrl();
    } else {
        return GUrl(saveController->getSaveFileName());
    }
}

bool EditSequenceDialogVirtualController::mergeAnnotations() const {
    return (ui->mergeAnnotationsBox->isChecked() && !modifyCurrentDocument());
}

bool EditSequenceDialogVirtualController::recalculateQualifiers() const {
    return ui->recalculateQualsCheckBox->isChecked();
}

DocumentFormatId EditSequenceDialogVirtualController::getDocumentFormatId() const {
    return saveController->getFormatIdToSave();
}

void EditSequenceDialogVirtualController::addInputDataWidgetToLayout(QWidget* w) {
    ui->globalLayout->insertWidget(0, w);
}

bool EditSequenceDialogVirtualController::modifyCurrentDocument() const {
    return !ui->saveToAnotherBox->isChecked();
}

void EditSequenceDialogVirtualController::initSaveController() {
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

void EditSequenceDialogVirtualController::sl_enterPressed() {
    accept();
}

void EditSequenceDialogVirtualController::sl_startPositionliClicked() {
    ui->insertPositionLineEdit->setText(QString::number(1));
}

void EditSequenceDialogVirtualController::sl_endPositionliClicked() {
    ui->insertPositionLineEdit->setText(QString::number(seqEndPos));
}

void EditSequenceDialogVirtualController::sl_beforeSlectionClicked() {
    SAFE_POINT(!config.selectionRegions.isEmpty(), "No selection", );
    U2Region containingregion = U2Region::containingRegion(config.selectionRegions);
    ui->insertPositionLineEdit->setText(QString::number(containingregion.startPos + 1));
}

void EditSequenceDialogVirtualController::sl_afterSlectionClicked() {
    SAFE_POINT(!config.selectionRegions.isEmpty(), "No selection", );
    U2Region containingregion = U2Region::containingRegion(config.selectionRegions);
    ui->insertPositionLineEdit->setText(QString::number(containingregion.endPos() + 1));
}


//////////////////////////////////////////////////////////////////////////
// EditSequenceDialogController

EditSequenceDialogController::EditSequenceDialogController(const EditSequencDialogConfig& cfg, QWidget* p)
    : EditSequenceDialogVirtualController(cfg, p) {

    seqPasterWidgetController = new SeqPasterWidgetController(this, config.initialText, true);
    addInputDataWidgetToLayout(seqPasterWidgetController);

    seqPasterWidgetController->disableCustomSettings();
    seqPasterWidgetController->setPreferredAlphabet(config.alphabet);

    auto evFilter = new SeqPasterEventFilter(this);
    seqPasterWidgetController->setEventFilter(evFilter);
    connect(evFilter, &SeqPasterEventFilter::si_enterPressed, this, &EditSequenceDialogController::sl_enterPressed);

    if (config.mode == EditSequenceMode_Replace) {
        seqPasterWidgetController->selectText();
    }
}

void EditSequenceDialogController::accept() {
    QString validationError = seqPasterWidgetController->validate();
    if (!validationError.isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), validationError);
        return;
    }

    if ((seqPasterWidgetController->getSequences().isEmpty() || seqPasterWidgetController->getSequences().first().seq == config.initialText) && config.mode == EditSequenceMode_Replace) {
        QDialog::reject();
        return;
    }

    EditSequenceDialogVirtualController::accept();
}

DNASequence EditSequenceDialogController::getNewSequence() const {
    return seqPasterWidgetController->getSequences().isEmpty() ? DNASequence() : seqPasterWidgetController->getSequences().first();
}


}  // namespace U2
