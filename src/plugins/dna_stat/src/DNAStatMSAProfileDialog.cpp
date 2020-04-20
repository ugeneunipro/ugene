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
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include <U2View/MSAEditor.h>
#include <U2View/WebWindow.h>

#include "DNAStatMSAProfileDialog.h"

namespace U2 {

const QString DNAStatMSAProfileDialog::HTML = "html";
const QString DNAStatMSAProfileDialog::CSV = "csv";

DNAStatMSAProfileDialog::DNAStatMSAProfileDialog(QWidget *p, MSAEditor *_c)
        : QDialog(p),
          ctx(_c),
          saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748883");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    initSaveController();
}

void DNAStatMSAProfileDialog::sl_formatSelected() {
    saveController->setFormat(csvRB->isChecked() ? CSV : HTML);
}

void DNAStatMSAProfileDialog::sl_formatChanged(const QString &newFormat) {
    if (HTML == newFormat) {
        htmlRB->setChecked(true);
    } else {
        csvRB->setChecked(true);
    }
}

void DNAStatMSAProfileDialog::initSaveController() {
    MultipleSequenceAlignmentObject *msaObj = ctx->getMaObject();
    if (msaObj == NULL) {
        return;
    }
    QString domain = "plugin_dna_stat";
    LastUsedDirHelper lod(domain, GUrlUtils::getDefaultDataPath());
    QString fileName = GUrlUtils::fixFileName(msaObj->getGObjectName());

    SaveDocumentControllerConfig config;
    config.defaultDomain = domain;
    config.defaultFormatId = HTML;
    config.defaultFileName = lod.dir + "/" + fileName + "_grid_profile" + "." + DNAStatMSAProfileDialog::HTML;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save file");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(HTML, HTML.toUpper(), QStringList() << HTML);
    formats.addFormat(CSV, CSV.toUpper(), QStringList() << CSV);

    saveController = new SaveDocumentController(config, formats, this);

    connect(saveController, SIGNAL(si_formatChanged(
                                           const QString &)), SLOT(sl_formatChanged(
                                                                           const QString &)));
    connect(htmlRB, SIGNAL(toggled(bool)), SLOT(sl_formatSelected()));
    connect(csvRB, SIGNAL(toggled(bool)), SLOT(sl_formatSelected()));
}

