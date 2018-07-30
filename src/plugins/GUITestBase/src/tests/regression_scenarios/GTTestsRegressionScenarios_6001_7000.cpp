/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include <QRadioButton>
#include <QSpinBox>
#include <QTableWidget>

#include <base_dialogs/MessageBoxFiller.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTRadioButton.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>

#include <U2View/DetView.h>

#include "GTTestsRegressionScenarios_6001_7000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsBookmarksTreeView.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMcaEditorStatusWidget.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelMca.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"

#include "runnables/ugene/corelibs/U2Gui/EditSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportAPRFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "../../workflow_designer/src/WorkflowViewItems.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_6031) {
    //1. Open samples/APR/gyrA.apr in read-only mode
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, true));
    GTFileDialog::openFile(os, dataDir + "samples/APR/gyrA.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: there is no prefix \27: before name of sequences
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    foreach(const QString& name, nameList) {
        CHECK_SET_ERR(!name.startsWith("\27"), QString("Unexpected start of the name"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_6033) {
    //    1. Open 'human_T1.fa'
    GTFileDialog::openFile(os, dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select the whole sequence.
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 199950);

    //3. Copy translation.
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);

    //4. Open the Project View, paste the data.
    if (!GTUtilsProjectTreeView::isVisible(os)) {
        GTUtilsProjectTreeView::openView(os);
    }

    GTUtilsProjectTreeView::click(os, "human_T1.fa");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool correct = false;
    foreach(const QString& name, GTUtilsProjectTreeView::getDocuments(os).keys()) {
        if (name.contains("clipboard") && name.contains(".seq")) {
            correct = true;
            break;
        }
    }

    CHECK_SET_ERR(correct, "Incorrect paste operation");
}

GUI_TEST_CLASS_DEFINITION(test_6038) {
//    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

//    2. Add 'Improve Reads with Trimmomatic', 'Classify Sequences with CLARK', 'Classify Sequences with Kraken' and 'Filter by Classification' elements to the scene.
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    const QString clarkName = "Classify Sequences with CLARK";
    const QString krakenName = "Classify Sequences with Kraken";
    const QString filterName = "Filter by Classification";

    WorkflowProcessItem *trimmomaticElement = GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);
    WorkflowProcessItem *clarkElement = GTUtilsWorkflowDesigner::addElement(os, clarkName);
    WorkflowProcessItem *krakenElement = GTUtilsWorkflowDesigner::addElement(os, krakenName);
    WorkflowProcessItem *filterElement = GTUtilsWorkflowDesigner::addElement(os, filterName);

//    3. Each element has 'Input data' parameter. Set it to 'PE reads'.
//    Expected state: 'Classify Sequences with Kraken' and 'Classify Sequences with CLARK' elements have two input slots ('Input URL 1' and 'Input URL 2') and some output slots;
//                    'Improve Reads with Trimmomatic' element has two input slots ('Input FASTQ URL 1' and 'Input FASTQ URL 2') and two output slots ('Output FASTQ URL 1' and 'Output FASTQ URL 2');
//                    'Filter by Classification' element has three input slots (two of them are 'Input URL 1' and 'Input URL 2'), and two output slots ('Output URL 1' and 'Output URL 2').
    {
        GTUtilsWorkflowDesigner::click(os, trimmomaticElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "PE reads", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input FASTQ URL 1"), QString("'Input FASTQ URL 1' slot not found in element '%1'").arg(trimmomaticName));
        CHECK_SET_ERR(inputSlotsNames.contains("Input FASTQ URL 2"), QString("'Input FASTQ URL 2' slot not found in element '%1'").arg(trimmomaticName));

        QTableWidget *outputPortTable = GTUtilsWorkflowDesigner::getOutputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != outputPortTable, "outputPortTable is NULL");

        QStringList outputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, outputPortTable); i++) {
            outputSlotsNames << GTTableView::data(os, outputPortTable, i, 0);
        }

        CHECK_SET_ERR(outputSlotsNames.contains("Output FASTQ URL 1 (by Improve Reads with Trimmomatic)"), QString("'Output FASTQ URL 1 (by Improve Reads with Trimmomatic)' slot not found in element '%1'").arg(trimmomaticName));
        CHECK_SET_ERR(outputSlotsNames.contains("Output FASTQ URL 2 (by Improve Reads with Trimmomatic)"), QString("'Output FASTQ URL 2 (by Improve Reads with Trimmomatic)' slot not found in element '%1'").arg(trimmomaticName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, clarkElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "PE reads", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(clarkName));
        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot not found in element '%1'").arg(clarkName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, krakenElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "PE reads", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(krakenName));
        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot not found in element '%1'").arg(krakenName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, filterElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "PE reads", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(filterName));
        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot not found in element '%1'").arg(filterName));

        QTableWidget *outputPortTable = GTUtilsWorkflowDesigner::getOutputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != outputPortTable, "outputPortTable is NULL");

        QStringList outputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, outputPortTable); i++) {
            outputSlotsNames << GTTableView::data(os, outputPortTable, i, 0);
        }

        CHECK_SET_ERR(outputSlotsNames.contains("Output URL 1 (by Filter by Classification)"), QString("'Output URL 1 (by Filter by Classification)' slot not found in element '%1'").arg(filterName));
        CHECK_SET_ERR(outputSlotsNames.contains("Output URL 2 (by Filter by Classification)"), QString("'Output URL 2 (by Filter by Classification)' slot not found in element '%1'").arg(filterName));
    }

