/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PhyMLDialogWidget.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/FileFilters.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "PhyMLSupport.h"

namespace U2 {

const QString PhyMlSettingsPrefixes::AminoAcidModelType(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_amino_acid_model_t");
const QString PhyMlSettingsPrefixes::DnaModelType(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_dna_model_t");
const QString PhyMlSettingsPrefixes::OptimiseEquilibriumFreq(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_eq_freq_flag");

const QString PhyMlSettingsPrefixes::EstimateTtRatio(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_est_trans_ratio");
const QString PhyMlSettingsPrefixes::TtRatio(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_trans_ratio");

const QString PhyMlSettingsPrefixes::EstimateSitesProportion(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_est_sites_prop");
const QString PhyMlSettingsPrefixes::InvariableSitesProportion(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_sites_proportion");

const QString PhyMlSettingsPrefixes::EstimateGammaFactor(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_est_gamma");
const QString PhyMlSettingsPrefixes::GammaFactor(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_gamma");

const QString PhyMlSettingsPrefixes::UseBootstrap(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_use_boot");
const QString PhyMlSettingsPrefixes::BootstrapReplicatesNumber(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_boot_repl");

const QString PhyMlSettingsPrefixes::UseFastMethod(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_use_fast");
const QString PhyMlSettingsPrefixes::FastMethodIndex(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_fast_method");

const QString PhyMlSettingsPrefixes::SubRatesNumber(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_sub_rates");
const QString PhyMlSettingsPrefixes::OptimiseTopology(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_opt_topol");
const QString PhyMlSettingsPrefixes::OptimiseBranchLengths(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_opt_lengths");

const QString PhyMlSettingsPrefixes::TreeImprovementType(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_tree_impr");
const QString PhyMlSettingsPrefixes::TreeSearchingType(CreatePhyTreeWidget::getAppSettingsRoot() + "/phyml_search");
const QString PhyMlSettingsPrefixes::UserTreePath(CreatePhyTreeWidget::getAppSettingsRoot() + "/user_tree");

PhyMlWidget::PhyMlWidget(const MultipleSequenceAlignment& ma, QWidget* parent)
    : CreatePhyTreeWidget(parent),
      isTtRationFixed(false),
      isTreeNumberSet(false) {
    setupUi(this);

    isAminoAcid = ma->getAlphabet()->isAmino();
    makeTTRatioControlsAvailable(!isAminoAcid);
    fillComboBoxes();

    createWidgetsControllers();

    widgetControllers.getDataFromSettings();
    sl_checkSubModelType(subModelCombo->currentText());

    connect(subModelCombo, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_checkSubModelType(const QString&)));
    connect(treeTypesCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_checkUserTreeType(int)));
    connect(treeImprovementsCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_checkTreeImprovement(int)));
    connect(inputFilePathButton, SIGNAL(clicked()), SLOT(sl_inputPathButtonClicked()));
    connect(optTopologyCheckbox, SIGNAL(clicked(bool)), SLOT(sl_optTopologyCheckboxClicked(bool)));

    optBranchCheckboxSavedState = optBranchCheckbox->isChecked();
    sl_optTopologyCheckboxClicked(optTopologyCheckbox->isChecked());
    sl_checkUserTreeType(treeTypesCombo->currentIndex());
}

void PhyMlWidget::fillComboBoxes() {
    subModelCombo->addItems(isAminoAcid ? PhyMLModelTypes::getAminoAcidModelTypes() : PhyMLModelTypes::getDnaModelTypes());
    fastMethodCombo->addItems(PhyMLRatioTestsTypes::ratioTestsTypes);
    fastMethodCombo->setCurrentIndex(PhyMLRatioTestsTypes::defaultRatioTestsTypeIndex);
    treeTypesCombo->addItems(TreeSearchingParams::getInputTreeTypes());
    treeImprovementsCombo->addItems(TreeSearchingParams::getTreeImprovementTypes());
}

void PhyMlWidget::makeTTRatioControlsAvailable(bool enabled) {
    transLabel->setEnabled(enabled);
    transEstimatedRb->setEnabled(enabled);
    transFixedRb->setEnabled(enabled);
    tranSpinBox->setEnabled(enabled && !transEstimatedRb->isChecked());
}

void PhyMlWidget::makeTTRatioControlsAvailable(SubstModelTrRatioType ttRatioType) {
    const bool shouldSavePreviousValue = transLabel->isEnabled();
    makeTTRatioControlsAvailable(true);

    switch (ttRatioType) {
        case ANY_TT_RATIO:
            if (isTtRationFixed) {
                transFixedRb->setChecked(true);
            } else {
                transEstimatedRb->setChecked(true);
            }
            break;
        case ONLY_FIXED_TT_RATIO:
            if (shouldSavePreviousValue) {
                isTtRationFixed = transFixedRb->isChecked();
            }
            transFixedRb->setChecked(true);
            transEstimatedRb->setEnabled(false);
            transFixedRb->setEnabled(false);
            break;
        case ONLY_ESTIMATED_TT_RATIO:
            if (shouldSavePreviousValue) {
                isTtRationFixed = transFixedRb->isChecked();
            }
            transEstimatedRb->setChecked(true);
            transEstimatedRb->setEnabled(false);
            transFixedRb->setEnabled(false);
            break;
        case WITHOUT_TT_RATIO:
            if (shouldSavePreviousValue) {
                isTtRationFixed = transFixedRb->isChecked();
            }
            transFixedRb->setChecked(false);
            transEstimatedRb->setChecked(false);
            makeTTRatioControlsAvailable(false);
            break;
    }
}

void PhyMlWidget::createWidgetsControllers() {
    // Substitution model.
    const QString subModelSettingsPath = isAminoAcid ? PhyMlSettingsPrefixes::AminoAcidModelType
                                                     : PhyMlSettingsPrefixes::DnaModelType;
    widgetControllers.addWidgetController(subModelCombo, subModelSettingsPath, "-m");

    // Number of substitution rate categories.
    widgetControllers.addWidgetController(substitutionSpinBox, PhyMlSettingsPrefixes::SubRatesNumber, "-c");

    // Transition / transversion ratio
    InputWidgetController* ttRatioEstimationController = widgetControllers.addWidgetController(transFixedRb, PhyMlSettingsPrefixes::EstimateTtRatio, "");
    InputWidgetController* ttRatioController = widgetControllers.addWidgetController(tranSpinBox, PhyMlSettingsPrefixes::TtRatio, "-t");
    ttRatioEstimationController->addDependentParameter(ParameterDependence(ttRatioController, true));

    // Proportion of invariable sites.
    InputWidgetController* sitesEstimationController = widgetControllers.addWidgetController(sitesFixedRb, PhyMlSettingsPrefixes::EstimateSitesProportion, "");
    InputWidgetController* sitesPropController = widgetControllers.addWidgetController(sitesSpinBox, PhyMlSettingsPrefixes::InvariableSitesProportion, "-v");
    sitesEstimationController->addDependentParameter(ParameterDependence(sitesPropController, true));

    // Gamma shape parameter.
    InputWidgetController* gammaEstimationController = widgetControllers.addWidgetController(gammaFixedRb, PhyMlSettingsPrefixes::EstimateGammaFactor, "");
    InputWidgetController* gammaController = widgetControllers.addWidgetController(gammaSpinBox, PhyMlSettingsPrefixes::GammaFactor, "-a");
    gammaEstimationController->addDependentParameter(ParameterDependence(gammaController, true));

    // Bootstrap replicates number.
    // -b (or --bootstrap) int:
    //		int >  0: int is the number of bootstrap replicates.
    //		int =  0: neither approximate likelihood ratio test nor bootstrap values are computed.
    //		int = -1: approximate likelihood ratio test returning aLRT statistics.
    //		int = -2: approximate likelihood ratio test returning Chi2-based parametric branch supports.
    //		int = -4: (default) SH-like branch supports alone.
    //		int = -5: approximate Bayes branch supports.
    InputWidgetController* bootstrapCheckBoxController = widgetControllers.addWidgetController(bootstrapRadioButton, PhyMlSettingsPrefixes::UseBootstrap, "");
    InputWidgetController* bootstrapController = widgetControllers.addWidgetController(bootstrapSpinBox, PhyMlSettingsPrefixes::BootstrapReplicatesNumber, "-b");
    // When bootstrapSpinBox has 0 -> use it in the command line. Otherwise, the real default will be used which is -4 (SH-like).
    bootstrapController->setEmitCommandLineOptionForDefaultFlag(true);
    bootstrapCheckBoxController->addDependentParameter(ParameterDependence(bootstrapController, true));

    // Fast methods.
    InputWidgetController* fastMethodCheckBoxController = widgetControllers.addWidgetController(fastMethodCheckbox, PhyMlSettingsPrefixes::UseFastMethod, "");
    QStringList cmdLineValues = {"-1", "-2", "-4", "-5"};
    InputWidgetController* fastMethodController = widgetControllers.addWidgetController(fastMethodCombo, PhyMlSettingsPrefixes::FastMethodIndex, "-b", cmdLineValues);
    fastMethodCheckBoxController->addDependentParameter(ParameterDependence(fastMethodController, true));

    // Optimisation options.
    widgetControllers.addWidgetController(optBranchCheckbox, PhyMlSettingsPrefixes::OptimiseBranchLengths, "");
    widgetControllers.addWidgetController(optTopologyCheckbox, PhyMlSettingsPrefixes::OptimiseTopology, "");

    // Tree improvements.
    QStringList treeImprovements = {"NNI", "SPR", "BEST"};
    widgetControllers.addWidgetController(treeImprovementsCombo, PhyMlSettingsPrefixes::TreeImprovementType, "-s", treeImprovements);

    // Equilibrium frequencies.
    widgetControllers.addWidgetController(freqOptimRadio, PhyMlSettingsPrefixes::OptimiseEquilibriumFreq, "");

    // Generated tree numbers.
    InputWidgetController* treeNumbersCheckBoxController = widgetControllers.addWidgetController(treeNumbersCheckbox, PhyMlSettingsPrefixes::UseBootstrap, "");
    InputWidgetController* treeNumbersController = widgetControllers.addWidgetController(treeNumbersSpinBox, PhyMlSettingsPrefixes::BootstrapReplicatesNumber, "-b");
    treeNumbersCheckBoxController->addDependentParameter(ParameterDependence(treeNumbersController, true));

    // Tree searching.
    widgetControllers.addWidgetController(treeTypesCombo, PhyMlSettingsPrefixes::TreeSearchingType, "");
    // User tree
    widgetControllers.addWidgetController(inputFileLineEdit, PhyMlSettingsPrefixes::UserTreePath, "");
}

void PhyMlWidget::sl_checkUserTreeType(int newIndex) {
    bool enableFileEdit = newIndex == 1;
    inputFileLineEdit->setEnabled(enableFileEdit);
    inputFilePathButton->setEnabled(enableFileEdit);
}

void PhyMlWidget::sl_checkTreeImprovement(int newIndex) {
    bool isNni = newIndex == 0;
    bool shouldSaveTreeNumbersValue = treeNumbersCheckbox->isEnabled();
    if (shouldSaveTreeNumbersValue) {
        isTreeNumberSet = treeNumbersCheckbox->isChecked();
    }

    if (isNni) {
        treeNumbersCheckbox->setChecked(false);
        treeNumbersCheckbox->setEnabled(false);
        treeNumbersSpinBox->setEnabled(false);
    } else {
        treeNumbersCheckbox->setChecked(isTreeNumberSet);
        treeNumbersCheckbox->setEnabled(true);
        treeNumbersSpinBox->setEnabled(treeNumbersCheckbox->isChecked());
    }
}

void PhyMlWidget::sl_inputPathButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Open an alignment file"), lod.dir, FileFilters::createFileFilterByObjectTypes({BaseDocumentFormats::NEWICK}));
    if (lod.url.isEmpty()) {
        return;
    }
    inputFileLineEdit->setText(lod.url);
}

void PhyMlWidget::sl_optTopologyCheckboxClicked(bool checked) {
    if (checked) {
        optBranchCheckboxSavedState = optBranchCheckbox->isChecked();
        optBranchCheckbox->setChecked(true);
        optBranchCheckbox->setEnabled(false);
    } else {
        optBranchCheckbox->setChecked(optBranchCheckboxSavedState);
        optBranchCheckbox->setEnabled(true);
    }
}

void PhyMlWidget::sl_checkSubModelType(const QString& newModel) {
    if (isAminoAcid) {
        makeTTRatioControlsAvailable(false);
        return;
    }

    const QStringList& allDnaModels = PhyMLModelTypes::getDnaModelTypes();
    int modelIndex = allDnaModels.indexOf(newModel);
    SAFE_POINT(modelIndex >= 0, QString("'%1' is incorrect substitution model for dna sequence").arg(newModel), );

    SubstModelTrRatioType ttRatioType = PhyMLModelTypes::getTtRatioType(newModel);
    makeTTRatioControlsAvailable(ttRatioType);
}

void PhyMlWidget::fillSettings(CreatePhyTreeSettings& settings) {
    settings.extToolArguments = generatePhyMlSettingsScript();
    settings.bootstrap = bootstrapRadioButton->isChecked();
    displayOptions->fillSettings(settings);
}

void PhyMlWidget::storeSettings() {
    widgetControllers.storeSettings();
    displayOptions->storeSettings();
}

void PhyMlWidget::restoreDefault() {
    widgetControllers.restoreDefault();
    displayOptions->restoreDefault();
    sl_checkSubModelType(subModelCombo->currentText());
}

bool PhyMlWidget::checkSettings(QString& message, const CreatePhyTreeSettings& settings) {
    bool fileExists = QFileInfo::exists(inputFileLineEdit->text());
    bool fileHaveToExist = treeTypesCombo->currentIndex() == 1;
    if (fileHaveToExist && !fileExists) {
        twSettings->setCurrentIndex(2);
        inputFileLineEdit->setFocus();
        if (!inputFileLineEdit->text().isEmpty()) {
            message = tr("File with the starting tree is not set.");
        } else {
            message = tr("File with the starting tree doesn't exist.");
        }
        return false;
    }

    // Check that PhyMl and temporary folder path defined.
    ExternalToolRegistry* reg = AppContext::getExternalToolRegistry();
    ExternalTool* phyml = reg->getById(PhyMLSupport::PHYML_ID);
    SAFE_POINT(phyml != nullptr, "External tool PHyML is not registered", false);

    const QString& path = phyml->getPath();
    const QString& name = phyml->getName();

    if (path.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
        msgBox->setWindowTitle(name);
        msgBox->setText(tr("Path for %1 tool not selected.").arg(name));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return false;
            default:
                SAFE_POINT(false, "Incorrect state of the message box", false);
        }
    }

    if (path.isEmpty()) {
        return false;
    }

    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, false);

    return displayOptions->checkSettings(message, settings);
}

QStringList PhyMlWidget::generatePhyMlSettingsScript() {
    QStringList script;
    if (isAminoAcid) {
        script << "-d"
               << "aa";
    }

    widgetControllers.addParametersToCmdLine(script);
    if (sitesEstimatedRb->isChecked()) {
        script << "-v"
               << "e";
    }
    if (gammaEstimatedRb->isChecked()) {
        script << "-a"
               << "e";
    }

    if (treeTypesCombo->currentIndex() == 1) {
        script << "-u";
        script << inputFileLineEdit->text();
    }

    QString optimisationOptions;
    if (optTopologyCheckbox->isChecked()) {
        optimisationOptions = "tl";
    } else if (optBranchCheckbox->isChecked()) {
        optimisationOptions += "l";
    }
    if (optimiseSubstitutionRateCheckbox->isChecked()) {
        optimisationOptions += "r";
    }
    if (!optimisationOptions.isEmpty()) {
        script << "-o";
        script << optimisationOptions;
    }

    return script;
}

}  // namespace U2
