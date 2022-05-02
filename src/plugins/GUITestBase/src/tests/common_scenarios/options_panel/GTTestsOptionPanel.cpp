/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GTTestsOptionPanel.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QFile>
#include <QFontComboBox>
#include <QTextStream>

#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"

namespace U2 {

namespace GUITest_common_scenarios_options_panel {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    Options panel. Information tab. Character occurrence
    //    1. Open file (samples/FASTA/human_T1.fa)
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));

    auto w = GTWidget::findWidget(os, "Characters Occurrence");
    GTWidget::click(os, w);
    QLabel* l = w->findChild<QLabel*>();
    QString s = QString("<table cellspacing=5><tr><td><b>A:&nbsp;&nbsp;"
                        "</td><td>62 842 &nbsp;&nbsp;</td><td>31.4%&nbsp;&nbsp;"
                        "</td></tr><tr><td><b>C:&nbsp;&nbsp;</td><td>40 041 &nbsp;"
                        "&nbsp;</td><td>20.0%&nbsp;&nbsp;</td></tr><tr><td><b>G:&nbsp;"
                        "&nbsp;</td><td>37 622 &nbsp;&nbsp;</td><td>18.8%&nbsp;&nbsp;"
                        "</td></tr><tr><td><b>T:&nbsp;&nbsp;</td><td>59 445 &nbsp;&nbsp;"
                        "</td><td>29.7%&nbsp;&nbsp;</td></tr></table>");
    CHECK_SET_ERR(l->text() == s, "Found: " + l->text());
    //    Expected state: next statistics has shown
    //    A: 62 842 31.4%
    //    C: 40 041 20.0%
    //    G: 37 622 18.8%
    //    T: 59 445 29.7%
}

GUI_TEST_CLASS_DEFINITION(test_0001_1) {
    //    Options panel. Information tab. Character occurrence
    //    1. Open file (_common_data/scenarios/_regression/1093/refrence.fa)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/1093/", "refrence.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));

    auto w = GTWidget::findWidget(os, "Characters Occurrence");
    GTWidget::click(os, w);
    QLabel* l = w->findChild<QLabel*>();
    QString s = QString("<table cellspacing=5><tr><td><b>A:&nbsp;&nbsp;</td><td>31 &nbsp;&nbsp;"
                        "</td><td>27.2%&nbsp;&nbsp;</td></tr><tr><td><b>C:&nbsp;&nbsp;"
                        "</td><td>30 &nbsp;&nbsp;</td><td>26.3%&nbsp;&nbsp;</td></tr><tr><td><b>G:&nbsp;"
                        "&nbsp;</td><td>26 &nbsp;&nbsp;</td><td>22.8%&nbsp;&nbsp;"
                        "</td></tr><tr><td><b>T:&nbsp;&nbsp;</td><td>27 &nbsp;&nbsp;"
                        "</td><td>23.7%&nbsp;&nbsp;</td></tr></table>");
    CHECK_SET_ERR(l->text() == s, "Found: " + l->text());
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Options panel. Information tab. Dinucleotides
    // 1. Open file (samples/FASTA/human_T1.fa)
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Activate Information tab on Options panel at the right edge of UGENE window. Expand Dinucleotides
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));

    auto w = GTWidget::findWidget(os, "Dinucleotides");
    GTWidget::click(os, w);
    QLabel* l = w->findChild<QLabel*>();
    QString s = QString("<table cellspacing=5><tr><td><b>AA:&nbsp;&nbsp;</td><td>"
                        "21 960 &nbsp;&nbsp;</td></tr><tr><td><b>AC:&nbsp;&nbsp;</td>"
                        "<td>10 523 &nbsp;&nbsp;</td></tr><tr><td><b>AG:&nbsp;&nbsp;"
                        "</td><td>13 845 &nbsp;&nbsp;</td></tr><tr><td><b>AT:&nbsp;"
                        "&nbsp;</td><td>16 514 &nbsp;&nbsp;</td></tr><tr><td><b>"
                        "CA:&nbsp;&nbsp;</td><td>15 012 &nbsp;&nbsp;</td></tr><tr>"
                        "<td><b>CC:&nbsp;&nbsp;</td><td>9 963 &nbsp;&nbsp;"
                        "</td></tr><tr><td><b>CG:&nbsp;&nbsp;</td><td>1 646 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>CT:&nbsp;&nbsp;</td><td>13 420 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>GA:&nbsp;&nbsp;</td><td>11 696 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>GC:&nbsp;&nbsp;</td><td>7 577 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>GG:&nbsp;&nbsp;</td><td>8 802 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>GT:&nbsp;&nbsp;</td><td>9 546 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>TA:&nbsp;&nbsp;</td><td>14 174 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>TC:&nbsp;&nbsp;</td><td>11 978 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>TG:&nbsp;&nbsp;</td><td>13 329 &nbsp;"
                        "&nbsp;</td></tr><tr><td><b>TT:&nbsp;&nbsp;</td><td>19 964 &nbsp;&nbsp;</td></tr></table>");
    CHECK_SET_ERR(l->text() == s, "Found: " + l->text());
    /*Expected state: next statistics has shown
AA:  21 960
AC:  10 523
AG:  13 845
AT:  16 514
CA:  15 012
CC:  9 963
CG:  1 646
CT:  13 420
GA:  11 696
GC:  7 577
GG:  8 802
GT:  9 546
TA:  14 174
TC:  11 978
TG:  13 329
TT:  19 964*/
}

