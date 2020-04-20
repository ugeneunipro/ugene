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

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "MrBayesDialogWidget.h"
#include "MrBayesSupport.h"

namespace U2 {

#define MR_BAYES_MODEL_TYPE "/mb_model_t"
#define MR_BAYES_RATE_VATIATION "/mb_rate_var"
#define MR_BAYES_GAMMA "/mb_gamma"
#define MR_BAYES_CHAIN_LENGTH "/mb_chain_length"
#define MR_BAYES_SUBSEMPL_FREQ "/mb_subsempl_freq"
#define MR_BAYES_BURNIN "/mb_burnin"
#define MR_BAYES_HEATED "/mb_heated"
#define MR_BAYES_TEMPR "/mb_tempr"
#define MR_BAYES_SEED "/mb_seed"

MrBayesWidget::MrBayesWidget(const MultipleSequenceAlignment &ma, QWidget *parent) :
    CreatePhyTreeWidget(parent)
{
    setupUi(this);
    DNAAlphabetType alphabetType = ma->getAlphabet()->getType();
    if ((alphabetType == DNAAlphabet_RAW) || (alphabetType == DNAAlphabet_NUCL)){
        isAminoAcidAlphabet = false;
        modelLabel1->setText(tr("Substitution model"));
        modelTypeCombo->addItems( MrBayesModelTypes::getSubstitutionModelTypes() );
    } else {
        isAminoAcidAlphabet = true;
        modelLabel1->setText(tr("Rate Matrix (fixed)"));
        modelTypeCombo->addItems( MrBayesModelTypes::getAAModelTypes() );
    }

    connect(rateVariationCombo, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_onRateChanged(const QString&)));
    rateVariationCombo->addItems(MrBayesVariationTypes::getVariationTypes());

    seedSpin->setValue(getRandomSeed());

    gammaCategoriesSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_GAMMA, 4).toInt());
    ngenSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_CHAIN_LENGTH, 10000).toInt());
    sfreqSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SUBSEMPL_FREQ, 1000).toInt());
    burninSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_BURNIN, 10).toInt());
    nheatedSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_HEATED, 4).toInt());
    tempSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_TEMPR, 0.4).toDouble());
    seedSpin->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SEED, getRandomSeed()).toInt());

    QString comboText = AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_MODEL_TYPE, isAminoAcidAlphabet ? modelTypeCombo->itemText(0) : MrBayesModelTypes::HKY85).toString();
    setComboText(modelTypeCombo, comboText);

    comboText = AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_RATE_VATIATION, MrBayesVariationTypes::gamma).toString();
    setComboText(rateVariationCombo, comboText);
}

void MrBayesWidget::sl_onRateChanged(const QString& modelName){
    if(modelName == MrBayesVariationTypes::equal || modelName == MrBayesVariationTypes::propinv){
        gammaCategoriesSpin->setEnabled(false);
    }else{
        gammaCategoriesSpin->setEnabled(true);
    }
}

void MrBayesWidget::fillSettings(CreatePhyTreeSettings& settings) {
    settings.mb_ngen = ngenSpin->value();
    settings.mrBayesSettingsScript = generateMrBayesSettingsScript();
    displayOptions->fillSettings(settings);
}

void MrBayesWidget::storeSettings(){
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_MODEL_TYPE, modelTypeCombo->currentText());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_RATE_VATIATION, rateVariationCombo->currentText());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + MR_BAYES_GAMMA, gammaCategoriesSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_CHAIN_LENGTH, ngenSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SUBSEMPL_FREQ, sfreqSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_BURNIN, burninSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_HEATED, nheatedSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_TEMPR, tempSpin->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SEED, seedSpin->value());
    displayOptions->storeSettings();
}
void MrBayesWidget::restoreDefault(){
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + MR_BAYES_MODEL_TYPE);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + MR_BAYES_RATE_VATIATION);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + MR_BAYES_GAMMA);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_CHAIN_LENGTH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SUBSEMPL_FREQ);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_BURNIN);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_HEATED);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_TEMPR);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() +  MR_BAYES_SEED);

    setComboText(modelTypeCombo, MrBayesModelTypes::HKY85);
    setComboText(rateVariationCombo, MrBayesVariationTypes::gamma);
    gammaCategoriesSpin->setValue(4);
    ngenSpin->setValue(10000);
    sfreqSpin->setValue(1000);
    burninSpin->setValue(10);
    nheatedSpin->setValue(4);
    tempSpin->setValue(0.4);
    seedSpin->setValue(getRandomSeed());

    displayOptions->restoreDefault();
}

