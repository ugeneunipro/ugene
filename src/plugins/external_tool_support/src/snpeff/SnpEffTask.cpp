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

#include "SnpEffSupport.h"
#include "SnpEffTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

#define RES_FILE_BASE "snp_eff"
#define GENE_FILE "snpEff_genes.txt"
#define SUMMARY_FILE "snpEff_summary.html"

//////////////////////////////////////////////////////////////////////////
//SnpEffParser

const QStringList SnpEffParser::stringsToIgnore = SnpEffParser::initStringsToIgnore();

SnpEffParser::SnpEffParser(const QString &genome)
    : ExternalToolLogParser(),
      genome(genome) {

}

void SnpEffParser::parseOutput( const QString& partOfLog ) {
    lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));

    foreach(const QString &buf, lastPartOfLog) {
        if (buf.contains("Could not reserve enough space for object heap", Qt::CaseInsensitive) ||
            buf.contains("Invalid maximum heap size", Qt::CaseInsensitive) ||
            buf.contains("Unable to allocate", Qt::CaseInsensitive) ||
            buf.contains("Failed to allocate", Qt::CaseInsensitive)) {
            setLastError(tr("A problem occurred during allocating memory for running SnpEff. Check the \"Tasks memory limit\" parameter in the UGENE Application Settings.It is recommended to set this value to the available RAM on the computer."));
        }
    }
}

void SnpEffParser::parseErrOutput( const QString& partOfLog ) {
    lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first() = lastErrLine+lastPartOfLog.first();
    lastErrLine = lastPartOfLog.takeLast();

    foreach (const QString &buf, lastPartOfLog) {
        if (stringsToIgnore.contains(buf)) {
            continue;
        }

        if (buf.contains("java.lang.OutOfMemoryError")) {
            setLastError(tr("There is not enough memory to complete the SnpEff execution.It is recommended to run SnpEff on a computer with RAM 4Gb or more."));
            continue;
        }
        if (buf.contains("Could not reserve enough space for object heap", Qt::CaseInsensitive) ||
            buf.contains("Invalid maximum heap size", Qt::CaseInsensitive) ||
            buf.contains("Unable to allocate", Qt::CaseInsensitive)) {
            setLastError(tr("A problem occurred during allocating memory for running SnpEff. Check the \"Tasks memory limit\" parameter in the UGENE Application Settings.It is recommended to set this value to the available RAM on the computer."));
            continue;
        }

        if (buf.contains("ERROR while connecting to http://downloads.sourceforge.net/project/snpeff/", Qt::CaseInsensitive)) {
            setLastError(tr("Failed to download SnpEff database. Check your internet connection."));
            continue;
        }

        if (!genome.isEmpty() && buf.contains("Property: '" + genome + ".genome' not found")) {
            setLastError(tr("Genome database '%1' is not found.").arg(genome));
            continue;
        }

        if (buf.contains("ERROR", Qt::CaseInsensitive)) {
            if (buf.startsWith("#")) {
                coreLog.details("SnpEff notificates about genome database error: " + buf);
            } else {
                coreLog.error("SnpEff: " + buf);
            }
        } else if (buf.contains("warning", Qt::CaseInsensitive) && buf.startsWith("#")) {
            coreLog.details("SnpEff notificates about genome database error: " + buf);
        }
    }
}

QStringList SnpEffParser::initStringsToIgnore() {
    QStringList result;

    result << "WARNINGS: Some warning were detected";
    result << "Warning type\tNumber of warnings";
    result << "ERRORS: Some errors were detected";
    result << "Error type\tNumber of errors";
    result << "Error: A fatal exception has occurred. Program will exit.";
    result << "Error: Could not create the Java Virtual Machine.";

    return result;
}


//////////////////////////////////////////////////////////////////////////
//SnpEffTask
SnpEffTask::SnpEffTask(const SnpEffSetting &settings)
:ExternalToolSupportTask(QString("snpEff for %1").arg(settings.inputUrl), TaskFlags_FOSE_COSC)
,settings(settings)
{

}

void SnpEffTask::prepare(){

    if (settings.inputUrl.isEmpty()){
        setError("No input URL");
        return ;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError("Folder does not exist: " + outDir.absolutePath());
        return ;
    }

    if(settings.genome.isEmpty()){
        setError("No path to genome lengths");
        return ;
    }

    const QStringList args = getParameters(stateInfo);
    CHECK_OP(stateInfo, );

    ExternalToolRunTask* etTask = new ExternalToolRunTask(SnpEffSupport::ET_SNPEFF_ID, args, new SnpEffParser(settings.genome), settings.outDir, QStringList(), QString(), true);
    setListenerForTask(etTask);
    etTask->setStandartOutputFile( getResFileUrl() );
    addSubTask(etTask);
}

void SnpEffTask::run(){
    CHECK_OP(stateInfo, );

    const QFileInfo resFile(getResFileUrl());
    if (!resFile.exists()) {
        setError("Result file does not exist: " + resFile.absoluteFilePath());
        return ;
    }
    resultUrl = getResFileUrl();
}

QString SnpEffTask::getSummaryUrl(){
    QString res = "";
    const QFileInfo resFile = QFileInfo(settings.outDir + "/" + SUMMARY_FILE);
    if (!resFile.exists()) {
        return res;
    }
    res = settings.outDir + "/" + SUMMARY_FILE;
    return res;
}

QString SnpEffTask::getResFileUrl(){
    QString res = "";
    res = settings.outDir + "/" + RES_FILE_BASE + "." + settings.outFormat;
    return res;
}

QString SnpEffTask::getDataPath() const{
    CHECK(NULL != AppContext::getAppSettings(), QString());
    CHECK(NULL != AppContext::getAppSettings()->getUserAppsSettings(), QString());
    CHECK(NULL != AppContext::getExternalToolRegistry(), QString());
    CHECK(NULL != AppContext::getExternalToolRegistry()->getById(SnpEffSupport::ET_SNPEFF_ID), QString());

    // The next part is for VEME conferention
    // It is done instead of UGENE-5318 resolving
    // Remove it after 1.24 version release
    if (settings.genome == "NC_002549") {
        return QFileInfo(AppContext::getExternalToolRegistry()->getById(SnpEffSupport::ET_SNPEFF_ID)->getPath()).dir().absolutePath() + "/data";
    }
    return AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath() + "/" + "snpeff_data_" + AppContext::getExternalToolRegistry()->getById(SnpEffSupport::ET_SNPEFF_ID)->getVersion();
}

QStringList SnpEffTask::getParameters(U2OpStatus & os) const{
    QStringList res;

    res << QString("-dataDir");

    QString additionalSlash("");
#ifdef Q_OS_WIN
    additionalSlash = "/";
#endif
    QString dataPath=getDataPath();
    if (dataPath.isEmpty()){
        os.setError(tr("SNPEff dataDir is not initialized."));
    }else{
        res << additionalSlash + dataPath;
    }

    res << QString("-i");
    res << settings.inFormat;

    res << QString("-o");
    res << settings.outFormat;

    res << QString("-upDownStreamLen");
    res << settings.updownLength;

    if(settings.canon){
        res << QString("-canon");
    }

    if(settings.hgvs){
        res << QString("-hgvs");
    }

    if(settings.lof){
        res << QString("-lof");
    }

    if(settings.motif){
        res << QString("-motif");
    }

    res << QString("-v");

    res << settings.genome;

    res << settings.inputUrl;

    return res;
}

} //namespace U2

