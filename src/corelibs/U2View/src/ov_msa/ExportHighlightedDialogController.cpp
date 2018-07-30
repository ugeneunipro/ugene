/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportHighlightedDialogController.h"
#include "ov_msa/MSAEditorSequenceArea.h"
#include "ui_ExportHighlightedDialog.h"

namespace U2{

ExportHighligtingDialogController::ExportHighligtingDialogController(MaEditorWgt *msaui_, QWidget* p )
    : QDialog(p),
      msaui(msaui_),
      saveController(NULL),
      ui(new Ui_ExportHighlightedDialog())
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "21433253");

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    CHECK(AppContext::getAppSettings(), );
    CHECK(AppContext::getAppSettings()->getUserAppsSettings(), );
    CHECK(msaui->getEditor(), );
    CHECK(msaui->getEditor()->getMaObject(), );

    initSaveController();

    int alignLength = msaui->getEditor()->getMaObject()->getLength();
    QRect selection = msaui->getSequenceArea()->getSelection().getRect();

    int startPos = -1;
    int endPos = -1;
    if (selection.isNull() || selection.width() == 1) {
        startPos = 1;
        endPos = alignLength;
    } else {
        startPos = selection.x() + 1;
        endPos = selection.x() + selection.width();
    }

    ui->startLineEdit->setMinimum(1);
    ui->endLineEdit->setMinimum(1);

    ui->startLineEdit->setMaximum(alignLength);
    ui->endLineEdit->setMaximum(alignLength);

    ui->startLineEdit->setValue(startPos);
    ui->endLineEdit->setValue(endPos);

    connect(ui->startLineEdit, SIGNAL(valueChanged(int)), SLOT(sl_regionChanged()));
    connect(ui->endLineEdit, SIGNAL(valueChanged(int)), SLOT(sl_regionChanged()));
}

ExportHighligtingDialogController::~ExportHighligtingDialogController(){
    delete ui;
}

void ExportHighligtingDialogController::accept(){
    startPos = ui->startLineEdit->value();
    endPos = ui->endLineEdit->value();
    if(ui->oneIndexRB->isChecked()){
        startingIndex = 1;
    }else{
        startingIndex = 0;
    }
    if (saveController->getSaveFileName().isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("Export to file URL is empty!"));
        return;
    }
    keepGaps = ui->keepGapsBox->isChecked();
    dots = ui->dotsBox->isChecked();
    transpose = ui->transposeBox->isChecked();
    url = GUrl(saveController->getSaveFileName());

    QDialog::accept();
}

void ExportHighligtingDialogController::lockKeepGaps(){
    ui->keepGapsBox->setChecked(true);
    ui->keepGapsBox->setDisabled(true);
}

void ExportHighligtingDialogController::sl_regionChanged(){
    bool validRange = ui->endLineEdit->value() - ui->startLineEdit->value() >= 0;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(validRange);

    if (validRange) {
        ui->startLineEdit->setStyleSheet("QSpinBox {}");
        ui->endLineEdit->setStyleSheet("QSpinBox {}");
    } else {
        ui->startLineEdit->setStyleSheet("QSpinBox { background-color: rgb(255, 200, 200); }");
        ui->endLineEdit->setStyleSheet("QSpinBox { background-color: rgb(255, 200, 200); }");
    }
}

void ExportHighligtingDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFileName = GUrlUtils::getDefaultDataPath() + "/" + msaui->getEditor()->getMaObject()->getGObjectName() + "_highlighting.txt";
    config.defaultFormatId = BaseDocumentFormats::PLAIN_TEXT;
    config.fileDialogButton = ui->fileButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save...");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::PLAIN_TEXT;

    saveController = new SaveDocumentController(config, formats, this);
}

}
