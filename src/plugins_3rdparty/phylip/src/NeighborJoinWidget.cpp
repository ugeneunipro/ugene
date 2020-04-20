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
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Settings.h>

#include "NeighborJoinWidget.h"
#include "dnadist.h"
#include "protdist.h"

namespace U2 {

#define DIST_MATRIX_MODEL_PATH "/model"
#define DIST_MATRIX_GAMMADISTR_PATH "/gamma_distr"
#define DIST_MATRIX_ALPHAFACTOR_PATH "/alfa_factor"
#define DIST_MATRIX_TRRATIO_PATH "/ttratio"
#define SEQ_BOOT_BOTTSTR_PATH "/bootstr"
#define SEQ_BOOT_REPL_PATH "/replicates"
#define SEQ_BOOT_SEED_PATH "/seq_boot_seed"
#define SEQ_BOOT_FRACTION_PATH "/fraction"
#define SEQ_BOOT_CONSENSUSID_PATH "/consensusID"

#define SEED_MIN 5
#define SEED_MAX 32765

QString ConsensusModelTypes::M1("M1");
QString ConsensusModelTypes::Strict("Strict");
QString ConsensusModelTypes::MajorityRuleExt("Majority Rule (extended)");
QString ConsensusModelTypes::MajorityRule("Majority Rule");

QList<QString> ConsensusModelTypes::getConsensusModelTypes() {
    static QList<QString> list;
    if (list.isEmpty()) {
        list.append(ConsensusModelTypes::MajorityRuleExt);
        list.append(ConsensusModelTypes::Strict);
        list.append(ConsensusModelTypes::MajorityRule);
        list.append(ConsensusModelTypes::M1);
    }

    return list;
}

NeighborJoinWidget::NeighborJoinWidget(const MultipleSequenceAlignment &ma, QWidget *parent) :
    CreatePhyTreeWidget(parent)
{
    setupUi(this);
    init(ma);
    connectSignals();
}

void NeighborJoinWidget::fillSettings(CreatePhyTreeSettings &settings) {
    settings.matrixId = cbModel->currentText();
    settings.useGammaDistributionRates = chbGamma->isChecked();
    settings.alphaFactor = sbAlpha->value();
    settings.ttRatio = transitionRatioSpinBox->value();
    settings.bootstrap = chbEnableBootstrapping->isChecked();
    settings.replicates = sbReplicatesNumber->value();
    settings.seed = sbSeed->value();
    settings.fraction = sbFraction->value();
    settings.consensusID = cbConsensusType->currentText();

    displayOptions->fillSettings(settings);
}

void NeighborJoinWidget::storeSettings() {
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_MODEL_PATH, cbModel->currentText());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_GAMMADISTR_PATH, chbGamma->isChecked());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_ALPHAFACTOR_PATH, sbAlpha->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_TRRATIO_PATH, transitionRatioSpinBox->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_BOTTSTR_PATH, chbEnableBootstrapping->isChecked());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_REPL_PATH, sbReplicatesNumber->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_SEED_PATH, sbSeed->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_FRACTION_PATH, sbFraction->value());
    AppContext::getSettings()->setValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_CONSENSUSID_PATH, cbConsensusType->currentText());

    displayOptions->storeSettings();
}

void NeighborJoinWidget::restoreDefault() {
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_MODEL_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_GAMMADISTR_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_ALPHAFACTOR_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_TRRATIO_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_BOTTSTR_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_REPL_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_SEED_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_FRACTION_PATH);
    AppContext::getSettings()->remove(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_CONSENSUSID_PATH);

    cbModel->setCurrentIndex(0);
    chbGamma->setChecked(false);
    sbAlpha->setValue(0.5);
    transitionRatioSpinBox->setValue(2.0);
    chbEnableBootstrapping->setChecked(false);
    sbReplicatesNumber->setValue(100);
    sbSeed->setValue(getRandomSeed());
    sbFraction->setValue(0.5);
    cbConsensusType->setCurrentIndex(0);

    displayOptions->restoreDefault();
}

