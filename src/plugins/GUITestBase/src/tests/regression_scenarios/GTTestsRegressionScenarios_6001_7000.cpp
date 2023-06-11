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

#include <api/GTUtils.h>
#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTScrollBar.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTUtilsText.h>

#include <QApplication>
#include <QDir>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTableWidget>
#include <QWizard>

#include <U2Core/HttpFileAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/DetView.h>
#include <U2View/McaEditorReferenceArea.h>

#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTTestsRegressionScenarios_6001_7000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMcaEditor.h"
#include "GTUtilsMcaEditorSequenceArea.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelMca.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPcr.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTMainWindow.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/AppSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditAnnotationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditSettingsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/FindRepeatsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportAPRFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/BuildTreeDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/ExtractSelectedAsMSADialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/utils_smith_waterman/SmithWatermanDialogBaseFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/TrimmomaticDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithCommandLineToolFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_6008) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click to the "Tettigonia_viridissima" row name in the name list.
    GTUtilsMsaEditor::clickSequence(9);

    // Click to the second base of the second row in the sequence area.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(1, 1));

    // Expected state: "Isophya_altaica_EF540820" is selected in the name list.
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Isophya_altaica_EF540820")),
                  "Expected sequence is not selected");

    // Click to the name list with the right mouse button to set the focus on it.
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi", Qt::RightButton);

    // Click Escape key to close the context menu.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Press the down arrow key.
    GTKeyboardDriver::keyClick(Qt::Key_Down);

    // Expected state: "Bicolorana_bicolor_EF540830" is selected in the name list. The whole row data is selected.
    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceSelected(QString("Bicolorana_bicolor_EF540830")),
                  "Expected sequence is not selected");
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 2, 604, 1));
}

GUI_TEST_CLASS_DEFINITION(test_6031) {
    // 1. Open samples/APR/gyrA.apr in read-only mode
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(true));
    GTUtilsProject::openFile(dataDir + "samples/APR/gyrA.apr");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there is no prefix \27: before name of sequences
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    foreach (const QString& name, nameList) {
        CHECK_SET_ERR(!name.startsWith("\27"), QString("Unexpected start of the name"));
    }
}

GUI_TEST_CLASS_DEFINITION(test_6033) {
    //    1. Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the whole sequence.
    GTUtilsSequenceView::selectSequenceRegion(1, 199950);

    // 3. Copy translation.
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);

    // 4. Open the Project View, paste the data.
    if (!GTUtilsProjectTreeView::isVisible()) {
        GTUtilsProjectTreeView::openView();
    }

    GTUtilsProjectTreeView::click("human_T1.fa");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    bool correct = false;
    QList<QString> documentKeys = GTUtilsProjectTreeView::getDocuments().keys();
    for (const QString& name : qAsConst(documentKeys)) {
        if (name.contains("clipboard") && name.contains(".seq")) {
            correct = true;
            break;
        }
    }

    CHECK_SET_ERR(correct, "Incorrect paste operation");
}

GUI_TEST_CLASS_DEFINITION(test_6043) {
    //    1. Open "_common_data/ugenedb/sec1_9_ugenedb.ugenedb".
    //    Expected state: the assembly is successfully opened, the coverage calculation finished, UGENE doesn't crash
    QString filePath = sandBoxDir + "test_6043.ugenedb";
    GTFile::copy(testDir + "_common_data/ugenedb/sec1_9_ugenedb.ugenedb", filePath);

    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    bool assemblyExists = GTUtilsProjectTreeView::checkItem("sec1_and_others");
    CHECK_SET_ERR(assemblyExists, "Assembly object is not found in the project view");
}

GUI_TEST_CLASS_DEFINITION(test_6045) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select annotation comment and press f2.
    // Expected state: message box about not allowed editing is appear.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, tr("Editing of \"comment\" annotation is not allowed!")));
    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem("comment");
    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_6047) {
    // 1. Open and convert APR file
    GTUtilsDialog::waitForDialog(new ImportAPRFileFiller(false, sandBoxDir + "test_6047", "MSF"));
    GTFileDialog::openFileWithDialog(testDir + "_common_data/apr/", "DNA.apr");
    GTUtilsDialog::checkNoActiveWaiters();

    // Check msa length and number of sequences
    int columns = GTUtilsMSAEditorSequenceArea::getLength();
    CHECK_SET_ERR(columns == 2139, "Unexpected length of msa: " + QString::number(columns));

    int rows = GTUtilsMSAEditorSequenceArea::getNameList().size();
    CHECK_SET_ERR(rows == 9, "Unexpected number of sequences in msa: " + QString::number(rows));
}

GUI_TEST_CLASS_DEFINITION(test_6058) {
    // 1. Open file human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Check following items in sequence view translations menu
    //"31. Blastocrithidia Nuclear"
    //"28. Condylostoma Nuclear"
    //"30. Peritrich Nuclear"
    //"27. Karyorelict Nuclear"
    //"25. Candidate Division SR1 and Gracilibacteria Code"
    GTWidget::click(GTWidget::findWidget("ADV_single_sequence_widget_0"));
    GTWidget::click(GTWidget::findWidget("AminoToolbarButton", GTWidget::findWidget("ADV_single_sequence_widget_0")));
    auto menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    QStringList actionText;
    foreach (QAction* a, menu->actions()) {
        actionText.append(a->text());
    }
    CHECK_SET_ERR(actionText.contains("31. Blastocrithidia Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("28. Condylostoma Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("30. Peritrich Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("27. Karyorelict Nuclear"), "expected translation not found");
    CHECK_SET_ERR(actionText.contains("25. Candidate Division SR1 and Gracilibacteria Code"), "expected translation not found");

    // just for closing popup menu
    GTMenu::clickMenuItemByName(menu, {"14. The Alternative Flatworm Mitochondrial Code"});
}

GUI_TEST_CLASS_DEFINITION(test_6062) {
    //    1. Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Add 'Map Reads with BWA' element to the scene.
    WorkflowProcessItem* bwaElement = GTUtilsWorkflowDesigner::addElement("Map Reads with BWA", true);

    //    3. Click to the element.
    GTUtilsWorkflowDesigner::click(bwaElement);

    //    Expected state: 'Library' attribute has value 'Single-end'; there is one table in 'Input data' widget, which contains information about input ports.
    const QString actualAttributeValue = GTUtilsWorkflowDesigner::getParameter("Library");
    const QString expectedAttributeValue = "Single-end";
    CHECK_SET_ERR(expectedAttributeValue == actualAttributeValue,
                  QString("An unexpected default value of 'Library' attribute: expected '%1', got '%2'")
                      .arg(expectedAttributeValue)
                      .arg(actualAttributeValue));

    QTableWidget* inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(inputPortTable1 != nullptr, "inputPortTable1 is NULL");

    QTableWidget* inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(1, false);
    CHECK_SET_ERR(inputPortTable2 == nullptr, "Table for the second input port unexpectedly found");

    //    4. Set 'Library' attribute value to 'Paired-end'.
    GTUtilsWorkflowDesigner::setParameter("Library", "Paired-end", GTUtilsWorkflowDesigner::comboValue);

    //    Expected state: there are two tables in 'Input data' widget.
    inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(inputPortTable1 != nullptr, "inputPortTable1 is NULL");

    inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(1);
    CHECK_SET_ERR(inputPortTable2 != nullptr, "Table for the second input port not found");

    //    4. Set 'Library' attribute value to 'Single-end'.
    GTUtilsWorkflowDesigner::clickParameter("Output folder");
    GTUtilsWorkflowDesigner::setParameter("Library", "Single-end", GTUtilsWorkflowDesigner::comboValue);

    //    Expected state: there is one table in 'Input data' widget.
    inputPortTable1 = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(inputPortTable1 != nullptr, "inputPortTable1 is NULL");

    inputPortTable2 = GTUtilsWorkflowDesigner::getInputPortsTable(1, false);
    CHECK_SET_ERR(inputPortTable2 == nullptr, "Table for the second input port unexpectedly found");
}

GUI_TEST_CLASS_DEFINITION(test_6066) {
    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select "Edit" -> "Annotations settings on sequence editing..." menu item in the Details View context menu.
    //    3. Choose "Split (separate annotations parts)" and press "OK".
    GTUtilsDialog::add(new PopupChooserByText({"Edit", "Annotation settings on editing..."}));
    GTUtilsDialog::add(new EditSettingsDialogFiller(EditSettingsDialogFiller::SplitSeparateAnnotationParts, false));
    GTWidget::click(GTUtilsSequenceView::getDetViewByNumber(), Qt::RightButton);

    //    4. Turn on the editing mode.
    GTUtilsSequenceView::enableEditingMode();

    //    5. Set cursor after position 60.
    GTUtilsSequenceView::setCursor(60);

    //    6. Click Space key.
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    //    Expected state: a gap is inserted, the annotation is split.
    // Do not check it here, to avoid view state changing

    //    7. Doubleclick the first part if the split annotation and click Delete key.
    GTUtilsAnnotationsTreeView::clickItem("misc_feature", 1, true);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: the annotation is removed from the Details View and from the Annotations Tree View.
    // Do not check it here, to avoid view state changing

    //    8. Doubleclick the second part of the split annotation.
    GTUtilsAnnotationsTreeView::clickItem("misc_feature", 1, true);

    //    Expected state: UGENE doesn't crash.
}

GUI_TEST_CLASS_DEFINITION(test_6071) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select 2-th CDS annotation in the Zoom view
    GTUtilsSequenceView::clickAnnotationPan("CDS", 2970);

    // 3. Scroll to the 3874 coordinate.
    GTUtilsSequenceView::goToPosition(3874);

    DetView* dw = GTUtilsSequenceView::getDetViewByNumber();
    const U2Region firstVisibleRange = dw->getVisibleRange();

    // 4. Click on 2-th CDS join annotation
    GTUtilsSequenceView::clickAnnotationDet("CDS", 3412);

    // Expected: visible range was not changed
    const U2Region secondVisibleRange = dw->getVisibleRange();
    CHECK_SET_ERR(firstVisibleRange == secondVisibleRange, "Visible range was changed after clicking on the annotation");
}

GUI_TEST_CLASS_DEFINITION(test_6075) {
    // Check that default group and document name in Create Annotations dialog are set to
    // the selected item values in the annotations tree view.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsAnnotationsTreeView::createAnnotation("test_group", "test_feature", "1..100", true, testDir + "_common_data/scenarios/sandbox/test-6075.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that in the new dialog the default group name and document path are set.
    QString selectedFeatureName = GTUtilsAnnotationsTreeView::getSelectedItem();
    CHECK_SET_ERR(selectedFeatureName == "test_feature", "Annotation is not selected");

    class CheckCreateAnnotationsDialogScenario1 : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto groupNameEdit = GTWidget::findLineEdit("leGroupName", dialog);
            CHECK_SET_ERR(groupNameEdit->text() == "test_group", "Group name is not set");

            auto featureNameEdit = GTWidget::findLineEdit("leAnnotationName", dialog);
            CHECK_SET_ERR(featureNameEdit->text().isEmpty(), "Feature name must be empty");

            auto documentNameCombo = GTWidget::findComboBox("cbExistingTable", dialog);
            QString documentName = documentNameCombo->currentText();
            CHECK_SET_ERR(documentNameCombo->isEnabled(), "Document selector must be enabled");
            CHECK_SET_ERR(documentName.startsWith("test-6075.gb"), "Document name must be set: " + documentName);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new CheckCreateAnnotationsDialogScenario1()));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    // Check that in the new dialog the default group name and document path are not set.
    GTUtilsSequenceView::clickOnDetView();  // drop the selection.
    selectedFeatureName = GTUtilsAnnotationsTreeView::getSelectedItem();
    CHECK_SET_ERR(selectedFeatureName.isEmpty(), "Annotation must not be selected");

    class CheckCreateAnnotationsDialogScenario2 : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto groupNameEdit = GTWidget::findLineEdit("leGroupName", dialog);
            QString groupName = groupNameEdit->text();
            CHECK_SET_ERR(groupName.isEmpty() || groupName == "<auto>", "Group name must be empty, current value: " + groupName);

            auto documentNameCombo = GTWidget::findComboBox("cbExistingTable", dialog);
            QString documentName = documentNameCombo->currentText();
            CHECK_SET_ERR(documentNameCombo->isEnabled(), "Document selector must be enabled");
            CHECK_SET_ERR(documentName.startsWith("test-6075.gb"), "Document name must be set: " + documentName);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new CheckCreateAnnotationsDialogScenario2()));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_6078) {
    // 1. Open human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select 1 - 10 chars
    GTUtilsSequenceView::selectSequenceRegion(1, 10);
    GTKeyboardUtils::copy();

    // 3. Enable edit mode
    GTUtilsSequenceView::enableEditingMode();

    // 4. Set the cursor to the 5-th pos
    GTUtilsSequenceView::setCursor(5);

    // 5. Press paste
    GTKeyboardUtils::paste();

    // Expected: cursor on the 15-th pos
    const qint64 pos = GTUtilsSequenceView::getCursor();
    CHECK_SET_ERR(pos == 15, QString("Incorrect cursor position, expected: 15, current: %1").arg(pos));
}

GUI_TEST_CLASS_DEFINITION(test_6083) {
    //    1. open document samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    //    2. Select first sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "exportSelectedMsaRowsToSeparateFilesAction"}, GTGlobals::UseKey));
    GTUtilsDialog::add(new ExportSelectedSequenceFromAlignment(testDir + "_common_data/scenarios/sandbox/", ExportSelectedSequenceFromAlignment::Ugene_db, true));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("Phaneroptera_falcata.ugenedb");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
}

GUI_TEST_CLASS_DEFINITION(test_6084) {
    // Check that modification in an alignment marks the object as 'modified' and 'undo' makes it 'non-modified' again.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Insert gap and check that object is modified.
    GTUtilsMSAEditorSequenceArea::click(QPoint(10, 10));
    GTKeyboardDriver::keyClick(Qt::Key_Space);
    GTUtilsProjectTreeView::itemModificationCheck("COI", true);

    // Undo and check that object is not modified.
    GTUtilsMsaEditor::undo();
    GTUtilsProjectTreeView::itemModificationCheck("COI", false);

    // Redo and check that object is modified again.
    GTUtilsMsaEditor::redo();
    GTUtilsProjectTreeView::itemModificationCheck("COI", true);
}

GUI_TEST_CLASS_DEFINITION(test_6087) {
    // 1. Open  samples/MMDB/1CRN.prt
    GTFileDialog::openFile(dataDir + "samples/MMDB/1CRN.prt");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Try to select a region.
    GTUtilsSequenceView::selectSequenceRegion(10, 20);

    // Expected: ugene was not crashed
    QVector<U2Region> regions = GTUtilsSequenceView::getSelection();
    CHECK_SET_ERR(regions.size() == 1, "Unexpected selection");
}

GUI_TEST_CLASS_DEFINITION(test_6102) {
    // 1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2) Run Smith-waterman search using:
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTTextEdit::setText(GTWidget::findTextEdit("teditPattern", dialog), "RPHP*VAS*LK*RHFARHGKIHN*E*KSSDQGQ");

            GTRadioButton::click("radioTranslation", dialog);

            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);
            //    3. Open tab "Input and output"
            GTTabWidget::setCurrentIndex(GTWidget::findTabWidget("tabWidget", dialog), 1);

            //    4. Chose in the combobox "Multiple alignment"
            GTComboBox::selectItemByText(GTWidget::findComboBox("resultViewVariants", dialog), "Multiple alignment");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new SmithWatermanDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find pattern [Smith-Waterman]..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::doubleClickItem("P1_NC_1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    const bool isAlphabetAmino = GTUtilsMsaEditor::getEditor()->getMaObject()->getAlphabet()->isAmino();
    CHECK_SET_ERR(isAlphabetAmino, "Alphabet is not amino");
}

GUI_TEST_CLASS_DEFINITION(test_6104) {
    // Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Build a phylogenetic tree. Check that the tree is synchronized with the alignment.
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_6104/COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* syncModeButton = GTAction::button("sync_msa_action");
    CHECK_SET_ERR(syncModeButton->isChecked(), "Sync mode must be ON/1");
    CHECK_SET_ERR(syncModeButton->isEnabled(), "Sync mode must be enabled/1");

    // Rename the first to "1".
    GTUtilsMSAEditorSequenceArea::renameSequence("Isophya_altaica_EF540820", "1");
}

GUI_TEST_CLASS_DEFINITION(test_6118) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Make workflow "Read FASTQ File with SE Reads" -> "Improve Reads with Trimmomatic"
    WorkflowProcessItem* readSEElement = GTUtilsWorkflowDesigner::addElement("Read FASTQ File with SE Reads");
    WorkflowProcessItem* trimmomaticElement = GTUtilsWorkflowDesigner::addElement("Improve Reads with Trimmomatic");
    GTUtilsWorkflowDesigner::connect(readSEElement, trimmomaticElement);

    class TrimmomaticScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 3. Add two "ILLUMINACLIP" steps with adapters with similar filenames located in different directories to Trimmomatic worker.
            GTWidget::click(GTWidget::findWidget("buttonAdd", dialog));
            auto menu = GTWidget::findMenuWidget("stepsMenu", dialog);
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/TruSeq3-SE.fa"));
            GTWidget::click(GTWidget::findWidget("tbBrowse", dialog));

            GTWidget::click(GTWidget::findWidget("buttonAdd", dialog));
            menu = GTWidget::findMenuWidget("stepsMenu", dialog);
            GTMenu::clickMenuItemByName(menu, {"ILLUMINACLIP"});
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            auto settingsStep1Widget = GTWidget::findWidget("TrimmomaticStepSettingsWidget_step_1", dialog);
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/regression/6118/deeperDir/TruSeq3-SE.fa"));
            GTWidget::click(GTWidget::findWidget("tbBrowse", settingsStep1Widget));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsWorkflowDesigner::click(readSEElement);
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq");

    GTUtilsWorkflowDesigner::click(trimmomaticElement);
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("TrimmomaticPropertyDialog", QDialogButtonBox::Ok, new TrimmomaticScenario()));
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTTableView::getCellPoint(table, 1, 1));
    GTMouseDriver::click();
    GTWidget::click(GTWidget::findWidget("trimmomaticPropertyToolButton", table));

    // 4. Run this workflow.
    // Expected state : there are no errors during execution.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_6135) {
    // Open "COI.aln".
    // Rename the second sequence to "Phaneroptera_falcata".
    // Current state: There are two sequences with name "Phaneroptera_falcata" (the first and the second one).
    // Select "Export -> Save subalignment" in the context menu.
    // Select only one "Phaneroptera_falcata" sequence and click "Extract".
    // Expected state: one selected sequence was exported.

    GTUtilsProject::openFile(dataDir + "samples/CLUSTALW/COI.aln");

    class NoButtonClickScenario : public CustomScenario {
    public:
        void run() override {
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsMSAEditorSequenceArea::renameSequence("Isophya_altaica_EF540820", "Phaneroptera_falcata");

    GTUtilsDialog::add(new PopupChooser({MSAE_MENU_EXPORT, "Save subalignment"}));
    GTUtilsDialog::add(new ExtractSelectedAsMSADialogFiller(new NoButtonClickScenario()));
    GTMenu::showContextMenu(GTUtilsMSAEditorSequenceArea::getSequenceArea(0));

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("COI.aln"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequencesNameList.length() == 1, "Length of namelist is not 1! Length: " + QString::number(sequencesNameList.length()));
}

GUI_TEST_CLASS_DEFINITION(test_6136) {
    // 1. Open "test/scenarios/_common_data/genbank/target_gene_new.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/target_gene_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "In Silico PCR" tab
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Fill values:
    // Forward: primer - TTTGGATCCAGCATCACCATCACCATCACGATCAAATAGAAGCAATG, mismathches - 27
    // Reverse: primer - AAACCTAGGTACGTAGTGGTAGTGGTAGTGCTAGTTTATCTTCGTTAC, mismathches - 27
    GTUtilsOptionPanelSequenceView::setForwardPrimer("TTTGGATCCAGCATCACCATCACCATCACGATCAAATAGAAGCAATG");
    GTUtilsOptionPanelSequenceView::setForwardPrimerMismatches(27);
    GTUtilsOptionPanelSequenceView::setReversePrimer("AAACCTAGGTACGTAGTGGTAGTGGTAGTGCTAGTTTATCTTCGTTAC");
    GTUtilsOptionPanelSequenceView::setReversePrimerMismatches(27);

    // 4. Press "find product(s)"
    GTUtilsOptionPanelSequenceView::pressFindProducts();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there is one product was found
    const int count = GTUtilsOptionPanelSequenceView::productsCount();
    CHECK_SET_ERR(count == 1, QString("Unexpected products quantity, expected: 1, current: %1").arg(count));

    // 5. Press "Extract product"
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));
    GTUtilsOptionPanelSequenceView::pressExtractProduct();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: Sequence length = 423
    const int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(length == 423, QString("Unexpected sequence length, expected: 423, current: %1").arg(length));

    // Check annotations
    foreach (const int i, QList<int>() << 30 << 376) {
        GTUtilsSequenceView::clickAnnotationPan("Misc. Feature", i, 0, true);
        QVector<U2Region> sel = GTUtilsSequenceView::getSelection();
        CHECK_SET_ERR(sel.size() == 1, QString("1. Unexpected selection annotation regions, expected: 1, current: %1").arg(sel.size()));
    }

    foreach (const int i, QList<int>() << 1 << 376) {
        GTUtilsSequenceView::clickAnnotationPan("misc_feature", i, 0, true);
        QVector<U2Region> sel = GTUtilsSequenceView::getSelection();
        CHECK_SET_ERR(sel.size() == 1, QString("2. Unexpected selection primer annotation regions, expected: 1, current: %1").arg(sel.size()));
    }
}

GUI_TEST_CLASS_DEFINITION(test_6167) {
    // 1. Change workflow designer output folder to sandbox
    class Custom : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::WorkflowDesigner);

            auto workflowOutputEdit = GTWidget::findLineEdit("workflowOutputEdit", dialog);

            GTLineEdit::setText(workflowOutputEdit, sandBoxDir);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "test\_common_data\regression\6167\6167.uwl" and run
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/regression/6167/6167.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::addInputFile("Read File URL(s)", dataDir + "samples/FASTQ/eas.fastq");

    class TrimmomaticCustomScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto addButton = GTWidget::findToolButton("buttonAdd", dialog);

            GTWidget::click(addButton);
            for (int i = 0; i < 4; i++) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
            }

            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            GTKeyboardDriver::keyClick(Qt::Key_Escape);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new TrimmomaticDialogFiller(new TrimmomaticCustomScenario()));
    GTUtilsWorkflowDesigner::click("Trimmomatic 1");
    GTUtilsWorkflowDesigner::setParameter("Trimming steps", "", GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new TrimmomaticDialogFiller(new TrimmomaticCustomScenario()));
    GTUtilsWorkflowDesigner::click("Trimmomatic 2");
    GTUtilsWorkflowDesigner::setParameter("Trimming steps", "", GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: There are no adapter files in the output directory
    QDir sandbox(sandBoxDir);
    QStringList filter = {"????.??.??_?\?-??"};
    QStringList sandboxEntry = sandbox.entryList(filter, QDir::AllEntries);
    CHECK_SET_ERR(sandboxEntry.size() == 1, QString("Unexpected nomber of folders, expected: 1, current62: %1").arg(sandboxEntry.size()));

    QString insideSandbox(sandBoxDir + sandboxEntry.first());
    QDir insideSandboxDir(insideSandbox);
    QStringList resultDirs = insideSandboxDir.entryList();
    CHECK_SET_ERR(resultDirs.size() == 5, QString("Unexpected number of result folders, expected: 5, current: %1").arg(resultDirs.size()));
}

GUI_TEST_CLASS_DEFINITION(test_6204) {
    // 1. Open the WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::loadWorkflow(testDir + "_common_data/scenarios/_regression/6204/6204.uwl");

    // 2. add 3 big alignments to "Read Alignment 1" worker ""_common_data/clustal/100_sequences.aln", ""_common_data/clustal/non_unique_row_names.aln"
    GTUtilsWorkflowDesigner::addInputFile("Read Alignment 1", testDir + "_common_data/clustal/100_sequences.aln");
    GTUtilsWorkflowDesigner::addInputFile("Read Alignment 1", testDir + "_common_data/clustal/non_unique_row_names.aln");
    GTUtilsWorkflowDesigner::addInputFile("Read Alignment", testDir + "_common_data/clustal/COI na.aln");
    GTUtilsWorkflowDesigner::runWorkflow();

    // There is no message "Task is in progress.." after finished task where 2 notifications are present
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::findLabelByText("The workflow task has been finished", GTUtilsDashboard::getDashboard());
}

GUI_TEST_CLASS_DEFINITION(test_6212) {
    // 1. Open the WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Add "Read File URL data" and "Improve Reads with Trimmomatic" elements and connect them.
    const QString readFileName = "Read File URL(s)";
    const QString trimmomaticName = "Improve Reads with Trimmomatic";
    WorkflowProcessItem* readFileNameElement = GTUtilsWorkflowDesigner::addElement(readFileName);
    WorkflowProcessItem* trimmomaticElement = GTUtilsWorkflowDesigner::addElement(trimmomaticName);
    GTUtilsWorkflowDesigner::addInputFile(readFileName, dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsWorkflowDesigner::connect(readFileNameElement, trimmomaticElement);
    GTUtilsWorkflowDesigner::click(trimmomaticName);
    QTableWidget* table1 = GTUtilsWorkflowDesigner::getInputPortsTable(0);
    CHECK_SET_ERR(table1 != nullptr, "QTableWidget isn't found");

    GTUtilsWorkflowDesigner::setTableValue("Input FASTQ URL 1", "Dataset name (by Read File URL(s))", GTUtilsWorkflowDesigner::comboValue, table1);

    // 3. Click on the Trimmomatic element, then click on the "Configure steps" parameter in the Property Editor, click on the appeared browse button in the value field.
    class TrimmomaticCustomScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto buttonBox = GTWidget::findDialogButtonBox("buttonBox", dialog);

            QPushButton* buttonOk = buttonBox->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(buttonOk != nullptr, "buttonOk unexpectedly not found");
            CHECK_SET_ERR(!buttonOk->isEnabled(), "buttonOk should be disabled");

            // 4. Close the dialog
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new TrimmomaticDialogFiller(new TrimmomaticCustomScenario()));
    GTUtilsWorkflowDesigner::click(trimmomaticName);
    GTUtilsWorkflowDesigner::setParameter("Trimming steps", "", GTUtilsWorkflowDesigner::customDialogSelector);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Click "Validate workflow".
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsWorkflowDesigner::validateWorkflow();

    // Expected state: Validation doesn't pass, there is an error about absent steps.
    QStringList errors = GTUtilsWorkflowDesigner::getErrors();
    CHECK_SET_ERR(errors.size() == 1, QString("Unexpected errors number, expected: 1, current: %1").arg(errors.size()));
    CHECK_SET_ERR(errors.first() == "Improve Reads with Trimmomatic: Required parameter is not set: Trimming steps", "Unexpected error in the log. Is should be something about Trimming steps");

    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

GUI_TEST_CLASS_DEFINITION(test_6225) {
    QString filePath = testDir + "_common_data/sanger/alignment_short.ugenedb";
    QString fileName = "sanger_alignment_short.ugenedb";

    // 1. Copy to 'sandbox' and open alignment_short.ugenedb
    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFileDialog::openFile(sandBoxDir, fileName);

    // 2. Open Consensus tab
    GTUtilsOptionPanelMca::openTab(GTUtilsOptionPanelMca::Consensus);

    // 3. Choose FASTA as file format
    GTUtilsOptionPanelMca::setFileFormat(GTUtilsOptionPanelMca::FASTA);

    // 4. Click Export
    GTUtilsOptionPanelMca::pushExportButton();
    GTUtilsTaskTreeView::waitTaskFinished();

    const int size = GTUtilsProjectTreeView::getDocuments().size();
    CHECK_SET_ERR(size == 2, QString("Unexpected documents number; expected: 2, current: %1").arg(size));
}

GUI_TEST_CLASS_DEFINITION(test_6226) {
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Align));
    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/fasta/reference.fa", QStringList());
    parameters.samOutput = false;
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/reads.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_6229) {
    GTUtilsDialog::add(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Align));
    AlignShortReadsFiller::UgeneGenomeAlignerParams parameters(testDir + "_common_data/fasta/reference.fa", QStringList());
    parameters.samOutput = false;
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/reads.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_6230) {
    //    2. Select "Tools" -> Sanger data analysis" -> "Map reads to reference...".
    //    3. Set "_common_data/sanger/reference.gb" as reference, "_common_data/sanger/sanger_*.ab1" as reads. Accept the dialog.
    //    Expected state: the task fails.
    //    4. After the task finish open the report.
    //    Expected state: there is an error message in the report: "The task uses a temporary folder to process the data. The folder path is required not to have spaces. Please set up an appropriate path for the "Temporary files" parameter on the "Directories" tab of the UGENE Application Settings.".
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AlignToReferenceBlastDialogFiller::setReference(testDir + "_common_data/sanger/reference.gb", dialog);

            QStringList reads;
            for (int i = 1; i < 21; i++) {
                reads << QString(testDir + "_common_data/sanger/sanger_%1.ab1").arg(i, 2, 10, QChar('0'));
            }
            AlignToReferenceBlastDialogFiller::setReads(reads, dialog);
            AlignToReferenceBlastDialogFiller::setDestination(sandBoxDir + "test_6230/test_6230.ugenedb", dialog);

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkItem("test_6230.ugenedb");
}