//    4. Set 'Input data' parameter in each element to 'SE reads or contigs' ('SE reads' in 'Improve Reads with Trimmomatic' element).
//    Expected state: 'Classify Sequences with Kraken' and 'Classify Sequences with CLARK' elements have one input slot ('Input URL 1') and some output slots;
//                    'Improve Reads with Trimmomatic' element has one input slot ('Input FASTQ URL 1') and one output slot ('Output FASTQ URL 1');
//                    'Filter by Classification' element has two input slots (one of them is 'Input URL 1'), and one output slot ('Output URL 1').
    {
        GTUtilsWorkflowDesigner::click(os, trimmomaticElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "SE reads", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input FASTQ URL 1"), QString("'Input FASTQ URL 1' slot not found in element '%1'").arg(trimmomaticName));
        CHECK_SET_ERR(!inputSlotsNames.contains("Input FASTQ URL 2"), QString("'Input FASTQ URL 2' slot unexpectedly found in element '%1'").arg(trimmomaticName));

        QTableWidget *outputPortTable = GTUtilsWorkflowDesigner::getOutputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != outputPortTable, "outputPortTable is NULL");

        QStringList outputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, outputPortTable); i++) {
            outputSlotsNames << GTTableView::data(os, outputPortTable, i, 0);
        }

        CHECK_SET_ERR(outputSlotsNames.contains("Output FASTQ URL 1 (by Improve Reads with Trimmomatic)"), QString("'Output FASTQ URL 1 (by Improve Reads with Trimmomatic)' slot not found in element '%1'").arg(trimmomaticName));
        CHECK_SET_ERR(!outputSlotsNames.contains("Output FASTQ URL 2 (by Improve Reads with Trimmomatic)"), QString("'Output FASTQ URL 2 (by Improve Reads with Trimmomatic)' slot unexpectedly found in element '%1'").arg(trimmomaticName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, clarkElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "SE reads or contigs", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(clarkName));
        CHECK_SET_ERR(!inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot unexpectedly found in element '%1'").arg(clarkName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, krakenElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "SE reads or contigs", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(krakenName));
        CHECK_SET_ERR(!inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot unexpectedly found in element '%1'").arg(krakenName));
    }

    {
        GTUtilsWorkflowDesigner::click(os, filterElement);
        GTUtilsWorkflowDesigner::setParameter(os, "Input data", "SE reads or contigs", GTUtilsWorkflowDesigner::comboValue);

        QTableWidget *inputPortTable = GTUtilsWorkflowDesigner::getInputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != inputPortTable, "inputPortTable is NULL");

        QStringList inputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, inputPortTable); i++) {
            inputSlotsNames << GTTableView::data(os, inputPortTable, i, 0);
        }

        CHECK_SET_ERR(inputSlotsNames.contains("Input URL 1"), QString("'Input URL 1' slot not found in element '%1'").arg(filterName));
        CHECK_SET_ERR(!inputSlotsNames.contains("Input URL 2"), QString("'Input URL 2' slot unexpectedly found in element '%1'").arg(filterName));

        QTableWidget *outputPortTable = GTUtilsWorkflowDesigner::getOutputPortsTable(os, 0);
        CHECK_SET_ERR(NULL != outputPortTable, "outputPortTable is NULL");

        QStringList outputSlotsNames;
        for (int i = 0; i < GTTableView::rowCount(os, outputPortTable); i++) {
            outputSlotsNames << GTTableView::data(os, outputPortTable, i, 0);
        }

        CHECK_SET_ERR(outputSlotsNames.contains("Output URL 1 (by Filter by Classification)"), QString("'Output URL 1 (by Filter by Classification)' slot not found in element '%1'").arg(filterName));
        CHECK_SET_ERR(!outputSlotsNames.contains("Output URL 2 (by Filter by Classification)"), QString("'Output URL 2 (by Filter by Classification)' slot unexpectedly found in element '%1'").arg(filterName));
    }

