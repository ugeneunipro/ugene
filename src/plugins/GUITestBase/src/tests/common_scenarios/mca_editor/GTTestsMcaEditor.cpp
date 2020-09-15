
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

#include <GTUtilsMsaEditor.h>
#include <base_dialogs/FontDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>

#include <QApplication>
#include <QList>

#include <U2View/MaEditorNameList.h>

#include "GTTestsMcaEditor.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorReference.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMcaEditorStatusWidget.h"
#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelMca.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_mca_editor {
//using namespace HI

const QString namesOfRow[16] = {"SZYD_Cas9_CR50",
                                "SZYD_Cas9_CR51",
                                "SZYD_Cas9_CR52",
                                "SZYD_Cas9_CR53",
                                "SZYD_Cas9_CR54",
                                "SZYD_Cas9_CR55",
                                "SZYD_Cas9_CR56",
                                "SZYD_Cas9_CR60",
                                "SZYD_Cas9_CR61",
                                "SZYD_Cas9_CR62",
                                "SZYD_Cas9_CR63",
                                "SZYD_Cas9_CR64",
                                "SZYD_Cas9_CR65",
                                "SZYD_Cas9_CR66",
                                "SZYD_Cas9_5B70",
                                "SZYD_Cas9_5B71"};

GUI_TEST_CLASS_DEFINITION(test_0001) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            CHECK_SET_ERR(minReadIdentity == 80, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            CHECK_SET_ERR(quality == 30, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            CHECK_SET_ERR(!output.isEmpty(), QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                reads << ("sanger_" + num + ".ab1");
            }
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, testDir + "_common_data/sanger/", reads));
            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //Expected state :
    //"reference_sanger_reads_alignment.ugenedb" in the Project View with object :
    //-{'mc' Mapped reads} for multiple chromatogram alignment object in Project View
    GTUtilsProjectTreeView::openView(os);
    bool check = GTUtilsProjectTreeView::checkItem(os, "Mapped reads");
    CHECK_SET_ERR(check, "'Mapped reads' is not present in the project view");

    //    Expected state : Reference name is  "KM099231" at the editor left corner
    //    Expected state : 16 reads are present
    int rows = GTUtilsMcaEditor::getReadsCount(os);
    CHECK_SET_ERR(rows == 16, QString("Incorrect rows quantity, expected: 16, current: %1").arg(rows));
    //    16 reads with names "SZYD_Cas9_CR50"..."SZYD_Cas9_CR56", "SZYD_Cas9_CR60"..."SZYD_Cas9_CR66", "SZYD_Cas9_CR70" and "SZYD_Cas9_CR71"
    QList<QString> rowNames = GTUtilsMcaEditor::getReadsNames(os);
    for (const QString &rowName : rowNames) {
        bool isNameFound = false;
        for (int i = 0; i < 16; i++) {
            QString currentName = namesOfRow[i];
            if (rowName == currentName) {
                isNameFound = true;
                break;
            }
        }
        CHECK_SET_ERR(isNameFound, QString("Name %1 is missing").arg(rowName));
    }

    //5. Report with info
    GTUtilsNotifications::checkNotificationReportText(os, QStringList() << "Mapped reads (16)" << "Filtered by low similarity (4)");

    // No Еrrors in the Log
    QStringList errors = GTUtilsLog::getErrors(os, GTLogTracer("error"));
    CHECK_SET_ERR(errors.isEmpty(), QString("Some errors found"));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Uncheck "Add to project" option
            GTCheckBox::setChecked(os, "addToProjectCheckbox", false);

            //5. Push "..." in "Result aligment"
            GTLineEdit::setText(os, "outputLineEdit", "...", QApplication::activeModalWidget());

            //6. In "Select output file" dialog set file name "Sanger" in test directory without  extension.ugenedb
            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));

            //7.Push Save
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //Expected state : "Result alignment" is filled <path> / Sanger.ugenedb
            output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutputContains = output.contains("Sanger.ugenedb");
            CHECK_SET_ERR(checkOutputContains, QString("incorrect output line"));

            //8. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : No Еrrors in the Log
    //Be sure that file <path>/Sanger.ugenedb is created on the disk
    QStringList errors = GTUtilsLog::getErrors(os, GTLogTracer("error"));
    CHECK_SET_ERR(errors.isEmpty(), QString("Some errors found"));

    //9. Open <path> / "Sanger.ugenedb" in the project
    GTFileDialog::openFile(os, sandBoxDir + "Sanger.ugenedb");

    //    Expected state :
    //"reference_sanger_reads_alignment.ugenedb" in the Project View with object :
    //-{'mc' Aligned reads} for multiple chromatogram alignment object in Project View
    //    Expected state : Aligment editor has been opened
    GTUtilsProject::checkProject(os);
    GTUtilsProjectTreeView::openView(os);
    //Expected state : Reference name is  "KM099231" at the editor left corner

    //    Expected state : 16 reads are present
    int rows = GTUtilsMcaEditor::getReadsCount(os);
    CHECK_SET_ERR(rows == 16, QString("Incorrect rows quantity, expected: 16, current: %1").arg(rows));

    //16 reads with names "SZYD_Cas9_CR50"..."SZYD_Cas9_CR56", "SZYD_Cas9_CR60"..."SZYD_Cas9_CR66", "SZYD_Cas9_5B70" and "SZYD_Cas9_5B71"
    QList<QString> rowNames = GTUtilsMcaEditor::getReadsNames(os);
    foreach (QString rowName, rowNames) {
        bool checkCurrentName = false;
        for (int i = 0; i < 16; i++) {
            QString currentName = namesOfRow[i];
            if (rowName == currentName) {
                checkCurrentName = true;
                break;
            }
        }
        CHECK_SET_ERR(checkCurrentName, QString("Name %1 is missing").arg(rowName));
    }

    //No Еrrors in the Log
    errors = GTUtilsLog::getErrors(os, GTLogTracer("error"));
    CHECK_SET_ERR(errors.isEmpty(), QString("Some errors found"));
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTLogTracer logTracer;

    //    1. Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open "Trim and аlign Sanger reads" sample

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    Expected state: "Trim and Align Sanger Reads" dialog has appered
            QWidget *wizard = GTWidget::getActiveModalWidget(os);
            const QString expectedTitle = "Map Sanger Reads to Reference";
            const QString actualTitle = wizard->windowTitle();
            CHECK_SET_ERR(expectedTitle == actualTitle, QString("Wizard title is incorrect: expected '%1', got '%2'").arg(expectedTitle).arg(actualTitle));

            GTWidget::click(os, wizard);

            //    3. Select Reference .../test/general/_common_data/sanger/reference.gb
            GTUtilsWizard::setParameter(os, "Reference", testDir + "_common_data/sanger/reference.gb");

            //    4. Push Next
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    5. On page "Input Sanger reads" add: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files) and click "Next" button
            QStringList readsList;
            for (int i = 1; i <= 20; i++) {
                readsList << testDir + QString("_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsList);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    6. Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    7. Push Run on Results page where "alignment.ugenedb" is result database by default
            const QString expectedResultFileName = "alignment.ugenedb";
            QString actualResultFileName = GTUtilsWizard::getParameter(os, "Mapped reads file").toString();
            CHECK_SET_ERR(expectedResultFileName == actualResultFileName, QString("An incorrect result file name: expected '%1', got '%2'").arg(expectedResultFileName).arg(actualResultFileName));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario()));

    GTUtilsWorkflowDesigner::addSample(os, "Trim and Map Sanger reads");

    //    Expected state: The workflow task has been finished successfully; "alignment.ugenedb" has created
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles(os);
    CHECK_SET_ERR(1 == outputFiles.size(), QString("Too many output files, exptected 1, got %1").arg(outputFiles.size()));

    const QString expectedResultFileName = "alignment.ugenedb";
    const QString actualResultFileName = outputFiles.first();
    CHECK_SET_ERR(expectedResultFileName == actualResultFileName, QString("An incorrect output file name, exptected '%1', got '%2'").arg(expectedResultFileName).arg(actualResultFileName));

    //    8. Click to "alignment.ugenedb" on the dashboard.
    GTUtilsDashboard::clickOutputFile(os, actualResultFileName);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    Expected state:
    //        "alignment.ugenedb" in the Project View with object:
    //        -{[mc] Aligned reads} for multiple chromatogram alignment object in Project View
    QMap<QString, QStringList> documents = GTUtilsProjectTreeView::getDocuments(os);
    CHECK_SET_ERR(1 == documents.count(), QString("An incorrect documents count: expected 1, got %1").arg(documents.count()));

    const QString actualDocumentName = documents.keys().first();
    CHECK_SET_ERR(expectedResultFileName == actualDocumentName, QString("An unexpected document name: expected '%1', got '%2'").arg(expectedResultFileName).arg(actualDocumentName));

    CHECK_SET_ERR(1 == documents.first().count(), QString("An incorrect objects count in '%1' document: expected 1, got %2").arg(documents.keys().first()).arg(documents.first().count()));

    const QString expectedObjectName = "[mc] Mapped reads";
    const QString actualObjectName = documents.first().first();
    CHECK_SET_ERR(expectedObjectName == actualObjectName, QString("An inexpected object name: expected '%1', got '%2'").arg(expectedObjectName).arg(actualObjectName));

    //    Expected state: Aligment editor has been opened
    GTUtilsMcaEditor::getEditorUi(os);

    //    Expected state: Reference name is  "Reference KM099231:" at the editor left corner
    const QString expectedReferenceLabelText = "Reference KM099231:";
    const QString actualReferenceLabelText = GTUtilsMcaEditor::getReferenceLabelText(os);
    CHECK_SET_ERR(expectedReferenceLabelText == actualReferenceLabelText, QString("An unexpected reference label text: expected '%1', got '%2'").arg(expectedReferenceLabelText).arg(actualReferenceLabelText));

    //    Expected state: "Consensus" is placed under reference name
    // untestable

    //    Expected state: 16 reads are present
    const int readsCount = GTUtilsMcaEditor::getReadsCount(os);
    CHECK_SET_ERR(16 == readsCount, QString("Unexpected count of reads: expected 16, got %1").arg(readsCount));

    //                    8 reads with names "SZYD_Cas9_CR60"..."SZYD_Cas9_CR66" and "SZYD_Cas9_5B71"
    const QSet<QString> expectedDirectReadsNames = QSet<QString>() << "SZYD_Cas9_5B71"
                                                                   << "SZYD_Cas9_CR60"
                                                                   << "SZYD_Cas9_CR61"
                                                                   << "SZYD_Cas9_CR62"
                                                                   << "SZYD_Cas9_CR63"
                                                                   << "SZYD_Cas9_CR64"
                                                                   << "SZYD_Cas9_CR65"
                                                                   << "SZYD_Cas9_CR66";
    const QStringList actualDirectReadsNames = GTUtilsMcaEditor::getDirectReadsNames(os);
    CHECK_SET_ERR(expectedDirectReadsNames == actualDirectReadsNames.toSet(), "Direct reads names are incorrect");

    //                    8 reverse reads with names "SZYD_Cas9_CR50"... "SZYD_Cas9_CR56" and "SZYD_Cas9_5B70"
    const QSet<QString> expectedReverseComplementReadsNames = QSet<QString>() << "SZYD_Cas9_CR50"
                                                                              << "SZYD_Cas9_CR51"
                                                                              << "SZYD_Cas9_CR52"
                                                                              << "SZYD_Cas9_CR53"
                                                                              << "SZYD_Cas9_CR54"
                                                                              << "SZYD_Cas9_CR55"
                                                                              << "SZYD_Cas9_CR56"
                                                                              << "SZYD_Cas9_5B70";
    const QStringList actualReverseComplementReadsNames = GTUtilsMcaEditor::getReverseComplementReadsNames(os);
    CHECK_SET_ERR(expectedReverseComplementReadsNames == actualReverseComplementReadsNames.toSet(), "Reverse complement reads names are incorrect");

    //                    No Еrrors in the Log
    GTUtilsLog::check(os, logTracer);

    //    9. Close active view
    GTUtilsMdi::closeActiveWindow(os);

    //    10. Select "Open view" from context menu and select "Open new view: "Sanger Reads Editor" from context view
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Open view"
                                                                              << "Open new view: Sanger Reads Editor",
                                                            GTGlobals::UseMouse));
    GTUtilsProjectTreeView::callContextMenu(os, "Mapped reads");

    //    Expected state:  Chromatogram sanger view is opened
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditor::getEditorUi(os);
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    1. Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open "Trim and аlign Sanger reads" sample

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    Expected state: "Trim and Align Sanger Reads" dialog has appered
            QWidget *wizard = GTWidget::getActiveModalWidget(os);
            const QString expectedTitle = "Map Sanger Reads to Reference";
            const QString actualTitle = wizard->windowTitle();
            CHECK_SET_ERR(expectedTitle == actualTitle, QString("Wizard title is incorrect: expected '%1', got '%2'").arg(expectedTitle).arg(actualTitle));

            GTWidget::click(os, wizard);

            //    3. Select Reference .../test/general/_common_data/alphabets/extended_amino_1000.fa
            GTUtilsWizard::setParameter(os, "Reference", testDir + "_common_data/alphabets/extended_amino_1000.fa");

            //    4. Push Next
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    5. On page "Input Sanger reads" add: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files) and click "Next" button
            QStringList readsList;
            for (int i = 1; i <= 20; i++) {
                readsList << testDir + QString("_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsList);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    6. Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    7. Push Run on Results page where "alignment.ugenedb" is result database by default
            const QString expectedResultFileName = "alignment.ugenedb";
            QString actualResultFileName = GTUtilsWizard::getParameter(os, "Mapped reads file").toString();
            CHECK_SET_ERR(expectedResultFileName == actualResultFileName, QString("An incorrect result file name: expected '%1', got '%2'").arg(expectedResultFileName).arg(actualResultFileName));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and Map Sanger reads");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Error:The input reference sequence 'seq6' contains characters that don't belong to DNA alphabet.
    GTUtilsLog::checkContainsError(os, trace, QString("The input reference sequence 'seq6' contains characters that don't belong to DNA alphabet."));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  /test/general/_common_data/alphabets/extended_amino_1000.fa
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/alphabets/extended_amino_1000.fa");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Uncheck "Add to project" option
            GTCheckBox::setChecked(os, "addToProjectCheckbox", false);

            //5. Push "..." in "Result aligment"
            GTLineEdit::setText(os, "outputLineEdit", "...", QApplication::activeModalWidget());

            //6. In "Select output file" dialog set file name "Sanger" in test directory without  extension.ugenedb
            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));

            //7.Push Save
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //Expected state : "Result alignment" is filled <path> / Sanger.ugenedb
            output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutputContains = output.contains("Sanger.ugenedb");
            CHECK_SET_ERR(checkOutputContains, QString("incorrect output line"));

            //8. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Error: The input reference sequence 'seq3' contains characters that don't belong to DNA alphabet.
    GTUtilsLog::checkContainsError(os, trace, QString("Task {Map Sanger reads to reference} finished with error: The input reference sequence 'seq6' contains characters that don't belong to DNA alphabet."));
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  /test/general/_common_data/alphabets/standard_dna_rna_amino_1000.fa
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/alphabets/standard_dna_rna_amino_1000.fa");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Uncheck "Add to project" option
            GTCheckBox::setChecked(os, "addToProjectCheckbox", false);

            //5. Push "..." in "Result aligment"
            GTLineEdit::setText(os, "outputLineEdit", "...", QApplication::activeModalWidget());

            //6. In "Select output file" dialog set file name "Sanger" in test directory without  extension.ugenedb
            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));

            //7.Push Save
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //Expected state : "Result alignment" is filled <path> / Sanger.ugenedb
            output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutputContains = output.contains("Sanger.ugenedb");
            CHECK_SET_ERR(checkOutputContains, QString("incorrect output line"));

            //8. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Error: More than one sequence in the reference file:  <path>/alphabets/standard_dna_rna_amino_1000.fa
    GTUtilsLog::checkContainsError(os, trace, QString("Task {Map Sanger reads to reference} finished with error: More than one sequence in the reference file:"));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    1. Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open "Trim and аlign Sanger reads" sample

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    Expected state: "Trim and Align Sanger Reads" dialog has appered
            QWidget *wizard = GTWidget::getActiveModalWidget(os);
            const QString expectedTitle = "Map Sanger Reads to Reference";
            const QString actualTitle = wizard->windowTitle();
            CHECK_SET_ERR(expectedTitle == actualTitle, QString("Wizard title is incorrect: expected '%1', got '%2'").arg(expectedTitle).arg(actualTitle));

            GTWidget::click(os, wizard);

            //    3. Select Reference .../test/general/_common_data/alphabets/standard_dna_rna_amino_1000.fa
            GTUtilsWizard::setParameter(os, "Reference", testDir + "_common_data/alphabets/standard_dna_rna_amino_1000.fa");

            //    4. Push Next
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    5. On page "Input Sanger reads" add: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files) and click "Next" button
            QStringList readsList;
            for (int i = 1; i <= 20; i++) {
                readsList << testDir + QString("_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsList);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    6. Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    7. Push Run on Results page where "alignment.ugenedb" is result database by default
            const QString expectedResultFileName = "alignment.ugenedb";
            QString actualResultFileName = GTUtilsWizard::getParameter(os, "Mapped reads file").toString();
            CHECK_SET_ERR(expectedResultFileName == actualResultFileName, QString("An incorrect result file name: expected '%1', got '%2'").arg(expectedResultFileName).arg(actualResultFileName));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and Map Sanger reads");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Error: More than one sequence in the reference file:  <path>/alphabets/standard_dna_rna_amino_1000.fa
    GTUtilsLog::checkContainsError(os, trace, QString("More than one sequence in the reference file:"));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  /test/general/_common_data/alphabets/standard_dna_rna_1000.fa
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/alphabets/standard_rna_1000.fa");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Uncheck "Add to project" option
            GTCheckBox::setChecked(os, "addToProjectCheckbox", false);

            //5. Push "..." in "Result aligment"
            GTLineEdit::setText(os, "outputLineEdit", "...", QApplication::activeModalWidget());

            //6. In "Select output file" dialog set file name "Sanger" in test directory without  extension.ugenedb
            GTUtilsTaskTreeView::waitTaskFinished(os);
            QStringList path;
            path << sandBoxDir + "Sanger";
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, path));

            //7.Push Save
            GTWidget::click(os, GTWidget::findExactWidget<QToolButton *>(os, "setOutputButton"));

            //Expected state : "Result alignment" is filled <path> / Sanger.ugenedb
            output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutputContains = output.contains("Sanger.ugenedb");
            CHECK_SET_ERR(checkOutputContains, QString("incorrect output line"));

            //8. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: Error: The input reference sequence 'seq3' contains characters that don't belong to DNA alphabet.
    GTUtilsLog::checkContainsError(os, trace, QString("Task {Map Sanger reads to reference} finished with error: The input reference sequence 'seq3' contains characters that don't belong to DNA alphabet."));
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //    1. Select "Tools>Workflow Designer"
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open "Trim and аlign Sanger reads" sample

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            //    Expected state: "Trim and Align Sanger Reads" dialog has appered
            QWidget *wizard = GTWidget::getActiveModalWidget(os);
            const QString expectedTitle = "Map Sanger Reads to Reference";
            const QString actualTitle = wizard->windowTitle();
            CHECK_SET_ERR(expectedTitle == actualTitle, QString("Wizard title is incorrect: expected '%1', got '%2'").arg(expectedTitle).arg(actualTitle));

            GTWidget::click(os, wizard);

            //    3. Select Reference .../test/general/_common_data/alphabets/standard_rna_1000.fa
            GTUtilsWizard::setParameter(os, "Reference", testDir + "_common_data/alphabets/standard_rna_1000.fa");

            //    4. Push Next
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    5. On page "Input Sanger reads" add: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files) and click "Next" button
            QStringList readsList;
            for (int i = 1; i <= 20; i++) {
                readsList << testDir + QString("_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsList);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    6. Push Next on "Trim and Filtering" page
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            //    7. Push Run on Results page where "alignment.ugenedb" is result database by default
            const QString expectedResultFileName = "alignment.ugenedb";
            QString actualResultFileName = GTUtilsWizard::getParameter(os, "Mapped reads file").toString();
            CHECK_SET_ERR(expectedResultFileName == actualResultFileName, QString("An incorrect result file name: expected '%1', got '%2'").arg(expectedResultFileName).arg(actualResultFileName));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTLogTracer trace;
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and Map Sanger reads");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Error: The input reference sequence 'seq3' contains characters that don't belong to DNA alphabet.
    GTUtilsLog::checkContainsError(os, trace, QString("The input reference sequence 'seq3' contains characters that don't belong to DNA alphabet."));
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            //Expected state : "Min read identity" option by default = 80 %
            int minReadIdentity = GTSpinBox::getValue(os, "minIdentitySpinBox");
            QString expected = "80";
            CHECK_SET_ERR(QString::number(minReadIdentity) == expected, QString("incorrect Read Identity value: expected 80%, got %1").arg(minReadIdentity));

            //Expected state : "Quality threshold" option by default = 30
            int quality = GTSpinBox::getValue(os, "qualitySpinBox");
            expected = "30";
            CHECK_SET_ERR(QString::number(quality) == expected, QString("incorrect quality value: expected 30, got %1").arg(quality));

            //Expected state : "Add to project" option is checked by default
            bool addToProject = GTCheckBox::getState(os, "addToProjectCheckbox");
            CHECK_SET_ERR(addToProject, QString("incorrect addToProject state: expected true, got false"));

            //Expected state : "Result aligment" field is filled by default
            QString output = GTLineEdit::getText(os, "outputLineEdit");
            bool checkOutput = output.isEmpty();
            CHECK_SET_ERR(!checkOutput, QString("incorrect output line: is empty"));

            //Expected state : "Result alignment" is pre - filled <path> / Documents / UGENE_Data / reference_sanger_reads_alignment.ugenedb]
            bool checkContainsFirst = output.contains(".ugenedb", Qt::CaseInsensitive);
            bool checkContainsSecond = output.contains("sanger_reads_alignment");
            bool checkContainsThird = output.contains("UGENE_Data");
            bool checkContainsFourth = output.contains("Documents");
            bool checkContains = checkContainsFirst && checkContainsSecond && checkContainsThird && checkContainsFourth;
            CHECK_SET_ERR(checkContains, QString("incorrect output line: do not contain default path"));

            //2. Select reference  .../test/general/_common_data/sanger/reference.gb
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "referenceLineEdit"), testDir + "_common_data/sanger/reference.gb");

            //3. Select Reads: .../test/general/_common_data/sanger/sanger_01.ab1-/sanger_20.ab1(20 files)]
            QStringList reads;
            for (int i = 1; i < 21; i++) {
                QString name = "sanger_";
                QString num = QString::number(i);
                if (num.size() == 1) {
                    num = "0" + QString::number(i);
                }
                name += num;
                name += ".ab1";
                reads << name;
            }
            QString readDir = testDir + "_common_data/sanger/";
            GTUtilsTaskTreeView::waitTaskFinished(os);
            GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, readDir, reads);
            GTUtilsDialog::waitForDialog(os, ob);

            GTWidget::click(os, GTWidget::findExactWidget<QPushButton *>(os, "addReadButton"));

            //4. Push "Align" button
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };

    //1. Select "Tools>Sanger data analysis>Reads quality control and alignment"
    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "Sanger data analysis"
                                                << "Map reads to reference...");
    GTUtilsDialog::waitAllFinished(os);

    //5. Select "SZYD_Cas9_5B71" read
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");

    //6. Select position 2120
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));

    //Expected state : aligned read sequence with chromatogram, black triangle, directed down and right before read name
    bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_5B71");
    CHECK_SET_ERR(isChromatogramShown, "Error: aligned read sequence should be displayed with a chromatogram")

    //7. Push black triangle
    GTUtilsMcaEditorSequenceArea::clickCollapseTriangle(os, "SZYD_Cas9_5B71", true);

    //Expected state : now it is white triangle, directed to the right
    //Aligned read sequence has been displayed without chromatogram
    isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_5B71");
    CHECK_SET_ERR(!isChromatogramShown, "Error: aligned read sequence should be displayed without chromatogram")

    //8. Push white triangle
    GTUtilsMcaEditorSequenceArea::clickCollapseTriangle(os, "SZYD_Cas9_5B71", false);

    //Expected state : now it is black triangle again
    //Aligned read sequence has been displayed with chromatogram
    isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, "SZYD_Cas9_5B71");
    CHECK_SET_ERR(isChromatogramShown, "Error: aligned read sequence should be displayed with chromatogram")
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //Expected state : "Show/Hide Chromatograms" button is in pressed state
    //All reads are expanded, chromatograms if any are displayed for each reads
    QList<QString> rows = GTUtilsMcaEditor::getReadsNames(os);
    foreach (QString row, rows) {
        bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, row);
        CHECK_SET_ERR(isChromatogramShown, QString("Error: read %1 should be displayed with chromatogram").arg(row));
    }

    //2. Push "Show/Hide Chromatograms" button in the main menu
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "chromatograms"));

    //Expected state : "Show/Hide Chromatograms" button is in normal state
    //All reads are collapsed
    foreach (QString row, rows) {
        bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, row);
        CHECK_SET_ERR(!isChromatogramShown, QString("Error: read %1 should be displayed without chromatogram").arg(row));
    }

    //3. Push "Show/Hide Chromatograms" button again
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "chromatograms"));

    //All reads are expanded, chromatograms if any are displayed for each reads
    //Expected state : All reads are expanded, chromatograms(if any) are displayed for each reads
    foreach (QString row, rows) {
        bool isChromatogramShown = GTUtilsMcaEditorSequenceArea::isChromatogramShown(os, row);
        CHECK_SET_ERR(isChromatogramShown, QString("Error: read %1 should be displayed with chromatogram").arg(row));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0012_1) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select "SZYD_Cas9_5B71" read
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");

    //3. Select position 2120
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));

    //4. Push "Show/Hide Trace"combo from context menu
    //Expected state : Combo with selected "A", "C", "G", "T" items and "Show all" item appeared
    QList<QStringList> paths;
    QList<QStringList> onlyLetterPaths;
    paths.append(QStringList() << "Appearance"
                               << "Show/hide trace"
                               << "A");
    onlyLetterPaths.append(QStringList() << "Appearance"
                                         << "Show/hide trace"
                                         << "A");
    paths.append(QStringList() << "Appearance"
                               << "Show/hide trace"
                               << "C");
    onlyLetterPaths.append(QStringList() << "Appearance"
                                         << "Show/hide trace"
                                         << "C");
    paths.append(QStringList() << "Appearance"
                               << "Show/hide trace"
                               << "G");
    onlyLetterPaths.append(QStringList() << "Appearance"
                                         << "Show/hide trace"
                                         << "G");
    paths.append(QStringList() << "Appearance"
                               << "Show/hide trace"
                               << "T");
    onlyLetterPaths.append(QStringList() << "Appearance"
                                         << "Show/hide trace"
                                         << "T");
    paths.append(QStringList() << "Appearance"
                               << "Show/hide trace"
                               << "Show all");
    //GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, paths, PopupChecker::CheckOptions(PopupChecker::Exists)));
    QStringList menuPath;
    menuPath << "Appearance"
             << "Show/hide trace";
    QStringList itemsNames;
    itemsNames << "A"
               << "C"
               << "G"
               << "T"
               << "Show all";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, itemsNames, PopupChecker::CheckOptions(PopupChecker::Exists)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Select "A"
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show/hide trace"
                                                                              << "A"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : Green trace vanished from all chromatograms

    //6. Select "C"
    //Expected state : "A" is unchecked
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    QStringList intermediateCheck;
    intermediateCheck << "A";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, intermediateCheck, PopupChecker::CheckOptions(PopupChecker::IsUnchecked)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show/hide trace"
                                                                              << "C"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : Blue trace vanished from all chromatograms

    //7. Select "G"
    //Expected state : "A" and "C"  are unchecked
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    intermediateCheck << "C";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, intermediateCheck, PopupChecker::CheckOptions(PopupChecker::IsUnchecked)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show/hide trace"
                                                                              << "G"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : Grey trace vanished from all chromatograms

    //8. Select "T"
    //Expected state : "A", "C", "G"  are unchecked
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    intermediateCheck << "G";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, intermediateCheck, PopupChecker::CheckOptions(PopupChecker::IsUnchecked)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show/hide trace"
                                                                              << "T"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : Red trace vanished from all chromatograms; there are no any traces on the screen

    //9. Select "All"
    //Expected state : All four traces are restored for all chromatograms
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    intermediateCheck << "T";
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, intermediateCheck, PopupChecker::CheckOptions(PopupChecker::IsUnchecked)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show/hide trace"
                                                                              << "Show all"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //10. Expand combo once more
    //Expected state : All four letters are checked
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, menuPath, intermediateCheck, PopupChecker::CheckOptions(PopupChecker::IsChecked)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
}

