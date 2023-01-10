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

#include "BlastNTask.h"

#include <QDomDocument>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/Log.h>

#include "BlastSupport.h"

namespace U2 {

ExternalToolRunTask* BlastNTask::createBlastTask() {
    QStringList arguments;
    // arguments <<"-p"<< settings.programName; //taskname
    //     if(!settings.filter.isEmpty()){
    //         arguments <<"-F"<<settings.filter;
    //     }
    arguments << "-db" << settings.databaseNameAndPath;
    arguments << "-evalue" << QString::number(settings.expectValue);
    arguments << "-task" << (settings.megablast ? "megablast" : "blastn");
    if (settings.wordSize <= 0) {
        arguments << "-word_size"
                  << "11";
    } else {
        arguments << "-word_size" << QString::number(settings.wordSize);
    }

    if (settings.directStrand == TriState_Yes) {
        arguments << "-strand"
                  << "plus";
    } else if (settings.directStrand == TriState_No) {
        arguments << "-strand"
                  << "minus";
    }
    if (!settings.isDefaultCosts) {
        arguments << "-gapopen" << QString::number(settings.gapOpenCost);
        arguments << "-gapextend" << QString::number(settings.gapExtendCost);
    }
    if (settings.isNucleotideSeq && (!settings.isDefaultScores)) {
        arguments << "-penalty" << QString::number(settings.mismatchPenalty);
        arguments << "-reward" << QString::number(settings.matchReward);
    } else if (!settings.isDefaultMatrix) {
        FAIL("'blastn' does not support custom matrix option", nullptr)
    }
    if (settings.numberOfHits != 0) {
        arguments << "-culling_limit" << QString::number(settings.numberOfHits);  //???
    }
    if (!settings.isGappedAlignment) {
        arguments << "-ungapped";
    }

    arguments << "-query" << url;
    if ((settings.xDropoffGA != 30) ||
        (settings.megablast && settings.xDropoffGA != 20)) {
        arguments << "-xdrop_gap" << QString::number(settings.xDropoffGA);
    }
    if (settings.xDropoffFGA != 100) {
        arguments << "-xdrop_gap_final" << QString::number(settings.xDropoffFGA);
    }

    if ((settings.xDropoffUnGA != 20) ||
        (settings.megablast && settings.xDropoffUnGA != 10)) {
        arguments << "-xdrop_ungap" << QString::number(settings.xDropoffUnGA);
    }
    if (settings.windowSize != 0) {
        arguments << "-window_size" << QString::number(settings.windowSize);
    }
    arguments << "-num_threads" << QString::number(settings.numberOfProcessors);
    arguments << "-outfmt" << QString::number(settings.outputType);  //"5";//Set output file format to xml
    if (settings.outputOriginalFile.isEmpty()) {
        arguments << "-out" << url + ".xml";
        settings.outputOriginalFile = url + ".xml";
    } else {
        arguments << "-out" << settings.outputOriginalFile;
    }

    algoLog.trace("BlastN arguments: " + arguments.join(" "));
    QString workingDirectory = QFileInfo(url).absolutePath();
    auto toolRunTask = new ExternalToolRunTask(BlastSupport::ET_BLASTN_ID, arguments, new ExternalToolLogParser(), workingDirectory);
    setListenerForTask(toolRunTask);
    return toolRunTask;
}
}  // namespace U2
