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

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>

#include <U2Algorithm/MSADistanceAlgorithm.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/TextUtils.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/Notification.h>
#include <U2Gui/SaveDocumentController.h>

#include <U2View/MSAEditor.h>
#include <U2View/WebWindow.h>

#include "DistanceMatrixMSAProfileDialog.h"

namespace U2 {

const QString DistanceMatrixMSAProfileDialog::HTML = "html";
const QString DistanceMatrixMSAProfileDialog::CSV = "csv";

DistanceMatrixMSAProfileDialog::DistanceMatrixMSAProfileDialog(QWidget* p, MSAEditor* _c)
    : QDialog(p),
      ctx(_c),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748884");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Generate"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QList<MSADistanceAlgorithmFactory*> algos = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactories();
    foreach(MSADistanceAlgorithmFactory* a, algos) {
        algoCombo->addItem(a->getName(), a->getId());
    }

    MultipleSequenceAlignmentObject* msaObj = ctx->getMaObject();
    if (msaObj != NULL) {
        QVector<U2Region> unitedRows;
        MultipleSequenceAlignment ma = msaObj->getMsaCopy();
        ma->sortRowsBySimilarity(unitedRows);
        if(unitedRows.size() < 2)
            groupStatisticsCheck->setEnabled(false);
    }

    initSaveController();
}

void DistanceMatrixMSAProfileDialog::initSaveController() {
    MultipleSequenceAlignmentObject* msaObj = ctx->getMaObject();
    if (msaObj == NULL) {
        return;
    }
    QString domain = "plugin_dna_stat";
    LastUsedDirHelper lod(domain, GUrlUtils::getDefaultDataPath());
    QString fileName = GUrlUtils::fixFileName(msaObj->getGObjectName());

    SaveDocumentControllerConfig config;
    config.defaultDomain = domain;
    config.defaultFormatId = HTML;
    config.defaultFileName = lod.dir + "/" + fileName + "_distance_matrix" + "." + DistanceMatrixMSAProfileDialog::HTML;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Save file");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(HTML, HTML.toUpper(), QStringList() << HTML);
    formats.addFormat(CSV, CSV.toUpper(), QStringList() << CSV);

    saveController = new SaveDocumentController(config, formats, this);

    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
    connect(htmlRB, SIGNAL(toggled(bool)), SLOT(sl_formatSelected()));
    connect(csvRB, SIGNAL(toggled(bool)), SLOT(sl_formatSelected()));
}