GUI_TEST_CLASS_DEFINITION(test_0012_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select "SZYD_Cas9_5B71" read
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");

    //3. Select position 2120
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));

    //4. Push "Show/Hide Trace"combo from main menu
    //Expected state : Combo with selected "A", "C", "G", "T" items and "Show all" item appeared
    const QStringList menuPath = QStringList() << "Actions"
                                               << "Appearance"
                                               << "Show/hide trace";
    const QStringList itemsNames = QStringList() << "A"
                                                 << "C"
                                                 << "G"
                                                 << "T"
                                                 << "Show all";
    GTMenu::checkMainMenuItemsState(os, menuPath, itemsNames, PopupChecker::CheckOption(PopupChecker::Exists));

    //5. Select "A"
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show/hide trace"
                                                << "A");
    ;

    //Expected state : Green trace vanished from all chromatograms

    //6. Select "C"
    //Expected state : "A" is unchecked
    QStringList intermediateCheck = QStringList() << "A";
    GTMenu::checkMainMenuItemsState(os, menuPath, intermediateCheck, PopupChecker::CheckOption(PopupChecker::IsUnchecked));

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show/hide trace"
                                                << "C");
    ;

    //Expected state : Blue trace vanished from all chromatograms

    //7. Select "G"
    //Expected state : "A" and "C"  are unchecked
    intermediateCheck << "C";
    GTMenu::checkMainMenuItemsState(os, menuPath, intermediateCheck, PopupChecker::CheckOption(PopupChecker::IsUnchecked));

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show/hide trace"
                                                << "G");
    ;

    //Expected state : Grey trace vanished from all chromatograms

    //8. Select "T"
    //Expected state : "A", "C", "G"  are unchecked
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    intermediateCheck << "G";
    GTMenu::checkMainMenuItemsState(os, menuPath, intermediateCheck, PopupChecker::CheckOption(PopupChecker::IsUnchecked));

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show/hide trace"
                                                << "T");
    ;

    //Expected state : Red trace vanished from all chromatograms; there are no any traces on the screen

    //9. Select "All"
    //Expected state : All four traces are restored for all chromatograms
    intermediateCheck << "T";
    GTMenu::checkMainMenuItemsState(os, menuPath, intermediateCheck, PopupChecker::CheckOption(PopupChecker::IsUnchecked));

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show/hide trace"
                                                << "Show all");
    ;

    //10. Expand combo once more
    //Expected state : All four letters are checked
    GTMenu::checkMainMenuItemsState(os, menuPath, intermediateCheck, PopupChecker::CheckOption(PopupChecker::IsChecked));

    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
}