GUI_TEST_CLASS_DEFINITION(test_0002_1) {
    // Options panel. Information tab. Dinucleotides
    //     1. Open file (_common_data/scenarios/_regression/1093/refrence.fa)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/1093/", "refrence.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Activate Information tab on Options panel at the right edge of UGENE window. Expand Dinucleotides
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));

    auto w = GTWidget::findWidget(os, "Dinucleotides");
    GTWidget::click(os, w);
    QLabel* l = w->findChild<QLabel*>();
    QString s = QString("<table cellspacing=5><tr><td><b>AA:&nbsp;&nbsp;"
                        "</td><td>6 &nbsp;&nbsp;</td></tr><tr><td><b>AC:&nbsp;&nbsp;"
                        "</td><td>9 &nbsp;&nbsp;</td></tr><tr><td><b>AG:&nbsp;&nbsp;"
                        "</td><td>3 &nbsp;&nbsp;</td></tr><tr><td><b>AT:&nbsp;&nbsp;"
                        "</td><td>13 &nbsp;&nbsp;</td></tr><tr><td><b>CA:&nbsp;&nbsp;"
                        "</td><td>5 &nbsp;&nbsp;</td></tr><tr><td><b>CC:&nbsp;&nbsp;"
                        "</td><td>1 &nbsp;&nbsp;</td></tr><tr><td><b>CG:&nbsp;&nbsp;"
                        "</td><td>20 &nbsp;&nbsp;</td></tr><tr><td><b>CT:&nbsp;&nbsp;"
                        "</td><td>4 &nbsp;&nbsp;</td></tr><tr><td><b>GA:&nbsp;&nbsp;"
                        "</td><td>9 &nbsp;&nbsp;</td></tr><tr><td><b>GC:&nbsp;&nbsp;"
                        "</td><td>11 &nbsp;&nbsp;</td></tr><tr><td><b>GG:&nbsp;&nbsp;"
                        "</td><td>1 &nbsp;&nbsp;</td></tr><tr><td><b>GT:&nbsp;&nbsp;"
                        "</td><td>5 &nbsp;&nbsp;</td></tr><tr><td><b>TA:&nbsp;&nbsp;"
                        "</td><td>10 &nbsp;&nbsp;</td></tr><tr><td><b>TC:&nbsp;&nbsp;"
                        "</td><td>9 &nbsp;&nbsp;</td></tr><tr><td><b>TG:&nbsp;&nbsp;"
                        "</td><td>2 &nbsp;&nbsp;</td></tr><tr><td><b>TT:&nbsp;&nbsp;"
                        "</td><td>5 &nbsp;&nbsp;</td></tr></table>");
    CHECK_SET_ERR(l->text() == s, "Found: " + l->text());
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Options panel. Information tab. Sequence length
    //    1. Open file (samples/FASTA/human_T1.fa)
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: sequence length must be "199950 nt"
    CHECK_SET_ERR(statisticsLabel->text().contains("<tr><td>Length: </td><td>199 950 nt</td></tr>"),
                  "Sequence length is wrong");
}

