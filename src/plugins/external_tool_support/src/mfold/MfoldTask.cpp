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
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "MfoldSettings.h"
#include "MfoldSupport.h"

// https://stackoverflow.com/a/13993058
static QString toAmpersandEncode(const QString& string) {
    QString encoded;
    for (int i = 0; i < string.size(); ++i) {
        QChar ch = string.at(i);
        ushort unicode = ch.unicode();
        encoded += unicode > 255 ? QString("&#%1;").arg((int)unicode) : ch;
    }
    return encoded;
}
static QString toAmpersandWithoutSpaces(const QString& string) {
    return toAmpersandEncode(string).replace(' ', "%20");
}

namespace U2 {
const QString DEG_SYM = QChar(0xB0);
const QString DELTA_SYM = QChar(0x3B4);
const QString MID_DOT_SYM = QChar(0xB7);

static int getStructuresNum(const GUrl& cwd, U2OpStatus& os) {
    QString path = cwd.getURLString();
    QDir dir(path);
    dir.setNameFilters({"*.ps"});
    int ans = dir.count();
    SAFE_POINT_EXT(ans > 0, os.setError(QString(QT_TR_NOOP("No output files expected in `%1`")).arg(path)), {});
    return ans;
}

namespace Th {
using ThermodynInfo = QHash<QString, double>;
const QString DELTA_G_KEY = "dG";
const QString DELTA_H_KEY = "dH";
const QString DELTA_S_KEY = "dS";
const QString TM_KEY = "T<sub>m<\\/sub>";

QVector<ThermodynInfo> parseThermodynamicInfo(const QString& filePath, U2OpStatus& os) {
    //  dG =      3.70 &nbsp; dH =    -31.40 &nbsp; dS =   -113.17 &nbsp; T<sub>m</sub> =    4.3
    // Reads all such entries from /cwd/SEQ_NAME.out.html
    // and converts them into an array of associative arrays "parameter name->value".
    QScopedPointer<IOAdapter> io(IOAdapterUtils::open(filePath, os, IOAdapterMode_Read));
    SAFE_POINT_OP(os, {});
    SAFE_POINT_NN(io, {});
    IOAdapterReader reader(io.data());

    QVector<ThermodynInfo> ans;
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, -1);
        line = line.simplified();
        if (!line.startsWith(DELTA_G_KEY)) {
            continue;
        }

        ThermodynInfo info;
        for (const QString& paramName : {DELTA_G_KEY, DELTA_H_KEY, DELTA_S_KEY, TM_KEY}) {
            // All parameters have form "dH = -31.40",
            // i.e. parameter_name + space + equals sign + space + double value.
            static const QString patternWithoutName = " =\\s?(-?\\d+\\.?\\d+)";
            QRegularExpression re(paramName + patternWithoutName);
            QRegularExpressionMatch result = re.match(line);
            bool ok;
            double val = result.captured(1).toDouble(&ok);
            SAFE_POINT_EXT(ok, os.setError(QString(QT_TR_NOOP("Error parsing `%1` param")).arg(paramName)), {});
            info[paramName] = val;
        };
        ans += info;
    }
    return ans;
}
QString constructTocList(const QVector<Th::ThermodynInfo>& thInfo) {
    QString ans;
    for (int i = 0; i < thInfo.size(); ++i) {
        QString val = QString::number(qAsConst(thInfo)[i][Th::DELTA_G_KEY]);
        ans += "<li><a href=\"#structure" + QString::number(i + 1) + "\">";
        ans += DELTA_SYM + "G=" + val;
        ans += "</a></li>";
    }
    return ans;
}
}  // namespace Th

namespace Det {
QString parseNextTblContent(IOAdapterReader& reader, U2OpStatus& os) {
    QString ans;
    // Find the beginning of the table.
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, -1).simplified();
        CHECK_OP(os, {});
        if (line.startsWith("<p><table ")) {
            ans += line.replace("<p><table border=3 cellpadding=4>", "");
            break;
        }
    }
    // Parse table until end.
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, -1).simplified();
        CHECK_OP(os, {});
        if (line.startsWith("</table><p>")) {
            break;
        }
        ans += line;
    }
    SAFE_POINT_EXT(!reader.atEnd(), os.setError("Unexpected EOF"), {});
    return ans;
}
};  // namespace Det