GUI_TEST_CLASS_DEFINITION(test_0013_1) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select "SZYD_Cas9_CR50" read
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR50");

    //3. Push Remove seuence(s) button on main menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove read"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : the read is deleted
    QStringList reads = GTUtilsMcaEditor::getReadsNames(os);
    bool isReadWasDelete = true;
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_CR50") {
            isReadWasDelete = false;
        }
    }
    CHECK_SET_ERR(isReadWasDelete, "Error: read SZYD_Cas9_CR50 was not delete");

    //4. Select 3 reads using Shift modifier
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B70");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR51");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    //5 Push Remove seuence(s) button
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove read"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : 3 reads are deleted
    //Expected state : No corresponding reads in the map
    reads = GTUtilsMcaEditor::getReadsNames(os);
    isReadWasDelete = true;
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_5B70" || read == "SZYD_Cas9_5B71" || read == "SZYD_Cas9_CR51") {
            isReadWasDelete = false;
        }
    }
    CHECK_SET_ERR(isReadWasDelete, "Error: read SZYD_Cas9_CR50 was not delete");

    //6. Push undo
    GTUtilsMcaEditor::undo(os);

    //Expected state : 3 reads are restored in the same place
    //Expected state : Map is restored too
    reads = GTUtilsMcaEditor::getReadsNames(os);
    int restoreReadsCount = 0;
    QStringList removedReads;
    removedReads << "SZYD_Cas9_5B70"
                 << "SZYD_Cas9_5B71"
                 << "SZYD_Cas9_5B70";
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_5B70") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_5B70");
        }
        if (read == "SZYD_Cas9_5B71") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_5B71");
        }
        if (read == "SZYD_Cas9_CR51") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_CR51");
        }
    }
    CHECK_SET_ERR(restoreReadsCount == 3, QString("Error: read %1 was not restore").arg(removedReads.first()));
}