GUI_TEST_CLASS_DEFINITION(test_6232_1) {
    // 1. Open "STEP1_pFUS2_a2a_5.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6232/STEP1_pFUS2_a2a_5.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments".Add "Esp3I" to the "Selected enzymes" in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state : the corresponding "Fragment" annotations have been created
    QStringList groupNames = GTUtilsAnnotationsTreeView::getGroupNames();
    CHECK_SET_ERR(groupNames.contains("fragments  (0, 2)"), "The group \"fragments  (0, 2)\" is unexpectedly absent");
}

GUI_TEST_CLASS_DEFINITION(test_6232_2) {
    // 1. Open "STEP1_pFUS2_a2a_5_2.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6232/STEP1_pFUS2_a2a_5_2.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments".Add "Esp3I" to the "Selected enzymes" in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "left_end_seq" qualifier of the first fragment and "right_end_seq" of the second fragment should have "TGAC" value
    /*QTreeWidgetItem* fragment1 = GTUtilsAnnotationsTreeView::findItem("Fragment 1");
    CHECK_SET_ERR(fragment1 != NULL, "Fragment 1 is not found");
*/
    GTUtilsAnnotationsTreeView::selectItemsByName({"Fragment 1"});
    QString firstValue = GTUtilsAnnotationsTreeView::getQualifierValue("left_end_seq", "Fragment 1");
    CHECK_SET_ERR(firstValue == "TGAC", QString("Unexpected qualifier value of the first fragment, expected: TGAC, current: %1").arg(firstValue));

    GTUtilsAnnotationsTreeView::selectItemsByName({"Fragment 2"});
    QString secondValue = GTUtilsAnnotationsTreeView::getQualifierValue("right_end_seq", "Fragment 2");
    CHECK_SET_ERR(secondValue == "TGAC", QString("Unexpected qualifier value of the first fragment, expected: TGAC, current: %1").arg(secondValue));
}

GUI_TEST_CLASS_DEFINITION(test_6232_3) {
    // 1. Open "STEP1_pFUS2_a2a_5.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6232/STEP1_pFUS2_a2a_5.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments".Add "Esp3I" to the "Selected enzymes", disable "Circular Molecule" checkBox and click "OK".
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTCheckBox::setChecked("circularBox", false, dialog);
            GTWidget::click(GTWidget::findWidget("addAllButton", dialog));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: despite the sequence is circular, fragments were found without the gap it the junction  point
    // Expected: there are two annotations with the following regions were created - U2Region(2, 2467) and U2Region(2473, 412)
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("fragments  (0, 2)");
    CHECK_SET_ERR(regions.size() == 2, QString("Unexpected number of fragments, expected: 2, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first() == U2Region(2, 2467), QString("Unexpected fragment region, expected: start = 2, length = 2467; current: start = %1, length = %2").arg(regions.first().startPos).arg(regions.first().length));
    CHECK_SET_ERR(regions.last() == U2Region(2473, 412), QString("Unexpected fragment region, expected: start = 2473, length = 412; current: start = %1, length = %2").arg(regions.last().startPos).arg(regions.last().length));
}

GUI_TEST_CLASS_DEFINITION(test_6232_4) {
    // 1. Open "STEP1_pFUS2_a2a_5_not_circular.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6232/STEP1_pFUS2_a2a_5_not_circular.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments".Add "Esp3I" to the "Selected enzymes" checkBox and click "OK".
    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller());
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Select "Actions > Cloning > Construct molecule...". Click "Add all" button, click "OK".
    QList<ConstructMoleculeDialogFiller::Action> actions;
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::AddAllFragments, "");
    actions << ConstructMoleculeDialogFiller::Action(ConstructMoleculeDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ConstructMoleculeDialogFiller(actions));
    GTMenu::clickMainMenuItem({"Actions", "Cloning", "Construct molecule..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: the first fragment begins from 3 base, there are "AC" bases before it
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("STEP1_pFUS2_a2a_5 Fragment 1  (0, 1)");
    CHECK_SET_ERR(regions.size() == 1, QString("Unexpected number of fragments, expected: 1, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first().startPos == 2, QString("Unexpected fragment startPos, expected: 2; current: %1").arg(regions.first().startPos));

    QString beginning = GTUtilsSequenceView::getBeginOfSequenceAsString(2);
    CHECK_SET_ERR(beginning == "AC", QString("Unexpected beginning, expected: AC, currecnt: %1").arg(beginning));
}

GUI_TEST_CLASS_DEFINITION(test_6233) {
    // 1. Find the link to the ET download page in the application settings.
    class FindUrlScenario : public CustomScenario {
    public:
        FindUrlScenario(QString& _url)
            : url(_url) {
        }
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);
            auto selectToolPackLabel = GTWidget::findLabel("selectToolPackLabel");

            QPoint labelGlobalPos = selectToolPackLabel->mapToGlobal(selectToolPackLabel->pos());
            GTMouseDriver::moveTo(labelGlobalPos);
            GTClipboard::clear();
            for (int row = 0; row < 70 && url.isEmpty(); row += 10) {
                for (int column = 0; column < 200 && url.isEmpty(); column += 10) {
                    GTThread::waitForMainThread();
                    QPoint newMousePos = labelGlobalPos + QPoint(column, row);
                    GTMouseDriver::moveTo(newMousePos);
                    Qt::CursorShape shape = selectToolPackLabel->cursor().shape();
                    if (shape != Qt::ArrowCursor) {
                        GTMouseDriver::click(Qt::RightButton);
                        GTKeyboardDriver::keyClick(Qt::Key_Down);
                        GTKeyboardDriver::keyClick(Qt::Key_Enter);
                        url = GTClipboard::text();
                    }
                }
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

        QString& url;
    };

    QString url;
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new FindUrlScenario(url)));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);

    CHECK_SET_ERR(!url.isEmpty(), "URL was not found");

    // 2. Fetch html by the link.
    auto factory = new HttpFileAdapterFactory();
    QScopedPointer<IOAdapter> io(factory->createIOAdapter());
    bool isOpened = io->open(url, IOAdapterMode_Read);
    CHECK_SET_ERR(isOpened, "HttpFileAdapter unexpectedly wasn't opened, url: " + url);

    QByteArray data(10000, 0);
    qint64 bytesRead = io->readBlock(data.data(), data.size());
    CHECK_SET_ERR(bytesRead > 100, "Expected at least some data to be read from url: " + url + ", error: " + io->errorString());
    CHECK_SET_ERR(!data.contains("Page not found"), "External Tools page is not found");
}

GUI_TEST_CLASS_DEFINITION(test_6235_1) {
    // 1. Open "_common_data/regression/6235/6235_1.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6235/6235_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments". Add "Esp3I" to the "Selected enzymes" in the appeared dialog, check "Circular molecule", click "OK".
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("addAllButton", dialog));

            GTCheckBox::setChecked("circularBox", true, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are two annotations with the following regions were created - U2Region(2, 2467) and U2Region(2473, 410)
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("fragments  (0, 2)");
    CHECK_SET_ERR(regions.size() == 2, QString("Unexpected number of fragments, expected: 2, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first() == U2Region(2, 2467), QString("Unexpected fragment region, expected: start = 2, length = 2467; current: start = %1, length = %2").arg(regions.first().startPos).arg(regions.first().length));
    CHECK_SET_ERR(regions.last() == U2Region(2473, 410), QString("Unexpected fragment region, expected: start = 2473, length = 410; current: start = %1, length = %2").arg(regions.last().startPos).arg(regions.last().length));
}

GUI_TEST_CLASS_DEFINITION(test_6235_2) {
    // 1. Open "_common_data/regression/6235/6235_1.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6235/6235_1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments". Add "Esp3I" to the "Selected enzymes" in the appeared dialog, uncheck "Circular molecule", click "OK".
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("addAllButton", dialog));

            GTCheckBox::setChecked("circularBox", false, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are two annotations with the following regions were created - U2Region(2, 2467) and U2Region(2473, 412)
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("fragments  (0, 2)");
    CHECK_SET_ERR(regions.size() == 2, QString("Unexpected number of fragments, expected: 2, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first() == U2Region(2, 2467), QString("Unexpected fragment region, expected: start = 2, length = 2467; current: start = %1, length = %2").arg(regions.first().startPos).arg(regions.first().length));
    CHECK_SET_ERR(regions.last() == U2Region(2473, 412), QString("Unexpected fragment region, expected: start = 2473, length = 412; current: start = %1, length = %2").arg(regions.last().startPos).arg(regions.last().length));
}

GUI_TEST_CLASS_DEFINITION(test_6235_3) {
    // 1. Open "_common_data/regression/6235/6235_2.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6235/6235_2.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments". Add "Esp3I" to the "Selected enzymes" in the appeared dialog, check "Circular molecule", click "OK".
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::click(GTWidget::findWidget("addAllButton", dialog));

            GTCheckBox::setChecked("circularBox", true, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are two annotations with the following regions were created - U2Region(2, 2467) and U2Region(2473, 412)
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("fragments  (0, 2)");
    CHECK_SET_ERR(regions.size() == 2, QString("Unexpected number of fragments, expected: 2, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first() == U2Region(416, 2467), QString("Unexpected fragment region, expected: start = 416, length = 2467; current: start = %1, length = %2").arg(regions.first().startPos).arg(regions.first().length));
    CHECK_SET_ERR(regions.last() == U2Region(2, 410), QString("Unexpected fragment region, expected: start = 2, length = 410; current: start = %1, length = %2").arg(regions.last().startPos).arg(regions.last().length));
}

GUI_TEST_CLASS_DEFINITION(test_6235_4) {
    // 1. Open "_common_data/regression/6235/6235_2.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/regression/6235/6235_2.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "Esp3I" enzyme in the appeared dialog, click "OK".
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"Esp3I"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "Actions > Cloning > Digest into fragments". Add "Esp3I" to the "Selected enzymes" in the appeared dialog, uncheck "Circular molecule", click "OK".
    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            CHECK_SET_ERR(nullptr != dialog, "activeModalWidget is NULL");

            GTWidget::click(GTWidget::findWidget("addAllButton", dialog));

            GTCheckBox::setChecked("circularBox", false, dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DigestSequenceDialogFiller(new Scenario));
    GTMenu::clickMainMenuItem({"Tools", "Cloning", "Digest into fragments..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: there are two annotations with the following regions were created - U2Region(2, 2467) and U2Region(2473, 412)
    QList<U2Region> regions = GTUtilsAnnotationsTreeView::getAnnotatedRegionsOfGroup("fragments  (0, 2)");
    CHECK_SET_ERR(regions.size() == 2, QString("Unexpected number of fragments, expected: 2, current: %1").arg(regions.size()));
    CHECK_SET_ERR(regions.first() == U2Region(416, 2467), QString("Unexpected fragment region, expected: start = 416, length = 2467; current: start = %1, length = %2").arg(regions.first().startPos).arg(regions.first().length));
    CHECK_SET_ERR(regions.last() == U2Region(0, 412), QString("Unexpected fragment region, expected: start = 0, length = 412; current: start = %1, length = %2").arg(regions.last().startPos).arg(regions.last().length));
}

GUI_TEST_CLASS_DEFINITION(test_6236) {
    // Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // Compose workflow read sequence -> Remote blast.
    WorkflowProcessItem* readElement = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    WorkflowProcessItem* remoteBlast = GTUtilsWorkflowDesigner::addElementByUsingNameFilter("Remote BLAST");
    GTUtilsWorkflowDesigner::connect(readElement, remoteBlast);

    // Set the input sequence file: "data/samples/Genbank/NC_014267.1.gb".
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Genbank/NC_014267.1.gb");

    // Run the workflow and wait for the expected message.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsLog::checkMessageWithWait(lt, "GET https://blast.ncbi.nlm.nih.gov/Blast.cgi?CMD=Get&FORMAT_TYPE=XML&RID", 90000);
}

GUI_TEST_CLASS_DEFINITION(test_6238) {
    // Open sandBoxDir + "eas.fastq".
    QString sandboxFastqFile = sandBoxDir + "eas.fastq";
    GTFile::copy(testDir + "_common_data/regression/6238/eas.fastq", sandboxFastqFile);

    GTUtilsProject::openMultiSequenceFileAsSequences(sandboxFastqFile);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Replace the file content with the content if the file "_common_data/regression/6238/6238.fastq".
    // Expected state : UGENE offers to reload the modified file.
    // Accept the offering.
    // Expected state: the file reloading failed, an error message in the log appears.
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes, "was modified. Do you want to reload"));

    QFile file(sandboxFastqFile);
    file.open(QFile::ReadWrite);
    QString badContent = GTFile::readAll(testDir + "_common_data/regression/6238/6238.fastq");
    file.write(badContent.toLocal8Bit());
    file.close();
    GTUtilsDialog::checkNoActiveWaiters(20000);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Failed to detect file format"));
    GTUtilsDialog::checkNoActiveWaiters(10000);

    CHECK_SET_ERR(lt.hasError("Failed to detect"), "Expected error not found");
}

GUI_TEST_CLASS_DEFINITION(test_6240) {
    // 1. Open WD. This step allows us to prevent a bad case, when, at the first opening of WD, the dialog "Choose output directory" appears and the filler below is catching it
    class Scenario : public CustomScenario {
    public:
        void run() override {
            GTUtilsWizard::setParameter("Input file(s)", dataDir + "samples/Assembly/chrM.sam");
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };
    // 2. Open "Tools" -> "NGS data analysis" -> "Reads quality control..." workflow
    // 3. Choose "samples/Assembly/chrM.sam" as input and click "Run"
    GTUtilsDialog::add(new StartupDialogFiller());
    GTUtilsDialog::add(new WizardFiller("Quality Control by FastQC Wizard", new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads quality control..."});

    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
}

GUI_TEST_CLASS_DEFINITION(test_6243) {
    // 1. Select "File" -> "Access remove database...".
    // 2 Select "ENSEMBL" database. Use any sample ID as "Resource ID". Accept the dialog.
    // Do it twice, for two different ids
    QList<QString> ensembleIds = QList<QString>() << "ENSG00000205571"
                                                  << "ENSG00000146463";
    for (const auto& id : qAsConst(ensembleIds)) {
        QList<DownloadRemoteFileDialogFiller::Action> actions;
        actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::SetResourceIds, {id});
        actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::SetDatabase, "ENSEMBL");
        actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::EnterSaveToDirectoryPath, sandBoxDir);
        actions << DownloadRemoteFileDialogFiller::Action(DownloadRemoteFileDialogFiller::ClickOk, "");

        GTUtilsDialog::waitForDialog(new DownloadRemoteFileDialogFiller(actions));
        GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseMouse);
        GTUtilsTaskTreeView::waitTaskFinished();
    }

    // Expected state: the sequences are downloaded. The files names contain the sequence ID.
    QString first = QString("%1.fa").arg(ensembleIds.first());
    QString second = QString("%1.fa").arg(ensembleIds.last());
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(first), QString("The first sequence '%1' is absent in the project tree view").arg(first));
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem(second), QString("The second sequence '%1' is absent in the project tree view").arg(second));
}

GUI_TEST_CLASS_DEFINITION(test_6247) {
    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    QString filePath = sandBoxDir + "alignment.ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    // 2. Open "Export consensus" tab, set "../sandbox/Mapped reads_consensus.txt" to the "Export to file" field and click export
    QString exportToFile = sandBoxDir + "Aligned reads_consensus.txt";
    GTUtilsOptionPanelMca::setExportFileName(exportToFile);
    GTUtilsOptionPanelMca::pushExportButton();
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open "alignment.ugenedb" again
    GTUtilsProjectTreeView::doubleClickItem("alignment.ugenedb");

    // 4. And again open "Export consensus" tab, and click export
    GTUtilsOptionPanelMca::pushExportButton();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();

    // Expected: there are 3 documents in the project tree: "alignment.ugenedb", "Aligned reads_consensus.txt" and "Aligned reads_consensus_1.txt"
    QMap<QString, QStringList> docs = GTUtilsProjectTreeView::getDocuments();
    CHECK_SET_ERR(docs.size() == 3, QString("Unexpected docs number, expected: 3, current: %1").arg(docs.size()));
    CHECK_SET_ERR(docs.contains("alignment.ugenedb"), "alignment.ugenedb in unexpectedly absent");
    CHECK_SET_ERR(docs.contains("Aligned reads_consensus.txt"), "alignment.ugenedb in unexpectedly absent");
    CHECK_SET_ERR(docs.contains("Aligned reads_consensus_1.txt"), "alignment.ugenedb in unexpectedly absent");
}

GUI_TEST_CLASS_DEFINITION(test_6249_1) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Compose workflow read file urls -> Fastqc quality control
    GTUtilsWorkflowDesigner::addElement("Read File URL(s)", true);
    GTUtilsWorkflowDesigner::addElement("FastQC Quality Control", true);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read File URL(s)"), GTUtilsWorkflowDesigner::getWorker("FastQC Quality Control"));

    // 3. Set the input sequence files: "data\samples\FASTQ\eas.fastq" and "data\samples\Assembly\chrM.sam"
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    // 4. Run workflow, and check result files on dashboard
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList outFiles = GTUtilsDashboard::getOutputFiles();

    CHECK_SET_ERR(outFiles.contains("eas_fastqc.html"), QString("Output file is not found: eas_fastqc.html"));
    CHECK_SET_ERR(outFiles.contains("chrM_fastqc.html"), QString("Output file is not found: chrM_fastqc.html"));
}

GUI_TEST_CLASS_DEFINITION(test_6249_2) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Compose workflow read file urls -> Fastqc quality control
    GTUtilsWorkflowDesigner::addElement("Read File URL(s)", true);
    GTUtilsWorkflowDesigner::addElement("FastQC Quality Control", true);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read File URL(s)"), GTUtilsWorkflowDesigner::getWorker("FastQC Quality Control"));

    // 3. Set the input sequence files: "data\samples\FASTQ\eas.fastq" and "data\samples\FASTQ\eas.fastq"
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    // 4. Set parameter "Output file" to any location
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("FastQC Quality Control"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setParameter("Output file", QDir(sandBoxDir).absolutePath() + "/test_6249_2_zzzz.html", GTUtilsWorkflowDesigner::textValue);

    // 5. Run workflow, and check result files on dashboard
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList outFiles = GTUtilsDashboard::getOutputFiles();

    CHECK_SET_ERR(outFiles.contains("test_6249_2_zzzz.html"), QString("Output file is not found: test_6249_2_zzzz.html"));
    CHECK_SET_ERR(outFiles.contains("test_6249_2_zzzz_1.html"), QString("Output file is not found: test_6249_2_zzzz_1.html"));
}

GUI_TEST_CLASS_DEFINITION(test_6249_3) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    // 2. Compose workflow read file urls -> Fastqc quality control x2
    GTUtilsWorkflowDesigner::addElement("Read File URL(s)", true);
    GTUtilsWorkflowDesigner::addElement("FastQC Quality Control", true);
    GTUtilsWorkflowDesigner::addElement("FastQC Quality Control", true);
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read File URL(s)"), GTUtilsWorkflowDesigner::getWorker("FastQC Quality Control"));
    GTUtilsWorkflowDesigner::connect(GTUtilsWorkflowDesigner::getWorker("Read File URL(s)"), GTUtilsWorkflowDesigner::getWorker("FastQC Quality Control 1"));

    // 3. Set the input sequence files: "data\samples\FASTQ\eas.fastq" and "data\samples\Assembly\chrM.sam"
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read File URL(s)"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTQ/eas.fastq");
    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/Assembly/chrM.sam");

    // 4. Run workflow, and check result files on dashboard
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList outFiles = GTUtilsDashboard::getOutputFiles();

    CHECK_SET_ERR(outFiles.contains("eas_fastqc.html"), QString("Output file is not found:  eas_fastqc.html"));
    CHECK_SET_ERR(outFiles.contains("chrM_fastqc.html"), QString("Output file is not found:  chrM_fastqc.html"));
    CHECK_SET_ERR(outFiles.contains("eas_fastqc_1.html"), QString("Output file is not found:  eas_fastqc_1.html"));
    CHECK_SET_ERR(outFiles.contains("chrM_fastqc_1.html"), QString("Output file is not found:  file chrM_fastqc_1.html"));
}

GUI_TEST_CLASS_DEFINITION(test_6256) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    QString tempDir = QDir(sandBoxDir + "test_6256").absolutePath();

    class Custom : public CustomScenario {
        void run() {
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::WorkflowDesigner);
            QWidget* dialog = GTWidget::getActiveModalWidget();
            QDir().mkpath(tempDir);
            GTFile::setReadOnly(tempDir);
            GTLineEdit::setText(GTWidget::findLineEdit("workflowOutputEdit", dialog), tempDir);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }

    public:
        QString tempDir;
    };
    // 2. Open application settings and change workflow output directory to nonexistent path
    Custom* c = new Custom();
    c->tempDir = tempDir;
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(c));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
    // 3. Add "Read File URL" element and validate workflow
    // Expected state: there are 2 erorrs after validation
    GTUtilsWorkflowDesigner::addElement("Read File URL(s)", true);
    GTUtilsWorkflowDesigner::validateWorkflow();
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTFile::setReadWrite(tempDir);

    CHECK_SET_ERR(GTUtilsWorkflowDesigner::getErrors().size() == 2, "Unexpected number of errors");
}