class MfoldTask::ReportHelper final {
    MfoldTask& t; // replace with const
    IOAdapterReader& detReader; //todo replace with pointer?
    int structuresNum = 0;
    QVector<Th::ThermodynInfo> thInfo;
    QVector<QString> detailTables;  // empty if report is too large
    bool largeReport = false;

public:
    ReportHelper(MfoldTask& t, IOAdapterReader& detReader)
        : t(t), detReader(detReader) {
        structuresNum = getStructuresNum(t.cwd, t.stateInfo);
        CHECK_OP(t.stateInfo, );
        auto thHtmlPath = t.inpSeqPath + ".out.html";
        thInfo = Th::parseThermodynamicInfo(thHtmlPath, t.stateInfo);
        CHECK_OP(t.stateInfo, );
        SAFE_POINT_EXT(structuresNum == thInfo.size(),
                       t.stateInfo.setError(tr("Found %1 images in `%2` and %3 thermodynamic tables in `%4`")
                                                .arg(structuresNum)
                                                .arg(t.cwd.getURLString())
                                                .arg(thInfo.size())
                                                .arg(thHtmlPath)), );
    }

    QString constructFileReport() {
        QString seqName = toAmpersandEncode(t.seqInfo.seqName);
        QString inpSeqPath = toAmpersandEncode(t.seqInfo.seqPath.getURLString());
        QString report = "<!DOCTYPE html>"
                         "<html lang=\"en\">"
                         "<head>"
                         "<style>"
                         "  ul { font-size: large; }"
                         "  .imgcell { vertical-align: top; text-align: left; max-height: 85vh; }"
                         "  .image { border: 1px solid; height: 50em; }"
                         "  .showinfo { vertical-align: top; text-align: left; }"
                         "  .dettbl { border-collapse: collapse; }"
                         "  .dettbl td, th { padding: 4px; border: 1px solid; }"
                         "</style>"
                         "<script src=\"https://unpkg.com/iv-viewer/dist/iv-viewer.js\"></script>"
                         "<link rel=\"stylesheet\" href=\"https://unpkg.com/iv-viewer/dist/iv-viewer.css\" />"
                         "<title>"
                         "Hairpins for " +
                         seqName + "</title>";
        report += "</head>"
                  "<body>"
                  "<h1>Task report [Mfold] (generated by UGENE)</h1>"
                  "<h2>Parameters</h2>";
        // Dump run info.
        /*
        | Sequence name:                 | CVU55762                  |
        | File:                          | C:/Users/user/CVU55762.gb |
        | Region:                        | 1..100                    |
        | Sequence type:                 | Circular DNA              |
        | Temperature:                   | 42                        |
        | Percent suboptimality:         | 10%                       |
        | Ionic conditions:              | Na=0.60 M                 |
        |                                | Mg=0.90 M                 |
        | Window:                        | 15                        |
        | Maximum distance paired bases: | 13                        |
        */
        report += "<table>"
                  "<tr>"
                  "<td>Sequence name:</td>"
                  "<td>" +
                  seqName + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Sequence path:</td>"
                  "<td>" +
                  inpSeqPath + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Region:</td>"
                  "<td>" +
                  U1AnnotationUtils::buildLocationString(t.settings.region->regions) + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Sequence type:</td>"
                  "<td>";
        report += t.seqInfo.isCircular ? "Circular " : "Linear ";
        report += t.seqInfo.isDna ? "DNA" : "RNA";
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td>Temperature:</td>"
                  "<td>" +
                  QString::number(t.settings.algoSettings.temperature) + "&deg;C</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Percent suboptimality:</td>"
                  "<td>" +
                  QString::number(t.settings.algoSettings.percent) + "%</td>";
        report += "</tr>"
                  "<tr>"
                  "<td rowspan=\"2\">Ionic conditions:</td>"
                  "<td>"
                  "Na=" +
                  QString::number(t.settings.algoSettings.naConc, 'f', 2) + " M";
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td>"
                  "Mg=" +
                  QString::number(t.settings.algoSettings.mgConc, 'f', 2) + " M";
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td>Window:</td>"
                  "<td>" +
                  (t.settings.algoSettings.window >= 0 ? QString::number(t.settings.algoSettings.window) : "default");
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td>Maximum distance between paired bases:</td>"
                  "<td>" +
                  (t.settings.algoSettings.maxBp > 0 ? QString::number(t.settings.algoSettings.maxBp) : "default");
        // Dump structures summary: links to fast jump.
        /*
        1. dG=-3.84
        2. dG=-3.58
        */
        report += "</td>"
                  "</tr>"
                  "</table>"
                  "<h2>Found structures</h2>"
                  "<ol>" +
                  Th::constructTocList(thInfo) + "</ol>";

        // Dump all hairpin stat and imgs.
        for (int i = 0; i < structuresNum; ++i) {
            QString iStr = QString::number(i + 1);
            const Th::ThermodynInfo& th = qAsConst(thInfo)[i];
            report += "<h2><a id=\"structure" + iStr + "\">Structure " + iStr + "</a></h2>";
            report += "<ul>"
                      "<li>&delta;G = " +
                      QString::number(th[Th::DELTA_G_KEY], 'f', 2) + " kcal/mol</li>";
            report += "<li>&delta;H = " +
                      QString::number(th[Th::DELTA_H_KEY], 'f', 2) + " kcal/mol</li>";
            report += "<li>&delta;S = " +
                      QString::number(th[Th::DELTA_S_KEY], 'f', 2) + " cal/(K&middot;mol)</li>";
            report += "<li>T<sub>m</sub> = " +
                      QString::number(th[Th::TM_KEY], 'f', 2) + "&deg;C</li>";
            QString imgPath = toAmpersandWithoutSpaces(GUrlUtils::fixFileName(t.seqInfo.seqPath.baseFileName())) +
                              ".txt_" + iStr + ".png";
            report += "</ul>"
                      "<table>"
                      "<tr>"
                      "<td class=\"imgcell\">"
                      "<img class=\"image\" src=\"" +
                      imgPath + "\" data-high-res-src=\"" + imgPath + "\" alt=\"\" />";
            report += "</td>"
                      "<td class=\"showinfo\">"
                      "<details>"
                      "<summary>"
                      "Show structure detailed info"
                      "</summary>"
                      "<table class=\"dettbl\">";
            QString detTable = Det::parseNextTblContent(detReader, t.stateInfo);
            SAFE_POINT_EXT(!detTable.isEmpty(), t.stateInfo.setError("Unexpected EOF"), {});
            if (!largeReport && detTable.count("</") < 300) {  // 300 closing tags ~ 50 rows
                detailTables += detTable;
            } else {
                largeReport = true;
                detailTables.clear();
            }
            report += detTable;
            report += "</table>"
                      "</details>"
                      "</td>"
                      "</tr>"
                      "</table>";
        }
        report += "<script>"
                  "Array.from(document.querySelectorAll(\".image\")).forEach((elem) => {"
                  "new ImageViewer(elem);"
                  "});"
                  "</script>"
                  "</body>"
                  "</html>";
        if (report.size() > 100'000) {
            largeReport = true;
        }
        return report;
    }

