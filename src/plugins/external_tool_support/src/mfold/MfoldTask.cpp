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

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>

#include "MfoldSettings.h"
#include "MfoldSupport.h"

namespace U2 {
const QString inSeqBasename = "inp.txt";

static QString constructOutPath(const QString& userPath) {
    CHECK(!userPath.isEmpty(), userPath);
    auto curDt = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm-ss");
    auto suffixedPath = FileAndDirectoryUtils::getAbsolutePath(userPath + "/mfold/" + curDt);
    return GUrlUtils::rollFileName(suffixedPath, "_");
}

MfoldTask::MfoldTask(const QByteArray& seq,
                     const MfoldSettings& settings,
                     bool isCircular,
                     bool isDna,
                     const QSize& imgSize)
    : Task(tr("Predict and visualize hairpins with Mfold"),
           TaskFlags_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      seq(seq), mfoldArgs("RUN_TYPE=html"), outPath(constructOutPath(settings.outPath)),
      cwd(ExternalToolSupportUtils::createTmpDir("mfold", stateInfo)), imgSize(imgSize) {
    GCOUNTER(cvar, "mfold");
    CHECK_OP(stateInfo, );

    // Check tool is ok.
    auto tool = AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID);
    CHECK_EXT(tool->isValid() && !tool->getPath().isEmpty(),
              setError(tr("Mfold tool is invalid, check it in settings")), );

    mfoldArgs.append(QString("SEQ=%1").arg(QDir(cwd).absoluteFilePath(inSeqBasename)));
    mfoldArgs.append(QString("T=%1").arg(settings.temperature));
    mfoldArgs.append(QString("P=%1").arg(settings.percent));
    mfoldArgs.append(QString("NA_CONC=%1").arg(settings.naConc));
    mfoldArgs.append(QString("MG_CONC=%1").arg(settings.mgConc));
    mfoldArgs.append(QString("ROT_ANG=%1").arg(settings.rotAng));
    mfoldArgs.append(QString("MAX=%1").arg(settings.maxFold));
    if (settings.window >= 0) {
        mfoldArgs.append(QString("W=%1").arg(settings.window));
    }
    if (settings.maxBp > 0) {
        mfoldArgs.append(QString("MAXBP=%1").arg(settings.maxBp));
    }
    if (settings.labFr >= 0) {
        mfoldArgs.append(QString("LAB_FR=%1").arg(settings.labFr));
    }
    if (isDna) {
        mfoldArgs.append(QString("NA=DNA"));
    }
    if (isCircular) {
        mfoldArgs.append(QString("LC=circular"));
    }
}

void MfoldTask::prepare() {
    CHECK(!hasError(), );

    // todo split to separate task?
    auto path = QDir(cwd).absoluteFilePath(inSeqBasename);
    QFile file(path);
    CHECK_EXT(file.open(QIODevice::ReadWrite), setError(QString(tr("Unable to create input file `%1`")).arg(path)), );
    file.write(seq);
    file.close();

    // todo use externalToolOutputFile?
    auto etTask = new ExternalToolRunTask(MfoldSupport::ET_MFOLD_ID, mfoldArgs, new ExternalToolLogParser(), cwd);
    etTask->setAdditionalEnvVariables({{"U2_GS_IMG_OUT_SIZE",
                                        QString("%1x%2").arg(imgSize.width()).arg(imgSize.height())}});
    addSubTask(etTask);
}

void MfoldTask::run() {
    CHECK(!hasError(), );

    // todo how to read them?
    // todo add more check for function return codes
    auto readFileInput = [this](const QString& fileName) -> QString {
        auto filePath = QDir(cwd).absoluteFilePath(fileName);
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
    QString hairpinHtml = readFileInput("inp.txt.det.html");  // html with hairpin info tables
    QString thermoHtml = readFileInput("inp.txt.out.html");  // html with dG and dH info
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

    report += "<!DOCTYPE html><html><body>";

    report += "<table cellpadding=4>";
    for (auto structBeginInd = thermoHtml.indexOf(dgStart); structBeginInd >= 0;) {
        // Row with text "Structure N".
        report += "<tr>";
        report += "<th style=\"border:solid; border-width:1px;\" colspan=\"2\">";
        report += "<b>Structure ";
        report += QString("%1").arg(hairpinNum);
        report += "</b></th></tr>";

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
        }
        report += "</table></td>";

        // Img cell.
        // If html is used by UGENE, then $${{IMG_PATH_PREFIX}} corresponds to tmp folder where the images are saved.
        // If html is saved to a file, then out.html and imgs should be together in outDir, and html should not use
        // an absolute path so that HTML report can be easily shared.
        report += "<td rowspan=\"2\" style=\"border:solid; border-width:1px;\">";
        report += QString("<br><img src=\"$${{IMG_PATH_PREFIX}}inp.txt_%1.png\" "
                          "style=\"max-width: 100%; max-height: 100%;\"/>")
                      .arg(hairpinNum++);
        report += "</td>";

        report += "</tr>";

        // Hairpin info table cell.
        report += "<tr>";
        report += "<td style=\"border:solid; border-width:1px;\">";
        int hairpinEnd = hairpinHtml.indexOf(" </table>", hairpinStart);
        auto hairpin = hairpinHtml.mid(hairpinStart, hairpinEnd - hairpinStart);
        hairpinStart = hairpinHtml.indexOf(hairpinHeader, hairpinEnd) + hairpinHeader.length();
        report += "<table border=1 cellpadding=4>";
        report += hairpin;
        report += "</table></td></tr>";
    }
    report += "</table>";
    report += "</body></html>";

    // Prepare output folder.
    if (!outPath.isEmpty()) {
        outPath = GUrlUtils::prepareDirLocation(outPath, stateInfo);
        CHECK_OP(stateInfo, );

        auto inDir = QDir(cwd), outDir = QDir(outPath);
        for (auto basename : inDir.entryList({"inp.txt_*.png"}, QDir::Files)) {
            QFile::copy(inDir.absoluteFilePath(basename), outDir.absoluteFilePath(basename));
        }

        auto path = outDir.absoluteFilePath("out.html");
        QFile file(path);
        CHECK_EXT(file.open(QIODevice::WriteOnly),
                  setError(QString(tr("Unable to create output file `%1`")).arg(path)), );
        file.write(QString(report).replace("$${{IMG_PATH_PREFIX}}", "").toLocal8Bit());
        file.close();
    }
    report.replace(QString("$${{IMG_PATH_PREFIX}}"), GUrlUtils::getSlashEndedPath(cwd));
}

QString MfoldTask::generateReport() const {
    return report;
}

MfoldTask* createMfoldTask(U2SequenceObject* seqObj, const MfoldSettings& settings, int windowWidth, U2OpStatus& os) {
    auto seqLen = seqObj->getSequenceLength();
    auto regionHasJunctionPoint = settings.region.length > seqLen - settings.region.startPos;
    QByteArray seq;
    if (regionHasJunctionPoint) {
        auto firstPartLen = seqLen - settings.region.startPos;
        seq = seqObj->getSequenceData({settings.region.startPos, firstPartLen}, os);
        CHECK_OP(os, nullptr);
        seq += seqObj->getSequenceData({0, settings.region.length - firstPartLen}, os);
        CHECK_OP(os, nullptr);
    } else {
        seq = seqObj->getSequenceData(settings.region, os);
        CHECK_OP(os, nullptr);
    }

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
    return new MfoldTask(seq, settings, seqObj->isCircular(), seqObj->getAlphabet()->isDNA(), imgSize);
}
}  // namespace U2
