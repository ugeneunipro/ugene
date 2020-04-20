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
#include <U2Core/CopyFileTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "TabixSupport.h"
#include "TabixSupportTask.h"

namespace U2 {

// TabixSupportTask
TabixSupportTask::TabixSupportTask(const GUrl& fileUrl, const GUrl& outputUrl)
    : ExternalToolSupportTask(tr("Generate index with Tabix task"), TaskFlags_NR_FOSE_COSC),
      fileUrl(fileUrl),
      bgzfUrl(outputUrl),
      bgzipTask(NULL),
      copyTask(NULL),
      tabixTask(NULL)
{
}

void TabixSupportTask::prepare() {
    algoLog.details(tr("Tabix indexing started"));

    if ( BgzipTask::checkBgzf( fileUrl )) {
        algoLog.info(tr("Input file '%1' is already bgzipped").arg(fileUrl.getURLString()));

        copyTask = new CopyFileTask(fileUrl.getURLString(), bgzfUrl.getURLString());
        addSubTask(copyTask);
        return;
    }

    if (bgzfUrl.isEmpty()) {
        bgzfUrl = GUrl(fileUrl.getURLString() + ".gz");
    }

    algoLog.info(tr("Saving data to file '%1'").arg(bgzfUrl.getURLString()));

    bgzipTask = new BgzipTask(fileUrl, bgzfUrl);
    addSubTask(bgzipTask);
}

QList<Task*> TabixSupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> res;

    if (hasError() || isCanceled()) {
        return res;
    }
    if ((subTask != bgzipTask) && (subTask != copyTask)) {
        return res;
    }

    if (subTask == copyTask) {
        bgzfUrl = copyTask->getTargetFilePath();
    }

    initTabixTask();
    res.append(tabixTask);
    return res;
}

const GUrl& TabixSupportTask::getOutputBgzf() const {
    return bgzfUrl;
}

const GUrl TabixSupportTask::getOutputTbi() const {
    GUrl tbi(bgzfUrl.getURLString() + ".tbi");
    return tbi;
}

void TabixSupportTask::initTabixTask() {
    QStringList arguments;
    arguments << "-f";
    arguments << bgzfUrl.getURLString();
    tabixTask = new ExternalToolRunTask(TabixSupport::ET_TABIX_ID, arguments, new ExternalToolLogParser());
    setListenerForTask(tabixTask);
}

}   // namespace U2