    QString constructUgeneReport() {
        if (largeReport) {
            return "The report is too large to be displayed in UGENE, it is saved to the file "
                   "`<a target=\"_blank\" href=\"" + t.outHtmlPath + "\">" + t.outHtmlPath + "</a>`";
        }
        SAFE_POINT_EXT(structuresNum == thInfo.size() && structuresNum == detailTables.size(),
                       t.stateInfo.setError(tr("%1 != %2 != %3")
                                                .arg(structuresNum)
                                                .arg(thInfo.size())
                                                .arg(detailTables.size())),
                       {});
        QString inpSeqPath = t.seqInfo.seqPath.getURLString();
        // Dump run info.
        /*
        | Parameters         | Sequence name:                         | CVU55762                  |
        |                    | Sequence path:                         | C:/Users/user/CVU55762.gb |
        |                    | Region:                                | 1..100                    |
        |                    | Sequence type:                         | Circular DNA              |
        |                    | Temperature:                           | 37°C                      |
        |                    | Percent suboptimality:                 | 5%                        |
        |                    | Ionic conditions:                      | Na=1.00 M                 |
        |                    |                                        | Mg=0.00 M                 |
        |                    | Window:                                | default                   |
        |                    | Maximum distance between paired bases: | default                   |
        | Output HTML report | C:/Users/user/out.html                                             |
*/
        QString report = "<table>"
                         "<tr>"
                         "<th width=\"200\" align=\"left\">Parameters</th>"
                         "<td width=\"" +
                         QString::number(qBound(300, t.windowWidth - 250, 8192));
        report += "\">"
                  "<table style=\"border-collapse: collapse; border: 0px;\">"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Sequence name:</td>"
                  "<td>" +
                  t.seqInfo.seqName + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Sequence path:</td>"
                  "<td>" +
                  inpSeqPath + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Region:</td>"
                  "<td>" +
                  U1AnnotationUtils::buildLocationString(t.settings.region->regions) + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Sequence type:</td>"
                  "<td>";
        report += t.seqInfo.isCircular ? "Circular " : "Linear ";
        report += t.seqInfo.isDna ? "DNA" : "RNA";
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Temperature:</td>"
                  "<td>" +
                  QString::number(t.settings.algoSettings.temperature) + DEG_SYM + "C</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Percent suboptimality:</td>"
                  "<td>" +
                  QString::number(t.settings.algoSettings.percent) + "%</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\" rowspan=\"2\">Ionic conditions:</td>"
                  "<td>Na=" +
                  QString::number(t.settings.algoSettings.naConc, 'f', 2) + " M</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Mg=" +
                  QString::number(t.settings.algoSettings.mgConc, 'f', 2) + " M</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Window:</td>"
                  "<td>";
        report += t.settings.algoSettings.window >= 0 ? QString::number(t.settings.algoSettings.window) : "default";
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">Maximum distance between paired bases:</td>"
                  "<td>";
        report += t.settings.algoSettings.maxBp > 0 ? QString::number(t.settings.algoSettings.maxBp) : "default";
        report += "</td>"
                  "</tr>"
                  "</table>"
                  "</td>"
                  "</tr>"
                  "<tr>"
                  "<th align=\"left\">Output HTML report</th>"
                  "<td>"
                  "<a href=\"" + t.outHtmlPath + "\">" + t.outHtmlPath + "</a>";
        // Dump structures summary: links to fast jump.
        /*
        1. dG=-3.84
        2. dG=-3.58
        */
        report += "</td>"
                  "</tr>"
                  "<tr>"
                  "<th align=\"left\">Found structures</th>"
                  "<td>"
                  "<ol style=\"margin-left: 12px; -qt-list-indent: 0;\">" +
                  Th::constructTocList(thInfo);
        // Dump all hairpin stat and imgs.
        report += "</ol>"
                  "</td>"
                  "</tr>"
                  "</table>"
                  "<table cellpadding=\"4\" style=\"border-collapse: collapse;\">";
        for (int i = 0; i < structuresNum; ++i) {
            QString iStr = QString::number(i + 1);
            const Th::ThermodynInfo& th = qAsConst(thInfo)[i];
            report += "<tr>"
                      "<th colspan=\"2\" style=\"border: 1px solid;\"><a name=\"structure" +
                      iStr + "\">Structure " + iStr + "</a></th>";
            report += "</tr>"
                      "<tr>"
                      "<td style=\"border: 1px solid;\">"
                      "<ul style=\"margin-left: 9px; -qt-list-indent: 0; list-style: none; font-size: large;\">"
                      "<li>" +
                      DELTA_SYM + "G = " + QString::number(th[Th::DELTA_G_KEY], 'f', 2) + " kcal/mol</li>";
            report += "<li>" +
                      DELTA_SYM + "H = " +
                      QString::number(th[Th::DELTA_H_KEY], 'f', 2) + " kcal/mol</li>";
            report += "<li>" +
                      DELTA_SYM + "S = " +
                      QString::number(th[Th::DELTA_S_KEY], 'f', 2) + " cal/(K" + MID_DOT_SYM + "mol)</li>";
            report += "<li>T<sub>m</sub> = " +
                      QString::number(th[Th::TM_KEY], 'f', 2) + DEG_SYM + "C</li>";
            report += "</ul>"
                      "</td>"
                      "<td rowspan=\"2\" style=\"border: 1px solid;\"><img src=\"" +
                      t.inpSeqPath + "_" + iStr + ".png\" /></td>";
            report += "</tr>"
                      "<tr>"
                      "<td style=\"border: 1px solid;\">"
                      "<table border=\"1\" cellpadding=\"4\" style=\"border-collapse: collapse;\">" +
                      detailTables[i];
            report += "</table>"
                      "</td>"
                      "</tr>";
        }
        report += "</table>";
        return report;
    }
};

