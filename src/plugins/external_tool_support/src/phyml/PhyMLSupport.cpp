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

#include "PhyMLDialogWidget.h"
#include "PhyMLSupport.h"
#include "PhyMLTask.h"
#include "PhyMLTests.h"

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
namespace U2 {

const QString PhyMLSupport::PHYML("PhyML Maximum Likelihood");
const QString PhyMLSupport::PHYML_ID("USUPP_PHYML");
const QString PhyMLSupport::PHYML_TEMP_DIR("phyml");

PhyMLSupport::PhyMLSupport(const QString& id, const QString& name) : ExternalTool(id, name, "")
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "PhyML.exe";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    executableFileName = "phyml";
#endif

    validationArguments << "--help";
    validMessage = "PhyML";
    description = tr("<i>PhyML</i> is a simple, fast, and accurate algorithm to estimate large phylogenies by maximum likelihood");
    versionRegExp = QRegExp("- PhyML (\\d+)");
    toolKitName = "PhyML";

    //register the method
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new PhyMLAdapter(), PhyMLSupport::PHYML);
}

////////////////////////////////////////
//PhyMLAdapter

Task* PhyMLAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s) {
    return new PhyMLSupportTask(ma, s);
}

CreatePhyTreeWidget * PhyMLAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment &ma, QWidget *parent) {
    return new PhyMlWidget(ma, parent);
}

////////////////////////////////////////
//PhyMLModelTypes

//Amino-acid substitution models
const QStringList PhyMLModelTypes::aminoSubstitutionModels(QStringList()
    << "LG"
    << "WAG"
    << "JTT"
    << "MtREV"
    << "Dayhoff"
    << "DCMut"
    << "RtREV"
    << "CpREV"
    << "VT"
    << "Blosum62"
    << "MtMam"
    << "MtArt"
    << "HIVw"
    << "HIVb");

//Dna substitution models
const QStringList PhyMLModelTypes::dnaSubstitutionModels(QStringList()
    << "HKY85"
    << "JC69"
    << "K80"
    << "F81"
    << "F84"
    << "TN93"
    << "GTR");

const QStringList PhyMLModelTypes::dnaModelsWithFixedTtRatio(QStringList()
    << "GTR"
    << "F81"
    << "JC69");

const QStringList PhyMLModelTypes::dnaModelsWithEstimatedTtRatio(QStringList()
    << "TN93");

SubstModelTrRatioType PhyMLModelTypes::getTtRatioType(const QString& modelName) {
    if(dnaSubstitutionModels.contains(modelName)) {
        if(dnaModelsWithFixedTtRatio.contains(modelName)) {
            return ONLY_FIXED_TT_RATIO;
        }
        if(dnaModelsWithEstimatedTtRatio.contains(modelName)) {
            return ONLY_ESTIMATED_TT_RATIO;
        }
        return ANY_TT_RATIO;
    }
    return WITHOUT_TT_RATIO;
}

////////////////////////////////////////
//PhyMLRatioTestsTypes

const QStringList PhyMLRatioTestsTypes::ratioTestsTypes(QStringList()
    << "aLRT"
    << "Chi2-based"
    << "SH-like");

////////////////////////////////////////
//TreeSearchingParams
const QStringList TreeSearchingParams::inputTreeTypes(QStringList()
    << "Make initial tree automatically (BioNJ)"
    << "Use tree from file");

const QStringList TreeSearchingParams::treeImprovementTypes(QStringList()
    << "NNI(fast)"
    << "SRT(a bit slower than NNI)"
    << "SRT & NNI(best of NNI and SPR search)");

}   // namespace U2