GUI_TEST_CLASS_DEFINITION(test_6262) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Add "Filter Annotations by Name", "Filter Annotations by Name" and connect them
    WorkflowProcessItem* element1 = GTUtilsWorkflowDesigner::addElement("Filter Annotations by Name");
    WorkflowProcessItem* element2 = GTUtilsWorkflowDesigner::addElement("Filter Annotations by Name");

    // 3. Check Input port.
    CHECK_SET_ERR(!GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't closed");
    GTGroupBox::setChecked("inputPortBox", true);
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name");
    CHECK_SET_ERR(GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't opened");
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name 1");
    CHECK_SET_ERR(GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't opened");
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name");
    CHECK_SET_ERR(GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't opened");

    // 4. Check Input port.
    GTGroupBox::setChecked("inputPortBox", false);
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name");
    CHECK_SET_ERR(!GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't closed");
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name 1");
    CHECK_SET_ERR(!GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't closed");
    GTUtilsWorkflowDesigner::click("Filter Annotations by Name");
    CHECK_SET_ERR(!GTGroupBox::getChecked("inputPortBox"), "Input Ports table isn't closed");

    // 5. Check Output port.
    CHECK_SET_ERR(!GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't closed");
    GTGroupBox::setChecked("outputPortBox", true);
    GTUtilsWorkflowDesigner::click(element1);
    CHECK_SET_ERR(GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't opened");
    GTUtilsWorkflowDesigner::click(element2);
    CHECK_SET_ERR(GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't opened");
    GTUtilsWorkflowDesigner::click(element1);
    CHECK_SET_ERR(GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't opened");

    // 6. Check Output port.
    GTGroupBox::setChecked("outputPortBox", false);
    GTUtilsWorkflowDesigner::click(element1);
    CHECK_SET_ERR(!GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't closed");
    GTUtilsWorkflowDesigner::click(element2);
    CHECK_SET_ERR(!GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't closed");
    GTUtilsWorkflowDesigner::click(element1);
    CHECK_SET_ERR(!GTGroupBox::getChecked("outputPortBox"), "Output Ports table isn't closed");
}

GUI_TEST_CLASS_DEFINITION(test_6277) {
    //    The test checks that the second column of a table with annotations colors on the "Annotations Highlighting" options panel tab in Sequence View is wide enough.
    //    UGENE behaviour differed if it was build with Qt5.4 and Qt5.7

    //    1. Open "data/samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Annotations Highlighting" options panel tab.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::AnnotationsHighlighting);

    //    Expected state: the second column's width is 60 pixels; it took at least 20% of the table width; the first column took all available space.
    auto table = GTWidget::findTreeWidget("OP_ANNOT_HIGHLIGHT_TREE");

    QScrollBar* scrollBar = table->verticalScrollBar();
    const int scrollBarWidth = scrollBar == nullptr ? 0 : (scrollBar->isVisible() ? scrollBar->width() : 0);

    // These numbers are defined in the widget stylesheet in AnnotHighlightTree.cpp
    const int MARGIN_LEFT = 5;
    const int MARGIN_RIGHT = 10;
    const int BORDER_WIDTH = 1;
    const int MAGIC_NUMBER = scrollBarWidth + MARGIN_LEFT + MARGIN_RIGHT + 2 * BORDER_WIDTH;

    const int COLOR_COLUMN_NUMBER = 1;
    int colorColumnWidth = table->columnWidth(COLOR_COLUMN_NUMBER);
    int totalTableWidth = table->width();

    const int EXPECTED_COLOR_COLUMN_WIDTH = 60;
    CHECK_SET_ERR(EXPECTED_COLOR_COLUMN_WIDTH == colorColumnWidth,
                  QString("Color column width is incorrect: expected %1, got %2")
                      .arg(EXPECTED_COLOR_COLUMN_WIDTH)
                      .arg(colorColumnWidth));

    CHECK_SET_ERR(static_cast<double>(colorColumnWidth) / totalTableWidth >= 0.2,
                  QString("Color column is too narrow: it's width is %1, the table width is %2")
                      .arg(colorColumnWidth)
                      .arg(totalTableWidth));

    const int ANNOTATION_NAME_COLUMN_NUMBER = 0;
    int annotationNameColumnWidth = table->columnWidth(ANNOTATION_NAME_COLUMN_NUMBER);
    CHECK_SET_ERR(annotationNameColumnWidth == totalTableWidth - colorColumnWidth - MAGIC_NUMBER,
                  QString("Annotation name column isn't stretched: it's width is %1, width of the color column is %2, "
                          "the table width is %3")
                      .arg(annotationNameColumnWidth)
                      .arg(colorColumnWidth)
                      .arg(totalTableWidth));

    //    3. Drag and drop the options panel tab's left border to enlarge it.
    GTUtilsOptionsPanel::resizeToMaximum();

    //    Expected state: the second column's width is 60 pixels; the first column took all available space.
    colorColumnWidth = table->columnWidth(COLOR_COLUMN_NUMBER);
    totalTableWidth = table->width();
    annotationNameColumnWidth = table->columnWidth(ANNOTATION_NAME_COLUMN_NUMBER);

    CHECK_SET_ERR(EXPECTED_COLOR_COLUMN_WIDTH == colorColumnWidth,
                  QString("Color column width is incorrect after resizing: expected %1, got %2")
                      .arg(EXPECTED_COLOR_COLUMN_WIDTH)
                      .arg(colorColumnWidth));

    CHECK_SET_ERR(annotationNameColumnWidth == totalTableWidth - colorColumnWidth - MAGIC_NUMBER,
                  QString("Annotation name column isn't stretched after resizing: it's width is %1, width of the color column is %2, "
                          "the table width is %3")
                      .arg(annotationNameColumnWidth)
                      .arg(colorColumnWidth)
                      .arg(totalTableWidth));
}

GUI_TEST_CLASS_DEFINITION(test_6279) {
    class Custom : public CustomScenario {
    public:
        virtual void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::findLineEdit("leAnnotationName", dialog);

            GTWidget::findRadioButton("rbGenbankFormat", dialog);

            GTWidget::findLineEdit("leLocation", dialog);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    // 1. Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Click CDS annotation on pan view
    GTUtilsSequenceView::clickAnnotationPan("CDS", 2970, 0, true);
    // 3. Press F2 to open Edit annotation dialog
    GTUtilsDialog::waitForDialog(new EditAnnotationFiller(new Custom()));
    GTKeyboardDriver::keyClick(Qt::Key_F2);
}

GUI_TEST_CLASS_DEFINITION(test_6283) {
    class Custom : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            // 2. Open a python tab
            AppSettingsDialogFiller::isExternalToolValid("python");

            // Expected:: Bio module is valid
            bool isToolValid = true;
            if (!isOsWindows()) {
                isToolValid = AppSettingsDialogFiller::isExternalToolValid("Bio");
            }
            if (!isToolValid) {
                GT_FAIL("Bio is not valid", );
            }

            bool isPathOnlyValidation = qgetenv("UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY") == "1";
            if (!isPathOnlyValidation) {
                // Expected: Bio module version is 1.73
                bool hasVersion = true;
                if (!isOsWindows()) {
                    hasVersion = AppSettingsDialogFiller::isToolDescriptionContainsString("Bio", "Version: 1.73");
                }
                if (!hasVersion) {
                    GT_FAIL("Incorrect Bio version", );
                }
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_6291) {
    // 1. Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Click CDS annotation on pan view
    // GTUtilsSequenceView::clickAnnotationPan("CDS", 2970, 0, true);
    // 3. Select qualifier
    GTUtilsAnnotationsTreeView::selectItemsByName({"CDS"});
    QString qValue = GTUtilsAnnotationsTreeView::getQualifierValue("product", GTUtilsAnnotationsTreeView::findItem("CDS"));
    // QTreeWidgetItem *item = GTUtilsAnnotationsTreeView::findItem("db_xref");
    GTUtilsAnnotationsTreeView::clickItem("product", 1, false);
    // 4. Click active action "Copy qualifier..." in menu actions
    GTMenu::clickMainMenuItem({"Actions", "Copy/Paste", "Copy qualifier 'product' value"}, GTGlobals::UseMouse);
    QString actualValue = GTClipboard::text();
    CHECK_SET_ERR(actualValue == qValue, QString("Qualifier text %1 differs with expected %2.").arg(actualValue).arg(qValue));
}

GUI_TEST_CLASS_DEFINITION(test_6298) {
    // 1. Open _common_data/scenarios/_regression/6298/small_with_one_char.fa
    // 2. Press "Join sequences into alignment..." radio button
    // 3. Press "OK" button
    // 4. Expected state: the alignment alphabet is "Standard amino acid"

    if (isOsMac()) {
        // hack for mac
        MainWindow* mw = AppContext::getMainWindow();
        CHECK_SET_ERR(mw != nullptr, "MainWindow is NULL");
        QMainWindow* mainWindow = mw->getQMainWindow();
        CHECK_SET_ERR(mainWindow != nullptr, "QMainWindow is NULL");
        auto w = qobject_cast<QWidget*>(mainWindow);
        GTWidget::click(w, Qt::LeftButton, QPoint(5, 5));
    }

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTUtilsProject::openFile(testDir + "_common_data/scenarios/_regression/6298/small_with_one_char.fa");

    // Expected state: alignment has been opened and whole msa alphabet is amino
    bool isAmino = GTUtilsMSAEditorSequenceArea::hasAminoAlphabet();
    CHECK_SET_ERR(isAmino, "Aligment has wrong alphabet type");
}

GUI_TEST_CLASS_DEFINITION(test_6301) {
    class Custom : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            bool isPathOnlyValidation = qgetenv("UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY") == "1";
            if (!isPathOnlyValidation) {
                // Expected: SPAdes description contains the following string - "Version: 3.xx.x"
                bool hasValidVersion = AppSettingsDialogFiller::isToolDescriptionContainsString("SPAdes", "Version: 3.");
                if (!hasValidVersion) {
                    GT_FAIL("Unexpected SPAdes version", );
                }
            }
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    // 1. Open "UGENE Application Settings", select "External Tools" tab.
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_6309) {
    class SetToolUrlScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);
            QPushButton* pushButton = box->button(QDialogButtonBox::Ok);
            CHECK_SET_ERR(pushButton != nullptr, "pushButton is NULL");

            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);
            QString tabixPath = AppSettingsDialogFiller::getExternalToolPath("Tabix");
            QDir tabixDir(tabixPath);
            tabixDir.cdUp();
            tabixDir.cdUp();
            QString extToolsPath = tabixDir.absolutePath();
            AppSettingsDialogFiller::setExternalToolsDir(extToolsPath);

            CHECK_SET_ERR(pushButton->isEnabled() == false, "pushButton is enabled");
            GTUtilsTaskTreeView::waitTaskFinished();
            CHECK_SET_ERR(pushButton->isEnabled() == true, "pushButton is disabled");
            GTWidget::click(pushButton);
        }
    };
    // 1. Open UGENE
    // 2. Open menu Settings->Preferences select page 'External tools'
    // 3. Press top '...' button and select folder with external tools
    // Expected state: Ok button and left Tree element with preferences pages are disabled while external tools validating

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new SetToolUrlScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
}

GUI_TEST_CLASS_DEFINITION(test_6314) {
    // 1. Copy "_common_data/clustal/align.aln" to sandbox and open it
    QString filePath = sandBoxDir + "test_6043.aln";
    GTFile::copy(testDir + "_common_data/clustal/align.aln", filePath);

    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Rename the first sequence with some very long name (more than 150 chars)
    QString veryLongName = QString("Q").repeated(200);
    GTUtilsMSAEditorSequenceArea::renameSequence("IXI_234", veryLongName);

    // 3. Save sequence and close the project
    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Save all"}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::closeProject(true);

    // 4. Open the file again
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the length of the first sequence name is 150 chars
    QString name = GTUtilsMSAEditorSequenceArea::getVisibleNames().first();

    CHECK_SET_ERR(name.size() == 150,
                  QString("Unexpected sequence name length, expected: 150, current: %1")
                      .arg(name.size()));
}

GUI_TEST_CLASS_DEFINITION(test_6350) {
    // 1. Open "human_T1.fa"
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Mark the sequence as circular
    GTUtilsProjectTreeView::markSequenceAsCircular("human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    // 3. Select a joined region, which contains a part in the end and a part in the beginning
    GTUtilsSequenceView::selectSeveralRegionsByDialog("150000..199950,1..50000");

    // 4. Export selected region as sequence
    GTUtilsDialog::waitForDialog(new ExportSelectedRegionFiller(sandBoxDir + "human_T1_reg.fa", QString()));
    GTMenu::clickMainMenuItem({"Actions", "Export", "Export selected sequence region..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there is the only sequence in the exported file
    QStringList list = GTUtilsProjectTreeView::getDocuments().value("human_T1_reg.fa");
    CHECK_SET_ERR(list.size() == 1,
                  QString("Unexpected sequence number, expected: 1, current: %1")
                      .arg(list.size()));
    CHECK_SET_ERR(list.first() == "[s] region [150000 199950]",
                  QString("Unexpected sequence name, expected: [s] region [150000 199950], current %1")
                      .arg(list.first()));
}

GUI_TEST_CLASS_DEFINITION(test_6397) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Place repeat worker
    GTUtilsWorkflowDesigner::addElement("Find Repeats");

    // Expected state: default value for "Apply 'Max distance' attribute" is True
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Find Repeats"));
    QString defaultAttr = GTUtilsWorkflowDesigner::getParameter("Apply 'Max distance' attribute");
    CHECK_SET_ERR(defaultAttr == "True", "Attribute value isn't 'True'");

    // 3. Set "Max distance" parameter to 0
    GTUtilsWorkflowDesigner::setParameter("Max distance", "0", GTUtilsWorkflowDesigner::spinValue, GTGlobals::UseKey);
    GTUtilsWorkflowDesigner::click(GTUtilsWorkflowDesigner::getWorker("Find Repeats"));
    GTUtilsWorkflowDesigner::clickParameter("Max distance");

    QList<QWidget*> list;
    foreach (QWidget* w, GTMainWindow::getMainWindowsAsWidget()) {
        list.append(w);
    }

    QSpinBox* qsb = nullptr;
    for (QWidget* w : qAsConst(list)) {
        foreach (QObject* o, w->findChildren<QObject*>()) {
            qsb = qobject_cast<QSpinBox*>(o);
            if (qsb != nullptr) {
                break;
            }
        }
        if (qsb != nullptr) {
            break;
        }
    }

    // Expected state: it set successfully, ensure that 0 is minimum value
    QString maxDistance = GTUtilsWorkflowDesigner::getParameter("Max distance", true);
    CHECK_SET_ERR(maxDistance == "0 bp", "Attribute value isn't 0 bp");
    CHECK_SET_ERR(qsb->minimum() == 0, "Minimum value isn't 0");

    // 4. Open human_t1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    class Custom : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto maxDistanceBox = GTWidget::findSpinBox("maxDistBox", dialog);
            GTSpinBox::checkLimits(maxDistanceBox, 0, 1000000);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    // 5. Open repeat finder dialog
    // Expected state: minimum value for max distance combobox is 0
    GTUtilsDialog::waitForDialog(new FindRepeatsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find repeats..."}, GTGlobals::UseMouse);
}

GUI_TEST_CLASS_DEFINITION(test_6398) {
    // 1. Open "_common_data/regression/6398/6398.gtf" file
    // Expected: 5 similarity points of the 'GTF" format
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("GTF", 5, 1));
    GTUtilsProject::openFile(testDir + "_common_data/regression/6398/6398.gtf");
    GTUtilsTaskTreeView::waitTaskFinished();
}

namespace GuiTests {

bool compareColorsInRange(const QColor& col1, const QColor& col2, int percentageRange) {
    float coeffRight = (100.0 + percentageRange) / 100.0;
    float coeffLeft = (100.0 - percentageRange) / 100.0;

    int maxLimitRed = col1.red() * coeffRight;
    int minLimitRed = col1.red() * coeffLeft;

    if (minLimitRed > col2.red() || maxLimitRed < col2.red()) {
        return false;
    }

    int maxLimitGreen = col1.green() * coeffRight;
    int minLimitGreen = col1.green() * coeffLeft;

    if (minLimitGreen > col2.green() || maxLimitGreen < col2.green()) {
        return false;
    }

    int maxLimitBlue = col1.blue() * coeffRight;
    int minLimitBlue = col1.blue() * coeffLeft;

    if (minLimitBlue > col2.blue() || maxLimitBlue < col2.blue()) {
        return false;
    }

    return true;
}

}  // namespace GuiTests

GUI_TEST_CLASS_DEFINITION(test_6455) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "regression_6455.ugenedb");
    QFile::copy(dataDir + "samples/Assembly/chrM.fa", sandBoxDir + "regression_6455.fa");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(sandBoxDir + "regression_6455.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "regression_6455.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    // 3. Click the "chrM" sequence object in Project View.
    GTUtilsProjectTreeView::click("chrM", "regression_6455.fa");

    // 4. Click "Set reference sequence", and zoom view until it possible.
    // Expected result: first visible symbol "C" with green background color.
    GTUtilsDialog::waitForDialog(new PopupChooser({"setReferenceAction"}));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsAssemblyBrowser::zoomToMax();
    GTUtilsAssemblyBrowser::scrollToStart(Qt::Horizontal);
    auto refArea = GTWidget::findWidget("Assembly reference sequence area");
    QString color = GTWidget::getColor(refArea, QPoint(5, 5)).name();
    QString colorOfG = "#09689c";
    CHECK_SET_ERR(GuiTests::compareColorsInRange(color, colorOfG, 10), QString("color is %1, expected: %2").arg(color).arg(colorOfG));

    // 5. Edit chrM by add 5 symbols at start
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Activate view: chrM [regression_6455.fa]"}));
    GTUtilsProjectTreeView::doubleClickItem("regression_6455.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsSequenceView::enableEditingMode();

    GTKeyboardDriver::keyClick('T');
    GTKeyboardDriver::keyClick('T');
    GTKeyboardDriver::keyClick('T');
    GTKeyboardDriver::keyClick('T');
    GTKeyboardDriver::keyClick('T');

    // 6. Switch back to assembly view.
    // Expected result: first visible symbol on the screen is "A" with a yellow background color.
    GTUtilsProjectTreeView::doubleClickItem("regression_6455.ugenedb");
    QWidget* assemblyBrowserWindow = GTUtilsAssemblyBrowser::getActiveAssemblyBrowserWindow();

    refArea = GTWidget::findWidget("Assembly reference sequence area", assemblyBrowserWindow);
    color = GTWidget::getColor(refArea, QPoint(5, 5)).name();

    // Remove association for assembly file or GUI framework will fail on shutdown on de-association dialog called from window->close().
    GTUtilsDialog::waitForDialog(new PopupChooser({"unassociateReferenceAction"}));
    GTWidget::click(refArea, Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters();

    QString colorOfT = "#ba546c";
    CHECK_SET_ERR(GuiTests::compareColorsInRange(color, colorOfT, 10), QString("color is %1, expected: %2").arg(colorOfT).arg(color));
}

GUI_TEST_CLASS_DEFINITION(test_6459) {
    // 1. Open "data/samples/Genbank/human_T1.fa".
    // 2. Open "Search in Sequence" options panel tab.
    // 3. Set "Substitute" algorithm.Check "Search with ambiguous bases" checkbox.
    // 4. Ensure that the search is performed on both strands(it is the default value).
    // 5. Enter the following pattern : "YYYGYY".
    // Expected result: 2738 results are found.

    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    SchedulerListener listener;
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");
    GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases();
    GTUtilsOptionPanelSequenceView::enterPattern("YYYGYY");

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2738"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_6475_1) {
    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on "Add element with external tool" button on the toolbar.
    //    3. Select "_common_data/scenarios/_regression/6475/test_6475_1.etc" file.
    //    Expected state: there is "test_6475_1" element on the scene.
    GTUtilsWorkflowDesigner::importCmdlineBasedElement(testDir + "_common_data/scenarios/_regression/6475/test_6475_1.etc");

    //    4. Run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();

    //    Expected state: the workflow finishes soon.
    GTUtilsTaskTreeView::waitTaskFinished(30000);
}

GUI_TEST_CLASS_DEFINITION(test_6475_2) {
    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on "Add element with external tool" button on the toolbar.
    //    3. Select "_common_data/scenarios/_regression/6475/test_6475_2.etc" file.
    GTUtilsWorkflowDesigner::importCmdlineBasedElement(testDir + "_common_data/scenarios/_regression/6475/test_6475_2.etc");

    //    4. Open "_common_data/scenarios/_regression/6475/test_6475_2.uwl".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6475/test_6475_2.uwl");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    5. Click on "Read File URL(s)" element.
    GTUtilsWorkflowDesigner::click("Read File URL(s)");

    //    6. Add "_common_data/fasta/fa2.fa" and "_common_data/fasta/fa3.fa" to "Dataset 1".
    GTUtilsWorkflowDesigner::setDatasetInputFiles(QStringList({testDir + "_common_data/fasta/fa2.fa", testDir + "_common_data/fasta/fa3.fa"}));

    //    4. Run the workflow.
    GTLogTracer lt;

    GTUtilsWorkflowDesigner::runWorkflow();

    //    Expected state: the workflow finishes soon without errors.
    GTUtilsTaskTreeView::waitTaskFinished(30000);
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    ;
}

GUI_TEST_CLASS_DEFINITION(test_6481_1) {
    //    Test to check that element with external tool will add to dashboard an URL to file that is set as parameter with type "Output file URL" and it will be opened by UGENE by default.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Create element with external tool" button on the toolbar.
    //    3. Fill the wizard with the following values (not mentioned values can be set with any value):
    //        Parameters page: a parameter with a type "Output file URL".
    //    4. Accept the wizard.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    QString elementName = GTUtils::genUniqueString("test_6481_1");
    settings.elementName = elementName;
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.parameters << CreateElementWithCommandLineToolFiller::ParameterData("output_file_url", qMakePair(CreateElementWithCommandLineToolFiller::OutputFileUrl, QString()));
    settings.command = "%USUPP_JAVA% -help $output_file_url";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click(elementName);
    GTUtilsWorkflowDesigner::setParameter("output_file_url", QFileInfo(testDir + "_common_data/fasta/human_T1_cutted.fa").absoluteFilePath(), GTUtilsWorkflowDesigner::textValue);

    //    6. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there is an output file on the dashboard.
    QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "There are no output files on the dashboard");

    int expectedCount = 1;
    CHECK_SET_ERR(expectedCount == outputFiles.size(), QString("There are too many output files on the dashboard: expected %1, got %2").arg(expectedCount).arg(outputFiles.size()));

    QString expectedName = "human_T1_cutted.fa";
    CHECK_SET_ERR(expectedName == outputFiles.first(), QString("An unexpected output file name: expected '%1', got '%2'").arg(expectedName).arg(outputFiles.first()));

    //    7. Open a menu on the output item on the dashboard.
    //    Expected state: there are two options in the menu: "Open containing folder" and "Open by operating system".
    // It is not trivial to get the menu items. It is not implemented yet.

    //    8. Click on the file on the dashboard.
    GTUtilsDashboard::clickOutputFile(outputFiles.first());

    //    Expected state: UGENE tries to open the file.
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("human_T1_cutted.fa", "AnnotatedDNAView");
}

GUI_TEST_CLASS_DEFINITION(test_6481_2) {
    //    Test to check that element with external tool will add to dashboard an URL to folder that is set as parameter with type "Output folder URL" and it doesn't have an option to be opened by UGENE.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Create element with external tool" button on the toolbar.
    //    3. Fill the wizard with the following values (not mentioned values can be set with any value):
    //        Parameters page: a parameter with a type "Output folder URL".
    //    4. Accept the wizard.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    QString elementName = GTUtils::genUniqueString("test_6481_2");
    settings.elementName = elementName;
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.parameters << CreateElementWithCommandLineToolFiller::ParameterData("output_folder_url", qMakePair(CreateElementWithCommandLineToolFiller::OutputFolderUrl, QString()));
    settings.command = "%USUPP_JAVA% -help $output_folder_url";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click(elementName);
    GTUtilsWorkflowDesigner::setParameter("output_folder_url", QFileInfo(sandBoxDir).absoluteFilePath(), GTUtilsWorkflowDesigner::textValue);

    //    6. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there is an output folder on the dashboard.
    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "There are no output files on the dashboard");
    const int expectedCount = 1;
    CHECK_SET_ERR(expectedCount == outputFiles.size(), QString("There are too many output files on the dashboard: expected %1, got %2").arg(expectedCount).arg(outputFiles.size()));
    const QString expectedName = "sandbox";
    CHECK_SET_ERR(expectedName == outputFiles.first(), QString("An unexpected output file name: expected '%1', got '%2'").arg(expectedName).arg(outputFiles.first()));

    //    7. Open a menu on the output item on the dashboard.
    //    Expected state: there is the only option "Open containing folder" in the menu.
    // It is not trivial to get the menu items. It is not implemented yet.

    //    8. Click on the output item.
    //    Expected state: a system file manager opens the folder.
    // It is impossible to check that the file manager is opened on the item clicking.
}

GUI_TEST_CLASS_DEFINITION(test_6481_3) {
    //    Test to check that it is possible to forbid to open by UGENE an URL to file that is added to a dashboard by element with external tool.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Add element with external tool" button on the toolbar.
    //    3. Select "_common_data/scenarios/_regression/6481/test_6481_3.etc". Accept the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/6481/test_6481_3.etc"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Add element with external tool");

    //    4. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click("test_6481_3");
    GTUtilsWorkflowDesigner::setParameter("output_file_url", QFileInfo(testDir + "_common_data/fasta/human_T1_cutted.fa").absoluteFilePath(), GTUtilsWorkflowDesigner::textValue);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there is an output file on the dashboard.
    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(!outputFiles.isEmpty(), "There are no output files on the dashboard");
    const int expectedCount = 1;
    CHECK_SET_ERR(expectedCount == outputFiles.size(), QString("There are too many output files on the dashboard: expected %1, got %2").arg(expectedCount).arg(outputFiles.size()));
    const QString expectedName = "human_T1_cutted.fa";
    CHECK_SET_ERR(expectedName == outputFiles.first(), QString("An unexpected output file name: expected '%1', got '%2'").arg(expectedName).arg(outputFiles.first()));

    //    7. Open a menu on the output item on the dashboard.
    //    Expected state: there is the only option in the menu: "Open containing folder".
    // It is not trivial to get the menu items. It is not implemented yet.

    //    8. Click on the output item.
    //    Expected state: the file is opened with some other application.
    // It is impossible to check that some other application is opened on the item clicking.
}

GUI_TEST_CLASS_DEFINITION(test_6481_4) {
    //    Test to check that it is possible to forbid to add to dashboard URLs to file that is set in parameters with types "Output file URL" or "Output folder URL" in element with external tool.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Add element with external tool" button on the toolbar.
    //    3. Select "_common_data/scenarios/_regression/6481/test_6481_4.etc". Accept the dialog.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/6481/test_6481_4.etc"));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Add element with external tool");

    //    4. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click("test_6481_4");
    GTUtilsWorkflowDesigner::setParameter("output_file_url", QFileInfo(testDir + "_common_data/fasta/human_T1_cutted.fa").absoluteFilePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter("output_folder_url", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);

    //    5. Launch the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there are no entries in the output widget on the dashboard.
    const QStringList outputFiles = GTUtilsDashboard::getOutputFiles();
    const int expectedCount = 0;
    CHECK_SET_ERR(expectedCount == outputFiles.size(), QString("There are too many output files on the dashboard: expected %1, got %2").arg(expectedCount).arg(outputFiles.size()));
}

GUI_TEST_CLASS_DEFINITION(test_6474_1) {
    // 1. Open "_common_data/scenarios/_regression/6474/6474.aln"
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6474/6474.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Open the highlighting tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    // 3. Select the "Percentage identity (colored)" color scheme
    GTUtilsOptionPanelMsa::setColorScheme("Percentage identity (colored)    ", GTGlobals::UseMouse);

    // Zoom to max
    GTUtilsMSAEditorSequenceArea::zoomToMax();

    // Expected colors:
    QStringList backgroundColors = {"#ffff00", "#00ffff", "#00ffff", "#00ff00", "#00ff00", "#ffffff", "#ffffff", "#ffffff", "#ffffff"};
    QStringList fontColors = {"#ff0000", "#0000ff", "#0000ff", "#000000", "#000000", "#000000", "#000000", "#000000", "#000000"};

    for (int i = 0; i < 9; i++) {
        GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(i, 0), fontColors[i], backgroundColors[i]);
    }
}

GUI_TEST_CLASS_DEFINITION(test_6474_2) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6474/6474.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Open the highlighting tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);

    // Select the "Percentage identity (colored)" color scheme.
    GTUtilsOptionPanelMsa::setColorScheme("Percentage identity (colored)    ", GTGlobals::UseMouse);

    // Zoom to max.
    GTUtilsMSAEditorSequenceArea::zoomToMax();

    // Expected colors:
    QStringList backgroundColors = {"#00ffff", "#ffffff"};
    QStringList fontColors = {"#0000ff", "#000000"};
    QList<int> columns = {1, 2, 5, 6};
    for (int i : qAsConst(columns)) {
        int colorIndex = (i == 1 || i == 2) ? 0 : 1;
        GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(i, 0), fontColors[colorIndex], backgroundColors[colorIndex]);
    }

    // Set Threshold to 900.
    auto colorThresholdSlider = GTWidget::findSlider("colorThresholdSlider");

    GTSlider::setValue(colorThresholdSlider, 900);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected colors: background - all columns #ffffff, font - all columns ##000000.
    for (int i : qAsConst(columns)) {
        GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(i, 0), fontColors[1], backgroundColors[1]);
    }

    // Set Threshold to 100.
    GTSlider::setValue(colorThresholdSlider, 100);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected colors: background - all columns #00ffff, font - all columns ##0000ff.
    for (int i : qAsConst(columns)) {
        GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(i, 0), fontColors[0], backgroundColors[0]);
    }
}

GUI_TEST_CLASS_DEFINITION(test_6488_1) {
    //    1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click "Create element with external tool" button on the toolbar.
    //    3. Fill the dialog with the following data:
    //        Element name: "UGENE-6488 test element 1"
    //        Command line tool: Integrated external tool "python"
    //        No inputs, parameters and outputs
    //        Command: "just a command"
    //        Element description on the scene: "description on the scene"
    //        Detailed element description: "detailed element description"
    //    4. Accept the dialog.
    //    Expected state: the element was created and put on the scene.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "UGENE-6488 test element 1";
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.command = "just a command";
    settings.description = "detailed element description";
    settings.prompter = "description on the scene";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Click on the element on the scene.
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 1");

    //    6. Call a context menu on the element on the scene.
    //    7. Select "Edit configuration..." menu item.
    //    8. Go to the "Command" page in the wizard.
    //    9. Set "a modified command" text as command.
    //    10. Go to the last page, accept the dialog.
    class ModifyScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(nullptr != wizard, "Can't cast current dialog to QWizard");

            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));

            GTTextEdit::setText(GTWidget::findTextEdit("teCommand", dialog), "a modified command");

            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));

            GTWidget::click(wizard->button(QWizard::FinishButton));
        }
    };

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(new ModifyScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit configuration..."}));
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 1", QPoint(), Qt::RightButton);

    //    11. Edit the element again.
    //    12. Go to the "Command" page in the wizard.
    //    Expected state: the command is "a modified command".
    class CheckScenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(nullptr != wizard, "Can't cast current dialog to QWizard");

            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));

            const QString expectedText = "a modified command";
            const QString actualText = GTTextEdit::getText(GTWidget::findTextEdit("teCommand", dialog));
            CHECK_SET_ERR(actualText == expectedText, QString("Unexpected command text: expected '%1', got '%2'").arg(expectedText).arg(actualText));

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsWorkflowDesigner::removeItem("UGENE-6488 test element 1");
    GTUtilsWorkflowDesigner::addElement("UGENE-6488 test element 1");

    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(new CheckScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Edit configuration..."}));
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 1", QPoint(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_6488_2) {
    //    1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click "Create element with external tool" button on the toolbar.
    //    3. Fill the dialog with the following data:
    //        Element name: "UGENE-6488 test element"
    //        Command line tool: Integrated external tool "python"
    //        No inputs, parameters and outputs
    //        Command: "just a command"
    //        Element description on the scene: "description on the scene"
    //        Detailed element description: "detailed element description"
    //    4. Accept the dialog.
    //    Expected state: the element was created and put on the scene.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "UGENE-6488 test element 2";
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.command = "just a command";
    settings.description = "detailed element description";
    settings.prompter = "description on the scene";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Click on the element on the scene.
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 2");

    //    6. Call a context menu on the element on the scene.
    //    7. Select "Edit configuration..." menu item.
    //    8. Go to the "Element appearance" page in the wizard.
    //    9. Set "a modified description on the scene" text as element description on the scene.
    //    10. Go to the last page, accept the dialog.
    class ModifyScenario : public CustomScenario {
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard != nullptr, "Can't cast current dialog to QWizard");

            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));

            GTTextEdit::setText(GTWidget::findTextEdit("tePrompter", dialog), "a modified description on the scene");

            GTWidget::click(wizard->button(QWizard::NextButton));

            GTWidget::click(wizard->button(QWizard::FinishButton));
        }
    };

    GTUtilsDialog::add(new PopupChooserByText({"Edit configuration..."}));
    GTUtilsDialog::add(new CreateElementWithCommandLineToolFiller(new ModifyScenario()));
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 2", {}, Qt::RightButton);

    //    11. Edit the element again.
    //    12. Go to the "Element appearance" page in the wizard.
    //    Expected state: the element description on the scene is "a modified description on the scene".
    class CheckScenario : public CustomScenario {
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();
            auto wizard = qobject_cast<QWizard*>(dialog);
            CHECK_SET_ERR(wizard != nullptr, "Can't cast current dialog to QWizard");

            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));
            GTWidget::click(wizard->button(QWizard::NextButton));

            const QString expectedText = "a modified description on the scene";
            const QString actualText = GTTextEdit::getText(GTWidget::findTextEdit("tePrompter", dialog));
            CHECK_SET_ERR(actualText == expectedText, QString("Unexpected command text: expected '%1', got '%2'").arg(expectedText).arg(actualText));

            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    };

    GTUtilsWorkflowDesigner::removeItem("UGENE-6488 test element 2");
    GTUtilsWorkflowDesigner::addElement("UGENE-6488 test element 2");

    GTUtilsDialog::add(new PopupChooserByText({"Edit configuration..."}));
    GTUtilsDialog::add(new CreateElementWithCommandLineToolFiller(new CheckScenario()));
    GTUtilsWorkflowDesigner::click("UGENE-6488 test element 2", QPoint(), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_6490) {
    //    Test to check that element with external tool will
    //    successfully create and run the command: `%TOOL_PATH% $oooo $oooo$oooo $oooo $oooo$oooo$oooo`.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Create element with external tool" button on the toolbar.
    //    3. Fill the wizard with the following values (not mentioned values can be set with any value):
    //        Parameters page: a parameter with a type "Output file URL".
    //    4. Accept the wizard.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "test_6490";
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.parameters << CreateElementWithCommandLineToolFiller::ParameterData("oooo",
                                                                                 qMakePair(CreateElementWithCommandLineToolFiller::ParameterString, QString("-version")),
                                                                                 QString("Desc-version"),
                                                                                 QString("OoOoO"));

    settings.command = "%USUPP_JAVA% $oooo $oooo$oooo $oooo $oooo$oooo$oooo";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click("test_6490");

    //    6. Launch the workflow.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there is an log string `-version -version-version -version -version-version-version`.
    bool desiredMessage = lt.hasMessage("$oooo $oooo$oooo $oooo $oooo$oooo$oooo");
    CHECK_SET_ERR(desiredMessage, "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_6541_1) {
    //  1. Open "COI_SHORT_21x88.aln".
    GTFileDialog::openFile(testDir + "_common_data/realign_sequences_in_alignment/", "COI_SHORT_21x70.aln");

    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    QAbstractButton* realignButton = GTAction::button("align_selected_sequences_to_alignment");
    //         Expected result : no sequences are selected.
    //         Expected result : the "Realign sequence(s) to other sequences" button is disabled.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect({});
    CHECK_SET_ERR(!realignButton->isEnabled(), "'align_selected_sequences_to_alignment' is unexpectedly enabled");

    //         Select all sequences in the alignment.
    //         Expected result : the "align_selected_sequences_to_alignment" button is disabled.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(69, 20));
    CHECK_SET_ERR(!realignButton->isEnabled(), "'align_selected_sequences_to_alignment' is unexpectedly enabled");

    //         Select none sequences.
    //         Expected result : the "align_selected_sequences_to_alignment" button is disabled.
    GTUtilsMSAEditorSequenceArea::cancelSelection();
    CHECK_SET_ERR(!realignButton->isEnabled(), "'align_selected_sequences_to_alignment' is unexpectedly enabled");

    //         Select 3 sequences in the alignment.
    //         Expected result : the "align_selected_sequences_to_alignment" button is enabled.
    GTUtilsMsaEditor::selectRows(18, 20);
    CHECK_SET_ERR(realignButton->isEnabled(), "'align_selected_sequences_to_alignment' is unexpectedly disabled");
    //         Click "align_selected_sequences_to_alignment".
    //         Expected result : the sequences are realigned.

    GTUtilsDialog::add(new PopupChooser({"align_selection_to_alignment_mafft"}));
    GTWidget::click(realignButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");
    //         Open "empty_mult_seq.fa".
    //         Expected result : there are no sequences in the Realignment Editor.The "align_selected_sequences_to_alignment" button is disabled.
    GTUtilsProject::closeProject(true, true);
    GTUtilsDialog::checkNoActiveWaiters();

    GTFileDialog::openFile(testDir + "_common_data/empty_sequences/", "empty_mult_seq.fa");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    realignButton = GTAction::button("align_selected_sequences_to_alignment");
    CHECK_SET_ERR(!realignButton->isEnabled(), "'align_selected_sequences_to_alignment' is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_6541_2) {
    //  Open "COI_SHORT_21x88_russian_letters.msf".
    //  Select ���� ����    ���Ɣ, ��� �� ���� ����, �����   ���� ���� ������ sequences.
    //  Expected result : "align_selected_sequences_to_alignment" button is enabled.
    GTFileDialog::openFile(testDir + "_common_data/realign_sequences_in_alignment/", "COI_SHORT_21x88_russian_letters.msf");
    GTUtilsMsaEditor::selectRows(18, 20);

    GTUtilsDialog::waitForDialog(new PopupChooser({"align_selection_to_alignment_mafft"}));
    QAbstractButton* realignButton = GTAction::button("align_selected_sequences_to_alignment");
    CHECK_SET_ERR(realignButton->isEnabled(), "'align_selected_sequences_to_alignment' button is unexpectedly disabled");
    //  Click "align_selected_sequences_to_alignment".
    //  Expected result : sequences realigned.
    GTWidget::click(realignButton);

    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectedly disabled");
}

GUI_TEST_CLASS_DEFINITION(test_6541_3) {
    //     Open "amino_ext.aln".
    GTFileDialog::openFile(testDir + "_common_data/realign_sequences_in_alignment/", "amino_ext.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //     Select "FOSB_MOUSE" sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence("FOSB_MOUSE");

    //     Expected result : "align_selected_sequences_to_alignment" button is enabled.
    QAbstractButton* realignButton = GTAction::button("align_selected_sequences_to_alignment");
    CHECK_SET_ERR(realignButton->isEnabled(), "'align_selected_sequences_to_alignment' button is unexpectedly disabled");

    //     Click "align_selected_sequences_to_alignment".
    GTUtilsDialog::add(new PopupChooser({"align_selection_to_alignment_mafft"}));
    GTWidget::click(realignButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    //     Expected result : sequences realigned.
    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectably disabled");
    GTUtilsProject::closeProject(true, true);

    //     Open �protein.aln�.
    GTFileDialog::openFile(testDir + "_common_data/realign_sequences_in_alignment/", "protein.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //     Select �Loach�, �Frog�, "Human" sequences.
    GTUtilsMsaEditor::selectRows(3, 5);

    //     Expected result : "align_selected_sequences_to_alignment" button is enabled.
    realignButton = GTAction::button("align_selected_sequences_to_alignment");
    CHECK_SET_ERR(realignButton->isEnabled(), "'align_selected_sequences_to_alignment' button is unexpectedly disabled");

    //     Click "align_selected_sequences_to_alignment".
    GTUtilsDialog::add(new PopupChooser({"align_selection_to_alignment_mafft"}));
    GTWidget::click(realignButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    //     Expected result : sequences realigned.
    undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectably disabled");
    GTUtilsProject::closeProject(true, true);

    //     Open �RAW.aln�.Select any sequence.
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "RAW.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //     Expected result : "align_selected_sequences_to_alignment" button is disabled.
    realignButton = GTAction::button("align_selected_sequences_to_alignment");
    CHECK_SET_ERR(!realignButton->isEnabled(), "'align_selected_sequences_to_alignment' button is unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_6544) {
    // 1. Open a DNA sequence in the SV.
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "Search in Sequence" tab on the options panel.
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Input a pattern that contains a character of the extended DNA alphabet, e.g. "ACWT".

    QWidget* patternInputLine = QApplication::focusWidget();
    CHECK_SET_ERR(patternInputLine != nullptr && patternInputLine->objectName() == "textPattern", "Focus is not on FindPattern widget");

    GTKeyboardDriver::keySequence("ACWT");

    // 4. Set algorithm to "Substitute" in the "Search algorithm" group.
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");

    // 5. Expected/current result: the search field background is red.
    auto editPatterns = GTWidget::findPlainTextEdit("textPattern");
    QString style0 = editPatterns->styleSheet();
    CHECK_SET_ERR(style0 == "background-color: " + GUIUtils::WARNING_COLOR.name() + ";", "unexpected styleSheet: " + style0);

    // 6. Make the "Search with ambiguous bases" option checked.

    GTUtilsOptionPanelSequenceView::setSearchWithAmbiguousBases();

    // 7. Expected result: the search field should have white background.
    QString style1 = editPatterns->styleSheet();
    CHECK_SET_ERR(style1 == "background-color: " + GUIUtils::OK_COLOR.name() + ";", "unexpected styleSheet: " + style1);
}

GUI_TEST_CLASS_DEFINITION(test_6546) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a square region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(3, 3));

    // Hold Ctrl key and click on a sequence name.
    GTUtilsMsaEditor::moveToSequenceName("Montana_montana");  // Y = 4.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Check that selection was extended by 1 row and X range is not changed.
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 4}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_1) {
    // Open an alignment in the Alignment Editor.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a horizontal line region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(3, 1));

    // Hold Ctrl key and click on a sequence name.
    GTUtilsMsaEditor::moveToSequenceName("Montana_montana");  // Y = 4.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: selection has 2 areas.
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 1}, {1, 4, 3, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_2) {
    // Open an alignment in the Alignment Editor.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a horizontal line region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 4), QPoint(4, 4));

    // Hold Ctrl key and click on the selected sequence name.
    GTUtilsMsaEditor::moveToSequenceName("Montana_montana");  // Y = 4
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: selection is empty.
    GTUtilsMsaEditor::checkSelection({});
}

GUI_TEST_CLASS_DEFINITION(test_6546_3) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a square region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(5, 5));

    // Hold Ctrl key and click on a sequence name in the middle of the region.
    GTUtilsMsaEditor::moveToSequenceName("Montana_montana");  // Y = 4
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: sequence in the middle was excluded from the selection.
    GTUtilsMsaEditor::checkSelection({{1, 1, 5, 3}, {1, 5, 5, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_4) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a horizontal line region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 3), QPoint(3, 3));

    // Extend it with Control.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequenceName("Montana_montana");  // Y = 4.

    // Extend it with Shift: the last cursor position is 4.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");  // Y = 8
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: selection was extended up to Zychia_baranovi (8), sequence 3 was excluded (cursor was on 4).
    GTUtilsMsaEditor::checkSelection({{1, 4, 3, 5}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_5) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a horizontal line region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(3, 1));  // Y = 1.

    // Make a multi-selection.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");  // Y = 8.
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Try to extend it with Shift: click on the first sequence again.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Isophya_altaica_EF540820");  // Y = 1.
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected state: the selection is from 8 to 1 (y).
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 8}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_6) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select a square region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(3, 3));

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected state: Shift has priority over the control.
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 8}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_7) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that selection can have 3 areas.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequenceName("Bicolorana_bicolor_EF540830");  // Y = 2.
    GTUtilsMsaEditor::clickSequenceName("Podisma_sapporensis");  // Y = 16.
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");  // Y = 8.
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    GTUtilsMsaEditor::checkSelection({{0, 2, 604, 1}, {0, 8, 604, 1}, {0, 16, 604, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_8) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that click with Shift selects an area inside but does not change cursor position.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 1), QPoint(3, 1));

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Zychia_baranovi");  // Y = 8.
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 8}});

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Montana_montana");  // Y = 4.
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMsaEditor::checkSelection({{1, 1, 3, 4}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_9) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that click with Shift on a single line selection does not toggle its state.
    GTUtilsMsaEditor::clickSequenceName("Isophya_altaica_EF540820");
    GTUtilsMsaEditor::checkSelection({{0, 1, 604, 1}});

    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Isophya_altaica_EF540820");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTUtilsMsaEditor::checkSelection({{0, 1, 604, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_6546_10) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that Ctrl + clicks works for first/last sequences correctly.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequence(0);
    GTUtilsMsaEditor::clickSequence(17);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsMsaEditor::checkSelection({{0, 0, 604, 1}, {0, 17, 604, 1}});

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequence(0);
    GTUtilsMsaEditor::clickSequence(17);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsMsaEditor::checkSelection({});
}

