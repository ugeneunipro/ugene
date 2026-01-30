/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "MfoldSettings.h"
#include "MfoldSupport.h"
#include "utils/OutputCollector.h"

// https://stackoverflow.com/a/13993058
static QString encodeToHtmlEntities(const QString& input) {
    QString output;
    for (const QChar& ch : qAsConst(input)) {
        ushort uni = ch.unicode();
        if (ch == '"') {
            output.append("&quot;");
        } else if (ch == '&') {
            output.append("&amp;");
        } else if (ch == '<') {
            output.append("&lt;");
        } else if (ch == '>') {
            output.append("&gt;");
        } else if (uni > 127) {  // For non-ASCII characters.
            output.append("&#" + QString::number(uni) + ";");
        } else {
            output.append(ch);
        }
    }
    return output;
}

namespace U2 {
namespace {
// Detailed table (detTable) of breakdown of each folding into loops can be very large
// (see CVU55762 results, any region length 500+). When displaying such table, UGENE begins to freeze.
// We need heuristic to determine if the table is large.

// Each table row contains from 4 to 7 tags. Let's round up tag expected number for one row to 5.
// In total, we will consider table large if it has more than 50 rows, i.e. 5*50=250 (closing) tags.
constexpr int maxDetTableTagNum = 250;
// Approximate experimentally determined constant at which UGENE freezes significantly when opening report.
constexpr int maxHtmlReportLen = 100000;
// At the bottom of each img there is seq name. Only first 50 symbols of seq name are shown.
constexpr int maxSeqNameLen = 50;
// By default, tool writes this size in ps files.
constexpr int defaultImgWidth = 792;
constexpr int defaultImgHeight = 1072;

const QString DEG_SYM = QChar(0xB0);
const QString DELTA_SYM = QChar(0x3B4);
const QString MID_DOT_SYM = QChar(0xB7);

static int getStructuresNum(const GUrl& cwd) {
    QString path = cwd.getURLString();
    QDir dir(path);
    dir.setNameFilters({"*.ps"});
    int ans = dir.count();
    return ans;
}

// Uses dash style.
static QString getLocationStr(const U2Location& l) {
    auto&& regions = l->regions;
    auto regionToStr = [](const U2Region& region) { return QString::number(region.startPos + 1) + '-' +
                                                           QString::number(region.endPos()); };
    QString regionStr = ':' + regionToStr(regions[0]);
    if (regions.size() > 1) {
        regionStr += ',' + regionToStr(regions[1]);
    }
    return regionStr;
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
            SAFE_POINT_EXT(ok, os.setError(MfoldTask::tr("Error parsing `%1` param").arg(paramName)), {});
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
        ans += "&delta;G=" + val;
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
    SAFE_POINT_EXT(!reader.atEnd(), os.setError(MfoldTask::tr("Unexpected EOF")), {});
    return ans;
}
};  // namespace Det
}  // namespace

class MfoldTask::ReportHelper final {
    const MfoldTask& t;
    int structuresNum = 0;
    QVector<Th::ThermodynInfo> thInfo;
    QVector<QString> detailTables;  // empty if report is too large
    bool largeReport = false;

public:
    ReportHelper(const MfoldTask& t, U2OpStatus& os)
        : t(t) {
        structuresNum = getStructuresNum(t.cwd);
        if (structuresNum <= 0) {
            return;
        }

        auto thHtmlPath = t.tmpSeqPath + ".out.html";
        thInfo = Th::parseThermodynamicInfo(thHtmlPath, os);
        SAFE_POINT_OP(os, );
        SAFE_POINT_EXT(structuresNum == thInfo.size(),
                       os.setError(tr("Found %1 images in `%2` and %3 thermodynamic tables in `%4`")
                                       .arg(structuresNum)
                                       .arg(t.cwd.getURLString())
                                       .arg(thInfo.size())
                                       .arg(thHtmlPath)), );
    }

