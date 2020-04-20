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

#include "GTTestsMetaPhlAn2.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTableWidget>
#include <QTreeWidget>

#include <base_dialogs/MessageBoxFiller.h>

#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>

#include <GTUtilsTaskTreeView.h>
#include "GTUtilsWorkflowDesigner.h"

#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>

#include "U2Test/UGUITest.h"

namespace U2 {

namespace GUITest_common_scenarios_mg_metaphlan2_external_tool {
using namespace HI;

const QString ET_PYTHON = "python";
const QString ET_NUMPY = "numpy";
const QString ET_BIO = "Bio";
const QString ET_BOWTIE_2_ALIGNER = "Bowtie 2 aligner";
const QString ET_BOWTIE_2_BUILD = "Bowtie 2 build indexer";
const QString ET_METAPHLAN = "MetaPhlAn2";
const QString UTIL_SCRIPT = "/utils/read_fastx.py";
const QString PATH_METAPHLAN2_WITHOUT_SCRIPT = "/_common_data/metagenomics/metaphlan2/external_tool/metaphlan2.py";
const QString PATH_PYTHON_WITHOUT_NUMPY = "/opt/share/virogenesis-dev/test_external_tools/python_without_numpy/bin";
const QString PATH_PYTHON_WITHOUT_BIO = "/opt/share/virogenesis-dev/test_external_tools/python_without_bio/bin";
const QString NAME_PYTHON = "python2.7";

void checkExternalToolValid(GUITestOpStatus &os, const QString& toolName, const bool shouldBeValid) {
    const bool isToolValid = AppSettingsDialogFiller::isExternalToolValid(os, toolName);
    if (isToolValid != shouldBeValid) {
        os.setError(QString("%1 %2 valid, but %3 be").arg(toolName)
                                                     .arg(shouldBeValid ? "isn't" : "is")
                                                     .arg(shouldBeValid ? "should" : "shoudn't"));
    }
}

void checkUtilScript(GUITestOpStatus &os, const bool shouldBeValid) {
    QString pathToMetaphlan = QDir::toNativeSeparators(AppSettingsDialogFiller::getExternalToolPath(os, ET_METAPHLAN));
    QString pathToMetaphlanDir = QFileInfo(pathToMetaphlan).absolutePath();
    QString utilNativeSeparators = QDir::toNativeSeparators(UTIL_SCRIPT);
    bool isValid = !AppSettingsDialogFiller::isToolDescriptionContainsString(os, ET_METAPHLAN, "MetaPhlAn2 script \"utils/read_fastx.py\" is not present!");
    if (isValid != shouldBeValid) {
        os.setError(QString("Unitl script %1 %2 exist, but %3 be").arg(utilNativeSeparators)
                                                                  .arg(shouldBeValid ? "doesn't" : "does")
                                                                  .arg(shouldBeValid ? "should" : "shoudn't"));
    }
}

void checkDependedTools(GUITestOpStatus &os, const QString& tool, const QStringList& toolList) {
    QStringList absentTools;
    foreach(const QString& str, toolList) {
        bool isOk = AppSettingsDialogFiller::isToolDescriptionContainsString(os, tool, str);
        if (!isOk) {
            absentTools << str;
        }
    }

    if (!absentTools.isEmpty()) {
        QString error;
        bool isSingleToolAbsent = absentTools.size() == 1;
        error += QString("%1 tool should be depended on the following %2: ").arg(tool).arg(isSingleToolAbsent ? "tool" : "tools");
        foreach(const QString& t, absentTools) {
            error += QString("%1 ,").arg(t);
        }
        error = error.left(error.size() - 1);
        os.setError(error);
    }
}

QString getPythonWithoutNumpyPath() {
    return QDir::toNativeSeparators(PATH_PYTHON_WITHOUT_NUMPY);
}

QString getPythonWithoutBioPath() {
    return QDir::toNativeSeparators(PATH_PYTHON_WITHOUT_BIO);
}

QString getMetaphlan2WithoutScriptPath() {
    return UGUITest::testDir + QDir::toNativeSeparators(PATH_METAPHLAN2_WITHOUT_SCRIPT);
}

GUI_TEST_CLASS_DEFINITION(test_0001) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //"python" is installed.
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, true);