GUI_TEST_CLASS_DEFINITION(test_0013_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select "SZYD_Cas9_CR50" read
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR50");

    //3. Push Remove seuence(s) button on context menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove read");
    ;

    //Expected state : the read is deleted
    QStringList reads = GTUtilsMcaEditor::getReadsNames(os);
    bool isReadWasDelete = true;
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_CR50") {
            isReadWasDelete = false;
        }
    }
    CHECK_SET_ERR(isReadWasDelete, "Error: read SZYD_Cas9_CR50 was not delete");

    //4. Select 3 reads using Shift modifier
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B70");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_5B71");
    GTUtilsMcaEditor::clickReadName(os, "SZYD_Cas9_CR51");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    //5. Push Remove seuence(s) button
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove read");
    ;

    //Expected state : 3 reads are deleted
    //Expected state : No corresponding reads in the map
    reads = GTUtilsMcaEditor::getReadsNames(os);
    isReadWasDelete = true;
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_5B70" || read == "SZYD_Cas9_5B71" || read == "SZYD_Cas9_CR51") {
            isReadWasDelete = false;
        }
    }
    CHECK_SET_ERR(isReadWasDelete, "Error: read SZYD_Cas9_CR50 was not delete");

    //6. Push undo
    GTUtilsMcaEditor::undo(os);

    //Expected state : 3 reads are restored in the same place
    //Expected state : Map is restored too
    reads = GTUtilsMcaEditor::getReadsNames(os);
    int restoreReadsCount = 0;
    QStringList removedReads;
    removedReads << "SZYD_Cas9_5B70"
                 << "SZYD_Cas9_5B71"
                 << "SZYD_Cas9_5B70";
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_5B70") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_5B70");
        }
        if (read == "SZYD_Cas9_5B71") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_5B71");
        }
        if (read == "SZYD_Cas9_CR51") {
            restoreReadsCount++;
            removedReads.removeOne("SZYD_Cas9_CR51");
        }
    }
    CHECK_SET_ERR(restoreReadsCount == 3, QString("Error: read %1 was not restore").arg(removedReads.first()));
}

GUI_TEST_CLASS_DEFINITION(test_0013_3) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select any symbol  in the "SZYD_Cas9_CR50" read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(5500, 2));

    //6. Push Remove seuence(s) from main or context menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove read");

    //Expected state: the read is deleted
    QStringList reads = GTUtilsMcaEditor::getReadsNames(os);
    bool isReadWasDelete = true;
    foreach (QString read, reads) {
        if (read == "SZYD_Cas9_CR50") {
            isReadWasDelete = false;
        }
    }
    CHECK_SET_ERR(isReadWasDelete, "Error: read SZYD_Cas9_CR50 was not delete");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    QString filePath = testDir + "_common_data/sanger/alignment_mca_0014.ugenedb";
    QString fileName = "alignment_mca_0014.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    QScrollBar *hscroll = GTWidget::findExactWidget<QScrollBar *>(os, "horizontal_names_scroll");
    bool isHidden = hscroll->isHidden();

    //2. Select vertical slider between reads named and read area
    //3. Move the slider to the left until part of the name becomes invisible
    while (isHidden) {
        GTUtilsMcaEditorSequenceArea::moveTheBorderBetweenAlignmentAndRead(os, -20);
        isHidden = hscroll->isHidden();
    }

    //Expected state : Horizontal scrolls bar appears
    CHECK_SET_ERR(!isHidden, "Horizontal scrolls bar not found");

    hscroll = GTWidget::findExactWidget<QScrollBar *>(os, "horizontal_names_scroll");
    isHidden = hscroll->isHidden();

    //4. Move the slider to the right until all names become visible
    while (!isHidden) {
        GTUtilsMcaEditorSequenceArea::moveTheBorderBetweenAlignmentAndRead(os, 20);
        isHidden = hscroll->isHidden();
    }

    //Expected state : Horizontal scrolls bar disappears
    CHECK_SET_ERR(isHidden, "Horizontal scrolls bar is found");
}

GUI_TEST_CLASS_DEFINITION(test_0015_1) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. In Option panelSelect consensuns mode = Strict
    GTUtilsOptionPanelMca::setConsensusType(os, "Strict");
    GTGlobals::sleep();

    //3. Push "Ctrl+Alt+v"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //Expected state : first difference between reference "T" and consensus "G"
    QString referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    QString consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'G', QString("Incorrect symbols, Expected ref = T, con = G, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //4. Push "Jump to next variation" button twice
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_mismatch"));
    GTGlobals::sleep();
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_mismatch"));

    //Expected state : difference between reference "T" and consensus "G"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'G', QString("Incorrect symbols, Expected ref = T, con = G, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //5. Push "Jump to next variation" from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to next variation"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : difference between reference "T" and consensus "C"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'C', QString("Incorrect symbols, Expected ref = T, con = C, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //6. Push "Jump to next variation" from main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to next variation");

    //Expected state : difference between reference "G" and consensus "A"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'G' && consensusChar[0] == 'A', QString("Incorrect symbols, Expected ref = G, con = G, current ref = %1, cons = %2").arg(referenceChar[0]).arg(consensusChar[0]));
}

GUI_TEST_CLASS_DEFINITION(test_0015_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_read_is_reference.ugenedb";
    QString fileName = "alignment_read_is_reference.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Push "Ctrl+Alt+v"
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('v', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsDialog::waitAllFinished(os);

    //3. Push "Jump to next variation" button
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_mismatch"));
    GTUtilsDialog::waitAllFinished(os);

    //4. Push "Jump to next variation" from context menu
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to next variation"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsDialog::waitAllFinished(os);

    //5. Push "Jump to next variation" from main menu
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to next variation");
    GTUtilsDialog::waitAllFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0016_1) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. In Option panelSelect consensuns mode = Strict
    GTUtilsOptionPanelMca::setConsensusType(os, "Strict");
    GTGlobals::sleep();

    //6. Push "Ctrl+Alt+Shift+v"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('v', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep(500);

    //Expected state : first difference between reference "T" and consensus GAP
    QString referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    QString consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == U2Mca::GAP_CHAR, QString("Incorrect symbols, Expected ref = T, con = GAP, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //3. Push "Jump to previous variation" button twice
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "prev_mismatch"));
    GTGlobals::sleep();
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "prev_mismatch"));

    //Expected state : difference between reference "C" and consensus GAP
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'C' && consensusChar[0] == U2Mca::GAP_CHAR, QString("Incorrect symbols, Expected ref = C, con = GAP, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //4. Push "Jump to previous variation" from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to previous variation"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : difference between reference "G" and consensus GAP
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'G' && consensusChar[0] == U2Mca::GAP_CHAR, QString("Incorrect symbols, Expected ref = G, con = GAP, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));

    //9. Push "Jump to next variation" from main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to previous variation");

    //Expected state : difference between reference "T" and consensus GAP
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == U2Mca::GAP_CHAR, QString("Incorrect symbols, Expected ref = T, con = GAP, current ref = %1, cons = %2").arg(referenceChar[0]).arg(referenceChar[0]));
}

GUI_TEST_CLASS_DEFINITION(test_0016_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_read_is_reference.ugenedb";
    QString fileName = "alignment_read_is_reference.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Push "Ctrl+Alt+Shift+v"
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('v', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Push "Jump to previous variation" button
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");

    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "prev_mismatch"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //3. Push "Jump to previous variation" from context menu
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to previous variation"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Push "Jump to previous variation" from main menu
    //Expected state : Notification "There are no variations in the consensus sequence" will be shown
    GTUtilsNotifications::waitForNotification(os, true, "There are no variations in the consensus sequence");

    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to previous variation");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0017_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

#ifndef Q_OS_LINUX
    //In linux, OS intercept this hotkey

    //2. Push "Ctrl+Alt+a"
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('a', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    GTUtilsTaskTreeView::waitTaskFinished(os);
#endif

    //3. Push "Jump to next variation" button
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_ambiguous"));

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Push "Jump to next variation" from context menu
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to next ambiguous character"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    //5. Push "Jump to next variation" from main menu
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to next ambiguous character");

    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0017_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_with_ambiguous.ugenedb";
    QString fileName = "alignment_with_ambiguous.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

#ifndef Q_OS_LINUX
    //In linux, OS intercept this hotkey

    //2. Push Ctrl + Alt + a
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('a', Qt::AltModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
#else
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_ambiguous"));
#endif
    GTGlobals::sleep();

    //Expected state : reference "C", consensus "N", read "N"
    QString referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    QString consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    QPoint readSelection(reg.startPos, 0);
    char readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'C' && consensusChar[0] == 'N' && readChar == 'N', QString("Incorrect symbols, Expected ref = C, con = N, read = N current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //3. Push "Jump to next ambiguous character" button twice
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_ambiguous"));
    GTGlobals::sleep();
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "next_ambiguous"));

    //Expected state : reference "C", consensus "M", read "M".
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'C' && consensusChar[0] == 'M' && readChar == 'M', QString("Incorrect symbols, Expected ref = C, con = M, read = M current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //4. Push "Jump to next ambiguous character" button from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to next ambiguous character"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state : reference "T", consensus "W", read "W"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'W' && readChar == 'W', QString("Incorrect symbols, Expected ref = T, con = W, read = W current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //5.Push "Jump to next ambiguous character" button from main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to next ambiguous character");

    //Expected state : reference "C", consensus "N", read "N"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'C' && consensusChar[0] == 'N' && readChar == 'N', QString("Incorrect symbols, Expected ref = C, con = N, read = N current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));
}

GUI_TEST_CLASS_DEFINITION(test_0018_1) {
    //    1. Open "_common_data/sanger/alignment.ugenedb".
    QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Push "Ctrl+Alt+Shift+a"
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('a', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    GTUtilsDialog::waitAllFinished(os);

    //3. Push "Jump to previous variation" button
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    QToolBar *toolbar = GTToolbar::getToolbar(os, "mwtoolbar_activemdi");
    QWidget *prevAmbiguousButton = GTToolbar::getWidgetForActionObjectName(os, toolbar, "prev_ambiguous");
    GTWidget::click(os, prevAmbiguousButton);
    GTUtilsDialog::waitAllFinished(os);

    //4. Push "Jump to next variation" from context menu
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to previous ambiguous character"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsDialog::waitAllFinished(os);

    //5. Push "Jump to previous variation" from main menu
    //Expected state : Notification "There are no ambiguous characters in the alignment.
    GTUtilsNotifications::waitForNotification(os, true, "There are no ambiguous characters in the alignment.");

    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to previous ambiguous character");

    GTUtilsDialog::waitAllFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0018_2) {
    QString filePath = testDir + "_common_data/sanger/alignment_with_ambiguous.ugenedb";
    QString fileName = "alignment_with_ambiguous.ugenedb";

    //1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Push "Ctrl+Alt+Shift+a"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('a', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTGlobals::sleep();

    //Expected state: reference "T", consensus "W", read "W"
    QString referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    QString consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    U2Region reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    QPoint readSelection(reg.startPos, 0);
    char readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'W' && readChar == 'W', QString("Incorrect symbols, Expected ref = T, con = W, read = W current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //3. Push "Jump to previous variation" button twice
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "prev_ambiguous"));
    GTGlobals::sleep();
    GTWidget::click(os, GTToolbar::getWidgetForActionObjectName(os, GTToolbar::getToolbar(os, "mwtoolbar_activemdi"), "prev_ambiguous"));

    //Expected state: reference "G", consensus "N", read "N"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'G' && consensusChar[0] == 'N' && readChar == 'N', QString("Incorrect symbols, Expected ref = G, con = N, read = N current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //4. Push "Jump to next variation" from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Navigation"
                                                                              << "Jump to previous ambiguous character"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: reference "C", consensus "N", read "N"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'C' && consensusChar[0] == 'N' && readChar == 'N', QString("Incorrect symbols, Expected ref = C, con = N, read = N current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));

    //5. Push "Jump to previous variation" from main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Navigation"
                                                << "Jump to previous ambiguous character");

    //Expected state: reference "T", consensus "W", read "W"
    referenceChar = GTUtilsMcaEditorSequenceArea::getSelectedReferenceReg(os);
    consensusChar = GTUtilsMcaEditorSequenceArea::getSelectedConsensusReg(os);
    reg = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    readSelection = QPoint(reg.startPos, 0);
    readChar = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, readSelection);
    CHECK_SET_ERR(referenceChar.size() == 1 && consensusChar.size() == 1, QString("Incorrect selection size, Expected ref = 1, cons = 1, Curren ref = %1, cons = %2").arg(QString::number(referenceChar.size())).arg(QString::number(consensusChar.size())));
    CHECK_SET_ERR(referenceChar[0] == 'T' && consensusChar[0] == 'W' && readChar == 'W', QString("Incorrect symbols, Expected ref = T, con = W, read = W current ref = %1, cons = %2, read = %3").arg(referenceChar[0]).arg(referenceChar[0]).arg(readChar));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select any read name by mouse
    QStringList visibleRows = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    QString firstVisibleRow = visibleRows.first();
    GTUtilsMcaEditor::moveToReadName(os, firstVisibleRow);

    //3. Using 'drag'n drop' move the name in the another place and release mouse button
    const QRect sequenceNameRect = GTUtilsMcaEditor::getReadNameRect(os, firstVisibleRow);
    GTUtilsMcaEditorSequenceArea::dragAndDrop(os, QPoint(sequenceNameRect.center().x(), sequenceNameRect.y() + (2 * sequenceNameRect.height())));

    //Expected state : The read is replaced in another place
    QStringList newOrederedVisibleRows = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    int size = visibleRows.size();
    bool isNewOrder = false;
    for (int i = 0; i < size; i++) {
        if (visibleRows[i] != newOrederedVisibleRows[i]) {
            isNewOrder = true;
            break;
        }
    }
    CHECK_SET_ERR(isNewOrder, "The order was not change");
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select any read
    QStringList visibleRows = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    QString firstVisibleRow = visibleRows.first();
    GTUtilsMcaEditor::clickReadName(os, firstVisibleRow);
    GTGlobals::sleep(500);

    //3. Push Esc
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(500);

    //Expected state : There is no selection
    U2Region reg = GTUtilsMcaEditorSequenceArea::getSelectedRowsNum(os);
    CHECK_SET_ERR(reg.length == 0, "Some reads are selected");

    //4. Select any region in the reference
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(os, 500);
    GTGlobals::sleep(500);

    //5. Push Esc
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(500);

    //Expected state : There is no selection
    U2Region sel = GTUtilsMcaEditorSequenceArea::getReferenceSelection(os);
    CHECK_SET_ERR(sel == U2Region(), "Some reference character is lselected");

    //6. Select any symbol in the read
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2120, 1));
    GTGlobals::sleep(1000);

    //7. Push Esc
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(500);

    //Expected state : There is no selection
    QRect selection = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(selection == QRect(), "Some character in alignent is lselected");
}

