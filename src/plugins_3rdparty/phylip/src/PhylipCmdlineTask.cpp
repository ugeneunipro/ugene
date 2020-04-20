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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CmdlineInOutTaskRunner.h>
#include <U2Core/DeleteObjectsTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "PhylipCmdlineTask.h"

namespace U2 {

const QString PhylipCmdlineTask::PHYLIP_CMDLINE = "phylip";
const QString PhylipCmdlineTask::MATRIX_ARG = "matrix";
const QString PhylipCmdlineTask::GAMMA_ARG = "gamma";
const QString PhylipCmdlineTask::ALPHA_ARG = "alpha-factor";
const QString PhylipCmdlineTask::TT_RATIO_ARG = "tt-ratio";
const QString PhylipCmdlineTask::BOOTSTRAP_ARG = "bootstrap";
const QString PhylipCmdlineTask::REPLICATES_ARG = "replicates";
const QString PhylipCmdlineTask::SEED_ARG = "seed";
const QString PhylipCmdlineTask::FRACTION_ARG = "fraction";
const QString PhylipCmdlineTask::CONSENSUS_ARG = "consensus";

PhylipCmdlineTask::PhylipCmdlineTask(const MultipleSequenceAlignment &msa, const CreatePhyTreeSettings &settings)
: PhyTreeGeneratorTask(msa, settings), cmdlineTask(NULL), msaObject(NULL), treeObject(NULL)
{
    setTaskName(tr("PHYLIP command line wrapper task"));
    tpm = Progress_SubTasksBased;
}

PhylipCmdlineTask::~PhylipCmdlineTask() {
    if (!dbiPath.isEmpty()) {
        QFile::remove(dbiPath);
    }
}

void PhylipCmdlineTask::prepare() {
    prepareTempDbi();
    CHECK_OP(stateInfo, );
    createCmdlineTask();
    CHECK_OP(stateInfo, );
    addSubTask(cmdlineTask);
}

Task::ReportResult PhylipCmdlineTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);
    QList<U2DataId> objects = cmdlineTask->getOutputObjects();
    if (objects.isEmpty()) {
        setError(tr("No tree objects found."));
        return ReportResult_Finished;
    }
    CHECK_OP(stateInfo, ReportResult_Finished);
    treeObject = new PhyTreeObject("tree", U2EntityRef(dbiRef, objects.first()));
    treeObject->setParent(this);
    result = treeObject->getTree();
    return ReportResult_Finished;
}

void PhylipCmdlineTask::createCmdlineTask() {
    CmdlineInOutTaskConfig config;
    CHECK_OP(stateInfo, );
    msaObject = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, const_cast<MultipleSequenceAlignment&>(inputMA), stateInfo);
    CHECK_OP(stateInfo, );
    msaObject->setParent(this);
    config.inputObjects << msaObject;
    config.outDbiRef = dbiRef;
    config.withPluginList = true;
    config.pluginList << PLUGIN_ID;
    config.command = "--" + PHYLIP_CMDLINE;
    QString argString = "--%1=\"%2\"";
    config.arguments << argString.arg(MATRIX_ARG).arg(settings.matrixId);
    config.arguments << argString.arg(GAMMA_ARG).arg(settings.useGammaDistributionRates);
    config.arguments << argString.arg(ALPHA_ARG).arg(settings.alphaFactor);
    config.arguments << argString.arg(TT_RATIO_ARG).arg(settings.ttRatio);
    config.arguments << argString.arg(BOOTSTRAP_ARG).arg(settings.bootstrap);
    config.arguments << argString.arg(REPLICATES_ARG).arg(settings.replicates);
    config.arguments << argString.arg(SEED_ARG).arg(settings.seed);
    config.arguments << argString.arg(FRACTION_ARG).arg(settings.fraction);
    config.arguments << argString.arg(CONSENSUS_ARG).arg(settings.consensusID);

    cmdlineTask = new CmdlineInOutTaskRunner(config);
}

void PhylipCmdlineTask::prepareTempDbi() {
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    dbiPath = GUrlUtils::prepareTmpFileLocation(tmpDirPath, "phylip", "ugenedb", stateInfo);
    CHECK_OP(stateInfo, );

    dbiRef = U2DbiRef(DEFAULT_DBI_ID, dbiPath);
    QHash<QString, QString> properties;
    properties[U2DbiOptions::U2_DBI_LOCKING_MODE] = "normal";
    DbiConnection(dbiRef, true, stateInfo, properties);
}

} // U2