            //"numpy" python module is installed.
            checkExternalToolValid(os, ET_NUMPY, true);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"bowtie-build" executable is not specified in UGENE.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_BOWTIE_2_BUILD, sandBoxDir);
            checkExternalToolValid(os, ET_BOWTIE_2_BUILD, false);

            //"utils/read_fastq.py" is present in the metaphlan tool folder.
            checkUtilScript(os, true);

            //"MetaPhlAn2" external tool is specified in UGENE.
            //Expected state: "MetaPhlAn2" tool is present and valid.
            checkExternalToolValid(os, ET_METAPHLAN, true);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //python" is not installed.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_PYTHON, sandBoxDir);
            checkExternalToolValid(os, ET_PYTHON, false);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, false);

            //"numpy" python module is installed.
            checkExternalToolValid(os, ET_NUMPY, false);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"utils/read_fastq.py" is present in the metaphlan tool folder.
            checkUtilScript(os, true);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            //Expected state: There is a message about tools "python" and "numpy".
            checkDependedTools(os, ET_METAPHLAN, QStringList() << ET_PYTHON << ET_NUMPY);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //python" is installed.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_PYTHON, getPythonWithoutNumpyPath(), NAME_PYTHON);
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, true);

            //"numpy" python module is not installed.
            checkExternalToolValid(os, ET_NUMPY, false);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"utils/read_fastq.py" is present in the metaphlan tool folder.
            checkUtilScript(os, true);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            //Expected state: There is a message about "numpy" tool.
            checkDependedTools(os, ET_METAPHLAN, QStringList() << ET_NUMPY);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //python" is installed.
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, true);

            //"numpy" python module is installed.
            checkExternalToolValid(os, ET_NUMPY, true);

            //"bowtie-align" executable is not specified in UGENE.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_BOWTIE_2_ALIGNER, sandBoxDir);
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, false);

            //"utils/read_fastq.py" is present in the metaphlan tool folder.
            checkUtilScript(os, true);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            //Expected state: There is a message about "bowtie2-align" tool.
            checkDependedTools(os, ET_METAPHLAN, QStringList() << ET_BOWTIE_2_ALIGNER);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);
            AppSettingsDialogFiller::setExternalToolPath(os, ET_METAPHLAN, getMetaphlan2WithoutScriptPath());

            //python" is installed.
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, true);

            //"numpy" python module is installed.
            checkExternalToolValid(os, ET_NUMPY, true);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"utils/read_fastq.py" is not present in the metaphlan tool folder.
            checkUtilScript(os, false);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //python" is installed.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_PYTHON, getPythonWithoutNumpyPath(), NAME_PYTHON);
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is installed.
            checkExternalToolValid(os, ET_BIO, true);

            //"numpy" python module is not installed.
            checkExternalToolValid(os, ET_NUMPY, false);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"utils/read_fastq.py" is not present in the metaphlan tool folder (but there is no message aboute it)
            checkUtilScript(os, true);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus &os){
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            //python" is installed.
            AppSettingsDialogFiller::setExternalToolPath(os, ET_PYTHON, getPythonWithoutBioPath(), NAME_PYTHON);
            checkExternalToolValid(os, ET_PYTHON, true);

            //"Bio" python module is not installed.
            checkExternalToolValid(os, ET_BIO, false);

            //"numpy" python module is installed.
            checkExternalToolValid(os, ET_NUMPY, true);

            //"bowtie-align" executable is specified in UGENE.
            checkExternalToolValid(os, ET_BOWTIE_2_ALIGNER, true);

            //"utils/read_fastq.py" is present in the metaphlan tool folder.
            checkUtilScript(os, true);

            //Expected state: "MetaPhlAn2" tool is present, but invalid.
            checkExternalToolValid(os, ET_METAPHLAN, false);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    //1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    CHECK_SET_ERR(!os.hasError(), os.getError());
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // 1. Open the "External Tools" page in the "Application Settings" dialog.
    // 2. Provide a valid MetaPhlAn2 executable, remove python executable.
    // 3. Apply settings.
    class Custom : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog != NULL, "AppSettingsDialogFiller isn't found");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::ExternalTools);

            AppSettingsDialogFiller::setExternalToolPath(os, ET_PYTHON, "/invalid_path/");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new Custom()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...", GTGlobals::UseMouse);

    // 4. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    // 5. Add "Classify Sequences with MetaPhlAn2" workflow element.
    GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with MetaPhlAn2");

    // Expected result : there are no errors about "python"and it's modules. There is a warning about "MetaPhlAn2" tool.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsWorkflowDesigner::validateWorkflow(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    QString error("Classify Sequences with MetaPhlAn2: External tool \"MetaPhlAn2\" is invalid. UGENE may not support this version of the tool or a wrong path to the tools is selected");
    CHECK_SET_ERR(errors.contains(error), "The expected error is absent");
    const int expectedErrorCount = 2;
    CHECK_SET_ERR(expectedErrorCount == errors.size(), QString("There are too many errors: expected %1, got %2").arg(expectedErrorCount).arg(errors.size()));
}

} // namespace GUITest_common_scenarios_mg_metaphlan2_external_tool

