/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
#include "MfoldTask.h"

#include <QByteArray>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QRegularExpression>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "MfoldSettings.h"
#include "MfoldSupport.h"

namespace U2 {
const QString outHtmlBasename = "out.html";
constexpr int maxHtmlLength = 100'000;

MfoldTask::MfoldTask(const QByteArray& seq,
                     const MfoldSettings& settings,
                     const MfoldSequenceInfo& seqInfo,
                     int windowWidth)
    : Task(tr("Predict and visualize hairpins with Mfold"),
           TaskFlags_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      seq(seq), settings(settings), seqInfo(seqInfo), windowWidth(windowWidth) {
    GCOUNTER(cvar, "mfold");
}

// todo what does this function return if tmp dir contains spaces or non-ascii
QString MfoldTask::getSeqFilePath() const {
    QString fileName = seqInfo.seqPath.baseFileName();
    QString inFilePath = cwd.getURLString() + '/' + fileName;
    return GUrlUtils::getLocalUrlFromUrl(inFilePath, fileName, ".txt", "");
}

void MfoldTask::prepare() {
    // todo decompose some code in this function to other functions
    // Check tool is ok.
    auto tool = AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID);
    CHECK_EXT(tool->isValid() && !tool->getPath().isEmpty(),
              setError(tr("Mfold tool is invalid, check it in settings")), );

    // Prepare cwd.
    cwd = ExternalToolSupportUtils::createTmpDir("mfold", stateInfo);
    CHECK_OP(stateInfo, );

    // Save sequence in cwd.
    QString seqPath = getSeqFilePath();
    CHECK_EXT(FileAndDirectoryUtils::storeTextToFile(seqPath, seq),
              setError(QString(tr("Unable to store input sequence to file `%1`")).arg(seqPath)), );  // todo check unicode space sequence name

    // Prepare script arguments.
    QStringList mfoldArgs = {
        "SEQ=" + seqPath,
        "RUN_TYPE=html",
        "NA=" + QString(seqInfo.isDna ? "DNA" : "RNA"),
        "LC=" + QString(seqInfo.isCircular ? "circular" : "linear"),
        "T=" + QString::number(settings.algoSettings.temperature),
        "P=" + QString::number(settings.algoSettings.percent),
        "NA_CONC=" + QString::number(settings.algoSettings.naConc),
        "MG_CONC=" + QString::number(settings.algoSettings.mgConc),
        "MAX=" + QString::number(settings.algoSettings.maxFold),
        "ROT_ANG=" + QString::number(settings.algoSettings.rotAng)};
    if (settings.algoSettings.window >= 0) {
        mfoldArgs += "W=" + QString::number(settings.algoSettings.window);
    }
    if (settings.algoSettings.maxBp > 0) {
        mfoldArgs += "MAXBP=" + QString::number(settings.algoSettings.maxBp);
    }
    if (settings.algoSettings.labFr >= 0) {
        mfoldArgs += "LAB_FR=" + QString::number(settings.algoSettings.labFr);
    }

    // Prepare out dir.
    auto curDt = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm-ss");
    auto suffixedPath = FileAndDirectoryUtils::getAbsolutePath(settings.outSettings.outPath + "/mfold/" + curDt);
    settings.outSettings.outPath = GUrlUtils::prepareDirLocation(GUrlUtils::rollFileName(suffixedPath, "_"), stateInfo);
    CHECK_OP(stateInfo, );

    // Prepare env args.
    StrStrMap envVars;
    QSize imgSize;
    // Mfold creates ps files. To convert them to PNG, we need to set width and height in Mfold script.
    // These imgs will definitely be used in HTML report of UGENE task. Qt supports a very limited portion of HTML
    // and does so with bugs https://bugreports.qt.io/browse/QTBUG-12283, so it cannot resize images to fit the screen.
    // We must set the correct width ourselves, otherwise img will go beyond the edges of the resulting table.
    // The only problem is with the width.

    // Mfold produces all images with a resolution of 792x1072. So the final width should be as follows:
    // 1. we will take the width of the image equal to 0.5 of report window width (we assume that the report window
    //     will be the same size as the SV window)
    // 2. limit it from below to 300
    // 3. limit it from above to 792 -- the default image width

    // Calculate the height so as to preserve the original proportions (792x1072).
    imgSize.setWidth(qBound(300, windowWidth / 2, 792));
    imgSize.setHeight(imgSize.width() * 1072 / 792);
    envVars["U2_GS_IMG_SIZE_FOR_UGENE_REPORT"] = QString("%1x%2").arg(imgSize.width()).arg(imgSize.height());
    envVars["U2_GS_IMG_OUT_PATH"] = settings.outSettings.outPath;
    envVars["U2_GS_IMG_DPI_FOR_OUT_REPORT"] = QString::number(settings.outSettings.dpi);
    auto etTask = new ExternalToolRunTask(MfoldSupport::ET_MFOLD_ID,
                                          mfoldArgs,
                                          new ExternalToolLogParser(),
                                          cwd.getURLString());
    etTask->setAdditionalEnvVariables(envVars);
    addSubTask(etTask);
}

void MfoldTask::run() {
    CHECK_OP(stateInfo, );

    QString seqPath = getSeqFilePath();
    // todo how to read them?
    // todo add more check for function return codes
    auto readFileInput = [this, &seqPath](const QString& ext) -> QString {
        auto filePath = seqPath + ext;
        QFile f(filePath);
        if (!f.exists() || !f.open(QIODevice::OpenModeFlag::ReadWrite)) {
            setError(QString(tr("Unable to open output file `%1`").arg(filePath)));
            return {};
        }
        QString content = f.readAll();
        f.close();
        return content;
    };

    // Read input.
    QString hairpinHtml = readFileInput(".det.html");  // html with hairpin info tables
    QString thermoHtml = readFileInput(".out.html");  // html with dG and dH info
    CHECK(!hasError(), );

    // Remove unnecessary data from the report.
    QString header =
        "<body bgcolor=\"#dfc890\" text=\"#000000\" link=\"#8e2323\" vlink=\"#2f4f4f\" alink=\"#00ffff\"><b>";
    thermoHtml.remove(0, thermoHtml.indexOf(header) + header.length());

    // Report parsing.
    QString hairpinHeader = "<table border=3 cellpadding=4>";
    auto hairpinStart = hairpinHtml.indexOf(hairpinHeader) + hairpinHeader.length();

    QString dgStart(" dG = ");
    int hairpinNum = 1;
    int hairpinCount = thermoHtml.count(dgStart);

    // Add task info table:
    // UGENE uses width=200 for printing task info table (status, time). We mimic a single style.
    // Second column will take up the entire remaining window width, plus subtract 50 pixels for padding and marging.
    QString info = "<table>"
        "<tr>"
            "<td >Sequence name:</td>"
            "<td>" + seqInfo.seqName + "</td>"
        "</tr>"
        "<tr>"
            "<td >File:</td>"
            "<td><a href=\"" + seqInfo.seqPath.getURLString() + "\">" + seqInfo.seqPath.getURLString() + "</a></td>"
        "</tr>"
        "<tr>"
            "<td >Region:</td>"
            "<td>" + U1AnnotationUtils::buildLocationString(settings.region->regions) + "</td>"
        "</tr>"
        "<tr>"
            "<td >Sequence type:</td>"
            "<td>" + (seqInfo.isCircular ? "Circular " : "Linear ") + (seqInfo.isDna ? "DNA" : "RNA") + "</td>"
        "</tr>"
        "<tr>"
            "<td >Temperature:</td>"
            "<td>" + QString::number(settings.algoSettings.temperature) + "&deg;C</td>"
        "</tr>"
        "<tr>"
            "<td >Percent suboptimality:</td>"
            "<td>" + QString::number(settings.algoSettings.percent) + "%</td>"
        "</tr>"
        "<tr>"
            "<td >Ionic conditions:</td>"
            "<td>Na=" + QString::number(settings.algoSettings.naConc, 'f') + " M<br>Mg=" + QString::number(settings.algoSettings.mgConc, 'f') + " M</td>"
        "</tr>"
        "<tr>"
            "<td >Window:</td>"
            "<td>" + (settings.algoSettings.window >= 0 ? QString::number(settings.algoSettings.window) : "default") + "</td>"
        "</tr>"
        "<tr>"
            "<td >Maximum distance between paired bases:</td>"
            "<td>" + (settings.algoSettings.maxBp > 0 ? QString::number(settings.algoSettings.maxBp) : "default") + "</td>"
        "</tr>"
        "</table>";
    report += QString("<table><tr><td width=200><b>Parameters</b></td><td width=%1>%2</td></tr>"
        "<tr><td><b>Output HTML report</b></td><td><a target=\"_blank\" href=\"file:///%3\">%3</a></td></tr>"
        "<tr><td><b>Found structures</b></td><td>{{FOUND_HAIRPINS_LIST}}</td></tr>"
        "</table>")
                  .arg(qBound(300, windowWidth - 250, 8192))
                  .arg(info)
                  .arg(QDir(settings.outSettings.outPath).absoluteFilePath(outHtmlBasename));

    report += "<table cellpadding=4>";
    QString fileName = seqInfo.seqPath.baseFileName();
    QString foundStructures;


    for (auto structBeginInd = thermoHtml.indexOf(dgStart); structBeginInd >= 0;) {
        // Row with text "Structure N".
        report += "<tr>";
        report += "<th style=\"border:solid; border-width:1px;\" colspan=\"2\">";
        report += QString("<a name=\"structure%1\"><b>Structure %1").arg(hairpinNum);
        report += "</b></a></th></tr>";

        report += "<tr>";

        // Thermo table cell.
        int structEndInd = thermoHtml.indexOf("\n", structBeginInd);
        auto info = thermoHtml.mid(structBeginInd, structEndInd - structBeginInd).split("&nbsp;");
        structBeginInd = thermoHtml.indexOf(dgStart, structEndInd);
        report += "<td cellpadding=4 style=\"border:solid; border-width:1px;\">";
        report += "<table cellpadding=4 border=1 style=\"max-height: 100%;\">";
        for (auto thParam : info) {
            report += "<tr>";
            for (auto part : thParam.split("=")) {
                report += "<td>" + part + "</td>";
            }
            report += "</tr>";
            static const QRegularExpression dgRe("\\s*dG\\s*=\\s*(-?\\d+\\.\\d+).*");
            QRegularExpressionMatch match = dgRe.match(thParam);
            if (match.hasMatch()) {
                foundStructures += QString("%1. <a href=\"#structure%1\">dG=%2</a><br>").arg(hairpinNum).arg(match.captured(1));
            }
        }
        report += "</table></td>";

        // Img cell.
        // If html is used by UGENE, then $${{IMG_PATH_PREFIX}} corresponds to tmp folder where the images are saved.
        // If html is saved to a file, then out.html and imgs should be together in outDir, and html should not use
        // an absolute path so that HTML report can be easily shared.
        report += "<td rowspan=\"2\" style=\"border:solid; border-width:1px;\">";
        report += QString("<br><img src=\"$${{IMG_PATH_PREFIX}}%1.txt_%2.png\" "
                          "style=\"max-width: 100%; max-height: 100%;\"/>")
                      .arg(fileName)
                      .arg(hairpinNum++);
        report += "</td>";

        report += "</tr>";

        // Hairpin info table cell.
        report += "<tr>";
        report += "<td style=\"border:solid; border-width:1px;\">";
        int hairpinEnd = hairpinHtml.indexOf(" </table>", hairpinStart);
        auto hairpin = hairpinHtml.mid(hairpinStart, hairpinEnd - hairpinStart);
        hairpinStart = hairpinHtml.indexOf(hairpinHeader, hairpinEnd) + hairpinHeader.length();
        report += "<details><table border=1 cellpadding=4>";
        report += hairpin;
        report += "</table></details></td></tr>";
    }
    report += "</table>";

   
    report.replace("{{FOUND_HAIRPINS_LIST}}", foundStructures);

    // Save report to file.
    auto path = QDir(settings.outSettings.outPath).absoluteFilePath(outHtmlBasename);
    QFile file(path);
    CHECK_EXT(file.open(QIODevice::WriteOnly),
              setError(QString(tr("Unable to create output file `%1`")).arg(path)), );
    QString outReport = "<!DOCTYPE html><html><body><h1>Task report [Mfold] (generated by UGENE)</h1>" + report + "</body></html>";
    outReport.replace("$${{IMG_PATH_PREFIX}}", "");
    file.write(outReport.toLocal8Bit());
    file.close();

    // For large report display error.
    if (report.length() > maxHtmlLength) {
        // todo a file or the file
        report = tr("The report is too large to be displayed in UGENE, it is saved to the file "
                    "`<a target=\"_blank\" href=\"file:///%1\">%1</a>`")
                     .arg(QDir(settings.outSettings.outPath).absoluteFilePath(outHtmlBasename));
        return;
    }
    report.replace(QString("$${{IMG_PATH_PREFIX}}"), GUrlUtils::getSlashEndedPath(cwd.getURLString()));
}

QString MfoldTask::generateReport() const {
    return report;
}

MfoldTask* createMfoldTask(U2SequenceObject* seqObj, const MfoldSettings& settings, int windowWidth, U2OpStatus& os) {
    auto seqLen = seqObj->getSequenceLength();
    U2Region region = settings.region->regions[0];
    auto regionHasJunctionPoint = region.length > seqLen - region.startPos;
    QByteArray seq;
    if (regionHasJunctionPoint) {
        auto firstPartLen = seqLen - region.startPos;
        U2Region firstRegionPart = {region.startPos, firstPartLen};
        U2Region secondRegionPart = {0, region.length - firstPartLen};
        seq = seqObj->getSequenceData(firstRegionPart, os);
        CHECK_OP(os, nullptr);
        seq += seqObj->getSequenceData(secondRegionPart, os);
        CHECK_OP(os, nullptr);
    } else {
        seq = seqObj->getSequenceData(region, os);
        CHECK_OP(os, nullptr);
    }

    MfoldSettings correctedSettings = settings;
    Mfold::toGenbankLocation(correctedSettings.region, seqLen);

    MfoldSequenceInfo seqInfo;
    seqInfo.seqName = seqObj->getSequenceName();
    seqInfo.seqPath = seqObj->getDocument()->getURL();
    seqInfo.isCircular = seqObj->isCircular();
    seqInfo.isDna = seqObj->getAlphabet()->isDNA();
    return new MfoldTask(seq, correctedSettings, seqInfo, windowWidth);
}
}  // namespace U2