GUI_TEST_CLASS_DEFINITION(test_0022_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Press Shift + R keys on the keyboard.
    GTKeyboardDriver::keyClick('R', Qt::ShiftModifier);
    GTGlobals::sleep();

    //Expected state : the character is selected in the replacement mode(i.e.the border of the character are drawn using another color and / or bold).
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //4. Press a key on the keyboard with another character of the same alphabet (e.g C key).
    GTKeyboardDriver::keyClick('C');
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one (e.g 'A' was replaced with 'C').
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //5. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'A'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //6. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");
}

GUI_TEST_CLASS_DEFINITION(test_0022_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'C')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'C'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the context menu in the sequence area.
    //Expected state: the menu contains an item "Edit > Replace character/gap".The item is enabled.A hotkey Shift + R is shown nearby.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Edit"
                                                                              << "Replace character/gap",
                                                            PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));

    //4. Select the item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Replace character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //Expected state : the character is selected in the replacement mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //5. Press a key on the keyboard with another character of the same alphabet (e.g GAP key).
    GTKeyboardDriver::keyClick(U2Mca::GAP_CHAR);
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one (e.g 'C' was replaced with GAP).
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == U2Mca::GAP_CHAR, "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //6. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");

    //7 Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character GAP
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == U2Mca::GAP_CHAR, "Incorrect selected character");
}

GUI_TEST_CLASS_DEFINITION(test_0022_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'C')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'C'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', QString("Incorrect selected character, expected: C, current: %1").arg(selectedChar));

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the main menu in the sequence area.
    //Expected state: the menu contains an item "Actions > Edit > Replace character". The item is enabled. A hotkey Shift+R is shown nearby.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Edit",
                                    QStringList() << "Replace character/gap",
                                    PopupChecker::CheckOption(PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));

    //4. Select the item.
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Replace character/gap");

    //Expected state : the character is selected in the replacement mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //5. Press a key on the keyboard with another character of the same alphabet (e.g GAP key).
    GTKeyboardDriver::keyClick(U2Mca::GAP_CHAR);
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one (e.g 'C' was replaced with GAP).
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == U2Mca::GAP_CHAR, QString("Incorrect selected character, expected: GAP, current: %1").arg(selectedChar));

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //6. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', QString("Incorrect selected character, expected: C, current: %1").arg(selectedChar));

    //7. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character GAP
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
}

GUI_TEST_CLASS_DEFINITION(test_0023_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Press Shift + R keys on the keyboard.
    GTKeyboardDriver::keyClick('R', Qt::ShiftModifier);
    GTGlobals::sleep();

    //Expected state : the character is selected in the replacement mode(i.e.the border of the character are drawn using another color and / or bold).
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //4. Press wrong symbol "#"
    GTKeyboardDriver::keyClick('#');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0023_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'C')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'C'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep(500);

    //3. Open the context menu in the sequence area.
    //Expected state: the menu contains an item "Edit > Replace character/gap".The item is enabled.A hotkey Shift + R is shown nearby.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Edit"
                                                                              << "Replace character/gap",
                                                            PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    //GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));

    //4. Select the item.
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Replace character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(500);

    //Expected state : the character is selected in the replacement mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //5. Push wrong symbol "!"
    GTKeyboardDriver::keyClick('!');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0023_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'C')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'C'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'C', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the main menu in the sequence area.
    //Expected state: the menu contains an item "Actions > Edit > Replace character". The item is enabled. A hotkey Shift+R is shown nearby.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Edit",
                                    QStringList() << "Replace character/gap",
                                    PopupChecker::CheckOption(PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2116, 1));

    //4. Select the item.
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Replace character/gap");

    //Expected state : the character is selected in the replacement mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 1, "Incorrect modification state");

    //5. Push wrong symbol "%"
    GTKeyboardDriver::keyClick('%');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0024_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Press Shift + I keys on the keyboard.
    GTKeyboardDriver::keyClick('I', Qt::ShiftModifier);
    GTGlobals::sleep();

    //Expected state :  the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //4. Press a key on the keyboard with another character of the same alphabet (e.g N key).
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //Expected state: Gap column has been inserted in all reads for this coordinate;
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    int y = sel.y();
    int rowNums = GTUtilsMcaEditorSequenceArea::getNameList(os).size();
    bool isGapColoumn = true;
    for (int i = 0; i < rowNums; i++) {
        if (i == y) {
            continue;
        }
        char ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, QPoint(x, i));
        if (ch != U2Mca::GAP_CHAR) {
            isGapColoumn = false;
            break;
        }
    }
    CHECK_SET_ERR(isGapColoumn, "Unexpected character in the row - not a GAP ");

    //Expected state: Gap has been inserted in the reference;
    QString refChar = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, x, 1);
    CHECK_SET_ERR(refChar.size() == 1, "Incorrect reference selection");
    CHECK_SET_ERR(refChar[0] == U2Mca::GAP_CHAR, "Incorrect reference character");

    //Expected state: consensus  sequence is recomputed according to the settings in the Option Panel
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'N', QString("Incorrect consensus character, expected: N, current: %1").arg(consSel));

    //5. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'A'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //6. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0024_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the context menu in the sequence area.
    //Expected state: the menu contains an item "Edit > Insert character/gap".The item is enabled.A hotkey Shift + R is shown nearby.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Edit"
                                                                              << "Insert character/gap",
                                                            PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));

    //4. Select the item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Insert character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //Expected state : the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //5. Press a key on the keyboard with any character of the same alphabet (e.g "N" key)
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //Expected state: Gap column has been inserted in all reads for this coordinate;
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    int y = sel.y();
    int rowNums = GTUtilsMcaEditorSequenceArea::getNameList(os).size();
    bool isGapColoumn = true;
    for (int i = 0; i < rowNums; i++) {
        if (i == y) {
            continue;
        }
        char ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, QPoint(x, i));
        if (ch != U2Mca::GAP_CHAR) {
            isGapColoumn = false;
            break;
        }
    }
    CHECK_SET_ERR(isGapColoumn, "Unexpected character in the row - not a GAP ");

    //Expected state: Gap has been inserted in the reference;
    QString refChar = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, x, 1);
    CHECK_SET_ERR(refChar.size() == 1, "Incorrect reference selection");
    CHECK_SET_ERR(refChar[0] == U2Mca::GAP_CHAR, "Incorrect reference character");

    //Expected state: consensus  sequence is recomputed according to the settings in the Option Panel
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'N', QString("Incorrect consensus character, expected: N, current: %1").arg(consSel));

    //6. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'A'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //7. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0024_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the main menu in the sequence area.
    //Expected state: the menu contains an item "Actions > Edit > Insert character/gap". The item is enabled.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Edit",
                                    QStringList() << "Insert character/gap",
                                    PopupChecker::CheckOption(PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));

    //4. Select the item.
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Insert character/gap");

    //Expected state : the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //5. Press a key on the keyboard with any character of the same alphabet (e.g "N" key)
    GTKeyboardDriver::keyClick('N');
    GTGlobals::sleep();

    //Expected state: Expected result: the original character of the alignment was replaced with the new one
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //Expected state: Gap column has been inserted in all reads for this coordinate;
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    int y = sel.y();
    int rowNums = GTUtilsMcaEditorSequenceArea::getNameList(os).size();
    bool isGapColoumn = true;
    for (int i = 0; i < rowNums; i++) {
        if (i == y) {
            continue;
        }
        char ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, QPoint(x, i));
        if (ch != U2Mca::GAP_CHAR) {
            isGapColoumn = false;
            break;
        }
    }
    CHECK_SET_ERR(isGapColoumn, "Unexpected character in the row - not a GAP ");

    //Expected state: Gap has been inserted in the reference;
    QString refChar = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, x, 1);
    CHECK_SET_ERR(refChar.size() == 1, "Incorrect reference selection");
    CHECK_SET_ERR(refChar[0] == U2Mca::GAP_CHAR, "Incorrect reference character");

    //Expected state: consensus  sequence is recomputed according to the settings in the Option Panel
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'N', QString("Incorrect consensus character, expected: N, current: %1").arg(consSel));

    //6. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);

    //Expected state: This is character 'A'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //7. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: This is character 'C'
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'N', "Incorrect selected character");

    //Expected state: selection is in normal mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0025_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Press Shift + I keys on the keyboard.
    GTKeyboardDriver::keyClick('I', Qt::ShiftModifier);
    GTGlobals::sleep();

    //Expected state: the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //4. Press wrong symbol "\"
    GTKeyboardDriver::keyClick('\\');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0025_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the context menu in the sequence area.
    //Expected state: the menu contains an item "Edit > Insert character/gap".The item is enabled.A hotkey Shift + I is shown nearby.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Edit"
                                                                              << "Insert character/gap",
                                                            PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));

    //4. Select the item.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Insert character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //Expected state: the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //5. Push wrong symbol "$"
    GTKeyboardDriver::keyClick('$');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0025_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(100);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    GTGlobals::sleep();

    //3. Open the main menu in the sequence area.
    //Expected state: the menu contains an item "Actions > Edit > Insert character/gap". The item is enabled. A hotkey Shift+I is shown nearby.
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Edit",
                                    QStringList() << "Insert character/gap",
                                    PopupChecker::CheckOption(PopupChecker::IsEnabled));
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));

    //4. Select the item.
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Insert character/gap");

    //Expected state : the character is selected in the insertion mode.
    modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 2, "Incorrect modification state");

    //5. Push wrong symbol "@"
    GTKeyboardDriver::keyClick('@');

    //Expected state: Frame is vanished and error notification appears:
    GTUtilsNotifications::waitForNotification(os, true, "It is not possible to insert the character into the alignment. Please use a character from DNA extended alphabet (upper-case or lower-case) or the gap character");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0026_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(1000);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "1 Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    GTGlobals::sleep(100);

    //3. Press Del keys on the keyboard.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(100);

    //Expected state: the character is replaced by close character, the sequence is shifted one character to the left
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "2 Incorrect selected character");
    qint64 newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength + 1, "Incorrect row length");

    //Expected state: Consensus sequence is recomputed according to the settings in the Option Panel
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));

    //Expected state: Reference sequence is not changed
    qint64 newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //4. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep(100);

    //Expected result: 'A' character appeared
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "3 Incorrect selected character");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'A', QString("Incorrect consensus character, expected: A, current: %1").arg(consSel));

    //Expected state: the sequence is shifted one character to the right
    newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength, "Incorrect row length");

    //5. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: the character is replaced by close character
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "4 Incorrect selected character");

    //Expected state: the sequence is shifted one character to the left
    newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));
}

