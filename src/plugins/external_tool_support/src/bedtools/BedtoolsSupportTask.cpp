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

#include "BedtoolsSupport.h"
#include "BedtoolsSupportTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ExternalToolRunTask.h>

#include <QFileInfo>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//BAMBEDConvertFactory
bool BAMBEDConvertFactory::isCustomFormatTask( const QString& detectedFormat, const QString& targetFormat ) {
    if (detectedFormat == BaseDocumentFormats::BAM && targetFormat == BaseDocumentFormats::BED){
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//BamBedConversionTask
BamBedConversionTask::BamBedConversionTask( const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir )
:ConvertFileTask(sourceURL, detectedFormat, targetFormat, dir)
{

}

void BamBedConversionTask::prepare(){
    QString extension = ".bed";
    QString destURL = workingDir + QFileInfo(sourceURL.getURLString()).fileName() + extension;
    targetUrl = GUrlUtils::rollFileName(destURL, QSet<QString>());

    QStringList args;
    args << "bamtobed";
    args << "-i";
    args << sourceURL.getURLString();

    ExternalToolRunTask* etTask = new ExternalToolRunTask(BedtoolsSupport::ET_BEDTOOLS_ID, args, new ExternalToolLogParser(), workingDir);
    etTask->setStandartOutputFile(targetUrl);
    addSubTask(etTask);
}

void BamBedConversionTask::run(){

}

} //namespace U2