//    5. Click 'Validate workflow' button on the toolbar.
    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsWorkflowDesigner::validateWorkflow(os);

//    Expected state: there could be errors, but there are neither errors not warnings about not connected slots.
    QSet<QString> acceptableErrors = QSet<QString>() << QString("%1: %1 : The mandatory \"Input URL 1\" slot is not connected.").arg(clarkName)
                                                     << QString("%1: External tool \"CLARK\" is not set. You can set it in Settings -> Preferences -> External Tools").arg(clarkName)
                                                     << QString("%1: External tool \"CLARK-l\" is not set. You can set it in Settings -> Preferences -> External Tools").arg(clarkName)
                                                     << QString("%1: %1 : The mandatory \"Input URL 1\" slot is not connected.").arg(krakenName)
                                                     << QString("%1: External tool \"kraken\" is not set. You can set it in Settings -> Preferences -> External Tools").arg(krakenName)
                                                     << QString("%1: %1 : The mandatory \"Input URL 1\" slot is not connected.").arg(filterName)
                                                     << QString("%1: %1 : The mandatory \"Input FASTQ URL 1\" slot is not connected.").arg(trimmomaticName)
                                                     << QString("%1: External tool \"Trimmomatic\" is not set. You can set it in Settings -> Preferences -> External Tools").arg(trimmomaticName)
                                                     << QString("%1: Required parameter is not set: Database").arg(clarkName)
                                                     << QString("%1: Required parameter is not set: Database").arg(krakenName)
                                                     << QString("%1: The database folder \"\" doesn't exist.").arg(krakenName)
                                                     << QString("%1: Required parameter is not set: Trimming steps").arg(trimmomaticName);

    QSet<QString> actualErrors = GTUtilsWorkflowDesigner::getErrors(os).toSet();
    CHECK_SET_ERR(acceptableErrors.contains(actualErrors), "There are unexpected errors after the workflow validation");
}

GUI_TEST_CLASS_DEFINITION(test_6043) {
//    1. Open "_common_data/ugenedb/sec1_9_ugenedb.ugenedb".
//    Expected state: the assembly is successfully opened, the coverage calculation finished, UGENE doens't crash
    const QString filePath = sandBoxDir + "test_6043.ugenedb";
    GTFile::copy(os, testDir + "_common_data/ugenedb/sec1_9_ugenedb.ugenedb", filePath);

    GTFileDialog::openFile(os, filePath);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const bool assemblyExists = GTUtilsProjectTreeView::checkItem(os, "sec1_and_others");
    CHECK_SET_ERR(assemblyExists, "Assembly object is not found in the project view");
}