namespace GUITest_common_scenarios_mg_metaphlan2_workflow_designer_element {

static const QString INPUT_DATA = "Input data";
static const QString DATABASE = "Database";
static const QString NUMBER_OF_THREADS = "Number of threads";
static const QString ANALYSIS_TYPE = "Analysis type";
static const QString TAX_LEVEL = "Tax level";
static const QString PRESENCE_THRESHOLD = "Presence threshold";
static const QString NORMALIZE_BY_METAGENOME_SIZE = "Normalize by metagenome size";
static const QString BOWTIE2_OUTPUT_FILE = "Bowtie2 output file";
static const QString OUTPUT_FILE = "Output file";

static const QStringList INPUT_DATA_VALUES = { "SE reads or contigs",
                                               "PE reads" };

static const QStringList ANALYSIS_TYPE_VALUES = { "Relative abundance",
                                                  "Relative abundance with reads statistics",
                                                  "Reads mapping",
                                                  "Clade profiles",
                                                  "Marker abundance table",
                                                  "Marker presence table" };

static const QStringList TAX_LEVEL_VALUES = { "All",
                                              "Kingdoms",
                                              "Phyla",
                                              "Classes",
                                              "Orders",
                                              "Families",
                                              "Genera",
                                              "Species" };

static const QStringList NORMALIZE_BY_METAGENOME_SIZE_VALUES = { "Skip",
                                                                 "Normalize" };


static const QString DEFAULT_OUTPUT_VALUE = "Auto";


GUI_TEST_CLASS_DEFINITION(test_0001) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Add "Classify Sequences with MetaPhlAn2" element to the scene
    GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with MetaPhlAn2");

    //3. Click on the element
    GTUtilsWorkflowDesigner::click(os, "Classify Sequences with MetaPhlAn2");

    //Expected default parameters:
    //Input data, Input data format, Database, Number of threads,
    //Analysis type, Tax level, Bowtie2 output file, Output file
    QStringList currentParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(currentParameters.size() == 7,
                  QString("Unexpected number of default parameters, expected: 7, current: %1")
                          .arg(currentParameters.size()));
    QStringList defaultParameters = { INPUT_DATA, DATABASE, NUMBER_OF_THREADS,
                                      ANALYSIS_TYPE, TAX_LEVEL, BOWTIE2_OUTPUT_FILE, OUTPUT_FILE };
    foreach(const QString& par, defaultParameters) {
        CHECK_SET_ERR(currentParameters.contains(par), QString("The default parameter \"%1\" is missed").arg(par));
    }