bool NeighborJoinWidget::checkMemoryEstimation(QString &msg, const MultipleSequenceAlignment &msa, const CreatePhyTreeSettings &settings) {
    AppResourcePool *s = AppContext::getAppSettings()->getAppResourcePool();
    const qint64 appMemMb = s->getMaxMemorySizeInMB();

    //****description******
    //dnadist_makevalues()
//     for (i = 0; i < spp; i++) {
//         nodep[i]->x = (phenotype)Malloc(endsite*sizeof(ratelike));
//         for (j = 0; j < endsite; j++)
//             nodep[i]->x[j]  = (ratelike)Malloc(rcategs*sizeof(sitelike));
//     }

    //rcategs = 1
    //sizeof(sitelike) = 32
    //sizeof(ratelike) = 4

    const qint64 spp = msa->getNumRows();
    const qint64 endsite = msa->getLength();
    const qint64 ugeneLowestMemoryUsageMb = 50;
    const qint64 minMemoryForDistanceMatrixMb = (qint64)(spp * endsite * 32 + endsite * 4) / (1024 * 1024);

    if (minMemoryForDistanceMatrixMb > appMemMb - ugeneLowestMemoryUsageMb) {
        msg = tr("Probably, for that alignment there is no enough memory to run PHYLIP dnadist module."
            "The module will require more than %1 MB in the estimation."
            "\nIt could cause an error. Do you want to continue?").arg(minMemoryForDistanceMatrixMb);
        return false;
    } else {
        return displayOptions->checkMemoryEstimation(msg, msa, settings);
    }
}

bool NeighborJoinWidget::checkSettings(QString &msg, const CreatePhyTreeSettings &settings) {
    if (!((settings.seed >= SEED_MIN) && (settings.seed <= SEED_MAX) && (settings.seed%2 == 1))) {
        msg = tr("Seed must be odd");
        return false;
    }

    return displayOptions->checkSettings(msg, settings);
}

void NeighborJoinWidget::sl_onMatrixModelChanged(const QString &matrixModelName) {
    transitionRatioSpinBox->setEnabled(matrixModelName == DNADistModelTypes::F84 || matrixModelName == DNADistModelTypes::Kimura);
}

void NeighborJoinWidget::sl_onConsensusTypeChanged(const QString &consensusTypeName) {
    const bool isM1 = (consensusTypeName == ConsensusModelTypes::M1);
    lblFraction->setEnabled(isM1);
    sbFraction->setEnabled(isM1);

    if (consensusTypeName == ConsensusModelTypes::Strict) {
        sbFraction->setValue(1.0);
    }

    if (consensusTypeName == ConsensusModelTypes::MajorityRule) {
        sbFraction->setValue(0.5);
    }

    if (consensusTypeName == ConsensusModelTypes::MajorityRuleExt) {
        sbFraction->setValue(0.5);
    }
}

void NeighborJoinWidget::init(const MultipleSequenceAlignment &ma) {
    const DNAAlphabetType alphabetType = ma->getAlphabet()->getType();
    if ((alphabetType == DNAAlphabet_RAW) || (alphabetType == DNAAlphabet_NUCL)) {
        cbModel->addItems(DNADistModelTypes::getDNADistModelTypes());
    } else {
        cbModel->addItems(ProtDistModelTypes::getProtDistModelTypes());
    }

    const QString matrixModelName = AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_MODEL_PATH, cbModel->itemText(0)).toString();
    cbModel->setCurrentIndex(cbModel->findText(matrixModelName));
    sl_onMatrixModelChanged(matrixModelName);

    chbGamma->setChecked(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_GAMMADISTR_PATH, false).toBool());
    sbAlpha->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_ALPHAFACTOR_PATH, 0.5).toDouble());
    transitionRatioSpinBox->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + DIST_MATRIX_TRRATIO_PATH, 2.0).toDouble());

    sbSeed->setValue(getRandomSeed());

    cbConsensusType->addItems(ConsensusModelTypes::getConsensusModelTypes());

    chbEnableBootstrapping->setChecked(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_BOTTSTR_PATH, false).toBool());

    sbReplicatesNumber->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_REPL_PATH, 100).toInt());
    sbSeed->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_SEED_PATH, getRandomSeed()).toInt());
    sbFraction->setValue(AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_FRACTION_PATH, 0.5).toDouble());

    const QString consensusTypeName = AppContext::getSettings()->getValue(CreatePhyTreeWidget::settingsPath() + SEQ_BOOT_CONSENSUSID_PATH, cbConsensusType->itemText(0)).toString();
    cbConsensusType->setCurrentIndex(cbConsensusType->findText(consensusTypeName));
    sl_onConsensusTypeChanged(consensusTypeName);
}

void NeighborJoinWidget::connectSignals() {
    connect(cbModel, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_onMatrixModelChanged(const QString &)));
    connect(cbConsensusType, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_onConsensusTypeChanged(const QString &)));
}

int NeighborJoinWidget::getRandomSeed() {
    int seed = 0;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    seed = qAbs(qrand());

    while (!checkSeed(seed)) {
        seed++;
        if (seed >= SEED_MAX) {
            seed = SEED_MIN;
        }
    }

    return seed;
}

bool NeighborJoinWidget::checkSeed(int seed) {
    return (seed >= SEED_MIN) && (seed <= SEED_MAX) && ((seed - 1) % 4 == 0);
}

}   // namespace U2
