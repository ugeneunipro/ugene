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

#include "FastqcTask.h"

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "FastqcSupport.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// FastQCParser

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
      progress(-1) {
}

int FastQCParser::getProgress() {
    // parsing Approx 20% complete for filename
    if (!lastPartOfLog.isEmpty()) {
        QString lastMessage = lastPartOfLog.last();
        QRegExp rx("Approx (\\d+)% complete");
        if (lastMessage.contains(rx)) {
            SAFE_POINT(rx.indexIn(lastMessage) > -1, "bad progress index", 0);
            int step = rx.cap(1).toInt();
            if (step > progress) {
                return progress = step;
            }
        }
    }
    return progress;
}

void FastQCParser::processErrLine(const QString& line) {
    if (isCommonError(line)) {
        ExternalToolLogParser::setLastError(tr("FastQC: %1").arg(line));
    } else if (isMultiLineError(line)) {
        setLastError(tr("FastQC failed to process input file '%1'. Make sure each read takes exactly four lines.")
                         .arg(inputFile));
    }
}

void FastQCParser::setLastError(const QString& value) {
    ExternalToolLogParser::setLastError(value);
    foreach (const QString& buf, lastPartOfLog) {
        CHECK_CONTINUE(!buf.isEmpty());

        ioLog.trace(buf);
    }
}

bool FastQCParser::isCommonError(const QString& err) const {
    foreach (const QString& commonError, WELL_KNOWN_ERRORS.values(Common)) {
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
// FastQCTask
FastQCTask::FastQCTask(const FastQCSetting& settings)
    : ExternalToolSupportTask(QString("FastQC for %1").arg(settings.inputFileUrl), TaskFlags_FOSE_COSC | TaskFlag_MinimizeSubtaskErrorText),
      settings(settings),
      temporaryDir(AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath() + "/") {
}

void FastQCTask::prepare() {
    if (settings.inputFileUrl.isEmpty()) {
        setError(tr("No input URL"));
        return;
    }

    if (QFileInfo(settings.inputFileUrl).size() == 0) {
        setError(tr("The input file '%1' is empty.").arg(settings.inputFileUrl));
        return;
    }

    QDir outputDir = QFileInfo(settings.outputDirUrl).absoluteDir();
    if (!outputDir.exists()) {
        setError(tr("Folder does not exist: %1").arg(outputDir.absolutePath()));
        return;
    }

    QStringList args = getParameters(stateInfo);
    CHECK_OP(stateInfo, );
    auto externalToolTask = new ExternalToolRunTask(FastQCSupport::ET_FASTQC_ID, args, new FastQCParser(settings.inputFileUrl), temporaryDir.path());
    setListenerForTask(externalToolTask);
    addSubTask(externalToolTask);
}

void FastQCTask::run() {
    CHECK_OP(stateInfo, );

    QString tmpResultFileUrl = getTmpResultFileUrl();
    QFileInfo tmpResultFileInfo(tmpResultFileUrl);
    if (!tmpResultFileInfo.exists()) {
        setError(tr("Result file does not exist: %1. See the log for details.").arg(tmpResultFileInfo.absoluteFilePath()));
        return;
    }
    if (!settings.outputFileNameOverride.isEmpty()) {
        QFileInfo overrideFileInfo(settings.outputFileNameOverride);
        resultFileUrl = GUrlUtils::rollFileName(settings.outputDirUrl + QDir::separator() + overrideFileInfo.baseName() + ".html", "_");
    } else {
        QFileInfo inputFileInfo(settings.inputFileUrl);
        resultFileUrl = GUrlUtils::rollFileName(settings.outputDirUrl + QDir::separator() + inputFileInfo.baseName() + "_fastqc.html", "_");
    }
    QFile tmpResultFile(tmpResultFileUrl);
    if (!tmpResultFile.rename(resultFileUrl)) {
        setError(tr("Unable to move result file from temporary directory to desired location: %1.").arg(resultFileUrl));
    }
}

QString FastQCTask::getTmpResultFileUrl() const {
    // FastQC source "OfflineRunner.java":
    //  String fileName = file.getFile().getName().replaceAll("stdin:","").replaceAll("\\.gz$","").replaceAll("\\.bz2$","")
    //   .replaceAll("\\.txt$","").replaceAll("\\.fastq$", "").replaceAll("\\.fq$", "").replaceAll("\\.csfastq$", "")
    //   .replaceAll("\\.sam$", "").replaceAll("\\.bam$", "")+"_fastqc.html";
    QFileInfo inputFileInfo(settings.inputFileUrl);
    QString resultFileName = inputFileInfo.fileName()
                                 .replace(QRegExp(".gz$"), "")
                                 .replace(QRegExp(".bz2$"), "")
                                 .replace(QRegExp(".txt$"), "")
                                 .replace(QRegExp(".fastq$"), "")
                                 .replace(QRegExp(".fq$"), "")
                                 .replace(QRegExp(".csfastq$"), "")
                                 .replace(QRegExp(".sam$"), "")
                                 .replace(QRegExp(".bam$"), "") +
                             "_fastqc.html";
    return temporaryDir.path() + QDir::separator() + resultFileName;
}

QStringList FastQCTask::getParameters(U2OpStatus& os) const {
    QStringList res;

    res << QString("-o");
    res << temporaryDir.path();

    if (!settings.contaminantsFileUrl.isEmpty()) {
        res << QString("-c");
        res << settings.contaminantsFileUrl;
    }

    if (!settings.adaptersFileUrl.isEmpty()) {
        res << QString("-a");
        res << settings.adaptersFileUrl;
    }

    ExternalTool* java = FastQCSupport::getJava();
    CHECK_EXT(java != nullptr, os.setError(tr("Java external tool is not found")), res);

    res << QString("-java");
    res << java->getPath();
    res << settings.inputFileUrl;
    return res;
}

}  // namespace U2
