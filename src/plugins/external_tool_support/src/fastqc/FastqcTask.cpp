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

#include "FastqcSupport.h"
#include "FastqcTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//FastQCParser

const QMap<FastQCParser::ErrorType, QString> FastQCParser::initWellKnownErrors() {
    QMap<ErrorType, QString> errors;
    errors.insertMulti(Common, "ERROR");
    errors.insertMulti(Common, "Failed to process file");
    errors.insertMulti(Multiline, "uk.ac.babraham.FastQC.Sequence.SequenceFormatException");
    errors.insertMulti(Multiline, "didn't start with '+'");

    return errors;
}

const QMap<FastQCParser::ErrorType, QString> FastQCParser::WELL_KNOWN_ERRORS = initWellKnownErrors();

FastQCParser::FastQCParser(const QString& _inputFile)
    : ExternalToolLogParser(false),
      inputFile(_inputFile),
      progress(-1)
{

}

int FastQCParser::getProgress() {
    //parsing Approx 20% complete for filename
    if(!lastPartOfLog.isEmpty()) {
        QString lastMessage = lastPartOfLog.last();
        QRegExp rx("Approx (\\d+)% complete");
        if(lastMessage.contains(rx)) {
            SAFE_POINT(rx.indexIn(lastMessage) > -1, "bad progress index", 0);
            int step = rx.cap(1).toInt();
            if(step > progress) {
                return progress = step;
            }
        }
    }
    return progress;
}

void FastQCParser::processErrLine(const QString &line) {
    if (isCommonError(line)){
        ExternalToolLogParser::setLastError(tr("FastQC: %1").arg(line));
    } else if (isMultiLineError(line)) {
        setLastError(tr("FastQC failed to process input file '%1'. Make sure each read takes exactly four lines.")
                     .arg(inputFile));
    }
}

void FastQCParser::setLastError(const QString &value) {
    ExternalToolLogParser::setLastError(value);
    foreach(const QString& buf, lastPartOfLog) {
        CHECK_CONTINUE(!buf.isEmpty());

        ioLog.trace(buf);
    }
}

bool FastQCParser::isCommonError(const QString& err)  const {
    foreach(const QString& commonError, WELL_KNOWN_ERRORS.values(Common)) {
        CHECK_CONTINUE(err.contains(commonError, Qt::CaseInsensitive));

        return true;
    }

    return false;
}

bool FastQCParser::isMultiLineError(const QString& err) {
    QStringList multiLineErrors = WELL_KNOWN_ERRORS.values(Multiline);
    if (err.contains(multiLineErrors.first()) && err.contains(multiLineErrors.last())) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
//FastQCTask
FastQCTask::FastQCTask(const FastQCSetting &settings)
:ExternalToolSupportTask(QString("FastQC for %1").arg(settings.inputUrl), TaskFlags_FOSE_COSC | TaskFlag_MinimizeSubtaskErrorText)
, settings(settings), temporaryDir(AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath() + "/")
{

}

void FastQCTask::prepare(){
    if (settings.inputUrl.isEmpty()){
        setError(tr("No input URL"));
        return ;
    }

    if (QFileInfo(settings.inputUrl).size() == 0) {
        setError(tr("The input file '%1' is empty.").arg(settings.inputUrl));
        return;
    }

    const QDir outDir = QFileInfo(settings.outDir).absoluteDir();
    if (!outDir.exists()) {
        setError(tr("Folder does not exist: %1").arg(outDir.absolutePath()));
        return ;
    }

    const QStringList args = getParameters(stateInfo);
    CHECK_OP(stateInfo, );
    ExternalToolRunTask* etTask = new ExternalToolRunTask(FastQCSupport::ET_FASTQC_ID, args, new FastQCParser(settings.inputUrl), temporaryDir.path());
    setListenerForTask(etTask);
    addSubTask(etTask);
}

void FastQCTask::run(){
    CHECK_OP(stateInfo, );

    QString resFileUrl = getResFileUrl();
    const QFileInfo resFile(resFileUrl);
    if (!resFile.exists()) {
        setError(tr("Result file does not exist: %1. See the log for details.").arg(resFile.absoluteFilePath()));
        return ;
    }
    if (!settings.fileName.isEmpty()) {
        QFileInfo fi(settings.fileName);
        resultUrl = GUrlUtils::rollFileName(settings.outDir + QDir::separator() + fi.baseName() + ".html", "_");
    } else {
        QFileInfo fi(settings.inputUrl);
        resultUrl = GUrlUtils::rollFileName(settings.outDir + QDir::separator() + fi.baseName() + "_fastqc.html", "_");
    }
    QFile result(resFileUrl);
    if (!result.rename(resultUrl)) {
        setError(tr("Unable to move result file from temporary directory to desired location: %1.").arg(resultUrl));
    }
}

QString FastQCTask::getResFileUrl() const{
    QString res;

    QFileInfo fi(settings.inputUrl);
    QString name = fi.fileName();
    //taken from FastQC source "OfflineRunner.java"
    //.replaceAll("\\.gz$","").replaceAll("\\.bz2$","").replaceAll("\\.txt$","").replaceAll("\\.fastq$", "").replaceAll("\\.fq$", "").replaceAll("\\.csfastq$", "").replaceAll("\\.sam$", "").replaceAll("\\.bam$", "")+"_fastqc.html");
    name.replace(QRegExp(".gz$"),"")
            .replace(QRegExp(".bz2$"),"")
            .replace(QRegExp(".txt$"),"")
            .replace(QRegExp(".fastq$"), "")
            .replace(QRegExp(".csfastq$"), "")
            .replace(QRegExp(".sam$"), "")
            .replace(QRegExp(".bam$"), "");
    name += "_fastqc.html";

    res = temporaryDir.path() + QDir::separator() + name;
    return res;
}

QStringList FastQCTask::getParameters(U2OpStatus & /*os*/) const{
    QStringList res;

    res << QString("-o");
    res << temporaryDir.path();


    if(!settings.conts.isEmpty()){
        res << QString("-c");
        res << settings.conts;
    }

    if(!settings.adapters.isEmpty()){
        res << QString("-a");
        res << settings.adapters;
    }

    ExternalTool *java = FastQCSupport::getJava();
    CHECK(NULL != java, res);
    res << QString("-java");
    res << java->getPath();

    res << settings.inputUrl;

    return res;
}

} //namespace U2
