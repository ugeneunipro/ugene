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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ConvertAlignment2StockholmTask.h"
#include "HmmerBuildFromFileTask.h"

namespace U2 {

HmmerBuildFromFileTask::HmmerBuildFromFileTask(const HmmerBuildSettings &settings, const QString &msaUrl)
    : ExternalToolSupportTask(tr("Build HMMER profile from file"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported),
      convertTask(NULL),
      buildTask(NULL),
      settings(settings),
      msaUrl(msaUrl)
{
    SAFE_POINT_EXT(!msaUrl.isEmpty(), tr("Msa URL is empty"), );
}

const QString & HmmerBuildFromFileTask::getHmmProfileUrl() const {
    return settings.profileUrl;
}

void HmmerBuildFromFileTask::prepare() {
    if (!isStockholm()) {
        prepareConvertTask();
        addSubTask(convertTask);
    } else {
        prepareBuildTask(msaUrl);
        addSubTask(buildTask);
    }
}

QList<Task *> HmmerBuildFromFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;
    CHECK_OP(stateInfo, result);

    if (subTask == convertTask) {
        prepareBuildTask(convertTask->getResultUrl());
        result << buildTask;
    } else if (subTask == buildTask) {
        removeTempDir();
    }

    return result;
}

Task::ReportResult HmmerBuildFromFileTask::report() {
    if (NULL != convertTask) {
        QFile(convertTask->getResultUrl()).remove();
    }
    return ReportResult_Finished;
}

QString HmmerBuildFromFileTask::generateReport() const {
    return HmmerBuildTask::getReport(this, settings, msaUrl);
}

bool HmmerBuildFromFileTask::isStockholm() {
    QString formatId;
    DocumentUtils::detectFormat(msaUrl, formatId);
    return formatId == BaseDocumentFormats::STOCKHOLM;
}

void HmmerBuildFromFileTask::prepareConvertTask() {
    convertTask = new ConvertAlignment2Stockholm(msaUrl, settings.workingDir);
    convertTask->setSubtaskProgressWeight(10);
}

void HmmerBuildFromFileTask::prepareBuildTask(const QString &stockholmMsaUrl) {
    buildTask = new HmmerBuildTask(settings, stockholmMsaUrl);
    setListenerForTask(buildTask);
    buildTask->setSubtaskProgressWeight(90);
}

void HmmerBuildFromFileTask::removeTempDir() {
    if (settings.workingDir.isEmpty()) {
        U2OpStatusImpl os;
        ExternalToolSupportUtils::removeTmpDir(settings.workingDir, os);
    }
}

}   // namespace U2
