/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/Counter.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DiamondBuildTask.h"
#include "DiamondSupport.h"
#include "GenomesPreparationTask.h"

namespace U2 {

DiamondBuildTask::DiamondBuildTask(const DiamondBuildTaskSettings &_settings)
    : ExternalToolSupportTask(tr("Build DIAMOND database"), TaskFlags_NR_FOSE_COSC),
      settings(_settings)
{
    GCOUNTER(cvar, tvar, "DiamondBuildTask");
    checkSettings();
}

const QString DiamondBuildTask::getDatabaseUrl() const {
    return settings.databaseUrl;
}

void DiamondBuildTask::prepare() {
    genomesPreparationTask = new GenomesPreparationTask(settings.genomesUrls, GUrlUtils::rollFileName(settings.workingDir + "/prepared_genomes.fa.gz", QSet<QString>()));
    addSubTask(genomesPreparationTask);
}

QList<Task *> DiamondBuildTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> newSubTasks;
    CHECK_OP(stateInfo, newSubTasks);

    if (genomesPreparationTask == subTask) {
        ExternalToolRunTask *buildTask = new ExternalToolRunTask(DiamondSupport::TOOL_NAME, getArguments(genomesPreparationTask->getPreparedGenomesFileUrl()), new ExternalToolLogParser);
        setListenerForTask(buildTask);
        newSubTasks << buildTask;
    }

    return newSubTasks;
}

void DiamondBuildTask::checkSettings() {
    SAFE_POINT_EXT(!settings.databaseUrl.isEmpty(), setError("Result database URL is empty"), );
    CHECK_EXT(!settings.genomesUrls.isEmpty(), setError(tr("There is no input files to build the database from")), );
    SAFE_POINT_EXT(!settings.taxonMapUrl.isEmpty(), setError(tr("Taxon map URL is empty")), );
    SAFE_POINT_EXT(!settings.taxonNodesUrl.isEmpty(), setError(tr("Taxon nodes URL is empty")), );
}

QStringList DiamondBuildTask::getArguments(const QString &preparedGenomesFileUrl) const {
    QStringList arguments;
    arguments << "makedb";
    arguments << "--in" << preparedGenomesFileUrl;
    arguments << "-d" << settings.databaseUrl;
    arguments << "--taxonmap" << settings.taxonMapUrl;
    arguments << "--taxonnodes" << settings.taxonNodesUrl;
    return arguments;
}

}   // namespace U2
