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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectModel.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>

#include "blast_plus/BlastPlusWorker.h"

#include "BlastRunCommonDialog.h"

namespace U2 {

using namespace LocalWorkflow;


////////////////////////////////////////
//BlastAllSupportRunCommonDialog
BlastRunCommonDialog::BlastRunCommonDialog(QWidget *parent, BlastType blastType, bool useCompValues, QStringList compValues)
: QDialog(parent), ca_c(NULL), useCompValues(useCompValues), compValues(compValues)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749012");
    buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Restore to default"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QString hitsToolTip;
    switch (blastType) {
        case BlastPlus:
            hitsLabel->setText(BlastPlusWorkerFactory::getHitsName() + ":");
            hitsToolTip = BlastPlusWorkerFactory::getHitsDescription();
            break;
        default:
            FAIL("Unknown BLAST type", );
    }
    dbSelector = new BlastDBSelectorWidgetController(this);
    dbSelectorWidget->layout()->addWidget(dbSelector);
    hitsLabel->setToolTip(hitsToolTip);
    numberOfHitsSpinBox->setToolTip(hitsToolTip);

    optionsTab->setCurrentIndex(0);

    //I don`t know what this in local BLAST
    phiPatternEdit->hide();
    phiPatternLabel->hide();
    //set avaliable number of threads
    numberOfCPUSpinBox->setMaximum(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    numberOfCPUSpinBox->setValue(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    //Connecting people
    connect(programName,SIGNAL(currentIndexChanged(int)),SLOT(sl_onProgNameChange(int)));
    connect(matrixComboBox,SIGNAL(currentIndexChanged(int)),SLOT(sl_onMatrixChanged(int)));
    sl_onMatrixChanged(0);

    connect(scoresComboBox,SIGNAL(currentIndexChanged(int)),SLOT(sl_onMatchScoresChanged(int)));
    sl_onMatchScoresChanged(0);
    connect(megablastCheckBox,SIGNAL(toggled(bool)),SLOT(sl_megablastChecked()));

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    restoreButton = buttonBox->button(QDialogButtonBox::Yes);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(okButton,SIGNAL(clicked()),SLOT(sl_runQuery()));
    connect(restoreButton,SIGNAL(clicked()),SLOT(sl_restoreDefault()));
    sl_onProgNameChange(0);
    okButton->setEnabled(false);

    connect(compStatsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onCompStatsChanged()));
    connect(dbSelector, SIGNAL(si_dbChanged()), SLOT(sl_lineEditChanged()));
    setupCompositionBasedStatistics();
    sl_onCompStatsChanged();
}

void BlastRunCommonDialog::setupCompositionBasedStatistics() {
    bool visible = useCompValues && compValues.contains(programName->currentText());
    compStatsLabel->setVisible(visible);
    compStatsComboBox->setVisible(visible);
}

const BlastTaskSettings &BlastRunCommonDialog::getSettings() const {
    return settings;
}
void BlastRunCommonDialog::sl_onMatchScoresChanged(int index){
    Q_UNUSED(index);
    if(programName->currentText() != "blastn"){
        return;
    }
    settings.matchReward=scoresComboBox->currentText().split(" ").at(0).toInt();
    settings.mismatchPenalty=scoresComboBox->currentText().split(" ").at(1).toInt();
    //For help see http://www.ncbi.nlm.nih.gov/staff/tao/URLAPI/blastall/blastall_node76.html
    //Last values is default
    if((scoresComboBox->currentText() == "1 -4") ||
        (scoresComboBox->currentText() == "1 -3")){//-G 1 -E 2; -G 0 -E 2;-G 2 -E 1; -G 1 -E 1; -G 2 -E 2
        costsComboBox->clear();
        costsComboBox->addItem("2 2");
        costsComboBox->addItem("1 2");
        costsComboBox->addItem("0 2");
        costsComboBox->addItem("2 1");
        costsComboBox->addItem("1 1");
    }else if(scoresComboBox->currentText() == "1 -2"){//-G 1 -E 2; -G 0 -E 2; -G 3 -E 1; -G 2 -E 1; -G 1 -E 1; -G 2 -E 2
        costsComboBox->clear();
        costsComboBox->addItem("2 2");
        costsComboBox->addItem("1 2");
        costsComboBox->addItem("0 2");
        costsComboBox->addItem("3 1");
        costsComboBox->addItem("2 1");
        costsComboBox->addItem("1 1");
    }else if(scoresComboBox->currentText() == "1 -1"){//-G 3 -E 2; -G 2 -E 2; -G 1 -E 2; -G 0 -E 2; -G 4 -E 1; -G 3 -E 1; -G 2 -E 1; -G 4 -E 2  :Not supported megablast
        costsComboBox->clear();
        costsComboBox->addItem("4 2");
        costsComboBox->addItem("3 2");
        costsComboBox->addItem("2 2");
        costsComboBox->addItem("1 2");
        costsComboBox->addItem("0 2");
        costsComboBox->addItem("4 1");
        costsComboBox->addItem("3 1");
        costsComboBox->addItem("2 1");
    }else if((scoresComboBox->currentText() == "2 -7") ||
             (scoresComboBox->currentText() == "2 -5")){//-G 2 -E 4; -G 0 -E 4; -G 4 -E 2; -G 2 -E 2; -G 4 -E 4
        costsComboBox->clear();
        costsComboBox->addItem("4 4");
        costsComboBox->addItem("2 4");
        costsComboBox->addItem("0 4");
        costsComboBox->addItem("4 2");
        costsComboBox->addItem("2 2");
    }else if(scoresComboBox->currentText() == "2 -3"){//-G 4 -E 4; -G 2 -E 4; -G 0 -E 4; -G 3 -E 3; -G 6 -E 2; -G 5 -E 2; -G 4 -E 2; -G 2 -E 2, -G 6 -E 4
        costsComboBox->clear();
        costsComboBox->addItem("6 4");
        costsComboBox->addItem("4 4");
        costsComboBox->addItem("2 4");
        costsComboBox->addItem("0 4");
        costsComboBox->addItem("3 3");
        costsComboBox->addItem("6 2");
        costsComboBox->addItem("5 2");
        costsComboBox->addItem("4 2");
        costsComboBox->addItem("2 2");
    }else if((scoresComboBox->currentText() == "4 -5") ||
             (scoresComboBox->currentText() == "5 -4")){//-G 6 -E 5; -G 5 -E 5; -G 4 -E 5; -G 3 -E 5; -G 12 -E 8
        costsComboBox->clear();
        costsComboBox->addItem("12 8");
        costsComboBox->addItem("6 5");
        costsComboBox->addItem("5 5");
        costsComboBox->addItem("4 5");
        costsComboBox->addItem("3 5");
    }else{
        assert(0);
    }
}

void BlastRunCommonDialog::sl_onMatrixChanged(int index){
    Q_UNUSED(index);
    if(programName->currentText() == "blastn"){
        return;
    }
    settings.matrix=matrixComboBox->currentText();
    //For help see http://www.ncbi.nlm.nih.gov/staff/tao/URLAPI/blastall/blastall_node77.html
    //Last values is default
    if(matrixComboBox->currentText() == "PAM30"){//-G 5 -E 2; -G 6 -E 2; -G 7 -E 2; -G 8 -E 1; -G 10 -E 1; -G 9 -E 1
        costsComboBox->clear();
        costsComboBox->addItem("9 1");
        costsComboBox->addItem("5 2");
        costsComboBox->addItem("6 2");
        costsComboBox->addItem("7 2");
        costsComboBox->addItem("8 1");
        costsComboBox->addItem("10 1");
    }else if(matrixComboBox->currentText() == "PAM70"){//-G 6 -E 2; -G 7 -E 2; -G 8 -E 2; -G 9 -E 2; -G 11 -E 1; -G 10 -E 1
        costsComboBox->clear();
        costsComboBox->addItem("10 1");
        costsComboBox->addItem("6 2");
        costsComboBox->addItem("7 2");
        costsComboBox->addItem("8 2");
        costsComboBox->addItem("9 2");
        costsComboBox->addItem("11 1");
    }else if(matrixComboBox->currentText() == "BLOSUM45"){//-G 10 -E 3; -G 11 -E 3; -G 12 -E 3; -G 12 -E 2; -G 13 -E 2, -G 14 -E 2;
                                                            //-G 16 -E 2; -G 15 -E 1; -G 16 -E 1; -G 17 -E 1; -G 18 -E 1; -G 19 -E 1; -G 15 -E 2
        costsComboBox->clear();
        costsComboBox->addItem("15 2");
        costsComboBox->addItem("10 3");
        costsComboBox->addItem("11 3");
        costsComboBox->addItem("12 3");
        costsComboBox->addItem("12 2");
        costsComboBox->addItem("13 2");
        costsComboBox->addItem("14 2");
        costsComboBox->addItem("16 2");
        costsComboBox->addItem("15 1");
        costsComboBox->addItem("16 1");
        costsComboBox->addItem("17 1");
        costsComboBox->addItem("18 1");
        costsComboBox->addItem("19 1");
    }else if(matrixComboBox->currentText() == "BLOSUM62"){//-G 7 -E 2; -G 8 -E 2; -G 9 -E 2; -G 10 -E 1; -G 12 -E 1; -G 11 -E 1
        costsComboBox->clear();
        costsComboBox->addItem("11 1");
        costsComboBox->addItem("7 2");
        costsComboBox->addItem("8 2");
        costsComboBox->addItem("9 2");
        costsComboBox->addItem("10 1");
        costsComboBox->addItem("12 1");
    }else if(matrixComboBox->currentText() == "BLOSUM80"){//-G 6 -E 2; -G 7 -E 2; -G 8 -E 2; -G 9 -E 1; -G 11 -E 1; -G 10 -E 1
        costsComboBox->clear();
        costsComboBox->addItem("10 1");
        costsComboBox->addItem("6 2");
        costsComboBox->addItem("7 2");
        costsComboBox->addItem("8 2");
        costsComboBox->addItem("9 1");
        costsComboBox->addItem("11 1");
    }else{
        assert(0);
    }
}

void BlastRunCommonDialog::sl_restoreDefault(){
    needRestoreDefault=true;
//    sl_onProgNameChange(0);
    megablastCheckBox->setChecked(false);
    sl_megablastChecked();
    sl_onMatchScoresChanged(0);
    sl_onMatrixChanged(0);
    evalueSpinBox->setValue(10);
    needRestoreDefault=false;
    bothStrandsButton->setChecked(true);
}
void BlastRunCommonDialog::sl_megablastChecked(){
    if(megablastCheckBox->isChecked()){
        if(wordSizeSpinBox->value()<12 || needRestoreDefault){
            wordSizeSpinBox->setValue(28);
        }
        wordSizeSpinBox->setMaximum(100);
        wordSizeSpinBox->setMinimum(12);
        xDropoffGASpinBox->setValue(20);
        xDropoffUnGASpinBox->setValue(20);
    }else{
        if(wordSizeSpinBox->value()<7 || needRestoreDefault){
            wordSizeSpinBox->setValue(11);
        }
        wordSizeSpinBox->setMaximum(100);
        wordSizeSpinBox->setMinimum(7);
        xDropoffGASpinBox->setValue(30);
        xDropoffUnGASpinBox->setValue(10);
    }
}

void BlastRunCommonDialog::sl_onProgNameChange(int index){
    Q_UNUSED(index);
    setupCompositionBasedStatistics();
    settings.programName=programName->currentText();
    if(programName->currentText() == "blastn"){//nucl
        programName->setToolTip(tr("Direct nucleotide alignment"));
        gappedAlignmentCheckBox->setEnabled(true);thresholdSpinBox->setValue(0);
    }else if(programName->currentText() == "blastp"){//amino
        programName->setToolTip(tr("Direct protein alignment"));
        gappedAlignmentCheckBox->setEnabled(true);
        thresholdSpinBox->setValue(11);
    }else if(programName->currentText() == "gpu-blastp"){//amino
        programName->setToolTip(tr("Direct protein alignment (on GPU)"));
        gappedAlignmentCheckBox->setEnabled(true);
        thresholdSpinBox->setValue(11);
    }else if(programName->currentText() == "blastx"){//nucl
        programName->setToolTip(tr("Protein alignment, input nucleotide is translated input protein before the search"));
        gappedAlignmentCheckBox->setEnabled(true);
        thresholdSpinBox->setValue(12);
    }else if(programName->currentText() == "tblastn"){//amino
        programName->setToolTip(tr("Protein alignment, nucleotide database is translated input protein before the search"));
        gappedAlignmentCheckBox->setEnabled(true);
        thresholdSpinBox->setValue(13);
    }else if(programName->currentText() == "tblastx"){//nucl
        programName->setToolTip(tr("Protein alignment, both input query and database are translated before the search"));
        gappedAlignmentCheckBox->setEnabled(false);
        thresholdSpinBox->setValue(13);
    }else{
        assert(0);
    }
    enableStrandBox( (programName->currentText() == "blastn") || ( programName->currentText().contains("blastx") ) );

    if (programName->currentText() == "tblastx") {
        costsLabel->hide();
        costsComboBox->hide();
    } else {
        costsLabel->show();
        costsComboBox->show();
    }
    if(programName->currentText() == "blastn"){
        megablastCheckBox->setEnabled(true);
        if(megablastCheckBox->isChecked()){
            wordSizeSpinBox->setValue(28);
            wordSizeSpinBox->setMaximum(100);
            wordSizeSpinBox->setMinimum(12);
        }else{
            wordSizeSpinBox->setValue(11);
            wordSizeSpinBox->setMaximum(100);
            wordSizeSpinBox->setMinimum(7);
        }
        windowSizeSpinBox->setValue(0);

        matchScoreLabel->show();
        scoresComboBox->show();
        matrixLabel->hide();
        matrixComboBox->hide();
        serviceLabel->hide();
        serviceComboBox->hide();
        thresholdSpinBox->hide();
        thresholdLabel->hide();
        sl_onMatchScoresChanged(0);
    }else{
        megablastCheckBox->setEnabled(false);
        wordSizeSpinBox->setValue(3);
        wordSizeSpinBox->setMaximum(3);
        wordSizeSpinBox->setMinimum(2);
        windowSizeSpinBox->setValue(40);

        matchScoreLabel->hide();
        scoresComboBox->hide();
        matrixLabel->show();
        matrixComboBox->show();
        serviceLabel->show();
        serviceComboBox->show();
        thresholdSpinBox->show();
        thresholdLabel->show();
        sl_onMatrixChanged(0);
    }
    //set X dropoff values
    if(programName->currentText() == "blastn"){
        megablastCheckBox->setEnabled(true);
        if(megablastCheckBox->isChecked()){
            xDropoffGASpinBox->setValue(20);
            xDropoffUnGASpinBox->setValue(10);
        }else{
            xDropoffGASpinBox->setValue(30);
            xDropoffUnGASpinBox->setValue(20);
        }
        xDropoffFGASpinBox->setValue(100);
        xDropoffGASpinBox->setEnabled(true);
        xDropoffFGASpinBox->setEnabled(true);
    }else if (programName->currentText() == "tblastx"){
        xDropoffGASpinBox->setValue(0);
        xDropoffGASpinBox->setEnabled(false);
        xDropoffUnGASpinBox->setValue(7);
        xDropoffFGASpinBox->setValue(0);
        xDropoffFGASpinBox->setEnabled(false);
    }else{
        xDropoffGASpinBox->setValue(15);
        xDropoffUnGASpinBox->setValue(7);
        xDropoffFGASpinBox->setValue(25);
        xDropoffGASpinBox->setEnabled(true);
        xDropoffFGASpinBox->setEnabled(true);
    }
}

void BlastRunCommonDialog::sl_onCompStatsChanged() {
    QString value = compStatsComboBox->currentText();
    settings.compStats = value.left(1);
}

void BlastRunCommonDialog::getSettings(BlastTaskSettings &localSettings){
    localSettings.programName=programName->currentText();
    localSettings.databaseNameAndPath = dbSelector->databasePathLineEdit->text() + "/" + dbSelector->baseNameLineEdit->text();
    localSettings.expectValue=evalueSpinBox->value();
    localSettings.wordSize=wordSizeSpinBox->value();
    localSettings.megablast=megablastCheckBox->isChecked();
    localSettings.numberOfHits=numberOfHitsSpinBox->value();
    localSettings.numberOfProcessors=numberOfCPUSpinBox->value();

    if (directStrandButton->isChecked()) {
        settings.directStrand = TriState_Yes;
    } else if (complStrandButton->isChecked()) {
        settings.directStrand = TriState_No;
    } else {
        settings.directStrand = TriState_Unknown;
    }

    localSettings.gapOpenCost=costsComboBox->currentText().split(" ").at(0).toInt();
    localSettings.gapExtendCost=costsComboBox->currentText().split(" ").at(1).toInt();
    //setup filters
    if(lowComplexityFilterCheckBox->isChecked()){
        localSettings.filter="L";
    }
    if(repeatsCheckBox->isChecked()){
        localSettings.filter=localSettings.filter.isEmpty() ? "R" : localSettings.filter+"; R";
    }
//    if(lowerCaseCheckBox->isChecked()){
//        settings.filter=settings.filter.isEmpty() ? "???" : settings.filter+"; ???";
//    }
    if(lookupMaskCheckBox->isChecked()){
        localSettings.filter=localSettings.filter.isEmpty() ? "m" : "m "+localSettings.filter;
    }

    if(localSettings.isNucleotideSeq){
        if((((scoresComboBox->currentText() == "1 -4") || (scoresComboBox->currentText() == "1 -3")) && costsComboBox->currentText()=="2 2") || //-G 2 -E 2
            ((scoresComboBox->currentText() == "1 -2") && costsComboBox->currentText()=="2 2") || //-G 2 -E 2
            ((scoresComboBox->currentText() == "1 -1") && costsComboBox->currentText()=="4 2") || //-G 4 -E 2
            (((scoresComboBox->currentText() == "2 -7") || (scoresComboBox->currentText() == "2 -5"))&& costsComboBox->currentText()=="4 4") || //-G 4 -E 4
            ((scoresComboBox->currentText() == "2 -3") && costsComboBox->currentText()=="6 4") || //-G 6 -E 4
            (((scoresComboBox->currentText() == "4 -5") || (scoresComboBox->currentText() == "5 -4")) && costsComboBox->currentText()=="12 8"))//-G 12 -E 8
        {
            localSettings.isDefaultCosts=true;
        }else{
            localSettings.isDefaultCosts=false;
        }
        localSettings.isDefautScores=(scoresComboBox->currentText() == "1 -3");
    }else{
        if(((matrixComboBox->currentText() == "PAM30") && costsComboBox->currentText()=="9 1") || //-G 9 -E 1
                ((matrixComboBox->currentText() == "PAM70") && costsComboBox->currentText()=="10 1") || //-G 10 -E 1
                ((matrixComboBox->currentText() == "BLOSUM45") && costsComboBox->currentText()=="15 2") ||
                ((matrixComboBox->currentText() == "BLOSUM62") && costsComboBox->currentText()=="11 1") ||
                ((matrixComboBox->currentText() == "BLOSUM80") && costsComboBox->currentText()=="10 1"))
        {
            localSettings.isDefaultCosts=true;
        }else{
            localSettings.isDefaultCosts=false;
        }
        localSettings.isDefaultMatrix=(matrixComboBox->currentText() == "BLOSUM62");
    }
    localSettings.isGappedAlignment=gappedAlignmentCheckBox->isChecked();
    localSettings.windowSize=windowSizeSpinBox->value();
    localSettings.threshold=thresholdSpinBox->value();
    localSettings.xDropoffGA=xDropoffGASpinBox->value();
    localSettings.xDropoffUnGA=xDropoffUnGASpinBox->value();
    localSettings.xDropoffFGA=xDropoffFGASpinBox->value();
    if((localSettings.programName == "blastn" && localSettings.threshold != 0) ||
            (localSettings.programName == "blastp" && localSettings.threshold != 11) ||
            (localSettings.programName == "gpu-blastp" && localSettings.threshold != 11) ||
            (localSettings.programName == "blastx" && localSettings.threshold != 12) ||
            (localSettings.programName == "tblastn" && localSettings.threshold != 13) ||
            (localSettings.programName == "tblastx" && localSettings.threshold != 13)){
        localSettings.isDefaultThreshold=false;
    }
    if (useCompValues && compValues.contains(settings.programName)) {
        localSettings.compStats = settings.compStats;
    }
}

void BlastRunCommonDialog::enableStrandBox( bool enable )
{
    bothStrandsButton->setEnabled(enable);
    directStrandButton->setEnabled(enable);
    complStrandButton->setEnabled(enable);
}

}//namespace
