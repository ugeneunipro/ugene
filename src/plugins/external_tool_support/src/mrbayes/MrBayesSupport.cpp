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

#include "MrBayesDialogWidget.h"
#include "MrBayesTask.h"
#include "MrBayesTests.h"
#include "MrBayesSupport.h"

#include <U2Algorithm/PhyTreeGeneratorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

namespace U2 {

const QString MrBayesSupport::ET_MRBAYES = "MrBayes";
const QString MrBayesSupport::ET_MRBAYES_ID = "USUPP_MRBAYES";
const QString MrBayesSupport::MRBAYES_TMP_DIR ="mrbayes";

MrBayesSupport::MrBayesSupport(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/mrbayes.png");
        grayIcon = QIcon(":external_tool_support/images/mrbayes_gray.png");
        warnIcon = QIcon(":external_tool_support/images/mrbayes_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "mb.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "mb";
#endif

    validationArguments << "";      // anyting to get info and exit with error
    validMessage = "MrBayes";
    description = tr("<i>MrBayes</i> is a program for the Bayesian estimation of phylogeny."
                   "Bayesian inference of phylogeny is based upon a quantity called the posterior "
                   "probability distribution of trees, which is the probability of a tree conditioned "
                   "on the observations. The conditioning is accomplished using Bayes's theorem. "
                   "The posterior probability distribution of trees is impossible to calculate analytically; "
                   "instead, MrBayes uses a simulation technique called Markov chain Monte Carlo (or MCMC) "
                   "to approximate the posterior probabilities of trees.");
    versionRegExp = QRegExp("MrBayes v(\\d+\\.\\d+\\.\\d+)");
    toolKitName = "MrBayes";

    //register the method
    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new MrBayesAdapter(), ET_MRBAYES);
}

////////////////////////////////////////
//MrBayesAdapter

Task * MrBayesAdapter::createCalculatePhyTreeTask(const MultipleSequenceAlignment &ma, const CreatePhyTreeSettings &s) {
    return new MrBayesSupportTask(ma, s);
}

CreatePhyTreeWidget * MrBayesAdapter::createPhyTreeSettingsWidget(const MultipleSequenceAlignment &ma, QWidget *parent) {
    return new MrBayesWidget(ma, parent);
}

////////////////////////////////////////
//MrBayesModelTypes

QString MrBayesModelTypes::poisson("poisson");
QString MrBayesModelTypes::jones("jones");
QString MrBayesModelTypes::dayhoff("dayhoff");
QString MrBayesModelTypes::mtrev("mtrev");
QString MrBayesModelTypes::mtmam("mtmam");
QString MrBayesModelTypes::wag("wag");
QString MrBayesModelTypes::rtrev("rtrev");
QString MrBayesModelTypes::cprev("cprev");
QString MrBayesModelTypes::vt("vt");
QString MrBayesModelTypes::blosum("blosum");
QString MrBayesModelTypes::equalin("equalin");

QStringList MrBayesModelTypes::getAAModelTypes(){
    static QStringList list;
    if (Q_UNLIKELY(list.isEmpty())) {
        list << MrBayesModelTypes::poisson
             << MrBayesModelTypes::jones
             << MrBayesModelTypes::dayhoff
             << MrBayesModelTypes::mtrev
             << MrBayesModelTypes::mtmam
             << MrBayesModelTypes::wag
             << MrBayesModelTypes::rtrev
             << MrBayesModelTypes::cprev
             << MrBayesModelTypes::vt
             << MrBayesModelTypes::blosum
             << MrBayesModelTypes::equalin;
    }

    return list;
}

QString MrBayesModelTypes::JC69("JC69 (Nst=1)");
QString MrBayesModelTypes::HKY85("HKY85 (Nst=2)");
QString MrBayesModelTypes::GTR("GTR (Nst=6)");

QStringList MrBayesModelTypes::getSubstitutionModelTypes() {
    static QStringList list;
    if (Q_UNLIKELY(list.isEmpty())) {
        list << MrBayesModelTypes::JC69
             << MrBayesModelTypes::HKY85
             << MrBayesModelTypes::GTR;
    }

    return list;
}

QString MrBayesVariationTypes::equal("equal");
QString MrBayesVariationTypes::gamma("gamma");
QString MrBayesVariationTypes::propinv("propinv");
QString MrBayesVariationTypes::invgamma("invgamma");

QStringList MrBayesVariationTypes::getVariationTypes() {
    static QList<QString> list;
    if (Q_UNLIKELY(list.isEmpty())) {
        list << MrBayesVariationTypes::equal
             << MrBayesVariationTypes::gamma
             << MrBayesVariationTypes::propinv
             << MrBayesVariationTypes::invgamma;
    }

    return list;
}

}   // namespace U2
