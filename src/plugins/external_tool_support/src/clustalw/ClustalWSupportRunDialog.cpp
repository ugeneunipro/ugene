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
#include <QPushButton>
#include <QToolButton>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "ClustalWSupportRunDialog.h"

namespace U2 {
////////////////////////////////////////
//ClustalWSupportRunDialog
ClustalWSupportRunDialog::ClustalWSupportRunDialog(const MultipleSequenceAlignment& _ma, ClustalWSupportTaskSettings& _settings, QWidget* _parent) :
        QDialog(_parent), ma(_ma->getCopy()), settings(_settings)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749014");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    inputGroupBox->setVisible(false);
    this->adjustSize();

    connect(this->iterationTypeCheckBox,SIGNAL(toggled(bool)),this,SLOT(sl_iterationTypeEnabled(bool)));

    if(ma->getAlphabet()->isAmino()){
        gapOpenSpinBox->setValue(10.0);
        gapExtSpinBox->setValue(0.2);
        proteinGapParamGroupBox->setEnabled(true);
        weightMatrixComboBox->clear();
        weightMatrixComboBox->addItem("BLOSUM");
        weightMatrixComboBox->addItem("PAM");
        weightMatrixComboBox->addItem("GONNET");
        weightMatrixComboBox->addItem("ID");
    }
}

void ClustalWSupportRunDialog::sl_iterationTypeEnabled(bool checked){
    if(checked){
        iterationTypeComboBox->removeItem(0);
    }else{
        iterationTypeComboBox->insertItem(0,"NONE");
        iterationTypeComboBox->setCurrentIndex(0);
    }
}

void ClustalWSupportRunDialog::accept(){
    if(gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if(gapExtCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtSpinBox->value();
    }
    if(gapDistancesCheckBox->isChecked()){
        settings.gapDist = gapDistancesSpinBox->value();
    }
    if(residueSpecificGapsOffCheckBox->isChecked()){
        settings.noPGaps=true;

    }
    if(hydrophilicGapsOffCheckBox->isChecked()){
        settings.noHGaps=true;
    }
    if(endGapsCheckBox->isChecked()){
        settings.endGaps=true;
    }
    if(weightMatrixCheckBox->isChecked()){
        settings.matrix=weightMatrixComboBox->currentText();
    }
    if(outOrderCheckBox->isChecked()){
        if(outOrderComboBox->currentText() == "Input"){
            settings.outOrderInput=true;
        }else{
            settings.outOrderInput=false;
        }
    }
    if(iterationTypeCheckBox->isChecked()){
        settings.iterationType=iterationTypeComboBox->currentText();
        if(maxIterationsCheckBox->isChecked()){
            settings.numIterations=maxIterationsSpinBox->value();
        }
    }
    QDialog::accept();
}

////////////////////////////////////////
//ClustalWWithExtFileSpecifySupportRunDialog
ClustalWWithExtFileSpecifySupportRunDialog::ClustalWWithExtFileSpecifySupportRunDialog(ClustalWSupportTaskSettings& _settings, QWidget* _parent) :
    QDialog(_parent),
    settings(_settings),
    saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749014");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Align"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController();

    connect(inputFilePathButton, SIGNAL(clicked()), SLOT(sl_inputPathButtonClicked()));

    proteinGapParamGroupBox->setEnabled(true);
    weightMatrixComboBox->insertSeparator(2);
    weightMatrixComboBox->addItem("BLOSUM");
    weightMatrixComboBox->addItem("PAM");
    weightMatrixComboBox->addItem("GONNET");
    weightMatrixComboBox->addItem("ID");
}

void ClustalWWithExtFileSpecifySupportRunDialog::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir,
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true));
    if (lod.url.isEmpty()) {
        return;
    }
    inputFileLineEdit->setText(lod.url);
}

void ClustalWWithExtFileSpecifySupportRunDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = outputFilePathButton;
    config.fileNameEdit = outputFileLineEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save an multiple alignment file");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::CLUSTAL_ALN;

    saveController = new SaveDocumentController(config, formats, this);
}

void ClustalWWithExtFileSpecifySupportRunDialog::sl_iterationTypeEnabled(bool checked){
    if(checked){
        iterationTypeComboBox->removeItem(0);
    }else{
        iterationTypeComboBox->insertItem(0,"NONE");
        iterationTypeComboBox->setCurrentIndex(0);
    }
}
void ClustalWWithExtFileSpecifySupportRunDialog::accept(){
    if(gapOpenCheckBox->isChecked()) {
        settings.gapOpenPenalty = gapOpenSpinBox->value();
    }
    if(gapExtCheckBox->isChecked()) {
        settings.gapExtenstionPenalty = gapExtSpinBox->value();
    }
    if(gapDistancesCheckBox->isChecked()){
        settings.gapDist = gapDistancesSpinBox->value();
    }
    if(residueSpecificGapsOffCheckBox->isChecked()){
        settings.noPGaps=true;

    }
    if(hydrophilicGapsOffCheckBox->isChecked()){
        settings.noHGaps=true;
    }
    if(endGapsCheckBox->isChecked()){
        settings.endGaps=true;
    }
    if(weightMatrixCheckBox->isChecked()){
        settings.matrix=weightMatrixComboBox->currentText();
    }
    if(iterationTypeCheckBox->isChecked()){
        settings.iterationType=iterationTypeComboBox->currentText();
        if(maxIterationsCheckBox->isChecked()){
            settings.numIterations=maxIterationsSpinBox->value();
        }
    }
    if(inputFileLineEdit->text().isEmpty()){
        QMessageBox::information(this, tr("Kalign with Align"),
                                 tr("Input file is not set!") );
    }else if(saveController->getSaveFileName().isEmpty()){
        QMessageBox::information(this, tr("Kalign with Align"),
                                 tr("Output file is not set!") );
    }
    else{
        settings.outputFilePath = saveController->getSaveFileName();
        settings.inputFilePath = inputFileLineEdit->text();
        QDialog::accept();
    }
}

}//namespace
