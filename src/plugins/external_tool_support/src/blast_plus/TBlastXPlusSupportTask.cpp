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

#include "TBlastXPlusSupportTask.h"
#include "BlastPlusSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

ExternalToolRunTask* TBlastXPlusSupportTask::createBlastPlusTask(){

    QStringList arguments;

    arguments <<"-db"<< settings.databaseNameAndPath;
    arguments <<"-evalue"<< QString::number(settings.expectValue);
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

    if(settings.isNucleotideSeq && (!settings.isDefautScores)){
        assert(false);
        coreLog.error(tr("Unexpected settings combination"));
    }else{
        if(!settings.isDefaultMatrix){
            arguments <<"-matrix"<< settings.matrix;
        }
    }
    if(settings.numberOfHits != 0){
        arguments <<"-culling_limit" << QString::number(settings.numberOfHits); //???
    }
    arguments <<"-query"<< url;

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
    arguments <<"-num_threads"<< QString::number(settings.numberOfProcessors);
    arguments <<"-outfmt"<< QString::number(settings.outputType);//"5";//Set output file format to xml
    if(settings.outputOriginalFile.isEmpty()){
        arguments <<"-out"<< url+".xml";
        settings.outputOriginalFile = url+".xml";
    }else{
        arguments <<"-out"<< settings.outputOriginalFile;
    }


    algoLog.trace("TBlastX+ arguments: "+arguments.join(" "));
    QString workingDirectory=QFileInfo(url).absolutePath();

    ExternalToolRunTask* runTask = new ExternalToolRunTask(BlastPlusSupport::ET_TBLASTX_ID, arguments, new ExternalToolLogParser(), workingDirectory);
    setListenerForTask(runTask);
    return runTask;
}

} // namespace U2

