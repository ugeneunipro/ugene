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

#include <QApplication>
#include <QCheckBox>
#include <QFileInfo>
#include <QLineEdit>
#include <QSpinBox>

#include <drivers/GTKeyboardDriver.h>

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

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
    GTLogTracer l;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 5; i <= 7; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0001").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    class CheckerFiller : public Filler {
    public:
        CheckerFiller(HI::GUITestOpStatus &os, const AlignToReferenceBlastDialogFiller::Settings& settings)
            : Filler(os, "AlignToReferenceBlastDialog"),
              settings(settings)
        {}

        virtual void run() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            QLineEdit* reference = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "referenceLineEdit", dialog));
            CHECK_SET_ERR(reference, "referenceLineEdit is NULL");
            GTLineEdit::setText(os, reference, settings.referenceUrl);

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

            QWidget* addReadButton = GTWidget::findWidget(os, "addReadButton");
            CHECK_SET_ERR(addReadButton, "addReadButton is NULL");
            foreach (const QString& read, settings.readUrls) {
                GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, read));
                GTWidget::click(os, addReadButton);
                GTGlobals::sleep();
            }

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    private:
        AlignToReferenceBlastDialogFiller::Settings settings;
    };

    GTLogTracer l;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    settings.readUrls << testDir + "_common_data/sanger/sanger_05.ab1";

    GTUtilsDialog::waitForDialog(os, new CheckerFiller(os, settings));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTLogTracer l;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 11; i <= 13; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0003").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsLog::checkContainsError(os, l, "No read satisfy minimum similarity criteria");
    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);

    settings.minIdentity = 30;

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkItem(os, "sanger_test_0003");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTLogTracer l;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/reference.gb";
    for (int i = 18; i <= 20; i++) {
        settings.readUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
    }
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0004").absoluteFilePath();
    settings.addResultToProject = false;

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);

    settings.addResultToProject = true;
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0004_1").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkItem(os, "sanger_test_0004_4");

    GTUtilsLog::check(os, l);
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
//    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter in the 'Map Sanger Reads to Reference' dialog.
//    1. Click "Tools" -> "Sanger data analysis" -> "Map reads to reference..." in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "active modal widget is NULL");

//    Expected state: 'Sequence name from file' value is set by default.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTComboBox::getCurrentText(os, "cbRowNaming", dialog);
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                          .arg(expectedRowNamingPolicy).arg(currentRowNamingPolicy));