    {//4. Check "Input data"
        //Expected: current "Input data" value is "SE reads or contigs", input table has one line
        QString inputDataValue = GTUtilsWorkflowDesigner::getParameter(os, INPUT_DATA);
        CHECK_SET_ERR(inputDataValue == INPUT_DATA_VALUES.first(),
                      QString("Unexpected \"Input data\" value, expected: SE reads or contigs, current: %1")
                              .arg(inputDataValue));

        QTableWidget* inputTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        int row = inputTable->rowCount();
        CHECK_SET_ERR(row == 1,
                      QString("Unexpected \"Input data\" row count, expected: 1, current: %1")
                              .arg(row));

        //5. Set "Input data" value on "PE reads"
        GTUtilsWorkflowDesigner::setParameter(os,
                                              INPUT_DATA,
                                              INPUT_DATA_VALUES.last(),
                                              GTUtilsWorkflowDesigner::comboValue);

        //Expected: input table has two lines
        inputTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        row = inputTable->rowCount();
        CHECK_SET_ERR(row == 2,
                      QString("Unexpected \"Input data\" row count, expected: 2, current: %1")
                              .arg(row));
    }

    {//7. Check "Database"
        //Expected: database path ends with 'data/ngs_classification/metaphlan2/mpa_v20_m200'
        QString databasePath = QDir::toNativeSeparators(GTUtilsWorkflowDesigner::getParameter(os, DATABASE));
        QString expectedEnd = QDir::toNativeSeparators("data/ngs_classification/metaphlan2/mpa_v20_m200");
        CHECK_SET_ERR(databasePath.endsWith(expectedEnd),
                      QString("Unexpected database path end: %1")
                              .arg(databasePath.right(expectedEnd.size())));
    }

    {//8. Check "Number of Threads"
        //Expected: expected optimal for the current OS threads num
        int threads = GTUtilsWorkflowDesigner::getParameter(os, NUMBER_OF_THREADS).toInt();
        int expectedThreads = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
        CHECK_SET_ERR(threads == expectedThreads,
                      QString("Unexpected threads num, expected: %1, current: %2")
                              .arg(expectedThreads)
                              .arg(threads));
    }

    {//9. Check "Analysis type"
        //Expected: Analysis type default value is "Relative abundance"
        QString analysisTypeDefault = GTUtilsWorkflowDesigner::getParameter(os, ANALYSIS_TYPE);
        CHECK_SET_ERR(analysisTypeDefault == ANALYSIS_TYPE_VALUES.first(),
                      QString("Unexpected Analysis type default value, expected: %1, current: %2")
                              .arg(ANALYSIS_TYPE_VALUES.first())
                              .arg(analysisTypeDefault));
    }

    {//10. Check "Tax level"
        //Expected: Tax level default value is "All"
        QString taxLevelDefault = GTUtilsWorkflowDesigner::getParameter(os, TAX_LEVEL);
        CHECK_SET_ERR(taxLevelDefault == TAX_LEVEL_VALUES.first(),
                      QString("Unexpected Tax level default value, expected: %1, current: %2")
                              .arg(TAX_LEVEL_VALUES.first())
                              .arg(taxLevelDefault));
    }

    {//11. Check "Bowtie2 output file"
        //Expected: Bowtie2 output file value is "Auto"
        QString bowtie2OutputFileDefault = GTUtilsWorkflowDesigner::getParameter(os, BOWTIE2_OUTPUT_FILE);
        CHECK_SET_ERR(bowtie2OutputFileDefault == DEFAULT_OUTPUT_VALUE,
                      QString("Unexpected Bowtie2 output file default value, expected: Auto, current: %1")
                              .arg(bowtie2OutputFileDefault));
    }