void DNAStatMSAProfileDialog::accept() {
    DNAStatMSAProfileTaskSettings s;
    MultipleSequenceAlignmentObject *msaObj = ctx->getMaObject();
    if (msaObj == NULL) {
        return;
    }
    s.profileName = msaObj->getGObjectName();
    s.profileURL = msaObj->getDocument()->getURLString();
    s.usePercents = percentsRB->isChecked();
    s.ma = msaObj->getMsaCopy();
    s.reportGaps = gapCB->isChecked();
    s.stripUnused = !unusedCB->isChecked();
    s.countGapsInConsensusNumbering = !skipGapPositionsCB->isChecked();
    if (saveBox->isChecked()) {
        s.outURL = saveController->getSaveFileName();
        if (s.outURL.isEmpty()) {
            QMessageBox::critical(this, tr("Error"), tr("File URL is empty"));
            return;
        }
        s.outFormat = csvRB->isChecked() ? DNAStatMSAProfileOutputFormat_CSV : DNAStatMSAProfileOutputFormat_HTML;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(new DNAStatMSAProfileTask(s));
    QDialog::accept();
}


//////////////////////////////////////////////////////////////////////////
// task
DNAStatMSAProfileTask::DNAStatMSAProfileTask(const DNAStatMSAProfileTaskSettings &_s)
        : Task(tr("Generate alignment profile"),
               TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled), s(_s) {
    setVerboseLogMode(true);
}

void DNAStatMSAProfileTask::run() {
    computeStats();
    if (hasError()) {
        return;
    }

    if (s.outFormat != DNAStatMSAProfileOutputFormat_Show && s.outURL.isEmpty()) {
        setError(tr("No output file name specified"));
        return;
    }

    QFile *f = NULL;
    if (s.outFormat == DNAStatMSAProfileOutputFormat_Show || s.outFormat == DNAStatMSAProfileOutputFormat_HTML) {
        bool forIntervalViewer = s.outFormat == DNAStatMSAProfileOutputFormat_Show;
        if (s.outFormat == DNAStatMSAProfileOutputFormat_HTML) {
            f = new QFile(s.outURL);
            if (!f->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
                setError(tr("Can't open file for write: %1").arg(s.outURL));
                return;
            }
        }
        int maxVal = s.usePercents ? 100 : s.ma->getNumRows();
        QString colors[] = {"#ff5555", "#ff9c00", "#60ff00", "#a1d1e5", "#dddddd"};

        // Using subset of the supported HTML features: https://doc.qt.io/qt-5/richtext-html-subset.html
        try {
            resultText = "<!DOCTYPE html>\n<html>\n<head>\n";

            //setup style
            resultText += "<style>\n";
            resultText += ".tbl {border-width: 1px; border-style: solid; border-color: #777777;}\n";
            resultText += ".tbl td {text-align: center; padding: 0 10px; white-space: nowrap;}\n";
            if (!forIntervalViewer) {
                resultText += ".tbl {border-spacing: 0; border-collapse: collapse;}\n";
                resultText += ".tbl td {border: 1px solid #777777;}\n";
            }
            resultText += "</style>\n";

            resultText += "</head>\n<body>\n";

            //header
            resultText += "<h2>" + tr("Multiple Sequence Alignment Grid Profile") + "</h2><br>\n";

            resultText += "<table>\n";
            resultText += "<tr><td><b>" + tr("Alignment file:") + "</b></td><td>" + s.profileURL + "@" + s.profileName +
                          "</td></tr>\n";
            resultText += "<tr><td><b>" + tr("Table content:") + "</b></td><td>" +
                          (s.usePercents ? tr("symbol percents") : tr("symbol counts")) + "</td></tr>\n";
            resultText += "</table>\n";
            resultText += "<br><br>\n";

            if (forIntervalViewer) {
                // Use of -1 for the cellspacing hides cell's border and makes
                // the border style compatible with a standard CSS 'border-collapse: collapse' mode.
                resultText += "<table class=tbl cellspacing=-1 cellpadding=0>";
            } else {
                resultText += "<table class=tbl>";
            }

            //consensus numbers line
            resultText += "<tr><td></td>";
            int pos = 1;
            for (int i = 0; i < columns.size(); i++) {
                const ColumnStat &cs = columns[i];
                QString posStr;
                bool nums = s.countGapsInConsensusNumbering || cs.consChar != U2Msa::GAP_CHAR;
                posStr = nums ? QString::number(pos++) : QString("&nbsp;");
                resultText += "<td>" + posStr + "</td>";
                FileAndDirectoryUtils::dumpStringToFile(f, resultText);
            }
            resultText += "</tr>\n";

            // consensus line
            resultText += "<tr><td> Consensus </td>";
            for (int i = 0; i < columns.size(); i++) {
                ColumnStat &cs = columns[i];
                resultText += "<td><b>" + QString(cs.consChar) + "</b></td>";
            }
            resultText += "</tr>\n";
            // base frequency
            QByteArray aChars = s.ma->getAlphabet()->getAlphabetChars();
            for (int i = 0; i < aChars.size(); i++) {
                char c = aChars[i];
                if (c == U2Msa::GAP_CHAR && !s.reportGaps) {
                    continue;
                }
                if (s.stripUnused && unusedChars.contains(c)) {
                    continue;
                }
                int idx = char2index[c];
                resultText += "<tr>";
                resultText += "<td> " + QString(c) + "</td>";
                for (int j = 0; j < columns.size(); j++) {
                    ColumnStat &cs = columns[j];
                    int val = cs.charFreqs[idx];
                    QString colorStr;
                    int hotness = qRound(100 * double(val) / maxVal);
                    if (hotness >= 90) {
                        colorStr = " bgcolor=" + colors[0];
                    } else if (hotness >= 70) {
                        colorStr = " bgcolor=" + colors[1];
                    } else if (hotness > 50) {
                        colorStr = " bgcolor=" + colors[2];
                    } else if (hotness > 25) {
                        colorStr = " bgcolor=" + colors[3];
                    } else if (hotness > 10) {
                        colorStr = " bgcolor=" + colors[4];
                    }
                    resultText += "<td" + colorStr + ">" + QString::number(cs.charFreqs[idx]) + "</td>";
                    FileAndDirectoryUtils::dumpStringToFile(f, resultText);
                }
                resultText += "</tr>\n";
            }
            resultText += "</table>\n";

            //legend:
            resultText += "<br><br>\n";
            resultText += "<table cellspacing=7 cellpadding=2><tr><td><b>" + tr("Legend:") + "&nbsp;&nbsp;</b>\n";
            resultText += "<td bgcolor=" + colors[4] + ">10%</td>\n";
            resultText += "<td bgcolor=" + colors[3] + ">25%</td>\n";
            resultText += "<td bgcolor=" + colors[2] + ">50%</td>\n";
            resultText += "<td bgcolor=" + colors[1] + ">70%</td>\n";
            resultText += "<td bgcolor=" + colors[0] + ">90%</td>\n";
            resultText += "</tr></table><br>\n";
            resultText += "</body>\n<html>\n";
        } catch (std::bad_alloc &e) {
            Q_UNUSED(e);
            verticalColumnNames.clear();
            columns.clear();
            consenusChars.clear();
            char2index.clear();
            unusedChars.clear();
            resultText.clear();
            if (s.outFormat == DNAStatMSAProfileOutputFormat_Show) {
                setError(
                        tr("There is not enough memory to show this grid profile in UGENE. You can save it to an HTML file and open it with a web browser."));
            } else {
                setError(tr("There is not enough memory to generate this grid profile in UGENE."));
            }
            return;
        }
    } else {
        f = new QFile(s.outURL);
        if (!f->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            setError(tr("Can't open file for write: %1").arg(s.outURL));
            return;
        }
        //out char freqs
        QByteArray aChars = s.ma->getAlphabet()->getAlphabetChars();
        for (int i = 0; i < aChars.size(); i++) {
            char c = aChars[i];
            if (c == U2Msa::GAP_CHAR && !s.reportGaps) {
                continue;
            }
            if (s.stripUnused && unusedChars.contains(c)) {
                continue;
            }
            int idx = char2index[c];
            resultText += QString(c);
            for (int j = 0; j < columns.size(); j++) {
                ColumnStat &cs = columns[j];
                resultText += "," + QString::number(cs.charFreqs[idx]);
                FileAndDirectoryUtils::dumpStringToFile(f, resultText);
            }
            resultText += "\n";
        }
    }

    if (f != NULL) {
        f->write(resultText.toLocal8Bit());
        f->close();
        delete f;
    }
}

QString DNAStatMSAProfileTask::generateReport() const {
    if (hasError()) {
        return tr("Task was finished with an error: %1").arg(getError());
    }
    if (isCanceled()) {
        return tr("Task was canceled.");
    }
    QString res;
    res += "<br>";
    res += tr("Grid profile for %1: <a href='%2'>%2</a>").arg(s.profileName).arg(QDir::toNativeSeparators(s.outURL)) +
           "<br>";
    return res;
}

bool DNAStatMSAProfileTask::isReportingEnabled() const {
    return !hasError() && !isCanceled() && s.outFormat != DNAStatMSAProfileOutputFormat_Show;
}

Task::ReportResult DNAStatMSAProfileTask::report() {
    if (hasError() || isCanceled() || s.outFormat != DNAStatMSAProfileOutputFormat_Show) {
        return Task::ReportResult_Finished;
    }
    assert(!resultText.isEmpty());
    QString title = s.profileName.isEmpty() ? tr("Alignment profile") : tr("Alignment profile for %1").arg(
            s.profileName);

    WebWindow *w = new WebWindow(title, resultText);
    // Qt 5.4 has a bug and does not process 'white-space: nowrap' correctly. Enforcing it using rich text styles.
    w->textBrowser->setWordWrapMode(QTextOption::NoWrap);

    w->setWindowIcon(QIcon(":core/images/chart_bar.png"));
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(w);
    return Task::ReportResult_Finished;
}

void DNAStatMSAProfileTask::computeStats() {
    //fill names
    QByteArray aChars = s.ma->getAlphabet()->getAlphabetChars();
    for (int i = 0; i < aChars.size(); i++) {
        char c = aChars[i];
        verticalColumnNames.append(QChar(c));
        char2index[uchar(c)] = i;
        unusedChars.insert(c);
    }

    //fill values
    columns.resize(s.ma->getLength());
    consenusChars.resize(s.ma->getLength());
    for (int pos = 0; pos < s.ma->getLength(); pos++) {
        int topCharCount = 0;
        ColumnStat &cs = columns[pos];
        cs.charFreqs.resize(aChars.size());
        cs.consChar = U2Msa::GAP_CHAR;
        for (int i = 0; i < s.ma->getNumRows(); i++) {
            char c = s.ma->getMsaRow(i)->charAt(pos);
            unusedChars.remove(c);
            int idx = char2index.value(c);
            int v = ++cs.charFreqs[idx];
            if (v > topCharCount) {
                topCharCount = v;
                cs.consChar = c;
            } else if (v == topCharCount) {
                cs.consChar = U2Msa::GAP_CHAR;
            }
        }
    }

    if (s.usePercents) {
        int charsInColumn = s.ma->getNumRows();
        for (int pos = 0; pos < s.ma->getLength(); pos++) {
            ColumnStat &cs = columns[pos];
            for (int i = 0; i < aChars.size(); i++) {
                char c = aChars[i];
                int idx = char2index.value(c);
                cs.charFreqs[idx] = qRound(100.0 * cs.charFreqs[idx] / charsInColumn);
            }
        }
    }
}


}//namespace