//    2. Set input data from "_common_data/sanger/" directory and the output file.
            AlignToReferenceBlastDialogFiller::setReference(os, testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(os, readsUrls, dialog);

            AlignToReferenceBlastDialogFiller::setDestination(os, sandBoxDir + "sanger_test_0005_1.ugenedb", dialog);

//    3. Click the 'Map' button.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

//    Expected state: the result alignment rows are named like "SZYD_Cas9_*".
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList expectedReadsnames = QStringList() << "SZYD_Cas9_5B70"
                                                         << "SZYD_Cas9_5B71"
                                                         << "SZYD_Cas9_CR50"
                                                         << "SZYD_Cas9_CR51"
                                                         << "SZYD_Cas9_CR52"
                                                         << "SZYD_Cas9_CR53"
                                                         << "SZYD_Cas9_CR54"
                                                         << "SZYD_Cas9_CR55"
                                                         << "SZYD_Cas9_CR56"
                                                         << "SZYD_Cas9_CR60"
                                                         << "SZYD_Cas9_CR61"
                                                         << "SZYD_Cas9_CR62"
                                                         << "SZYD_Cas9_CR63"
                                                         << "SZYD_Cas9_CR64"
                                                         << "SZYD_Cas9_CR65"
                                                         << "SZYD_Cas9_CR66";
    const QStringList readsNames = GTUtilsMcaEditor::getReadsNames(os);
    CHECK_SET_ERR(expectedReadsnames == readsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
//    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter in the 'Map Sanger Reads to Reference' dialog.
//    1. Click "Tools" -> "Sanger data analysis" -> "Map reads to reference..." in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "active modal widget is NULL");

//    Expected state: 'Sequence name from file' value is set by default.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTComboBox::getCurrentText(os, "cbRowNaming", dialog);
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                          .arg(expectedRowNamingPolicy).arg(currentRowNamingPolicy));

//    2. Set input data from "_common_data/sanger/" directory and the output file.
            AlignToReferenceBlastDialogFiller::setReference(os, testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(os, readsUrls, dialog);

            AlignToReferenceBlastDialogFiller::setDestination(os, sandBoxDir + "sanger_test_0005_2.ugenedb", dialog);

//    3. Set 'Read name in result alignment' to 'File name'.
            GTComboBox::setIndexWithText(os, "cbRowNaming", dialog, "File name");

//    4. Click the 'Map' button.
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

//    Expected state: the result alignment rows are named like "sanger_*".
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList expectedReadsnames = QStringList() << "sanger_01"
                                                         << "sanger_02"
                                                         << "sanger_04"
                                                         << "sanger_05"
                                                         << "sanger_06"
                                                         << "sanger_07"
                                                         << "sanger_08"
                                                         << "sanger_09"
                                                         << "sanger_10"
                                                         << "sanger_14"
                                                         << "sanger_15"
                                                         << "sanger_16"
                                                         << "sanger_17"
                                                         << "sanger_18"
                                                         << "sanger_19"
                                                         << "sanger_20";
    const QStringList readsNames = GTUtilsMcaEditor::getReadsNames(os);
    CHECK_SET_ERR(expectedReadsnames == readsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0005_3) {
//    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter of the 'Map to Reference' workflow element.
//    1. Open 'Trim and map Sanger reads' workflow sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {

//    Expected state: wizard has appeared.
            QWidget *wizard = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != wizard, "active modal widget is NULL");
            GTWidget::clickWindowTitle(os, wizard);

//    2. Fill it with any valid data until the 'Mapping settings' page.
            GTUtilsWizard::setParameter(os, "Reference", QFileInfo(testDir + "_common_data/sanger/reference.gb").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QFileInfo(QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'))).absoluteFilePath();
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsUrls);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

//    Expected state: 'Sequence name from file' value is set by default for the 'Read name in result alignment' parameter.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTUtilsWizard::getParameter(os, "Read name in result alignment").toString();
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                          .arg(expectedRowNamingPolicy).arg(currentRowNamingPolicy));

//    3. Fill the wizard till the end. Run the workflow.
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::setParameter(os, "Mapped reads file", QFileInfo(sandBoxDir + "sanger_test_0005_3.ugenedb").absoluteFilePath());
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTGlobals::sleep(200);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and map Sanger reads");

//    Expected state: the result alignment rows are named like "SZYD_Cas9_*".
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDashboard::clickOutputFile(os, "sanger_test_0005_3.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList expectedReadsnames = QStringList() << "SZYD_Cas9_5B70"
                                                         << "SZYD_Cas9_5B71"
                                                         << "SZYD_Cas9_CR50"
                                                         << "SZYD_Cas9_CR51"
                                                         << "SZYD_Cas9_CR52"
                                                         << "SZYD_Cas9_CR53"
                                                         << "SZYD_Cas9_CR54"
                                                         << "SZYD_Cas9_CR55"
                                                         << "SZYD_Cas9_CR56"
                                                         << "SZYD_Cas9_CR60"
                                                         << "SZYD_Cas9_CR61"
                                                         << "SZYD_Cas9_CR62"
                                                         << "SZYD_Cas9_CR63"
                                                         << "SZYD_Cas9_CR64"
                                                         << "SZYD_Cas9_CR65"
                                                         << "SZYD_Cas9_CR66";
    const QStringList readsNames = GTUtilsMcaEditor::getReadsNames(os);
    CHECK_SET_ERR(expectedReadsnames == readsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0005_4) {
//    // Check 'Sequence name from file' value of the 'Read name in result alignment' parameter of the 'Map to Reference' workflow element.
//    1. Open 'Trim and map Sanger reads' workflow sample.GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {

//    Expected state: wizard has appeared.
             QWidget *wizard = QApplication::activeModalWidget();
             CHECK_SET_ERR(NULL != wizard, "active modal widget is NULL");
             GTWidget::clickWindowTitle(os, wizard);

//    2. Fill it with any valid data until the 'Mapping settings' page.
            GTUtilsWizard::setParameter(os, "Reference", QFileInfo(testDir + "_common_data/sanger/reference.gb").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QStringList readsUrls;
            for (int i = 1; i <= 20; i++) {
                readsUrls << QFileInfo(QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'))).absoluteFilePath();
            }
            GTUtilsWizard::setInputFiles(os, QList<QStringList>() << readsUrls);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

//    Expected state: 'Sequence name from file' value is set by default for the 'Read name in result alignment' parameter.
            const QString expectedRowNamingPolicy = "Sequence name from file";
            const QString currentRowNamingPolicy = GTUtilsWizard::getParameter(os, "Read name in result alignment").toString();
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                          .arg(expectedRowNamingPolicy).arg(currentRowNamingPolicy));


//    3. Set the 'Read name in result alignment' to 'File name'.
            GTUtilsWizard::setParameter(os, "Read name in result alignment", "File name");

//    4. Fill the wizard till the end. Run the workflow.
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::setParameter(os, "Mapped reads file", QFileInfo(sandBoxDir + "sanger_test_0005_4.ugenedb").absoluteFilePath());
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTGlobals::sleep(200);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Map Sanger Reads to Reference", new Scenario));
    GTUtilsWorkflowDesigner::addSample(os, "Trim and map Sanger reads");

//    Expected state: the result alignment rows are named like "sanger_*".
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDashboard::clickOutputFile(os, "sanger_test_0005_4.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList expectedReadsnames = QStringList() << "sanger_01"
                                                         << "sanger_02"
                                                         << "sanger_04"
                                                         << "sanger_05"
                                                         << "sanger_06"
                                                         << "sanger_07"
                                                         << "sanger_08"
                                                         << "sanger_09"
                                                         << "sanger_10"
                                                         << "sanger_14"
                                                         << "sanger_15"
                                                         << "sanger_16"
                                                         << "sanger_17"
                                                         << "sanger_18"
                                                         << "sanger_19"
                                                         << "sanger_20";
    const QStringList readsNames = GTUtilsMcaEditor::getReadsNames(os);
    CHECK_SET_ERR(expectedReadsnames == readsNames, "Incorrect reads names");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
//    // Check that reads that consists of gaps and N only are skipped
//    1. Select "Tools" -> "Sanger data analysis" -> "Map reads to reference..." item in the main menu.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

//    2. Set '_common_data/sanger/dataset3/reference.gb' as reference and the next files as reads:
//        '_common_data/sanger/dataset3/gaps.ab1'
//        '_common_data/sanger/dataset3/N.ab1'
//        '_common_data/sanger/dataset3/N_and_gaps.ab1'
//        '_common_data/sanger/dataset3/pFB7-CDK5RAP2_P1713799_009.ab1'
//        Set 'Read name in result alignment' option to 'File name'.
//        Accept the dialog.
            AlignToReferenceBlastDialogFiller::setReference(os, QFileInfo(testDir + "_common_data/sanger/dataset3/reference.gb").absoluteFilePath(), dialog);

            const QStringList reads = QStringList() << testDir + "_common_data/sanger/dataset3/gaps.ab1"
                                                    << testDir + "_common_data/sanger/dataset3/N.ab1"
                                                    << testDir + "_common_data/sanger/dataset3/N_and_gaps.ab1"
                                                    << testDir + "_common_data/sanger/dataset3/pFB7-CDK5RAP2_P1713799_009.ab1";
            AlignToReferenceBlastDialogFiller::setReads(os, reads, dialog);

            GTComboBox::setIndexWithText(os, "cbRowNaming", dialog, "File name");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the report contains information about 3 filtered reads, their similarity is 0%. The result alignment contains one mapped read with the name 'pFB7-CDK5RAP2_P1713799_009'.
    // It is too hard to check the report, because we change it too often. Just check the rows count.
    const int rowsCount = GTUtilsMcaEditor::getReadsCount(os);
    CHECK_SET_ERR(1 == rowsCount, QString("Unexpected rows count: expect 1, got %1").arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTLogTracer l;

    AlignToReferenceBlastDialogFiller::Settings settings;
    settings.referenceUrl = testDir + "_common_data/sanger/dataset5/Reference.fna";
    settings.readUrls << QString(testDir + "_common_data/sanger/dataset5/187_260_V49595_10.ab1");
    settings.outAlignment = QFileInfo(sandBoxDir + "sanger_test_0007").absoluteFilePath();

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    CHECK_SET_ERR(l.hasError(), "Alignment should fail");

    settings.minIdentity = 70;

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    const int rowsCount = GTUtilsMcaEditor::getReadsCount(os);
    CHECK_SET_ERR(1 == rowsCount, QString("Unexpected rows count: expect 1, got %1").arg(rowsCount));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTLogTracer l;

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

    GTUtilsDialog::waitForDialog(os, new AlignToReferenceBlastDialogFiller(settings, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Sanger data analysis" << "Map reads to reference...");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.checkMessage("trimming was skipped"), "Could not find the message about skipped trimming");
}


}   // namespace GUITest_common_scenarios_sanger
}   // namespace U2
