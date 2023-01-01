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

#include "RPSBlastTask.h"

#include <QFileInfo>

#include "BlastSupport.h"

namespace U2 {

ExternalToolRunTask* RPSBlastTask::createBlastTask() {
    QStringList arguments;
    arguments << "-db" << settings.databaseNameAndPath;
    arguments << "-evalue" << QString::number(settings.expectValue);
    arguments << "-query" << url;
    arguments << "-outfmt"
              << "5";  // Set output file format to xml
    arguments << "-out" << url + ".xml";  // settings.outputRepFile;

    algoLog.trace("RPSBlast arguments: " + arguments.join(" "));

    QString workingDirectory = QFileInfo(url).absolutePath();
    auto runTask = new ExternalToolRunTask(BlastSupport::ET_RPSBLAST_ID, arguments, new ExternalToolLogParser(), workingDirectory);
    setListenerForTask(runTask);
    return runTask;
}

LocalCDSearch::LocalCDSearch(const CDSearchSettings& cdSearchSettings) {
    BlastTaskSettings settings;
    settings.databaseNameAndPath = cdSearchSettings.localDbFolder + "/" + cdSearchSettings.dbName;
    settings.querySequences = {cdSearchSettings.query};
    settings.expectValue = cdSearchSettings.ev;
    settings.alphabet = cdSearchSettings.alp;
    settings.needCreateAnnotations = false;
    task = new RPSBlastTask(settings);
}

QList<SharedAnnotationData> LocalCDSearch::getCDSResults() const {
    return task->getResultAnnotations();
}

}  // namespace U2