    {//12. Check "Output file"
        //Expected: Output file value is "Auto"
        QString outputFileDefault = GTUtilsWorkflowDesigner::getParameter(os, OUTPUT_FILE);
        CHECK_SET_ERR(outputFileDefault == DEFAULT_OUTPUT_VALUE,
                      QString("Unexpected Bowtie2 output file default value, expected: Auto, current: %1")
                              .arg(outputFileDefault));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Add "Classify Sequences with MetaPhlAn2" element to the scene
    GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with MetaPhlAn2");

    //3. Click on the element
    GTUtilsWorkflowDesigner::click(os, "Classify Sequences with MetaPhlAn2");

    {
        //4. Check if "Analysis type" parameter has 6 values:
        //Relative abundance, Relative abundance with reads statistics, Reads mapping
        //Clade profiles, Marker abundance table, Marker presence table
        QStringList analysisTypeValues = GTUtilsWorkflowDesigner::getComboBoxParameterValues(os, ANALYSIS_TYPE);
        CHECK_SET_ERR(analysisTypeValues.size() == ANALYSIS_TYPE_VALUES.size(),
            QString("Unexpected \"Analysis type\" values size, expected: %1, current: %2")
            .arg(ANALYSIS_TYPE_VALUES.size())
            .arg(analysisTypeValues.size()));

        foreach(const QString& value, ANALYSIS_TYPE_VALUES) {
            CHECK_SET_ERR(analysisTypeValues.contains(value),
                QString("Analysis type doesn't contain %1 value, but should be")
                .arg(value));
        }
    }

    {
        //5. Check if "Tax level" parameter has 8 values:
        //All, Kingdoms, Phyla, Classes, Orders, Families, Genera, Species
        QStringList taxLevelValues = GTUtilsWorkflowDesigner::getComboBoxParameterValues(os, TAX_LEVEL);
        CHECK_SET_ERR(taxLevelValues.size() == TAX_LEVEL_VALUES.size(),
            QString("Unexpected \"Tax level\" values size, expected: %1, current: %2")
            .arg(TAX_LEVEL_VALUES.size())
            .arg(taxLevelValues.size()));

        foreach(const QString& value, TAX_LEVEL_VALUES) {
            CHECK_SET_ERR(taxLevelValues.contains(value),
                QString("Tax level doesn't contain %1 value, but should be")
                .arg(value));
        }
    }

    //6. Set "Analysis type" value on "Marker abundance table"
    GTUtilsWorkflowDesigner::setParameter(os,
                                          ANALYSIS_TYPE,
                                          ANALYSIS_TYPE_VALUES[4],
                                          GTUtilsWorkflowDesigner::comboValue);

    {
        //7. Check if "Normalize by metagenome size" parameter has 2 values:
        //Skip, Normalize
        QStringList normalizeValues = GTUtilsWorkflowDesigner::getComboBoxParameterValues(os, NORMALIZE_BY_METAGENOME_SIZE);
        CHECK_SET_ERR(normalizeValues.size() == NORMALIZE_BY_METAGENOME_SIZE_VALUES.size(),
            QString("Unexpected \"Normalize by metagenome size\" values size, expected: %1, current: %2")
                    .arg(NORMALIZE_BY_METAGENOME_SIZE_VALUES.size())
                    .arg(normalizeValues.size()));

        foreach(const QString& value, NORMALIZE_BY_METAGENOME_SIZE_VALUES) {
            CHECK_SET_ERR(normalizeValues.contains(value),
                QString("Normalize by metagenome size doesn't contain %1 value, but should be")
                .arg(value));
        }
    }

    {
        //8. Check if "Input data" parameter has 2 values:
        //SE reads or contigs, PE reads
        QStringList inputDataValues = GTUtilsWorkflowDesigner::getComboBoxParameterValues(os, INPUT_DATA);
        CHECK_SET_ERR(inputDataValues.size() == INPUT_DATA_VALUES.size(),
            QString("Unexpected \"Input data\" values size, expected: %1, current: %2")
                    .arg(INPUT_DATA_VALUES.size())
                    .arg(inputDataValues.size()));

        foreach(const QString& value, INPUT_DATA_VALUES) {
            CHECK_SET_ERR(inputDataValues.contains(value),
                QString("Input data doesn't contain %1 value, but should be")
                        .arg(value));
        }
    }
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Add "Classify Sequences with MetaPhlAn2" element to the scene
    GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with MetaPhlAn2");

    //3. Click on the element
    GTUtilsWorkflowDesigner::click(os, "Classify Sequences with MetaPhlAn2");

    //4. Clear Database value
    GTUtilsWorkflowDesigner::setParameter(os, DATABASE, QString(), GTUtilsWorkflowDesigner::lineEditWithFileSelector);

    //5. Click to another parameter to change focus
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected:: Database has value "Required"
    QString databaseValue = GTUtilsWorkflowDesigner::getParameter(os, DATABASE);
    CHECK_SET_ERR(databaseValue == "Required",
                  QString("Unexpected Database value, expected: \"Required\", current: \"%1\"")
                          .arg(databaseValue));
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Add "Classify Sequences with MetaPhlAn2" element to the scene
    GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with MetaPhlAn2");

    //3. Click on the element
    GTUtilsWorkflowDesigner::click(os, "Classify Sequences with MetaPhlAn2");

    QStringList allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);