GUI_TEST_CLASS_DEFINITION(test_6546_11) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that Ctrl + clicks works for first/last sequences correctly.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsMsaEditor::clickSequence(0);
    GTUtilsMsaEditor::clickSequence(12);
    GTUtilsMsaEditor::clickSequence(17);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsMsaEditor::checkSelection({{0, 0, 604, 1}, {0, 12, 604, 1}, {0, 17, 604, 1}});

    GTUtilsMsaEditor::clickSequence(12);
    GTUtilsMsaEditor::checkSelection({{0, 12, 604, 1}});
}

GUI_TEST_CLASS_DEFINITION(test_6548_1) {
    // 1. Open _common_data/scenarios/_regression/6548/6548_extended_DNA.aln.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6548/6548_extended_DNA.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Open OP tab and select "Weak similarities" color scheme
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Weak similarities");

    // Zoom in multiple times to make chars bigger. (Do not zoom to max because it will move some chars out of the screen on Windows agents).
    for (int i = 0; i < 5; i++) {
        GTUtilsMSAEditorSequenceArea::zoomIn();
    }

    // First column check
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(0, 0), "#0000ff", "#00ffff");
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(0, 3), "#000000", "#ffffff");  // Gap symbol

    // Second column check
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 5), "#ff00ff", "#ffffff");  // Second frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 9), "#000000", "#ffffff");  // Third frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 12), "#000000", "#c0c0c0");  // Fourth frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 14), "#ff6600", "#ffffff");  // Fifth frequent symbol

    // Third column check: T > G > C > A > R
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 3), "#0000ff", "#00ffff");  // T
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 2), "#ff00ff", "#ffffff");  // G
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 1), "#000000", "#ffffff");  // C
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 0), "#000000", "#c0c0c0");  // A
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 4), "#ff6600", "#ffffff");  // R

    // Fourth column: M > S > V > W > Y
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 0), "#0000ff", "#00ffff");  // M
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 2), "#ff00ff", "#ffffff");  // S
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 4), "#000000", "#ffffff");  // V
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 1), "#000000", "#c0c0c0");  // W
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 3), "#ff6600", "#ffffff");  // Y

    // Fifth column: D > H > K > N > X
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(4, 2), "#0000ff", "#00ffff");  // D
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(4, 1), "#ff00ff", "#ffffff");  // H
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(4, 0), "#000000", "#ffffff");  // K
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(4, 3), "#000000", "#c0c0c0");  // N
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(4, 4), "#ff6600", "#ffffff");  // X
}

GUI_TEST_CLASS_DEFINITION(test_6548_2) {
    // NOTE: here is problems with detecting #ff00ff color so double condition is used fontColor == "#ff00ff" || fontColor == "#ff66ff"
    // 1. Open _common_data/clustal/RNA_nucl_ext_rand_seq.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal/RNA_nucl_ext_rand_seq.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open OP tab and select "Weak similarities" color scheme
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Highlighting);
    auto colorScheme = GTWidget::findComboBox("colorScheme");
    GTComboBox::selectItemByText(colorScheme, "Weak similarities");

    // Zoom to max.
    GTUtilsMSAEditorSequenceArea::zoomToMax();

    // Second column check
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 1), "#0000ff", "#00ffff");  // Second frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 0), "#ff00ff", "#ffffff");  // Third frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 2), "#000000", "#ffffff");  // Fourth frequent symbol
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(1, 3), "#000000", "#c0c0c0");  // Fifth frequent symbol

    // Third column check: A > R > Y
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 3), "#0000ff", "#00ffff");  // A
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 2), "#ff00ff", "#ffffff");  // R
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 1), "#000000", "#ffffff");  // Y
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(2, 0), "#000000", "#ffffff");  // gap

    // Fourth column: S > W
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 0), "#000000", "#ffffff");  // S
    GTUtilsMSAEditorSequenceArea::checkMsaCellColors(QPoint(3, 1), "#000000", "#c0c0c0");  // W
}

GUI_TEST_CLASS_DEFINITION(test_6564) {
    // 1. Open general/_common_data/scenarios/msa/ma2_gap_col.aln.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Enable "Collapsing mode". As result 2 names in the name list are hidden.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select a region in the first sequence (click on any base of the sequence).
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 0));  // Y = 0.

    // 4. Press_ Shift_ and click to the sequence number 3 in the name list (on the left).
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Bicolorana_bicolor_EF540830");  // Y = 2.
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // 5. Sequences 0, 1, 2 selected (because _Shift_ was used), X-range was not changed.
    GTUtilsMsaEditor::checkSelection({{0, 0, 4, 3}});
}

GUI_TEST_CLASS_DEFINITION(test_6566) {
    // 1. Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Enable "Collapsing mode"
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Click on any character of "Phaneroptera_falcata" sequence
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));

    // 4. Click on "Phaneroptera_falcata" name in the Name List
    GTUtilsMsaEditor::clickSequenceName("Phaneroptera_falcata");

    // 5. Expected result: the "Phaneroptera_falcata" sequence is selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 604, 1));
}

GUI_TEST_CLASS_DEFINITION(test_6569) {
    // 1. Open the WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // 2. Find the "Grouper" element on the palette, click on it and click on the scene.
    WorkflowProcessItem* grouperElement = GTUtilsWorkflowDesigner::addElement("Grouper");
    GTUtilsWorkflowDesigner::click(grouperElement);

    // Expected: No crash
}

GUI_TEST_CLASS_DEFINITION(test_6580) {
    //    Test to check that element with external tool will
    //    successfully create and run the command: `%TOOL_PATH% $oooo $oooo$oooo $oooo $oooo$oooo$oooo`.

    //    1. Open the Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Click on the "Create element with external tool" button on the toolbar.
    //    3. Fill the wizard with the following values (not mentioned values can be set with any value):
    //        Parameters page: a parameter with a type "Output file URL".
    //    4. Accept the wizard.
    CreateElementWithCommandLineToolFiller::ElementWithCommandLineSettings settings;
    settings.elementName = "test_6580";
    settings.tooltype = CreateElementWithCommandLineToolFiller::CommandLineToolType::IntegratedExternalTool;
    settings.parameters << CreateElementWithCommandLineToolFiller::ParameterData("oooo",
                                                                                 qMakePair(CreateElementWithCommandLineToolFiller::ParameterString, QString("-version")),
                                                                                 QString("Desc-version"),
                                                                                 QString("OoOoO"));

    settings.command = "%USUPP_JAVA% $OoOoO $OoOoO$OoOoO $OoOoO $OoOoO$OoOoO$OoOoO";
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(settings));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Create element with external tool");

    //    5. Create a valid workflow with the new element.
    GTUtilsWorkflowDesigner::click("test_6580");

    //    6. Launch the workflow.
    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the workflow execution finishes, there is an log string `-version -version-version -version -version-version-version`.
    bool desiredMessage = lt.hasMessage("-version -version-version -version -version-version-version");
    CHECK_SET_ERR(desiredMessage, "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_6581) {
    // 1. Open "test/general/_common_data/scenarios/_regression/6581/COI_modified_one_char_sequence.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6581", "COI_modified_one_char_sequence.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Align sequence to this alignment" button on the toolbarand select attached sequence "Sequence.txt".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/scenarios/_regression/6581/Sequence.txt"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the sequence has been aligned to the alignment.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 18, QString("Unexpected name list size, expected: 18, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[17] == "F", QString("Unexpected name, expected: \"F\", current: %1").arg(names[17]));
}

GUI_TEST_CLASS_DEFINITION(test_6586_1) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 3. Skip the first page
            auto rbIntegratedTool = GTWidget::findRadioButton("rbIntegratedTool", dialog);

            GTRadioButton::click(rbIntegratedTool);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QStringList addButtonNames = {"pbAddInput", "pbAdd", "pbAddOutput"};
            QStringList dataTableNames = {"tvInput", "tvAttributes", "tvOutput"};
            QStringList deleteButtonNames = {"pbDeleteInput", "pbDelete", "pbDeleteOutput"};

            for (int i = 0; i < addButtonNames.size(); i++) {
                // 4. Add two rows and text "name"
                auto add = GTWidget::findWidget(addButtonNames[i], dialog);

                auto table = GTWidget::findTableView(dataTableNames[i]);

                GTWidget::click(add);
                GTWidget::click(add);

                // Expected: focus on the table view, the first column of the second row was selected
                CHECK_SET_ERR(table->hasFocus(), "QTableView doesn't have focus");

                GTKeyboardDriver::keySequence("name");
                QAbstractItemModel* baseModel = table->model();
                CHECK_SET_ERR(nullptr != baseModel, "QItemSelectionModel not found");

                QString expectedString = baseModel->data(baseModel->index(1, 0)).toString();
                CHECK_SET_ERR(expectedString == "name", QString("Expected string not found, expected: name, current: %1").arg(expectedString));

                auto del = GTWidget::findWidget(deleteButtonNames[i], dialog);

                GTWidget::click(del);
                GTWidget::click(del);

                // 5. Go to the nex page and process 4 again
                GTUtilsWizard::clickButton(GTUtilsWizard::Next);
                // 6. 4-5 should be done 3 times in general
            }

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    // 2. Open "Create Cmdline Based Worker Wizard"
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(new Scenario));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);
}

GUI_TEST_CLASS_DEFINITION(test_6586_2) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // 3. Skip the first page
            auto rbIntegratedTool = GTWidget::findRadioButton("rbIntegratedTool", dialog);

            GTRadioButton::click(rbIntegratedTool);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QStringList addButtonNames = {"pbAddInput", "pbAdd", "pbAddOutput"};
            QStringList dataTableNames = {"tvInput", "tvAttributes", "tvOutput"};
            QStringList deleteButtonNames = {"pbDeleteInput", "pbDelete", "pbDeleteOutput"};

            for (int i = 0; i < addButtonNames.size(); i++) {
                // 4. Add two rows, remove one and text "name"
                auto add = GTWidget::findWidget(addButtonNames[i], dialog);

                auto table = GTWidget::findTableView(dataTableNames[i]);

                auto del = GTWidget::findWidget(deleteButtonNames[i], dialog);

                GTWidget::click(add);
                GTWidget::click(add);
                GTWidget::click(del);

                // Expected: focus on the table view, the first column of the second row was selected
                CHECK_SET_ERR(table->hasFocus(), "QTableView doesn't have focus");

                GTKeyboardDriver::keySequence("name");
                QAbstractItemModel* baseModel = table->model();
                CHECK_SET_ERR(nullptr != baseModel, "QItemSelectionModel not found");

                QString expectedString = baseModel->data(baseModel->index(0, 0)).toString();
                CHECK_SET_ERR(expectedString == "name", QString("Expected string not found, expected: name, current: %1").arg(expectedString));

                GTWidget::click(del);

                // 5. Go to the nex page and process 4 again
                GTUtilsWizard::clickButton(GTUtilsWizard::Next);
                // 6. 4-5 should be done 3 times in general
            }

            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    // 2. Open "Create Cmdline Based Worker Wizard"
    GTUtilsDialog::waitForDialog(new CreateElementWithCommandLineToolFiller(new Scenario));
    QAbstractButton* createElement = GTAction::button("createElementWithCommandLineTool");
    GTWidget::click(createElement);
}

GUI_TEST_CLASS_DEFINITION(test_6616_1) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Hide overview", "Hide zoom view" and "Hide details view"
    auto toolbar = GTWidget::findWidget("views_tool_bar_NC_001363");

    GTWidget::click(GTWidget::findWidget("show_hide_overview", toolbar));

    GTWidget::click(GTWidget::findWidget("show_hide_details_view", toolbar));

    GTWidget::click(GTWidget::findWidget("show_hide_zoom_view", toolbar));

    // 3. Close the project and open it again
    GTUtilsProject::closeProject(true);
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Overiview, Details view and Zoom view are invisible
    toolbar = GTWidget::findWidget("views_tool_bar_NC_001363");

    QStringList views = {
        "show_hide_overview",
        "show_hide_details_view",
        "show_hide_zoom_view",
    };
    foreach (const QString& v, views) {
        auto button = GTWidget::findToolButton(v, toolbar);
        CHECK_SET_ERR(!button->isChecked(), QString("%1 QToolButton should bew unchecked").arg(v));
    }
}

GUI_TEST_CLASS_DEFINITION(test_6616_2) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Disable "Wrap sequence", "Show complementary strand"
    QAction* wrapMode = GTAction::findActionByText("Wrap sequence");
    CHECK_SET_ERR(wrapMode != nullptr, "Cannot find Wrap sequence action");
    if (wrapMode->isChecked()) {
        GTWidget::click(GTAction::button(wrapMode));
    }

    QAction* compStrand = GTAction::findActionByText("Show complementary strand");
    CHECK_SET_ERR(compStrand != nullptr, "Cannot find \"Show complementary strand\" action");

    GTWidget::click(GTAction::button(compStrand));

    // 3. Choose "Translate selection" mode
    auto translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChooser({"translate_selection_radiobutton"}));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 4. Close the project and open it again
    GTUtilsProject::closeProject(true);
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "Wrap sequence", "Show complementary strand" are disabled, all graphs are visible
    wrapMode = GTAction::findActionByText("Wrap sequence");
    CHECK_SET_ERR(wrapMode != nullptr, "Cannot find Wrap sequence action");
    CHECK_SET_ERR(!wrapMode->isChecked(), "Wrap sequence mode should be disabled");

    compStrand = GTAction::findActionByText("Show complementary strand");
    CHECK_SET_ERR(compStrand != nullptr, "Cannot find Wrap sequence action");
    CHECK_SET_ERR(!compStrand->isChecked(), "Show complementary strand mode should be disabled");

    translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChecker({"translate_selection_radiobutton"}, PopupChecker::IsChecked));
    GTWidget::click(translationsMenuToolbarButton);
}

GUI_TEST_CLASS_DEFINITION(test_6616_3) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Choose "Set up frames manually" mode and disable the all frames
    GTUtilsSequenceView::getActiveSequenceViewWindow();
    auto translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChooser({"set_up_frames_manually_radiobutton"}));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    QStringList frames = {"Frame +1", "Frame +2", "Frame +3", "Frame -1", "Frame -2", "Frame -3"};
    foreach (const QString& frame, frames) {
        GTUtilsDialog::waitForDialog(new PopupChooserByText({frame}));
    }
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Close the project and open it again
    GTUtilsProject::closeProject(true);
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "Set up frames manually" mode is choosen and the all frames are disable
    GTUtilsSequenceView::getActiveSequenceViewWindow();
    translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChecker({"set_up_frames_manually_radiobutton"}, PopupChecker::IsChecked));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText(QStringList(), frames));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}

GUI_TEST_CLASS_DEFINITION(test_6616_4) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Choose "Show all frames" mode
    auto translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChooser({"show_all_frames_radiobutton"}));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Close the project and open it again
    GTUtilsProject::closeProject(true);
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "Show all frames" mode is choosen
    translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChecker({"show_all_frames_radiobutton"}, PopupChecker::IsChecked));
    GTWidget::click(translationsMenuToolbarButton);
}

GUI_TEST_CLASS_DEFINITION(test_6616_5) {
    // 1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Toggle annotation density graph"
    QAction* destGraph = GTAction::findAction("density_graph_action");
    CHECK_SET_ERR(destGraph != nullptr, "Cannot find \"Toggle annotation density graph\" action");

    GTWidget::click(GTAction::button(destGraph));

    // 3. Close the project and open it again
    GTUtilsProject::closeProject(true);
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: "Toggle annotation density graph" is turned on

    destGraph = GTAction::findAction("density_graph_action");
    CHECK_SET_ERR(destGraph != nullptr, "Cannot find \"Toggle annotation density graph\" action");
    CHECK_SET_ERR(destGraph->isChecked(), "\"Toggle annotation density graph\" is unchecked, but should be");
}