    QString constructFileReport(U2OpStatus& os) {
        QString seqName = encodeToHtmlEntities(t.seqInfo.seqName);
        QString inpSeqPath = encodeToHtmlEntities(t.seqInfo.seqPath.getURLString());
        QString report = "<!DOCTYPE html>"
                         "<html lang=\"en\">"
                         "<head>"
                         "<style>"
                         "  ul { font-size: large; }"
                         "  .imgcell { vertical-align: top; text-align: left; }"
                         "  .image { max-height: 85vh; }"
                         "  .showinfo { vertical-align: top; text-align: left; }"
                         "  .dettbl { border-collapse: collapse; }"
                         "  .dettbl td, th { padding: 4px; border: 1px solid; }"
                         "</style>"
                         "<title>"
                         "Hairpins for " +
                         seqName + "</title>";
        report += "</head>"
                  "<body>"
                  "<h1>Task report [mfold] (generated by UGENE)</h1>"
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
                  QString::number(t.settings.algoSettings.temperature) + " &deg;C</td>";
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
                  "</table>";

        if (structuresNum > 0) {
            QScopedPointer<IOAdapter> detIo(IOAdapterUtils::open(t.tmpSeqPath + ".det.html", os, IOAdapterMode_Read));
            SAFE_POINT_OP(os, {});
            SAFE_POINT_NN(detIo, {});
            IOAdapterReader detReader(detIo.data());
            report += "<h2>Found structures</h2>"
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
                QString fileNameWithoutExt = QUrl(GUrl(t.tmpSeqPath).fileName()).toEncoded() + '_' + iStr + '.';
                report += "</ul>"
                          "<table>"
                          "<tr>"
                          "<td class=\"imgcell\">"
                          "<a href=\"" +
                          fileNameWithoutExt + "pdf\" target=\"_blank\">";
                report += "<img class=\"image\" src=\"" +
                          fileNameWithoutExt + "png\" alt=\"\" />";
                report += "</a>"
                          "</td>"
                          "<td class=\"showinfo\">"
                          "<details>"
                          "<summary>Show structure detailed info</summary>"
                          "<table class=\"dettbl\">";
                QString detTable = Det::parseNextTblContent(detReader, os);
                SAFE_POINT_OP(os, {});
                SAFE_POINT_EXT(!detTable.isEmpty(), os.setError("Unexpected EOF"), {});
                if (!largeReport && detTable.count("</") < maxDetTableTagNum) {
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
        }
        report += "<details>"
                  "<summary>mfold log</summary>"
                  "<pre>" +
                  t.log + "</pre>";
        report += "</details>"
                  "</body>"
                  "</html>";
        if (report.size() > maxHtmlReportLen) {
            largeReport = true;
        }
        return report;
    }

    QString constructUgeneReport(U2OpStatus& os) {
        CHECK_EXT(largeReport || structuresNum == detailTables.size(),
                  os.setError(QString("%1 != %2").arg(structuresNum).arg(detailTables.size())),
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

        // UGENE uses width=200 to print task info table (status, time). We mimic single style.
        // Second column will take up the entire remaining window width plus subtract 50 pixels for padding, margins,
        // and borders.
        // Min value is set to 300 for no reason, max is 8K UHD width.
        QString report = "<table>"
                         "<tr>"
                         "<th width=\"200\" align=\"left\">" +
                         tr("Parameters") + "</th>";
        report += "<td width=\"" +
                  QString::number(qBound(300, t.windowWidth - 250, 7680));
        report += "\">"
                  "<table style=\"border-collapse: collapse; border: 0px;\">"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Sequence name") + ":</td>";
        report += "<td>" + t.seqInfo.seqName.toHtmlEscaped() + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Sequence path") + ":</td>";
        report += "<td>" + inpSeqPath.toHtmlEscaped() + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Region") + ":</td>";
        report += "<td>" + U1AnnotationUtils::buildLocationString(t.settings.region->regions) + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Sequence type") + ":</td>";
        report += "<td>" + (t.seqInfo.isCircular ? tr("Circular") : tr("Linear")) + " " +
                  (t.seqInfo.isDna ? tr("DNA") : tr("RNA")) + "</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Temperature") + ":</td>";
        report += "<td>" + QString::number(t.settings.algoSettings.temperature) + " " + DEG_SYM + "C</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Percent suboptimality") + ":</td>";
        report += "<td>" + QString::number(t.settings.algoSettings.percent) + "%</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\" rowspan=\"2\">" +
                  tr("Ionic conditions") + ":</td>";
        report += "<td>Na=" + QString::number(t.settings.algoSettings.naConc, 'f', 2) + " M</td>";
        report += "</tr>"
                  "<tr>"
                  "<td>Mg=" +
                  QString::number(t.settings.algoSettings.mgConc, 'f', 2) + " M</td>";
        report += "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Window") + ":</td>";
        report += "<td>" +
                  (t.settings.algoSettings.window >= 0 ? QString::number(t.settings.algoSettings.window)
                                                       : tr("default")) +
                  "</td>"
                  "</tr>"
                  "<tr>"
                  "<td style=\"padding-right: 10px;\">" +
                  tr("Maximum distance between paired bases") + ":</td>";
        report += "<td>" +
                  (t.settings.algoSettings.maxBp > 0 ? QString::number(t.settings.algoSettings.maxBp) : tr("default")) +
                  "</td>"
                  "</tr>"
                  "</table>"
                  "</td>"
                  "</tr>";