GUI_TEST_CLASS_DEFINITION(test_0003_1) {
    //    Options panel. Information tab. Sequence length
    //    1. Open file (_common_data/scenarios/_regression/1093/refrence.fa)
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/1093/", "refrence.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state: sequence length must be "114 nt"
    CHECK_SET_ERR(statisticsLabel->text().contains("<tr><td>Length: </td><td>114 nt</td></tr>"),
                  "Sequence length is wrong");
}
GUI_TEST_CLASS_DEFINITION(test_0004) {
    // 1. Open file (samples/FASTA/human_T1.fa)
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // 2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));
    auto w = GTWidget::findWidget(os, "Characters Occurrence");
    GTWidget::click(os, w);

    QPoint point = GTMouseDriver::getMousePosition();

    GTMouseDriver::moveTo(point - QPoint(15, 0));  // move 15 pix left
    GTMouseDriver::press();

    GTMouseDriver::moveTo(point + QPoint(80, 0));  // move 80 pix right
    GTMouseDriver::release();

    GTThread::waitForMainThread();

    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text(os);
    QString text = QString("A:  \n"
                           "62 842   \n"
                           "31.4%  \n"
                           "C:  \n"
                           "40 041   \n"
                           "20.0%  \n"
                           "G:  \n"
                           "37 622   \n"
                           "18.8%  \n"
                           "T:  \n"
                           "59 445   \n"
                           "29.7%  ");
    CHECK_SET_ERR(clipboardText.contains(text), "\nExpected:\n" + text + "\nFound: " + clipboardText);

    // 3. Use context menu to select and copy information from "Character Occurrence". Paste copied information into test editor
    // Expected state: copied and pasted information are identical
}
GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    Options panel. Copying
    //    1. Open file (_common_data\fasta\multy_fa.fa). Open fiel in separate sequences mode.
    GTUtilsProject::openMultiSequenceFileAsSequences(os, testDir + "_common_data/fasta/multy_fa.fa");

    //    2. Activate Information tab on Options panel at the right edge of UGENE window.
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTWidget::click(os, GTWidget::findWidget(os, "OP_SEQ_INFO"));
    auto w = GTWidget::findWidget(os, "Characters Occurrence");
    GTWidget::click(os, w);

    QLabel* l = w->findChild<QLabel*>();
    QString s = l->text();

    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_1"));
    // w=GTWidget::findWidget(os,"Characters Occurrence");
    GTWidget::click(os, w);
    // l=w->findChild<QLabel*>();

    CHECK_SET_ERR(s != l->text(), l->text());
    //    3. Activate another opened sequence.
    //    Expected state: information in options panel has changed
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj3.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "1.gb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick();

    // Press ctrl+f. Check focus. Find subsequence TA
    GTUtilsOptionsPanel::runFindPatternWithHotKey("TA", os);

    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));

    GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");
}

