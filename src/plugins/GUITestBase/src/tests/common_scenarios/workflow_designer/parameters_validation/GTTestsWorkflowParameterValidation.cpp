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

#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>

#include <QDir>

#include <U2Core/U2SafePoints.h>

#include "GTTestsWorkflowParameterValidation.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "system/GTFile.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_common_scenarios_workflow_parameters_validation {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Open WD sample "Align Sequences with MUSCLE
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Set some name for an output file
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write alignment"));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 1));
    GTMouseDriver::click();
    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTKeyboardDriver::keySequence(s + "/wd_pv_0001.sto");
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 3. Add the file "test/_common_data/clustal/align.aln" as input
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/align.aln");

    // 4. Move this file somewhere from its folder
    class RenameHelper {
    public:
        RenameHelper(const QString& url)
            : file(url), renamed(false) {
            oldName = file.fileName();
            renamed = file.rename("wd_pv_0001.aln");
            if (!renamed) {
                GT_FAIL("Can not rename the file", );
            }
        }
        ~RenameHelper() {
            if (renamed) {
                file.rename(oldName);
            }
        }

    private:
        QFile file;
        QString oldName;
        bool renamed;
    };
    QString inFile = QFileInfo(testDir + "_common_data/clustal/align.aln").absoluteFilePath();
    RenameHelper h(inFile);

    // 5. In WD press the "Validate" button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow)."));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected state: The "File not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList("Read alignment: File not found:");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    Workflow dataset input folder validation
    //    1. Open WD sample "Align Sequences with MUSCLE"
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    //    2. Set some name for an output file
    auto table = GTWidget::findTableView("table");
    QPoint writeAlignmentCenter = GTUtilsWorkflowDesigner::getItemCenter("Write alignment");
    GTMouseDriver::moveTo(writeAlignmentCenter);
    GTMouseDriver::click();
    QPoint cellPoint = GTTableView::getCellPosition(table, 1, 1);
    GTMouseDriver::moveTo(cellPoint);
    GTMouseDriver::click();
    QString dirPath = QFileInfo(testDir + "_common_data/scenarios/sandbox/").absoluteFilePath();
    GTKeyboardDriver::keySequence(dirPath + "/wd_pv_0002.sto");
    QWidget* activeWindow = GTUtilsMdi::activeWindow();
    CHECK_SET_ERR(activeWindow, "Active window wasn't found");
    GTWidget::click(activeWindow);

    //    3. Create a new empty folder somewhere (e.g. in the "test/_tmp" folder)
    QDir outputDir(testDir + "_common_data/scenarios/sandbox/wd_pv_0002_out/1/2/3/4/");
    bool success = outputDir.mkpath(outputDir.absolutePath());
    CHECK_SET_ERR(success, QString("Can't create a new folder: '%1'").arg(outputDir.absolutePath()));

    //    4. Add that folder as input in WD
    QPoint readAlignmentCenter = GTUtilsWorkflowDesigner::getItemCenter("Read alignment");
    GTMouseDriver::moveTo(readAlignmentCenter);
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFolder(outputDir.absolutePath());
    GTWidget::click(activeWindow);

    //    5. Delete this folder
    QString s = outputDir.absolutePath();
    GTFile::removeDir(s);
    outputDir.rmpath(outputDir.absolutePath());

    //    6. In WD press the "Validate" button
    MessageBoxDialogFiller* filler = new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow).");
    GTUtilsDialog::waitForDialog(filler);
    QAbstractButton* validateButton = GTAction::button("Validate workflow");
    CHECK_SET_ERR(validateButton, "Validate button wasn't found");
    GTWidget::click(validateButton);

    //    Expected state: The "folder not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList("Read alignment: folder not found:");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // 1. Create the following workflow { Read Sequence -> Find Pattern -> Write Sequence }
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("Find Pattern");
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    WorkflowProcessItem* seqReader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* patternFinder = GTUtilsWorkflowDesigner::getWorker("Find Pattern");
    WorkflowProcessItem* seqWriter = GTUtilsWorkflowDesigner::getWorker("Write Sequence");

    GTUtilsWorkflowDesigner::connect(seqReader, patternFinder);
    GTUtilsWorkflowDesigner::connect(patternFinder, seqWriter);

    // 2. Set some name for an output file

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Sequence"));
    GTMouseDriver::click();
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 3));
    GTMouseDriver::click();
    GTKeyboardDriver::keySequence("sequence.gb");
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 3. Add some valid sequence file as input

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/fa1.fa");

    // 4. Set some nonexistent path to a file as the "Pattern file" parameter of the "Find Substrings" worker

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Find Pattern"));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 2));
    GTMouseDriver::click();
    GTKeyboardDriver::keySequence("pattern_file.txt");
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 5. In WD press the "Validate" button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow)."));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected state: The "File not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList("File not found");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTLogTracer lt;
    QDir d(testDir + "_common_data/scenarios/sandbox/permDir");
    bool sucsess = d.mkpath(d.absolutePath());
    CHECK_SET_ERR(sucsess, QString("Can't create a new folder: '%1'").arg(d.absolutePath()));

    // 1. Open WD sample "Align Sequences with MUSCLE
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsWorkflowDesigner::addSample("Align sequences with MUSCLE");
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    // 2. Set some name for an output file
    GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write alignment"));
    GTMouseDriver::click();
    // GTGlobals::sleep(60000);
    QString s = QFileInfo(testDir + "_common_data/scenarios/sandbox/permDir").absoluteFilePath();
    GTUtilsWorkflowDesigner::setParameter("Output file", QVariant(s + "/wd_pv_0001.sto"), GTUtilsWorkflowDesigner::textValue);
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 3. Add the file "test/_common_data/clustal/align.aln" as input
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read alignment"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/clustal/align.aln");

    QFile dir(testDir + "_common_data/scenarios/sandbox/permDir");
    CHECK_SET_ERR(dir.exists(), "Sandbox not found");
    QFile::Permissions p = dir.permissions();

    p &= ~QFile::WriteOwner;
    p &= ~QFile::WriteUser;
    p &= ~QFile::WriteGroup;
    p &= ~QFile::WriteOther;
    bool res = dir.setPermissions(p);
    CHECK_SET_ERR(res, "Fucking test");

    // 5. In WD press the "Validate" button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow)."));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected state: The "File not found" error has appeared in the "Error list"
    p |= QFile::WriteOwner;
    p |= QFile::WriteUser;
    p |= QFile::WriteGroup;
    p |= QFile::WriteOther;
    res = dir.setPermissions(p);
    CHECK_SET_ERR(res, "Not good test");
    int i = GTUtilsWorkflowDesigner::checkErrorList("Can't access output file path:");
    CHECK_SET_ERR(i == 1, "There are no error messages about write access in WD folder");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Workflow worker folder parameter validation
    // 1. Create the following workflow { Read Sequence -> CD Search -> Write Sequence }
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    GTUtilsWorkflowDesigner::addAlgorithm("CD Search");
    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);

    WorkflowProcessItem* seqReader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    WorkflowProcessItem* patternFinder = GTUtilsWorkflowDesigner::getWorker("CD Search");
    WorkflowProcessItem* seqWriter = GTUtilsWorkflowDesigner::getWorker("Write Sequence");

    GTUtilsWorkflowDesigner::connect(seqReader, patternFinder);
    GTUtilsWorkflowDesigner::connect(patternFinder, seqWriter);

    // 2. Set some name for an output file

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write Sequence"));
    GTMouseDriver::click();
    auto table = GTWidget::findTableView("table");
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 3));
    GTMouseDriver::click();
    GTKeyboardDriver::keySequence("sequence.gb");
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 3. Add some valid sequence file as input

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/fasta/fa1.fa");

    // 4. Create an empty folder somewhere (e.g. in the "test/_tmp" folder)
    QDir newDir(testDir);
    newDir.mkdir("_empty_tmp");

    // 5. Set the path to this folder as the "Database folder" parameter of the "CD Search" worker
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("CD Search"));
    GTMouseDriver::click();
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, 2));
    GTMouseDriver::click();
    GTKeyboardDriver::keySequence(testDir + "_empty_tmp");
    GTWidget::click(GTUtilsMdi::activeWindow());

    // 6. Remove this folder
    newDir.rmdir("_empty_tmp");

    // 7. In WD press the "Validate" button
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "Please fix issues listed in the error list (located under workflow)."));
    GTWidget::click(GTAction::button("Validate workflow"));

    // Expected state: The "File not found" error has appeared in the "Error list"
    GTUtilsWorkflowDesigner::checkErrorList("Folder not found");
}

}  // namespace GUITest_common_scenarios_workflow_parameters_validation

}  // namespace U2
