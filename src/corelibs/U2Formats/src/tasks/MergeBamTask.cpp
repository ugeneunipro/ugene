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
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/BaseDocumentFormats.h>

#include <U2Formats/BAMUtils.h>

#include "MergeBamTask.h"
#include "DocumentFormatUtils.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//MergeBamTask
MergeBamTask::MergeBamTask(const QStringList& urls, const QString &dir, const QString & outName, bool sortInputBams)
: Task(DocumentFormatUtils::tr("Merge BAM files with SAMTools merge"), TaskFlags_FOSCOE)
, outputName(outName)
, workingDir(dir)
, targetUrl("")
, bamUrls(urls)
, sortInputBams(sortInputBams)
{
    if (!workingDir.endsWith("/") && !workingDir.endsWith("\\")) {
        this->workingDir += "/";
    }
    if(outputName.isEmpty()){
        outputName = "merged.bam";
    }
}

QString MergeBamTask::getResult() const {
    return targetUrl;
}

void cleanupTempDir(const QStringList &tempDirFiles) {
    foreach(const QString& url, tempDirFiles) {
        QFile toDelete(url);
        if (toDelete.exists(url)) {
            toDelete.remove(url);
        }
    }
}

void MergeBamTask::run(){
    if(bamUrls.isEmpty()){
        stateInfo.setError("No BAM files to merge");
        return;
    }
    targetUrl = workingDir + outputName;
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    if (sortInputBams) {
        QStringList sortedNamesList;
        foreach(const QString& url, bamUrls) {
            QFileInfo fi(url);
            QString sortedName = tmpDirPath + "/" + fi.completeBaseName() + "_sorted.bam";
            sortedNamesList.append(sortedName);
            BAMUtils::sortBam(url, sortedName, stateInfo);
            if (stateInfo.isCoR()) {
                cleanupTempDir(sortedNamesList);
                return;
            }
        }
        BAMUtils::mergeBam(sortedNamesList, targetUrl, stateInfo);
        cleanupTempDir(sortedNamesList);
    } else {
        BAMUtils::mergeBam(bamUrls, targetUrl, stateInfo);
    }

    CHECK_OP(stateInfo, );
    if (stateInfo.isCoR()) {
        return;
    }

    BAMUtils::createBamIndex(targetUrl, stateInfo);

}

} // U2