void DistanceMatrixMSAProfileDialog::accept() {
    DistanceMatrixMSAProfileTaskSettings s;
    MultipleSequenceAlignmentObject* msaObj = ctx->getMaObject();
    if (msaObj == NULL) {
        return;
    }
    s.profileName = msaObj->getGObjectName();
    s.profileURL = msaObj->getDocument()->getURLString();
    s.usePercents = percentsRB->isChecked();
    s.algoId = algoCombo->currentData().toString();
    s.ma = msaObj->getMsaCopy();
    s.excludeGaps = checkBox->isChecked();
    s.showGroupStatistic = groupStatisticsCheck->isChecked();
    s.ctx = ctx;

    if (saveBox->isChecked()) {
        s.outURL = saveController->getSaveFileName();
        if (s.outURL.isEmpty()) {
            QMessageBox::critical(this, tr("Error"), tr("File URL is empty"));
            return;
        }
        s.outFormat = csvRB->isChecked() ? DistanceMatrixMSAProfileOutputFormat_CSV : DistanceMatrixMSAProfileOutputFormat_HTML;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(new DistanceMatrixMSAProfileTask(s));
    QDialog::accept();
}

void DistanceMatrixMSAProfileDialog::sl_formatSelected() {
    saveController->setFormat(csvRB->isChecked() ? CSV : HTML);
}

void DistanceMatrixMSAProfileDialog::sl_formatChanged(const QString &newFormatId) {
    if (HTML == newFormatId) {
        htmlRB->setChecked(true);
    } else {
        csvRB->setChecked(true);
    }
}


//////////////////////////////////////////////////////////////////////////
// task

DistanceMatrixMSAProfileTask::DistanceMatrixMSAProfileTask(const DistanceMatrixMSAProfileTaskSettings& _s)
: Task(tr("Generate distance matrix"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled), s(_s)
{
    setVerboseLogMode(true);
}

void DistanceMatrixMSAProfileTask::prepare() {
    MSADistanceAlgorithmFactory* factory = AppContext::getMSADistanceAlgorithmRegistry()->getAlgorithmFactory(s.algoId);
    if(s.excludeGaps){
        factory->setFlag(DistanceAlgorithmFlag_ExcludeGaps);
    }else{
        factory->resetFlag(DistanceAlgorithmFlag_ExcludeGaps);
    }
    MSADistanceAlgorithm* algo = factory->createAlgorithm(s.ma);
    if (algo == NULL) {
        return;
    }
    addSubTask(algo);
}

QList<Task*> DistanceMatrixMSAProfileTask::onSubTaskFinished(Task* subTask) {
    MSADistanceAlgorithm* algo = qobject_cast<MSADistanceAlgorithm*>(subTask);
    QList<Task*> res;
    if (algo != NULL) {
        if (algo->hasError() || algo->isCanceled()) {
            setError(algo->getError());
            return res;
        }
        if (s.outFormat != DistanceMatrixMSAProfileOutputFormat_Show && s.outURL.isEmpty()) {
            setError(tr("No output file name specified"));
            return res;
        }
        QFile *f = NULL;
        if (s.outFormat == DistanceMatrixMSAProfileOutputFormat_Show || s.outFormat == DistanceMatrixMSAProfileOutputFormat_HTML) {
            if (s.outFormat == DistanceMatrixMSAProfileOutputFormat_HTML) {
                f = new QFile(s.outURL);
                if (!f->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
                    setError(tr("Can't open file for write: %1").arg(s.outURL));
                    return res;
                }
            }
            QString colors[] = { "#ff5555", "#ff9c00", "#60ff00", "#a1d1e5", "#dddddd" };

            //setup style
            resultText = "<!DOCTYPE html>\n<html>\n<head>\n";
            resultText += "<style>\n";
            resultText += ".tbl {border-width: 1px; border-style: solid; border-spacing: 0; border-collapse: collapse;}\n";
            resultText += ".tbl td {max-width: 400px; min-width: 20px; text-align: center; border-width: 1px; border-style: solid; padding: 0 10px;}\n";
            resultText += "</style>\n";
            resultText += "</head>\n<body>\n";

            //header
            resultText += "<h2>" + tr("Multiple Sequence Alignment Distance Matrix") + "</h2><br>\n";

            resultText += "<table>\n";
            resultText += "<tr><td><b>" + tr("Alignment file:") + "</b></td><td>" + s.profileURL + "@" + s.profileName + "</td></tr>\n";
            resultText += "<tr><td><b>" + tr("Table content:") + "</b></td><td>" + (s.usePercents ? (algo->getName() + " in percent") : algo->getName()) + "</td></tr>\n";
            resultText += "</table>\n";
            resultText += "<br><br>\n";

            FileAndDirectoryUtils::dumpStringToFile(f, resultText);
            bool isSimilarity = algo->isSimilarityMeasure();
            try {
                createDistanceTable(algo, s.ma->getMsaRows(), f);
            } catch (std::bad_alloc &e) {
                Q_UNUSED(e);
                setError(tr("There is not enough memory to show this distance matrix in UGENE. You can save it to an HTML file and open it with a web browser."));
                return res;
            }

            resultText += "<br><br>\n";

            if (s.showGroupStatistic) {
                resultText += "<tr><td><b>" + tr("Group statistics of multiple alignment") + "</td></tr>\n";
                resultText += "<table>\n";
                QVector<U2Region> unitedRows;
                s.ma->sortRowsBySimilarity(unitedRows);
                QList<MultipleSequenceAlignmentRow> rows;
                int i = 1;
                srand(QDateTime::currentDateTime().toTime_t());
                foreach(const U2Region &reg, unitedRows) {
                    MultipleSequenceAlignmentRow row = s.ma->getMsaRow(reg.startPos + qrand() % reg.length);
                    row->setName(QString("Group %1: ").arg(i) + "(" + row->getName() + ")");
                    rows.append(s.ma->getMsaRow(reg.startPos + qrand() % reg.length)->getExplicitCopy());

                    resultText += "<tr><td><b>" + QString("Group %1: ").arg(i) + "</b></td><td>";
                    for (int x = reg.startPos; x < reg.endPos(); x++) {
                        resultText += s.ma->getMsaRow(x)->getName() + ", ";
                    }
                    resultText += "\n";
                    i++;
                    FileAndDirectoryUtils::dumpStringToFile(f, resultText);
                }
                resultText += "</table>\n";
                resultText += "<br><br>\n";
                try {
                    createDistanceTable(algo, rows, f);
                } catch (std::bad_alloc &e) {
                    Q_UNUSED(e);
                    setError(tr("There is not enough memory to show this distance matrix in UGENE. You can save it to an HTML file and open it with a web browser."));
                    return res;
                }


                //legend:
                resultText += "<br><br>\n";
                resultText += "<table><tr><td><b>" + tr("Legend:") + "&nbsp;&nbsp;</b>\n";
                if (isSimilarity) {
                    resultText += "<td bgcolor=" + colors[4] + ">10%</td>\n";
                    resultText += "<td bgcolor=" + colors[3] + ">25%</td>\n";
                    resultText += "<td bgcolor=" + colors[2] + ">50%</td>\n";
                    resultText += "<td bgcolor=" + colors[1] + ">70%</td>\n";
                    resultText += "<td bgcolor=" + colors[0] + ">90%</td>\n";
                } else {
                    resultText += "<td bgcolor=" + colors[0] + ">10%</td>\n";
                    resultText += "<td bgcolor=" + colors[1] + ">25%</td>\n";
                    resultText += "<td bgcolor=" + colors[2] + ">50%</td>\n";
                    resultText += "<td bgcolor=" + colors[3] + ">70%</td>\n";
                    resultText += "<td bgcolor=" + colors[4] + ">90%</td>\n";
                }
                resultText += "</tr></table><br>\n";
            }
            resultText += "</body>\n<html>\n";
        } else {
            f = new QFile(s.outURL);
            if (!f->open(QIODevice::Truncate | QIODevice::WriteOnly)) {
                setError(tr("Can't open file for write: %1").arg(s.outURL));
                return res;
            }
            resultText += " ";
            for (int i = 0; i < s.ma->getNumRows(); i++) {
                QString name = s.ma->getMsaRow(i)->getName();
                TextUtils::wrapForCSV(name);
                resultText += "," + name;
                FileAndDirectoryUtils::dumpStringToFile(f, resultText);
            }
            resultText += "\n";

            for (int i = 0; i < s.ma->getNumRows(); i++) {
                QString name = s.ma->getMsaRow(i)->getName();
                TextUtils::wrapForCSV(name);
                resultText += name;
                for (int j = 0; j < s.ma->getNumRows(); j++) {
                    int val = algo->getSimilarity(i, j, s.usePercents);

                    resultText += "," + QString::number(val) + (s.usePercents ? "%" : "");
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
    return res;
}


void DistanceMatrixMSAProfileTask::createDistanceTable(MSADistanceAlgorithm* algo, const QList<MultipleSequenceAlignmentRow> &rows, QFile *f)
{
    int maxVal = s.usePercents ? 100 : s.ma->getLength();
    QString colors[] = {"#ff5555", "#ff9c00", "#60ff00", "#a1d1e5", "#dddddd"};
    bool isSimilarity = algo->isSimilarityMeasure();

    if(rows.size() < 2) {
        resultText += "<tr><td><b>"+tr("There is not enough groups to create distance matrix!") + "</td></tr>\n";
        return;
    }

    bool forIntervalViewer = s.outFormat == DistanceMatrixMSAProfileOutputFormat_Show;
    if (forIntervalViewer) {
        // Use of -1 for the cellspacing hides cell's border and makes
        // the border style compatible with a standard CSS 'border-collapse: collapse' mode.
        resultText += "<table class=tbl cellspacing=-1 cellpadding=0>\n";
    } else {
        resultText += "<table class=tbl>\n";
    }

    resultText += "<tr><td></td>";
    for (int i=0; i < rows.size(); i++) {
        QString name = rows.at(i)->getName();
        resultText += "<td> " + name + "</td>";
    }
    resultText += "</tr>\n";

    //out char freqs
    for (int i=0; i < rows.size(); i++) {
        QString name = rows.at(i)->getName();
        resultText += "<tr>";
        resultText += "<td> " + name + "</td>";
        for (int j=0; j < rows.size(); j++) {
            int val = algo->getSimilarity(i, j, s.usePercents);

            QString colorStr = "";
            if (i != j) {
                int hotness = qRound(100 * double(val) / maxVal);
                if ((hotness  >= 90 && isSimilarity) || (hotness <= 10 && !isSimilarity))  {
                    colorStr = " bgcolor=" + colors[0];
                } else if ((hotness  > 70 && isSimilarity) || (hotness <= 25 && !isSimilarity)) {
                    colorStr = " bgcolor=" + colors[1];
                } else if ((hotness  > 50 && isSimilarity) || (hotness <= 50 && !isSimilarity)) {
                    colorStr = " bgcolor=" + colors[2];
                } else if ((hotness  > 25 && isSimilarity) || (hotness <= 70 && !isSimilarity)) {
                    colorStr = " bgcolor=" + colors[3];
                } else if ((hotness  > 10 && isSimilarity) || (hotness < 90 && !isSimilarity)) {
                    colorStr = " bgcolor=" + colors[4];
                }
            }
            resultText += "<td"+colorStr+">" + QString::number(val) + (s.usePercents ? "%" : "") + "</td>";
            FileAndDirectoryUtils::dumpStringToFile(f, resultText);
        }
        resultText += "</tr>\n";
    }
    resultText += "</table>\n";
}
QString DistanceMatrixMSAProfileTask::generateReport() const {
    if (hasError() || isCanceled()) {
        return tr("Task was finished with an error: %1").arg(getError());
    }
    QString res;
    res += "<br>";
    res += QString(tr("Distanse matrix for %1: <a href='%2'>%2</a>")).arg(s.profileName).arg(QDir::toNativeSeparators(s.outURL)) + "<br>";
    return res;
}

bool DistanceMatrixMSAProfileTask::isReportingEnabled() const {
    return !hasError() && !isCanceled() && s.outFormat != DistanceMatrixMSAProfileOutputFormat_Show;
}


Task::ReportResult DistanceMatrixMSAProfileTask::report() {
    if (hasError() || isCanceled() || s.outFormat != DistanceMatrixMSAProfileOutputFormat_Show) {
        return Task::ReportResult_Finished;
    }
    assert(!resultText.isEmpty());
    QString title = s.profileName.isEmpty() ? tr("Distance matrix") : tr("Distance matrix for %1").arg(s.profileName);
    WebWindow* w = new WebWindow(title, resultText);

    // Qt 5.4 has a bug and does not process 'white-space: nowrap' correctly. Enforcing it using rich text styles.
    w->textBrowser->setWordWrapMode(QTextOption::NoWrap);

    w->setWindowIcon(QIcon(":core/images/chart_bar.png"));
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(w);
    return Task::ReportResult_Finished;
}

DistanceMatrixMSAProfileTaskSettings::DistanceMatrixMSAProfileTaskSettings() :
    usePercents(false),
    excludeGaps(false),
    showGroupStatistic(false),
    outFormat(DistanceMatrixMSAProfileOutputFormat_Show),
    ctx(NULL)
{

}

}//namespace