GUI_TEST_CLASS_DEFINITION(test_0026_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(500);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "1 Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    GTGlobals::sleep(100);

    //3. Press "Remove selection" from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove character/gap"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(100);

    //Expected state: the character is replaced by close character, the sequence is shifted one character to the left
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "2 Incorrect selected character");
    qint64 newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength + 1, "Incorrect row length");

    //Expected state: Consensus sequence is recomputed according to the settings in the Option Panel
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));

    //Expected state: Reference sequence is not changed
    qint64 newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //4. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep(100);

    //Expected result: 'A' character appeared
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "3 Incorrect selected character");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'A', QString("Incorrect consensus character, expected: A, current: %1").arg(consSel));

    //Expected state: the sequence is shifted one character to the right
    newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength, "Incorrect row length");

    //5. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: the character is replaced by close character
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "4 Incorrect selected character");

    //Expected state: the sequence is shifted one character to the left
    newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));
}

GUI_TEST_CLASS_DEFINITION(test_0026_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one character in the ane read (e.g. this is character 'A')
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2118, 1));
    GTGlobals::sleep(1000);

    //Expected state: his is character 'A'
    char selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'A', "1 Incorrect selected character");

    //Expected state: the character is selected in the normal mode(i.e.borders of the character are drawn using a dashed line).
    short modState = GTUtilsMcaEditorSequenceArea::getCharacterModificationMode(os);
    CHECK_SET_ERR(modState == 0, "Incorrect modification state");
    qint64 rowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    qint64 refLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    GTGlobals::sleep(100);

    //3. Press "Remove character/gap" from main
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove character/gap");
    GTGlobals::sleep(100);

    //Expected state: the character is replaced by close character, the sequence is shifted one character to the left
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "2 Incorrect selected character");
    qint64 newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength + 1, "Incorrect row length");

    //Expected state: Consensus sequence is recomputed according to the settings in the Option Panel
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    int x = sel.x();
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));

    //Expected state: Reference sequence is not changed
    qint64 newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //4. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep(100);

    //Expected result: 'A' character appeared
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    GTGlobals::sleep(100);
    CHECK_SET_ERR(selectedChar == 'A', "3 Incorrect selected character");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'A', QString("Incorrect consensus character, expected: A, current: %1").arg(consSel));

    //Expected state: the sequence is shifted one character to the right
    newRowLength = GTUtilsMcaEditorSequenceArea::getRowLength(os, 1);
    CHECK_SET_ERR(rowLength == newRowLength, "Incorrect row length");

    //5. Push Redo (Ctrl+Y)
    GTUtilsMcaEditor::redo(os);

    //Expected state: the character is replaced by close character
    selectedChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(selectedChar == 'G', "4 Incorrect selected character");

    //Expected state: the sequence is shifted one character to the left
    newRefLength = GTUtilsMcaEditorSequenceArea::getReferenceLength(os);
    CHECK_SET_ERR(refLength == newRefLength, "Error: reference length was changed");

    //Expected result: consensus  sequence is restored
    sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    x = sel.x();
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(x, 1));
    CHECK_SET_ERR(consSel.size() == 1, "Incorrect consensus selection");
    CHECK_SET_ERR(consSel[0] == 'G', QString("Incorrect consensus character, expected: G, current: %1").arg(consSel));
}

GUI_TEST_CLASS_DEFINITION(test_0027_1) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Find the column, composed by gaps exept one symbol in the row
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2071, 1));
    GTGlobals::sleep();

    //3. Replace this symbol by gap
    GTKeyboardDriver::keyClick('R', Qt::ShiftModifier);
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(U2Mca::GAP_CHAR);
    GTGlobals::sleep(1000);

    //7. Press Shift + Delete
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTGlobals::sleep(1000);

    //Expected state: Gap column is vanished
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    QString refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    char rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));

    //4. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep();

    //Expected result : gap column was restored
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == U2Mca::GAP_CHAR, QString("Invalid reference selected character, expected: GAP, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == U2Mca::GAP_CHAR, QString("Invalid row selected character, expected: GAP, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == U2Mca::GAP_CHAR, QString("Invalid consensus selected character, expected: GAP, current: %1").arg(consSel[0]))

    //5. Push Redo(Ctrl + Y)
    GTUtilsMcaEditor::redo(os);

    //Expected result : Gap column is vanished
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));
}

GUI_TEST_CLASS_DEFINITION(test_0027_2) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Find the column, composed by gaps exept one symbol in the row
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2071, 1));
    GTGlobals::sleep();

    //3. Replace this symbol by gap
    GTKeyboardDriver::keyClick('R', Qt::ShiftModifier);
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(U2Mca::GAP_CHAR);
    GTGlobals::sleep(1000);

    //4. Press "Remove all columns of gaps" from context menu
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit"
                                                                              << "Remove all columns of gaps"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    //Expected state: Gap column is vanished
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing 1");
    QString refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    char rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));

    //5. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep();

    //Expected result : gap column was restored
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing 2");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == U2Mca::GAP_CHAR, QString("Invalid reference selected character, expected: GAP, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == U2Mca::GAP_CHAR, QString("Invalid row selected character, expected: GAP, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == U2Mca::GAP_CHAR, QString("Invalid consensus selected character, expected: GAP, current: %1").arg(consSel[0]))

    //6. Push Redo(Ctrl + Y)
    GTUtilsMcaEditor::redo(os);

    //Expected result : Gap column is vanished
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing 3");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));
}