MfoldTask::MfoldTask(const QByteArray& seq,
                     const MfoldSettings& settings,
                     const MfoldSequenceInfo& seqInfo,
                     int windowWidth)
    : Task(tr("Predict and visualize hairpins with Mfold"),
           TaskFlags_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      seq(seq), settings(settings), seqInfo(seqInfo), windowWidth(windowWidth) {
    GCOUNTER(cvar, "mfold");
}

void MfoldTask::prepare() {
    // Check tool is ok.
    auto tool = AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID);
    CHECK_EXT(tool->isValid() && !tool->getPath().isEmpty(),
              setError(tr("Mfold tool is invalid, check it in settings")), );

    // Prepare cwd.
    cwd = ExternalToolSupportUtils::createTmpDir("mfold", stateInfo);
    CHECK_OP(stateInfo, );

    // Save sequence in cwd.
    inpSeqPath = constructSeqFilePath();
    CHECK_EXT(FileAndDirectoryUtils::storeTextToFile(inpSeqPath, seq),
              setError(QString(tr("Unable to store input sequence to file `%1`")).arg(inpSeqPath)), );

    // Prepare out dir.
    settings.outSettings.outPath = GUrlUtils::prepareDirLocation(constructOutPath(), stateInfo);
    CHECK_OP(stateInfo, );
    settings.outSettings.outPath = GUrlUtils::getSlashEndedPath(settings.outSettings.outPath);
    outHtmlPath = settings.outSettings.outPath + "out.html";

    auto etTask = new ExternalToolRunTask(MfoldSupport::ET_MFOLD_ID,
                                          constructEtArgs(),
                                          new ExternalToolLogParser(),
                                          cwd.getURLString());
    etTask->setAdditionalEnvVariables(constructEtEnv());
    addSubTask(etTask);
}

