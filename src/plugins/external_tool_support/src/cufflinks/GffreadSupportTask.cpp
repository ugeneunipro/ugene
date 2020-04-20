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

#include "CufflinksSupport.h"

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/U2SafePoints.h>

#include "GffreadSupportTask.h"

namespace U2 {

GffreadSupportTask::GffreadSupportTask(const GffreadSettings &_settings)
: ExternalToolSupportTask(tr("Running Gffread task"), TaskFlags_NR_FOSE_COSC), settings(_settings)
{

}

void GffreadSupportTask::prepare() {
    checkFormat(settings.genomeUrl, BaseDocumentFormats::FASTA);
    CHECK_OP(stateInfo, );
    checkFormat(settings.transcriptsUrl, BaseDocumentFormats::GTF);
    CHECK_OP(stateInfo, );

    ExternalToolRunTask* runTask = new ExternalToolRunTask(CufflinksSupport::ET_GFFREAD_ID, settings.getArguments(), new ExternalToolLogParser());
    setListenerForTask(runTask);
    addSubTask(runTask);
}

QString GffreadSupportTask::result() const {
    return settings.outputUrl;
}

void GffreadSupportTask::checkFormat(const QString &url, const DocumentFormatId &target) {
    FormatDetectionConfig cfg;
    cfg.bestMatchesOnly = false;
    QList<FormatDetectionResult> result = DocumentUtils::detectFormat(url, cfg);
    if (result.isEmpty()) {
        setError(tr("Unknown file format: %1").arg(url));
        return;
    }
    foreach (const FormatDetectionResult &r, result) {
        SAFE_POINT(NULL != r.format, "NULL doc format", );
        if (r.format->getFormatId() == target) {
            return;
        }
    }
    setError(tr("The file format is not [%1]: %2").arg(target).arg(url));
}

QStringList GffreadSettings::getArguments() const {
    QStringList args;
    args << "-w";
    args << outputUrl;
    args << "-g";
    args << genomeUrl;
    args << transcriptsUrl;
    return args;
}

} // U2