GUI_TEST_CLASS_DEFINITION(test_0027_3) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Find the column, composed by gaps exept one symbol in the row
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2071, 1));
    GTGlobals::sleep();

    //3 Replace this symbol by gap
    GTKeyboardDriver::keyClick('R', Qt::ShiftModifier);
    GTGlobals::sleep(1000);
    GTKeyboardDriver::keyClick(U2Mca::GAP_CHAR);
    GTGlobals::sleep(1000);

    //4. Press "Remove all columns of gaps" from main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Edit"
                                                << "Remove all columns of gaps");
    GTGlobals::sleep();

    //Expected state: Gap column is vanished
    QRect sel = GTUtilsMcaEditorSequenceArea::getSelectedRect(os);
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    QString refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    char rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    QString consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));

    //5. Push Undo (Ctrl+Z)
    GTUtilsMcaEditor::undo(os);
    GTGlobals::sleep();

    //Expected result : gap column was restored
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == U2Mca::GAP_CHAR, QString("Invalid reference selected character, expected: GAP, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == U2Mca::GAP_CHAR, QString("Invalid row selected character, expected: GAP, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == U2Mca::GAP_CHAR, QString("Invalid consensus selected character, expected: GAP, current: %1").arg(consSel[0]))

    //6. Push Redo(Ctrl + Y)
    GTUtilsMcaEditor::redo(os);

    //Expected result : Gap column is vanished
    CHECK_SET_ERR(sel.width() == 1 && sel.height() == 1, "Incorrect selection after gaps column removing");
    refSel = GTUtilsMcaEditorSequenceArea::getReferenceReg(os, sel.x(), 1);
    CHECK_SET_ERR(refSel[0] == 'G', QString("Invalid reference selected character, expected: G, current: %1").arg(refSel[0]));
    rowChar = GTUtilsMcaEditorSequenceArea::getSelectedReadChar(os);
    CHECK_SET_ERR(rowChar == 'G', QString("Invalid row selected character, expected: G, current: %1").arg(rowChar));
    consSel = GTUtilsMcaEditorSequenceArea::getConsensusStringByRegion(os, U2Region(sel.x(), 1));
    CHECK_SET_ERR(consSel[0] == 'G', QString("Invalid consensus selected character, expected: G, current: %1").arg(consSel[0]));
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    // 2. Click "Show chromatograms" button on the toolbar.
    GTUtilsMcaEditor::toggleShowChromatogramsMode(os);

    //2. Select one symbol in the read
    QPoint point(2218, 1);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, point);
    GTGlobals::sleep(100);

    //3. Push Space key
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTGlobals::sleep(100);

    //Expected state : Gap is inserted before symbol
    char ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, point);
    CHECK_SET_ERR(ch == U2Mca::GAP_CHAR, QString("Incorrect character, expected GAP, current %1").arg(ch));

    //4. Push Васkspace key
    GTKeyboardDriver::keyClick(Qt::Key_Backspace);
    GTGlobals::sleep(100);

    //Expected state : Gap is removed
    ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, point);
    CHECK_SET_ERR(ch == 'T', QString("Incorrect character, expected T, current %1").arg(ch));
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Select one symbol in the read
    QPoint point(2218, 1);
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, point);
    GTGlobals::sleep(100);

    QPoint startMousePosotion = GTMouseDriver::getMousePosition();

    //3. Move mouse with pressed left button to the right on one position
    GTUtilsMcaEditorSequenceArea::dragAndDrop(os, QPoint(startMousePosotion.x() + 20, startMousePosotion.y()));
    GTGlobals::sleep(100);

    //Expected state: Gap is inserted before symbol
    char ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, point);
    CHECK_SET_ERR(ch == U2Mca::GAP_CHAR, QString("Incorrect character, expected GAP, current %1").arg(ch));

    //4. Move mouse with pressed left button to the left on one position
    GTUtilsMcaEditorSequenceArea::dragAndDrop(os, startMousePosotion);
    GTGlobals::sleep(100);

    //Expected state : Gap is removed
    ch = GTUtilsMcaEditorSequenceArea::getReadCharByPos(os, point);
    CHECK_SET_ERR(ch == 'T', QString("Incorrect character, expected T, current %1").arg(ch));
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    int startRowHeinght = GTUtilsMcaEditorSequenceArea::getRowHeight(os, 0);

    //2. Push Zoom In
    GTUtilsMcaEditor::zoomIn(os);
    GTGlobals::sleep(100);

    //Expected state : All Characters increased in size
    int currentRowHeight = GTUtilsMcaEditorSequenceArea::getRowHeight(os, 0);
    CHECK_SET_ERR(startRowHeinght < currentRowHeight, QString("Unexpected row height, must be higher then start height_1 == %1, current %2").arg(QString::number(startRowHeinght)).arg(QString::number(currentRowHeight)));

    //3. Push Zoom out
    GTUtilsMcaEditor::zoomOut(os);
    GTGlobals::sleep(100);

    //Expected state : All Characters reduced in size
    currentRowHeight = GTUtilsMcaEditorSequenceArea::getRowHeight(os, 0);
    CHECK_SET_ERR(startRowHeinght == currentRowHeight, QString("Unexpected row height, must be equal start height_2 == %1, current %2").arg(QString::number(startRowHeinght)).arg(QString::number(currentRowHeight)));

    //4. Push Zoom In 2 times
    GTUtilsMcaEditor::zoomIn(os);
    GTGlobals::sleep(100);
    GTUtilsMcaEditor::zoomIn(os);
    GTGlobals::sleep();

    //Expected state : All Characters increased in size
    currentRowHeight = GTUtilsMcaEditorSequenceArea::getRowHeight(os, 0);
    CHECK_SET_ERR(startRowHeinght < currentRowHeight, QString("Unexpected row height, must be higher then start height_3 == %1, current %2").arg(QString::number(startRowHeinght)).arg(QString::number(currentRowHeight)));

    //5. Push Reset Zoom
    GTUtilsMcaEditor::resetZoom(os);
    GTGlobals::sleep(100);

    //Expected state : All Characters reduced in size
    currentRowHeight = GTUtilsMcaEditorSequenceArea::getRowHeight(os, 0);
    CHECK_SET_ERR(startRowHeinght == currentRowHeight, QString("Unexpected row height, must be equal start height_4 == %1, current %2").arg(QString::number(startRowHeinght)).arg(QString::number(currentRowHeight)));
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2. Push General button
    GTUtilsOptionPanelMca::openTab(os, GTUtilsOptionPanelMca::General);
    GTGlobals::sleep();

    //Expected state :Sequence number: 16
    int height = GTUtilsOptionPanelMca::getHeight(os);
    CHECK_SET_ERR(height == 16, QString("Incorrect height, expected: 16, current: %1").arg(QString::number(height)));
    GTGlobals::sleep();

    //Expected state: Reference length: 11937
    int length = GTUtilsOptionPanelMca::getLength(os);
    CHECK_SET_ERR(length == 11937, QString("Incorrect length, expected: 11937, current: %1").arg(QString::number(length)))
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //2.Push "Consensus" button
    GTUtilsOptionPanelMca::openTab(os, GTUtilsOptionPanelMca::Consensus);

    //Expected state :"Consenus mode" is expanded
    bool isTabOpen = GTUtilsOptionPanelMca::isTabOpened(os, GTUtilsOptionPanelMca::Consensus);
    CHECK_SET_ERR(isTabOpen, "Consensus tab is not open");

    //Expected state :"Simple extended" by default (combo with 2 values: "Simple extended" and "Strict")
    QStringList types = GTUtilsOptionPanelMca::getConsensusTypes(os);
    bool hasMembers = true;
    foreach (const QString &type, types) {
        if (type != "Simple extended" && type != "Strict") {
            hasMembers = false;
        }
    }
    CHECK_SET_ERR(hasMembers, "There are no some consensus types");

    //Expected state :Threshold = 100 % (can be changed)
    int threshold = GTUtilsOptionPanelMca::getThreshold(os);
    CHECK_SET_ERR(threshold == 100, QString("Unexpected threshold, expected: 100, current^ %1").arg(QString::number(threshold)));

    //3. Set Threshold = 50 %
    GTUtilsOptionPanelMca::setThreshold(os, 50);
    GTGlobals::sleep();

    //4. Push "Reset to default value"
    GTUtilsOptionPanelMca::pushResetButton(os);
    GTGlobals::sleep();

    //Expected state : Threshold = 100 %
    threshold = GTUtilsOptionPanelMca::getThreshold(os);
    CHECK_SET_ERR(threshold == 100, QString("Unexpected threshold, expected: 100, current^ %1").arg(QString::number(threshold)));

    //5. Collapse "Consenus mode"
    GTUtilsOptionPanelMca::closeTab(os, GTUtilsOptionPanelMca::Consensus);

    //Expected state : "Consenus mode" is collapsed
    isTabOpen = GTUtilsOptionPanelMca::isTabOpened(os, GTUtilsOptionPanelMca::Consensus);
    CHECK_SET_ERR(!isTabOpen, "Consensus tab is open");
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //Expected state: Aligned Reads Map is in the bottom screen by default. Show / Hide overview button is in pressed state
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Appearance",
                                    QStringList() << "Show overview",
                                    PopupChecker::CheckOption(PopupChecker::IsChecked));
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);

    //2. Push Show / Hide overview button on the main menu
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions"
                                                << "Appearance"
                                                << "Show overview");

    //Expected state: There are no map on the screen. Show / Hide overview button is is in released state
    // simple = GTWidget::findWidget(os, "mca_overview_area_sanger");
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Appearance",
                                    QStringList() << "Show overview",
                                    PopupChecker::CheckOption(PopupChecker::IsUnchecked));
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);

    //3. Close editor and open it again(map state should be saved)

    //Expected state: There is no map on the screen
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
    //1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //Expected state : Aligned Reads Map is in the bottom screen by default
    GTMenu::checkMainMenuItemsState(os, QStringList() << "Actions"
                                                      << "Appearance",
                                    QStringList() << "Show overview",
                                    PopupChecker::CheckOption(PopupChecker::IsChecked));
    GTGlobals::sleep(200);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(200);

    //2. Select transparent square  and move it by mouse  down
    QWidget *simple = GTWidget::findWidget(os, "mca_overview_area_sanger");
    GTWidget::click(os, simple);
    QStringList list = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    QPoint p = GTMouseDriver::getMousePosition();
    QPoint rightP(p.x(), p.y() + 50);
    GTUtilsMcaEditorSequenceArea::dragAndDrop(os, rightP);
    GTGlobals::sleep();

    //Еxpected state : Alighed reads area moved down
    QStringList listOne = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(list != listOne, "Visible area not change");

    //3. Move it by mouse up
    QPoint leftP(p.x(), p.y() - 50);
    GTUtilsMcaEditorSequenceArea::dragAndDrop(os, leftP);

    //Еxpected state : Alighed reads area moved up
    QStringList listTwo = GTUtilsMcaEditorSequenceArea::getVisibleNames(os);
    CHECK_SET_ERR(list != listTwo, "Visible area not change");
}

GUI_TEST_CLASS_DEFINITION(test_0040_1) {
    QString filePath = testDir + "_common_data/sanger/alignment.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    // chrom show/hide
    QAbstractButton *showChromsButton = GTAction::button(os, "chromatograms");
    CHECK_SET_ERR(showChromsButton != NULL, "show/hide chromatograms button was not found");
    GTWidget::click(os, showChromsButton);
    bool chromCheckedState = showChromsButton->isChecked();

    // overview show/hide
    QAbstractButton *showOverviewButton = GTAction::button(os, "overview");
    CHECK_SET_ERR(showOverviewButton != NULL, "overview button was not found");
    GTWidget::click(os, showOverviewButton);
    bool overviewCheckedState = showOverviewButton->isChecked();

    // offsets show/hide
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Show offsets"));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);

    QAction *offsetAction = GTUtilsMcaEditor::getOffsetAction(os);
    bool offsetCheckedState = offsetAction->isChecked();

    // close the view
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTUtilsProjectTreeView::click(os, "sanger_alignment.ugenedb", Qt::RightButton);
    GTGlobals::sleep();

    // open the view again
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::getActiveMcaEditorWindow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // check saved states
    showChromsButton = GTAction::button(os, "chromatograms");
    CHECK_SET_ERR(showChromsButton != NULL, "show/hide chromatograms button was not found");
    CHECK_SET_ERR(chromCheckedState == showChromsButton->isChecked(), "Show/hide chromatograms button state was not saved");

    showOverviewButton = GTAction::button(os, "overview");
    CHECK_SET_ERR(showOverviewButton != NULL, "overview button was not found");
    CHECK_SET_ERR(overviewCheckedState == showOverviewButton->isChecked(), "Show/hide overview button state was not saved");

    offsetAction = GTUtilsMcaEditor::getOffsetAction(os);
    CHECK_SET_ERR(offsetAction != NULL, "overview button was not found");
    CHECK_SET_ERR(offsetCheckedState == offsetAction->isChecked(), "Show/hide offset button state was not saved");
}

