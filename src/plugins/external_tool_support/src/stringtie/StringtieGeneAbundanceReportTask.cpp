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
#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "StringtieGeneAbundanceReportTask.h"

namespace U2 {
namespace LocalWorkflow {

const int StringtieGeneAbundanceReportTask::BUFF_SIZE = (4 * 1024 * 1024);
const QString StringtieGeneAbundanceReportTask::inputDelimiter = "\t";
const QString StringtieGeneAbundanceReportTask::outputDelimiter = "\t";
const QString StringtieGeneAbundanceReportTask::columnName = "FPKM";

// The comparator must be a static function,
// Otherwise it will not be possible to pass it as an argument
static bool compareFpkm(const QStringList& first, const QStringList& second) {
    return first[0] < second[0];
}

StringtieGeneAbundanceReportTask::StringtieGeneAbundanceReportTask(const QStringList &_stringtieReports,
                                                                   const QString &_reportUrl,
                                                                   const QString &_workingDir)
    : Task(tr("StringTie Gene Abundance Report Task"), TaskFlag_None),
      stringtieReports(_stringtieReports),
      workingDir(_workingDir),
      reportUrl(_reportUrl)
{
    if (reportUrl.isEmpty()) {
        reportUrl = "StringTie_report.txt";
    }
    GCOUNTER(cvar, tvar, "StringtieGeneAbundanceReportTask");
    SAFE_POINT_EXT(!reportUrl.isEmpty(), setError("Report URL is empty"), );
}

const QString &StringtieGeneAbundanceReportTask::getReportUrl() const {
    return reportUrl;
}

void StringtieGeneAbundanceReportTask::run() {
    CHECK((stringtieReports.size() > 0), );

    if (!QFileInfo(reportUrl).isAbsolute()) {
        QString tmpDir = FileAndDirectoryUtils::createWorkingDir(workingDir,
                                                                 FileAndDirectoryUtils::WORKFLOW_INTERNAL,
                                                                 "",
                                                                 workingDir);
        CHECK_EXT(QDir(tmpDir).exists(), setError(tr("The directory \"%1\" did not created").arg(tmpDir)), );
        reportUrl = tmpDir + reportUrl;
    }
    reportUrl = GUrlUtils::rollFileName(reportUrl, "_");

    QFile reportFile(reportUrl);
    if ((reportFile.exists() && reportFile.open(QIODevice::Truncate))
            || (!reportFile.exists() && reportFile.open(QIODevice::ReadWrite))) {
        reportFile.close();
    } else {
        setError(reportFile.errorString());
    }
    CHECK_OP(stateInfo, );

    QString runDir = FileAndDirectoryUtils::createWorkingDir(workingDir,
                                                             FileAndDirectoryUtils::WORKFLOW_INTERNAL,
                                                             "",
                                                             workingDir);
    CHECK_EXT(QDir(runDir).exists(), setError(tr("The directory \"%1\" did not created").arg(runDir)), );

    // 1st - sort&shrink every input file to temp file
    QMap<QString,QString> mapFileReports;
    foreach (QString tsvFile, stringtieReports) {
        QString tempFile = sortAndShrinkToTemp(tsvFile, runDir);
        mapFileReports[tempFile] = tsvFile;
    }
    CHECK_OP(stateInfo, );

    // 2nd - merge files to reportUrl
    mergeFpkmToReportUrl(mapFileReports, reportUrl);
    CHECK_OP(stateInfo, );

    // Remove FPKM subdir
    QDir fpkmDir(runDir + "/" + columnName + "/");
    if (fpkmDir.exists()) {
        fpkmDir.removeRecursively();
    }
}

bool StringtieGeneAbundanceReportTask::mergeFpkmToReportUrl(QMap<QString,QString> mapFiles, QString reportUrl) {
    int valueCount = mapFiles.size();
    QMap<QString, QVector<QString> > map;
    int fileIndex = 0;

    foreach (QString tempFile, mapFiles.keys()) {
        QString tsvFile = mapFiles[tempFile];
        GUrl url(tempFile);
        IOAdapterId ioId = IOAdapterUtils::url2io(url);
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
        QScopedPointer<IOAdapter> io(iof->createIOAdapter());

        if (!io->open(url, IOAdapterMode_Read)) {
            setError(L10N::errorOpeningFileRead(url));
            return false;
        }
        bool notEmpty = true;
        QByteArray block(BUFF_SIZE, '\0');
        bool terminatorFound = false;
        // skip header
        int blockLen = io->readLine(block.data(), BUFF_SIZE, &terminatorFound);
        CHECK_EXT(blockLen <= 0 || terminatorFound,
            setError(tr("Too long line while reading a file: %1").arg(tempFile)),
            false);

        while (notEmpty) {
            blockLen = io->readLine(block.data(), BUFF_SIZE, &terminatorFound);
            if (blockLen > 0) {
                CHECK_EXT(terminatorFound,
                          setError(tr("Too long line while reading a file: %1").arg(tempFile)),
                          false);
                QString line = QString::fromLocal8Bit(block.data(), blockLen);
                QStringList buf = line.split(inputDelimiter, QString::KeepEmptyParts);
                CHECK_EXT(buf.size() == 3,
                          setError(tr("Bad line format of input: \"%1\"").arg(line)),
                          false);
                QString key = buf[0] + outputDelimiter + buf[1];
                QVector<QString>& values = map[key];
                if (values.size() != valueCount) {
                    values.resize(valueCount);
                }
                if (buf[2].isEmpty()) {
                    buf[2] = "n/a";
                }
                values[fileIndex] = buf[2];
            } else {
                notEmpty = false;
            }
        }
        io->close();
        fileIndex++;
    }

    // save merged file
    QFile file(reportUrl);
    if (!file.open(QIODevice::Append)) {
        setError(tr("Cannot open a file: %1").arg(reportUrl));
        return false;
    }
    QTextStream out(&file);

    // header
    out << "Gene ID" << outputDelimiter << "Gene Name";
    foreach (QString tempFile, mapFiles.keys()) {
        QString tsvFile = mapFiles[tempFile];
        GUrl url(tsvFile);
        QString head = url.baseFileName();
        head = head.remove(QRegExp("\\.tab$", Qt::CaseInsensitive))
                .remove(QRegExp("_gene_abund$", Qt::CaseInsensitive))
                .remove(QRegExp("_abund$", Qt::CaseInsensitive));
        out << outputDelimiter << head;
    }
    out << "\n";

    //values
    foreach(const QString& key, map.keys()) {
        QVector<QString> values = map[key];
        out << key;
        foreach (const QString val, values) {
            out << outputDelimiter << val;
        }
        out << "\n";
    }
    file.close();

    return true;
}

QString StringtieGeneAbundanceReportTask::sortAndShrinkToTemp(QString tsvFile, QString runDir) {
    GUrl url(tsvFile);
    IOAdapterId ioId = IOAdapterUtils::url2io(url);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());