void MfoldTask::run() {
    CHECK_OP(stateInfo, );

    QScopedPointer<IOAdapter> detIo(IOAdapterUtils::open(inpSeqPath + ".det.html", stateInfo, IOAdapterMode_Read));
    CHECK_OP(stateInfo, );
    SAFE_POINT_NN(detIo, );
    IOAdapterReader detReader(detIo.data());

    ReportHelper helper(*this, detReader);
    CHECK_OP(stateInfo, );

    QString fileReport = helper.constructFileReport();
    CHECK_OP(stateInfo, );
    QFile file(outHtmlPath);
    CHECK_EXT(file.open(QIODevice::WriteOnly),
              setError(QString(tr("Unable to create output file `%1`")).arg(outHtmlPath)), );
    file.write(fileReport.toLocal8Bit());
    file.close();

    report = helper.constructUgeneReport();
    CHECK_OP(stateInfo, );
}

QString MfoldTask::generateReport() const {
    return report;
}

// todo what does this function return if tmp dir contains spaces or non-ascii
QString MfoldTask::constructSeqFilePath() const {
    QString fileName = GUrlUtils::fixFileName(seqInfo.seqPath.baseFileName());
    QString inFilePath = GUrlUtils::getSlashEndedPath(cwd.getURLString()) + fileName;
    return GUrlUtils::getLocalUrlFromUrl(inFilePath, fileName, ".txt", "");
}

QString MfoldTask::constructOutPath() const {
    auto curDt = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm-ss");
    auto suffixedPath = FileAndDirectoryUtils::getAbsolutePath(settings.outSettings.outPath + "/mfold/" + curDt);
    return GUrlUtils::rollFileName(suffixedPath, "_");
}

QStringList MfoldTask::constructEtArgs() const {
    QStringList etArgs = {
        "SEQ=" + inpSeqPath,
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
        etArgs += "W=" + QString::number(settings.algoSettings.window);
    }
    if (settings.algoSettings.maxBp > 0) {
        etArgs += "MAXBP=" + QString::number(settings.algoSettings.maxBp);
    }
    if (settings.algoSettings.labFr >= 0) {
        etArgs += "LAB_FR=" + QString::number(settings.algoSettings.labFr);
    }
    return etArgs;
}

StrStrMap MfoldTask::constructEtEnv() const {
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
    return envVars;
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