GUI_TEST_CLASS_DEFINITION(test_0006_1) {
    // DEFFERS: OTHER SOURSE FILE, OTHER SUBSEQUENCE
    // PROJECT IS CLOSED MANUALLY TO CACHE MESSAGEBOX
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionsPanel::runFindPatternWithHotKey("TTTTTAAAAA", os);

    GTWidget::click(os, GTWidget::findWidget(os, "getAnnotationsPushButton"));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem(os, "misc_feature");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os, item));

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    QList<QString> keys = GTUtilsProjectTreeView::getDocuments(os).keys();
    QString name;
    foreach (const QString& key, keys) {
        if (key.startsWith("MyDocument")) {
            name = key;
            break;
        }
    }
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, name));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__REMOVE_SELECTED));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // nucl statistics 1
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    QString s = QString("<table cellspacing=5>"
                        "<tr><td>Length: </td><td>200 nt</td></tr>"
                        "<tr><td>GC content: </td><td>44.50%</td></tr>"
                        "<tr><td>Melting temperature: </td><td>79.78 &#176;C</td></tr>"
                        "<tr><td colspan=2><b>ssDNA:</b></td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Molecular weight: </td><td>61909.78 Da</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Extinction coefficient: </td><td>1987400 l/(mol * cm)</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;nmole/OD<sub>260</sub>: </td><td>0.50</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;%1g/OD<sub>260</sub>: </td><td>31.15</td></tr>"
                        "<tr><td colspan=2><b>dsDNA:</b></td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Molecular weight: </td><td>123446.17 Da</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Extinction coefficient: </td><td>3118241 l/(mol * cm)</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;nmole/OD<sub>260</sub>: </td><td>0.32</td></tr>"
                        "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;%1g/OD<sub>260</sub>: </td><td>39.59</td></tr>"
                        "</table>")
                    .arg(QChar(0x3BC));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(statisticsLabel->text() == s, QString("Unexpected statistics label text:\nexpected:\n%1\nFound:\n%2").arg(s).arg(statisticsLabel->text()));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // nucl statistics 2
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    QString s = QString("Length: </td><td>199 950 nt");
    QString s1 = QString("GC content: </td><td>38.84%");
    QString s2 = QString("Melting temperature: </td><td>80.82 &#176;C");

    // ssDNA
    QString s3 = QString("Molecular weight: </td><td>61730845.26 Da");
    QString s4 = QString("Extinction coefficient: </td><td>1954366300 l/(mol * cm)");
    QString s5 = QString("nmole/OD<sub>260</sub>: </td><td>0.00");
    QString s6 = QString("g/OD<sub>260</sub>: </td><td>31.59");

    // dsDNA
    QString s7 = QString("Molecular weight: </td><td>123527891.93 Da");
    QString s8 = QString("Extinction coefficient: </td><td>3136291737 l/(mol * cm)");
    QString s9 = QString("nmole/OD<sub>260</sub>: </td><td>0.00");
    QString s10 = QString("g/OD<sub>260</sub>: </td><td>39.39");

    GTUtilsOptionsPanel::resizeToMaximum(os);
    QString labelText = statisticsLabel->text();

    CHECK_SET_ERR(labelText.contains(s), QString("label text: %1. It does not contais %2").arg(labelText).arg(s));
    CHECK_SET_ERR(labelText.contains(s1), QString("label text: %1. It does not contais %2").arg(labelText).arg(s1));
    CHECK_SET_ERR(labelText.contains(s2), QString("label text: %1. It does not contais %2").arg(labelText).arg(s2));
    CHECK_SET_ERR(labelText.contains(s3), QString("label text: %1. It does not contais %2").arg(labelText).arg(s3));
    CHECK_SET_ERR(labelText.contains(s4), QString("label text: %1. It does not contais %2").arg(labelText).arg(s4));
    CHECK_SET_ERR(labelText.contains(s5), QString("label text: %1. It does not contais %2").arg(labelText).arg(s5));
    CHECK_SET_ERR(labelText.contains(s6), QString("label text: %1. It does not contais %2").arg(labelText).arg(s6));
    CHECK_SET_ERR(labelText.contains(s7), QString("label text: %1. It does not contais %2").arg(labelText).arg(s7));
    CHECK_SET_ERR(labelText.contains(s8), QString("label text: %1. It does not contais %2").arg(labelText).arg(s8));
    CHECK_SET_ERR(labelText.contains(s9), QString("label text: %1. It does not contais %2").arg(labelText).arg(s9));
    CHECK_SET_ERR(labelText.contains(s10), QString("label text: %1. It does not contais %2").arg(labelText).arg(s10));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // amino statistics
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "titin.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    QString s = QString("<table cellspacing=5>"
                        "<tr><td>Length: </td><td>26 926 aa</td></tr>"
                        "<tr><td>Molecular weight: </td><td>2993901.23</td></tr>"
                        "<tr><td>Isoelectic point: </td><td>6.74</td></tr></table>");

    CHECK_SET_ERR(statisticsLabel->text() == s, "Found: " + statisticsLabel->text());
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // nucl statistics update on selection
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");
    GTUtilsOptionsPanel::resizeToMaximum(os);
    QString labelText = statisticsLabel->text();

    QString s = QString("Length: </td><td>199 950 nt");
    QString s1 = QString("GC content: </td><td>38.84%");
    QString s2 = QString("Melting temperature: </td><td>80.82 &#176;C");

    // ssDNA
    QString s3 = QString("Molecular weight: </td><td>61730845.26 Da");
    QString s4 = QString("Extinction coefficient: </td><td>1954366300 l/(mol * cm)");
    QString s5 = QString("nmole/OD<sub>260</sub>: </td><td>0.00");
    QString s6 = QString("g/OD<sub>260</sub>: </td><td>31.59");

    // dsDNA
    QString s7 = QString("Molecular weight: </td><td>123527891.93 Da");
    QString s8 = QString("Extinction coefficient: </td><td>3136291737 l/(mol * cm)");
    QString s9 = QString("nmole/OD<sub>260</sub>: </td><td>0.00");
    QString s10 = QString("g/OD<sub>260</sub>: </td><td>39.39");

    CHECK_SET_ERR(labelText.contains(s), QString("label text: %1. It does not contais %2").arg(labelText).arg(s));
    CHECK_SET_ERR(labelText.contains(s1), QString("label text: %1. It does not contais %2").arg(labelText).arg(s1));
    CHECK_SET_ERR(labelText.contains(s2), QString("label text: %1. It does not contais %2").arg(labelText).arg(s2));
    CHECK_SET_ERR(labelText.contains(s3), QString("label text: %1. It does not contais %2").arg(labelText).arg(s3));
    CHECK_SET_ERR(labelText.contains(s4), QString("label text: %1. It does not contais %2").arg(labelText).arg(s4));
    CHECK_SET_ERR(labelText.contains(s5), QString("label text: %1. It does not contais %2").arg(labelText).arg(s5));
    CHECK_SET_ERR(labelText.contains(s6), QString("label text: %1. It does not contais %2").arg(labelText).arg(s6));
    CHECK_SET_ERR(labelText.contains(s7), QString("label text: %1. It does not contais %2").arg(labelText).arg(s7));
    CHECK_SET_ERR(labelText.contains(s8), QString("label text: %1. It does not contais %2").arg(labelText).arg(s8));
    CHECK_SET_ERR(labelText.contains(s9), QString("label text: %1. It does not contais %2").arg(labelText).arg(s9));
    CHECK_SET_ERR(labelText.contains(s10), QString("label text: %1. It does not contais %2").arg(labelText).arg(s10));

    // select sequence region
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 40);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(labelText != statisticsLabel->text(), "Statistics did not change");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // raw alphabet
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/numbers_in_the_middle.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    QString s = QString("<table cellspacing=5>"
                        "<tr><td>Length: </td><td>230 </td></tr>"
                        "</table>");

    CHECK_SET_ERR(statisticsLabel->text() == s, "Found: " + statisticsLabel->text());
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // focus change
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/numbers_in_the_middle.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto statisticsLabel = GTWidget::findLabel(os, "Common Statistics");

    auto w0 = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    GTWidget::click(os, w0);
    QString s = QString("<table cellspacing=5>"
                        "<tr><td>Length: </td><td>70 </td></tr>"
                        "</table>");
    CHECK_SET_ERR(statisticsLabel->text() == s, "Statistics is wrong!");

    auto w1 = GTWidget::findWidget(os, "ADV_single_sequence_widget_1");
    GTWidget::click(os, w1);
    s = QString("<table cellspacing=5>"
                "<tr><td>Length: </td><td>70 nt</td></tr>"
                "<tr><td>GC content: </td><td>49.29%</td></tr>"
                "<tr><td>Melting temperature: </td><td>75.36 &#176;C</td></tr>"
                "<tr><td colspan=2><b>ssDNA:</b></td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Molecular weight: </td><td>21572.21 Da</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Extinction coefficient: </td><td>656800 l/(mol * cm)</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;nmole/OD<sub>260</sub>: </td><td>1.52</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;%1g/OD<sub>260</sub>: </td><td>32.84</td></tr>"
                "<tr><td colspan=2><b>dsDNA:</b></td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Molecular weight: </td><td>43128.92 Da</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Extinction coefficient: </td><td>1090150 l/(mol * cm)</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;nmole/OD<sub>260</sub>: </td><td>0.92</td></tr>"
                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;%1g/OD<sub>260</sub>: </td><td>39.56</td></tr>"
                "</table>")
            .arg(QChar(0x3BC));

    CHECK_SET_ERR(statisticsLabel->text() == s, "Statistics is wrong!");

    auto w2 = GTWidget::findWidget(os, "ADV_single_sequence_widget_2");
    GTWidget::click(os, w2);
    s = QString("<table cellspacing=5>"
                "<tr><td>Length: </td><td>70 aa</td></tr>"
                "<tr><td>Molecular weight: </td><td>5752.43</td></tr>"
                "<tr><td>Isoelectic point: </td><td>5.15</td></tr></table>");
    CHECK_SET_ERR(statisticsLabel->text() == s, "Statistics is wrong!");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // 1. Open linear nucl sequence
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open "Circular View Settings" tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);

    // 3. Check the hint: it is visible
    auto openCvWidget = GTWidget::findWidget(os, "openCvWidget");
    CHECK_SET_ERR(openCvWidget->isVisible(), "Hint label and OpenCV button should be visible");

    // 4. Open CV
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 5. Check the hint: it is hidden
    CHECK_SET_ERR(openCvWidget->isHidden(), "Hint label and OpenCV button should be hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // 1. Open sequence with CV
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 2. Set some circular settings
    const int fontSize1 = 28;
    GTUtilsOptionPanelSequenceView::setTitleFontSize(os, fontSize1);
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 3. Open another sequence
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 4. Check difference between the modified and newly opened settings
    const int fontSize2 = GTUtilsOptionPanelSequenceView::getTitleFontSize(os);
    CHECK_SET_ERR(fontSize1 != fontSize2, "CV Settings should be differenct for different documents");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // 1. Open sequence
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open CV
    // 3. Open CV Settings tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 4. Select each available label position option
    auto positionComboBox = GTWidget::findComboBox(os, "labelPositionComboBox");
    CHECK_SET_ERR(positionComboBox->count() == 4, "Wrong amount of available label position");
    GTComboBox::selectItemByIndex(os, positionComboBox, 0);
    GTComboBox::selectItemByIndex(os, positionComboBox, 1);
    GTComboBox::selectItemByIndex(os, positionComboBox, 2);
    GTComboBox::selectItemByIndex(os, positionComboBox, 3);
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    // 1. Open sequence with CV
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open CV Settings tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);

    // 3. Check font spinboxes bound values
    auto titleFontSpinBox = GTWidget::findSpinBox(os, "fontSizeSpinBox");
    auto rulerFontSpinBox = GTWidget::findSpinBox(os, "rulerFontSizeSpinBox");
    auto annotFontSpinBox = GTWidget::findSpinBox(os, "labelFontSizeSpinBox");

    GTSpinBox::checkLimits(os, titleFontSpinBox, 7, 48);
    GTSpinBox::checkLimits(os, rulerFontSpinBox, 7, 24);
    GTSpinBox::checkLimits(os, annotFontSpinBox, 7, 24);
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // 1. Open sequence with CV
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open CV Settings tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);

    // 3. Check default conditions of checkboxes, uncheck them
    auto titleCheckBox = GTWidget::findCheckBox(os, "titleCheckBox");
    auto lengthCheckBox = GTWidget::findCheckBox(os, "lengthCheckBox");
    auto rulerLineCheckBox = GTWidget::findCheckBox(os, "rulerLineCheckBox");
    auto rulerCoordsCheckBox = GTWidget::findCheckBox(os, "rulerCoordsCheckBox");

    CHECK_SET_ERR(titleCheckBox->isChecked(), "Show/hide title checkBox is unchecked");
    CHECK_SET_ERR(lengthCheckBox->isChecked(), "Show/hide sequence length checkBox is unchecked");
    CHECK_SET_ERR(rulerLineCheckBox->isChecked(), "Show/hide ruler line checkBox is unchecked");
    CHECK_SET_ERR(rulerCoordsCheckBox->isChecked(), "Show/hide ruler coordinates checkBox is unchecked");

    GTCheckBox::setChecked(os, titleCheckBox, false);
    GTCheckBox::setChecked(os, lengthCheckBox, false);
    GTCheckBox::setChecked(os, rulerLineCheckBox, false);
    GTCheckBox::setChecked(os, rulerCoordsCheckBox, false);
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    // 1. Open sequence with CV
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open CV Settings tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::CircularView);
    GTUtilsOptionPanelSequenceView::toggleCircularView(os);

    // 3. Check font combobox and bold attribute button
    auto boldButton = GTWidget::findWidget(os, "boldButton");
    GTWidget::click(os, boldButton);
    CHECK_SET_ERR(qobject_cast<QPushButton*>(boldButton)->isChecked(), "Bold button is not checked");

    QFontComboBox* fontComboBox = qobject_cast<QFontComboBox*>(GTWidget::findWidget(os, "fontComboBox"));
    CHECK_SET_ERR(fontComboBox != nullptr, "Font comboBox is NULL");