GUI_TEST_CLASS_DEFINITION(test_6047) {
    //1. Open and convert APR file
    GTUtilsDialog::waitForDialog(os, new ImportAPRFileFiller(os, false, sandBoxDir + "test_6047", "MSF"));
    GTFileDialog::openFile(os, testDir + "_common_data/apr/HCVtest.apr");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Check msa length and number of sequences
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getLength(os) == 488, "Unexpected length of msa");
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::getNameList(os).size() == 231, "Unexpected quantaty of sequences");
}

GUI_TEST_CLASS_DEFINITION(test_6058_1) {
    //1. Open file human_t1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Check following items in sequence view translations menu
    //"31. Blastocrithidia Nuclear"
    //"28. Condylostoma Nuclear"
    //"30. Peritrich Nuclear"
    //"27. Karyorelict Nuclear"
    //"25. Candidate Division SR1 and Gracilibacteria Code"
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTWidget::click(os, GTWidget::findWidget(os, "AminoToolbarButton", GTWidget::findWidget(os, "ADV_single_sequence_widget_0")));
    QMenu *menu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
    QStringList actionText;
    foreach(QAction *a, menu->actions()) {
        actionText.append(a->text());
    }
    CHECK_SET_ERR(actionText.contains("31. Blastocrithidia Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("28. Condylostoma Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("30. Peritrich Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("27. Karyorelict Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("25. Candidate Division SR1 and Gracilibacteria Code"), "expected translation not found");

    //just for closing popup menu
    GTMenu::clickMenuItemByName(os, menu, QStringList() << "14. The Alternative Flatworm Mitochondrial Code");
}

GUI_TEST_CLASS_DEFINITION(test_6058_2) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Check "30. Peritrich Nuclear" "Genetic code" parameter option in "Classify Sequences with DIAMOND" WD element
    WorkflowProcessItem *diamondElement = GTUtilsWorkflowDesigner::addElement(os, "Classify Sequences with DIAMOND", true);
    GTUtilsWorkflowDesigner::click(os, diamondElement);
    GTUtilsWorkflowDesigner::setParameter(os, "Genetic code", "30. Peritrich Nuclear", GTUtilsWorkflowDesigner::comboValue);

    //3. Check "27. Karyorelict Nuclear" "Genetic code" parameter option in "ORF Marker" WD element
    WorkflowProcessItem *orfmarkerElement = GTUtilsWorkflowDesigner::addElement(os, "ORF Marker", true);
    GTUtilsWorkflowDesigner::click(os, orfmarkerElement);
    GTUtilsWorkflowDesigner::setParameter(os, "Genetic code", "27. Karyorelict Nuclear", GTUtilsWorkflowDesigner::comboValue);
}

GUI_TEST_CLASS_DEFINITION(test_6062) {
    class InnerOs : public GUITestOpStatus {
    public:
        void setError(const QString & err) {
            innerError = err;
        }

        QString getError() const {
            return innerError;
        }

        bool hasError() const {
            return !innerError.isEmpty();
        }

        void reset() {
            innerError.clear();
        }

    private:
        QString innerError;
    };

//    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Add 'Map Reads with BWA' element to the scene.
    WorkflowProcessItem *bwaElement = GTUtilsWorkflowDesigner::addElement(os, "Map Reads with BWA", true);

//    3. Click to the element.
    GTUtilsWorkflowDesigner::click(os, bwaElement);

//    Expected state: 'Library' attribute has value 'Single-end'; there is one table in 'Input data' widget, which contains information about input ports.
    const QString actualAttributeValue = GTUtilsWorkflowDesigner::getParameter(os, "Library");
    const QString expectedAttributeValue = "Single-end";
    CHECK_SET_ERR(expectedAttributeValue == actualAttributeValue,
                  QString("An unexpected default value of 'Library' attribute: expected '%1', got '%2'")
                  .arg(expectedAttributeValue).arg(actualAttributeValue));

    InnerOs innerOs;

    QTableWidget *inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 0);
    CHECK_OP_SET_ERR(innerOs, "Table for the first input port not found");
    CHECK_SET_ERR(NULL != inputPortTable1, "inputPortTable1 is NULL");

    QTableWidget *inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 1);
    CHECK_SET_ERR(innerOs.hasError(), "Table for the second input port unexpectedly found");
    CHECK_SET_ERR(NULL == inputPortTable2, "Table for the second input port unexpectedly found");

    innerOs.reset();