        if (structuresNum > 0) {
            QString outPath = t.outHtmlPath.toHtmlEscaped();
            report += "<tr>"
                      "<th align=\"left\">" +
                      tr("Output HTML report") + "</th>";
            report += "<td>"
                      "<a href=\"" +
                      outPath + "\">" + outPath + "</a>";
            report += "</td>"
                      "</tr>";
        }

        if (!largeReport && structuresNum > 0) {
            // Dump structures summary: links to fast jump.
            /* 1. dG=-3.84
               2. dG=-3.58
            */
            report += "<tr>"
                      "<th align=\"left\">" +
                      tr("Found structures") + "</th>";
            report += "<td>"
                      "<ol style=\"margin-left: 12px; -qt-list-indent: 0;\">" +
                      Th::constructTocList(thInfo) + "</ol>";
            report += "</td>"
                      "</tr>";
        }
        report += "</table>";

        if (!largeReport && structuresNum > 0) {
            report += "<table cellpadding=\"4\" style=\"border-collapse: collapse;\">";
            for (int i = 0; i < structuresNum; ++i) {
                QString iStr = QString::number(i + 1);
                const Th::ThermodynInfo& th = qAsConst(thInfo)[i];
                report += "<tr>"
                          "<th colspan=\"2\" style=\"border: 1px solid;\"><a name=\"structure" +
                          iStr + "\">" + tr("Structure") + " " + iStr + "</a></th>";
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
                          t.tmpSeqPath + "_" + iStr + ".png\" /></td>";
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
        }
        report += "<p>" + tr("mfold log") + ":</p>";
        report += "<pre>" + t.log + "</pre>";
        return report;
    }
};

MfoldTask::MfoldTask(const DNASequence& seq,
                     const MfoldSettings& settings,
                     const MfoldSequenceInfo& seqInfo,
                     int windowWidth)
    : Task(tr("Predict and visualize hairpins with mfold"),
           TaskFlag_PropagateSubtaskDesc | TaskFlag_CancelOnSubtaskCancel | TaskFlag_ReportingIsSupported |
               TaskFlag_ReportingIsEnabled),
      tmpSeq(seq), settings(settings), seqInfo(seqInfo), windowWidth(windowWidth) {
    SAFE_POINT_EXT(!tmpSeq.isNull(), setError(L10N::badArgument("tmpSeq")), );
    SAFE_POINT_EXT(!settings.region->regions.empty(), setError(L10N::badArgument("region")), );
    SAFE_POINT_EXT(settings.outSettings.dpi > 0, setError(L10N::badArgument("dpi")), );
    SAFE_POINT_EXT(windowWidth > 0, setError(L10N::badArgument("windowWidth")), );
    GCOUNTER(cvar, "mfold");
}