    if (!io->open(url, IOAdapterMode_Read)) {
        setError(L10N::errorOpeningFileRead(url));
        return NULL;
    }

    QByteArray block(BUFF_SIZE, '\0');
    int blockLen = 0;
    QString text;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        int sizeBefore = text.length();
        QString line = QString::fromLocal8Bit(block.data(), blockLen);
        text.append(line);
        if (text.length() != sizeBefore + line.length()) {
            setError(L10N::errorReadingFile(url));
        }
    }
    io->close();

    // parse text to list of lists
    QList<QStringList> parsedLines = parseLinesIntoTokens(text);
    CHECK_EXT(parsedLines.size() > 0,
              setError(tr("Unexpected error while parsing input data")),
              NULL);

    // header
    QStringList header = parsedLines[0];
    int indexFpkm = header.indexOf(columnName);
    CHECK_EXT(indexFpkm != -1,
              setError(tr("Bad file format, there is no %2 column: \"%1\"").arg(tsvFile).arg(columnName)),
              NULL);
    QString fileFpkm = runDir + "/" + columnName + "/" + url.baseFileName() + ".fpkm";
    fileFpkm = GUrlUtils::rollFileName(fileFpkm, "_");
    FileAndDirectoryUtils::createWorkingDir(fileFpkm,
                                            FileAndDirectoryUtils::FILE_DIRECTORY,
                                            "",
                                            workingDir);

    // sort
    QList<QStringList>::iterator itBegin = parsedLines.begin();
    QList<QStringList>::iterator itEnd = parsedLines.end();
    itBegin++;
    std::sort(itBegin, itEnd, compareFpkm);

    // save sorted file
    QFile file(fileFpkm);
    if (!file.open(QIODevice::Append)) {
        setError(tr("Cannot open a file: %1\nError is :").arg(fileFpkm).arg(L10N::errorOpeningFileWrite(fileFpkm)));
        return NULL;
    }
    QTextStream out(&file);
    foreach (const QStringList line, parsedLines) {
        CHECK_EXT(line.size() >= indexFpkm,
                  setError(tr("Bad line format of input: \"%1\"").arg(line.join("\t"))),
                  NULL);

        out << line[0] << inputDelimiter << line[1] << inputDelimiter << line[indexFpkm] << "\n";
    }
    file.close();

    return fileFpkm;
}

QList<QStringList> StringtieGeneAbundanceReportTask::parseLinesIntoTokens(const QString& text) {
    QList<QStringList> result;
    QStringList lines = text.split('\n', QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        QStringList tokens = line.split(inputDelimiter, QString::KeepEmptyParts);
        result.append(tokens);
    }
    return result;
}

}   // namespace LocalWorkflow
}   // namespace U2