//    4. Set 'Library' attribute value to 'Paired-end'.
    GTUtilsWorkflowDesigner::setParameter(os, "Library", "Paired-end", GTUtilsWorkflowDesigner::comboValue);

//    Expected state: there are two tables in 'Input data' widget.
    inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 0);
    CHECK_OP_SET_ERR(innerOs, "Table for the first input port not found");
    CHECK_SET_ERR(NULL != inputPortTable1, "inputPortTable1 is NULL");

    inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 1);
    CHECK_OP_SET_ERR(innerOs, "Table for the second input port not found");
    CHECK_SET_ERR(NULL != inputPortTable2, "Table for the second input port not found");

//    4. Set 'Library' attribute value to 'Single-end'.
    GTUtilsWorkflowDesigner::clickParameter(os, "Output folder");
    GTUtilsWorkflowDesigner::setParameter(os, "Library", "Single-end", GTUtilsWorkflowDesigner::comboValue);

//    Expected state: there is one table in 'Input data' widget.
    inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 0);
    CHECK_OP_SET_ERR(innerOs, "Table for the first input port not found");
    CHECK_SET_ERR(NULL != inputPortTable1, "inputPortTable1 is NULL");

    inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(innerOs, 1);
    CHECK_SET_ERR(innerOs.hasError(), "Table for the second input port unexpectedly found");
    CHECK_SET_ERR(NULL == inputPortTable2, "Table for the second input port unexpectedly found");
}

GUI_TEST_CLASS_DEFINITION(test_6066) {
//    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Select "Edit" -> "Annotations settings on sequence editing..." menu item in the Details View context menu.
//    3. Choose "Split (separate annotations parts)" and press "OK".
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Edit" << "Annotations settings on sequence editing..."));
    GTUtilsDialog::waitForDialog(os, new EditSettingsDialogFiller(os, EditSettingsDialogFiller::SplitSeparateAnnotationParts, false));
    GTWidget::click(os, GTUtilsSequenceView::getDetViewByNumber(os), Qt::RightButton);

//    4. Turn on the editing mode.
    GTUtilsSequenceView::enableEditingMode(os);

//    5. Set cursor after position 60.
    GTUtilsSequenceView::setCursor(os, 60);

//    6. Click Space key.
    GTKeyboardDriver::keyClick(Qt::Key_Space);

//    Expected state: a gap is inserted, the annotation is split.
    // Do not check it here, to avoid view state changing

//    7. Doubleclick the first part if the split annotation and click Delete key.
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2, 0, true);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();

//    Expected state: the annotation is removed from the Details View and from the Annotations Tree View.
    // Do not check it here, to avoid view state changing

//    8. Doubleclick the second part of the split annotation.
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 3, 0, true);

//    Expected state: UGENE doesn't crash.
}

GUI_TEST_CLASS_DEFINITION(test_6071) {
    //1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select 2-th CDS annotation in the Zoom view
    GTUtilsSequenceView::clickAnnotationPan(os, "CDS", 2970);

    //3. Scroll to the 3874 coordinate.
    GTUtilsSequenceView::goToPosition(os, 3874);

    DetView* dw = GTUtilsSequenceView::getDetViewByNumber(os);
    const U2Region firstVisibleRange = dw->getVisibleRange();

    //4. Click on 2-th CDS join annotation
    GTUtilsSequenceView::clickAnnotationDet(os, "CDS", 3412);

    //Expected: visible range was not changed
    const U2Region secondVisibleRange = dw->getVisibleRange();
    CHECK_SET_ERR(firstVisibleRange == secondVisibleRange, "Visible range was changed after clicking on the annotation");
}

