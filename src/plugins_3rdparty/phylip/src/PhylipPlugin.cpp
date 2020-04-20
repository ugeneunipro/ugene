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

#include <QDialog>
#include <QMenu>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CmdlineInOutTaskRunner.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/GTestFrameworkComponents.h>
#include "PhylipPluginTests.h"
#include "PhylipCmdlineTask.h"
#include "PhylipTask.h"
#include "NeighborJoinAdapter.h"

#include "PhylipPlugin.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    PhylipPlugin *plug = new PhylipPlugin();
    return plug;
}

const QString PhylipPlugin::PHYLIP_NEIGHBOUR_JOIN("PHYLIP Neighbor Joining");

PhylipPlugin::PhylipPlugin() 
: Plugin(tr("PHYLIP"), tr("PHYLIP (the PHYLogeny Inference Package) is a package of programs for inferring phylogenies (evolutionary trees)."
         " Original version at: http://evolution.genetics.washington.edu/phylip.html"))
{

    PhyTreeGeneratorRegistry* registry = AppContext::getPhyTreeGeneratorRegistry();
    registry->registerPhyTreeGenerator(new NeighborJoinAdapter(), PHYLIP_NEIGHBOUR_JOIN);

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = PhylipPluginTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) { 
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }

    processCmdlineOptions();
}

namespace {
    CreatePhyTreeSettings fetchSettings() {
        CreatePhyTreeSettings settings;
        CMDLineRegistry *cmdLineRegistry = AppContext::getCMDLineRegistry();
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::MATRIX_ARG)) {
            settings.matrixId = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::MATRIX_ARG);
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::GAMMA_ARG)) {
            settings.useGammaDistributionRates = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::GAMMA_ARG).toInt();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::ALPHA_ARG)) {
            settings.alphaFactor = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::ALPHA_ARG).toDouble();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::TT_RATIO_ARG)) {
            settings.ttRatio = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::TT_RATIO_ARG).toDouble();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::BOOTSTRAP_ARG)) {
            settings.bootstrap = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::BOOTSTRAP_ARG).toInt();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::REPLICATES_ARG)) {
            settings.replicates = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::REPLICATES_ARG).toInt();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::SEED_ARG)) {
            settings.seed = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::SEED_ARG).toInt();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::FRACTION_ARG)) {
            settings.fraction = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::FRACTION_ARG).toDouble();
        }
        if (cmdLineRegistry->hasParameter(PhylipCmdlineTask::CONSENSUS_ARG)) {
            settings.consensusID = cmdLineRegistry->getParameterValue(PhylipCmdlineTask::CONSENSUS_ARG);
        }
        return settings;
    }
}

void PhylipPlugin::processCmdlineOptions() {
    CMDLineRegistry *cmdLineRegistry = AppContext::getCMDLineRegistry();
    CHECK(cmdLineRegistry->hasParameter(PhylipCmdlineTask::PHYLIP_CMDLINE), );
    CHECK(cmdLineRegistry->hasParameter(CmdlineInOutTaskRunner::OUT_DB_ARG), );
    CHECK(cmdLineRegistry->hasParameter(CmdlineInOutTaskRunner::IN_DB_ARG), );
    CHECK(cmdLineRegistry->hasParameter(CmdlineInOutTaskRunner::IN_ID_ARG), );

    CreatePhyTreeSettings settings = fetchSettings();
    QString outDbString = cmdLineRegistry->getParameterValue(CmdlineInOutTaskRunner::OUT_DB_ARG);
    QString inDbString = cmdLineRegistry->getParameterValue(CmdlineInOutTaskRunner::IN_DB_ARG);
    QString idString = cmdLineRegistry->getParameterValue(CmdlineInOutTaskRunner::IN_ID_ARG);

    U2OpStatus2Log os;
    U2DbiRef outDbiRef = CmdlineInOutTaskRunner::parseDbiRef(outDbString, os);
    CHECK_OP(os, );
    U2DbiRef inDbiRef = CmdlineInOutTaskRunner::parseDbiRef(inDbString, os);
    CHECK_OP(os, );
    U2DataId dataId = CmdlineInOutTaskRunner::parseDataId(idString, inDbiRef, os);
    CHECK_OP(os, );

    Task *t = new PhylipTask(U2EntityRef(inDbiRef, dataId), outDbiRef, settings);
    connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(t), SLOT(registerTask()));
}

}//namespace
