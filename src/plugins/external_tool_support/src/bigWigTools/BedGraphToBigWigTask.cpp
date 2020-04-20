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

#include <QDir>

#include "BigWigSupport.h"
#include "BedGraphToBigWigTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//BedGraphToBigWigParser
BedGraphToBigWigParser::BedGraphToBigWigParser()
    :ExternalToolLogParser() {

}

void BedGraphToBigWigParser::parseOutput( const QString& partOfLog ){
    ExternalToolLogParser::parseOutput(partOfLog);
}

void BedGraphToBigWigParser::parseErrOutput( const QString& partOfLog ){
    lastPartOfLog=partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first()=lastErrLine+lastPartOfLog.first();
    lastErrLine=lastPartOfLog.takeLast();
    foreach(QString buf, lastPartOfLog){
            if(buf.contains("ERROR", Qt::CaseInsensitive)){
                    coreLog.error("bedGraphToBigWig: " + buf);
            }
    }
}


//////////////////////////////////////////////////////////////////////////
//BedGraphToBigWigTask
BedGraphToBigWigTask::BedGraphToBigWigTask(const BedGraphToBigWigSetting &settings)
:ExternalToolSupportTask(QString("bedGrapthToBigWig for %1").arg(settings.inputUrl), TaskFlags_FOSE_COSC)
,settings(settings)
{

}

void BedGraphToBigWigTask::prepare(){
    if (settings.inputUrl.isEmpty()){
        setError("No input URL");
        return ;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError("Folder does not exist: " + outDir.absolutePath());
        return ;
    }

    if(settings.genomePath.isEmpty()){
        setError("No path to genome lengths");
        return ;
    }

    const QStringList args = getParameters(stateInfo);
    CHECK_OP(stateInfo, );

    ExternalToolRunTask* etTask = new ExternalToolRunTask(BigWigSupport::ET_BIGWIG_ID, args, new BedGraphToBigWigParser(), settings.outDir);
    setListenerForTask(etTask);
    addSubTask(etTask);
}

void BedGraphToBigWigTask::run(){
    CHECK_OP(stateInfo, );

    resultUrl = settings.outDir + settings.outName;
}

QStringList BedGraphToBigWigTask::getParameters(U2OpStatus &/*os*/){
    QStringList res;

    res << settings.inputUrl;
    res << settings.genomePath;
    res << settings.outDir + settings.outName;
    res << QString("-blockSize=%1").arg(settings.blockSize);
    res << QString("-itemsPerSlot=%1").arg(settings.itemsPerSlot);
    if(settings.uncompressed){
        res << QString("-unc");
    }

    return res;
}

} //namespace U2