void MrBayesWidget::setComboText(QComboBox* combo, const QString& text){
    for(int i = 0; i<combo->count(); i++){
        if(combo->itemText(i) == text){
            combo->setCurrentIndex(i);
            break;
        }
    }
}

#define SEED_MIN 5
#define SEED_MAX 32765

int MrBayesWidget::getRandomSeed(){
    int seed = 0;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    seed = qAbs(qrand());

    while(!((seed >= SEED_MIN) && (seed <=SEED_MAX))){
        seed++;
        if(seed >=SEED_MAX){
            seed = SEED_MIN;
        }
    }

    return seed;
}

bool MrBayesWidget::checkSettings(QString &messsage, const CreatePhyTreeSettings &settings) {
    //Check that MrBayes and tempory folder path defined
    ExternalToolRegistry* reg = AppContext::getExternalToolRegistry();
    ExternalTool* mb= reg->getById(MrBayesSupport::ET_MRBAYES_ID);
    assert(mb);
    const QString& path = mb->getPath();
    const QString& name = mb->getName();
    if (path.isEmpty()){
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle(name);
        msgBox->setText(tr("Path for %1 tool not selected.").arg(name));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        switch (ret) {
           case QMessageBox::Yes:
               AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
               break;
           case QMessageBox::No:
               return false;
               break;
           default:
               assert(false);
               break;
        }
    }
    if (path.isEmpty()){
        return false;
    }
    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, false);

    return displayOptions->checkSettings(messsage, settings);
}

QString MrBayesWidget::generateMrBayesSettingsScript(){
    QString script;
    script.append("Begin MrBayes;\n");

    script.append("lset ");
    if(!isAminoAcidAlphabet){
        int nst = 0;

        QString currentNst = modelTypeCombo->currentText();
        if(currentNst == MrBayesModelTypes::JC69){
            nst = 1;
        }else if(currentNst == MrBayesModelTypes::HKY85){
            nst = 2;
        }else if(currentNst == MrBayesModelTypes::GTR){
            nst = 6;
        }else{
            nst = 2; //by default
        }

        script = script.append("Nst=%1 ").arg(nst);
    }
    QString rates = rateVariationCombo->currentText();
    script = script.append("rates=%1").arg(rates);
    if(rates == MrBayesVariationTypes::gamma || rates == MrBayesVariationTypes::invgamma){
        int gvar = gammaCategoriesSpin->value();
        script = script.append(" ngammacat=%1 ").arg(gvar);
    }
    script.append(";\n");

    if(isAminoAcidAlphabet){
        QString model = modelTypeCombo->currentText();
        script = script.append("prset aamodelpr=fixed(%1);\n").arg(model);
    }

    int ngen, sfreq, printfreq, nchains, seed, burnin;
    double temp;
    ngen = ngenSpin->value();
    sfreq = sfreqSpin->value();
    printfreq = 1000;
    nchains = nheatedSpin->value();
    seed = seedSpin->value();
    temp = tempSpin->value();
    burnin = burninSpin->value();

    script = script.append("mcmc ngen=%1 samplefreq=%2 printfreq=%3 nchains=%4 temp=%5 savebrlens=yes "
        "starttree=random;\n").arg(ngen).arg(sfreq).arg(printfreq).arg(nchains).arg(temp).arg(seed);

    if(sfreq < burnin) burnin = 0;
    script = script.append("sumt burnin=%1;\n").arg(burnin);

    script = script.append("set seed=%1;\n").arg(seed);

    script.append("End;\n");
    return script;
}

}//namespace
