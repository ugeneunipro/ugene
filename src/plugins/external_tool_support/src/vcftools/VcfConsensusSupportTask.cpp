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

#include "VcfConsensusSupportTask.h"
#include "VcfConsensusSupport.h"

#include "samtools/TabixSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>

namespace U2 {

VcfConsensusSupportTask::VcfConsensusSupportTask(const GUrl &inputFA, const GUrl &inputVcf, const GUrl &output)
    : ExternalToolSupportTask(tr("Create Vcf Consensus"), TaskFlags_NR_FOSE_COSC),
      inputFA(inputFA),
      inputVcf(inputVcf),
      output(output),
      tabixTask(NULL),
      vcfTask(NULL)
{
}

void VcfConsensusSupportTask::prepare() {
    algoLog.details(tr("VcfConsensus started"));

    SAFE_POINT_EXT(AppContext::getAppSettings() != NULL, setError(tr("AppSettings is NULL")), );
    const UserAppsSettings* userAS = AppContext::getAppSettings()->getUserAppsSettings();
    SAFE_POINT_EXT(userAS != NULL, setError(tr("UserAppsSettings is NULL")), );
    QString tmpDirPath( userAS->getCurrentProcessTemporaryDirPath(VcfConsensusSupport::VCF_CONSENSUS_TMP_DIR) );
    SAFE_POINT_EXT(!tmpDirPath.isEmpty(), setError(tr("Temporary folder is not set!")), );
    GUrl tmp( tmpDirPath + "/" + inputVcf.fileName() + ".gz");

    QDir tmpDir( tmpDirPath );
    if (!tmpDir.mkpath(tmpDirPath)){
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }

    algoLog.info(tr("Saving temporary data to file '%1'").arg(tmp.getURLString()));

    tabixTask = new TabixSupportTask(inputVcf, tmp);
    tabixTask->addListeners(QList <ExternalToolListener*>() << getListener(0));

    addSubTask(tabixTask);
}

QList<Task*> VcfConsensusSupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> res;

    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask != tabixTask) {
        return res;
    }

    ExternalToolRegistry* extToolReg = AppContext::getExternalToolRegistry();
    SAFE_POINT_EXT(extToolReg, setError(tr("ExternalToolRegistry is NULL")), res);

    ExternalTool *vcfToolsET = extToolReg->getById(VcfConsensusSupport::ET_VCF_CONSENSUS_ID);
    ExternalTool *tabixET = extToolReg->getById(TabixSupport::ET_TABIX_ID);
    SAFE_POINT_EXT(vcfToolsET, setError(tr("There is no VcfConsensus external tool registered")), res);
    SAFE_POINT_EXT(tabixET, setError(tr("There is no Tabix external tool registered")), res);

    QMap <QString, QString> envVariables;
    envVariables["PERL5LIB"] = getPath(vcfToolsET);

    vcfTask = new ExternalToolRunTask(VcfConsensusSupport::ET_VCF_CONSENSUS_ID, QStringList() << tabixTask->getOutputBgzf().getURLString(),
        new ExternalToolLogParser(), "", QStringList() << getPath(tabixET));
    vcfTask->setStandartInputFile( inputFA.getURLString() );
    vcfTask->setStandartOutputFile( output.getURLString() );
    vcfTask->setAdditionalEnvVariables(envVariables);

    setListenerForTask(vcfTask, 1);
    res.append(vcfTask);
    return res;
}

const GUrl& VcfConsensusSupportTask::getResultUrl() {
    return output;
}

QString VcfConsensusSupportTask::getPath(ExternalTool *et) {
    if (et == NULL) {
        setError(tr("Trying to get path of NULL external tool"));
        return QString();
    }
    if ( et->getPath().isEmpty() ) {
        setError(tr("Path to %1").arg(et->getName()));
        return QString();
    }
    QFileInfo fileInfo(et->getPath());
    return fileInfo.absolutePath();
}

} // namespace U2
