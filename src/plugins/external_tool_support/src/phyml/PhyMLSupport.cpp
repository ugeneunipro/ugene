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

#include "PhyMLSupport.h"

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>

#include "PhyMLDialogWidget.h"
#include "PhyMLTask.h"

namespace U2 {

const QString PhyMLSupport::PHYML_ID("USUPP_PHYML");
const QString PhyMLSupport::PHYML_TEMP_DIR("phyml");
const QString PhyMLSupport::ET_PHYML_ALGORITHM_NAME_AND_KEY("PhyML Maximum Likelihood");

PhyMLSupport::PhyMLSupport()
    : ExternalTool(PhyMLSupport::PHYML_ID, "phyml", "PhyML Maximum Likelihood") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = isOsWindows() ? "PhyML.exe" : "phyml";
    validationArguments << "--help";
    validationMessageRegExp = "PhyML";
    description = tr("<i>PhyML</i> is a simple, fast, and accurate algorithm to estimate large phylogenies by maximum likelihood");
    versionRegExp = QRegExp("- PhyML (\\d+(\\.\\d+)*)");
    toolKitName = "PhyML";

    // register the method
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new PhyMLAdapter(), PhyMLSupport::ET_PHYML_ALGORITHM_NAME_AND_KEY);
}

////////////////////////////////////////
// PhyMLAdapter

Task* PhyMLAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s) {
    return new PhyMLSupportTask(ma, s);
}

CreatePhyTreeWidget* PhyMLAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment& ma, QWidget* parent) {
    return new PhyMlWidget(ma, parent);
}

////////////////////////////////////////
// PhyMLModelTypes

// Amino-acid substitution models
const QStringList PhyMLModelTypes::aminoSubstitutionModels = {
    "LG",
    "WAG",
    "JTT",
    "MtREV",
    "Dayhoff",
    "DCMut",
    "RtREV",
    "CpREV",
    "VT",
    "AB",
    "Blosum62",
    "MtMam",
    "MtArt",
    "HIVw",
    "HIVb",
};

// Dna substitution models
const QStringList PhyMLModelTypes::dnaSubstitutionModels = {
    "HKY85",
    "JC69",
    "K80",
    "F81",
    "F84",
    "TN93",
    "GTR",
};

const QStringList PhyMLModelTypes::dnaModelsWithFixedTtRatio = {
    "GTR",
    "F81",
    "JC69",
};

const QStringList PhyMLModelTypes::dnaModelsWithEstimatedTtRatio({"TN93"});

SubstModelTrRatioType PhyMLModelTypes::getTtRatioType(const QString& modelName) {
    if (dnaSubstitutionModels.contains(modelName)) {
        if (dnaModelsWithFixedTtRatio.contains(modelName)) {
            return ONLY_FIXED_TT_RATIO;
        }
        if (dnaModelsWithEstimatedTtRatio.contains(modelName)) {
            return ONLY_ESTIMATED_TT_RATIO;
        }
        return ANY_TT_RATIO;
    }
    return WITHOUT_TT_RATIO;
}

////////////////////////////////////////
// PhyMLRatioTestsTypes

const QStringList PhyMLRatioTestsTypes::ratioTestsTypes = {
    "aLRT",
    "Chi2-based",
    "SH-like",
    "Bayes branch",
};

const int PhyMLRatioTestsTypes::defaultRatioTestsTypeIndex = 2;  // SH-like;

////////////////////////////////////////
// TreeSearchingParams
const QStringList TreeSearchingParams::inputTreeTypes = {
    "Make initial tree automatically (BioNJ)",
    "Use tree from file",
};

const QStringList TreeSearchingParams::treeImprovementTypes = {
    "NNI(fast)",
    "SRT(a bit slower than NNI)",
    "SRT & NNI(best of NNI and SPR search)",
};

}  // namespace U2