GUI_TEST_CLASS_DEFINITION(test_6078) {
    //1. Open human_T1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Select 1 - 10 chars
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 10);
    GTKeyboardUtils::copy(os);

    //3. Enable edit mode
    GTUtilsSequenceView::enableEditingMode(os);

    //4. Set the cursor to the 5-th pos
    GTUtilsSequenceView::setCursor(os, 5);

    //5. Press paste
    GTKeyboardUtils::paste(os);
    GTGlobals::sleep();

    //Expected: cursor on the 15-th pos
    const qint64 pos = GTUtilsSequenceView::getCursor(os);
    CHECK_SET_ERR(pos == 15, QString("Incorrect cursor position, expected: 15, current: %1").arg(pos));
}

GUI_TEST_CLASS_DEFINITION(test_6083) {
    //    1. open document samples/CLUSTALW/COI.aln
        GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");
    //    2. Select first sequence
        GTUtilsMSAEditorSequenceArea::click(os,QPoint(0,0));
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os,QStringList()<<MSAE_MENU_EXPORT<<"Save sequence",GTGlobals::UseKey));
        Runnable* r = new ExportSelectedSequenceFromAlignment(os,testDir + "_common_data/scenarios/sandbox/",ExportSelectedSequenceFromAlignment::Ugene_db,true);
        GTUtilsDialog::waitForDialog(os, r);

        GTMenu::showContextMenu(os,GTUtilsMdi::activeWindow(os));
        GTGlobals::sleep();
        GTUtilsTaskTreeView::waitTaskFinished(os);

        GTUtilsProjectTreeView::click(os, "Phaneroptera_falcata.ugenedb");
        GTKeyboardDriver::keyClick( Qt::Key_Delete);
        GTGlobals::sleep();
}


GUI_TEST_CLASS_DEFINITION(test_6087) {
    //1. Open  samples/MMDB/1CRN.prt
    GTFileDialog::openFile(os, dataDir + "samples/MMDB/1CRN.prt");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Try to select a region.
    GTUtilsSequenceView::selectSequenceRegion(os, 10, 20);

    //Expected: ugene was not crashed
    QVector<U2Region> regions = GTUtilsSequenceView::getSelection(os);
    CHECK_SET_ERR(regions.size() == 1, "Unexpected selection");
}

GUI_TEST_CLASS_DEFINITION(test_6102) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //    2) Run Smith-waterman search using:
        class Scenario : public CustomScenario {
            void run(HI::GUITestOpStatus &os) {
                QWidget *dialog = QApplication::activeModalWidget();
                CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");
                GTTextEdit::setText(os, GTWidget::findExactWidget<QTextEdit *>(os, "teditPattern", dialog), "RPHP*VAS*LK*RHFARHGKIHN*E*KSSDQGQ");

                GTRadioButton::click(os, "radioTranslation", dialog);

                GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);
                //    3. Open tab "Input and output"
                            GTTabWidget::setCurrentIndex(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget", dialog), 1);

                //    4. Chose in the combobox "Multiple alignment"
                            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "resultViewVariants", dialog), "Multiple alignment");
                GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            }
        };

        GTUtilsDialog::waitForDialog(os, new SmithWatermanDialogFiller(os, new Scenario));
        GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Find pattern [Smith-Waterman]...", GTGlobals::UseMouse);
        GTUtilsTaskTreeView::waitTaskFinished(os);

        GTUtilsProjectTreeView::doubleClickItem(os, "P1_NC_1.aln");
        GTUtilsTaskTreeView::waitTaskFinished(os);

        const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor(os)->getMaObject()->getAlphabet()->isAmino();
        CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino");
}