    //Expected: Tax value parameter is represented bu default
    CHECK_SET_ERR(allParameters.contains(TAX_LEVEL), "Tax level parameter is'nt represented by default");

    //4. Set "Analysis type" value to "Relative abundance with reads statistics"
    GTUtilsWorkflowDesigner::setParameter(os, ANALYSIS_TYPE, ANALYSIS_TYPE_VALUES[1], GTUtilsWorkflowDesigner::comboValue);

    //Change focus to avoid problems
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected: Tax value parameter is represented
    allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(allParameters.contains(TAX_LEVEL), "Tax level parameter isn't represented");

    //5. Set "Analysis type" value to "Reads mapping"
    GTUtilsWorkflowDesigner::setParameter(os, ANALYSIS_TYPE, ANALYSIS_TYPE_VALUES[2], GTUtilsWorkflowDesigner::comboValue);

    //Change focus to avoid problems
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected: 7 parameters are represented
    allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(allParameters.size() == 6,
                  QString("Unexpected parameters number, expected: 6, current: %1")
                          .arg(allParameters.size()));

    //6. Set "Analysis type" value to "Reads mapping"
    GTUtilsWorkflowDesigner::setParameter(os, ANALYSIS_TYPE, ANALYSIS_TYPE_VALUES[3], GTUtilsWorkflowDesigner::comboValue);

    //Change focus to avoid problems
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected: 7 parameters are represented
    allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(allParameters.size() == 6,
                  QString("Unexpected parameters number, expected: 6, current: %1")
                          .arg(allParameters.size()));

    //7. Set "Analysis type" value to "Marker abundance table"
    GTUtilsWorkflowDesigner::setParameter(os, ANALYSIS_TYPE, ANALYSIS_TYPE_VALUES[4], GTUtilsWorkflowDesigner::comboValue);

    //Change focus to avoid problems
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected: Normalize by metagenome size parameter is represented
    allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(allParameters.contains(NORMALIZE_BY_METAGENOME_SIZE), "Normalize by metagenome size parameter isn't represented");

    //8. Set "Analysis type" value to "Marker presence table"
    GTUtilsWorkflowDesigner::setParameter(os, ANALYSIS_TYPE, ANALYSIS_TYPE_VALUES[5], GTUtilsWorkflowDesigner::comboValue);

    //Change focus to avoid problems
    GTUtilsWorkflowDesigner::setParameter(os, INPUT_DATA, INPUT_DATA_VALUES.first(), GTUtilsWorkflowDesigner::comboValue);

    //Expected: Presence threshold parameter is represented
    allParameters = GTUtilsWorkflowDesigner::getAllParameters(os);
    CHECK_SET_ERR(allParameters.contains(PRESENCE_THRESHOLD), "Presence threshold parameter isn't represented");
}

}

} // namespace U2