void MfoldTask::prepare() {
    SAFE_POINT_OP(stateInfo, );

    // Check tool is ok.
    auto tool = AppContext::getExternalToolRegistry()->getById(MfoldSupport::ET_MFOLD_ID);
    CHECK_EXT(tool->isValid() && !tool->getPath().isEmpty(),
              setError(tr("mfold tool is invalid, check it in settings")), );

    // Prepare cwd.
    cwd = ExternalToolSupportUtils::createTmpDir("mfold", stateInfo);
    SAFE_POINT_OP(stateInfo, );

    // Save sequence in cwd.
    tmpSeqPath = constructTmpSeqFilePath();
    tmpSeq.setName(constructSeqName());
    saveTmpSeq();
    SAFE_POINT_OP(stateInfo, );

    // Prepare out dir.
    settings.outSettings.outPath = GUrlUtils::prepareDirLocation(constructOutPath(), stateInfo);
    CHECK_OP(stateInfo, );
    settings.outSettings.outPath = GUrlUtils::getSlashEndedPath(settings.outSettings.outPath);
    outHtmlPath = settings.outSettings.outPath + "out.html";

    etStdoutStderrListener = new OutputCollector(false);
    auto etTask = new ExternalToolRunTask(MfoldSupport::ET_MFOLD_ID,
                                          constructEtArgs(),
                                          new ExternalToolLogParser(),
                                          cwd.getURLString());
    etTask->setAdditionalEnvVariables(constructEtEnv());
    etTask->addOutputListener(etStdoutStderrListener);
    addSubTask(etTask);
}

void MfoldTask::run() {
    SAFE_POINT_OP(stateInfo, );
    SAFE_POINT_NN(etStdoutStderrListener, );
    log = etStdoutStderrListener->getLog();
    if (propagateSubtaskError() && log.contains("No foldings.")) {
        setError(tr("No hairpins found. Nothing to show"));
    }

    U2OpStatus2Log os;
    ReportHelper helper(*this, os);
    SAFE_POINT_OP(os, );

    QString fileReport = helper.constructFileReport(os);
    SAFE_POINT_OP(os, );
    QFile file(outHtmlPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(fileReport.toLocal8Bit());
        file.close();
    } else {
        setError(QString(tr("Unable to create output file `%1`")).arg(outHtmlPath));
    }

    report = helper.constructUgeneReport(os);
    SAFE_POINT_OP(os, );
}

QString MfoldTask::generateReport() const {
    return report;
}

QString MfoldTask::constructTmpSeqFilePath() const {
    QString fileName = "inp";
    QString inFilePath = GUrlUtils::getSlashEndedPath(cwd.getURLString()) + fileName;
    return GUrlUtils::getLocalUrlFromUrl(inFilePath, fileName, ".fa", "");
}

QString MfoldTask::constructSeqName() const {
    QString regionStr = getLocationStr(settings.region);
    QString seqName = seqInfo.seqName;
    // Replace non-printable ASCII and non-ASCII characters with _
    seqName.replace(QRegularExpression("[^\x20-\x7E]"), "_");
    // Replace all backslashes with double backslashes because in seqName backslash is the backslash, but in ps file
    // backslash escapes the next character(s). See page 29 of
    // https://web.archive.org/web/20060614215453/http://partners.adobe.com/public/developer/en/ps/PLRM.pdf
    seqName.replace('\\', "\\\\");
    // Unbalanced parentheses are also prohibited, see ps specification linked above. Escaping the parentheses helps.
    seqName.replace('(', "\\(");
    seqName.replace(')', "\\)");
    // Name with two or more dots also won't work. Proof: formid.f:88
    seqName.replace(QRegularExpression("\\.\\.+"), ".");

    seqName = seqName.left(std::min(seqName.size(), maxSeqNameLen - regionStr.size()));
    return seqName + regionStr;
}

