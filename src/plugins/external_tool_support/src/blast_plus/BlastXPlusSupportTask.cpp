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
#include <QDomDocument>

#include "BlastXPlusSupportTask.h"
#include "BlastPlusSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>

#include <U2Core/CreateAnnotationTask.h>

namespace U2 {

ExternalToolRunTask* BlastXPlusSupportTask::createBlastPlusTask(){

    QStringList arguments;
    //arguments <<"-p"<< settings.programName; //taskname
//    if(!settings.filter.isEmpty()){
//        arguments <<"-F"<<settings.filter;
//    }
    arguments <<"-db"<< settings.databaseNameAndPath;
    arguments <<"-evalue"<< QString::number(settings.expectValue);
//    arguments <<"-task"<< (settings.megablast ? "megablast" : "blastn");
    if(settings.wordSize <= 0){
        arguments <<"-word_size"<< "3";
    }else{
        arguments <<"-word_size"<< QString::number(settings.wordSize);
    }

    if (settings.directStrand == TriState_Yes) {
        arguments << "-strand" << "plus";
    } else if (settings.directStrand == TriState_No) {
        arguments << "-strand" << "minus";
    }

    if(!settings.isDefaultCosts){
        arguments <<"-gapopen"<< QString::number(settings.gapOpenCost);
        arguments <<"-gapextend"<< QString::number(settings.gapExtendCost);
    }
    if(settings.isNucleotideSeq && (!settings.isDefautScores)){
        assert(false);
        arguments <<"-penalty"<< QString::number(settings.mismatchPenalty);
        arguments <<"-reward"<< QString::number(settings.matchReward);
    }else{
        if(!settings.isDefaultMatrix){
            arguments <<"-matrix"<< settings.matrix;
        }
    }
    if(settings.numberOfHits != 0){
        arguments <<"-culling_limit" << QString::number(settings.numberOfHits); //???
    }
    if(!settings.isGappedAlignment){
        arguments << "-ungapped";
    }

    arguments <<"-query"<< url;
    if(settings.xDropoffGA != 15)
    {
        arguments << "-xdrop_gap" << QString::number(settings.xDropoffGA);
    }
    if(settings.xDropoffFGA != 25)
    {
        arguments << "-xdrop_gap_final" << QString::number(settings.xDropoffFGA);
    }

    if(settings.xDropoffUnGA != 7)
    {
        arguments << "-xdrop_ungap" << QString::number(settings.xDropoffUnGA);
    }
    if(!settings.isDefaultThreshold){
        arguments << "-threshold" << QString::number(settings.threshold);
    }
    if(settings.windowSize !=40)
    {
        arguments << "-window_size" << QString::number(settings.windowSize);
    }
    if (!settings.compStats.isEmpty()) {
        arguments << "-comp_based_stats" << settings.compStats;
    }
    arguments <<"-num_threads"<< QString::number(settings.numberOfProcessors);
    arguments <<"-outfmt"<< QString::number(settings.outputType);//"5";//Set output file format to xml
    if(settings.outputOriginalFile.isEmpty()){
        arguments <<"-out"<< url+".xml";
        settings.outputOriginalFile = url+".xml";
    }else{
        arguments <<"-out"<< settings.outputOriginalFile;
    }

    algoLog.trace("BlastX+ arguments: "+arguments.join(" "));
    QString workingDirectory = QFileInfo(url).absolutePath();
    ExternalToolRunTask* runTask = new ExternalToolRunTask(BlastPlusSupport::ET_BLASTX_ID, arguments, new ExternalToolLogParser(), workingDirectory);
    setListenerForTask(runTask);
    return runTask;
}
}//namespace