GUI_TEST_CLASS_DEFINITION(test_6619) {
    // 1. Open "_common_data/scenarios/_regression/6619/murine2.gb".
    GTLogTracer lt;
    GTFileDialog::openFile(testDir + "_common_data/scenarios/_regression/6619", "murine2.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click on the "NP 597742.2" in the Zoom view.
    GTUtilsSequenceView::clickAnnotationPan("CDS", 2970);

    // 3. Click on the 3000th character in the Details view.
    GTUtilsSequenceView::setCursor(3000, true);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Expected: No errors
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_6620) {
    // 1. Open "test/general/_common_data/sanger/sanger_01.ab1".
    GTFileDialog::openFile(testDir + "_common_data/sanger", "sanger_01.ab1");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: Details view is visible
    auto toolbar = GTWidget::findWidget("views_tool_bar_SZYD_Cas9_5B70");

    auto button = GTWidget::findToolButton("show_hide_details_view", toolbar);
    CHECK_SET_ERR(button->isChecked(), "show_hide_details_view QToolButton should bew checked");
}

GUI_TEST_CLASS_DEFINITION(test_6628_1) {
    // 1.  Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Make sure, that MAFFT is valid
    GTUtilsExternalTools::checkValidation("MAFFT");

    // 3. Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // 4. Select "_common_data\empty_sequences\multifasta_with_gap_seq.fa".
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/multifasta_with_gap_seq.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsNotifications::waitForNotification(true, "The following sequence(s) were not aligned as they do not contain meaningful characters: \"seq2\", \"seq4\".");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result : sequences made of gaps only are not aligned, i.e. "seq1", "seq3"and "seq5" are aligned.
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment + 3,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment + 3).arg(sequenceNumberAfterAlignment));
    GTUtilsMSAEditorSequenceArea::hasSequencesWithNames({"seq1", "seq3", "seq5"});

    // Also, an error it the log appears:The following sequence(s) were not aligned as they do not contain meaningful characters: "seq2", "seq4".
    CHECK_SET_ERR(lt.hasError("The following sequence(s) were not aligned as they do not contain meaningful characters: \"seq2\", \"seq4\"."), "Expected error not found");

    // The "Undo" button is enabled
    CHECK_SET_ERR(GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is disabled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_2) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // Select "_common_data\empty_sequences\multifasta_with_gap_seq.fa".
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/multifasta_with_gap_seq.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsNotifications::waitForNotification(true, "The following sequence(s) were not aligned as they do not contain meaningful characters: \"seq2\", \"seq4\".");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result : sequences made of gaps only are not aligned, i.e. "seq1", "seq3"and "seq5" are aligned.
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment + 3,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment + 3).arg(sequenceNumberAfterAlignment));
    GTUtilsMSAEditorSequenceArea::hasSequencesWithNames({"seq1", "seq3", "seq5"});

    // Also, an error it the log appears:The following sequence(s) were not aligned as they do not contain meaningful characters: "seq2", "seq4".
    CHECK_SET_ERR(lt.hasError("The following sequence(s) were not aligned as they do not contain meaningful characters: \"seq2\", \"seq4\"."), "Expected error not found");

    // The "Undo" button is enabled
    CHECK_SET_ERR(GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is disabled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_3) {
    // Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // Select "_common_data\empty_sequences\gap_only_seq.fa".
    int sequenceCountBefore = GTUtilsMsaEditor::getSequencesCount();

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/gap_only_seq.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("UGENE");
    GTUtilsNotifications::waitForNotification(true, "The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\".");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the alignment is not modified.
    int sequenceCountAfter = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceCountAfter == sequenceCountBefore,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceCountBefore).arg(sequenceCountAfter));

    // Also, an error it the log appears: The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\".
    CHECK_SET_ERR(lt.hasError("The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\"."), "Expected error not found");

    // The "Undo" button is disabled
    CHECK_SET_ERR(!GTUtilsMsaEditor::isUndoEnabled(), "The Undo button is enabled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_4) {
    // 1.  Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Make sure, that MAFFT is valid
    GTUtilsExternalTools::checkValidation("MAFFT");

    // 3. Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // 4. Select "_common_data\empty_sequences\gap_only_seq.fa".
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/gap_only_seq.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsNotifications::waitForNotification(true, "The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\".");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the alignment is not modified.
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment).arg(sequenceNumberAfterAlignment));

    // Also, an error it the log appears: The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\".
    CHECK_SET_ERR(lt.hasError("The following sequence(s) were not aligned as they do not contain meaningful characters: \"gap-only-sequence\"."), "Expected error not found");

    // The "Undo" button is disabled
    CHECK_SET_ERR(!GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is enebled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_5) {
    // 1.  Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // 3. Select "_common_data\empty_sequences\empty_file.fa".
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/empty_file.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsNotifications::waitForNotification(true, "'Load sequences and add to alignment task' task failed: Data from the \"empty_file.fa\" file can't be alignment to the \"COI\" alignment - the file is empty.");
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the COI alignment is not modified,
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment).arg(sequenceNumberAfterAlignment));

    // but the error notification is the following: Data from the "empty-file.fa" file can't be alignment to the "COI" alignment - the file is empty.
    CHECK_SET_ERR(lt.hasError("Task {Load sequences and add to alignment task} finished with error: Data from the \"empty_file.fa\" file can't be alignment to the \"COI\" alignment - the file is empty."), "Expected error not found");

    // The "Undo" button is disabled
    CHECK_SET_ERR(!GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is enabled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_6) {
    // 1.  Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // 3. Select "_common_data\empty_sequences\incorrect_fasta_header_only.fa".
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/incorrect_fasta_header_only.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsNotifications::waitForNotification(true, "'Load sequences and add to alignment task' task failed: Data from the \"incorrect_fasta_header_only.fa\" file can't be alignment to the \"COI\" alignment - the file format is invalid.");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();

    // Expected result: the COI alignment is not modified,
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment).arg(sequenceNumberAfterAlignment));

    // but the error notification is the following: Data from the "empty-file.fa" file can't be alignment to the "COI" alignment - the file format is invalid.
    CHECK_SET_ERR(lt.hasError("Task {Load sequences and add to alignment task} finished with error: Data from the \"incorrect_fasta_header_only.fa\" file can't be alignment to the \"COI\" alignment - the file format is invalid."), "Expected error not found");

    // The "Undo" button is disabled
    CHECK_SET_ERR(!GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is enabled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6628_7) {
    // 1.  Open "COI.aln" file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Click "align_new_sequences_to_alignment_action" button on the Alignment Editor toolbar.
    // 3. Select "_common_data\empty_sequences\incorrect_multifasta_with_empty_seq.fa".
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();
    int sequenceNumberBeforeAlignment = GTUtilsMsaEditor::getSequencesCount();
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/empty_sequences/incorrect_multifasta_with_empty_seq.fa"));
    GTUtilsMsaEditor::activateAlignSequencesToAlignmentMenu("MAFFT");
    GTUtilsNotifications::waitForNotification(true, "'Load sequences and add to alignment task' task failed: Data from the \"incorrect_multifasta_with_empty_seq.fa\" file can't be alignment to the \"COI\" alignment - the file format is invalid.");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the COI alignment is not modified,
    int sequenceNumberAfterAlignment = GTUtilsMsaEditor::getSequencesCount();
    CHECK_SET_ERR(sequenceNumberAfterAlignment == sequenceNumberBeforeAlignment,
                  QString("Unexpected number of sequences, expected: %1, current: %2").arg(sequenceNumberBeforeAlignment).arg(sequenceNumberAfterAlignment));

    // but the error notification is the following: Data from the "empty-file.fa" file can't be alignment to the "COI" alignment - the file format is invalid.
    CHECK_SET_ERR(lt.hasError("Task {Load sequences and add to alignment task} finished with error: Data from the \"incorrect_multifasta_with_empty_seq.fa\" file can't be alignment to the \"COI\" alignment - the file format is invalid."), "Expected error not found");

    // The "Undo" button is disabled
    CHECK_SET_ERR(!GTUtilsMsaEditor::isUndoEnabled(), "The \"Undo\" button is enebled, but shouldn't be");
}

GUI_TEST_CLASS_DEFINITION(test_6636) {
    // 1. Open "_common_data/clustal/COI_and_short.aln".
    GTFileDialog::openFile(testDir + "_common_data/clustal/COI_and_short.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // 2. Select "clipboard16" sequence. Drag and drop this sequence a bit.
    // Expected state: "Undo" button is enabled
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 18), QPoint(19, 18));
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(9, 18), QPoint(18, 18));

    QAbstractButton* undoButton = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undoButton->isEnabled(), "'Undo' button is unexpectably disabled");
}

GUI_TEST_CLASS_DEFINITION(test_6640) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Press Esc key
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 3. Expected state: No any selection
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 0, 0));
}

GUI_TEST_CLASS_DEFINITION(test_6640_1) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select second sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Isophya_altaica_EF540820");

    // 3. Press Esc key
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 4. Expected state: No any selected sequence
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 0, 0));
}

GUI_TEST_CLASS_DEFINITION(test_6640_2) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select one column in consensus
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(1);

    // 3. Expected state: All sequences are selected
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(1, 0, 1, 18));
}

GUI_TEST_CLASS_DEFINITION(test_6640_3) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select one sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Isophya_altaica_EF540820");

    // 3. Expected state: Consensus changes background color from white to grey
}

GUI_TEST_CLASS_DEFINITION(test_6640_4) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Show chromatograms" button on the toolbar.
    GTUtilsMcaEditor::toggleShowChromatogramsMode();

    // 3. Select 4 reads
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR51");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B70");
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_5B71");
    GTUtilsMcaEditor::clickReadName("SZYD_Cas9_CR50");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // 4. Expected state: 4 reads are selected
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 2, QString("1. Unexpected selection! Expected selection size == 2, actual selection size == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR50", QString("Unexpected selected read, expected: SZYD_Cas9_CR50, current: %1").arg(name[0]));
    CHECK_SET_ERR(name[1] == "SZYD_Cas9_CR51", QString("Unexpected selected read, expected: SZYD_Cas9_CR51, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_6640_5) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select second read
    GTUtilsMcaEditor::clickReadName(QString("SZYD_Cas9_5B70"));

    // 3. Press Esc key
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 4. Expected state: No any selected read
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 0, QString("1. Unexpected selection! Expected selection size == 0, actual selection size == %1").arg(QString::number(name.size())));
}

GUI_TEST_CLASS_DEFINITION(test_6640_6) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select second read
    GTUtilsMcaEditor::clickReadName(QString("SZYD_Cas9_5B70"));

    // 3. Select any column on reference
    GTUtilsMcaEditorSequenceArea::clickToReferencePositionCenter(2);

    // 4. Expected state: No any selected read, selected column only
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 0, QString("1. Unexpected selection! Expected selection size == 4, actual selection size == %1").arg(QString::number(name.size())));
}

GUI_TEST_CLASS_DEFINITION(test_6649) {
    // UTEST-43
    GTFileDialog::openFile(testDir + "_common_data/cmdline/pcr/pET-24.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));
    GTUtilsPcr::setPrimer(U2Strand::Direct, "GCTCTCCCTTATGCGACTCC");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "GCGTCCCATTCGCCAATCC");

    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(), "Wrong results count");

    auto annsComboBox = GTWidget::findComboBox("annsComboBox");
    GTComboBox::selectItemByIndex(annsComboBox, 1);
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));
    GTWidget::click(GTWidget::findWidget("extractProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    int length = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(length == 642, QString("Sequence length mismatch. Expected: %1. Actual: %2").arg(642).arg(length));
}

GUI_TEST_CLASS_DEFINITION(test_6651) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select the last sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence("Hetrodes_pupus_EF540832");

    // 3. Remove it with the Delete hotkey.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 4. Expected result: the new last sequence is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 16, 604, 1));
}
GUI_TEST_CLASS_DEFINITION(test_6652) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select a region in the Sequence Area.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    // 3. Click into the middle of the region.
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(9, 5));

    // 4. Expected result: only the clicked cell is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(9, 5, 1, 1));
}
GUI_TEST_CLASS_DEFINITION(test_6652_1) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Select a region.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    // 3. Move the mouse cursor inside the region, push the mouse left button down, drag the mouse cursor to the right.
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    const QString selection1 = GTClipboard::text();
    GTUtilsMSAEditorSequenceArea::dragAndDropSelection(QPoint(9, 5), QPoint(10, 5));

    // 4. The same region (but shifted to the right) is selected.
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(8, 3), QPoint(13, 7), selection1);
}

GUI_TEST_CLASS_DEFINITION(test_6654) {
    // 1. Open "COI.aln" sample alignment.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");

    GTUtilsMsaEditor::moveToSequenceName("Roeseliana_roeseli");
    GTMouseDriver::click();

    MSAEditor* msaEditor = GTUtilsMsaEditor::getEditor();
    QRect selection = msaEditor->getSelection().toRect();

    CHECK_SET_ERR(selection.x() == 0, QString("1. Expected selection x: 0, actual: %1").arg(selection.x()));
    CHECK_SET_ERR(selection.width() == 604, QString("1. Expected selection width: 604, actual: %1").arg(selection.width()));

    GTUtilsMSAEditorSequenceArea::click(QPoint(5, 18));

    selection = msaEditor->getSelection().toRect();
    CHECK_SET_ERR(selection.x() == 0, QString("2. Expected selection x: 0, actual: %1").arg(selection.x()));
    CHECK_SET_ERR(selection.width() == 0, QString("2. Expected selection width: 0, actual: %1").arg(selection.width()));
}

GUI_TEST_CLASS_DEFINITION(test_6655) {
    // 1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select the "Conocephalus_percaudata" sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence(QString("Conocephalus_percaudata"));

    // 4. Click arrow down
    GTKeyboardDriver::keyClick(Qt::Key_Down);
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is not collapsed");

    // 5. Click right arrow
    GTKeyboardDriver::keyClick(Qt::Key_Right);

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is collapsed");

    GTKeyboardDriver::keyClick(Qt::Key_Left);

    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Mecopoda_elongata__Sumatra_")),
                  "Required sequence is collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_6656) {
    // Check that UGENE does not crash when invalid custom region is used in the Search panel of Sequence view.
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("TTT");

    QWidget* optionsPanel = GTUtilsOptionsPanel::getActiveOptionsWidget();
    GTUtilsOptionPanelSequenceView::setRegionType("Custom region");
    GTLineEdit::setText("editStart", "3", optionsPanel);
    GTLineEdit::setText("editEnd", "2", optionsPanel);

    GTWidget::findLabelByText("Warning: Invalid search region.");

    GTLineEdit::setText("editEnd", "4", optionsPanel);
    GTWidget::findLabelByText("Warning: Search region is too small.");
}

GUI_TEST_CLASS_DEFINITION(test_6659) {
    // 1. Open an alignment (e.g. "_common_data/scenarios/msa/ma2_gapped.aln").
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the "General" tab on the options panel.
    GTWidget::click(GTWidget::findWidget("OP_MSA_GENERAL"));

    // 3. The "Copy" button is disabled.
    auto copyButton = GTWidget::findToolButton("copyButton");
    CHECK_SET_ERR(!copyButton->isEnabled(), "copyButton is unexpectedly enabled");

    // 4. Select any region and press ctrl+c
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 6), QPoint(1, 9));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    // 5. Press ctrl+v
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // 6. Expected state: the region will appear under the main alignment
    GTUtilsMsaEditor::selectRows(0, 12);
    int numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    numSelectedSequences = GTUtilsMSAEditorSequenceArea::getSelectedSequencesNum();
    CHECK_SET_ERR(numSelectedSequences == 13, "There is no selection in MSA, but expected");
}

GUI_TEST_CLASS_DEFINITION(test_6667_1) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // 3. Click to the "Switch on/off collapsing" on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 4. Enter the following pattern: "TTATT".
    GTUtilsOptionPanelMsa::enterPattern("TTATT");

    // 5. Wait for the search task finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    QRect expectedSelection(7, 2, 5, 1);
    QRect actualSelection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(expectedSelection == actualSelection,
                  QString("Incorrect selection after the pattern search. Expected: %1, actual %2")
                      .arg(GTUtilsText::rectToString(expectedSelection))
                      .arg(GTUtilsText::rectToString(actualSelection)));

    // 6. Click "next" button
    GTUtilsOptionPanelMsa::clickNext();

    expectedSelection = QRect(4, 4, 5, 1);
    actualSelection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(expectedSelection == actualSelection,
                  QString("Incorrect selection after the pattern search. Expected: %1, actual %2")
                      .arg(GTUtilsText::rectToString(expectedSelection))
                      .arg(GTUtilsText::rectToString(actualSelection)));

    // 7. Click "next" button
    GTUtilsOptionPanelMsa::clickNext();

    expectedSelection = QRect(7, 2, 5, 1);
    actualSelection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(expectedSelection == actualSelection,
                  QString("Incorrect selection after the pattern search. Expected: %1, actual %2")
                      .arg(GTUtilsText::rectToString(expectedSelection))
                      .arg(GTUtilsText::rectToString(actualSelection)));
}

GUI_TEST_CLASS_DEFINITION(test_6672) {
    // 1. Open "_common_data/scenarios/msa/ma2_gap_8_col.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/ma2_gap_8_col.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Remove 20-21 columns with gaps
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(19, 0), QPoint(20, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Switch collapsing mode on
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 4. Remove 4-19 columns
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(3, 0), QPoint(18, 5));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Remove 3 column
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 0), QPoint(2, 2));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. Remove 2 column
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(1, 2));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 7. Expected state: One first column left in collapsing mode
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(0, 1), "A\nT");
}
GUI_TEST_CLASS_DEFINITION(test_6673) {
    // 1. Open "_common_data/scenarios/msa/translations_nucl.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "P" sequense, last in the name list.
    GTUtilsMSAEditorSequenceArea::selectSequence("P");

    // 3. Select first symbol and insert gap.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 4));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // 4. Click "Delete" button 3 times.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: a line consisting of gaps should be removed
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("P")), "Required sequence is not removed");
}

GUI_TEST_CLASS_DEFINITION(test_6673_1) {
    // 1. Open "_common_data/scenarios/msa/translations_nucl.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "translations_nucl.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch collapsing mode on.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select "S" sequense.
    GTUtilsMSAEditorSequenceArea::selectSequence("S");

    // 4. Select first symbol and insert gap.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Expected result: one gap is inserted for "S" collapsing group
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(0, 4), "T\n-\nG\nC\nA");
}

GUI_TEST_CLASS_DEFINITION(test_6676_1) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    //    3. Enter the following pattern: "TAAGACTTCT".
    GTUtilsOptionPanelMsa::enterPattern("TAAGACTTCT");

    //    4. Wait for the search task finish.
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: there is 1 result: the first 10 bases of the first row are found, the result is selected.
    QRect expectedSelection(0, 0, 10, 1);
    QRect actualSelection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(expectedSelection == actualSelection, QString("Incorrect selection after the pattern search"));

    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/1");

    //    5. Ensure that focus is set to the pattern input widget.
    GTWidget::click(GTWidget::findWidget("textPattern"));

    //    6. Set cursor in the pattern input widget before the last symbol.
    GTKeyboardDriver::keyClick(Qt::Key_End);
    GTKeyboardDriver::keyClick(Qt::Key_Left);

    //    7. Click Delete key.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    //    Expected state: the pattern is "TAAGACTTC". The alignment is not modified. The first 9 bases of the first row are selected.
    const QString expectedPattern = "TAAGACTTC";
    const QString actualPattern = GTUtilsOptionPanelMsa::getPattern();
    CHECK_SET_ERR(expectedPattern == actualPattern, QString("Incorrect pattern: expected '%1', got '%2'").arg(expectedPattern).arg(actualPattern));

    GTUtilsProjectTreeView::itemModificationCheck("COI.aln", false);

    expectedSelection = QRect(0, 0, 9, 1);
    actualSelection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(expectedSelection == actualSelection, QString("Incorrect selection after the modified pattern search"));
}

GUI_TEST_CLASS_DEFINITION(test_6676_2) {
    //    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "General" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    //    3. Enter the following reference sequence name: "TAAGACTTCT".
    auto sequenceLineEdit = GTWidget::findLineEdit("sequenceLineEdit");
    GTWidget::click(sequenceLineEdit);
    GTKeyboardDriver::keySequence("TAAGACTTCT");

    //    4. Click Left key on the keyboard.
    GTKeyboardDriver::keyClick(Qt::Key_Left);

    //    5. Click Delete key.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    //    Expected state: the reference sequence name "TAAGACTTC". The alignment is not modified.
    const QString expectedText = "TAAGACTTC";
    const QString actualText = sequenceLineEdit->text();
    CHECK_SET_ERR(expectedText == actualText, QString("Incorrect sequence name: expected '%1', got '%2'").arg(expectedText).arg(actualText));

    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsProjectTreeView::itemModificationCheck("COI.aln", false);
}

GUI_TEST_CLASS_DEFINITION(test_6677) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select the second column
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(1);

    // 4. Click collapse triangle:
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Mecopoda_elongata__Ishigaki__J");

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(1, 0, 1, 18));
}
GUI_TEST_CLASS_DEFINITION(test_6677_1) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select the first character in the collapsed group.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 13));

    // 4. Click collapse triangle:
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Mecopoda_elongata__Ishigaki__J");

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 13, 1, 2));
}
GUI_TEST_CLASS_DEFINITION(test_6684) {
    class BuildDotPlotScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto minLenBox = GTWidget::findSpinBox("minLenBox", dialog);
            CHECK_SET_ERR(minLenBox->value() == 70, "Min lengths value doesn't match: " + QString::number(minLenBox->value()));

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new DotPlotFiller(new BuildDotPlotScenario()));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/fasta/AMINO.fa", testDir + "_common_data/fasta/AMINO.fa"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});

    GTWidget::findWidget("dotplot widget", GTUtilsMdi::activeWindow());
}

GUI_TEST_CLASS_DEFINITION(test_6684_1) {
    class BuildDotPlot100Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto minLenBox = GTWidget::findSpinBox("minLenBox", dialog);
            CHECK_SET_ERR(minLenBox->value() == 100, "1. Min lengths value doesn't match: " + QString::number(minLenBox->value()));

            auto invertedCheckBox = GTWidget::findCheckBox("invertedCheckBox", dialog);
            CHECK_SET_ERR(invertedCheckBox->isEnabled(), "Inverted checkbox should be enabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    class BuildDotPlot70Scenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto minLenBox = GTWidget::findSpinBox("minLenBox", dialog);
            CHECK_SET_ERR(minLenBox->value() == 70, "2. Min lengths value doesn't match: " + QString::number(minLenBox->value()));

            auto invertedCheckBox = GTWidget::findCheckBox("invertedCheckBox", dialog);
            CHECK_SET_ERR(!invertedCheckBox->isEnabled(), "Inverted checkbox should be disabled");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new DotPlotFiller(new BuildDotPlot100Scenario()));
    GTUtilsDialog::waitForDialog(
        new BuildDotPlotFiller(
            testDir + "_common_data/fasta/reference_ACGT_rand_1000.fa",
            testDir + "_common_data/fasta/reference_ACGT_rand_1000.fa"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTWidget::findWidget("dotplot widget", GTUtilsMdi::activeWindow());

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTUtilsDialog::waitForDialog(new DotPlotFiller(new BuildDotPlot70Scenario()));
    GTUtilsDialog::waitForDialog(
        new BuildDotPlotFiller(
            testDir + "_common_data/fasta/reference_ACGT_rand_1000.fa",
            testDir + "_common_data/fasta/AMINO.fa"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
}

GUI_TEST_CLASS_DEFINITION(test_6685_1) {
    // 1. Open "_common_data/genbank/short.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/short.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Click right button on the sequence view -> Copy/Paste. The all following buttons with hotkeys are disabled:
    // Copy selected sequence -- Ctrl + C
    // Copy selected complementary 5 '-3' sequence -- Ctrl + Shift + C
    // Copy amino acids -- Ctrl + T
    // Copy amino acids of complementary 5 '-3' strand -- Ctrl + Shift + T
    // Copy annotation sequence
    // Copy annotation amino acids
    // Copy qualifier text
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy selected sequence", QKeySequence(Qt::CTRL | Qt::Key_C)},
                                                                            {"Copy selected complementary 5'-3' sequence", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C)},
                                                                            {"Copy amino acids", QKeySequence(Qt::CTRL | Qt::Key_T)},
                                                                            {"Copy amino acids of complementary 5'-3' strand", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T)},
                                                                            {"Copy annotation sequence", QKeySequence()},
                                                                            {"Copy annotation amino acids", QKeySequence()},
                                                                            {"Copy qualifier text", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_6685_2) {
    // 1. Open "_common_data/fasta/AMINO.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta/AMINO.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Click right button on the sequence view -> Copy/Paste. The all following buttons with hotkeys are disabled:
    // Copy selected sequence -- Ctrl + C
    // Copy annotation
    // Copy qualifier text
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy selected sequence", QKeySequence(Qt::CTRL | Qt::Key_C)},
                                                                            {"Copy annotation", QKeySequence()},
                                                                            {"Copy qualifier text", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    // The all following buttons do not exist:
    // Copy selected complementary 5 '-3' sequence
    // Copy amino acids
    // Copy amino acids of complementary 5 '-3' strand
    // Copy annotation amino acids
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            "Copy selected complementary 5'-3' sequence",
                                                                            "Copy amino acids",
                                                                            "Copy amino acids of complementary 5'-3' strand",
                                                                            "Copy annotation amino acids",
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::isNotVisible)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_6685_3) {
    // 1. Open "_common_data/genbank/short.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/short.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Click on the CDS annotation.
    GTUtilsSequenceView::clickAnnotationPan("CDS", 2);

    // 3. Click right button on the sequence view -> Copy/Paste. Items 1-4 and 9 are disabled, items 5-8 are enabled.
    // Copy selected sequence
    // Copy selected complementary 5 '-3' sequence
    // Copy amino acids
    // Copy amino acids of complementary 5 '-3' strand
    // Copy annotation sequence -- Ctrl + C
    // Copy annotation amino acids -- Ctrl + T
    // Copy qualifier text
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy selected sequence", QKeySequence()},
                                                                            {"Copy selected complementary 5'-3' sequence", QKeySequence()},
                                                                            {"Copy amino acids", QKeySequence()},
                                                                            {"Copy amino acids of complementary 5'-3' strand", QKeySequence()},
                                                                            {"Copy qualifier text", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy annotation sequence", QKeySequence(Qt::CTRL | Qt::Key_C)},
                                                                            {"Copy annotation amino acids", QKeySequence(Qt::CTRL | Qt::Key_T)},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    // 4. Click Ctrl + C
    // Expected state: CGTAGG in the clipboard
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "CGTAGG", QString("Unexpected sequence, expected: CGTACG, current: %1").arg(clipText));

    // 5. Click Ctrl + T
    // Expected state: RR in the clipboard
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "RR", QString("Unexpected sequence, expected: RR, current: %1").arg(clipText));
}

GUI_TEST_CLASS_DEFINITION(test_6685_4) {
    // 1. Open "_common_data/genbank/short.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/short.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Select region 3-8
    GTUtilsSequenceView::selectSequenceRegion(3, 8);

    // 3. Click right button on the zoom view -> Copy/Paste. Items 1-4 and 9-10 are disabled, items 5-8 are enabled.
    // Copy selected sequence -- Ctrl + C
    // Copy selected complementary 5 '-3' sequence -- Ctrl + Shift + C
    // Copy amino acids -- Ctrl + T -- Ctrl + Shift + T
    // Copy amino acids of complementary 5 '-3' strand
    // Copy annotation sequence
    // Copy annotation amino acids
    // Copy qualifier text
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy selected sequence", QKeySequence(Qt::CTRL | Qt::Key_C)},
                                                                            {"Copy selected complementary 5'-3' sequence", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C)},
                                                                            {"Copy amino acids", QKeySequence(Qt::CTRL | Qt::Key_T)},
                                                                            {"Copy amino acids of complementary 5'-3' strand", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T)},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy annotation sequence", QKeySequence()},
                                                                            {"Copy annotation amino acids", QKeySequence()},
                                                                            {"Copy qualifier text", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    // 4. Click Ctrl + C
    // Expected state: GTAGGT in the clipboard
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "GTAGGT", QString("Unexpected sequence, expected: GTAGGT, current: %1").arg(clipText));

    // 5. Click Ctrl + Shift + C
    // Expected state: ACCTAC in the clipboard
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier | Qt::ShiftModifier);
    clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "ACCTAC", QString("Unexpected sequence, expected: ACCTAC, current: %1").arg(clipText));

    // 6. Click Ctrl + T
    // Expected state: VG in the clipboard
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "VG", QString("Unexpected sequence, expected: VG, current: %1").arg(clipText));

    // 7. Click Ctrl + Shift + T
    // Expected state: TY in the clipboard
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier | Qt::ShiftModifier);
    clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "TY", QString("Unexpected sequence, expected: TY, current: %1").arg(clipText));
}