GUI_TEST_CLASS_DEFINITION(test_6118) {
    //1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //2. Make scheme "Read FASTQ File with SE Reads" -> "Improve Reads with Trimmomatic"
    const QString readSEName = "Read FASTQ File with SE Reads";
    const QString trimmomaticName = "Improve Reads with Trimmomatic";

    WorkflowProcessItem *readSEElement = GTUtilsWorkflowDesigner::addElement(os, readSEName);
    WorkflowProcessItem *trimmomaticElement = GTUtilsWorkflowDesigner::addElement(os, trimmomaticName);
    GTUtilsWorkflowDesigner::connect(os, readSEElement, trimmomaticElement);


    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            //3. Add two "ILLUMINACLIP" steps with adapters with similar filenames located in different directories to Trimmomatic worker.
            GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
            QMenu *menu = qobject_cast<QMenu*>(GTWidget::findWidget(os, "stepsMenu"));
            GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTGlobals::sleep(500);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/regression/6118/TruSeq3-SE.fa"));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowse"));

            GTWidget::click(os, GTWidget::findWidget(os, "buttonAdd"));
            menu = qobject_cast<QMenu*>(GTWidget::findWidget(os, "stepsMenu"));
            GTMenu::clickMenuItemByName(os, menu, QStringList() << "ILLUMINACLIP");
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
            GTGlobals::sleep(500);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/regression/6118/deeperDir/TruSeq3-SE.fa"));
            GTWidget::click(os, GTWidget::findWidget(os, "tbBrowse"));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsWorkflowDesigner::click(os, readSEElement);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/FASTQ/eas.fastq");

    GTUtilsWorkflowDesigner::click(os, trimmomaticElement);
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new Scenario()));
    QTableView* table = GTWidget::findExactWidget<QTableView*>(os, "table");
    GTMouseDriver::moveTo(GTTableView::getCellPoint(os, table, 1, 1));
    GTMouseDriver::click();
    GTGlobals::sleep(500);
    GTWidget::click(os, GTWidget::findButtonByText(os, "...", table));
    GTGlobals::sleep(500);

    //4. Run this workflow.
    //Expected state : there are no errors during execution.
    GTLogTracer l;
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!l.hasError(), "Errors in the log");
}

GUI_TEST_CLASS_DEFINITION(test_6136) {
    // 1. Open "test/scenarios/_common_data/genbank/target_gene_new.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/target_gene_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open "In Silico PCR" tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    //3. Fill values:
    //Forward: primer - TTTGGATCCAGCATCACCATCACCATCACGATCAAATAGAAGCAATG, mismathches - 27
    //Reverse: primer - AAACCTAGGTACGTAGTGGTAGTGGTAGTGCTAGTTTATCTTCGTTAC, mismathches - 27
    GTUtilsOptionPanelSequenceView::setForwardPrimer(os, "TTTGGATCCAGCATCACCATCACCATCACGATCAAATAGAAGCAATG");
    GTUtilsOptionPanelSequenceView::setForwardPrimerMismatches(os, 27);
    GTUtilsOptionPanelSequenceView::setReversePrimer(os, "AAACCTAGGTACGTAGTGGTAGTGGTAGTGCTAGTTTATCTTCGTTAC");
    GTUtilsOptionPanelSequenceView::setReversePrimerMismatches(os, 27);

    //4. Press "find product(s)"
    GTUtilsOptionPanelSequenceView::pressFindProducts(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: there is one product was found
    const int count = GTUtilsOptionPanelSequenceView::productsCount(os);
    CHECK_SET_ERR(count == 1, QString("Unexpected products quantity, expected: 1, current: %1").arg(count));

    //5. Press "Extract product"
    GTUtilsOptionPanelSequenceView::pressExtractProduct(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected: Sequence length = 423
    const int length = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(length == 423, QString("Unexpected sequence length, expected: 423, current: %1").arg(length));

    //Check annotaions
    foreach(const int i, QList<int>() << 30 << 376) {
        GTUtilsSequenceView::clickAnnotationPan(os, "Misc. Feature", i, 0, true);
        QVector<U2Region> sel = GTUtilsSequenceView::getSelection(os);
        CHECK_SET_ERR(sel.size() == 1, QString("Unexpected selection annotation regions, expected: 1, current: %1").arg(sel.size()));
    }

    foreach(const int i, QList<int>() << 1 << 376) {
        GTUtilsSequenceView::clickAnnotationPan(os, "misc_feature", i, 0, true);
        QVector<U2Region> sel = GTUtilsSequenceView::getSelection(os);
        CHECK_SET_ERR(sel.size() == 1, QString("Unexpected selection primer annotation regions, expected: 1, current: %1").arg(sel.size()));
    }
}

} // namespace GUITest_regression_scenarios

} // namespace U2
