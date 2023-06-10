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

#include "GTTestsAssemblyBrowser.h"
#include <GTUtilsMsaEditor.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <src/GTUtilsSequenceView.h>
#include <system/GTClipboard.h>

#include <QApplication>
#include <QDir>

#include <U2Core/GUrlUtils.h>

#include <U2Designer/PropertyWidget.h>

#include "GTGlobals.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "primitives/GTComboBox.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportConsensusDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportCoverageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExtractAssemblyRegionDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTFile.h"

namespace U2 {

namespace GUITest_Assembly_browser {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // It is possible to reach negative coord in assembly browser (UGENE-105)

    // 1. Open _common_data/scenarios/assembly/example-alignment.ugenedb
    GTFileDialog::openFile(testDir + "_common_data/scenarios/assembly/", "example-alignment.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* window = GTUtilsAssemblyBrowser::getActiveAssemblyBrowserWindow();
    GTWidget::click(window);
    // 2. Zoom in until overview selection transforms to cross-hair
    for (int i = 0; i < 24; i++) {
        GTKeyboardDriver::keyClick('=', Qt::ShiftModifier);
    }
    // 3. Move it to the very left
    GTKeyboardDriver::keyClick(Qt::Key_Home);
    // 4. Try to zoom out
    for (int i = 0; i < 24; i++) {
        GTKeyboardDriver::keyClick('-');
    }
    // Expected state: coordinates is not negative
    // CHECK_SET_ERR(AssemblyRuler::browser->calcAsmPosX(qint pos), "Coordinates is negative");
    auto assRuler = GTWidget::findWidget("AssemblyRuler", window);

    QObject* l = assRuler->findChild<QObject*>("start position");
    CHECK_SET_ERR(l != nullptr, "first QObject for taking cursor name not found");

    QObject* startPositionObject = l->findChild<QObject*>();
    CHECK_SET_ERR(startPositionObject != nullptr, "second QObject for taking cursor name not found");

    QString coordinate = startPositionObject->objectName();
    CHECK_SET_ERR(!coordinate.contains("-"), "coordinate is negative:" + coordinate);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 1. open view for _common_data\scenarios\assembly\example-alignment.bam
    // GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller("replace" ));
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller());
    GTFileDialog::openFile(testDir + "_common_data/scenarios/assembly/", "example-alignment.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();
    // 2. convert bam file to example-alignment.ugenedb
    // Expected state: conversion finished without error
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    Test default values and bounds of all GUI-elements.

    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on the consensus area, select "Export coverage" menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    QList<ExportCoverageDialogFiller::Action> actions;

    //    3. Check default values.
    //    file path line edit - is "%ugene_data%/chrM_coverage.bedgraph";
    //    format - "bedgraph";
    //    compress check box - is not set;
    //    additional options combobox - is invisible;
    //    threshold - value is 1, min value is 0, max value is INT_MAX.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFormat, "Bedgraph");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckCompress, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckOptionsVisibility, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckThreshold, 1);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckThresholdBounds, QPoint(0, 65535));

    //    4. Set format "histogram".
    //    Expected state: additional options combobox is still invisible.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Histogram");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckOptionsVisibility, false);

    //    5. Set format "per-base"
    //    Expected state: additional options combobox becomes visible, export coverage checkbox is set, export bases count checkbox is not set.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckOptionsVisibility, true);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckExportCoverage, true);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckExportBasesQuantity, false);

    //    6. Set any file path via select file dialog.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SelectFile, sandBoxDir + "/common_assembly_browser/test_0010.txt");

    //    7. Cancel "Export the Assembly Coverage" dialog.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, QVariant());
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();

    GTUtilsTaskTreeView::waitTaskFinished();

    //    8. Create a file it the same folder as you set in the point 6 with name "chrM_coverage.bedgraph".
    GTFile::create(sandBoxDir + "/common_assembly_browser/chrM_coverage.bedgraph");

    //    9. Call "Export the Assembly Coverage" dialog  again.
    //    Expected state: the file path is "%path_from_point_6%/chrM_coverage_1.bedgraph"
    actions.clear();
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(QFileInfo(sandBoxDir + "common_assembly_browser/chrM_coverage_1.bedgraph").absoluteFilePath()));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, QVariant());
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //    Test compress checkbox GUI action, test format changing GUI action

    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on the consensus area, select "Export coverage" menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    QList<ExportCoverageDialogFiller::Action> actions;
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));

    //    3. Check the "compress" checkbox.
    //    Expected state: a ".gz" suffix was added to the file path.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetCompress, true);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph.gz"));

    //    4. Uncheck the "compress" checkbox.
    //    Expected state: the ".gz" suffix was removed to the file path.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetCompress, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));

    //    5. Write the ".gz" suffix to the file path manually, then check the checkbox.
    //    Expected state: the file path is not changed.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph.gz"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetCompress, true);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph.gz"));

    //    6. Remove the ".gz" suffix from the file path manually, then uncheck the checkbox.
    //    Expected state: the file path is not changed.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetCompress, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));

    //    7. Set format "Histogram".
    //    Expected state: the file extension is ".histogram".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Histogram");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.histogram"));

    //    8. Set format "Per base".
    //    Expected state: the file extension is ".txt".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.txt"));

    //    9. Set format "Bedgraph".
    //    Expected state: the file extension is ".bedgraph".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Bedgraph");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph"));

    //    10.  the "compress" checkbox.
    //    Expected state: the file extension is ".bedgraph.gz".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetCompress, true);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph.gz"));

    //    11. Set format "Histogram".
    //    Expected state: the file extension is ".histogram.gz".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Histogram");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.histogram.gz"));

    //    12. Set format "Per base".
    //    Expected state: the file extension is ".txt.gz".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.txt.gz"));

    //    13. Set format "Bedgraph".
    //    Expected state: the file extension is ".bedgraph.gz".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Bedgraph");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgraph.gz"));

    //    14. Manually edit the suffix: remove the 'h' (the suffix should be ".bedgrap.gz").
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.bedgrap.gz"));

    //    15. Set format "Histogram".
    //    Expected state: the file extension is ".bedgrap.histogram.gz".
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Histogram");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::CheckFilePath, QDir::toNativeSeparators(GUrlUtils::getDefaultDataPath() + "/chrM_coverage.histogram.gz"));

    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, QVariant());
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    //    Some negative tests for the output file path.

    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on the consensus area, select "Export coverage" menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    GTFile::removeDir(sandBoxDir + "common_assembly_browser/test_0012/test_0012");
    QDir().mkpath(sandBoxDir + "common_assembly_browser/test_0012");
    GTFile::setReadOnly(sandBoxDir + "common_assembly_browser/test_0012");

    //    3. Set the empty path and accept the dialog.
    //    Expected state: a messagebox appears, dialog is not closed.

    //    4. Set the path to a file in a read-only folder and accept the dialog.
    //    Expected state: a messagebox appears, dialog is not closed.

    //    5. Set the path to a file in a non-existent folder, which parent is read-only, and accept the dialog.
    //    Expected state: a messagebox appears, dialog is not closed.

    //    6. Set the path to an existent read-only file.
    //    Expected state: a messagebox appears, dialog is not closed.
    QList<ExportCoverageDialogFiller::Action> actions = {
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ExpectMessageBox, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0012/test_0012.txt")),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ExpectMessageBox, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0012/test_0012/test_0012.txt")),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ExpectMessageBox, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, ""),
        ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, QVariant())};
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //    Some positive tests for the output file path.

    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on the consensus area, select "Export coverage" menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    QList<ExportCoverageDialogFiller::Action> actions;

    //    3. Click the "Select file" button, select any non-existent file in the writable folder and accept the dialog.
    //    Expected state: dialog closes, file appears.
    QDir().mkpath(sandBoxDir + "common_assembly_browser/test_0013");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SelectFile, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0013/test_0013_1.txt"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFile::check(sandBoxDir + "common_assembly_browser/test_0013/test_0013_1.txt");

    //    4. Call the dialog again. Write the valid output file path manually. Path should be to a non-existent file in the writable folder. Accept the dialog.
    //    Expected state: dialog closes, file appears.
    actions.clear();
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0013/test_0013_2.txt"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFile::check(sandBoxDir + "common_assembly_browser/test_0013/test_0013_2.txt");

    //    5. Call the dialog again. Write the valid output file path manually. Path should be to a non-existent file in a non-existent folder with the writable parent folder. Accept the dialog.
    //    Expected state: dialog closes, file appears.
    actions.clear();
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0013/test_0013/test_0013_3.txt"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFile::check(sandBoxDir + "common_assembly_browser/test_0013/test_0013/test_0013_3.txt");

    //    6. Call the dialog again. Set the output file path to an existent writable file. Accept the dialog.
    //    Expected state: dialog closes, file is overwritten.
    GTFile::copy(testDir + "_common_data/text/text.txt", sandBoxDir + "common_assembly_browser/test_0013/test_0013_4.txt");
    const qint64 fileSizeBefore = GTFile::getSize(sandBoxDir + "common_assembly_browser/test_0013/test_0013_4.txt");
    actions.clear();
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir::toNativeSeparators(sandBoxDir + "common_assembly_browser/test_0013/test_0013_4.txt"));
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTFile::check(sandBoxDir + "common_assembly_browser/test_0013/test_0013/test_0013_4.txt");
    const qint64 fileSizeAfter = GTFile::getSize(sandBoxDir + "common_assembly_browser/test_0013/test_0013_4.txt");
    CHECK_SET_ERR(fileSizeAfter != fileSizeBefore, "File wasn't overwritten");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    //    Test for the unselected export type

    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Call context menu on the consensus area, select "Export coverage" menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    QList<ExportCoverageDialogFiller::Action> actions;

    //    3. Set "Per base" format, uncheck all export types (both coverage and bases count). Accept the dialog.
    //    Expected state: a messagebox appears, dialog is not closed.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetExportCoverage, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetExportBasesQuantity, false);
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ExpectMessageBox, "");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, "");

    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog();
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    //    Test default state of the export coverage worker, possible gui changes.

    //    1. Open Workflow Designer.
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //    2. Add an "Extract Coverage from Assembly" element to the scene.
    GTUtilsWorkflowDesigner::addAlgorithm("Extract Coverage from Assembly");

    //    3. Check that "Output file" parameter is required, "Export" parameter is invisible.
    GTUtilsWorkflowDesigner::click("Extract Coverage from Assembly");
    bool isOutputFileRequired = GTUtilsWorkflowDesigner::isParameterRequired("Output file");
    bool isFormatRequired = GTUtilsWorkflowDesigner::isParameterRequired("Format");
    bool isExportTypeVisible = GTUtilsWorkflowDesigner::isParameterVisible("Export");
    bool isThresholdRequired = GTUtilsWorkflowDesigner::isParameterRequired("Threshold");
    CHECK_SET_ERR(isOutputFileRequired, "The 'Output file' parameter is unexpectedly not required");
    CHECK_SET_ERR(!isFormatRequired, "The 'Format' parameter is unexpectedly required");
    CHECK_SET_ERR(!isExportTypeVisible, "The 'Export' parameter is unexpectedly visible");
    CHECK_SET_ERR(!isThresholdRequired, "The 'Threshold' parameter is unexpectedly required");

    //    4. Check parameters default values.
    //    Expected state: values are:
    //    Output file - "assembly_coverage.bedgraph";
    //    Format - "Bedgraph";
    //    Threshold default value - "1";
    //    Threshold minimum value - "0";
    //    Threshold maximum value - "65535";
    QString outputFileValue = GTUtilsWorkflowDesigner::getParameter("Output file");
    QString formatValue = GTUtilsWorkflowDesigner::getParameter("Format");
    QString thresholdValue = GTUtilsWorkflowDesigner::getParameter("Threshold");
    CHECK_SET_ERR(outputFileValue == "assembly_coverage.bedgraph", QString("1. Unexpected default value of the 'Output file' parameter: expected '%1', got '%2'").arg("assembly_coverage.bedgraph").arg(outputFileValue));
    CHECK_SET_ERR(formatValue == "Bedgraph", QString("Unexpected default value of the 'Format' parameter: expected '%1', got '%2'").arg("Bedgraph").arg(formatValue));
    CHECK_SET_ERR(thresholdValue == "1", QString("Unexpected default value of the 'Threshold' parameter: expected '%1', got '%2'").arg("1").arg(thresholdValue));

    GTUtilsWorkflowDesigner::clickParameter("Threshold");
    auto sbThreshold = qobject_cast<QSpinBox*>(GTUtilsWorkflowDesigner::getParametersTable()->findChild<QSpinBox*>());
    GTSpinBox::checkLimits(sbThreshold, 0, 65535);

    //    5. Set format "Histogram".
    //    Expected state: output file is "assembly_coverage.histogram", "Export" parameter is invisible.
    GTUtilsWorkflowDesigner::clickParameter("Threshold");
    GTUtilsWorkflowDesigner::setParameter("Format", 1, GTUtilsWorkflowDesigner::comboValue);
    outputFileValue = GTUtilsWorkflowDesigner::getParameter("Output file");
    isExportTypeVisible = GTUtilsWorkflowDesigner::isParameterVisible("Export");
    CHECK_SET_ERR("assembly_coverage.histogram" == outputFileValue, QString("2. Unexpected value of the 'Output file' parameter: expected '%1', got '%2'").arg("assembly_coverage.histogram").arg(outputFileValue));
    CHECK_SET_ERR(!isExportTypeVisible, "The 'Export' parameter is unexpectedly visible");

    //    6. Set format "Per base".
    //    Expected state: output file is "assembly_coverage.txt", "Export" parameter appears, it is required, its default value is "coverage".
    GTUtilsWorkflowDesigner::clickParameter("Threshold");
    GTUtilsWorkflowDesigner::setParameter("Format", 2, GTUtilsWorkflowDesigner::comboValue);
    outputFileValue = GTUtilsWorkflowDesigner::getParameter("Output file");
    isExportTypeVisible = GTUtilsWorkflowDesigner::isParameterVisible("Export");
    const bool isExportTypeRequired = GTUtilsWorkflowDesigner::isParameterRequired("Export");
    const QString exportTypeValue = GTUtilsWorkflowDesigner::getParameter("Export");
    CHECK_SET_ERR("assembly_coverage.txt" == outputFileValue, QString("3. Unexpected value of the 'Output file' parameter: expected '%1', got '%2'").arg("assembly_coverage.txt").arg(outputFileValue));
    CHECK_SET_ERR(isExportTypeVisible, "The 'Export' parameter is not 'visible'");
    CHECK_SET_ERR(isExportTypeRequired, "The 'Export' parameter is not 'required'");
    CHECK_SET_ERR(exportTypeValue == "coverage", QString("An unexpected value of 'Export' parameter: expected '%1' got '%2'").arg("coverage").arg(exportTypeValue));

    //    7. Set format "Bedgraph".
    //    Expected state: output file is "assembly_coverage.bedgraph", "Export" parameter is invisible.
    GTUtilsWorkflowDesigner::clickParameter("Threshold");
    GTUtilsWorkflowDesigner::setParameter("Format", 0, GTUtilsWorkflowDesigner::comboValue);
    outputFileValue = GTUtilsWorkflowDesigner::getParameter("Output file");
    isExportTypeVisible = GTUtilsWorkflowDesigner::isParameterVisible("Export");
    CHECK_SET_ERR("assembly_coverage.bedgraph" == outputFileValue, QString("4. Unexpected default value of the 'Output file' parameter: expect '%1', got '%2'").arg("assembly_coverage.bedgraph").arg(outputFileValue));
    CHECK_SET_ERR(!isExportTypeVisible, "The 'Export' parameter is unexpectedly visible");

    //    8. Enter any value to the "Output file" parameter.
    //    Expected state: a popup completer appears, it contains extensions for the compressed format.
    /*    GTUtilsWorkflowDesigner::clickParameter("Output file");
    URLWidget *urlWidget = qobject_cast<URLWidget *>(GTUtilsWorkflowDesigner::getParametersTable()->findChild<URLWidget *>());
    GTKeyboardDriver::keySequence("aaa");
    GTKeyboardDriver::keyPress(Qt::Key_Enter);
    CHECK_SET_ERR(NULL != urlWidget, "Output file url widget was not found");
    QTreeWidget *completer = urlWidget->findChild<QTreeWidget *>();
    CHECK_SET_ERR(completer != NULL, "auto completer widget was not found");
    bool itemFound = !completer->findItems("aaa.bedgraph.gz", Qt::MatchExactly).isEmpty();
    CHECK_SET_ERR(itemFound, "Completer item was not found");
*/
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    //    Test for dialog availability
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0016.ugenedb");
    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".

    GTFileDialog::openFile(sandBoxDir + "assembly_test_0016.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    QList<ExportCoverageDialogFiller::Action> actions;

    //    2. Call context menu on the consensus area, select {Export coverage} menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.

    //    3. Cancel the dialog.
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickCancel, "");
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog(GTUtilsAssemblyBrowser::Consensus);

    //    4. Call context menu on the overview area, select {Export coverage} menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.

    //    5. Cancel the dialog.
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog(GTUtilsAssemblyBrowser::Overview);

    //    6. Zoom to reads somewhere. Call context menu on the reads area, select {Export -> Coverage} menu item.
    //    Expected state: an "Export the Assembly Coverage" dialog appears.
    GTUtilsAssemblyBrowser::zoomToMax();
    GTUtilsDialog::waitForDialog(new ExportCoverageDialogFiller(actions));
    GTUtilsAssemblyBrowser::callExportCoverageDialog(GTUtilsAssemblyBrowser::Reads);
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0017.ugenedb");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0017.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Click the "chrM" sequence object in Project View.
    GTUtilsProjectTreeView::click("chrM", "chrM.fa");

    // 4. Right-click on the reference area.
    // Expected: "Unassociate" is disabled.
    GTUtilsDialog::waitForDialog(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsDisabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // 5. Click "Set reference sequence".
    GTUtilsDialog::waitForDialog(new PopupChooser({"setReferenceAction"}));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // 6. Right-click on the reference area.
    // Expected: "Unassociate" is enabled.
    GTUtilsDialog::waitForDialog(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // 7. Click "Unassociate".
    GTUtilsDialog::waitForDialog(new PopupChooser({"unassociateReferenceAction"}));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // 8. Right-click on the reference area.
    // Expected: "Unassociate" is disabled.
    GTUtilsDialog::waitForDialog(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsDisabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
}

namespace {
void prepareBigFasta(const QString& url) {
    QFile file(url);
    bool opened = file.open(QIODevice::WriteOnly);
    if (!opened) {
        GT_FAIL("Can't open a file: " + url, );
    }
    file.write(">assembly_test_0018\n");
    for (int i = 0; i < 1000000; i++) {
        file.write("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        file.write("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    }
}
}  // namespace

GUI_TEST_CLASS_DEFINITION(test_0018) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0018.ugenedb");
    prepareBigFasta(sandBoxDir + "assembly_test_0018.fa");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0018.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Click the "chrM" sequence object in Project View.
    GTUtilsProjectTreeView::click("chrM", "chrM.fa");

    // 4. Click the "Set reference sequence" toolbar button.
    GTWidget::click(GTAction::button("setReferenceAction"));

    // 5. Clear the objects selection in Project View (e.g. click the document "chrM.fa").
    GTUtilsProjectTreeView::click("chrM.fa");

    // 6. Click "Set reference sequence".
    // 7. Choose the file "assembly_test_0018.fa" from sandbox.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(sandBoxDir + "assembly_test_0018.fa"));
    GTWidget::click(GTAction::button("setReferenceAction"));

    // 8. Right-click on the reference area while the file is loading.
    // Expected: "Unassociate" and "Set reference sequence" are disabled.
    GTUtilsDialog::waitForDialog(new PopupChecker({"setReferenceAction"}, PopupChecker::IsDisabled));
    // GTUtilsDialog::waitForDialog(new PopupChecker({"setReferenceAction"}, PopupChecker::IsDisabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
    // GTUtilsDialog::waitForDialog(new PopupChecker({"unassociateReferenceAction"}, PopupChecker::IsDisabled));
    // GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);

    // 9. Right-click on the reference area after loading.
    // Expected: "Unassociate" and "Set reference sequence" are enabled.
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new PopupCheckerByText(QStringList(), {"Set reference", "Unassociate"}, PopupChecker::IsEnabled));
    GTWidget::click(GTWidget::findWidget("Assembly reference sequence area"), Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0019.ugenedb");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0019.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Click the "chrM" sequence object in Project View.
    GTUtilsProjectTreeView::click("chrM", "chrM.fa");

    // 5. Click the "Set reference sequence" actions menu item.
    // Expected: it becomes reference.
    GTMenu::clickMainMenuItem({"Actions", "Set reference"});

    // 6. Add the "human_T1" object to the selection.
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // 7. Click the "Set reference sequence" actions menu item.
    // Expected: message box about two sequences appears.
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "You have more than one sequence"));
    GTMenu::clickMainMenuItem({"Actions", "Set reference"});

    // 8. Click the "chrM.fa" sequence object in Project View.
    GTUtilsProjectTreeView::click("chrM.fa");

    // 9. Click the "Set reference sequence" actions menu item.
    // Expected: file dialog appears.
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Genbank/murine.gb"));
    GTMenu::clickMainMenuItem({"Actions", "Set reference"});
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0020.ugenedb");

    // 1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0020.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Set reference sequence".
    // Expected: file dialog appears.
    // 3. Choose "data/samples/Assembly/chrM.fa".
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.fa"));
    GTWidget::click(GTAction::button("setReferenceAction"));
    // Expected: it becomes reference.
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0021.ugenedb");

    // 1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0021.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Set reference sequence".
    // Expected: file dialog appears.
    // 3. Choose "data/samples/FASTQ/eas.fastq".
    // Expected: error notification is shown.
    GTUtilsNotifications::waitForNotification(true, "There are more than one sequence in file");
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/FASTQ/eas.fastq"));
    GTWidget::click(GTAction::button("setReferenceAction"));
}