GUI_TEST_CLASS_DEFINITION(test_6685_5) {
    // 1. Open "_common_data/genbank/short.gb".
    GTFileDialog::openFile(testDir + "_common_data/genbank/short.gb");
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    // 2. Double click on the CDS annotation.
    GTUtilsSequenceView::clickAnnotationPan("CDS", 2, 0, true);

    // 3. Click right button on the zoom view -> Copy/Paste. The all following buttons with hotkwys are enabled except the last one
    // Copy selected sequence -- Ctrl + C
    // Copy selected complementary 5 '-3' sequence -- Ctrl + Shift + C
    // Copy amino acids -- Ctrl + T
    // Copy amino acids of complementary 5 '-3' strand -- Ctrl + Shift + T
    // Copy annotation sequence
    // Copy annotation amino acids
    // Copy qualifier text
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy selected sequence", QKeySequence(Qt::CTRL | Qt::Key_C)},
                                                                            {"Copy selected complementary 5'-3' sequence", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C)},
                                                                            {"Copy amino acids", QKeySequence(Qt::CTRL | Qt::Key_T)},
                                                                            {"Copy amino acids of complementary 5'-3' strand", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T)},
                                                                            {"Copy annotation sequence", QKeySequence()},
                                                                            {"Copy annotation amino acids", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsEnabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Copy/Paste"}, {
                                                                            {"Copy qualifier text", QKeySequence()},
                                                                        },
                                                        PopupChecker::CheckOptions(PopupChecker::IsDisabled)));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();

    // 4. Click Ctrl + C
    // Expected state: CGTAGG in the clipboard
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "CGTAGG", QString("Unexpected sequence, expected: CGTACG, current: %1").arg(clipText));

    // 5. Click Ctrl + T
    // Expected state: RR in the clipboard
    GTKeyboardDriver::keyClick('t', Qt::ControlModifier);
    clipText = GTClipboard::text();
    CHECK_SET_ERR(clipText == "RR", QString("Unexpected sequence, expected: RR, current: %1").arg(clipText));
}

GUI_TEST_CLASS_DEFINITION(test_6691_1) {
    // UTEST-44
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    GTUtilsOptionPanelMsa::enterPattern("ACCTAT");
    QRect selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selection.x() == 118, "Wrong selection");
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/14");

    GTUtilsOptionPanelMsa::setAlgorithm("Substitute");
    GTUtilsOptionPanelMsa::setMatchPercentage(65);
    selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selection.x() == 4, "Wrong selection");
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/533");

    GTUtilsOptionPanelMsa::enterPattern("TTTT");
    GTUtilsOptionPanelMsa::setCheckedRemoveOverlappedResults(true);
    selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selection.x() == 6, "Wrong selection");
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/752");
}

GUI_TEST_CLASS_DEFINITION(test_6691_2) {
    // UTEST-45
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    GTUtilsOptionPanelMsa::enterPattern("ACCTAT");
    QRect selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selection.x() == 118, "Wrong selection");
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/14");

    GTUtilsMSAEditorSequenceArea::replaceSymbol(QPoint(410, 1), '-');
    selection = GTUtilsMSAEditorSequenceArea::getSelectedRect();
    CHECK_SET_ERR(selection.x() == 410, "Wrong selection");
    GTUtilsOptionPanelMsa::checkResultsText("Results: -/14");
}

GUI_TEST_CLASS_DEFINITION(test_6692) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Click to the "Switch on/off collapsing" on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Expected result: there are two collapsing groups one right after another: "Conocephalus_discolor" and "Mecopoda_elongata_Ishigaki_J".
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "1 Conocephalus_discolor is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");

    // 4. Expand "Conocephalus_discolor" group.
    GTUtilsMSAEditorSequenceArea::selectSequence("Conocephalus_discolor");
    GTUtilsMsaEditor::toggleCollapsingGroup("Conocephalus_discolor");

    // 5. Remove the first group: select all sequences from the "Conocephalus_discolor" group and press Delete key on the keyboard.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 6. Expected result: "Conocephalus_discolor" group is removed, "Mecopoda_elongata_Ishigaki_J" is still collapsed.
    QStringList modifiedNames = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(originalNames.length() - modifiedNames.length() == 3, "The number of sequences remained unchanged.");
    CHECK_SET_ERR(!modifiedNames.contains("Conocephalus_discolor"), "Removed sequence is present in multiple alignment.");

    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_6692_1) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Show chromatograms" button on the toolbar.
    GTUtilsMcaEditor::toggleShowChromatogramsMode();

    // 3. Expand "SZYD_Cas9_CR51" row (the third).
    GTUtilsMcaEditor::clickReadName(QString("SZYD_Cas9_CR51"));
    GTKeyboardDriver::keyClick(Qt::Key_Right);

    // 4. Click to the first row name. Press Delete key on the keyboard.
    GTUtilsMcaEditor::removeRead(QString("SZYD_Cas9_5B70"));

    // 5. Expected result: the first row is removed. "SZYD_Cas9_CR51" row is expanded, all other rows are collapsed.
    CHECK_SET_ERR(GTUtilsMcaEditorSequenceArea::isChromatogramShown(QString("SZYD_Cas9_CR51")),
                  "Required sequence is collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_6692_2) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Click to the "Switch on/off collapsing" on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Expected result: there are two collapsing groups one right after another: "Conocephalus_discolor" and "Mecopoda_elongata_Ishigaki_J".
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "1 Conocephalus_discolor is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");

    // 4. Expand "Conocephalus_discolor" group.
    GTUtilsMsaEditor::toggleCollapsingGroup("Conocephalus_discolor");

    // 5. Select "Conocephalus_discolor" and "Conocephalus_sp." sequence.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 10), QPoint(11, 11));

    // 6. Press the Delete key on the keyboard.

    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 7. Expected result: "Conocephalus_discolor" group is removed, but the third sequence from is the "Conocephalus_discolor" group is still present in the alignment. Also "Mecopoda_elongata_Ishigaki_J" is collapsed.

    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");

    CHECK_SET_ERR(GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Conocephalus_percaudata")),
                  "Required sequence is removed");
}
GUI_TEST_CLASS_DEFINITION(test_6692_3) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Click to the "Switch on/off collapsing" on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Expected result: there are two collapsing groups one right after another: "Conocephalus_discolor" and "Mecopoda_elongata_Ishigaki_J".
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "1 Conocephalus_discolor is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");

    // 4. Expand "Conocephalus_discolor" group.
    GTUtilsMsaEditor::toggleCollapsingGroup("Conocephalus_discolor");

    // 5. Select the "Conocephalus_discolor" sequence only. Press the Delete key on the keyboard.
    GTUtilsMSAEditorSequenceArea::removeSequence("Conocephalus_discolor");

    // 6. Expected result: "Conocephalus_discolor" sequence is removed. There are two collapsed groups: "Conocephalus_sp." and "Mecopoda_elongata_Ishigaki_J". The "Conocephalus_sp." group has one internal sequence "Conocephalus_percaudata" in it, the group is opened. The "Mecopoda_elongata_Ishigaki_J" is collapsed.
    GTUtilsMsaEditor::toggleCollapsingGroup("Conocephalus_sp.");
    CHECK_SET_ERR(!GTUtilsMSAEditorSequenceArea::isSequenceVisible(QString("Conocephalus_discolor")),
                  "Required sequence is not removed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "1 Conocephalus_percaudata is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Mecopoda_elongata__Sumatra_"),
                  "2 Mecopoda_elongata_Ishigaki_J is not collapsed");
}
GUI_TEST_CLASS_DEFINITION(test_6693) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select any region in "Mecopoda_elongata_Sumatra".
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 13), QPoint(5, 13));
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);

    GTUtilsMsaEditor::toggleCollapsingMode();

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 0, 0));
}

GUI_TEST_CLASS_DEFINITION(test_6697) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the first column and press the Delete key.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 3. Expected state: the new first column is selected.

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(0, 9), "A\nA\nA\nA\nA\nA\nA\nA\n-\nA");

    // 4. Press the Delete key again.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 5. Expected state: the new first column is selected.
    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 0), QPoint(0, 9), "G\nG\nG\nG\nG\nG\nG\nG\n-\nG");
}

GUI_TEST_CLASS_DEFINITION(test_6689) {
    // UTEST-42
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMsaEditor::clickSequence(0);

    QRect rowNameRect = GTUtilsMsaEditor::getSequenceNameRect(0);
    QRect destinationRowNameRect = GTUtilsMsaEditor::getSequenceNameRect(16);
    GTMouseDriver::dragAndDrop(rowNameRect.center(), destinationRowNameRect.center());

    QAbstractButton* undo = GTAction::button("msa_action_undo");
    CHECK_SET_ERR(undo->isEnabled(), "Undo button should be enabled");
    GTWidget::click(undo);
    CHECK_SET_ERR(!undo->isEnabled(), "Undo button should be disabled");
}

GUI_TEST_CLASS_DEFINITION(test_6703) {
    // 1. Open "_common_data/regression/6703/1.aln
    // Expected state: 'Remove all gaps' button is disabled
    // Close view
    GTFileDialog::openFile(testDir + "_common_data/regression/6703/1.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    auto button = GTWidget::findButtonByText("Remove all gaps");
    CHECK_SET_ERR(!button->isEnabled(), "'Remove all gaps' unexpectedly enabled");
    GTUtilsMdi::closeWindow("1 [1.aln]");

    // 2. Open "_common_data/regression/6703/2.aln
    // Expected state: 'Remove all gaps' button is enabled
    GTFileDialog::openFile(testDir + "_common_data/regression/6703/2.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    button = GTWidget::findButtonByText("Remove all gaps");
    CHECK_SET_ERR(button->isEnabled(), "'Remove all gaps' unexpectedly disabled");

    // 3. Do menu with corresponding action
    // Expected state: 'Remove all gaps' button is disabled
    QWidget* seqArea = GTWidget::findWidget("msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seqArea);
    CHECK_SET_ERR(!button->isEnabled(), "'Remove all gaps' unexpectedly enabled");
    GTUtilsMdi::closeWindow("2 [2.aln]");

    // 4. Open "_common_data/regression/6703/3.aln
    // Expected state: 'Remove all gaps' button is enabled
    GTFileDialog::openFile(testDir + "_common_data/regression/6703/3.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    button = GTWidget::findButtonByText("Remove all gaps");
    CHECK_SET_ERR(button->isEnabled(), "'Remove all gaps' unexpectedly disabled");

    // 5. Do menu with corresponding action
    // Expected state: 'Remove all gaps' button is disabled
    seqArea = GTWidget::findWidget("msa_editor_sequence_area");
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_EDIT", "Remove all gaps"}));
    GTMenu::showContextMenu(seqArea);
    CHECK_SET_ERR(!button->isEnabled(), "'Remove all gaps' unexpectedly enabled");
}

GUI_TEST_CLASS_DEFINITION(test_6705) {
    // 1. Select "Tools > NGS data analysis > Reads quality control" in the main menu.
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads quality control..."});

    // Expected result: the "Choose Output Directory" dialog appears.

    // 2. Close the dialog with the cross button in the dialog title area.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Expected result: UGENE doesn't crash.
}

GUI_TEST_CLASS_DEFINITION(test_6706) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Go to the "Highlighting" tab.
    // 3. Set the "Agreements" highlighting value.
    GTUtilsOptionPanelMsa::setHighlightingScheme("Agreements");

    // 4. Go to the "General" tab.
    // 5. Set "Phaneroptera_falcata" as "Reference sequence".
    GTUtilsOptionPanelMsa::addReference("Phaneroptera_falcata");

    // 6. Select characters 1-3 of the "Isophya_altaica_EF540820" sequence
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 1), QPoint(2, 1));

    // 7. Set "Rich text (HTML)" in the "Format" popup menu on th bottom of the "General" tab.
    // 8. Click "Copy".
    GTUtilsOptionPanelMsa::copySelection(GTUtilsOptionPanelMsa::CopyFormat::Rich_text);

    // Expected result: the clipboard contains the data which you can see in the attached file.
    QString url = testDir + "_common_data/scenarios/_regression/6706/6706.txt";
    bool eq = GTFile::equals(url);

    CHECK_SET_ERR(eq, "file should be equal to the clipboard");
}

GUI_TEST_CLASS_DEFINITION(test_6707) {
    // 1. Create a folder and put any file in there.
    QDir(sandBoxDir).mkdir("test_6707");

    IOAdapterUtils::writeTextFile(sandBoxDir + "test_6707/file.txt", "Hello!");

    // 2. Open the "Alignment Color Scheme" tab of the "Application settings" dialog.
    // 3. Set "Directory to save color scheme" to the folder you created on the step 1.
    class Custom : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::AlignmentColorScheme);
            auto colorsDirEdit = GTWidget::findLineEdit("colorsDirEdit", dialog);

            GTLineEdit::setText(colorsDirEdit, sandBoxDir + "test_6707/file.txt");
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Folder is a regular file."));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText(colorsDirEdit, sandBoxDir + "test_6707");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new Custom()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
    // Expected result: the file is still in the folder and is not removed/modified.
    CHECK_SET_ERR(IOAdapterUtils::readTextFile(sandBoxDir + "test_6707/file.txt") == "Hello!", "The file was removed or modified");
}

GUI_TEST_CLASS_DEFINITION(test_6709) {
    // Open “_common_data\cmdline\DNA_circular.gb”
    GTFileDialog::openFile(testDir + "_common_data/cmdline/DNA_circular.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Click "Show/hide amino acid translations > Translation selection"
    auto translationsMenuToolbarButton = GTWidget::findWidget("translationsMenuToolbarButton");

    GTUtilsDialog::waitForDialog(new PopupChooser({"translate_selection_radiobutton"}));
    GTWidget::click(translationsMenuToolbarButton);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Select the following region "1..10, 740..744".
    GTUtilsDialog::waitForDialog(new SelectSequenceRegionDialogFiller("1..10, 740..744"));
    GTKeyboardDriver::keyClick('A', Qt::ControlModifier);

    // Expected result: the selected strand translation is "LS*LP".
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy amino acids"}));
    GTMenu::showContextMenu(GTUtilsSequenceView::getPanOrDetView());

    QString text = GTClipboard::text();
    CHECK_SET_ERR(text == "LS*LP", QString("Unexpected text in the clipboard, expected: LS*LP, current: %1").arg(text));
}

GUI_TEST_CLASS_DEFINITION(test_6710) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Remove all data from the alignment except the first column.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 3. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 4. Switch off the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 5. Click on the "Conocephalus_sp." row in the Name List.
    // 6. Press and hold Shift key on the keyboard.
    // 7. Click on the "Conocephalus_percaudata" row in the Name List. Release Shift.
    GTUtilsMsaEditor::clickSequenceName("Conocephalus_sp.");
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    GTUtilsMsaEditor::clickSequenceName("Conocephalus_percaudata");
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // 8. Expected result: two rows are selected: "Conocephalus_sp." and "Conocephalus_percaudata".
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 5, 1, 2));
}

GUI_TEST_CLASS_DEFINITION(test_6711) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the last column and press the Delete key.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(13, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Expected result: the last column is removed, the new last column is selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(12, 0, 1, 10));

    // 4. Press the Delete key again. Expected result: the last column is removed the new last column is selected.
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(11, 0, 1, 10));
}

GUI_TEST_CLASS_DEFINITION(test_6712) {
    // Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Move Conocephalus_sp. from 6th position to first position.
    GTUtilsMsaEditor::clickSequence(5);
    QRect rowNameRect = GTUtilsMsaEditor::getSequenceNameRect(5);
    QRect destinationRowNameRect = GTUtilsMsaEditor::getSequenceNameRect(0);
    GTMouseDriver::dragAndDrop(rowNameRect.center(), destinationRowNameRect.center());

    // Turn on the collapsing mode
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: Conocephalus_discolor, Conocephalus_percaudata are collapsed.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_discolor"),
                  "Conocephalus_discolor is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "Conocephalus_percaudata is not collapsed");

    // Open the group
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Conocephalus_sp.");

    // Insert space to (2, 1) position
    GTUtilsMSAEditorSequenceArea::clickToPosition(QPoint(2, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Select (2, 0, 2, 2) rectangle
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(2, 0), QPoint(2, 2));

    // Press delete
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // Conocephalus_sp. in the 7th position, Conocephalus_sp. in the first, Conocephalus_percaudata in the second
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList[0] == "Conocephalus_sp.", "The first sequence is incorrect");
    // CHECK_SET_ERR(nameList[1] == "Conocephalus_percaudata", "The second sequence is incorrect " + nameList[1]);
    // CHECK_SET_ERR(nameList[6] == "Conocephalus_discolor", "The 6th sequence is incorrect");

    // Expected state: Conocephalus_percaudata is collapsed.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Conocephalus_sp.");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_percaudata"),
                  "Conocephalus_percaudata is not collapsed");
}

GUI_TEST_CLASS_DEFINITION(test_6714) {
    // 1. Open "_common_data/sanger/alignment.ugenedb".
    const QString filePath = sandBoxDir + suite + "_" + name + ".ugenedb";
    GTFile::copy(testDir + "_common_data/sanger/alignment.ugenedb", filePath);
    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click on the third row name in the Name List.
    GTUtilsMcaEditor::clickReadName(QString("SZYD_Cas9_CR51"));

    // 3. Scroll down a bit with a vertical scrollbar.
    GTUtilsMcaEditor::scrollToRead(QString("SZYD_Cas9_CR52"));

    // 4. Expected result: the highlighting rectangle match the row position: selected read "SZYD_Cas9_CR51"
    QStringList name = GTUtilsMcaEditorSequenceArea::getSelectedRowsNames();
    CHECK_SET_ERR(name.size() == 1, QString("1. Unexpected selection! Expected selection size == 1, actual selection size == %1").arg(QString::number(name.size())));
    CHECK_SET_ERR(name[0] == "SZYD_Cas9_CR51", QString("Unexpected selected read, expected: SZYD_Cas9_CR51, current: %1").arg(name[0]));
}

GUI_TEST_CLASS_DEFINITION(test_6715) {
    QDir().mkpath(sandBoxDir + "read_only_dir");
    GTFile::setReadOnly(sandBoxDir + "read_only_dir");

    class Scenario : public CustomScenario {
    public:
        Scenario() {};
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto tree = GTWidget::findTreeWidget("tree");

            GTTreeWidget::click(GTTreeWidget::findItem(tree, "  Alignment Color Scheme"));

            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Ok", "You don't have permissions to write in selected folder."));
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(QFileInfo(sandBoxDir + "read_only_dir").absoluteFilePath(), "", GTFileDialogUtils::Choose, GTGlobals::UseMouse));

            GTWidget::click(GTWidget::findWidget("colorsDirButton", dialog));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    // 1. Open {Settings -> Preferences -> Alignment Color Scheme}.
    GTUtilsDialog::waitForDialog(new NewColorSchemeCreator(new Scenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    // 2. Choose read only folder by pressing "..." button
    // Expected state: warning message about read only folder has appeared
}

GUI_TEST_CLASS_DEFINITION(test_6718) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the "Hetrodes_pupus_EF540832" sequence (the last one).
    GTUtilsMSAEditorSequenceArea::selectSequence(17);

    // 3. Go to the "General" tab.
    // 4. Click "Copy".
    GTUtilsOptionPanelMsa::copySelection();

    // Expected: the file as the same as _common_data/scenarios/_regression/6718/6718.aln
    QString pattern = testDir + "_common_data/scenarios/_regression/6718/6718.aln";
    bool eq = GTFile::equals(pattern);

    CHECK_SET_ERR(eq, "file should be equal to the clipboard");
}

GUI_TEST_CLASS_DEFINITION(test_6730) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Switch on the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select the first column and press the Delete key 6 times.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTKeyboardDriver::keyClick(Qt::Key_Delete);

    // 4. Expected result: the whole column is selected.
    QString expectedSelection = "T\nA\n-\n-\nA\nT\nA";
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedSelection, QString("unexpected selection:\n%1").arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_6734) {
    // 1. Open "_common_data/scenarios/msa/ma.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList originalNames = GTUtilsMSAEditorSequenceArea::getNameList();
    // 2. Open OP and search pattern "AA"
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::enterPattern("AA");

    // 3. Edit alignment
    GTUtilsMSAEditorSequenceArea::replaceSymbol(QPoint(2, 2), '-');

    // 4. Click to the "Switch on/off collapsing" on the toolbar.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected state: UGENE isn't crash
}

GUI_TEST_CLASS_DEFINITION(test_6736_1) {
    // Check that UGENE does not show a falthy invalid pattern error for multi-line patterns.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/1-column.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::enterPattern("A\nT", true);

    QWidget* optionsPanel = GTUtilsOptionsPanel::getActiveOptionsWidget();
    QList<QLabel*> labels = GTWidget::findLabelByText("Warning:", optionsPanel, {false});
    CHECK_SET_ERR(labels.isEmpty(), "No warning labels expected/1");

    GTUtilsOptionPanelMsa::enterPattern("A\nTT", true);
    labels = GTWidget::findLabelByText("Warning:", optionsPanel, {false});
    CHECK_SET_ERR(labels.isEmpty(), "No warning labels expected/2");

    GTUtilsOptionPanelMsa::enterPattern("AA\nTT", true);
    GTWidget::findLabelByText("Warning: Pattern is too long.");
}

GUI_TEST_CLASS_DEFINITION(test_6736_2) {
    // Check that "Search is names" mode does not trigger "Search in sequence" validation errors.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/1-column.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::setSearchContext("Sequence Names");

    GTUtilsOptionPanelMsa::enterPattern("conocephalus", true);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: no warnings, the result label shows 3 rows matched.
    QWidget* optionsPanel = GTUtilsOptionsPanel::getActiveOptionsWidget();
    QList<QLabel*> labels = GTWidget::findLabelByText("Warning:", optionsPanel, {false});
    CHECK_SET_ERR(labels.isEmpty(), "No warning labels expected");

    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/3");
}

GUI_TEST_CLASS_DEFINITION(test_6739) {
    // 1. Open "_common_data/scenarios/msa/ma2_gapped.aln".
    GTFileDialog::openFile(testDir + "_common_data/scenarios/msa/", "ma2_gapped.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Remove all columns except the first one.
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(1, 0), QPoint(13, 9));
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Toggle the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 4. Expected result: there are two collapsed groups.
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Isophya_altaica_EF540820"),
                  "1 Isophya_altaica_EF540820 is not collapsed");
    CHECK_SET_ERR(GTUtilsMsaEditor::isSequenceCollapsed("Conocephalus_discolor"),
                  "2 Conocephalus_discolor is not collapsed");

    // 5. Select the second sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence("Bicolorana_bicolor_EF540830");

    // 6. Open the second group.
    GTUtilsMSAEditorSequenceArea::clickCollapseTriangle("Bicolorana_bicolor_EF540830");

    // 7. Expected result: sequences in the second group are selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 1, 1, 4));
}
GUI_TEST_CLASS_DEFINITION(test_6740) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Enable the collapsing mode.
    GTUtilsMsaEditor::toggleCollapsingMode();

    // 3. Select all sequences.
    GTUtilsMsaEditor::selectRows(0, 16, GTGlobals::UseMouse);

    // 4. Click to the center of the selection.
    GTUtilsMSAEditorSequenceArea::click(QPoint(4, 3));

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(4, 3), QPoint(4, 3), "T");
}
GUI_TEST_CLASS_DEFINITION(test_6742) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QStringList mainItems = {
        "Overview",
        "Show offsets",
        "Zoom In",
        "Zoom Out",
        "Zoom To Selection",
        "Reset Zoom",
        "Colors",
        "Highlighting",
        "Change Font",
        "Clear selection",
    };
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance"}, mainItems, PopupChecker::Exists));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    mainItems.removeOne("Reset Zoom");
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance"}, mainItems, PopupChecker::IsEnabled));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    QStringList colorsItems = {
        "No colors",
        "Jalview",
        "Percentage identity",
        "Percentage identity (colored)",
        "Percentage identity (gray)",
        "UGENE",
        "UGENE Sanger",
        "Weak similarities",
    };
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Colors"}, colorsItems));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    QStringList highlightingItems = {
        "No highlighting",
        "Agreements",
        "Disagreements",
        "Gaps",
        "Conservation level",
        "Transitions",
        "Transversions",
        "Use dots",
    };
    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Highlighting"}, highlightingItems));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    GTUtilsDialog::waitForDialog(new PopupCheckerByText({"Appearance", "Colors", "Custom schemes", "Create new color scheme"}, PopupChecker::IsEnabled));
    GTMouseDriver::click(Qt::RightButton);
}
GUI_TEST_CLASS_DEFINITION(test_6746) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // 3. On the "Search in Alignment" options panel tab select "Custom columns region"
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);
    GTUtilsOptionPanelMsa::setRegionType("Custom columns region");
    GTUtilsOptionPanelMsa::setRegion(1, 200);

    // 4. Enter the following pattern: "ACGT".
    GTUtilsOptionPanelMsa::enterPattern("ACGT");

    // Expected result: Results: 1/2
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/2");
}
GUI_TEST_CLASS_DEFINITION(test_6749) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // 3. Input "AC" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("AC");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: Results: 1/573
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/573");

    // 4. Change search context to the "Sequence Names"
    GTUtilsOptionPanelMsa::setSearchContext("Sequence Names");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: "Results 1/1"
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/1");
}

GUI_TEST_CLASS_DEFINITION(test_6749_1) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    //  Rename on sequence name in Russian "мой класс".
    GTUtilsMSAEditorSequenceArea::renameSequence("Phaneroptera_falcata", "мой класс", true);

    // Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // Change search context to the "Sequence Names"
    GTUtilsOptionPanelMsa::setSearchContext("Sequence Names");

    // Input "мой" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("мой", true);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: Results: 1/1
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/1");
}
GUI_TEST_CLASS_DEFINITION(test_6749_2) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // Input "AC" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("FGH");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result; red background and warning "Input value contains characters that do not match the active alphabet!"
    auto editPatterns = GTWidget::findPlainTextEdit("textPattern");
    QString style0 = editPatterns->styleSheet();
    CHECK_SET_ERR(style0 == "background-color: " + GUIUtils::WARNING_COLOR.name() + ";", "unexpected styleSheet: " + style0);

    // Select using CTRL+SHIFT +F "Sequence Names"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('f', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result; white background, no any warning
    QString style1 = editPatterns->styleSheet();
    CHECK_SET_ERR(style1 == "background-color: " + GUIUtils::OK_COLOR.name() + ";", "unexpected styleSheet: " + style1);
}
GUI_TEST_CLASS_DEFINITION(test_6749_3) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open "Search in Alignment" options panel tab.
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::Search);

    // Input "AC" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("TTGGAGATGAT");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: Results: 1/9
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/9");

    // Enable "Collapsing mode"
    GTUtilsMsaEditor::toggleCollapsingMode();

    // Expected result: Results: -/8
    GTUtilsOptionPanelMsa::checkResultsText("Results: -/8");
}
GUI_TEST_CLASS_DEFINITION(test_6750) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Press Ctrl+F
    GTKeyboardDriver::keyClick('f', Qt::ControlModifier);

    // 3. Input "AC" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("AC");
    // Expected result: Results: 1/573
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/573");

    // 4. Press Ctrl+Shift+F
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('f', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: "Results 1/1"
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/1");

    // 5. Call the "Search in sequences" context menu
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "search_in_sequences"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: "Results: 1/573"
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/573");

    // 6. Call the "Search in sequence names" context menu
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_NAVIGATION, "search_in_sequence_names"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());

    // Expected state: "Results: 1/1
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/1");
}