QString MfoldTask::constructOutPath() const {
    auto curDt = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm-ss");
    auto suffixedPath = FileAndDirectoryUtils::getAbsolutePath(settings.outSettings.outPath + "/mfold/" + curDt);
    return GUrlUtils::rollFileName(suffixedPath, "_");
}

QStringList MfoldTask::constructEtArgs() const {
    QStringList etArgs = {
        "SEQ=" + tmpSeqPath,
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
    // mfold creates ps files. To convert them to PNG, we need to set width and height in mfold script.
    // These imgs will definitely be used in HTML report of UGENE task. Qt supports a very limited portion of HTML
    // and does so with bugs https://bugreports.qt.io/browse/QTBUG-12283, so it cannot resize images to fit the screen.
    // We must set the correct width ourselves, otherwise img will go beyond the edges of the resulting table.
    // The only problem is with the width.

    // mfold produces all images with a resolution of 792x1072 (defaultImgWidth x defaultImgHeight).
    // So the final width should be as follows:
    // 1. we will take the width of the image equal to 0.5 of report window width (we assume that the report window
    //     will be the same size as the SV window)
    // 2. limit it from below to 300
    // 3. limit it from above to 792 -- the default image width

    // Calculate the height so as to preserve the original proportions (792x1072).
    imgSize.setWidth(qBound(300, windowWidth / 2, defaultImgWidth));
    imgSize.setHeight(imgSize.width() * defaultImgHeight / defaultImgWidth);
    envVars["U2_GS_IMG_SIZE_FOR_UGENE_REPORT"] = QString("%1x%2").arg(imgSize.width()).arg(imgSize.height());
    envVars["U2_GS_IMG_OUT_PATH"] = QString(settings.outSettings.outPath).replace('%', "%%");
    envVars["U2_GS_IMG_DPI_FOR_OUT_REPORT"] = QString::number(settings.outSettings.dpi);
    return envVars;
}

void MfoldTask::saveTmpSeq() {
    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::FASTA);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QScopedPointer<Document> doc(format->createNewLoadedDocument(iof, tmpSeqPath, stateInfo));
    SAFE_POINT_OP(stateInfo, );
    U2EntityRef seqRef = U2SequenceUtils::import(stateInfo, doc->getDbiRef(), tmpSeq);
    SAFE_POINT_OP(stateInfo, );
    doc->addObject(new U2SequenceObject(tmpSeq.getName(), seqRef));
    SAFE_POINT_OP(stateInfo, );
    format->storeDocument(doc.data(), stateInfo);
}

MfoldTask* createMfoldTask(U2SequenceObject* seqObj, const MfoldSettings& settings, int windowWidth, U2OpStatus& os) {
    auto seqLen = seqObj->getSequenceLength();
    U2Region region = settings.region->regions[0];
    auto regionHasJunctionPoint = region.length > seqLen - region.startPos;
    QByteArray seqData;
    if (regionHasJunctionPoint) {
        auto firstPartLen = seqLen - region.startPos;
        U2Region firstRegionPart = {region.startPos, firstPartLen};
        U2Region secondRegionPart = {0, region.length - firstPartLen};
        seqData = seqObj->getSequenceData(firstRegionPart, os);
        CHECK_OP(os, nullptr);
        seqData += seqObj->getSequenceData(secondRegionPart, os);
        CHECK_OP(os, nullptr);
    } else {
        seqData = seqObj->getSequenceData(region, os);
        CHECK_OP(os, nullptr);
    }

    DNASequence seq(seqData, seqObj->getAlphabet());

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
