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

#include "CreateDocumentFromTextDialogController.h"

#include <QFileInfo>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/FastaFormat.h>
#include <U2Formats/GenbankPlainTextFormat.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/SaveDocumentController.h>

#include "CreateSequenceFromTextAndOpenViewTask.h"
#include "ui_CreateDocumentFromTextDialog.h"

namespace U2 {

CreateDocumentFromTextDialogController::CreateDocumentFromTextDialogController(QWidget *p)
    : QDialog(p),
      saveController(NULL) {
    ui = new Ui_CreateDocumentFromTextDialog();
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "46499629");

    initSaveController();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(ui->filepathEdit, SIGNAL(textChanged(QString)), SLOT(sl_filepathTextChanged()));
    ui->nameEdit->setText("Sequence");

    addSeqPasterWidget();
}

void CreateDocumentFromTextDialogController::accept() {
    QString validationError = w->validate();
    if (!validationError.isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), validationError);
        return;
    }

    const QString url = saveController->getSaveFileName();
    QFileInfo fi(url);

    if (fi.baseName().isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), tr("Filename is empty"));
        return;
    }

    if (url.isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), tr("No path specified"));
        return;
    }

    U2OpStatus2Log os;
    QString fullPath = GUrlUtils::prepareFileLocation(url, os);

    if (fullPath.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), os.getError());
        return;
    }

    if (ui->nameEdit->text().isEmpty()) {
        QMessageBox::critical(this, this->windowTitle(), tr("Sequence name is empty"));
        return;
    }

    CHECK_OP(os, );

    Task *task = new CreateSequenceFromTextAndOpenViewTask(prepareSequences(), saveController->getFormatIdToSave(), GUrl(fullPath), ui->saveImmediatelyBox->isChecked());
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    QDialog::accept();
}

void CreateDocumentFromTextDialogController::addSeqPasterWidget() {
    w = new SeqPasterWidgetController(this);
    w->allowFastaFormat(true);
    ui->verticalLayout->insertWidget(0, w);
}

QList<DNASequence> CreateDocumentFromTextDialogController::prepareSequences() const {
    QList<DNASequence> sequences = w->getSequences();
    for (int i = 0; i < sequences.size(); i++) {
        if (sequences[i].getName().isEmpty()) {
            sequences[i].setName(ui->nameEdit->text());
        }
    }
    return sequences;
}

void CreateDocumentFromTextDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
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

CreateDocumentFromTextDialogController::~CreateDocumentFromTextDialogController() {
    delete ui;
}

void CreateDocumentFromTextDialogController::sl_filepathTextChanged() {
    QFileInfo newFile(saveController->getSaveFileName());
    if (ui->nameEdit->text() != newFile.baseName()) {
        newFile.baseName().isEmpty() ? ui->nameEdit->setText("Sequence") : ui->nameEdit->setText(newFile.baseName());
    }
}

}    // namespace U2