GUI_TEST_CLASS_DEFINITION(test_6751) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click twice on the "Consensus:" sign above the Name List area.
    GTWidget::click(GTWidget::findWidget("consensusLabel"));
    GTWidget::click(GTWidget::findWidget("consensusLabel"));

    // 3. Select "Align" -> "Align with MUSCLE…" and click on the "Align" button.

    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // Expected result: the alignment process has passed successfully.
}
GUI_TEST_CLASS_DEFINITION(test_6752) {
    // 1. Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select the first "Phaneroptera_falcata" sequence.
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");

    // 3. Right-click on the last "Hetrodes_pupus_EF540832" sequence.
    GTUtilsMSAEditorSequenceArea::moveTo(QPoint(0, 18));
    GTMouseDriver::click(Qt::RightButton);

    // Expected result: the first sequence is selected only, the context menu is shown.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 604, 1));
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
GUI_TEST_CLASS_DEFINITION(test_6754) {
    GTLogTracer lt;

    // 1. open document samples/CLUSTALW/COI.aln
    GTUtilsProject::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    // 2. Click "Align > Align with MUSCLE…" and click "Align".
    GTUtilsDialog::waitForDialog(new MuscleDialogFiller(MuscleDialogFiller::Default, true, true));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_ALIGN, "Align with muscle"}, GTGlobals::UseMouse));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);
    // 3. Click on any sequence during the aligning process.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));

    // 4. There are no errors in the log
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 1, 1));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_6758_1) {
    // copy murine.gb to sandbox as murine.1.gb
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "murine.1.gb");

    // copy murine.gb to sandbox as murine.2.gb
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "murine.2.gb");

    // open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // add "Read sequence" and "Write Annotations" elements and connect them.
    auto addSequenceElement = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    auto writeAnnotationElement = GTUtilsWorkflowDesigner::addElement("Write Annotations");
    GTUtilsWorkflowDesigner::connect(addSequenceElement, writeAnnotationElement);

    // add "murine.1.gb" and "murine.2.gb" as the input of the "Read sequence" element.
    GTUtilsWorkflowDesigner::click(addSequenceElement);
    GTUtilsWorkflowDesigner::setDatasetInputFiles({sandBoxDir + "murine.1.gb", sandBoxDir + "murine.2.gb"});

    // run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // expected: there are two output files "murine.1.gb" and "murine.2.gb", each of them contains one annotation table.
    auto outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(outputFiles.contains("murine.1.gb"), "\"murine.1.gb\" should be, but it's not");
    CHECK_SET_ERR(outputFiles.contains("murine.2.gb"), "\"murine.1.gb\" should be, but it's not");
}

GUI_TEST_CLASS_DEFINITION(test_6758_2) {
    // copy murine.gb to sandbox as murine.1.gb
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "murine.1.gb");

    // copy murine.gb to sandbox/folder as murine.1.gb
    QDir(sandBoxDir + "folder").mkpath(".");
    GTFile::copy(dataDir + "samples/Genbank/murine.gb", sandBoxDir + "folder/murine.1.gb");

    // open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    // add "Read sequence" and "Write Annotations" elements and connect them.
    auto addSequenceElement = GTUtilsWorkflowDesigner::addElement("Read Sequence", true);
    auto writeAnnotationElement = GTUtilsWorkflowDesigner::addElement("Write Annotations");
    GTUtilsWorkflowDesigner::connect(addSequenceElement, writeAnnotationElement);

    // add "murine.1.gb" and "murine.2.gb" as the input of the "Read sequence" element.
    GTUtilsWorkflowDesigner::click(addSequenceElement);
    GTUtilsWorkflowDesigner::setDatasetInputFiles({sandBoxDir + "murine.1.gb", sandBoxDir + "folder/murine.1.gb"});

    // run the workflow.
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // expected: there are two output files "murine.1.gb" and "murine.2.gb", each of them contains one annotation table.
    auto outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(outputFiles.contains("murine.1.gb"), "\"murine.1.gb\" should be, but it's not");
    CHECK_SET_ERR(outputFiles.contains("murine.1_1.gb"), "\"murine.1_1.gb\" should be, but it's not");
}

GUI_TEST_CLASS_DEFINITION(test_6759) {
    GTLogTracer lt;

    // The test just check that there are no crash hile rotating circular view
    //    1. Open sequence
    //    2. Open annotation file
    //    3. Add annotation file to sequence
    //    4. Find splitter
    //    5. Find scroll in splitter
    //    6. Scroll to end, then to begin

    GTFileDialog::openFile(
        testDir + "_common_data/regression/6759/",
        "sequence.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFileDialog::openFile(
        testDir + "_common_data/regression/6759/",
        "annotations.gb");
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("Unknown features"),
                  "No 'Unknown features' object!");

    //    Use context menu on annotation in tree view
    GTUtilsDialog::add(new PopupChooserByText({"Add", "Objects with annotations..."}));
    GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller("annotations.gb", "Unknown features"));
    GTUtilsDialog::add(new CreateObjectRelationDialogFiller());
    //    On question "Found annotations that are out of sequence range, continue?" answer "Yes"
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes));

    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
    //    Check {add-> Objects with annotations} action
    GTUtilsTaskTreeView::waitTaskFinished();

    int seqNum = GTUtilsSequenceView::getSeqWidgetsNumber();
    CHECK_SET_ERR(seqNum == 1, QString("Too many seqWidgets count: %1").arg(seqNum));

    auto horScroll = GTWidget::findScrollBar("CircularViewSplitter_horScroll");

    // We use sleep as scrolling is executing too fast without sleep
    // Also, we don't want to break different scrolls by some optimizations (if any)
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 13);
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 39);
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 360);
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 360 - 13);
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 360 - 39);
    GTScrollBar::moveSliderWithMouseToValue(horScroll, 0);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}
GUI_TEST_CLASS_DEFINITION(test_6760) {
    // 1. Open /data/samples/fasta/human_T1.fa
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Open additional a second copy of the sequence view
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Open In", "Open new view: Sequence View"}, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::callContextMenu("human_T1.fa");

    // 3. Open /data/samples/gff/5prime_utr_intron_A20.gff
    GTFileDialog::openFile(dataDir + "samples/GFF/5prime_utr_intron_A20.gff");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Drag and drop first annotation from gff file to sequence view of human_T1
    // 5. On question "Found annotations that are out of sequence range, continue?" answer "Yes"
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Yes));
    GTUtilsAnnotationsTreeView::addAnnotationsTableFromProject("Ca20Chr1 features");

    // 6. Switch to another view
    GTUtilsMdi::closeActiveWindow();
    GTUtilsMdi::activateWindow("human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Expected result: the annotation is present in another sequence view too.
    GTUtilsAnnotationsTreeView::findItem("5_prime_UTR_intron");
}

GUI_TEST_CLASS_DEFINITION(test_6797_1) {
    // Checks recent list behavior when document file is missed.
    QString filePath = QFileInfo(sandBoxDir + "test_6797.aln").absoluteFilePath();
    GTFile::copy(testDir + "_common_data/clustal/align.aln", filePath);

    GTFileDialog::openFile(filePath);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::closeProject(true);
    GTUtilsTaskTreeView::waitTaskFinished();

    QFile(filePath).remove();

    GTUtilsStartPage::openStartPage();
    GTUtilsStartPage::checkRecentListUrl("test_6797.aln", true);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Remove From List"));
    GTMenu::clickMainMenuItem({"File", "Recent files", filePath});

    // Check that recent files list link does not exit.
    GTMenu::checkMainMenuItemState({"File", "Recent files"}, PopupChecker::IsDisabled);
    GTUtilsStartPage::checkRecentListUrl("test_6797.aln", false);
}

GUI_TEST_CLASS_DEFINITION(test_6797_2) {
    // Checks recent list behavior when project file is missed.
    QString filePath = QFileInfo(sandBoxDir + "test_6797.uprj").absoluteFilePath();

    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::saveProjectAs(filePath);
    GTUtilsProject::closeProject(false);
    GTUtilsTaskTreeView::waitTaskFinished();

    QFile(filePath).remove();

    GTUtilsStartPage::openStartPage();
    GTUtilsStartPage::checkRecentListUrl("test_6797.uprj", true);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Remove From List"));
    GTMenu::clickMainMenuItem({"File", "Recent projects", filePath});

    // Check that recent projects list link does not exit.
    GTMenu::checkMainMenuItemState({"File", "Recent projects"}, PopupChecker::IsDisabled);
    GTUtilsStartPage::checkRecentListUrl("test_6797.uprj", false);
}

GUI_TEST_CLASS_DEFINITION(test_6798) {
    GTFileDialog::openFile(dataDir + "/samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();

    // Select file "pattern_with_uppercase_letter.txt with content "TTGT"
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(testDir + "_common_data/FindAlgorithm/", "pattern_with_uppercase_letter.txt");

    // Actual state: "Results: 1/2317"
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2317"), "Results string not match");

    // Select file pattern_with_lowcase_letter.txt with content "ttgt"
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(testDir + "_common_data/FindAlgorithm/", "pattern_with_lowercase_letter.txt");

    // Expected state: "Results: 1/2317"
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2317"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_6807) {
    class CheckWarningScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTWidget::findLabel("warningLabel", dialog);

            GTLineEdit::setText("fileEdit", sandBoxDir + "/test_6807.html", dialog);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    // 1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(testDir + "_common_data/clustal/", "big.aln");
    GTUtilsTaskTreeView::waitTaskFinished();
    // 2. Do MSA area context menu->Statistics->generate grid profile
    // Expected state: grid profile dialog contains warning label
    // 3. Accept dialog
    // Expected state: grid profile task finished
    GTUtilsDialog::waitForDialog(new GenerateAlignmentProfileDialogFiller(new CheckWarningScenario()));
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_STATISTICS, "Generate grid profile"}));
    GTMenu::showContextMenu(GTUtilsMdi::activeWindow());
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_6808) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Select using CTRL+SHIFT +F "Sequence Names"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('f', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Input "Mec" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("Mec");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Press "Group" button
    GTWidget::click(GTWidget::findWidget("groupResultsButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: the first three sequences are Mecopoda_elongata__Ishigaki__J, Mecopoda_elongata__Sumatra_, Mecopoda_sp.__Malaysia_
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList.size() == 18, "Wrong sequences count");
    CHECK_SET_ERR(nameList[0] == "Mecopoda_elongata__Ishigaki__J", "The first sequence is incorrect");
    CHECK_SET_ERR(nameList[1] == "Mecopoda_elongata__Sumatra_", "The second sequence is incorrect");
    CHECK_SET_ERR(nameList[2] == "Mecopoda_sp.__Malaysia_", "The third sequence is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_6808_1) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select using CTRL+SHIFT +F "Sequence Names"
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTKeyboardDriver::keyClick('f', Qt::ShiftModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::Search);

    // Input "Mec" pattern to the "Search pattern field"
    GTUtilsOptionPanelMsa::enterPattern("Mec");

    // "Result=-/3" is displayed
    GTUtilsOptionPanelMsa::checkResultsText("Results: 1/3");

    // Press "Group" button
    GTWidget::click(GTWidget::findWidget("groupResultsButton"));

    // Expected result: the first three sequences are Mecopoda_elongata__Ishigaki__J, Mecopoda_elongata__Sumatra_, Mecopoda_sp.__Malaysia_
    QStringList nameList0 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList0[0] == "Mecopoda_elongata__Ishigaki__J", "The first sequence is incorrect");
    CHECK_SET_ERR(nameList0[1] == "Mecopoda_elongata__Sumatra_", "The second sequence is incorrect");
    CHECK_SET_ERR(nameList0[2] == "Mecopoda_sp.__Malaysia_", "The third sequence is incorrect");

    // Push Next button
    GTWidget::click(GTWidget::findWidget("nextPushButton"));

    // Result: first line is selected
    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2[0] == "Mecopoda_elongata__Ishigaki__J", "The first sequence is incorrect");
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 604, 1));

    // Press "Group" button -> the group is moved to the end of the list
    GTWidget::click(GTWidget::findWidget("groupResultsButton"));
    QStringList nameList3 = GTUtilsMSAEditorSequenceArea::getNameList();
    int length = nameList2.size();
    CHECK_SET_ERR(nameList3[length - 3] == "Mecopoda_elongata__Ishigaki__J", "The first sequence is incorrect");
    CHECK_SET_ERR(nameList3[length - 2] == "Mecopoda_elongata__Sumatra_", "The second sequence is incorrect");
    CHECK_SET_ERR(nameList3[length - 1] == "Mecopoda_sp.__Malaysia_", "The third sequence is incorrect");

    // Push Previous button
    GTWidget::click(GTWidget::findWidget("prevPushButton"));

    // Expected result: third line is selected
    QStringList nameList4 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList4[length - 1] == "Mecopoda_sp.__Malaysia_", "The sequence is incorrect");
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 17, 604, 1));
}

GUI_TEST_CLASS_DEFINITION(test_6809) {
    // Open "COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Open General OP tab
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);

    // Set "Sort by" as "Name" and "Sort order" as "Ascending"
    auto sortByCombo = GTWidget::findComboBox("sortByComboBox");
    GTComboBox::selectItemByText(sortByCombo, "Name");

    auto sortOrderCombo = GTWidget::findComboBox("sortOrderComboBox");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");

    // Press "Sort" button
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected result:
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList[0] == "Bicolorana_bicolor_EF540830", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList[1] == "Conocephalus_discolor", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList[2] == "Conocephalus_percaudata", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList[15] == "Roeseliana_roeseli", "The 16 sequence is incorrect");
    CHECK_SET_ERR(nameList[16] == "Tettigonia_viridissima", "The 17 sequence is incorrect");
    CHECK_SET_ERR(nameList[17] == "Zychia_baranovi", "The 18 sequence is incorrect");

    // Set "Sort by" as "Name" and "Sort order" as "Descending"
    GTComboBox::selectItemByText(sortByCombo, "Name");
    GTComboBox::selectItemByText(sortOrderCombo, "Descending");

    // Press "Sort" button
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state:
    QStringList nameList1 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList1[0] == "Zychia_baranovi", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList1[1] == "Tettigonia_viridissima", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList1[2] == "Roeseliana_roeseli", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList1[15] == "Conocephalus_percaudata", "The 16 sequence is incorrect");
    CHECK_SET_ERR(nameList1[16] == "Conocephalus_discolor", "The 17 sequence is incorrect");
    CHECK_SET_ERR(nameList1[17] == "Bicolorana_bicolor_EF540830", "The 18 sequence is incorrect");

    // Set "Sort by" as "Length" and "Sort order" as "Descending"
    GTComboBox::selectItemByText(sortByCombo, "Length");
    GTComboBox::selectItemByText(sortOrderCombo, "Descending");

    // Press "Sort" button
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state:
    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2[0] == "Hetrodes_pupus_EF540832", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList2[1] == "Tettigonia_viridissima", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList2[2] == "Roeseliana_roeseli", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList2[15] == "Deracantha_deracantoides_EF540", "The 16 sequence is incorrect");
    CHECK_SET_ERR(nameList2[16] == "Zychia_baranovi", "The 17 sequence is incorrect");
    CHECK_SET_ERR(nameList2[17] == "Podisma_sapporensis", "The 18 sequence is incorrect");

    // Set "Sort by" as "Length" and "Sort order" as "Ascending"
    GTComboBox::selectItemByText(sortByCombo, "Length");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");

    // Press "Sort" button
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state:
    QStringList nameList3 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList3[0] == "Podisma_sapporensis", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList3[1] == "Zychia_baranovi", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList3[2] == "Deracantha_deracantoides_EF540", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList3[15] == "Conocephalus_discolor", "The 16 sequence is incorrect");
    CHECK_SET_ERR(nameList3[16] == "Bicolorana_bicolor_EF540830", "The 17 sequence is incorrect");
    CHECK_SET_ERR(nameList3[17] == "Hetrodes_pupus_EF540832", "The 18 sequence is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_6816) {
    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Set both primers "Y"
    QWidget* primerBox = GTWidget::findWidget("forwardPrimerBox");
    auto primerLe = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", primerBox));
    GTLineEdit::setText(primerLe, "y", true);

    primerBox = GTWidget::findWidget("reversePrimerBox");
    primerLe = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", primerBox));
    GTLineEdit::setText(primerLe, "y", true);

    // Expected state: "Show primer details" label is hidden, "Unable to calculate primer statistics." warning message is shown
    auto detailsLinkLabel = dynamic_cast<QLabel*>(GTWidget::findWidget("detailsLinkLabel"));
    CHECK_SET_ERR(detailsLinkLabel->isHidden(), "detailsLinkLabel unexpectedly shown");

    auto warningLabel = GTWidget::findLabel("warningLabel");
    CHECK_SET_ERR(warningLabel->text().contains("Unable to calculate primer statistics."), "Incorrect warning message");
}

GUI_TEST_CLASS_DEFINITION(test_6826) {
    // Open "Application settings", go to the "Resources" tab and set values "Optimize for CPU count" and "Threads limit" to 1.
    class ThreadsLimitScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::Resources);

            auto cpuBox = GTWidget::findSpinBox("cpuBox", dialog);
            GTSpinBox::setValue(cpuBox, 1, GTGlobals::UseKeyBoard);

            auto threadsBox = GTWidget::findSpinBox("threadBox", dialog);
            GTSpinBox::setValue(threadsBox, 1, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new ThreadsLimitScenario));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});

    class InSilicoWizardScenario : public CustomScenario {
    public:
        void run() {
            GTWidget::getActiveModalWidget();

            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << QFileInfo(testDir + "_common_data/fasta/400000_symbols_msa.fasta").absoluteFilePath()));
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setParameter("Primers URL", QFileInfo(testDir + "_common_data/cmdline/pcr/primers2.fa").absoluteFilePath());

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    // Open WD and choose the "In Silico PCR" sample.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new InSilicoWizardScenario()));
    GTUtilsWorkflowDesigner::addSample("In Silico PCR");

    // The task is finished with no timeouts/deadocks.
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_6847) {
    //    1. Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "/samples/FASTA", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Switch on the editing mode.
    QAction* editMode = GTAction::findActionByText("Switch on the editing mode");
    GTWidget::click(GTAction::button(editMode));

    QPoint editButtonPoint = GTMouseDriver::getMousePosition();
    GTMouseDriver::moveTo(editButtonPoint + QPoint(100, 0));  // Move mouse to the right into the sequence area.
    GTMouseDriver::click();

    // 3. Paste content with non-sequence characters
    // Expected state: log contains error message
    GTClipboard::setText("?!@#$%^*(");
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Paste sequence"}));
    MWMDIWindow* mdiWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    GTMouseDriver::moveTo(mdiWindow->mapToGlobal(mdiWindow->rect().center()));
    GTMouseDriver::click(Qt::RightButton);
    CHECK_SET_ERR(lt.hasError("No sequences detected in the pasted content."), "Expected error not found");
    GTWidget::click(GTAction::button(editMode));
}
GUI_TEST_CLASS_DEFINITION(test_6860) {
    // Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first column of the alignment.

    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);

    // Press the "Shift" button and do not release it.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);

    // Click on the 10th column.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(9);

    // Press the right arrow button.
    GTKeyboardDriver::keyPress(Qt::Key_Right);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTKeyboardDriver::keyRelease(Qt::Key_Right);

    // Expected state: columns from 1 to 11 are selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(0, 0, 11, 18));
}

GUI_TEST_CLASS_DEFINITION(test_6862) {
    //      Make sure the GUI of the Filter BAM/SAM files element is working correctly:
    // Open WD
    // Add Filter BAM/SAM files element to the scene
    // Highlight this element
    // Double click on the Accept flag value
    // Select "Mate strand"
    //  Expected state: "Mate strand" checked
    // Click on an empty place on the scene
    // Select Filter BAM/SAM files again
    //  Expected state: Accept flag value is "Mate strand", the Mate strand is checked

    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addElement("Filter BAM/SAM files");

    GTUtilsWorkflowDesigner::click("Filter BAM/SAM files");
    GTUtilsWorkflowDesigner::setParameter("Accept flag", "Mate strand", GTUtilsWorkflowDesigner::ComboChecks);

    QString checkboxValue = GTComboBox::getCurrentText(GTWidget::findComboBox("mainWidget"));
    CHECK_SET_ERR(checkboxValue == "Mate strand", QString("Accept flag value: expected 'Mate strand', current: '%1'").arg(checkboxValue))

    // Click on an empty place on the scene
    GTWidget::click(GTWidget::findWidget("sceneView"));
    // Select Filter BAM/SAM files element again
    GTUtilsWorkflowDesigner::click("Filter BAM/SAM files");

    // Check that Mate strand is checked
    GTUtilsWorkflowDesigner::clickParameter("Accept flag");
    GTUtilsWorkflowDesigner::clickParameter("Accept flag");
    checkboxValue = GTComboBox::getCurrentText(GTWidget::findComboBox("mainWidget"));
    CHECK_SET_ERR(checkboxValue == "Mate strand", QString("Accept flag value (1): expected 'Mate strand', current: '%1'").arg(checkboxValue))

    // To successfully complete the test
    GTWidget::click(GTWidget::findWidget("sceneView"));
}

GUI_TEST_CLASS_DEFINITION(test_6872) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class FillTrimAndMapWizardWithHumanT1 : public CustomScenario {
    public:
        void run() override {
            GTUtilsWizard::setParameter("Reference", QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath());
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setInputFiles({{dataDir + "samples/FASTA/human_T1.fa"}});
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            QString expectedRowNamingPolicy = "Sequence name from file";
            QString currentRowNamingPolicy = GTUtilsWizard::getParameter("Read name in result alignment").toString();
            CHECK_SET_ERR(expectedRowNamingPolicy == currentRowNamingPolicy,
                          QString("An incorrect default value of the 'Read name in result alignment' parameter: expected '%1', got '%2'")
                              .arg(expectedRowNamingPolicy)
                              .arg(currentRowNamingPolicy));
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };
    // 1. Open "Trim and Map Sanger reads" sample in workflow.
    // 2. Set human_T1.fa as input files on first and second wizard pages.
    // 3. Run schema.
    // Expected state: workflow stopped work with "Not enouch memory to finish the task." error message in the log.
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new WizardFiller("Map Sanger Reads to Reference", new FillTrimAndMapWizardWithHumanT1()));
    GTUtilsWorkflowDesigner::addSample("Trim and map Sanger reads");

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("Needed amount of memory for this task is"), "No expected message in the log");
}

GUI_TEST_CLASS_DEFINITION(test_6875) {
    // 1. Open "_common_data/genbank/HQ007052.gb" sequence.
    GTFileDialog::openFile(testDir + "_common_data/genbank/HQ007052.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Select "Actions > Analyze > Find restriction sites", check "DraRI" enzyme in the appeared dialog, click "OK".
    // Expected state: ugene not crashed
    GTUtilsDialog::waitForDialog(new FindEnzymesDialogFiller({"DraRI"}));
    GTMenu::clickMainMenuItem({"Actions", "Analyze", "Find restriction sites..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
}
GUI_TEST_CLASS_DEFINITION(test_6897) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first sequence in the name list
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");

    // Set text to the clipboard ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    // Paste the selection
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the copied sequence is inserted right below the selected sequence
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Unexpected name list size, expected: 19, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[1] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[1]));
}
GUI_TEST_CLASS_DEFINITION(test_6897_1) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first sequence in the name list
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");

    // Set text to the clipboard ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    // Press the Esc button
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Paste the selection
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the copied sequence is inserted right below the selected sequence
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Unexpected name list size, expected: 19, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[18] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[1]));
}
GUI_TEST_CLASS_DEFINITION(test_6897_2) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Set text to the clipboard ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA\r\n>human_T3\r\nACCTGA"
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n"
                         ">human_T2\r\nACCTGA\r\n"
                         ">human_T3\r\nACCTGA");

    // Select the 8th sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Deracantha_deracantoides_EF540");

    // Paste the selection
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the copied sequence is inserted right below the selected sequence
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 21, QString("Unexpected name list size, expected: 21, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[8] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[8]));
}
GUI_TEST_CLASS_DEFINITION(test_6898) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first sequence in the name list
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");

    // Set text to the clipboard ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    // Paste the selection with Ctrl+Alt+V
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);

    // Expected state: the copied sequence is inserted right above the selected sequence
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Unexpected name list size, expected: 19, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[0] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[0]));
}
GUI_TEST_CLASS_DEFINITION(test_6898_1) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first sequence in the name list
    GTUtilsMSAEditorSequenceArea::selectSequence("Phaneroptera_falcata");

    // Set text to the clipboard ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n"
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    // Press the Esc button
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // Paste the selection with Ctrl+Alt+V
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);

    // Expected state: the copied sequence is inserted right above the first sequence
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Unexpected name list size, expected: 19, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[0] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[0]));
}
GUI_TEST_CLASS_DEFINITION(test_6898_2) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Set text to the clipboard ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA\r\n>human_T3\r\nACCTGA"
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n"
                         ">human_T2\r\nACCTGA\r\n"
                         ">human_T3\r\nACCTGA");

    // Select the 8th sequence
    GTUtilsMSAEditorSequenceArea::selectSequence("Deracantha_deracantoides_EF540");

    // Paste the selection with Ctrl+Alt+V
    GTKeyboardDriver::keyPress(Qt::Key_Alt);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);

    // Expected state: the copied sequences are inserted right above the 8th sequence.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 21, QString("Unexpected name list size, expected: 21, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[7] == "human_T1", QString("Unexpected name, expected: \"human_T1\", current: %1").arg(names[7]));
}
GUI_TEST_CLASS_DEFINITION(test_6899) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::selectRows(0, 5);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the copied sequences are inserted right above the 8th sequence.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 24, QString("Unexpected name list size, expected: 21, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[6] == "Phaneroptera_falcata_1", QString("Unexpected name, expected: \"Phaneroptera_falcata_1\", current: %1").arg(names[6]));
}
GUI_TEST_CLASS_DEFINITION(test_6899_1) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto copyType = GTWidget::findComboBox("copyType");

    GTComboBox::selectItemByText(copyType, "Plain text");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 5), QPoint(16, 9));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    QString expectedClipboard = "CTACTAATTCGA\nTTATTAATTCGA\nTTGCTAATTCGA\nTTATTAATCCGG\nCTATTAATTCGA";
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedClipboard, QString("Unexpected clipboard text, expected: %1, current: %2").arg(expectedClipboard).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_6901) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Advanced", "Convert to RNA alphabet (T->U)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto copyType = GTWidget::findComboBox("copyType");
    GTComboBox::selectItemByText(copyType, "Plain text");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 5), QPoint(16, 9));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    QString expectedClipboard = "CUACUAAUUCGA\nUUAUUAAUUCGA\nUUGCUAAUUCGA\nUUAUUAAUCCGG\nCUAUUAAUUCGA";
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedClipboard, QString("Unexpected clipboard text, expected: %1, current: %2").arg(expectedClipboard).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_6903) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    auto toDnaButton = GTWidget::findToolButton("convertNucleicAlphabetButton");
    GTWidget::click(toDnaButton);

    auto copyType = GTWidget::findComboBox("copyType");
    GTComboBox::selectItemByText(copyType, "Plain text");

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(5, 5), QPoint(16, 9));

    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy/Paste", "Copy (custom format)"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    QString expectedClipboard = "CUACUAAUUCGA\nUUAUUAAUUCGA\nUUGCUAAUUCGA\nUUAUUAAUCCGG\nCUAUUAAUUCGA";
    QString clipboardText = GTClipboard::text();
    CHECK_SET_ERR(clipboardText == expectedClipboard, QString("Unexpected clipboard text, expected: %1, current: %2").arg(expectedClipboard).arg(clipboardText));
}

GUI_TEST_CLASS_DEFINITION(test_6903_1) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);

    // Check that button is present.
    auto button = GTWidget::findToolButton("convertNucleicAlphabetButton");
    CHECK_SET_ERR(button->isVisible(), QString("Check 1. Conversion button must be present"));

    // Lock the document and check that the button is not present.
    GTUtilsDocument::lockDocument("COI.aln");
    CHECK_SET_ERR(!button->isVisible(), QString("Check 2. Conversion button must not be present"));

    // Lock the document and check that the button is back.
    GTUtilsDocument::unlockDocument("COI.aln");
    CHECK_SET_ERR(button->isVisible(), QString("Check 3. Conversion button must be present"));
}

