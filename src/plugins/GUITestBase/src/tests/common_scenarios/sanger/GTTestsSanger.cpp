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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

#include <QCheckBox>
#include <QFileInfo>

#include "GTTestsSanger.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_sanger {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 5; i <= 7; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0001").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    class CheckerFiller : public Filler {
    public:
        CheckerFiller(const AlignToReferenceBlastDialogFiller::Settings& settings)
            : Filler("AlignToReferenceBlastDialog"),
              settings(settings) {
        }

        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            auto reference = GTWidget::findLineEdit("referenceLineEdit", dialog);
            GTLineEdit::setText(reference, settings.referenceUrl);

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            auto addReadButton = GTWidget::findWidget("addReadButton");
            foreach (const QString& read, settings.readUrls) {
                GTUtilsDialog::waitForDialog(new GTFileDialogUtils(read));
                GTWidget::click(addReadButton);
            }

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }

    private:
        AlignToReferenceBlastDialogFiller::Settings settings;
    };

    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    settings.readUrls << testDir + "_common_data/sanger/sanger_05.ab1";

    GTUtilsDialog::waitForDialog(new CheckerFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 11; i <= 13; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0003").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasError("None of the reads satisfy minimum similarity criteria."), "Expected error is not found");
    GTUtilsProject::checkProject(GTUtilsProject::NotExists);

    settings.minIdentity = 30;

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkItem("sanger_test_0003");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 18; i <= 20; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0004").absoluteFilePath();
    settings.addResultToProject = false;

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProject::checkProject(GTUtilsProject::NotExists);

    settings.addResultToProject = true;
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0004_1").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::checkItem("sanger_test_0004_1");

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    //    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter in the 'Map Sanger Reads to Reference' dialog.
    //    1. Click "Tools" -> "Sanger data analysis" -> "Map reads to reference..." in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: 'Sequence name from file' value is set by default.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTComboBox::getCurrentText("cbRowNaming", dialog);
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                              .arg(expectedRowNamingPolicy)
                              .arg(currentRowNamingPolicy));

            //    2. Set input data from "_common_data/sanger/" directory and the output file.
            AlignToReferenceBlastDialogFiller::setReference(testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(readsUrls, dialog);

            AlignToReferenceBlastDialogFiller::setDestination(sandBoxDir + "sanger_test_0005_1.ugenedb", dialog);

            //    3. Click the 'Map' button.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    //    Expected state: the result alignment rows are named like "SZYD_Cas9_*".
    GTUtilsTaskTreeView::waitTaskFinished();

    const QStringList expectedReadsNames = {"SZYD_Cas9_5B70",
                                            "SZYD_Cas9_5B71",
                                            "SZYD_Cas9_CR50",
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
                                            "SZYD_Cas9_CR66"};
    const QStringList readsNames = GTUtilsMcaEditor::getReadsNames();
    CHECK_SET_ERR(expectedReadsNames == readsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    //    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter in the 'Map Sanger Reads to Reference' dialog.
    //    1. Click "Tools" -> "Sanger data analysis" -> "Map reads to reference..." in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    Expected state: 'Sequence name from file' value is set by default.
            QString expectedRowNamingPolicy = "Sequence name from file";
            QString currentRowNamingPolicy = GTComboBox::getCurrentText("cbRowNaming", dialog);
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                              .arg(expectedRowNamingPolicy)
                              .arg(currentRowNamingPolicy));

            //    2. Set input data from "_common_data/sanger/" directory and the output file.
            AlignToReferenceBlastDialogFiller::setReference(testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(readsUrls, dialog);

            AlignToReferenceBlastDialogFiller::setDestination(sandBoxDir + "sanger_test_0005_2.ugenedb", dialog);

            //    3. Set 'Read name in result alignment' to 'File name'.
            GTComboBox::selectItemByText("cbRowNaming", dialog, "File name");

            //    4. Click the 'Map' button.
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    //    Expected state: the result alignment rows are named like "sanger_*".
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList expectedReadsNames = {"sanger_01",
                                      "sanger_02",
                                      "sanger_04",
                                      "sanger_05",
                                      "sanger_06",
                                      "sanger_07",
                                      "sanger_08",
                                      "sanger_09",
                                      "sanger_10",
                                      "sanger_14",
                                      "sanger_15",
                                      "sanger_16",
                                      "sanger_17",
                                      "sanger_18",
                                      "sanger_19",
                                      "sanger_20"};
    QStringList readsNames = GTUtilsMcaEditor::getReadsNames();
    CHECK_SET_ERR(expectedReadsNames == readsNames, "Incorrect reads names: " + readsNames.join(","));
}

GUI_TEST_CLASS_DEFINITION(test_0005_3) {
    //    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter of the 'Map to Reference' workflow element.
    //    1. Open 'Trim and map Sanger reads' workflow sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
    public:
        void run() {
            //    Expected state: wizard has appeared.
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTWidget::clickWindowTitle(wizard);

            //    2. Fill it with any valid data until the 'Mapping settings' page.
            GTUtilsWizard::setParameter("Reference", QFileInfo(testDir + "_common_data/sanger/reference.gb").absoluteFilePath());
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QFileInfo(QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'))).absoluteFilePath();
            }
            GTUtilsWizard::setInputFiles(QList<QStringList>() << readsUrls);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //    Expected state: 'Sequence name from file' value is set by default for the 'Read name in result alignment' parameter.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTUtilsWizard::getParameter("Read name in result alignment").toString();
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                              .arg(expectedRowNamingPolicy)
                              .arg(currentRowNamingPolicy));

            //    3. Fill the wizard till the end. Run the workflow.
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::setParameter("Mapped reads file", QFileInfo(sandBoxDir + "sanger_test_0005_3.ugenedb").absoluteFilePath());
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Map Sanger Reads to Reference", new Scenario));
    GTUtilsWorkflowDesigner::addSample("Trim and map Sanger reads");

    //    Expected state: the result alignment rows are named like "SZYD_Cas9_*".
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDashboard::clickOutputFile("sanger_test_0005_3.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList expectedReadsNames = {"SZYD_Cas9_5B70",
                                      "SZYD_Cas9_5B71",
                                      "SZYD_Cas9_CR50",
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
                                      "SZYD_Cas9_CR66"};
    QStringList readsNames = GTUtilsMcaEditor::getReadsNames();
    CHECK_SET_ERR(readsNames == expectedReadsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0005_4) {
    //    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter of the 'Map to Reference' workflow element.
    //    1. Open 'Trim and map Sanger reads' workflow sample.GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            //    Expected state: wizard has appeared.
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTWidget::clickWindowTitle(wizard);

            //    2. Fill it with any valid data until the 'Mapping settings' page.
            GTUtilsWizard::setParameter("Reference", QFileInfo(testDir + "_common_data/sanger/reference.gb").absoluteFilePath());
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QFileInfo(QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'))).absoluteFilePath();
            }
            GTUtilsWizard::setInputFiles(QList<QStringList>() << readsUrls);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            //    Expected state: 'Sequence name from file' value is set by default for the 'Read name in result alignment' parameter.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTUtilsWizard::getParameter("Read name in result alignment").toString();
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                              .arg(expectedRowNamingPolicy)
                              .arg(currentRowNamingPolicy));

            //    3. Set the 'Read name in result alignment' to 'File name'.
            GTUtilsWizard::setParameter("Read name in result alignment", "File name");

            //    4. Fill the wizard till the end. Run the workflow.
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::setParameter("Mapped reads file", QFileInfo(sandBoxDir + "sanger_test_0005_4.ugenedb").absoluteFilePath());
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Map Sanger Reads to Reference", new Scenario));
    GTUtilsWorkflowDesigner::addSample("Trim and map Sanger reads");

    //    Expected state: the result alignment rows are named like "sanger_*".
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDashboard::clickOutputFile("sanger_test_0005_4.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList expectedReadsNames = {"sanger_01",
                                      "sanger_02",
                                      "sanger_04",
                                      "sanger_05",
                                      "sanger_06",
                                      "sanger_07",
                                      "sanger_08",
                                      "sanger_09",
                                      "sanger_10",
                                      "sanger_14",
                                      "sanger_15",
                                      "sanger_16",
                                      "sanger_17",
                                      "sanger_18",
                                      "sanger_19",
                                      "sanger_20"};
    QStringList readsNames = GTUtilsMcaEditor::getReadsNames();
    CHECK_SET_ERR(readsNames == expectedReadsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    // Check that reads that consists of gaps and N only are skipped
    //    1. Select "Tools" -> "Sanger data analysis" -> "Map reads to reference..." item in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            //    2. Set '_common_data/sanger/dataset3/reference.gb' as reference and the next files as reads:
            //        '_common_data/sanger/dataset3/gaps.ab1'
            //        '_common_data/sanger/dataset3/N.ab1'
            //        '_common_data/sanger/dataset3/N_and_gaps.ab1'
            //        '_common_data/sanger/dataset3/pFB7-CDK5RAP2_P1713799_009.ab1'
            //        Set 'Read name in result alignment' option to 'File name'.
            //        Accept the dialog.
            AlignToReferenceBlastDialogFiller::setReference(QFileInfo(testDir + "_common_data/sanger/dataset3/reference.gb").absoluteFilePath(), dialog);

            QStringList reads = {testDir + "_common_data/sanger/dataset3/gaps.ab1",
                                 testDir + "_common_data/sanger/dataset3/N.ab1",
                                 testDir + "_common_data/sanger/dataset3/N_and_gaps.ab1",
                                 testDir + "_common_data/sanger/dataset3/pFB7-CDK5RAP2_P1713799_009.ab1"};
            AlignToReferenceBlastDialogFiller::setReads(reads, dialog);

            GTComboBox::selectItemByText("cbRowNaming", dialog, "File name");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the report contains information about 3 filtered reads, their similarity is 0%. The result alignment contains one mapped read with the name 'pFB7-CDK5RAP2_P1713799_009'.
    // It is too hard to check the report, because we change it too often. Just check the rows count.
    int rowsCount = GTUtilsMcaEditor::getReadsCount();
    CHECK_SET_ERR(rowsCount == 1, QString("Unexpected rows count: expect 1, got %1").arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/dataset5/Reference.fna";
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset5/187_260_V49595_10.ab1");
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0007").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    CHECK_SET_ERR(lt.hasErrors(), "Expected to have errors in the log, but no errors found");

    settings.minIdentity = 70;

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();

    const int rowsCount = GTUtilsMcaEditor::getReadsCount();
    CHECK_SET_ERR(1 == rowsCount, QString("Unexpected rows count: expect 1, got %1").arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTLogTracer lt;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/dataset4/reference.gb";

    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_009.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_010.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_025.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_026.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_041.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_043.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_044.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_059.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_060.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_075.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_076.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_091.ab1");
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset4/ab1/pFB7-CDK5RAP2_P1713799_092.ab1");

    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0008").absoluteFilePath();

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(settings));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("trimming was skipped"), "Could not find the message about skipped trimming");
}

}  // namespace GUITest_common_scenarios_sanger
}  // namespace U2