GUI_TEST_CLASS_DEFINITION(test_0022) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0022.ugenedb");

    // 1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0022.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Click "Set reference sequence".
    // Expected: file dialog appears.
    // 3. Choose "data/samples/CLUSTALW/COI.aln".
    // Expected: error notification is shown.
    GTUtilsNotifications::waitForNotification(true, "does not contain sequences");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/COI.aln"));
    GTWidget::click(GTAction::button("setReferenceAction"));
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0023.ugenedb");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0023.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Click the "Set reference sequence" toolbar button.
    // Expected: file dialog appears.
    // 4. Choose "samples/Assembly/chrM.fa".
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.fa"));
    GTWidget::click(GTAction::button("setReferenceAction"));

    // Expected: the sequence becomes reference.
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0024) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0024.ugenedb");

    // 1. Open "samples/Assembly/chrM.fa".
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Unload the document.
    GTUtilsDocument::unloadDocument("chrM.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0024.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Click the "Set reference sequence" toolbar button.
    // Expected: file dialog appears.
    // 5. Choose "samples/Assembly/chrM.fa".
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/Assembly/chrM.fa"));
    GTWidget::click(GTAction::button("setReferenceAction"));

    // Expected: the document is loaded, the sequence becomes reference.
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    QFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "assembly_test_0025.ugenedb");

    // 1. Open "samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Unload the document.
    GTUtilsDocument::unloadDocument("COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 3. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(sandBoxDir + "assembly_test_0025.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 4. Click the "Set reference sequence" toolbar button.
    // Expected: file dialog appears.
    // 5. Choose "samples/CLUSTALW/COI.aln".
    // Expected: the document is loaded, error notification is shown.
    GTUtilsDialog::add(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/COI.aln"));
    GTWidget::click(GTAction::button("setReferenceAction"));

    GTUtilsNotifications::waitForNotification(true, "does not contain sequences");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0026_1) {
    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Select region to extract and import extracted file to project
    GTUtilsDialog::add(new ExtractAssemblyRegionDialogFiller(sandBoxDir + "/test_26_1.bam", U2Region(228, 1488), "BAM"));
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "/test_26_1.ugenedb"));
    QAbstractButton* button = GTAction::button("ExtractAssemblyRegion");
    GTWidget::click(button);
    GTUtilsTaskTreeView::waitTaskFinished();
    //      3. Check expected coverage values
    auto coveredRegionsLabel = GTWidget::findLabel("CoveredRegionsLabel", GTUtilsMdi::activeWindow());

    QString textFromLabel = coveredRegionsLabel->text();
    CHECK_SET_ERR(textFromLabel.contains("229"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("222"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("215"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("194"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("192"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("190"), "expected coverage value not found");
}

GUI_TEST_CLASS_DEFINITION(test_0026_2) {
    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::getActiveAssemblyBrowserWindow();

    //    2. Select region to extract and import extracted file to project
    GTUtilsDialog::add(new ExtractAssemblyRegionDialogFiller(sandBoxDir + "/test_26_2.sam", U2Region(4500, 300), "SAM"));
    GTUtilsDialog::add(new ImportBAMFileFiller(sandBoxDir + "/test_26_2.ugenedb"));
    QAbstractButton* button = GTAction::button("ExtractAssemblyRegion");
    GTWidget::click(button);
    GTUtilsTaskTreeView::waitTaskFinished();
    //      3. Check expected coverage values
    auto coveredRegionsLabel = GTWidget::findLabel("CoveredRegionsLabel", GTUtilsMdi::activeWindow());

    QString textFromLabel = coveredRegionsLabel->text();
    CHECK_SET_ERR(textFromLabel.contains("157"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("65"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("55"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("53"), "expected coverage value not found");
}

GUI_TEST_CLASS_DEFINITION(test_0026_3) {
    //    1. Open "_common_data/ugenedb/chrM.sorted.bam.ugenedb".
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Select region to extract and import extracted file to project
    GTUtilsDialog::waitForDialog(new ExtractAssemblyRegionDialogFiller(sandBoxDir + "/test_26_3.ugenedb", U2Region(6500, 900), "UGENE Database"));
    GTWidget::click(GTAction::button("ExtractAssemblyRegion"));
    GTUtilsTaskTreeView::waitTaskFinished();

    //      3. Check expected coverage values
    auto coveredRegionsLabel = GTWidget::findLabel("CoveredRegionsLabel", GTUtilsMdi::activeWindow());

    QString textFromLabel = coveredRegionsLabel->text();
    CHECK_SET_ERR(textFromLabel.contains("330"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("253"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("193"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("187"), "expected coverage value not found");
    CHECK_SET_ERR(textFromLabel.contains("186"), "expected coverage value not found");
}

GUI_TEST_CLASS_DEFINITION(test_0027) {
    //    1. Open assembly
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMsaEditor::checkMsaEditorWindowIsActive();

    //    2. Open COI.aln
    GTFileDialog::openFile(testDir + "_common_data/scenarios/assembly/", "example-alignment.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    3. Drag and drop COI object to assembly browser
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Only a nucleotide sequence or a variant track objects can be added to the Assembly Browser"));
    GTUtilsAssemblyBrowser::addRefFromProject("COI");
    //    Expected: error message box appears
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    // 1. Open assembly
    GTFileDialog::openFile(dataDir + "samples/Assembly", "chrM.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    // 2. Lock document for editing
    GTUtilsDocument::lockDocument("chrM.sorted.bam.ugenedb");
    // 3. Try to add reference
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "This action requires changing the assembly object that is locked for editing"));
    GTUtilsAssemblyBrowser::addRefFromProject("chrM", GTUtilsProjectTreeView::findIndex("chrM.fa"));
    // Expected: Error message appears
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0029) {
    //    1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    for (int i = 0; i < 15; i++) {
        GTUtilsAssemblyBrowser::zoomIn(GTUtilsAssemblyBrowser::Hotkey);
    }
    //    2. Go to some position using position selector on the toolbar(check "Go" button and "Enter" hotkey)
    GTUtilsAssemblyBrowser::goToPosition(1000);
    int scrollVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Horizontal)->value();
    CHECK_SET_ERR(scrollVal == 999, QString("Unexpected scroll value1: %1").arg(scrollVal))

    GTUtilsAssemblyBrowser::goToPosition(2000);
    scrollVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Horizontal)->value();
    CHECK_SET_ERR(scrollVal == 1999, QString("Unexpected scroll value2: %1").arg(scrollVal))

    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    //    1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Move reads area right and down with mouse
    GTUtilsAssemblyBrowser::zoomToReads();
    for (int i = 0; i < 8; i++) {
        GTUtilsAssemblyBrowser::zoomIn(GTUtilsAssemblyBrowser::Hotkey);
    }

    int initHorVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Horizontal)->value();
    int initVerVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Vertical)->value();

    GTMouseDriver::press();
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(-200, -200));
    GTMouseDriver::release();

    //    Check scrollbars, rules values etc.
    int finalHorVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Horizontal)->value();
    int finalVerVal = GTUtilsAssemblyBrowser::getScrollBar(Qt::Vertical)->value();
    CHECK_SET_ERR(finalHorVal > initHorVal, QString("Unexpected horizontal scroll values. Initial: %1, final %2").arg(initHorVal).arg(finalHorVal));
    CHECK_SET_ERR(finalVerVal > initVerVal, QString("Unexpected vertical scroll values. Initial: %1, final %2").arg(initVerVal).arg(finalVerVal));
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    //    1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Click "zoom to reads" link
    GTUtilsAssemblyBrowser::zoomToReads();
    //    Check zoom
    GTUtilsDialog::waitForDialog(new PopupChecker({"Export"}));
    GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Reads);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0032) {
    //    1. Open assembly
    GTFile::copy(testDir + "_common_data/ugenedb/chrM.sorted.bam.ugenedb", sandBoxDir + "chrM.sorted.bam.ugenedb");
    GTFileDialog::openFile(sandBoxDir + "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Rename assembly object
    QModelIndex documentIndex = GTUtilsProjectTreeView::findIndex("chrM.sorted.bam.ugenedb");
    QModelIndex objectIndex = GTUtilsProjectTreeView::findIndex("chrM", documentIndex);
    GTUtilsProjectTreeView::rename(objectIndex, "new_name");
    //    Check UGENE title
    GTMainWindow::checkTitle("-* UGENE");
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    //    1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Open "Assembly browser settings" OP tab
    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));
    GTUtilsAssemblyBrowser::zoomToReads();
    //    3. Change reads highlighting to "strand direction" and "complement"
    auto box = GTWidget::findComboBox("READS_HIGHLIGHTNING_COMBO");
    GTComboBox::selectItemByText(box, "Strand direction");
    GTComboBox::selectItemByText(box, "Paired reads");
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    //    1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    //    2. Open "Assembly browser settings" OP tab
    GTWidget::click(GTWidget::findWidget("OP_ASS_SETTINGS"));
    GTUtilsAssemblyBrowser::zoomToReads();
    //    3. Change consensus algorithm
    auto box = GTWidget::findComboBox("consensusAlgorithmCombo");
    GTComboBox::selectItemByText(box, "SAMtools");
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    GTFileDialog::openFile(dataDir + "samples/Assembly/chrM.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    GTUtilsAssemblyBrowser::addRefFromProject("chrM", GTUtilsProjectTreeView::findIndex("chrM.fa"));

    class Scenario : public CustomScenario {
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            auto filepathLineEdit = GTWidget::findLineEdit("filepathLineEdit", dialog);
            GTLineEdit::setText(filepathLineEdit, sandBoxDir + "chrM.snp");

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };
    //    Export consensus
    GTUtilsDialog::waitForDialog(new ExportConsensusDialogFiller(new Scenario()));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Export consensus variations..."}));
    GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Consensus);

    CHECK_SET_ERR(GTUtilsProjectTreeView::checkItem("chrM.snp"), "chrM.snp is not found");
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    // 1. Open assembly
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    // Check these hotkeys: up, down, left, right, +, -, pageup, pagedown
    GTUtilsAssemblyBrowser::zoomToReads();
    for (int i = 0; i < 7; i++) {
        GTUtilsAssemblyBrowser::zoomIn(GTUtilsAssemblyBrowser::Hotkey);
    }

    QScrollBar* vScrollBar = GTUtilsAssemblyBrowser::getScrollBar(Qt::Vertical);
    QScrollBar* hScrollBar = GTUtilsAssemblyBrowser::getScrollBar(Qt::Horizontal);

    int vScrollBarValue = vScrollBar->value();
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Down);
    }
    CHECK_SET_ERR(vScrollBar->value() == vScrollBarValue + 3, QString("1. Unexpected vScrollBar value: %1, before: %2").arg(vScrollBar->value()).arg(vScrollBarValue));

    vScrollBarValue = vScrollBar->value();
    for (int i = 0; i < 2; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Up);
    }
    CHECK_SET_ERR(vScrollBar->value() == vScrollBarValue - 2, QString("2. Unexpected vScrollBar value: %1, before: %2").arg(vScrollBar->value()).arg(vScrollBarValue));

    int hScrollBarValue = hScrollBar->value();
    for (int i = 0; i < 3; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Left);
    }
    CHECK_SET_ERR(hScrollBar->value() == hScrollBarValue - 3, QString("3. Unexpected hScrollBar value: %1, before: %2").arg(hScrollBar->value()).arg(hScrollBarValue));

    hScrollBarValue = hScrollBar->value();
    for (int i = 0; i < 2; i++) {
        GTKeyboardDriver::keyClick(Qt::Key_Right);
    }
    CHECK_SET_ERR(hScrollBar->value() == hScrollBarValue + 2, QString("4. Unexpected hScrollBar value: %1, before: %2").arg(hScrollBar->value()).arg(hScrollBarValue));

    vScrollBarValue = vScrollBar->value();
    GTKeyboardDriver::keyClick(Qt::Key_PageDown);
    CHECK_SET_ERR(vScrollBar->value() > vScrollBarValue + 10, QString("5. Unexpected vScrollBar value: %1, before: %").arg(vScrollBar->value()).arg(vScrollBarValue));

    GTKeyboardDriver::keyClick(Qt::Key_PageUp);
    CHECK_SET_ERR(vScrollBar->value() == vScrollBarValue, QString("6. Unexpected vScrollBar value: %1").arg(vScrollBar->value()));
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    // Open assembly.
    GTFileDialog::openFile(testDir + "_common_data/ugenedb", "chrM.sorted.bam.ugenedb");
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();

    // Zoom to the reads level.
    GTUtilsAssemblyBrowser::zoomToReads();
    for (int i = 0; i < 10; i++) {
        GTUtilsAssemblyBrowser::zoomIn(GTUtilsAssemblyBrowser::Hotkey);
    }

    // Shift view to the 1k+ coordinates to check that large numbers are correctly formatted.
    GTUtilsAssemblyBrowser::goToPosition(5000);

    // Copy read information and check that it is valid.
    GTUtilsDialog::waitForDialog(new PopupChooser({"copy_read_information"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(GTWidget::findWidget("assembly_reads_area"));
    QString clipboard = GTClipboard::text();
    CHECK_SET_ERR(clipboard.startsWith('>') && clipboard.contains("From") &&
                      clipboard.contains("Length") && clipboard.contains("Row") &&
                      clipboard.contains("Cigar") && clipboard.contains("Strand"),
                  "Unexpected clipboard: " + clipboard)

    // Check that read position is copied as a number.
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Copy current position to clipboard"}, GTGlobals::UseMouse));
    GTMenu::showContextMenu(GTWidget::findWidget("assembly_reads_area"));
    clipboard = GTClipboard::text();
    bool ok;
    clipboard.toInt(&ok);
    CHECK_SET_ERR(ok, "unexpected clipboard: " + clipboard)
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    // Based on UGENE-7677
    // 1. open view for _common_data\bam\more_then_100000_reads.bam
    // Expected state: conversion finished without error
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller());
    GTFileDialog::openFile(testDir + "_common_data/bam/", "more_then_100000_reads.bam");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::checkNoActiveWaiters();
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
    auto readsCount = GTUtilsAssemblyBrowser::getReadsCount();
    CHECK_SET_ERR(readsCount > 1000000, QString("Unexpected reads count, expected: >1000000, current: %1").arg(readsCount));
}

}  // namespace GUITest_Assembly_browser
}  // namespace U2