GUI_TEST_CLASS_DEFINITION(test_6903_2) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    auto nucleicButton = GTWidget::findToolButton("convertNucleicAlphabetButton");
    auto aminoButton = GTWidget::findToolButton("convertAminoAlphabetButton");
    CHECK_SET_ERR(nucleicButton->isVisible(), QString("Convert to nucleic button must be visible"));
    CHECK_SET_ERR(nucleicButton->text() == "RNA", QString("Wrong button text in DNA mode, expected 'RNA', got '" + nucleicButton->text() + "'"));
    CHECK_SET_ERR(!aminoButton->isVisible(), QString("Convert to amino button must not be visible"));

    // Insert a character that will change alignment alphabet to Extended DNA.
    GTUtilsMSAEditorSequenceArea::replaceSymbol({1, 1}, 'R');
    CHECK_SET_ERR(!nucleicButton->isVisible(), QString("Convert to nucleic button must not be visible in Extended DNA mode"));
    CHECK_SET_ERR(!aminoButton->isVisible(), QString("Convert to amino button must not be visible in Extended DNA mode"));

    // Undo, original state must be restored.
    GTUtilsMsaEditor::undo();
    CHECK_SET_ERR(nucleicButton->isVisible(), QString("Convert to nucleic button must be visible /2"));
    CHECK_SET_ERR(nucleicButton->text() == "RNA", QString("Wrong button text in DNA mode, expected 'RNA', got '" + nucleicButton->text() + "' /2"));
    CHECK_SET_ERR(!aminoButton->isVisible(), QString("Convert to amino button must not be visible /2"));

    // Insert a character that will change alignment alphabet to RAW.
    GTUtilsMSAEditorSequenceArea::replaceSymbol({1, 1}, 'Q');
    CHECK_SET_ERR(nucleicButton->isVisible(), QString("Convert to nucleic button must be visible in RAW mode"));
    CHECK_SET_ERR(nucleicButton->text() == "DNA", QString("Wrong button text in RAW, expected 'DNA', got '" + nucleicButton->text() + "'"));
    CHECK_SET_ERR(aminoButton->isVisible(), QString("Convert to amino button must be visible in RAW mode"));
}

GUI_TEST_CLASS_DEFINITION(test_6916) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsMsaEditor::selectRows(0, 5);
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the copied sequences have original names + _1.
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 24, QString("Unexpected name list size, expected: 21, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[6] == "Phaneroptera_falcata_1", QString("Unexpected name, expected: \"Phaneroptera_falcata_1\", current: %1").arg(names[6]));
    CHECK_SET_ERR(names[7] == "Isophya_altaica_EF540820_1", QString("Unexpected name, expected: \"Isophya_altaica_EF540820_1\", current: %1").arg(names[7]));
    CHECK_SET_ERR(names[8] == "Bicolorana_bicolor_EF540830_1", QString("Unexpected name, expected: \"Bicolorana_bicolor_EF540830_1\", current: %1").arg(names[8]));
    CHECK_SET_ERR(names[9] == "Roeseliana_roeseli_1", QString("Unexpected name, expected: \"Roeseliana_roeseli_1\", current: %1").arg(names[9]));
    CHECK_SET_ERR(names[10] == "Montana_montana_1", QString("Unexpected name, expected: \"Montana_montana_1\", current: %1").arg(names[10]));
}

GUI_TEST_CLASS_DEFINITION(test_6916_1) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Add sequence from file
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/murine.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // Add the same sequence one more time
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/murine.gb"));
    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_LOAD_SEQ", "Sequence from file"}));
    GTWidget::click(GTUtilsMdi::activeWindow(), Qt::RightButton);

    // Expected state: two sequences have been added to the end of alignment, one of them has _1 suffix
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 20, QString("Unexpected name list size, expected: 21, current: %1").arg(names.size()));

    CHECK_SET_ERR(names[18] == "NC_001363", QString("Unexpected name, expected: \"NC_001363\", current: %1").arg(names[18]));
    CHECK_SET_ERR(names[19] == "NC_001363_1", QString("Unexpected name, expected: \"NC_001363_1\", current: %1").arg(names[19]));
}

GUI_TEST_CLASS_DEFINITION(test_6924) {
    class Scenario : public CustomScenario {
    public:
        void run() {
            GTUtilsWizard::setParameter("Input file(s)", QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq").absoluteFilePath());
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };
    // Open "Tools" -> "NGS data analysis" -> "Reads quality control..." workflow
    GTUtilsDialog::add(new StartupDialogFiller());
    GTUtilsDialog::add(new WizardFiller("Quality Control by FastQC Wizard", new Scenario()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Reads quality control..."});
    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
    // There should be no notifications.
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Unexpected notification");
}

GUI_TEST_CLASS_DEFINITION(test_6926) {
    class AddCustomToolScenario : public CustomScenario {
    public:
        void run() {
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            GTWidget::click(GTWidget::findWidget("ArrowHeader_Custom tools"));

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(testDir + "_common_data/workflow/custom tools configs/my_custom_tool.xml"));
            GTWidget::click(GTWidget::findWidget("pbImport"));

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    class CheckCustomToolScenario : public CustomScenario {
    public:
        void run() {
            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::ExternalTools);

            auto treeWidget = GTWidget::findTreeWidget("twCustomTools");
            QStringList itemNames = GTTreeWidget::getItemNames(treeWidget);
            CHECK_SET_ERR(itemNames.length() == 1, "Expected to have 1 item in the tree, got: " + QString::number(itemNames.length()));
            CHECK_SET_ERR(itemNames.first() == "My custom tool", "Expected to find 'My custom tool' in the list, got: " + itemNames.first());

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new AddCustomToolScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CheckCustomToolScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."});
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_6927) {
    // Open COI.aln.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first column of the alignment.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(0);

    // Press the right arrow button 9 times. The 10th column should now be selected.
    for (int i = 0; i < 9; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
    }
    // Press the "Shift" button and do not release it.
    GTKeyboardDriver::keyPress(Qt::Key_Shift);
    // Click on the 10th column.
    GTUtilsMSAEditorSequenceArea::selectColumnInConsensus(19);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);

    // Expected state: columns from 10 to 20 are selected.
    GTUtilsMSAEditorSequenceArea::checkSelectedRect(QRect(9, 0, 11, 18));
}

GUI_TEST_CLASS_DEFINITION(test_6941) {
    // Open de novo assembly dialog
    // Fill it and run
    // Expected result: no errors

    GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class custom : public CustomScenario {
    public:
        void run() {
            GTUtilsWizard::setParameter("FASTQ files", QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq").absoluteFilePath());

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::setParameter("Reference genome", QFileInfo(testDir + "_common_data/cmdline/external-tool-support/spades/reference_1K.fa.gz").absoluteFilePath());

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    GTUtilsDialog::add(new ConfigurationWizardFiller("Configure Raw DNA-Seq Data Processing", {"Single-end"}));
    GTUtilsDialog::add(new WizardFiller("Raw DNA-Seq Data Processing Wizard", new custom()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Raw DNA-Seq data processing..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    // Expected: The dashboard appears
    GTUtilsDashboard::getDashboard();
    // There should be no notifications.
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Unexpected notification");
}

GUI_TEST_CLASS_DEFINITION(test_6952) {
    // 1. Click Tools -> BLAST -> Remote NCBI BLAST... Choose _common_data/fasta/human_T1_cutted.fa as input file
    // 2. CLick Next two times, then Run
    // Expected state: no errors
    class RemoteBLASTWizardFiller : public CustomScenario {
    public:
        void run() override {
            GTWidget::getActiveModalWidget();

            GTUtilsWizard::setInputFiles(QList<QStringList>() << (QStringList() << QFileInfo(testDir + "_common_data/fasta/human_T1_cutted.fa").absoluteFilePath()));

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };
    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Remote BLASTing Wizard", new RemoteBLASTWizardFiller()));
    GTMenu::clickMainMenuItem({"Tools", "BLAST", "Remote NCBI BLAST..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    QStringList errors = GTUtilsWorkflowDesigner::getErrors();
    CHECK_SET_ERR(errors.empty(), "Unexpected errors");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(),
                  "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString());
}

GUI_TEST_CLASS_DEFINITION(test_6953) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first sequence
    GTUtilsMsaEditor::clickSequence(0);

    // Press Ctrl+X
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);

    // Press Ctrl+V
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 18, QString("Unexpected name list size, expected: 18, current: %1").arg(names.size()));

    CHECK_SET_ERR(names[0] == "Isophya_altaica_EF540820", QString("Unexpected name, expected: \"Isophya_altaica_EF540820\", current: %1").arg(names[0]));
    CHECK_SET_ERR(names[1] == "Phaneroptera_falcata", QString("Unexpected name, expected: \"Phaneroptera_falcata\", current: %1").arg(names[1]));

    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('z', Qt::ControlModifier);

    // Select (0, 0, 4, 2) rectangle
    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(0, 0), QPoint(3, 1));

    // Press Ctrl+X
    GTKeyboardDriver::keyClick('x', Qt::ControlModifier);

    // Press Ctrl+V
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    GTUtilsMSAEditorSequenceArea::checkSelection(QPoint(0, 2), QPoint(3, 3), "TAAG\nTAAG");

    // Expected state: the selection pasted under the Isophya_altaica_EF540820 sequence
    QStringList names1 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names1.size() == 20, QString("Unexpected name list size, expected: 18, current: %1").arg(names1.size()));

    CHECK_SET_ERR(names1[2] == "Phaneroptera_falcata_1", QString("Unexpected name, expected: \"Phaneroptera_falcata_1\", current: %1").arg(names1[2]));
    CHECK_SET_ERR(names1[3] == "Isophya_altaica_EF540820_1", QString("Unexpected name, expected: \"Isophya_altaica_EF540820_1\", current: %1").arg(names1[3]));
}

GUI_TEST_CLASS_DEFINITION(test_6954) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select any sequence in the name list
    GTUtilsMsaEditor::clickSequence(0);

    // Click to "Consensus" label above the list
    GTWidget::click(GTWidget::findWidget("consensusLabel"));

    // Press Ctrl-C & Ctrl-V
    GTKeyboardDriver::keyClick('c', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the sequence is added to the MSA right  below the selection because the focus is on the MSA
    QStringList names = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(names.size() == 19, QString("Unexpected name list size, expected: 19, current: %1").arg(names.size()));
    CHECK_SET_ERR(names[1] == "Phaneroptera_falcata_1", QString("Unexpected name, expected: \"Phaneroptera_falcata_1\", current: %1").arg(names[1]));

    // Move focus to the Project Tree view
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    // Press Ctrl+V
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);

    // Expected state: the sequence is inserted into the project because the focus now is on the project list.
    GTUtilsProjectTreeView::checkItem("Phaneroptera_falcata");
}

GUI_TEST_CLASS_DEFINITION(test_6959) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Select the first 5 sequences
    GTUtilsMsaEditor::selectRows(0, 4);

    // Sort–>by name
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);

    auto sortByCombo = GTWidget::findComboBox("sortByComboBox");
    GTComboBox::selectItemByText(sortByCombo, "Name");

    auto sortOrderCombo = GTWidget::findComboBox("sortOrderComboBox");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");

    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected result:
    QStringList nameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList[0] == "Bicolorana_bicolor_EF540830", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList[1] == "Isophya_altaica_EF540820", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList[2] == "Montana_montana", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList[3] == "Phaneroptera_falcata", "The 4 sequence is incorrect");
    CHECK_SET_ERR(nameList[4] == "Roeseliana_roeseli", "The 5 sequence is incorrect");
    CHECK_SET_ERR(nameList[5] == "Metrioptera_japonica_EF540831", "The 6 sequence is incorrect");

    // Select the first sequence
    GTUtilsMsaEditor::clickSequence(0);

    // Sort–>by name
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);
    GTComboBox::selectItemByText(sortByCombo, "Name");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the order of all sequences is changed
    QStringList nameList1 = GTUtilsMSAEditorSequenceArea::getNameList();

    CHECK_SET_ERR(nameList1[0] == "Bicolorana_bicolor_EF540830", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList1[1] == "Conocephalus_discolor", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList1[2] == "Conocephalus_percaudata", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList1[3] == "Conocephalus_sp.", "The 4 sequence is incorrect");
    CHECK_SET_ERR(nameList1[4] == "Deracantha_deracantoides_EF540", "The 5 sequence is incorrect");
    CHECK_SET_ERR(nameList1[5] == "Gampsocleis_sedakovii_EF540828", "The 6 sequence is incorrect");

    // Select the first 5 sequences
    GTUtilsMsaEditor::selectRows(0, 4);

    // Sort–>by length
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);
    GTComboBox::selectItemByText(sortByCombo, "Length");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the selected sequences are sorted, the order of the other sequences is not changed
    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2[0] == "Deracantha_deracantoides_EF540", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList2[1] == "Bicolorana_bicolor_EF540830", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList2[2] == "Conocephalus_discolor", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList2[3] == "Conocephalus_percaudata", "The 4 sequence is incorrect");
    CHECK_SET_ERR(nameList2[4] == "Conocephalus_sp.", "The 5 sequence is incorrect");
    CHECK_SET_ERR(nameList2[5] == "Gampsocleis_sedakovii_EF540828", "The 6 sequence is incorrect");

    // Select the first sequence
    GTUtilsMsaEditor::clickSequence(0);

    // Sort–>by length
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);
    GTComboBox::selectItemByText(sortByCombo, "Length");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the order of all sequences is changed
    QStringList nameList3 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList3[0] == "Podisma_sapporensis", "The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList3[1] == "Zychia_baranovi", "The 2 sequence is incorrect");
    CHECK_SET_ERR(nameList3[2] == "Deracantha_deracantoides_EF540", "The 3 sequence is incorrect");
    CHECK_SET_ERR(nameList3[3] == "Mecopoda_sp.__Malaysia_", "The 4 sequence is incorrect");
    CHECK_SET_ERR(nameList3[4] == "Bicolorana_bicolor_EF540830", "The 5 sequence is incorrect");
    CHECK_SET_ERR(nameList3[5] == "Conocephalus_discolor", "The 6 sequence is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_6960) {
    class ProjectPathValidationScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* const dialog = GTWidget::getActiveModalWidget();
            const auto lePath = GTWidget::findLineEdit("projectFilePathEdit", dialog);

            const QString expected = U2::UserAppsSettings().getDefaultDataDirPath() + "/project.uprj";
            const QString actual = lePath->text();
            CHECK_SET_ERR(expected == actual,
                          QString("Default project file path: expected \"%1\", actual \"%2\"").arg(expected, actual))
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    // 1. Select "File->New project...". The "Create New Project" dialog appears
    //    Expected state: In this dialog "Save project to file" field contains "~/Documents/UGENE_Data/project.uprj"
    // 2. Close this dialog
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("CreateNewProjectDialog", QDialogButtonBox::Cancel, new ProjectPathValidationScenario()));
    GTMenu::clickMainMenuItem({"File", "New project..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open any file
    GTUtilsProject::openFile(dataDir + "samples/ABIF/A01.abi");
    // 4. Select "File->Save project as...". The "Save project as" dialog appears
    //    Expected state: In this dialog "Save project to file" field contains "~/Documents/UGENE_Data/project.uprj"
    // 5. Close this dialog
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("CreateNewProjectDialog", QDialogButtonBox::Cancel, new ProjectPathValidationScenario()));
    GTMenu::clickMainMenuItem({"File", "Save project as..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 6. Select "File->Export project...". The "Export Project" dialog appears
    //    Expected state: In this dialog "Project file name" field contains "~/Documents/UGENE_Data/project.uprj"
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("ExportProjectDialog", QDialogButtonBox::Cancel, new ProjectPathValidationScenario()));
    GTMenu::clickMainMenuItem({"File", "Export project..."});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_6963) {
    // 1. Open Application Settings and check if WindowsVista on Windows or Macintosh on macOS styles are exist
    class CheckStyleScenario : public CustomScenario {
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            AppSettingsDialogFiller::openTab(AppSettingsDialogFiller::General);

            auto styleCombo = GTWidget::findComboBox("styleCombo", dialog);

            QString text;
            if (isOsWindows()) {
                text = "WindowsVista";
            } else if (isOsMac()) {
                text = "Macintosh";
            } else if (isOsLinux()) {
                text = "Fusion";
            }

            GTComboBox::selectItemByText(styleCombo, text);
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new AppSettingsDialogFiller(new CheckStyleScenario()));
    GTMenu::clickMainMenuItem({"Settings", "Preferences..."}, GTGlobals::UseMouse);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_6966) {
    // Open 100bp file
    GTUtilsProject::openFile(testDir + "_common_data/fasta/100bp.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Enable enzymes auto-annotations (with default enzymes selected).
    GTUtilsDialog::waitForDialog(new PopupChooser({"Restriction Sites"}));
    GTWidget::click(GTWidget::findWidget("AutoAnnotationUpdateAction"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Open 200bp file
    GTUtilsProject::openFile(testDir + "_common_data/fasta/200bp.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 0..200 range is auto annotated with enzymes
    // The 'EcoRV' is found only once in the 200bp sequence at the position > 100.
    GTUtilsAnnotationsTreeView::clickItem("EcoRV", 1, true);
}

GUI_TEST_CLASS_DEFINITION(test_6968) {
    // Check 'too-many-results' scenario.

    // Open 'human_T1.fa'
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Open "Find restriction sites" dialog
    class SelectAllScenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            // Select all sites.
            GTWidget::click(GTWidget::findWidget("selectAllButton", dialog));
            GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_ANALYSE", "Find restriction sites"}));
    GTUtilsDialog::add(new FindEnzymesDialogFiller(QStringList(), new SelectAllScenario()));
    GTUtilsSequenceView::openPopupMenuOnSequenceViewArea();
}

GUI_TEST_CLASS_DEFINITION(test_6971) {
    // Open WD.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class CheckWizardIsActiveAndCancelItScenario : public CustomScenario {
    public:
        void run() {
            GTWidget::getActiveModalWidget();
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("Illumina PE Reads De Novo Assembly Wizard", new CheckWizardIsActiveAndCancelItScenario()));

    // Add De novo assemble Illumina PE Reads sample
    GTUtilsWorkflowDesigner::addSample("De novo assemble Illumina PE Reads");

    // Expected state: sample will appear
    // Click Cancel
}

GUI_TEST_CLASS_DEFINITION(test_6979) {
    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Build phy-tree with default settings
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_6979_COI.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check tree widget with tabs was created.

    auto tabWidget = GTWidget::findTabWidget("MsaEditorTreeTab");
    CHECK_SET_ERR(tabWidget->currentIndex() == 0, "Expected first tab to be active")

    // Build another phy-tree
    GTUtilsDialog::waitForDialog(new BuildTreeDialogFiller(sandBoxDir + "test_6979_COI_1.nwk", 0, 0, true));
    GTWidget::click(GTAction::button("Build Tree"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: a tab with tree created last is activated.
    CHECK_SET_ERR(tabWidget->currentIndex() == 1, "Expected second tab to be active")
}

GUI_TEST_CLASS_DEFINITION(test_6981) {
    // Open murine.gb
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Create an annotation of length 1 close to the sequence end, so it will be on the last (but never the first) line of Det View.
    GTUtilsAnnotationsTreeView::createAnnotation("<auto>", "ann", "complement(5809..5809)");

    // Ensure that the annotation we created is not selected: select another annotation.
    GTUtilsAnnotationsTreeView::selectItemsByName({"CDS"});

    // Click the annotation location in DetView.
    GTUtilsSequenceView::clickAnnotationDet("ann", 5809);

    // Expected state: the annotation has been selected
    QString annTreeItem = GTUtilsAnnotationsTreeView::getSelectedItem();
    CHECK_SET_ERR(annTreeItem == "ann", QString("Incorrect selected item name, expected: ann, current: %1").arg(annTreeItem));
}

GUI_TEST_CLASS_DEFINITION(test_6990_1) {
    // Use context menu to check "Sort By leading gap" action.

    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_leading_gap"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Expected state: the order of the sequences is not changed
    QStringList nameList0 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList0[0] == "Phaneroptera_falcata", "1. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList0[17] == "Hetrodes_pupus_EF540832", "1. The last sequence is incorrect");

    // Insert gap to the (0, 0) position.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Sort by leading gap.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_leading_gap"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Expected state: the last sequence is Phaneroptera_falcata
    QStringList nameList1 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList1[0] == "Isophya_altaica_EF540820", "2. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList1[17] == "Phaneroptera_falcata", "2. The last sequence is incorrect");

    // Sort by leading gap -> Descending.
    GTUtilsDialog::waitForDialog(new PopupChooser({MSAE_MENU_SORT, "action_sort_by_leading_gap_descending"}));
    GTUtilsMSAEditorSequenceArea::callContextMenu();

    // Expected state: the last sequence is Hetrodes_pupus_EF540832.
    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2[0] == "Phaneroptera_falcata", "3. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList2[17] == "Hetrodes_pupus_EF540832", "3. The last sequence is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_6990_2) {
    // Use Options panel tab to check "Sort By leading gap" action.

    // Open COI.aln
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    // Sort by leading gap
    GTUtilsOptionPanelMsa::openTab(GTUtilsOptionPanelMsa::General);
    GTUtilsOptionPanelMsa::checkTabIsOpened(GTUtilsOptionPanelMsa::General);

    auto sortByCombo = GTWidget::findComboBox("sortByComboBox");
    GTComboBox::selectItemByText(sortByCombo, "Leading gap");

    auto sortOrderCombo = GTWidget::findComboBox("sortOrderComboBox");
    GTComboBox::selectItemByText(sortOrderCombo, "Ascending");
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the order of the sequences is not changed
    QStringList nameList0 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList0[0] == "Phaneroptera_falcata", "1. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList0[17] == "Hetrodes_pupus_EF540832", "1. The last sequence is incorrect");

    // Insert gap to the (0, 0) position.
    GTUtilsMSAEditorSequenceArea::click(QPoint(0, 0));
    GTKeyboardDriver::keyClick(Qt::Key_Space);

    // Sort by leading gap.
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the last sequence is Phaneroptera_falcata
    QStringList nameList1 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList1[0] == "Isophya_altaica_EF540820", "2. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList1[17] == "Phaneroptera_falcata", "2. The last sequence is incorrect");

    // Sort by leading gap -> Descending.
    GTComboBox::selectItemByText(sortOrderCombo, "Descending");
    GTWidget::click(GTWidget::findWidget("sortButton"));

    // Expected state: the last sequence is Hetrodes_pupus_EF540832.
    QStringList nameList2 = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(nameList2[0] == "Phaneroptera_falcata", "3. The 1 sequence is incorrect");
    CHECK_SET_ERR(nameList2[17] == "Hetrodes_pupus_EF540832", "3. The last sequence is incorrect");
}

GUI_TEST_CLASS_DEFINITION(test_6995) {
    // Open an MCA object and use context menu action to go to start position of direct and complement reads.
    GTFileDialog::openFile(testDir + "_common_data/sanger", "alignment_short.ugenedb");
    GTUtilsMcaEditor::checkMcaEditorWindowIsActive();

    McaEditorReferenceArea* referenceArea = GTUtilsMcaEditor::getReferenceArea();

    U2Region visibleRange = referenceArea->getVisibleRange();
    CHECK_SET_ERR(visibleRange.startPos == 0, "Invalid start position");

    // Check direct read first.
    GTUtilsMcaEditor::clickReadName(1);
    GTUtilsDialog::waitForDialog(new PopupChooser({MCAE_MENU_NAVIGATION, "center-read-start-end-action"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();

    visibleRange = referenceArea->getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(2053), "Direct read is not centered: " + visibleRange.toString());

    // Check complement read.
    GTUtilsMcaEditor::clickReadName(2);
    GTUtilsDialog::waitForDialog(new PopupChooser({MCAE_MENU_NAVIGATION, "center-read-start-end-action"}));
    GTUtilsMcaEditorSequenceArea::callContextMenu();

    visibleRange = referenceArea->getVisibleRange();
    CHECK_SET_ERR(visibleRange.contains(6151), "Complement read is not centered: " + visibleRange.toString());
}

GUI_TEST_CLASS_DEFINITION(test_6999) {
    // 1. Create read_only_dir
    const QString projectPath = QFileInfo(sandBoxDir + "read_only_dir/project.uprj").absoluteFilePath();

    QDir().mkpath(sandBoxDir + "read_only_dir");
    GTFile::setReadOnly(sandBoxDir + "read_only_dir");

    // 2. Open any file
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::StandardButton::Ok));
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("New Project", projectPath));
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Select "File->Save project as..."
    // 4. Save project to file "read_only_dir/project.proj"
    // 5. Click "Save"
    //    Expected state: Message Box with text "Folder is read-only" appears
    GTMenu::clickMainMenuItem({"File", "Save project as..."});

    QWidget* dialog = GTWidget::getActiveModalWidget();
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

GUI_TEST_CLASS_DEFINITION(test_7000) {
    // 1. Create read_only_dir
    QDir().mkpath(sandBoxDir + "read_only_dir");
    GTFile::setReadOnly(sandBoxDir + "read_only_dir");

    // 2. Create new project and open samples/FASTA/human_T1.fa
    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("New Project", sandBoxDir + "proj.uprj"));
    GTMenu::clickMainMenuItem({"File", "New project..."});
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Call "New annotation" dialog (Ctrl+N).
    // 4. Set region 1..1.
    // 5. Set as "New document" "path to/read_only_dir/annot.gb".
    // 6. Click "Create".
    QString annotationPath = QFileInfo(sandBoxDir + "read_only_dir/annot.gb").absoluteFilePath();
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "", "1..1", annotationPath));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTLogTracer lt;

    // 7.1. Close UGENE.
    // 7.2. Click "Yes", then "Save", then "Cancel".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils("/", GTGlobals::UseMouse, GTFileDialogUtils::Cancel));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Save, "permission", "permissionBox"));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Yes", "Save document: "));
    GTMenu::clickMainMenuItem({"File", "Exit"});
    //      Expected state: 1) The log has "Task {Shutdown} canceled" message;
    //                      2) Project tree has "annot.gb" document;
    //                      3) Annotations tree has "annot.gb" item.
    CHECK_SET_ERR(lt.hasMessage("Task {Shutdown} canceled"), "Expected message not found");
    GTUtilsProjectTreeView::getItemCenter("Annotations");
    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 1)");

    lt.clear();

    // 8.1. Close UGENE.
    // 8.2. Click "Yes", then "Cancel".
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Cancel, "permission", "permissionBox"));
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Yes", "Save document: "));
    GTMenu::clickMainMenuItem({"File", "Exit"});
    //      Expected state: similar.
    CHECK_SET_ERR(lt.hasMessage("Task {Shutdown} canceled"), "Expected message not found");
    GTUtilsProjectTreeView::getItemCenter("Annotations");
    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 1)");

    lt.clear();

    // 9.1. Close UGENE.
    // 9.2. Click "Cancel".
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("Cancel"));
    GTMenu::clickMainMenuItem({"File", "Exit"});
    //      Expected state: similar.
    CHECK_SET_ERR(lt.hasMessage("Task {Shutdown} canceled"), "Expected message not found");
    GTUtilsProjectTreeView::getItemCenter("Annotations");
    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 1)");

    // 10. Create another annotation with region 1..1 and path "path to/read_only_dir/annot1.gb".
    annotationPath = QFileInfo(sandBoxDir + "read_only_dir/annot1.gb").absoluteFilePath();
    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "misc_feature", "", "1..1", annotationPath));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    class Clicker : public CustomScenario {
    public:
        void run() override {
            auto labelsList = GTWidget::findLabelByText("Save document", GTWidget::getActiveModalWidget());
            QMessageBox::StandardButton b = labelsList.first()->text().endsWith("annot1.gb") ? QMessageBox::Cancel : QMessageBox::No;
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(b));
        }
    };

    lt.clear();

    // 11. Close UGENE.
    // 12. Click "No", then "Cancel"
    GTUtilsDialog::waitForDialog(new Filler("", new Clicker()));
    GTUtilsDialog::waitForDialog(new Filler("", new Clicker()));
    GTMenu::clickMainMenuItem({"File", "Exit"});
    //     Expected state: similar.
    CHECK_SET_ERR(lt.hasMessage("Task {Shutdown} canceled"), "Expected message not found");
    GTUtilsProjectTreeView::getItemCenter("annot.gb");
    GTUtilsProjectTreeView::getItemCenter("annot1.gb");
    GTUtilsAnnotationsTreeView::findItem("Annotations [annot.gb] *");
    GTUtilsAnnotationsTreeView::findItem("Annotations [annot1.gb] *");
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2