GUI_TEST_CLASS_DEFINITION(test_0040_2) {
    QString filePath = testDir + "_common_data/sanger/alignment.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    // cons type and threshold
    GTUtilsOptionPanelMca::setConsensusType(os, "Strict");
    GTUtilsOptionPanelMca::setThreshold(os, 57);

    // close the view with ugenedb
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTUtilsProjectTreeView::click(os, "sanger_alignment.ugenedb", Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    // open COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::General);

    QComboBox *consensusCombo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "consensusType"));
    CHECK_SET_ERR(consensusCombo != nullptr, "consensusCombo is NULL");
    GTComboBox::selectItemByText(os, consensusCombo, "Levitsky");

    QSpinBox *thresholdSpinBox = qobject_cast<QSpinBox *>(GTWidget::findWidget(os, "thresholdSpinBox"));
    CHECK_SET_ERR(thresholdSpinBox != nullptr, "consensusCombo is NULL");
    GTSpinBox::setValue(os, thresholdSpinBox, 68, GTGlobals::UseKeyBoard);

    // close the view with COI
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTUtilsProjectTreeView::click(os, "COI.aln", Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    // open ugenedb and check consensus settings
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    CHECK_SET_ERR(GTUtilsOptionPanelMca::getConsensusType(os) == "Strict", "Consensus algorithm type for MCA was not saved");
    CHECK_SET_ERR(GTUtilsOptionPanelMca::getThreshold(os) == 57, "Consensus threshold for MCA was not saved");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTUtilsProjectTreeView::click(os, "sanger_alignment.ugenedb", Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    // open COI.aln and check consensus settings
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive(os);

    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(os, GTUtilsOptionPanelMsa::General);

    consensusCombo = qobject_cast<QComboBox *>(GTWidget::findWidget(os, "consensusType"));
    GTComboBox::checkCurrentValue(os, consensusCombo, "Levitsky");
    CHECK_SET_ERR(GTUtilsOptionPanelMsa::getThreshold(os) == 68, "Consensus threshold for MSA was not saved");
}

GUI_TEST_CLASS_DEFINITION(test_0040_3) {
    QString filePath = testDir + "_common_data/sanger/alignment.ugenedb";
    QString fileName = "sanger_alignment.ugenedb";

    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new FontDialogFiller(os));

    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Appearance"
                                                                              << "Change characters font..."));
    GTUtilsMcaEditorSequenceArea::callContextMenu(os);
    GTUtilsDialog::waitAllFinished(os);

    ScaleBar *scaleBar = GTWidget::findExactWidget<ScaleBar *>(os, "peak_height_slider");

    QAbstractButton *plusButton = scaleBar->getPlusButton();
    GTWidget::click(os, plusButton);
    GTWidget::click(os, plusButton);
    GTWidget::click(os, plusButton);
    GTWidget::click(os, plusButton);
    GTWidget::click(os, plusButton);

    int peakHight = scaleBar->value();

    // close ugenedb
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_project__remove_selected_action"));
    GTUtilsProjectTreeView::click(os, "sanger_alignment.ugenedb", Qt::RightButton);
    GTUtilsDialog::waitAllFinished(os);

    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    scaleBar = GTWidget::findExactWidget<ScaleBar *>(os, "peak_height_slider");
    CHECK_SET_ERR(scaleBar->value() == peakHight, "Peak height was not saved");
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    const QString NONE_MARK = "-";
    const QString GAP_MARK = "gap";

    //    Check values on the status bar with different selections in the MCA

    //    1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + getSuite() + "_" + getName() + ".ugenedb";
    GTFile::copy(os, testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(os, filePath);
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive(os);

    //    Expected state: Line: - / 16; RefPos: - / 11878; ReadPos: - / -.
    QString rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    QString rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    QString referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    QString referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    QString readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    QString readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR(NONE_MARK == rowNumberString, QString("1. Unexpected row number label: expected '%1', got '%2'").arg(NONE_MARK).arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("1. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("1. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("1. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("1. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR(NONE_MARK == readLengthString, QString("1. Unexpected read length label: expected '%1', got '%2'").arg(NONE_MARK).arg(readLengthString));

    //    2. Select the first row in the name list.
    GTUtilsMcaEditor::clickReadName(os, 0);
    GTGlobals::sleep(100);

    //    Expected state: Line: 1 / 16; RefPos: - / 11878; ReadPos: - / 956.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("1" == rowNumberString, QString("2. Unexpected row number label: expected '%1', got '%2'").arg("1").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("2. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("2. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("2. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("2. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR("956" == readLengthString, QString("2. Unexpected read length label: expected '%1', got '%2'").arg("956").arg(readLengthString));

    //    3. Select the second row in the name list.
    GTUtilsMcaEditor::clickReadName(os, 1);
    GTGlobals::sleep(100);

    //    Expected state: Line: 2 / 16; RefPos: - / 11878; ReadPos: - / 1173.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("2" == rowNumberString, QString("3. Unexpected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("3. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("3. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("3. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("3. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR("1173" == readLengthString, QString("3. Unexpected read length label: expected '%1', got '%2'").arg("1173").arg(readLengthString));

    //    4. Select the last row in the name list.
    GTUtilsMcaEditor::clickReadName(os, 15);
    GTGlobals::sleep(1000);

    //    Expected state: Line: 16 / 16; RefPos: - / 11878; ReadPos: - / 1048.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("16" == rowNumberString, QString("4. Unexpected row number label: expected '%1', got '%2'").arg("16").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("4. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("4. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("4. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("4. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR("1048" == readLengthString, QString("4. Unexpected read length label: expected '%1', got '%2'").arg("1048").arg(readLengthString));

    //    5. Click "Show chromatograms" button on the toolbar.
    GTUtilsMcaEditor::toggleShowChromatogramsMode(os);
    GTGlobals::sleep(1000);

    //    Expected state: all rows have been expanded, the labels are the same as in the previous step.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("16" == rowNumberString, QString("5. Unexpected row number label: expected '%1', got '%2'").arg("16").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("5. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("5. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("5. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("5. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR("1048" == readLengthString, QString("5. Unexpected read length label: expected '%1', got '%2'").arg("1048").arg(readLengthString));
    GTGlobals::sleep(500);
    //    6. Select the first row in the name list.
    GTUtilsMcaEditor::clickReadName(os, 0);
    GTGlobals::sleep(500);

    //    Expected state: Line: 1 / 16; RefPos: - / 11878; ReadPos: - / 956.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("1" == rowNumberString, QString("6. Unexpected row number label: expected '%1', got '%2'").arg("1").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("6. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(NONE_MARK == referencePositionString, QString("6. Unexpected reference position label: expected '%1', got '%2'").arg(NONE_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("6. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("6. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR("956" == readLengthString, QString("6. Unexpected read length label: expected '%1', got '%2'").arg("956").arg(readLengthString));

    //    7. Select the first base in the reference.
    GTUtilsMcaEditorReference::clickToPosition(os, 0);
    GTGlobals::sleep(100);

    //    Expected state: Line: - / 16; RefPos: 1 / 11878; ReadPos: - / -.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR(NONE_MARK == rowNumberString, QString("7. Unexpected row number label: expected '%1', got '%2'").arg(NONE_MARK).arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("7. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("1" == referencePositionString, QString("7. Unexpected reference position label: expected '%1', got '%2'").arg("1").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("7. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("7. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR(NONE_MARK == readLengthString, QString("7. Unexpected read length label: expected '%1', got '%2'").arg(NONE_MARK).arg(readLengthString));

    //    8. Select the third base in the reference.
    GTUtilsMcaEditorReference::clickToPosition(os, 2);
    GTGlobals::sleep(100);

    //    Expected state: Line: - / 16; RefPos: 3 / 11878; ReadPos: - / -.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR(NONE_MARK == rowNumberString, QString("8. Unexpected row number label: expected '%1', got '%2'").arg(NONE_MARK).arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("8. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("3" == referencePositionString, QString("8. Unexpected reference position label: expected '%1', got '%2'").arg("3").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("8. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("8. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR(NONE_MARK == readLengthString, QString("8. Unexpected read length label: expected '%1', got '%2'").arg(NONE_MARK).arg(readLengthString));

    //    9. Select the last base in the reference.
    GTUtilsMcaEditorReference::clickToPosition(os, 11936);
    GTGlobals::sleep(1000);

    //    Expected state: Line: - / 16; RefPos: 11878 / 11878; ReadPos: - / -.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR(NONE_MARK == rowNumberString, QString("9. Unexpected row number label: expected '%1', got '%2'").arg(NONE_MARK).arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("9. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("11878" == referencePositionString, QString("9. Unexpected reference position label: expected '%1', got '%2'").arg("11878").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("9. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("9. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR(NONE_MARK == readLengthString, QString("9. Unexpected read length label: expected '%1', got '%2'").arg(NONE_MARK).arg(readLengthString));

    //    10. Select a column with a gap in the reference.
    GTUtilsMcaEditorReference::clickToPosition(os, 2071);
    GTGlobals::sleep(100);

    //    Expected state: Line: - / 16; RefPos: gap / 11878; ReadPos: - / -.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR(NONE_MARK == rowNumberString, QString("10. Unexpected row number label: expected '%1', got '%2'").arg(NONE_MARK).arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("10. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(GAP_MARK == referencePositionString, QString("10. Unexpected reference position label: expected '%1', got '%2'").arg(GAP_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("10. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(NONE_MARK == readPositionString, QString("10. Unexpected read position label: expected '%1', got '%2'").arg(NONE_MARK).arg(readPositionString));
    CHECK_SET_ERR(NONE_MARK == readLengthString, QString("10. Unexpected read length label: expected '%1', got '%2'").arg(NONE_MARK).arg(readLengthString));

    //    11. Select the first base of the second read.
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(2052, 1));
    GTGlobals::sleep(100);

    //    Expected state: Line: 2 / 16; RefPos: 2053 / 11878; ReadPos: 1 / 1173.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("2" == rowNumberString, QString("11. Unexpected row number label: expected '%1', got '%2'").arg("2").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("11. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("2053" == referencePositionString, QString("11. Unexpected reference position label: expected '%1', got '%2'").arg("2053").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("11. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("1" == readPositionString, QString("11. Unexpected read position label: expected '%1', got '%2'").arg("1").arg(readPositionString));
    CHECK_SET_ERR("1173" == readLengthString, QString("11. Unexpected read length label: expected '%1', got '%2'").arg("1173").arg(readLengthString));

    //    12. Select the third base of the fourth read.
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(4615, 3));
    GTGlobals::sleep(100);

    //    Expected state: Line: 4 / 16; RefPos: 4570 / 11878; ReadPos: 3 / 1014.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("4" == rowNumberString, QString("12. Unexpected row number label: expected '%1', got '%2'").arg("4").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("12. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("4570" == referencePositionString, QString("12. Unexpected reference position label: expected '%1', got '%2'").arg("4570").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("12. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("3" == readPositionString, QString("12. Unexpected read position label: expected '%1', got '%2'").arg("3").arg(readPositionString));
    CHECK_SET_ERR("1014" == readLengthString, QString("12. Unexpected read length label: expected '%1', got '%2'").arg("1014").arg(readLengthString));

    //    13. Select the fourth symbol of the fourth read (it is a gap).
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(4616, 3));
    GTGlobals::sleep(100);

    //    Expected state: Line: 4 / 16; RefPos: 4571 / 11878; ReadPos: gap / 1014.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("4" == rowNumberString, QString("13. Unexpected row number label: expected '%1', got '%2'").arg("4").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("13. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("4571" == referencePositionString, QString("13. Unexpected reference position label: expected '%1', got '%2'").arg("4571").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("13. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(GAP_MARK == readPositionString, QString("13. Unexpected read position label: expected '%1', got '%2'").arg(GAP_MARK).arg(readPositionString));
    CHECK_SET_ERR("1014" == readLengthString, QString("13. Unexpected read length label: expected '%1', got '%2'").arg("1014").arg(readLengthString));

    //    14. Select the 19 symbol of the 7 read (it is a gap, the reference also contains a gap on this position).
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(3070, 6));
    GTGlobals::sleep(100);

    //    Expected state: Line: 7 / 16; RefPos: gap / 11878; ReadPos: gap / 1036.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("7" == rowNumberString, QString("14. Unexpected row number label: expected '%1', got '%2'").arg("7").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("14. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR(GAP_MARK == referencePositionString, QString("14. Unexpected reference position label: expected '%1', got '%2'").arg(GAP_MARK).arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("14. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR(GAP_MARK == readPositionString, QString("14. Unexpected read position label: expected '%1', got '%2'").arg(GAP_MARK).arg(readPositionString));
    CHECK_SET_ERR("1036" == readLengthString, QString("14. Unexpected read length label: expected '%1', got '%2'").arg("1036").arg(readLengthString));

    //    15. Select the 21 symbol of the 7 read.
    GTUtilsMcaEditorSequenceArea::clickToPosition(os, QPoint(3072, 6));
    GTGlobals::sleep(100);

    //    Expected state: Line: 7 / 16; RefPos: 3073 / 11878; ReadPos: 20 / 1036.
    rowNumberString = GTUtilsMcaEditorStatusWidget::getRowNumberString(os);
    rowCountString = GTUtilsMcaEditorStatusWidget::getRowsCountString(os);
    referencePositionString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedPositionString(os);
    referenceLengthString = GTUtilsMcaEditorStatusWidget::getReferenceUngappedLengthString(os);
    readPositionString = GTUtilsMcaEditorStatusWidget::getReadUngappedPositionString(os);
    readLengthString = GTUtilsMcaEditorStatusWidget::getReadUngappedLengthString(os);
    CHECK_SET_ERR("7" == rowNumberString, QString("15. Unexpected row number label: expected '%1', got '%2'").arg("7").arg(rowNumberString));
    CHECK_SET_ERR("16" == rowCountString, QString("15. Unexpected rows count label: expected '%1', got '%2'").arg("16").arg(rowCountString));
    CHECK_SET_ERR("3061" == referencePositionString, QString("15. Unexpected reference position label: expected '%1', got '%2'").arg("3061").arg(referencePositionString));
    CHECK_SET_ERR("11878" == referenceLengthString, QString("15. Unexpected reference length label: expected '%1', got '%2'").arg("11878").arg(referenceLengthString));
    CHECK_SET_ERR("20" == readPositionString, QString("15. Unexpected read position label: expected '%1', got '%2'").arg("20").arg(readPositionString));
    CHECK_SET_ERR("1036" == readLengthString, QString("15. Unexpected read length label: expected '%1', got '%2'").arg("1036").arg(readLengthString));
}

}    //namespace GUITest_common_scenarios_mca_editor

}    //namespace U2
