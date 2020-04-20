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

#include <QFileInfo>

#include "RPSBlastSupportTask.h"
#include "BlastPlusSupport.h"

namespace U2 {

ExternalToolRunTask* RPSBlastSupportTask::createBlastPlusTask() {
    QStringList arguments;
    arguments << "-db" << settings.databaseNameAndPath;
    arguments << "-evalue" << QString::number(settings.expectValue);
    arguments << "-query" << url;
    arguments << "-outfmt" << "5"; //Set output file format to xml
    arguments << "-out" << url+".xml"; //settings.outputRepFile;

    algoLog.trace("RPSBlast arguments: "+arguments.join(" "));
    QString workingDirectory = QFileInfo(url).absolutePath();
    ExternalToolRunTask* runTask = new ExternalToolRunTask(BlastPlusSupport::ET_RPSBLAST_ID, arguments, new ExternalToolLogParser(), workingDirectory);
    setListenerForTask(runTask);
    return runTask;
}

LocalCDSearch::LocalCDSearch(const CDSearchSettings& settings) {
    BlastTaskSettings stngs;
    stngs.databaseNameAndPath = settings.localDbFolder + "/" + settings.dbName;
    stngs.querySequence = settings.query;
    stngs.expectValue = settings.ev;
    stngs.alphabet = settings.alp;
    stngs.needCreateAnnotations = false;
    task = new RPSBlastSupportTask(stngs);
}

QList<SharedAnnotationData> LocalCDSearch::getCDSResults() const {
    return task->getResultedAnnotations();
}

} //namespace