#ifdef Q_OS_LINUX
    GTComboBox::selectItemByText(os, fontComboBox, "Serif");
#else
    GTComboBox::selectItemByText(os, fontComboBox, "Verdana");
#endif
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    // 1. Open linear nucl sequence
    // 2. Open "Circular View Settings" tab
    // 3. Check the hint: it is visible
    // 4. Push "Show CVs" button
    // 5. Check the hint: it is hidden, the settings are visible
    // 6. Hide CV, using tool bar
    // 7. The hint is visible again

    ADVSingleSequenceWidget* seqWidget = GTUtilsProject::openFileExpectSequence(os,
                                                                                dataDir + "samples/Genbank",
                                                                                "sars.gb",
                                                                                "NC_004718");
    GTWidget::click(os, GTWidget::findWidget(os, "OP_CV_SETTINGS"));

    auto openCvWidget = GTWidget::findWidget(os, "openCvWidget");
    CHECK_SET_ERR(openCvWidget->isVisible(), "Hint label and OpenCV button should be visible");

    GTWidget::click(os, GTWidget::findWidget(os, "openCvButton"));
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget), "No CV opened");
    CHECK_SET_ERR(openCvWidget->isHidden(), "Hint label and OpenCV button should be hidden");

    GTUtilsCv::cvBtn::click(os, seqWidget);
    CHECK_SET_ERR(openCvWidget->isVisible(), "Hint label and OpenCV button should be visible");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    // 1. Open linear nucl sequence
    // 2. Open "Circular View Settings" tab
    // 3. Check the hint: it is visible
    // 4. Open another sequence (circular)
    // 5. Open "Circular View Settings" tab
    // 6. Check the hint: it is hidden
    // 7. Return to the first question
    // 8. The hint is visible, the settings are hidden

    ADVSingleSequenceWidget* seqWidget1 = GTUtilsProject::openFileExpectSequence(os,
                                                                                 dataDir + "samples/Genbank",
                                                                                 "sars.gb",
                                                                                 "NC_004718");
    CHECK_SET_ERR(!GTUtilsCv::isCvPresent(os, seqWidget1), "CV opened");
    GTWidget::click(os, GTWidget::findWidget(os, "OP_CV_SETTINGS"));

    auto openCvWidget1 = GTWidget::findWidget(os, "openCvWidget");
    CHECK_SET_ERR(openCvWidget1->isVisible(), "Hint label and OpenCV button should be visible");

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "NC_014267.1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QList<ADVSingleSequenceWidget*> seqWidgets = GTUtilsMdi::activeWindow(os)->findChildren<ADVSingleSequenceWidget*>();
    CHECK_SET_ERR(seqWidgets.size() == 1, "Wrong number of sequences");
    ADVSingleSequenceWidget* seqWidget2 = seqWidgets.first();
    CHECK_SET_ERR(GTUtilsCv::isCvPresent(os, seqWidget2), "No CV opened");

    auto parent = GTWidget::findWidget(os, "NC_014267 [NC_014267.1.gb]");
    GTWidget::click(os, GTWidget::findWidget(os, "OP_CV_SETTINGS", parent));
    auto openCvWidget2 = GTWidget::findWidget(os, "openCvWidget", parent);
    CHECK_SET_ERR(openCvWidget2->isHidden(), "Hint label and OpenCV button should be hidden");
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    // Check Options panel -> Information tab -> Codons.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Activate Information tab on Options panel at the right edge of UGENE window. Expand "Codons"
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    auto reportPanel = GTWidget::findWidget(os, "options_panel_codons_widget");
    GTWidget::click(os, reportPanel);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Whole sequence.
    QLabel* reportLabel = GTWidget::findWidgetByType<QLabel*>(os, reportPanel, "Failed to find label inside codons panel");
    QString text = reportLabel->text();
    CHECK_SET_ERR(text.contains("whole sequence"), "Expected to see 'whole sequence' in the report");
    CHECK_SET_ERR(text.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>16 558 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (AAA:16558)");

    // Selected regions (same as whole sequence).
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 199950);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>5 501 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (AAA:5501)");

    // Selected regions: 1 codon on direct and 1 on complement frames.
    GTUtilsSequenceView::selectSequenceRegion(os, 19, 21);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("<td><b>CAA:&nbsp;&nbsp;</b></td><td>1 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (CAA:1)");
    CHECK_SET_ERR(text.contains("<td><b>TTG:&nbsp;&nbsp;</b></td><td>1 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (TTG:1)");

    GTUtilsSequenceView::selectSequenceRegion(os, 19, 20);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("Selection is too small"), "Expected to see 'Selection is too small' in the report");

    // Annotation.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "test-group", "test-feature", "1..199950");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected annotation"), "Expected to see 'selected annotation' in the report");
    CHECK_SET_ERR(text.contains("<td><b>AAA:&nbsp;&nbsp;</b></td><td>2 929 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (AAA:2929)");

    // Remove the annotation -> report is reset to the last selected region: 19, 20 that is 'too small'.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "MyDocument.gb"));
    GTMouseDriver::click();
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report/2");
    CHECK_SET_ERR(text.contains("Selection is too small"), "Expected to see 'Selection is too small' in the report/2");
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    // Check Options panel -> Information tab -> Amino acids.
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Activate Information tab on Options panel at the right edge of UGENE window. Expand "Amino Acids".
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Statistics);

    auto reportPanel = GTWidget::findWidget(os, "options_panel_amino_acids_widget");
    GTWidget::click(os, reportPanel);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Whole sequence.
    QLabel* reportLabel = GTWidget::findWidgetByType<QLabel*>(os, reportPanel, "Failed to find label inside amino acids panel");
    QString text = reportLabel->text();
    CHECK_SET_ERR(text.contains("whole sequence"), "Expected to see 'whole sequence' in the report");
    CHECK_SET_ERR(text.contains("<td><b>L:&nbsp;&nbsp;</b></td><td>44 225 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (L:44225)");

    // Selected region (whole sequence range).
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 199950);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("<td><b>L:&nbsp;&nbsp;</b></td><td>14 476 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (L:14476)");

    // Selected regions: 1 amino acid on direct and 1 on complement frames.
    GTUtilsSequenceView::selectSequenceRegion(os, 19, 21);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("<td><b>L:&nbsp;&nbsp;</b></td><td>1 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (L:1)");
    CHECK_SET_ERR(text.contains("<td><b>Q:&nbsp;&nbsp;</b></td><td>1 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (Q:1)");

    GTUtilsSequenceView::selectSequenceRegion(os, 19, 20);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report");
    CHECK_SET_ERR(text.contains("Selection is too small"), "Expected to see 'Selection is too small' in the report");

    // Annotation.
    GTUtilsAnnotationsTreeView::createAnnotation(os, "test-group", "test-feature", "1..199950");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected annotation"), "Expected to see 'selected annotation' in the report");
    CHECK_SET_ERR(text.contains("<td><b>L:&nbsp;&nbsp;</b></td><td>6 975 &nbsp;&nbsp;</td>"),
                  "Amino acids report does not contain expected entry (L:6975)");

    // Remove the annotation -> report is reset to the last selected region: 19, 20 that is 'too small'.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "MyDocument.gb"));
    GTMouseDriver::click();
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    text = reportLabel->text();
    CHECK_SET_ERR(text.contains("selected region"), "Expected to see 'selected region' in the report/2");
    CHECK_SET_ERR(text.contains("Selection is too small"), "Expected to see 'Selection is too small' in the report/2");
}

}  // namespace GUITest_common_scenarios_options_panel
}  // namespace U2
